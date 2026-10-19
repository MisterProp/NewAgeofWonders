// mapviz.cpp - renders a decoded map to a PNG (see aow/mapviz.hpp).
#include "aow/mapviz.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <utility>

#include "aow/game_state.hpp"
#include "aow/ilb.hpp"
#include "aow/png.hpp"
#include "aow/unit_catalog.hpp"

namespace fs = std::filesystem;

namespace aow {

// See mapviz.hpp. Engine object draw anchor plus the sprite's own visual offset
// (base+shift), which the image's Show adds inside itself:
//   single-hex  HSEngine.TMapObject.Show@55609a00   -> HXtoHP - 8
//   multi-hex   HSEngine.TMultiHexMO.Show@5560aaec  -> HXtoHP + dXmainP/dYmainP
// mapviz's hex origin IS HXtoHP (`HSEngine.HXtoHP` returns `x*0x20 + 8`, and the
// render loop's `field_x` is the same 8), so a single-hex object lands
// `kHexFieldMargin` px right of the engine's. That 8 px is a known, pre-existing
// offset, not a cancellation: measured against the original game, the reference's
// terrain sits 8 px left of the port's *relative to the units* (which both draw at
// HXtoHP) - see docs/16_UNIT_ANIM.md section 10. Changing it shifts every level
// render and the benchmarks tuned to it, so it is left as a separate TODO.
void object_sprite_origin(int hex_sx, int hex_sy, const MapResource* resource,
                          int vis_x, int vis_y, int& ox, int& oy) {
    if (resource != nullptr && resource->has_shift) {
        ox = hex_sx + resource->dx_main + vis_x + kHexFieldMargin;
        oy = hex_sy + resource->dy_main + vis_y;
    } else {
        ox = hex_sx + vis_x;
        oy = hex_sy + vis_y;
    }
}

// See mapviz.hpp. City.TCity.UpdateImages@557ac494 maps the hex's terrain code
// to the block offset *inside the city resource's own image library* (its
// `iVar5`): terrain 3 (snow) -> 0x14, the 5/7/8/9/12/13 families (wasteland,
// earth/rock/lava, upper) -> 0x28, everything else -> 0. City1.ILB lays those
// blocks out 20 apart (0, 0x14, 0x28), each holding [body, random, 4 walls,
// razed]; the underground block uses the `U_wall` sprites.
int city_terrain_block(int terrain) {
    if (terrain == 3) return 0x14;
    if (terrain == 5 || terrain == 7 || terrain == 8 || terrain == 9 ||
        terrain == 12 || terrain == 13) {
        return 0x28;
    }
    return 0;
}

// See mapviz.hpp (AoWE.TRaceResource.GetCityImage@55759ebc). `size` is the city
// resource's +0x78 (1..4), `terrain` the hex's terrain code.
std::size_t city_image_slot(int size, int terrain) {
    if (terrain == 5) {
        return static_cast<std::size_t>(size + 0x3b);
    }
    return static_cast<std::size_t>(terrain * 10 + size + 0x45);
}

// The fallback GetCityImage tries when `city_image_slot` is absent from the
// race library: `size + (wall_flag == 0 ? 0x32 : 0x3c) - 1`. `wall_flag` is the
// TMapField byte at +0x12 (a walled city sets it); the port passes 1 when the
// city resource carries a wall type. The engine's exact byte is still
// [TODO(RE)].
std::size_t city_image_fallback_slot(int size, int wall_flag) {
    return static_cast<std::size_t>(size + (wall_flag == 0 ? 0x32 : 0x3c) - 1);
}

// See mapviz.hpp. The four `City.TCityResource` records name their library
// `CITY1.ILB`..`CITY4.ILB`; ActivateMsg@557adee8 stores the size in
// resource+0x78 and the four libraries are the village..city art (56x63,
// 87x88, 87x105, 120x113), so the trailing digit 1..4 is that size.
int city_resource_size(const MapResource& resource) {
    std::string stem = fs::path(resource.ilb).stem().string();
    for (char& c : stem) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (stem.size() == 5 && stem.compare(0, 4, "city") == 0 &&
        stem[4] >= '1' && stem[4] <= '4') {
        return stem[4] - '0';
    }
    return 0;
}

// See mapviz.hpp. `TCity.UpdateImages@557ac494` asks `GetCityImage` for the
// city's own race byte (rw tag 0xd) and, when that byte is negative, falls back
// to the city resource's `C_random` block. An owned city has a better answer:
// `TCity.UpdateFlagID@557ac8b0` resolves an owned city's race from its owner
// (`TPlayer.GetRace`, `+0xa5`) to build the city's *flag* byte, and the port
// already flies that race's flag - so the body is drawn from the same race
// library. Only a city without a usable owner keeps the engine's `C_random`
// placeholder (the byte stays unknown and the caller's block fallback runs).
int city_body_race(int city_race, unsigned owner,
                   const std::vector<std::uint8_t>& player_races,
                   std::size_t race_count) {
    if (city_race >= 0) return city_race;  // the map set a race: it wins
    if (owner >= 1 && owner < player_races.size() &&
        player_races[owner] < race_count) {
        return static_cast<int>(player_races[owner]);
    }
    return -1;  // unowned, unknown roster, or an owner race that is itself unset
}

// See mapviz.hpp. HSEngine.HNtoHX@5560e08c returns the six neighbour offsets as
// (dx,dy) per direction 1..6 = N/NE/E/S/W/NW; HSEngine.TranslateHX@509C8955
// folds the column stagger (odd column + odd dx -> one row up). The transition
// byte then collapses the E/W and NE/NW pairs onto the dedicated straight bits,
// exactly as re/exports/hsepack/Road.TRoad.UpdateTransitions.c does
// (`(b & 0x22) == 0x22 -> (b & 0xDD) | 0x80`, `(b & 0x14) == 0x14 ->
// (b & 0xEB) | 0x40`).
std::uint8_t road_transition_byte(std::uint32_t x, std::uint32_t y,
                                  std::uint32_t width, std::uint32_t height,
                                  const std::vector<std::uint8_t>& connected) {
    static const int dx[6] = {0, 1, 1, 0, -1, -1};   // N, NE, E, S, W, NW
    static const int dy[6] = {-1, -1, 0, 1, 0, -1};
    std::uint8_t b = 0;
    for (int i = 0; i < 6; ++i) {
        int nx = static_cast<int>(x) + dx[i];
        int ny = static_cast<int>(y) + dy[i];
        if ((x & 1u) != 0 && (dx[i] & 1) != 0) ++ny;  // TranslateHX stagger
        if (nx < 0 || ny < 0 || nx >= static_cast<int>(width) ||
            ny >= static_cast<int>(height)) {
            continue;
        }
        if (connected[static_cast<std::size_t>(ny) * width +
                      static_cast<std::size_t>(nx)] != 0) {
            b = static_cast<std::uint8_t>(b | (1u << i));
        }
    }
    if ((b & 0x22) == 0x22) b = static_cast<std::uint8_t>((b & 0xDD) | 0x80);
    if ((b & 0x14) == 0x14) b = static_cast<std::uint8_t>((b & 0xEB) | 0x40);
    return b;
}

// See mapviz.hpp. The engine tests a structure's runtime resource byte `+0x45`
// (AoWE.TStructure.MapFieldMsgProc@5575f730, message 0x1131) to decide whether a
// road may connect into its hex; that byte is not serialised, so classify by the
// placed object's class instead. Everything that is not a road, a terrain
// hexagon/decoration, a mountain, a crop, a unit/army, an item, a fire or a
// transient effect cloud is a structure whose tile a road links to. [TODO(RE):
// recover the +0x45 flag so exotic non-road-connectable structures are excluded.]
bool road_connectable_object(std::uint32_t class_id) {
    switch (class_id) {
        case kRoadClassId:            // Road.TRoad
        case 0x20166:                 // Terrain.TTerrainMO (trees/rocks/bones...)
        case 0x20168:                 // Mountain.TMountainMO
        case 0x2036a:                 // Crops.TCrop
        case 0x20217:                 // AoWE.TArmyHS
        case 0x20271:                 // AoWE.TItemHS
        case 0x2077e:                 // AoWE.TFire
        case 0x21242:                 // SmokeCloud.TSmokeCloud
        case 0x2123f:                 // PoisonC.TPoisonCloud
        case 0x2009c:                 // OverlayObject.TOverlayObject
        case 0x2016c:                 // AoWE.TFrozenWaterHS
            return false;
        default:
            return true;
    }
}

// See mapviz.hpp. Mirrors Road.TRoadResource.Show@5561713c's branch tree.
void road_frames(std::uint8_t b, std::vector<std::size_t>& out) {
    out.clear();
    if (b == 0) {
        out.push_back(7);  // isolated tile
        return;
    }
    if ((b & 0x40) == 0) {
        if (b & 0x10) out.push_back(5);  // W arm
        if (b & 0x04) out.push_back(3);  // E arm
    } else {
        out.push_back(7);  // E/W straight
    }
    if ((b & 0x80) == 0) {
        if (b & 0x02) out.push_back(2);  // NE arm
        if (b & 0x20) out.push_back(6);  // NW arm
    } else {
        out.push_back(8);  // NE/NW straight
    }
    if (b & 0x01) out.push_back(1);  // N arm
    if (b & 0x08) out.push_back(4);  // S arm
}

// See mapviz.hpp. AoWE.Land@5575aea4 returns non-zero for every terrain except
// 0 (water), 6 (ice), 9 (lava), 10 (dirt) and 13 (upper-ice):
//   if (t != 0 && (char)(t-6) != 0 && (byte)(t-9) > 1 && (char)(t-13) != 0)
//       return 1;  else return 0;
bool bridge_land(int terrain) {
    return terrain != 0 && terrain != 6 && terrain != 9 && terrain != 10 &&
           terrain != 13;
}

// HSEngine.HNtoHX@5560e08c gives the axial offset of the neighbour in HN
// direction 1..6 = the table (0,-1)/(1,-1)/(1,0)/(0,1)/(-1,0)/(-1,-1);
// HSEngine.CenterHNtoHX@5560e17c then folds the column stagger (odd column +
// odd dx -> one row down). On the staggered grid those six neighbours render as
// N, NE, SE, S, SW, NW (the +-32px columns always carry a +-16px row offset).
// Returns false only for an out-of-range direction; the caller checks bounds.
bool hn_neighbour(std::uint32_t x, std::uint32_t y, int dir, int& nx, int& ny) {
    if (dir < 1 || dir > 6) return false;
    static const int dx[6] = {0, 1, 1, 0, -1, -1};   // N, NE, E, S, W, NW
    static const int dy[6] = {-1, -1, 0, 1, 0, -1};
    const int i = dir - 1;
    nx = static_cast<int>(x) + dx[i];
    ny = static_cast<int>(y) + dy[i];
    if ((x & 1u) != 0 && (dx[i] & 1) != 0) ++ny;  // CenterHNtoHX stagger
    return true;
}

// See mapviz.hpp. Mirrors HxBridge.THexagonBridge.UpdateTransitions@55799fd8
// (starting from the serialised 0xff) -> DirectionValid@55799c04: a direction d
// is valid when both the HN (d+1) and HN (d+4) neighbours are land, tested in
// the order 1, 2, 0.
std::uint8_t hex_bridge_direction(std::uint32_t x, std::uint32_t y,
                                  std::uint32_t width, std::uint32_t height,
                                  const std::vector<std::uint8_t>& land) {
    auto is_land = [&](int dir) {
        int nx, ny;
        if (!hn_neighbour(x, y, dir, nx, ny)) return false;
        if (nx < 0 || ny < 0 || nx >= static_cast<int>(width) ||
            ny >= static_cast<int>(height)) {
            return false;
        }
        return land[static_cast<std::size_t>(ny) * width +
                    static_cast<std::size_t>(nx)] != 0;
    };
    static const int order[3] = {1, 2, 0};
    for (int d : order) {
        if (is_land(d + 1) && is_land(d + 4)) {
            return static_cast<std::uint8_t>(d);
        }
    }
    return 0xff;
}


namespace {

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

// Indexes every *.ILB under `root` by its lowercase stem.
void index_ilbs(const std::string& root, std::map<std::string, std::string>& idx) {
    std::error_code ec;
    if (!fs::exists(root, ec)) return;
    for (fs::recursive_directory_iterator it(root, ec), end;
         !ec && it != end; it.increment(ec)) {
        if (!it->is_regular_file(ec)) continue;
        if (lower(it->path().extension().string()) != ".ilb") continue;
        idx.emplace(lower(it->path().stem().string()), it->path().string());
    }
}

struct Sprite {
    std::vector<std::uint8_t> rgba;
    std::uint32_t w = 0, h = 0;
    // Visual-box offset: base_x+xshift / base_y+yshift from the ILB descriptor.
    // The engine blits the decoded content at (anchor + vx, anchor + vy) -
    // re/exports/ilpack/TSprite16.ShowOpaque.c (`dest = param + base + shift`).
    std::int32_t vx = 0, vy = 0;
    // The ILB class the frame came from. Class 0x16 (TSprite16) is drawn with a
    // *blend* in the sequence path (see blit_brighten), not an opaque blit.
    std::uint32_t class_id = 0;
    bool ok = false;
};

// Rejects absurd dimensions (a mis-parsed record would otherwise request a
// multi-gigabyte buffer).
bool plausible(std::uint32_t w, std::uint32_t h) {
    return w > 0 && h > 0 && w <= 2048 && h <= 2048;
}

// Every frame of one image library (one .ILB file), decoded on first use.
struct Library {
    bool tried = false;
    std::vector<Sprite> frames;      // indexed by image-slot id (see get_library)
    std::vector<std::string> names;  // lowercased entry names, same index
};

// Fills a Library from a parsed ILB, indexed by the record's **image-slot id**.
void fill_library(Library& lib, const IlbFile& ilb) {
    std::string err;
    // Index by `IlbEntry::index`, not by the
    // array position: libraries can skip ids (EARTHHEX.ILB runs 0..32 then
    // 40..43) and the engine's `TCustomImageLibrary.Get(i)` uses the id. That is
    // how `IsoHex.TUpperIsometricHexagon.Show` asking for frame 0x28 resolves to
    // `hu_er.BMP` (EARTHHEX slot 40) and how HEXTRANS.ILB's 16-spaced terrain
    // blocks line up. For a contiguous library (id == position) nothing changes.
    std::size_t n = 0;
    for (const IlbEntry& e : ilb.entries()) {
        if (static_cast<std::size_t>(e.index) > n) n = e.index;
    }
    lib.frames.assign(n + 1, Sprite{});
    lib.names.assign(n + 1, std::string());
    for (std::size_t i = 0; i < ilb.entries().size(); ++i) {
        const IlbEntry& e = ilb.entries()[i];
        const std::size_t slot = static_cast<std::size_t>(e.index);
        if (slot > n) continue;
        Sprite s;
        s.vx = e.base_x + e.xshift;
        s.vy = e.base_y + e.yshift;
        s.class_id = e.class_id;
        if (plausible(e.width, e.height) && plausible(e.rle_w, e.rle_h)) {
            try {
                if (ilb.decode_rgb(i, s.rgba, s.w, s.h, err) && plausible(s.w, s.h)) {
                    s.ok = true;
                }
            } catch (const std::exception&) {
                s.ok = false;
            }
        }
        lib.frames[slot] = std::move(s);
        lib.names[slot] = lower(e.name);
    }
}

const Library& get_library(std::map<std::string, Library>& cache,
                           const std::string& file) {
    Library& lib = cache[file];
    if (lib.tried || file.empty()) return lib;
    lib.tried = true;
    IlbFile ilb;
    std::string err;
    if (!ilb.load(file, err) || ilb.entries().empty()) return lib;
    fill_library(lib, ilb);
    return lib;
}

// The decoded-sprite cache is shared by every render in the process, exactly as
// the engine's `TILManagerList` keeps its libraries loaded: a level re-render
// (the in-game screen re-renders after every unit move) and the movement overlay
// would otherwise rebuild the ILB index and re-decode the whole level's art -
// measured at ~286 ms per move, and ~340 ms per hover for the overlay alone.
// Keyed by file path, so different game roots coexist.
std::map<std::string, Library>& shared_libraries() {
    static std::map<std::string, Library> cache;
    return cache;
}

// The game's unit roster (Unitres.pfs + Unitgfx.pfs + Herores.pfs + Release.hss),
// shared for the life of the process and keyed by the game root like
// `shared_resolver`: loading it parses three .PFS files and every unit's
// animation sequence list, so it is far too heavy to redo per rendered frame -
// and a live front-end now resolves the armies on *every* map frame
// (`draw_unit_overlays`, which re-composites the units over the buildings).
// Measured: the hover refresh went from 0.3 ms to 11.3 ms per step when the
// per-frame pass loaded its own copy (build.ps1's `game hover` guard caught it).
//
// The pointer is to the cached object and must not outlive the process' root
// cache; `UnitEntry::gfx` points into the *same* catalog's `graphics`, so a
// caller must never copy a catalog out of here (the whole map owns the object -
// `std::unique_ptr` keeps that pointer stable whatever the map does).
const UnitCatalog* shared_unit_catalog(const std::string& root) {
    static std::map<std::string, std::unique_ptr<UnitCatalog>> cache;
    auto it = cache.find(root);
    if (it == cache.end()) {
        auto cat = std::unique_ptr<UnitCatalog>(new UnitCatalog());
        std::string err;
        if (!load_unit_catalog(root, *cat, err)) cat->units.clear();
        it = cache.emplace(root, std::move(cat)).first;
    }
    const UnitCatalog* cat = it->second.get();
    return (cat != nullptr && !cat->units.empty()) ? cat : nullptr;
}

// A map-object resource that carries its image library INLINE (rw tag 8) is
// decoded straight from those bytes; `key` must not look like a file path.
const Library& get_library_embedded(std::map<std::string, Library>& cache,
                                    const std::string& key,
                                    const std::vector<std::uint8_t>& data) {
    Library& lib = cache[key];
    if (lib.tried) return lib;
    lib.tried = true;
    IlbFile ilb;
    std::string err;
    if (!ilb.load_memory(data.data(), data.size(), err) ||
        ilb.entries().empty()) {
        return lib;
    }
    fill_library(lib, ilb);
    return lib;
}

// Logical object-library names ("gr_tree.BMP") name a sprite set whose *.ILB
// on disk uses an abbreviated stem ("zgr_Tre.ILB"): the retail resource table
// stores no file/entry matching the logical name, so map it explicitly. The
// families are "<terrain>_<object>" -> "z<terrain>_<abbrev>.ILB"; the terrain
// prefix selects the folder (gr=Grass, st=Stone, sn=Snow, de=Desert, wl=Wall,
// ug=Underground) and the object word selects the sprite set. Keys are the
// lowercased name stem (extension stripped).
const std::map<std::string, std::string>& object_library_alias() {
    static const std::map<std::string, std::string> a = {
        {"gr_tree", "zgr_tre"},    {"gr_dtree", "zgr_dtree"},
        {"gr_flwer", "zgr_flwr"},  {"gr_rocks", "zgr_srk"},
        {"gr_toad", "zgr_toad"},   {"gr_willow", "zgr_wllw"},
        {"gr_bones", "zgr_bone"},  {"gr_ston", "zgr_ston"},
        {"gr_stone", "zgr_ston"},  {"gr_hill", "zgr_hill"},
        {"gr_shrub", "zgr_shrb"},  {"gr_pine", "zgr_pine"},
        {"gr_crops", "zgr_crop"},  {"crops", "zgr_crop"},
        {"st_tree", "zst_tree"},   {"st_dtree", "zst_dtree"},
        {"st_bones", "zst_bone"},  {"st_ston", "zst_ston"},
        {"st_hill", "zst_hill"},   {"st_shrub", "zst_shrb"},
        {"st_lveg", "zst_lveg"},   {"st_brk", "zst_brk"},
        {"st_crops", "zst_crops"}, {"sn_tree", "zsn_pine"},
        {"sn_dtree", "zsn_dtree"}, {"sn_bones", "zsn_bone"},
        {"sn_ston", "zsn_ston"},   {"sn_rocks", "zsn_srk"},
        {"sn_hill", "zsn_hill"},   {"sn_shrub", "zsn_shrb"},
        {"sn_lveg", "zsn_lveg"},   {"sn_crops", "zsn_crops"},
        {"wl_tree", "zwl_tree"},   {"wl_dtree", "zwl_dtree"},
        {"wl_bones", "zwl_bone"},  {"wl_rocks", "zwl_srk"},
        {"wl_ston", "zwl_ston"},   {"wl_hill", "zwl_hill"},
        {"wl_shrub", "zwl_shrb"},  {"wl_lveg", "zwl_lveg"},
        {"wl_grave", "zwl_grave"}, {"de_tree", "zde_tree"},
        {"de_dtree", "zde_dtree"}, {"de_bones", "zde_bone"},
        {"de_crops", "zde_crops"}, {"de_cact", "zde_cact"},
        {"ug_bones", "zug_bone"},  {"ug_crop", "zug_crop"},
        {"ug_lveg", "zug_lveg"},
        // `H_Border.BMP` is the outermost terrain of every map (the 6-hex
        // frame around the playable area, class BorderHx.TBorderHexagon). Its
        // 12-variant 48x32 base hexagon library is *not* in the retail install
        // (no H_Border.ILB exists anywhere under the game root - the only
        // h_*-entry libraries on disk are H_Faces/IceHex/Waterhex plus the
        // TCombat\Base_hex set H_De/H_Dr/H_Gr/H_Ic/H_La/H_Sn/H_St/H_Wa/H_Wl).
        // In the original the frame reads as open ocean - it measures the same
        // ~(41,57,90) as the water hexes - so fall back to the world-map water
        // library, whose `H_Wa2.BMP` base hexagon is byte-for-byte the ocean
        // tile the frame abuts.
        {"h_border", "waterhex"},
        // Roads (Road.TRoadResource, engine class 0x20165). The logical library
        // `Rd_<terrain>.BMP` names the road overlay set that lives as
        // `H_<Terrain>Dr.ILB` (`Rd_gr` -> H_GrDr, `Rd_de` -> H_DeDr, ...). The
        // road resources normally carry that 8-frame overlay library INLINE (rw
        // tag 8, slots 1..8), so this is only a fallback - but it pins the family
        // in one place and is the decompile-backed name pairing.
        {"rd_gr", "h_grdr"}, {"rd_de", "h_dedr"}, {"rd_sn", "h_sndr"},
        {"rd_st", "h_stdr"}, {"rd_wl", "h_wldr"},
    };
    return a;
}

// Transient effects the engine composites with a blend mode (additive/screen),
// so a plain opaque blit leaves a dark square (e.g. GreenTwinkle). The static
// map view skips them. `aow::is_effect_library` is the single definition (it
// also recognises the `EFFECTS\` prefix); this wrapper keeps the call sites
// reading naturally.
bool is_transient_effect(const std::string& stem) {
    return is_effect_library(stem);
}

// MOUNTAIN.ILB packs five 5-frame terrain blocks in order: desert (Mnt_de),
// grass (gr_mnt2), snow (Mnt_sn), stone (Mnt_st), wall (wl_mnt). Within a block
// the five frames are the mountain *sizes* (60x50, 93x66, 93x66, 125x101,
// 188x128 - frames 1 and 2 are the same size).
//
// The block is chosen by the resource's `res_type`. That mapping was originally
// guessed and was WRONG for three of the five terrains (snow/wall/desert were
// permuted), so e.g. a snow map drew desert mountains. It is now pinned by
// cross-referencing every map's mountain `res_type` mix against that map's
// terrain tiles (`tools/mtmap.ps1`, 14 maps):
//
//   * Frosty Lagoon / Frozen Waters / Icedale (snow-only maps) use res_type 14
//     almost exclusively          -> 14 = snow;
//   * Green Valley (grass-only) uses 17 -> 17 = grass;
//   * Blackwood Forest has NO desert tiles yet its most common mountain type is
//     13, and the wall volcano resources (`MOUNTAIN\WL_VOLCS/L`) carry res_type
//     13 -> 13 = wall;
//   * Desolate Maugogom / Blood Isles / Lord of the Rings: 16 tracks the stone
//     tile count and 15 the desert tile count -> 16 = stone, 15 = desert.
//
// The other two mountain types (10 and 22) are the underground twins; their
// five shapes are shared with the surface blocks (identified by the resource's
// dXmainP/dYmainP triple) but their res_type does not select one, so they are
// left unmapped pending a decompile of TMapObjectResource.ShowComplexImage.
// See re/exports/hsepack/HSEngine.TMultiHexMOR.ReadWrite.c and
// Mountain.TMountainMO.CanPlace.c, and docs/01_FORMATS.md.
//
// Returns the block's first frame, or -1 when the type is unknown.
int mountain_block(std::uint32_t type) {
    switch (type) {
        case 15: return 0;   // desert
        case 17: return 5;   // grass
        case 14: return 10;  // snow
        case 16: return 15;  // stone
        case 13: return 20;  // wall
        default: return -1;
    }
}

// Resolves a resource image-library name (e.g. H_gr.BMP,
// MOUNTAIN/MOUNTAIN.ILB, zgr_Trees5.BMP) to an *.ILB file:
//   1. fast path - the name's stem matches an ILB file stem (the terrain
//      libraries: "H_gr.BMP" -> H_Gr.ILB);
//   2. fallback - some ILB contains an entry carrying that image name (the
//      object libraries: "zgr_Trees5.BMP" lives inside zgr_Tre.ILB);
//   3. alias - the logical object-library names ("gr_tree.BMP") map to their
//      abbreviated file stem via object_library_alias().
// The entry-name index is built once, reading only ILB directories.
class LibResolver {
public:
    explicit LibResolver(std::map<std::string, std::string> index)
        : index_(std::move(index)) {}

    std::string resolve(const std::string& name) {
        if (name.empty()) return {};
        std::string n = name;
        for (char& c : n) {
            if (c == '\\') c = '/';
        }
        const std::string stem = lower(fs::path(n).stem().string());
        const auto it = index_.find(stem);
        if (it != index_.end()) return it->second;
        const std::string by_entry = find_by_entry_name(stem);
        if (!by_entry.empty()) return by_entry;
        const auto alias = object_library_alias().find(stem);
        if (alias != object_library_alias().end()) {
            const auto f = index_.find(alias->second);
            if (f != index_.end()) return f->second;
        }
        return {};
    }

private:
    std::string find_by_entry_name(const std::string& stem) {
        const auto hit = entry_names_.find(stem);
        if (hit != entry_names_.end()) return hit->second;
        if (!scanned_) {
            scanned_ = true;
            for (const auto& kv : index_) {
                std::vector<std::string> names;
                std::string err;
                if (!ilb_entry_names(kv.second, names, err)) continue;
                for (const std::string& nm : names) {
                    const std::string key = lower(fs::path(nm).stem().string());
                    entry_names_.emplace(key, kv.second);  // first ILB wins
                }
            }
        }
        const auto it = entry_names_.find(stem);
        return (it != entry_names_.end()) ? it->second : std::string();
    }

    std::map<std::string, std::string> index_;        // file stem -> path
    std::map<std::string, std::string> entry_names_;  // entry stem -> path
    bool scanned_ = false;
};

// The game-root ILB index and the resolver that owns it, shared for the life of
// the process: building the index is a recursive directory walk (tens of
// thousands of files on a real install) and the resolver's first unresolved
// lookup scans every ILB's entry names. Both are read-only after the first use
// apart from that lazy scan, and the port renders from one thread.
LibResolver& shared_resolver(const std::string& root) {
    static std::map<std::string, LibResolver> cache;
    auto it = cache.find(root);
    if (it == cache.end()) {
        std::map<std::string, std::string> idx;
        index_ilbs(root, idx);
        it = cache.emplace(root, LibResolver(std::move(idx))).first;
    }
    return it->second;
}

// The two sprites of one owner flag, already positioned inside the canvas (see
// `structure_flag_sprites`).
struct FlagSprites {
    const Sprite* pole = nullptr;
    int pole_x = 0, pole_y = 0;
    const Library* pole_lib = nullptr;
    std::size_t pole_frame = 0;
    std::string pole_name;
    const Sprite* flag = nullptr;
    int flag_x = 0, flag_y = 0;
    const Library* flag_lib = nullptr;
    std::size_t flag_frame = 0;
    std::string flag_name;
};

// Resolves and positions the owner flag a placed takeable structure draws
// (`AoWE.TPlayerStructure.Show@55760f94` -> `AoWE.TFlagControl.ShowFlag
// @5575a1e8` / `ShowStructureFlag@5575a34c`). The structure's sprite box
// top-left is `sx,sy` (= `hex_origin(x, y)` = HXtoHP) and `clock` the map clock.
//
// The engine hands the flag control `HXtoHP + (resource+0x68, +0x6c)` and blits
//   * the pole at that anchor, and
//   * the flag at anchor + (-0x15, -0x10),
// each with its own `base+shift` added inside the image's Show - the exact same
// composition `TMapObjectResource.ShowComplexImage` uses for a structure's
// sequence frames, so the port adds the same `kHexFieldMargin` in x as those.
//
// Returns false (leaving `out` untouched) when the structure draws no flag, or
// when either library/frame cannot be resolved.
bool structure_flag_sprites(const MapHs& hs, const MapResource* r, int sx, int sy,
                            int clock, const std::vector<std::uint8_t>& races,
                            const std::vector<std::string>& race_ilbs,
                            LibResolver& resolver,
                            std::map<std::string, Library>& cache,
                            FlagSprites& out) {
    StructureFlag sf;
    if (!resolve_structure_flag(r, hs.class_id, hs.structure_owner, hs.city_level,
                                hs.city_flags, races, race_ilbs, clock, sx, sf)) {
        return false;
    }
    auto load = [&](const std::string& name) -> const Library* {
        const std::string file = resolver.resolve(name);
        if (file.empty()) return nullptr;
        const Library& lib = get_library(cache, file);
        return lib.frames.empty() ? nullptr : &lib;
    };
    const Library* pole_lib = load(sf.pole_library);
    const Library* flag_lib = load(sf.flag_library);
    out.pole_name = sf.pole_library;
    out.flag_name = sf.flag_library;
    if (pole_lib == nullptr && flag_lib == nullptr) return false;
    bool any = false;
    if (pole_lib != nullptr && sf.pole_frame < pole_lib->frames.size()) {
        const Sprite& s = pole_lib->frames[sf.pole_frame];
        if (s.ok) {
            out.pole = &s;
            out.pole_lib = pole_lib;
            out.pole_frame = sf.pole_frame;
            out.pole_x = sx + sf.dx + s.vx + kHexFieldMargin;
            out.pole_y = sy + sf.dy + s.vy;
            any = true;
        }
    }
    if (flag_lib != nullptr && sf.flag_frame < flag_lib->frames.size()) {
        const Sprite& s = flag_lib->frames[sf.flag_frame];
        if (s.ok) {
            out.flag = &s;
            out.flag_lib = flag_lib;
            out.flag_frame = sf.flag_frame;
            out.flag_x = sx + sf.dx + sf.sprite_dx + s.vx + kHexFieldMargin;
            out.flag_y = sy + sf.dy + sf.sprite_dy + s.vy;
            any = true;
        }
    }
    return any;
}


void blit(std::vector<std::uint8_t>& dst, int dw, int dh, const Sprite& s,
          int dx, int dy) {
    if (!s.ok) return;
    for (std::uint32_t y = 0; y < s.h; ++y) {
        const int ty = dy + static_cast<int>(y);
        if (ty < 0 || ty >= dh) continue;
        for (std::uint32_t x = 0; x < s.w; ++x) {
            const int tx = dx + static_cast<int>(x);
            if (tx < 0 || tx >= dw) continue;
            const std::uint8_t* sp = &s.rgba[(static_cast<std::size_t>(y) * s.w + x) * 4];
            if (sp[3] == 0) continue;
            std::uint8_t* dp = &dst[(static_cast<std::size_t>(ty) * dw + tx) * 4];
            if (sp[3] == 255) {
                dp[0] = sp[0]; dp[1] = sp[1]; dp[2] = sp[2]; dp[3] = 255;
            } else {
                for (int k = 0; k < 3; ++k) {
                    dp[k] = static_cast<std::uint8_t>(
                        (sp[k] * sp[3] + dp[k] * (255 - sp[3])) / 255);
                }
                dp[3] = 255;
            }
        }
    }
}

// AoW1's class-0x16 (TSprite16) sequence sprites are drawn with a BLEND, not an
// opaque colour-key blit. `HSEngine.TMapObjectResource.ShowComplexImage@5560a134`
// calls the image's vtable+0x84 slot, and for a mask-less TSprite16 that slot is
// `ImageLib.TSprite16.ShowTransparent@0x55211ea0` -> `ShowBlended@0x55211b64`
// with the GFXE blend table - the per-channel BRIGHTEN table
// (`GFXE.GenerateBrightenBlendTable`, i.e. `dst = max(dst, src)`, the same table
// the hexagon selector is painted with). A glow sprite therefore leaves the
// terrain untouched where it is dark and lights it up where it is bright; an
// opaque blit instead paints its dark surround over the map - the black discs.
// Measured against the original game's screenshot of `Bloody Marauders.hsm`
// (bin/GlowRef.png vs bin/ourGlow.png, aligned): on the glow pixels max() gives
// a mean |delta| of 25.5 where the opaque blit gives 33.1 and a 50% lerp 37.2.
void blit_brighten(std::vector<std::uint8_t>& dst, int dw, int dh,
                   const Sprite& s, int dx, int dy) {
    if (!s.ok) return;
    for (std::uint32_t y = 0; y < s.h; ++y) {
        const int ty = dy + static_cast<int>(y);
        if (ty < 0 || ty >= dh) continue;
        for (std::uint32_t x = 0; x < s.w; ++x) {
            const int tx = dx + static_cast<int>(x);
            if (tx < 0 || tx >= dw) continue;
            const std::uint8_t* sp =
                &s.rgba[(static_cast<std::size_t>(y) * s.w + x) * 4];
            if (sp[3] == 0) continue;
            std::uint8_t* dp = &dst[(static_cast<std::size_t>(ty) * dw + tx) * 4];
            for (int k = 0; k < 3; ++k) {
                if (sp[k] > dp[k]) dp[k] = sp[k];
            }
            dp[3] = 255;
        }
    }
}

// Flat terrain hexagon (fallback when a terrain library cannot be resolved).
// Same footprint as the real tile: a `w` x `h` hexagon (48x32) with 45-degree
// flanks and a flat top/bottom edge of `w - h` px (48-32 -> 16px). That 16px
// flat edge is exactly the overlap the engine's column-staggered grid relies on:
// neighbouring columns advance by `w - h/2` (32) so the flanks interlock without
// a seam. `(ox, oy)` is the sprite box's top-left corner (see origin()).
void fill_hex(std::vector<std::uint8_t>& dst, int dw, int dh, int ox, int oy,
              int w, int h, const std::uint8_t* rgb) {
    const int hh = h / 2;  // 45-degree flank: horizontal run == vertical rise
    for (int y = 0; y < h; ++y) {
        const int ty = oy + y;
        if (ty < 0 || ty >= dh) continue;
        // Rows run from flat-edge (0) to waist (hh) and back symmetrically.
        const int edge = (y < hh) ? y : (h - 1 - y);
        const int inset = hh - edge;
        for (int tx = ox + inset, last = ox + w - 1 - inset; tx <= last; ++tx) {
            if (tx < 0 || tx >= dw) continue;
            std::uint8_t* dp = &dst[(static_cast<std::size_t>(ty) * dw + tx) * 4];
            dp[0] = rgb[0]; dp[1] = rgb[1]; dp[2] = rgb[2]; dp[3] = 255;
        }
    }
}

// Terrain-type colour from the library name.
void terrain_color(const std::string& ilb, std::uint8_t* rgb) {
    const std::string n = lower(ilb);
    auto has = [&](const char* s) { return n.find(s) != std::string::npos; };
    // Ocean blue, matching the decoded `H_Wa2.BMP` base hexagon (its pixels
    // average ~(34,56,90)); the fallback under-layer uses the same colour so no
    // flat hue bleeds through the hex seams.
    if (has("water") || has("h_wa")) { rgb[0] = 34; rgb[1] = 56; rgb[2] = 90; }
    else if (has("ice")) { rgb[0] = 180; rgb[1] = 220; rgb[2] = 235; }
    else if (has("snow") || has("h_sn")) { rgb[0] = 235; rgb[1] = 235; rgb[2] = 245; }
    // H_Border.BMP is the map's outer frame; it is ocean in the original and is
    // resolved to the water hexagon library (see object_library_alias).
    else if (has("border")) { rgb[0] = 34; rgb[1] = 56; rgb[2] = 90; }
    else if (has("h_de")) { rgb[0] = 215; rgb[1] = 195; rgb[2] = 130; }
    else if (has("h_wl")) { rgb[0] = 150; rgb[1] = 120; rgb[2] = 90; }
    else if (has("h_st")) { rgb[0] = 170; rgb[1] = 180; rgb[2] = 110; }
    else if (has("h_gr")) { rgb[0] = 95; rgb[1] = 150; rgb[2] = 70; }
    else if (has("rock") || has("earth")) { rgb[0] = 120; rgb[1] = 100; rgb[2] = 80; }
    else { rgb[0] = 120; rgb[1] = 120; rgb[2] = 120; }
}

// The engine's terrain-TYPE number for a base hexagon resource, from its library
// name. Evidence: AoWHex.TAoWHexagon.UpdateTransition@5579a9e0 builds the overlay
// frame as `ownterr*16 + 6 + edge` and HEXTRANS.ILB lays its 9-frame blocks out
// 16 apart at idx 22/38/54/70/86 for T_GR/T_DE/T_SN/T_ST/T_WL - so the land
// families are terrain 1..5. Returns -1 for anything else (water/border/ice/...,
// whose cases are the special branches of that function and are out of scope).
int terrain_type_of(const std::string& ilb) {
    const std::string n = lower(ilb);
    auto has = [&](const char* s) { return n.find(s) != std::string::npos; };
    if (has("h_gr")) return 1;  // grass  -> T_GR
    if (has("h_de")) return 2;  // desert -> T_DE
    if (has("h_sn")) return 3;  // snow   -> T_SN
    if (has("h_st")) return 4;  // stone  -> T_ST
    if (has("h_wl")) return 5;  // wall   -> T_WL
    return -1;
}

}  // namespace

// One shoreline shelf sprite a water/ice hexagon draws: `frame` is an image slot
// of its own library (WATERHEX.ILB `t_wa-gr` slot 0..0x1e, ICEHEX.ILB), `dy` the
// vertical anchor (the engine uses -0x20 for most pieces and -0x1f for the
// "corner" frames 0x16/0x13/0x1e/0x12/0x1c). See water_shore_pieces.
// ShorePiece / water_shore_pieces are declared in aow/mapviz.hpp so mapres_test
// can lock the frame selection.

// See mapviz.cpp header comment / docs/ROADMAP_mapviewer_fidelity.md (Phase 4
// coast). The water hexagon composites its tan shoreline shelf from sprites
// chosen by a 6-bit neighbour mask. Evidence:
//   AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4 calls
//   IsoHex.TLowerIsometricHexagon.ShowDynamic@5561a400 with `this`; that function
//   reads the sprite's +0x14 mask (one bit per direction, set by
//   AoWHex.TAoWWaterHexagon.UpdateTransitions@5579ad98 for every neighbour that
//   carries no raised/border hexagon - i.e. the flat land) and blits the matching
//   library slots at x = HXtoHP - 0x13, y = HXtoHP.y - 0x20 (or - 0x1f). `mask`
//   bit i is direction i+1 (N/NE/E/S/W/NW, HSEngine.HNtoHX + TranslateHX);
//   `variant` is the engine's per-hex 0..2 phase (sprite +0x15 >> 6, reduced from
//   its RandInt(4); the port derives a deterministic hash instead). The result is
//   the ordered list of (frame, dy) pieces to blit.
void water_shore_pieces(int b, int variant, std::vector<ShorePiece>& out) {
    out.clear();
    if (b == 0) return;
    const int y_far = -0x20;   // iVar5
    const int y_near = -0x1f;  // iVar2 - 0x1f
    if ((b & 1) == 0) {
        if ((b & 6) == 6) out.push_back({variant + 10, y_far});
        else if ((b & 2) == 2) out.push_back({0x11, y_far});
        if ((b & 0x30) == 0x30) out.push_back({variant + 7, y_far});
        else if ((b & 0x20) == 0x20) out.push_back({0x10, y_far});
    } else {
        if ((b & 0x23) == 0x23) out.push_back({6, y_far});
        else if ((b & 0x23) == 1) out.push_back({0x0d, y_far});
        if ((b & 7) == 7) out.push_back({0x19, y_far});
        else if ((b & 3) == 3) out.push_back({variant + 3, y_far});
        if ((b & 0x31) == 0x31) out.push_back({0x18, y_far});
        else if ((b & 0x21) == 0x21) out.push_back({variant, y_far});
    }
    if ((b & 8) == 0) {
        if ((b & 0xe) == 4) out.push_back({0x0f, y_far});
        else if ((b & 0x38) == 0x10) out.push_back({0x0e, y_far});
    } else {
        if ((b & 0xe) == 0xe) out.push_back({0x16, y_near});
        else if ((b & 0xc) == 0xc)
            out.push_back({variant == 2 ? 0x13 : variant + 0x1e, y_near});
        if ((b & 0x38) == 0x38) out.push_back({0x15, y_far});
        else if ((b & 0x18) == 0x18)
            out.push_back({variant == 2 ? 0x12 : variant + 0x1c, y_far});
        if ((b & 0x1c) == 8) out.push_back({0x17, y_far});
    }
}
namespace {

// True when a library entry name denotes a *base* terrain hexagon rather than a
// transition/fringe overlay. Base hexes are named `zH_*.BMP` / `H_*.BMP` (they
// contain "h_"); the overlays are `t_wa-*`, `T_Ic-*`, `ZT_*` (no "h_").
// `name` must already be lowercased.
bool is_base_hex_name(const std::string& name) {
    return name.find("h_") != std::string::npos;
}

// The sprite index to draw for a *terrain* hexagon. A mixed library such as
// WATERHEX.ILB lists its transition overlays first and the `H_Wa2` base hexes
// last, so the stored variant frame must index into the base-hex entries, not
// the raw frame list. Falls back to the stored frame when the library carries
// no recognisable base entries.
std::size_t terrain_frame(const Library& lib, std::size_t want) {
    if (lib.frames.empty()) return 0;
    if (lib.names.size() != lib.frames.size()) return want % lib.frames.size();
    std::size_t base_count = 0;
    for (const std::string& n : lib.names) {
        if (is_base_hex_name(n)) ++base_count;
    }
    if (base_count == 0) return want % lib.frames.size();
    const std::size_t k = want % base_count;
    std::size_t seen = 0;
    for (std::size_t i = 0; i < lib.names.size(); ++i) {
        if (!is_base_hex_name(lib.names[i])) continue;
        if (seen == k) return i;
        ++seen;
    }
    return want % lib.frames.size();
}

// The base terrain hexagon is 48x32 (docs/01_FORMATS.md); confirm that against
// a decoded base frame and adopt the real dimensions if they differ, so the
// grid step and the fallback hexagon always match the art.
void detect_tile_size(const std::vector<MapResource>& res, LibResolver& resolver,
                      std::map<std::string, Library>& cache, int& w, int& h) {
    for (const MapResource& r : res) {
        if (r.class_id != 0x20153 && r.class_id != 0x2073e) continue;
        const std::string file = resolver.resolve(r.ilb);
        if (file.empty()) continue;
        const Library& lib = get_library(cache, file);
        if (lib.names.size() != lib.frames.size()) continue;
        for (std::size_t i = 0; i < lib.frames.size(); ++i) {
            if (!is_base_hex_name(lib.names[i]) || !lib.frames[i].ok) continue;
            const std::uint32_t fw = lib.frames[i].w, fh = lib.frames[i].h;
            if (fw >= 16 && fw <= 256 && fh >= 16 && fh <= 256 && fw > fh) {
                w = static_cast<int>(fw);
                h = static_cast<int>(fh);
                return;
            }
        }
    }
}

}  // namespace

std::map<std::uint32_t, std::size_t> map_resource_frames(
    const std::vector<MapResource>& res) {
    std::map<std::uint32_t, std::size_t> out;
    std::map<std::pair<std::uint32_t, std::string>,
             std::vector<const MapResource*>> groups;
    for (const MapResource& r : res) {
        if (r.ilb.empty()) continue;
        std::string name = r.ilb;
        for (char& c : name) {
            if (c == '\\') c = '/';
        }
        groups[{r.res_type, lower(fs::path(name).stem().string())}].push_back(&r);
    }
    for (auto& kv : groups) {
        std::vector<const MapResource*>& v = kv.second;
        std::sort(v.begin(), v.end(),
                  [](const MapResource* a, const MapResource* b) {
                      return a->res_index < b->res_index;
                  });
        // `res_index` is a contiguous counter per (res_type, library) *run* - a
        // library's own frames. Some libraries appear in more than one run (e.g.
        // H_Wa2.BMP at idx 40..48 and 102..107 around the H_Wl/H_co blocks), and
        // a run's frame is its offset from that run's first id. Starting a new
        // run whenever the counter is not `prev + 1` keeps every frame < the
        // library's entry count instead of smearing a stale base across a gap.
        std::uint32_t run = 0;
        bool have_prev = false;
        std::uint32_t prev = 0;
        for (const MapResource* r : v) {
            if (!have_prev || r->res_index != prev + 1) run = r->res_index;
            have_prev = true;
            prev = r->res_index;
            out[r->id] = static_cast<std::size_t>(r->res_index - run);
        }
    }
    return out;
}

// The engine's UNIT placement (TUnitHS.Show@55784614): a unit sprite's visual
// box lands at `HXtoHP + sequence xoff/yoff (+ the sprite's own base+shift)`.
// Every `UNITS\*.ILB` sequence in the Unitgfx tag-5 TImageSequenceList stores
// the same xoff/yoff (-16,-24), i.e. the 64x64 cell is centred up-left of the
// hex so the figure stands on it (docs/13_UNITS_ON_MAP.md).
constexpr int kUnitSeqDx = -16;
constexpr int kUnitSeqDy = -24;
// How far a unit's sprite box can reach from `hex_origin` (the 64x64 cell at the
// sequence offset -16/-24 plus the sprite's own base+shift): `draw_unit_overlays`
// rejects a hex this far outside its clip rectangle before resolving the sprite.
constexpr int kUnitClipMargin = 128;

namespace {

// Resolves the sprite of one placed unit: the library and its idle slot (the
// library's first/lowest image-slot entry - the standing pose). A plain TUnit
// The Unitgfx record a placed unit draws with: a plain TUnit looks its Unitres
// record up by id (== the unit's rw tag 10); a hero/leader (rw tag 8) goes
// through the Herores table (a hero whose THeroResource index is unset (-1)
// picks the first hero resource - the engine assigns the hero's race at load).
const UnitGraphics* find_unit_gfx(const UnitCatalog* cat, const MapUnit& u) {
    if (cat == nullptr) return nullptr;
    if (u.is_hero) {
        const std::uint32_t hi =
            u.resource < 0 ? 0u : static_cast<std::uint32_t>(u.resource);
        return cat->find_hero_gfx(hi);
    }
    for (const UnitEntry& e : cat->units) {
        if (static_cast<std::int32_t>(e.id) == u.resource) return e.gfx;
    }
    return nullptr;
}

// One placed unit's resolved pose: the image library, the image-slot the engine
// would draw, and the sequence's own xoff/yoff (the anchor is
// `HXtoHP(hex) + xoff/yoff + sprite base+shift`, TUnitHS.Show@0x55784614).
struct UnitPose {
    const Library* lib = nullptr;
    std::size_t slot = 0;
    int xoff = kUnitSeqDx;
    int yoff = kUnitSeqDy;
    bool ok = false;
};

// Resolves the pose `aow::unit_sequence_index` selects: the standing pose
// (`seq[dir-1]`) when `moving` is false, the walking cycle (`seq[dir+5]`) when
// it is. The frame within the sequence comes from `image_sequence_frame`
// (clock/phase). Falls back to the unit library's lowest decoded frame when the
// unit ships no sequence for the pose, so a placed unit always draws something.
UnitPose resolve_unit_pose(const UnitCatalog* cat, LibResolver& resolver,
                           std::map<std::string, Library>& cache,
                           const MapUnit& u, int direction, bool moving,
                           int clock, int phase) {
    UnitPose out;
    const UnitGraphics* g = find_unit_gfx(cat, u);
    if (g == nullptr) return out;
    auto first_ok = [](const Library& lib, std::size_t& slot) -> bool {
        for (std::size_t i = 0; i < lib.frames.size(); ++i) {
            if (lib.frames[i].ok) {
                slot = i;
                return true;
            }
        }
        return false;
    };
    const int idx = unit_sequence_index(direction, moving);
    if (idx >= 0 && static_cast<std::size_t>(idx) < g->sequences.size() &&
        !g->sequences[static_cast<std::size_t>(idx)].empty()) {
        const ImageSequence& seq = g->sequences[static_cast<std::size_t>(idx)];
        const std::string file = resolver.resolve(seq.library);
        if (!file.empty()) {
            const Library& lib = get_library(cache, file);
            const int frame = image_sequence_frame(
                seq, static_cast<int>(lib.frames.size()), clock, phase);
            if (frame >= 0 &&
                static_cast<std::size_t>(frame) < lib.frames.size() &&
                lib.frames[static_cast<std::size_t>(frame)].ok) {
                out.lib = &lib;
                out.slot = static_cast<std::size_t>(frame);
                out.xoff = seq.xoff;
                out.yoff = seq.yoff;
                out.ok = true;
                return out;
            }
            if (first_ok(lib, out.slot)) {
                out.lib = &lib;
                out.xoff = seq.xoff;
                out.yoff = seq.yoff;
                out.ok = true;
                return out;
            }
        }
    }
    if (g->sprite_lib.empty()) return out;
    const std::string file = resolver.resolve(g->sprite_lib);
    if (file.empty()) return out;
    const Library& lib = get_library(cache, file);
    if (!first_ok(lib, out.slot)) return out;
    out.lib = &lib;
    out.ok = true;
    return out;
}

// Resolves the *ground shadow* `AoWE.TUnitHS.ShowShadow@0x55784504` draws: the
// sequence `aow::unit_shadow_sequence_index` selects out of the very same list
// (the shipped slots 24..29, `UNITS\USHADOW.ILB`), framed by
// `image_sequence_frame` - every shipped shadow sequence is mode 0 (a stored
// base frame), so the map clock never changes it. Unlike the body there is **no
// fallback**: the 13 shipped records that carry only 24 sequences have no shadow
// slot at all and the engine then draws nothing, so neither does the port.
UnitPose resolve_unit_shadow(const UnitCatalog* cat, LibResolver& resolver,
                             std::map<std::string, Library>& cache,
                             const MapUnit& u, int direction, int clock,
                             int phase) {
    UnitPose out;
    const UnitGraphics* g = find_unit_gfx(cat, u);
    if (g == nullptr) return out;
    const int idx = unit_shadow_sequence_index(direction);
    if (idx < 0 || static_cast<std::size_t>(idx) >= g->sequences.size()) {
        return out;
    }
    const ImageSequence& seq = g->sequences[static_cast<std::size_t>(idx)];
    if (seq.empty()) return out;
    const std::string file = resolver.resolve(seq.library);
    if (file.empty()) return out;
    const Library& lib = get_library(cache, file);
    const int frame = image_sequence_frame(
        seq, static_cast<int>(lib.frames.size()), clock, phase);
    if (frame < 0 || static_cast<std::size_t>(frame) >= lib.frames.size() ||
        !lib.frames[static_cast<std::size_t>(frame)].ok) {
        return out;
    }
    out.lib = &lib;
    out.slot = static_cast<std::size_t>(frame);
    out.xoff = seq.xoff;
    out.yoff = seq.yoff;
    out.ok = true;
    return out;
}

// --- The army's size banner ------------------------------------------------
// `AoWE.TArmyHS.Show@0x557914d8` draws, after its lead unit's sprite, the
// owner race's PartyShield carrying the army's unit count, and a crown above it
// when the army carries a (visible) leader:
//
//   race  = (army.owner > 0) ? TPlayer.GetRace : 0x20   (TArmyHS.UpdateFlagID)
//   count = TArmy.GetUnitCount() - TArmy.ConcealedUnitsCount()
//   if race == 0x20:  Flags.ILB       frame count + 0x27   (ownerless shield)
//   else:             RACES\R*.ILB    frame count + 0x1d   (race shield)
//   crown (army+0x10 & 0x20): Flags.ILB frame race + 0x50
//   shield at HXtoHP + (-5, -12) + sprite base+shift
//   crown  at HXtoHP + (-5, -14) + sprite base+shift
//
// The army's stored race byte (+0x16) is `0x20` for the independents (owner 0)
// and else the owner player's race (`TPlayer+0xa5`), exactly what
// `aow::map_player_races` returns and what the owner flags already use. The
// crown bit is set by `TArmy.UpdateConcealment` when `TArmy.Leader` finds a
// `TLeader` (class 0x20232) that is not concealed from the seated player - the
// map renderer has no fog, so a leader in the army means the crown is drawn.
//
// The banner art is a PartyShield: class 0x16 in the race libraries, class 0x11
// in Flags.ILB. The reference screenshot `bin/counterRef.png` pins both the
// frames and the placement: RHUMAN frame 30 (count 1) at (15,13) and frame 34
// (count 5) at (47,29) over hex origins (15,22)/(47,38), and Flags crown frame
// 80 at (45,24) - all three match the decoded sprites pixel-for-pixel with an
// OPAQUE (colour-keyed) blit, not the class-0x16 brighten the map object path
// uses. `TArmyHS.Show` calls the image's `+0x84` Show slot, which for this art
// writes the sprite's own palette values.
constexpr int kArmyBannerDx = -5;         // TArmyHS.Show: `local_8 + -5`
constexpr int kArmyBannerDy = -0xc;       // TArmyHS.Show: `local_c + -0xc`
constexpr int kArmyCrownDy = -0xe;        // TArmyHS.Show: `local_c + -0xe`
constexpr int kArmyBannerFrameBase = 0x1d;   // race library shield frame
constexpr int kNeutralBannerFrameBase = 0x27;  // Flags.ILB ownerless shield
constexpr int kArmyCrownFrameBase = 0x50;      // Flags.ILB crown frame
constexpr std::uint8_t kNeutralRace = 0x20;

// The race image libraries in faction order (docs/11_UNIT_CATALOG.md). Used
// when a caller has no `Release.hss` race list at hand - the live unit overlay
// path only receives the map, not the resource table.
const char* race_library_fallback(int race) {
    static const char* const kLibraries[] = {
        "RACES\\RHUMAN.ILB",   "RACES\\RAZRAC.ILB", "RACES\\RLIZARDS.ILB",
        "RACES\\RFROSTL.ILB",  "RACES\\RELF.ILB",   "RACES\\RHALFLNG.ILB",
        "RACES\\RDWARF.ILB",   "RACES\\RHIGHMAN.ILB", "RACES\\RDARKELF.ILB",
        "RACES\\RORC.ILB",     "RACES\\RGOBLIN.ILB", "RACES\\RUNDEAD.ILB",
    };
    constexpr int kCount =
        static_cast<int>(sizeof(kLibraries) / sizeof(kLibraries[0]));
    if (race < 0 || race >= kCount) return nullptr;
    return kLibraries[race];
}

// One army's resolved banner parts. `lib`/`slot` are kept for the diagnostic
// CSV (and so a test can name the frame that was drawn).
struct ArmyBanner {
    const Sprite* shield = nullptr;
    const Library* shield_lib = nullptr;
    bool neutral = false;  // the ownerless Flags.ILB shield (owner 0)
    std::size_t shield_slot = 0;
    int shield_x = 0;
    int shield_y = 0;
    const Sprite* crown = nullptr;
    const Library* crown_lib = nullptr;
    std::size_t crown_slot = 0;
    int crown_x = 0;
    int crown_y = 0;
};

bool army_has_leader(const MapHs& army) {
    for (const MapUnit& u : army.units) {
        if (u.class_id == kMapLeaderClassId) return true;
    }
    return false;
}

// Resolves the banner `AoWE.TArmyHS.Show` draws for `army` at hex origin
// (sx, sy). `race_libraries` may be empty; the shipped faction table then
// supplies the same names. Returns true when the shield resolved (the crown is
// optional: it needs a leader and an owned race).
bool resolve_army_banner(const MapHs& army,
                         const std::vector<std::uint8_t>& player_races,
                         const std::vector<std::string>& race_libraries,
                         LibResolver& resolver,
                         std::map<std::string, Library>& cache, int sx, int sy,
                         ArmyBanner& out) {
    out = ArmyBanner{};
    std::uint8_t race = kNeutralRace;
    if (army.owner > 0 && army.owner < player_races.size()) {
        race = player_races[army.owner];
    }
    const int race_index = static_cast<int>(race);
    std::string race_lib;
    if (race != kNeutralRace && race_index >= 0) {
        if (race_index < static_cast<int>(race_libraries.size()) &&
            !race_libraries[static_cast<std::size_t>(race_index)].empty()) {
            race_lib = race_libraries[static_cast<std::size_t>(race_index)];
        } else {
            const char* fallback = race_library_fallback(race_index);
            if (fallback != nullptr) race_lib = fallback;
        }
    }
    // A missing / unset race behaves like the engine's ownerless byte.
    const bool neutral = race_lib.empty();
    out.neutral = neutral;
    // `TArmy.GetUnitCount` counts children; a shipped party is at most 8
    // (`TArmy.MaxSize`), the shields carry 0..9 (slot 0x1d/0x27 is the blank
    // zero-count shield). Clamp so a malformed army cannot index past the art.
    const std::size_t count =
        std::min<std::size_t>(army.units.size(), static_cast<std::size_t>(9));
    const int frame = static_cast<int>(
        count + (neutral ? kNeutralBannerFrameBase : kArmyBannerFrameBase));
    auto load = [&](const std::string& name) -> const Library* {
        if (name.empty()) return nullptr;
        const std::string file = resolver.resolve(name);
        if (file.empty()) return nullptr;
        return &get_library(cache, file);
    };
    const std::string shield_name = neutral ? "Flags.ILB" : race_lib;
    const Library* shield_lib = load(shield_name);
    if (shield_lib != nullptr && frame >= 0 &&
        static_cast<std::size_t>(frame) < shield_lib->frames.size() &&
        shield_lib->frames[static_cast<std::size_t>(frame)].ok) {
        const Sprite& s = shield_lib->frames[static_cast<std::size_t>(frame)];
        out.shield = &s;
        out.shield_lib = shield_lib;
        out.shield_slot = static_cast<std::size_t>(frame);
        out.shield_x = sx + kArmyBannerDx + s.vx;
        out.shield_y = sy + kArmyBannerDy + s.vy;
    }
    // `TArmy.UpdateConcealment` sets army+0x10 bit 0x20 when the army holds a
    // TLeader visible to the seated player; the map renderer draws every army.
    if (!neutral && army_has_leader(army)) {
        const int crown_frame = race_index + kArmyCrownFrameBase;
        const Library* crown_lib = load("Flags.ILB");
        if (crown_lib != nullptr && crown_frame >= 0 &&
            static_cast<std::size_t>(crown_frame) < crown_lib->frames.size() &&
            crown_lib->frames[static_cast<std::size_t>(crown_frame)].ok) {
            const Sprite& s =
                crown_lib->frames[static_cast<std::size_t>(crown_frame)];
            out.crown = &s;
            out.crown_lib = crown_lib;
            out.crown_slot = static_cast<std::size_t>(crown_frame);
            out.crown_x = sx + kArmyBannerDx + s.vx;
            out.crown_y = sy + kArmyCrownDy + s.vy;
        }
    }
    return out.shield != nullptr || out.crown != nullptr;
}

// The banner is drawn in the same pass as its army, after the body and before
// the next army (`TArmyHS.Show`: body, enchantments, banner, crown). The shield
// and crown are colour-keyed art, so they use the opaque/alpha blit - see the
// reference-crop note above.

// The engine's shadow composite (aow/mapviz.hpp, blit_shadow_rgba).
//
// `TUnitHS.ShowShadow` blits the shadow image through the image's vtable+0x84
// slot. For the class-0x11 art of `UNITS\USHADOW.ILB` that slot is
// `ImageLib.TRLESprite16.ShowTransparent@0x55210474`, whose DirectDraw path is
// `FUN_55210ec4` (565) / `FUN_55211248` (555). Its pixel loop skips the RLE runs
// of the escape word (the transparent part of the mask) and writes, for every
// other 16-bit pixel,
//
//     dst = ((src & 0xF7DF) + (dst & 0xF7DF)) >> 1
//
// i.e. a 50 % average of the shadow art and what is already under it - the
// shadow is *not* an opaque sprite (that would stamp its dark grey pixels over
// the terrain) and not a `TShadow` (class 0x14, the 75 % darkening mask the
// *structure* shadows use). `0xF7DF` clears the red/green low bits of both
// operands so the add cannot carry between the 565 fields, and the shift then
// re-centres each field: the port reproduces that on its 8-bit canvas by
// narrowing both operands to 565 (the exact fields the engine works in) and
// widening the result back with the ILB decoder's bit replication.
//
// Measured on `bin/ShadowRef.png` (the original game's screenshot of the Human
// leader's shadow, 160 mask pixels): this blend gives a mean |delta| of 7.2,
// where an 8-bit average gives 9.3, a 0.5625 darkening 10.4, a 0.5 darkening
// 11.7, a 0.75 darkening (`GenerateShadowBlendTable`'s table) 18.2 and an opaque
// blit 33.9.
void blit_shadow_buffer(std::vector<std::uint8_t>& canvas, int cw, int ch,
                        const std::vector<std::uint8_t>& sprite, int sw, int sh,
                        int ox, int oy, bool bgra) {
    if (sw <= 0 || sh <= 0 ||
        sprite.size() <
            static_cast<std::size_t>(sw) * static_cast<std::size_t>(sh) * 4) {
        return;
    }
    const int r0 = bgra ? 2 : 0;  // the canvas channel that holds red
    const int b0 = bgra ? 0 : 2;
    for (int y = 0; y < sh; ++y) {
        const int dy = oy + y;
        if (dy < 0 || dy >= ch) continue;
        for (int x = 0; x < sw; ++x) {
            const int dx = ox + x;
            if (dx < 0 || dx >= cw) continue;
            const std::size_t si =
                (static_cast<std::size_t>(y) * static_cast<std::size_t>(sw) +
                 static_cast<std::size_t>(x)) * 4;
            if (sprite[si + 3] == 0) continue;  // the RLE escape word
            const std::size_t di =
                (static_cast<std::size_t>(dy) * static_cast<std::size_t>(cw) +
                 static_cast<std::size_t>(dx)) * 4;
            const unsigned sr = sprite[si + static_cast<std::size_t>(r0)];
            const unsigned sg = sprite[si + 1];
            const unsigned sb = sprite[si + static_cast<std::size_t>(b0)];
            const unsigned dr = canvas[di + static_cast<std::size_t>(r0)];
            const unsigned dg = canvas[di + 1];
            const unsigned db = canvas[di + static_cast<std::size_t>(b0)];
            const unsigned src =
                (sr >> 3) << 11 | (sg >> 2) << 5 | (sb >> 3);
            const unsigned dst =
                (dr >> 3) << 11 | (dg >> 2) << 5 | (db >> 3);
            const unsigned v = ((src & 0xF7DFu) + (dst & 0xF7DFu)) >> 1;
            const unsigned r5 = (v >> 11) & 31u;
            const unsigned g6 = (v >> 5) & 63u;
            const unsigned b5 = v & 31u;
            canvas[di + static_cast<std::size_t>(r0)] =
                static_cast<std::uint8_t>(r5 << 3 | r5 >> 2);
            canvas[di + 1] = static_cast<std::uint8_t>(g6 << 2 | g6 >> 4);
            canvas[di + static_cast<std::size_t>(b0)] =
                static_cast<std::uint8_t>(b5 << 3 | b5 >> 2);
            canvas[di + 3] = 255;
        }
    }
}

}  // namespace

const UnitGraphics* find_unit_graphics(const UnitCatalog& cat,
                                       const MapUnit& u) {
    return find_unit_gfx(&cat, u);
}

// See mapviz.hpp. `TUnitHS.Show@0x55784614` draws the walking pose from
// `seq[dir + 5]` (`aow::unit_sequence_index`), and `image_sequence_frame` frames
// it from that sequence's own frame table and `period + 1` spacing - so this is
// the pair `walk_settle_phase` needs to make the stride finish on the pose the
// standing pose follows.
bool unit_walk_cycle(const UnitGraphics& g, int direction, int& cycle_length,
                     int& spacing) {
    const int idx = unit_sequence_index(direction, /*moving=*/true);
    if (idx < 0 ||
        static_cast<std::size_t>(idx) >= g.sequences.size()) {
        return false;
    }
    const ImageSequence& seq = g.sequences[static_cast<std::size_t>(idx)];
    // Mode 4 only: the walking cycles are frame tables (`<d>01..<d>08`). A
    // static pose never advances and a mode-5 table re-randomises its entry
    // every `period + 1` ticks, so neither has a last pose to settle on.
    if (seq.empty() || (seq.mode & 0x7f) != 4 || seq.frame_table.empty()) {
        return false;
    }
    cycle_length = static_cast<int>(seq.frame_table.size());
    spacing = static_cast<int>(seq.period) + 1;
    return cycle_length > 1 && spacing > 0;
}

bool unit_walk_cycle(const UnitCatalog& cat, const MapUnit& unit, int direction,
                     int& cycle_length, int& spacing) {
    const UnitGraphics* g = find_unit_graphics(cat, unit);
    if (g == nullptr) return false;
    return unit_walk_cycle(*g, direction, cycle_length, spacing);
}

bool render_map_rgba(const Map& m, const std::vector<MapResource>& res,
                     const std::string& ilb_root, const MapVizOptions& opt,
                     std::vector<std::uint8_t>& rgba, std::uint32_t& out_w,
                     std::uint32_t& out_h, std::string& error,
                     std::vector<std::uint8_t>* contents) {
    rgba.clear();
    out_w = out_h = 0;
    if (contents != nullptr) contents->clear();
    if (opt.level < 0 || static_cast<std::size_t>(opt.level) >= m.levels.size()) {
        error = "level out of range";
        return false;
    }
    const MapLevel& lv = m.levels[static_cast<std::size_t>(opt.level)];
    if (lv.width == 0 || lv.height == 0) {
        error = "empty level";
        return false;
    }

    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    const std::map<std::uint32_t, std::size_t> frames = map_resource_frames(res);

    // The game's unit roster (Unitres.pfs + Unitgfx.pfs + Herores.pfs) maps a
    // placed army's lead unit to its sprite library. Shared process-wide
    // (`shared_unit_catalog`): loading it reads three .PFS files, and the live
    // screens re-render a level after every move - only maps that carry armies
    // ever ask for it, and a missing Release/ is non-fatal.
    auto ensure_units = [&]() -> const UnitCatalog* {
        return shared_unit_catalog(ilb_root);
    };
    // Resolves a placed unit's *standing* pose for its stored facing (TUnitHS rw
    // tag 9): the engine draws `seq[dir-1]` when the unit is not moving, so an
    // army that just walked keeps facing the way it arrived. A plain TUnit looks
    // its Unitres record up by id (== the unit's rw tag 10); a hero/leader (rw
    // tag 8) goes through the Herores table.
    auto unit_stand = [&](const MapUnit& u, int direction, const Library** out_lib,
                          std::size_t& out_slot, int& xoff, int& yoff) -> bool {
        const UnitPose p = resolve_unit_pose(ensure_units(), resolver, cache, u,
                                             direction, false, 0, 0);
        if (!p.ok) return false;
        *out_lib = p.lib;
        out_slot = p.slot;
        xoff = p.xoff;
        yoff = p.yoff;
        return true;
    };

    // The army's ground shadow (`TUnitHS.ShowShadow@0x55784504`): the same
    // resolution, out of the shadow slots 24..29. Most shipped shadows are static
    // (mode 0, a stored base frame), but the flyers (dragons, drakes, the eagle
    // rider, the wyvern, the bat, the goblin bomber, the demon, Astra, the
    // fairy) ship a 4- or 6-frame table, so the shadow is framed from the map
    // clock exactly like a building's overlay - `image_sequence_frame` with the
    // map clock and the per-hex phase. A unit that ships no shadow slot simply
    // casts none.
    auto unit_shadow = [&](const MapUnit& u, int direction, int clock,
                           int phase, const Library** out_lib,
                           std::size_t& out_slot, int& xoff, int& yoff) -> bool {
        const UnitPose p = resolve_unit_shadow(ensure_units(), resolver, cache, u,
                                               direction, clock, phase);
        if (!p.ok) return false;
        *out_lib = p.lib;
        out_slot = p.slot;
        xoff = p.xoff;
        yoff = p.yoff;
        return true;
    };

    // Phase 1: the base hexagon is 48x32; confirm that against the art so the
    // grid step and the fallback hexagon always match.
    int tile_w = 48, tile_h = 32;
    detect_tile_size(res, resolver, cache, tile_w, tile_h);
    // The engine's hex grid (HSEngine.HXtoHP@8E9B8701) is axis-aligned and
    // column-staggered - NOT a 45-degree isometric diamond. A 48x32 hexagon has
    // 45-degree flanks, so its column pitch is `tile_w - tile_h/2` (adjacent
    // columns overlap by half a tile height and interlock) and every odd column
    // is pushed down half a tile height to slot between its neighbours:
    //
    //   HXtoHP(x, y) = (x*0x20 + 8, y*0x20 + (x & 1)*0x10)   [hex -> pixels]
    //
    // HPtoHX is its exact inverse. The `+ 8` is the left margin for the leftmost
    // hexagon's flank; the sprite box's top-left is the engine's draw position.
    const int pitch_x = tile_w - tile_h / 2;  // 32  (HXtoHP: hexX * 0x20)
    const int pitch_y = tile_h;               // 32  (HXtoHP: hexY * 0x20)
    const int stagger = tile_h / 2;           // 16  (HXtoHP: (hexX & 1) * 0x10)
    const int field_x = 8;                    //     (HXtoHP: + 8)
    const int dw = field_x + pitch_x * (static_cast<int>(lv.width) - 1) + tile_w;
    const int dh = stagger + pitch_y * (static_cast<int>(lv.height) - 1) + tile_h;
    std::vector<std::uint8_t> canvas(static_cast<std::size_t>(dw) * dh * 4, 0);
    for (std::size_t i = 0; i < canvas.size(); i += 4) {
        canvas[i] = 12; canvas[i + 1] = 12; canvas[i + 2] = 20; canvas[i + 3] = 255;
    }
    // The split layer. `canvas` takes the terrain hexagons (and their coast
    // shelves / edge transitions); everything the sweep draws *over* them -
    // decorations, roads, bridges, cities and the armies - goes to this
    // transparent layer instead, so a caller can re-composite one hex in the
    // engine's own order (mapviz.hpp, `contents`).
    std::vector<std::uint8_t> contents_canvas;
    if (contents != nullptr) {
        contents_canvas.assign(static_cast<std::size_t>(dw) * dh * 4, 0);
    }
    // Objects and units go here; the terrain hexagons keep going to `canvas`.
    // A class-0x16 sprite (TSprite16) is *blended* (see blit_brighten); every
    // other class is alpha-composited (its RLE/alpha mask is exact).
    auto blit_blended = [&](std::vector<std::uint8_t>& dst, const Sprite& s,
                            int ox, int oy) {
        if (s.class_id == 0x16) {
            blit_brighten(dst, dw, dh, s, ox, oy);
        } else {
            blit(dst, dw, dh, s, ox, oy);
        }
    };
    auto blit_obj = [&](const Sprite& s, int ox, int oy) {
        blit_blended(contents == nullptr ? canvas : contents_canvas, s, ox, oy);
    };
    // The army banner is colour-keyed art: `TArmyHS.Show`'s +0x84 blit writes
    // the sprite's own palette values (the reference crop matches exactly), so
    // it must NOT go through the class-0x16 brighten path above.
    auto blit_banner = [&](const Sprite& s, int ox, int oy) {
        blit(contents == nullptr ? canvas : contents_canvas, dw, dh, s, ox, oy);
    };
    // The ground shadow of a unit: the engine's 50 % average (blit_shadow_buffer)
    // over whatever the sweep painted so far, in the same layer as the body.
    auto blit_shadow = [&](const Sprite& s, int ox, int oy) {
        blit_shadow_buffer(contents == nullptr ? canvas : contents_canvas, dw, dh,
                           s.rgba, static_cast<int>(s.w), static_cast<int>(s.h),
                           ox, oy, false);
    };

    // The owner flags are the owning player's *race* flags, so resolve the roster
    // (TPlayer rw tag 2) once; the race libraries are `race_ilbs` below (docs/19).
    const std::vector<std::uint8_t> player_races = map_player_races(m);

    // Map-space top-left corner of a hex's sprite box, straight from the engine:
    // HXtoHP(x, y) -> (x*0x20 + 8, y*0x20 + (x & 1)*0x10).
    auto origin = [&](std::uint32_t x, std::uint32_t y, int& sx, int& sy) {
        const int ix = static_cast<int>(x);
        sx = field_x + ix * pitch_x;
        sy = static_cast<int>(y) * pitch_y + (ix & 1) * stagger;
    };

    // The sprite the engine would draw for a resource id, plus the library and
    // frame index it came from (so the diagnostic CSV can name the entry).
    struct Picked {
        const Sprite* sprite = nullptr;
        const Library* lib = nullptr;
        std::size_t frame = 0;
        // ShowComplexImage draws a sequence at (anchor + seq xoff/yoff), where the
        // anchor is HXtoHP itself - the multi-hex dXmainP/dYmainP shift belongs to
        // the *simple* branch only.
        bool by_seq = false;
        int seq_dx = 0;
        int seq_dy = 0;
        // The body sequence's animation mode (0 = static). The object pass leaves
        // an animated body out when `no_animated_overlays` is set, so a live
        // front-end can composite it per map frame instead.
        std::uint8_t body_mode = 0;
        // The sequences the body's `.next` chain adds over it - what
        // ShowComplexImage draws after the body (a mine's sparkle, a wizard
        // tower's orb). Each carries its own xoff/yoff and mode.
        struct Layer {
            const Sprite* sprite = nullptr;
            const Library* lib = nullptr;
            std::size_t frame = 0;
            int dx = 0;
            int dy = 0;
            std::uint8_t mode = 0;
        };
        std::vector<Layer> layers;
        // Diagnostic (debug_csv): the library actually drawn (the resource's
        // sequence body when it has one, otherwise its primary name) and, for a
        // city, its race/faction byte.
        std::string ilb;
        std::uint32_t class_id = 0;
        int race = -1;
    };
    // `terrain` selects the base-hex frame inside a (possibly mixed) terrain
    // library; objects use the stored variant frame directly (see
    // map_resource_frames). Null sprite when the library cannot be located or
    // decoded.
    auto pick = [&](std::uint32_t resource, bool terrain,
                    int phase = 0) -> Picked {
        Picked out;
        const MapResource* r = find_map_resource(res, resource);
        if (r == nullptr) return out;
        out.class_id = r->class_id;
        // Prefer a library carried inline by the resource (rw tag 8): those are
        // the resources whose stored string is only a logical label ("gr_tree.BMP")
        // that names no file or ILB entry anywhere, so the engine cannot be loading
        // them from disk (TILManagerList.LinkToIL matches exact paths only).
        const Library* libp = nullptr;
        bool embedded = false;
        if (!r->ilb_data.empty()) {
            const Library* e = &get_library_embedded(
                cache, "@res:" + std::to_string(r->id), r->ilb_data);
            if (!e->frames.empty()) {
                libp = e;
                embedded = true;
                out.ilb = r->ilb;
            }
        }
        // An image-sequence list (rw tags 9/10) names the library AND the frame
        // explicitly (HSEngine.TMultiHexMO.Show -> TMapObjectResource.ShowComplexImage
        // -> TImageSequence.GetFrameIndex). This is authoritative, so it wins over
        // both the inline slot-0 rule and the shared-library `want` heuristic.
        // `body_sequence(r)` is the sequence the engine's chain shows as the
        // structure's BODY, not the first entry of the list: a Shipyard's list
        // opens with a `SMOKE.ILB` effect sequence chained in front of the dock
        // (`PRO_SYRD.ILB`), and the old "first name in the blob" scan made the
        // renderer pick the smoke and then drop the whole structure as an additive
        // effect. See aow/map_resources.hpp.
        const ImageSequence* body = nullptr;
        if (libp == nullptr && r->has_seq && !terrain) {
            // Terrain hexagons are drawn through Hexagon.THexagon.Show, not
            // ShowComplexImage: their tag-9 sequence list only names their own
            // library, so the terrain branch keeps using terrain_frame()/`want`
            // and must not take the sequence frame/anchor.
            body = body_sequence(*r);
            const std::string lib_name =
                body != nullptr ? body->library : r->seq_lib;
            const std::string file = resolver.resolve(lib_name);
            if (!file.empty()) {
                libp = &get_library(cache, file);
                out.seq_dx = r->seq_dx;
                out.seq_dy = r->seq_dy;
                out.by_seq = true;
                out.ilb = lib_name;
            }
        }
        const bool by_seq = out.by_seq;
        if (libp == nullptr) {  // no inline library (or it did not parse)
            const std::string file = resolver.resolve(r->ilb);
            if (file.empty()) return out;
            libp = &get_library(cache, file);
            out.ilb = r->ilb;
        }
        const Library& lib = *libp;
        if (lib.frames.empty()) return out;
        const auto it = frames.find(resource);
        const std::size_t want = (it == frames.end()) ? 0u : it->second;
        // A resource's own library is addressed as `Get(resource+0x20, 0)` by
        // HSEngine.TMultiHexMO.Show, i.e. its slot 0; a SHARED library is
        // understood as a slice starting at the resource's offset (`want`).
        out.frame = embedded ? 0u
                  : by_seq   ? r->seq_frame
                  : terrain  ? terrain_frame(lib, want)
                             : (want % lib.frames.size());
        // A sequence's frame is not always its stored base frame: a mode-1/2/4/5
        // sequence animates off the map clock (`AoWHSMap+0x40`) with the
        // per-object phase, exactly as ImageLib.TImageSequence.GetFrameIndex
        // resolves it. This is what makes a building animate.
        if (by_seq && body != nullptr) {
            out.body_mode = static_cast<std::uint8_t>(body->mode & 0x7f);
            const int f = image_sequence_frame(
                *body, static_cast<int>(lib.frames.size()), opt.clock, phase);
            if (f >= 0) out.frame = static_cast<std::size_t>(f);
        }
        // MOUNTAIN.ILB packs five terrain blocks of five size frames (idx 0..24:
        // Mnt_de, gr_mnt2, Mnt_sn, Mnt_st, wl_mnt) and then idx 25..29 - a single
        // 60x50 mound per terrain (`de_mnt`, `gr_mnt2`, `sn_mnt`, `st_mnt`,
        // `wl_mnt`) - which is the HILL sprite. When a mountain/hill resource does
        // NOT carry an inline library it is understood as a reference into that one
        // shared file, so pick the block's three *largest* frames (offsets 2/3/4 ->
        // 93x66 / 125x78 / 188x118) as before. This is an empirical fit - the exact
        // rule lives in TMapObjectResource.ShowComplexImage [TODO(RE)].
        //
        // A resource WITH an inline library must NOT be re-indexed here: its slot 0
        // already is the right sprite (engine: `Get(resource+0x20, 0)`), and the
        // block offsets would land on a different - e.g. a big mountain where the
        // map has a hill.
        if (!embedded && !by_seq && !terrain &&
            lower(fs::path(r->ilb).stem().string()) == "mountain") {
            const int base = mountain_block(r->res_type);
            if (base >= 0) {
                out.frame = static_cast<std::size_t>(base) + 2 + (want % 3);
            }
        }
        // The body's `.next` chain: ShowComplexImage draws every chained sequence
        // too, over the body. Those carry the *animated* part of a building - the
        // mine's sparkle (`STR_MINE.ILB` mode 5, frames 10..15), the wizard
        // tower's glowing orb (`STR_WTWR.ILB` mode 4, frames 10..21) - and each is
        // resolved from its own library, xoff/yoff and frame (the same phase).
        if (by_seq && body != nullptr) {
            bool first = true;
            for (const ImageSequence* s : image_sequence_chain(*body)) {
                if (first) {  // the body itself was drawn above
                    first = false;
                    continue;
                }
                if (s->library.empty() || is_effect_library(s->library)) {
                    continue;
                }
                const std::string lf = resolver.resolve(s->library);
                if (lf.empty()) continue;
                const Library& ll = get_library(cache, lf);
                if (ll.frames.empty()) continue;
                const int f = image_sequence_frame(
                    *s, static_cast<int>(ll.frames.size()), opt.clock, phase);
                if (f < 0 || static_cast<std::size_t>(f) >= ll.frames.size()) {
                    continue;
                }
                const Sprite& ls = ll.frames[static_cast<std::size_t>(f)];
                if (!ls.ok) continue;
                Picked::Layer layer;
                layer.sprite = &ls;
                layer.lib = &ll;
                layer.frame = static_cast<std::size_t>(f);
                layer.dx = s->xoff;
                layer.dy = s->yoff;
                layer.mode = static_cast<std::uint8_t>(s->mode & 0x7f);
                out.layers.push_back(layer);
            }
        }
        out.lib = &lib;
        if (out.frame >= lib.frames.size()) return out;  // guard: no such slot
        const Sprite& s = lib.frames[out.frame];
        if (s.ok) out.sprite = &s;
        return out;
    };

    // Phase 0: optional per-hex resolution dump (which entry each hex drew).
    std::ofstream csv;
    if (!opt.debug_csv.empty()) {
        csv.open(opt.debug_csv, std::ios::binary);
        if (csv) csv << "layer,x,y,resource,ilb,frame,entry,w,h,ok\n";
    }
    auto log_hex = [&](const char* layer, std::uint32_t x, std::uint32_t y,
                       std::uint32_t resource, const std::string& ilb,
                       const Picked& p) {
        if (!csv) return;
        const std::string entry =
            (p.lib != nullptr && p.frame < p.lib->names.size())
                ? p.lib->names[p.frame] : std::string();
        csv << layer << ',' << x << ',' << y << ',' << resource << ',' << ilb
            << ',' << p.frame << ',' << entry << ','
            << (p.sprite ? p.sprite->w : 0) << ','
            << (p.sprite ? p.sprite->h : 0) << ','
            << (p.sprite ? 1 : 0) << '\n';
    };

    // Terrain number of every hex's tag-50 resource, straight from its
    // TTerrainList (resource rw tag 7): 0 water, 1..5 land families, 6 ice,
    // 7/8 earth/rock, 9/10 lava/dirt, 11..13 upper families, 14 coast, 15 border.
    // Shared by the coastline pass (Phase 4) and the city pass (a city's body
    // block/slot depends on its terrain code).
    std::vector<int> terrain_of_hex(lv.tiles.size(), -1);
    for (std::size_t i = 0; i < lv.tiles.size(); ++i) {
        for (const MapHs& hs : lv.tiles[i].objects) {
            if (hs.tag != 50) continue;
            const MapResource* r = find_map_resource(res, hs.resource);
            if (r != nullptr && r->has_terrain) terrain_of_hex[i] = r->terrain;
            else if (r != nullptr) terrain_of_hex[i] = terrain_type_of(r->ilb);
            break;
        }
    }

    // Race image libraries (RACES\R*.ILB) indexed by faction. A city's rw tag
    // 0xd (MapHs::race) selects its row; see map_resources.hpp.
    const std::vector<std::string> race_ilbs = race_image_libraries(res);

    // The owner flag a "takeable" structure draws (docs/19_STRUCTURE_FLAGS.md):
    // a flagpole from Flags.ILB plus the owner's race flag, animated off the map
    // clock (HXtoHP.x is folded into the clock, so columns are out of phase). A
    // live front-end leaves it out of the cached level
    // (`opt.no_animated_overlays`) and composites it per map frame inside
    // draw_structure_overlays instead.
    auto blit_flag = [&](const MapHs& hs, const MapResource* r, std::uint32_t x,
                         std::uint32_t y, int sx, int sy) {
        if (opt.no_animated_overlays || !opt.objects) return;
        FlagSprites fs;
        if (!structure_flag_sprites(hs, r, sx, sy, opt.clock, player_races,
                                    race_ilbs, resolver, cache, fs)) {
            return;
        }
        const Sprite* parts[2] = {fs.pole, fs.flag};
        const int px[2] = {fs.pole_x, fs.flag_x};
        const int py[2] = {fs.pole_y, fs.flag_y};
        const Library* libs[2] = {fs.pole_lib, fs.flag_lib};
        const std::size_t frames[2] = {fs.pole_frame, fs.flag_frame};
        const std::string names[2] = {fs.pole_name, fs.flag_name};
        for (int i = 0; i < 2; ++i) {
            if (parts[i] == nullptr) continue;
            blit_obj(*parts[i], px[i], py[i]);
            Picked lp;
            lp.sprite = parts[i];
            lp.lib = libs[i];
            lp.frame = frames[i];
            lp.by_seq = true;
            log_hex("flag", x, y, hs.resource, names[i], lp);
        }
    };

    // Draw a City.TCity the way the engine does (AoWE.TCity.Show@5575f958 ->
    // AoWE.TCityResource.ShowCity@557ade3c). `cr` is the city's TCityResource
    // (CityN.ILB) and `tcode` the hex's terrain number. Returns the resource's
    // ILB name for the debug CSV.
    auto draw_city = [&](const MapHs& hs, const MapResource* cr, std::uint32_t x,
                         std::uint32_t y, int sx, int sy, int tcode) {
        if (tcode < 0) tcode = 1;  // unknown hex terrain -> surface
        const int size = city_resource_size(*cr);

        const std::string cfile = resolver.resolve(cr->ilb);
        const Library* clib =
            cfile.empty() ? nullptr : &get_library(cache, cfile);
        std::string race_name;

        // The race the body is drawn for: the city's own rw tag 0xd, or - when
        // the map left it unset - its owner's race (see city_body_race).
        const int race =
            city_body_race(hs.race, hs.structure_owner, player_races,
                           race_ilbs.size());

        // 1) race-specific body (TCity.UpdateImages -> TRaceResource.GetCityImage).
        const Library* body_lib = nullptr;
        std::size_t body_frame = 0;
        if (race >= 0 &&
            static_cast<std::size_t>(race) < race_ilbs.size() &&
            !race_ilbs[static_cast<std::size_t>(race)].empty()) {
            race_name = race_ilbs[static_cast<std::size_t>(race)];
            const std::string rf = resolver.resolve(race_name);
            if (!rf.empty()) {
                const Library& rl = get_library(cache, rf);
                const int wall_flag =
                    (hs.wall_type > 0 && hs.wall_type != 3) ? 1 : 0;
                std::size_t slot = city_image_slot(size, tcode);
                if (!(slot < rl.frames.size() && rl.frames[slot].ok)) {
                    slot = city_image_fallback_slot(size, wall_flag);
                }
                if (slot < rl.frames.size() && rl.frames[slot].ok) {
                    body_lib = &rl;
                    body_frame = slot;
                }
            }
        }
        // 2) otherwise the city resource's own terrain block: race >= 0 uses
        // block[T] (else slot 0), no race uses block[T+1] (else slot 1) - the
        // engine's `C_random` "?" placeholder, which now only an unowned /
        // unknown-owner city reaches.
        if (body_lib == nullptr && clib != nullptr && !clib->frames.empty()) {
            const std::size_t T =
                static_cast<std::size_t>(city_terrain_block(tcode));
            std::size_t slot = 0;
            if (race >= 0) {
                slot = (T < clib->frames.size() && clib->frames[T].ok) ? T : 0;
            } else {
                slot = (T + 1 < clib->frames.size() && clib->frames[T + 1].ok)
                           ? T + 1
                           : 1;
            }
            if (slot < clib->frames.size() && clib->frames[slot].ok) {
                body_lib = clib;
                body_frame = slot;
            }
        }

        // Walls (always from the resource library): block offset T + wall*2, with
        // the second sprite (+1) as the back layer. Wall types 0 and 3 = none.
        const Library* wtop_lib = nullptr;
        std::size_t wtop = 0;
        const Library* wbot_lib = nullptr;
        std::size_t wbot = 0;
        if (clib != nullptr && !clib->frames.empty() && hs.wall_type != 0 &&
            hs.wall_type != 3) {
            const std::size_t T =
                static_cast<std::size_t>(city_terrain_block(tcode));
            const std::size_t w = static_cast<std::size_t>(hs.wall_type) * 2;
            const std::size_t k = T + w;
            if (k + 1 < clib->frames.size() && clib->frames[k].ok &&
                clib->frames[k + 1].ok) {
                wbot_lib = clib; wbot = k;
                wtop_lib = clib; wtop = k + 1;
            } else if (w + 1 < clib->frames.size() && clib->frames[w].ok &&
                       clib->frames[w + 1].ok) {
                wbot_lib = clib; wbot = w;
                wtop_lib = clib; wtop = w + 1;
            }
        }

        // All layers share the anchor HXtoHP + resource dx_main/dy_main; each
        // sprite then adds its own base+shift (inside object_sprite_origin).
        auto layer = [&](const char* tag, const Library* lib, std::size_t frame,
                         const std::string& ilb_name) {
            Picked lp;
            lp.class_id = cr->class_id;
            lp.race = race;  // the body's race (own rw tag 0xd, else the owner's)
            if (lib != nullptr && frame < lib->frames.size()) {
                lp.lib = lib;
                lp.frame = frame;
                lp.sprite = &lib->frames[frame];
            }
            log_hex(tag, x, y, hs.resource, ilb_name, lp);
            if (lp.sprite == nullptr || !lp.sprite->ok) return;
            int ox = 0, oy = 0;
            object_sprite_origin(sx, sy, cr, lp.sprite->vx, lp.sprite->vy, ox,
                                 oy);
            blit_obj(*lp.sprite, ox, oy);
        };
        // ShowCity order: back wall (city+0x68), body (city+0x70), front wall
        // (city+0x6c).
        const std::string body_ilb = (body_lib == clib) ? cr->ilb : race_name;
        layer("city", wtop_lib, wtop, cr->ilb);
        layer("city", body_lib, body_frame, body_ilb);
        layer("city", wbot_lib, wbot, cr->ilb);
    };

    // Phase 1: guarantee full coverage. First lay a solid base hexagon under
    // *every* hex (a transparent or missing sprite must never expose the
    // near-black backdrop), then blit each hex's terrain sprite on top.
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const MapTile& t = lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
            int sx, sy;
            origin(x, y, sx, sy);
            std::uint8_t rgb[3] = {120, 120, 120};
            for (const MapHs& hs : t.objects) {
                if (hs.tag != 50) continue;
                const MapResource* r = find_map_resource(res, hs.resource);
                if (r != nullptr) terrain_color(r->ilb, rgb);
                break;
            }
            fill_hex(canvas, dw, dh, sx, sy, tile_w, tile_h, rgb);
        }
    }
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const MapTile& t = lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
            const MapHs* terr = nullptr;
            for (const MapHs& hs : t.objects) {
                if (hs.tag == 50) { terr = &hs; break; }
            }
            if (terr == nullptr) {
                log_hex("terrain", x, y, 0, std::string(), Picked{});
                continue;
            }
            int sx, sy;
            origin(x, y, sx, sy);
            Picked p = pick(terr->resource, true);
            // Phase 5: raised terrain (the whole underground level) is built from
            // IsoHex.TUpperIsometricHexagon hexes (class 0x20170), whose library is
            // EARTHHEX/ROCKHEX. IsoHex.TUpperIsometricHexagon.Show@55619e24 draws
            // the base at frame `(sprite+0x15 >> 6) + 0x28` (a RandInt(4) per-hex
            // variant -> EARTHHEX 0x28..0x2b = `hu_er.BMP`, the hexagon tops) and
            // the cliff edges at frames 0x1f/0x20/0x1e for mask bits 0/1/5.
            // terrain_frame() cannot find these (no `h_*` entry matches), so it fell
            // back to frame 0 (`tu_xx-er`, a cliff sprite) for every hex; select the
            // base variant here. The deterministic variant replaces RandInt(4).
            if (terr->class_id == kIsoUpperHexagonClassId && p.lib != nullptr) {
                const std::size_t v =
                    static_cast<std::size_t>((x * 5u + y * 11u) % 4u);
                const std::size_t fi = 0x28 + v;
                if (fi < p.lib->frames.size() && p.lib->frames[fi].ok) {
                    p.frame = fi;
                    p.sprite = &p.lib->frames[fi];
                }
            }
            // A WATER hexagon's base tile is one of the sixteen `H_Wa2` wave
            // frames WATERHEX.ILB carries at slots 0x8c..0x9b, chosen per hex by
            // AoWHex.TAoWWaterHexagon.Show@5579aeac:
            //   ((field>>3) + x + y*2 + (sprite+0x15>>6)) & 0xf + 0x8c
            // The engine's `field>>3` is the hexagon object's address and the
            // +0x15 phase a RandInt(4) from Create, so the port stands in a
            // positional hash for both (aow::water_base_frame) - which is what
            // turns the ocean into the original's field of wave phases instead of
            // one repeated tile. ICEHEX.ILB has no such run, so an ice tile keeps
            // the stored frame.
            if (terr->class_id == kWaterHexagonClassId && p.lib != nullptr) {
                const std::size_t wf = static_cast<std::size_t>(
                    water_base_frame(static_cast<int>(x), static_cast<int>(y)));
                if (wf < p.lib->frames.size() && p.lib->frames[wf].ok) {
                    p.frame = wf;
                    p.sprite = &p.lib->frames[wf];
                }
            }
            if (p.sprite != nullptr) blit(canvas, dw, dh, *p.sprite, sx, sy);
            const MapResource* r = find_map_resource(res, terr->resource);
            log_hex("terrain", x, y, terr->resource,
                    p.ilb.empty() && r != nullptr ? r->ilb : p.ilb, p);
        }
    }

    // Phase 4 (coastline): a water/ice hexagon draws its tan shoreline shelf toward
    // flat-land neighbours. Evidence: AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4 ->
    // IsoHex.TLowerIsometricHexagon.ShowDynamic@5561a400; the +0x14 mask is set by
    // AoWHex.TAoWWaterHexagon.UpdateTransitions@5579ad98. See water_shore_pieces.
    {
        // `terrain_of_hex` (computed above) is the terrain number of every hex.
        // dir 0..5 = N/NE/E/S/W/NW (HSEngine.HNtoHX + TranslateHX column stagger).
        const std::vector<int>& terr = terrain_of_hex;
        static const int dx6[6] = {0, 1, 1, 0, -1, -1};
        static const int dy6[6] = {-1, -1, 0, 1, 0, -1};
        std::vector<ShorePiece> pieces;
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                const std::size_t idx = static_cast<std::size_t>(y) * lv.width + x;
                const int own = terr[idx];
                if (own != 0 && own != 6) continue;  // water / ice only
                const MapResource* ownr = nullptr;
                for (const MapHs& hs : lv.tiles[idx].objects) {
                    if (hs.tag != 50) continue;
                    ownr = find_map_resource(res, hs.resource);
                    break;
                }
                if (ownr == nullptr) continue;
                // Two masks, both computed by UpdateTransitions@5579ad98. Its
                // nesting matters:
                //
                //   if (FindOwnedHS(nf, TLowerIsometricHexagon) == 0) {       // #
                //       if (FindOwnedHS(nf, TBorderHexagon) == 0) {
                //           sprite+0x14 |= 1 << dir;   // the SHELF (flat land)
                //           variant[dir] = (nf.terrain == 3) ? 0x28
                //                        : (nf.terrain == 5) ? 0x50 : 0;
                //       }                             // else: a border hexagon ->
                //   } else {                          //     NEITHER mask
                //       if (this->GetTerrain(0) != nf.terrain)
                //           sprite+0x15 |= 1 << dir;   // the FOAM
                //   }
                //
                // `TLowerIsometricHexagon` is this hexagon's own family (water and
                // ice - AoWHex.TAoWWaterHexagon derives from it), so the foam lands
                // exactly on a water/ice boundary (the `T_Ic-Wa2` art) and the
                // shelf on a flat-land one; a map-border neighbour gets nothing.
                int mask = 0;
                int foam = 0;
                for (int d = 0; d < 6; ++d) {
                    int nx = static_cast<int>(x) + dx6[d];
                    int ny = static_cast<int>(y) + dy6[d];
                    if ((x & 1u) != 0 && (dx6[d] & 1) != 0) ++ny;  // TranslateHX stagger
                    if (nx < 0 || ny < 0 || nx >= static_cast<int>(lv.width) ||
                        ny >= static_cast<int>(lv.height)) {
                        continue;
                    }
                    const int nt =
                        terr[static_cast<std::size_t>(ny) * lv.width + nx];
                    if (nt >= 1 && nt <= 5) {
                        // Flat land: the shoreline shelf.
                        mask |= 1 << d;
                    } else if ((nt == 0 || nt == 6) && nt != own) {
                        // The other half of the water/ice family, a different
                        // terrain: the foam.
                        foam |= 1 << d;
                    }
                }
                if (mask == 0 && foam == 0) continue;
                const std::string file = resolver.resolve(ownr->ilb);
                if (file.empty()) continue;
                const Library& lib = get_library(cache, file);
                if (lib.frames.empty()) continue;
                int sx, sy;
                origin(x, y, sx, sy);
                if (mask != 0) {
                    // The shelf's per-hex phase is the engine's `sprite >> 8 & 3`
                    // (3 -> 0), taken from the hexagon's address; positional here.
                    water_shore_pieces(
                        mask,
                        water_shore_variant(static_cast<int>(x),
                                            static_cast<int>(y)),
                        pieces);
                    // The engine blits these slots from two layers at the same
                    // anchor: IsoHex.TLowerIsometricHexagon.ShowDynamic (called
                    // from AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4) and then
                    // the water hexagon's own overlay, which adds the per-direction
                    // +0x1a..+0x1f variant (0x28 for a snow neighbour, 0x50 for
                    // wasteland, 0 otherwise) to select the `t_wa-sn` / `t_wa-wl`
                    // slot runs. The port draws the one layer those variants select;
                    // for the grass/desert/steppe families the variant is 0 and the
                    // two layers pick identical slots.
                    for (const ShorePiece& pc : pieces) {
                        if (pc.frame < 0 ||
                            static_cast<std::size_t>(pc.frame) >= lib.frames.size()) {
                            continue;
                        }
                        const Sprite& s = lib.frames[static_cast<std::size_t>(pc.frame)];
                        if (!s.ok) continue;
                        // Engine anchor: x = HXtoHP - 0x13, y = HXtoHP.y - 0x20 (or -0x1f).
                        blit(canvas, dw, dh, s, sx - 0x13 + s.vx, sy + pc.dy + s.vy);
                        Picked lg;
                        lg.lib = &lib;
                        lg.frame = static_cast<std::size_t>(pc.frame);
                        lg.sprite = &s;
                        log_hex("coast", x, y, ownr->id, ownr->ilb, lg);
                    }
                }
                // AoWHex.TAoWWaterHexagon.Show@5579aeac: after the base tile at
                // HXtoHP - 8, one `T_Ic-Wa2` foam sprite per mask bit 0/1/5, at
                // (-6,-9) from it - the water/ice boundary's foam.
                static const int foam_frames[3] = {kWaterFoamBit0Frame,
                                                   kWaterFoamBit1Frame,
                                                   kWaterFoamBit5Frame};
                static const int foam_bits[3] = {0x01, 0x02, 0x20};
                for (int k = 0; k < 3; ++k) {
                    if ((foam & foam_bits[k]) == 0) continue;
                    const std::size_t ff = static_cast<std::size_t>(foam_frames[k]);
                    if (ff >= lib.frames.size() || !lib.frames[ff].ok) continue;
                    const Sprite& s = lib.frames[ff];
                    blit(canvas, dw, dh, s, sx + kWaterFoamDx + s.vx,
                         sy + kWaterFoamDy + s.vy);
                    Picked lg;
                    lg.lib = &lib;
                    lg.frame = ff;
                    lg.sprite = &s;
                    log_hex("foam", x, y, ownr->id, ownr->ilb, lg);
                }
            }
        }
    }

    // Phase 4 (water animation): the water hexagons' own one-shot ripple/fish/
    // sparkle sequences (AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4). The
    // engine advances a counter on every hexagon once per rendered map frame, so
    // this belongs to the dynamic pass: `opt.clock` replays the rule from each
    // hexagon's Create up to that map frame (0 - the default - draws nothing,
    // which keeps the golden renders stable), and a live front-end leaves it out
    // of the cached level (no_animated_overlays) and calls draw_water_overlays
    // itself each frame instead. See docs/18_WATER_ANIM.md.
    if (!opt.no_animated_overlays) {
        int wx0 = 0, wy0 = 0, wx1 = 0, wy1 = 0;
        std::string werr;
        draw_water_overlays(m, res, ilb_root, opt.level, opt.clock, canvas, dw,
                            dh, /*bgra=*/false, wx0, wy0, wx1, wy1, werr,
                            opt.water);
    }

    // Phase 4: land terrain-transition overlays. Evidence:
    // re/exports/aowepack/AoWHex.TAoWHexagon.UpdateTransition.c. Per upper edge
    // (0,1,2 = NW, N, NE - HN {6,1,2}) the engine compares this hex's terrain
    // (`own`) with the neighbour's and, when both are plain land families, selects
    //   frame = own*16 + 6 + edge + variant*3          (variant in {0,1,2})
    // out of HexTrans.ILB (`resource+0x34`, linked by THexagonResource.MsgProc),
    // then Hexagon.THexagonResource.Show blits it at the base hex's (-6,-9). Only
    // the upper three edges are drawn because each hex sprite's upper edge is the
    // one that sits on top of its northern neighbour. The water/coast cases
    // (terrain 0/6/9/10/13) and the "long" variants are out of scope here.
    {
        const std::string htfile = resolver.resolve("HEXTRANS.ILB");
        const Library* htlib = htfile.empty() ? nullptr : &get_library(cache, htfile);
        if (htlib != nullptr && !htlib->frames.empty()) {
            // Upper-neighbour offsets by column parity (edge order NW, N, NE).
            const int ex[2][3] = {{-1, 0, 1}, {-1, 0, 1}};
            const int ey[2][3] = {{-1, -1, -1}, {0, -1, 0}};
            auto terr_res = [&](std::uint32_t x2, std::uint32_t y2) -> const MapResource* {
                const MapTile& t2 = lv.tiles[static_cast<std::size_t>(y2) * lv.width + x2];
                for (const MapHs& hs : t2.objects) {
                    if (hs.tag != 50) continue;
                    return find_map_resource(res, hs.resource);
                }
                return nullptr;
            };
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const MapResource* ownr = terr_res(x, y);
                    if (ownr == nullptr) continue;
                    const int own = terrain_type_of(ownr->ilb);
                    if (own < 1 || own > 5) continue;
                    int sx, sy;
                    origin(x, y, sx, sy);
                    const std::size_t par = x & 1u;
                    for (int e = 0; e < 3; ++e) {
                        const int nx = static_cast<int>(x) + ex[par][e];
                        const int ny = static_cast<int>(y) + ey[par][e];
                        if (nx < 0 || ny < 0 || nx >= static_cast<int>(lv.width) ||
                            ny >= static_cast<int>(lv.height)) {
                            continue;
                        }
                        const MapResource* nr = terr_res(static_cast<std::uint32_t>(nx),
                                                         static_cast<std::uint32_t>(ny));
                        if (nr == nullptr) continue;
                        const int nb = terrain_type_of(nr->ilb);
                        if (nb < 1 || nb > 5 || nb == own) continue;
                        // RandInt(3) in the engine; a positional hash keeps the
                        // render deterministic while still varying the variant.
                        const std::size_t variant = static_cast<std::size_t>(
                            (x * 7u + y * 13u + static_cast<std::uint32_t>(e) * 31u) % 3u);
                        const std::size_t fi =
                            static_cast<std::size_t>(own * 16 + 6 + e) + variant * 3;
                        if (fi >= htlib->frames.size()) continue;
                        const Sprite& s = htlib->frames[fi];
                        if (!s.ok) continue;
                        blit(canvas, dw, dh, s, sx - 6 + s.vx, sy - 9 + s.vy);
                        Picked lg;
                        lg.lib = htlib;
                        lg.frame = fi;
                        lg.sprite = &s;
                        log_hex("transition", x, y, ownr->id,
                                std::string("HEXTRANS.ILB"), lg);
                    }
                }
            }
        }
    }

    if (opt.objects) {
        // Phase 4b: roads. A scenario places road objects (engine class 0x20164,
        // Road.TRoad.ClassID@55616ecc) whose resource (class 0x20165,
        // Road.TRoadResource) carries its overlay library INLINE (rw tag 8,
        // entries `Rd_*.BMP` at image slots 1..8, 60x50). The engine recomputes a
        // per-road transition byte from the six neighbouring road objects
        // (Road.TRoad.UpdateTransitions@55616f8c) and draws the matching overlay
        // frames (Road.TRoadResource.Show@5561713c) at HXtoHP + (-14,-9). The
        // port had no name to resolve `Rd_gr.BMP` to, so roads were dropped; the
        // inline library + this pass restore them.
        {
            // A tile is "road-connected" when it carries a road, or a structure a
            // road may lead into (the engine's 0x1131 field query - see
            // road_connectable_object). A road's arms then point at up to six
            // connected neighbours, cities/buildings included.
            std::vector<std::uint8_t> connected(
                static_cast<std::size_t>(lv.width) * lv.height, 0);
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const MapTile& t =
                        lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                    for (const MapHs& hs : t.objects) {
                        if (hs.tag == 50) continue;  // terrain hexagon
                        if (hs.class_id != kRoadClassId &&
                            !road_connectable_object(hs.class_id)) {
                            continue;
                        }
                        connected[static_cast<std::size_t>(y) * lv.width + x] = 1;
                        break;
                    }
                }
            }
            std::vector<std::size_t> rframes;
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const MapTile& t =
                        lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                    const MapHs* road = nullptr;
                    for (const MapHs& hs : t.objects) {
                        if (hs.class_id != kRoadClassId) continue;
                        road = &hs;
                        break;
                    }
                    if (road == nullptr) continue;
                    const MapResource* r = find_map_resource(res, road->resource);
                    if (r == nullptr) continue;
                    // Prefer the inline `Rd_*.BMP` library (the same one pick()
                    // would use); fall back to the H_<Terrain>Dr.ILB alias.
                    const Library* lib = nullptr;
                    if (!r->ilb_data.empty()) {
                        const Library* e = &get_library_embedded(
                            cache, "@res:" + std::to_string(r->id), r->ilb_data);
                        if (!e->frames.empty()) lib = e;
                    }
                    if (lib == nullptr) {
                        const std::string file = resolver.resolve(r->ilb);
                        if (!file.empty()) {
                            const Library& l2 = get_library(cache, file);
                            if (!l2.frames.empty()) lib = &l2;
                        }
                    }
                    if (lib == nullptr) continue;
                    const std::uint8_t b = road_transition_byte(
                        x, y, lv.width, lv.height, connected);
                    road_frames(b, rframes);
                    int sx, sy;
                    origin(x, y, sx, sy);
                    for (std::size_t fi : rframes) {
                        if (fi >= lib->frames.size()) continue;
                        const Sprite& s = lib->frames[fi];
                        if (!s.ok) continue;
                        blit_obj(s, sx + kRoadAnchorDx + s.vx,
                                 sy + kRoadAnchorDy + s.vy);
                        Picked lg;
                        lg.lib = lib;
                        lg.frame = fi;
                        lg.sprite = &s;
                        log_hex("road", x, y, road->resource, r->ilb, lg);
                    }
                }
            }
        }
        // Phase 4d: bridges. A scenario stores only the bridge's resource id
        // (HxBridge.THexagonBridge, engine class 0x2087a); the orientation byte
        // (0 = N/S, 1 = NE/SW, 2 = SE/NW) is recomputed by the engine from the two
        // opposite land neighbours (UpdateTransitions@55799fd8 -> DirectionValid
        // @55799c04) and used as the library frame (`Show@55799a70` does
        // `Get(resource+0x20, direction)` at HXtoHP + (-0x17,-0x14)). See
        // aow/mapviz.hpp for the rule. The three-frame library (Str_brdg.bmp /
        // U_brdg.bmp) is carried INLINE, so `pick` already decodes it.
        {
            std::vector<std::uint8_t> land(
                static_cast<std::size_t>(lv.width) * lv.height, 0);
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const MapTile& t =
                        lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                    for (const MapHs& hs : t.objects) {
                        if (hs.tag != 50) continue;  // terrain hexagon
                        const MapResource* r =
                            find_map_resource(res, hs.resource);
                        if (r != nullptr && r->has_terrain &&
                            bridge_land(r->terrain)) {
                            land[static_cast<std::size_t>(y) * lv.width + x] = 1;
                        }
                        break;
                    }
                }
            }
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const MapTile& t =
                        lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                    const MapHs* bridge = nullptr;
                    for (const MapHs& hs : t.objects) {
                        if (hs.class_id != kHexBridgeClassId) continue;
                        bridge = &hs;
                        break;
                    }
                    if (bridge == nullptr) continue;
                    const std::uint8_t dir =
                        hex_bridge_direction(x, y, lv.width, lv.height, land);
                    if (dir == 0xff) continue;  // engine keeps the invalid 0xff
                    const Picked p = pick(bridge->resource, false);
                    if (p.lib == nullptr || dir >= p.lib->frames.size()) continue;
                    const Sprite& s = p.lib->frames[dir];
                    if (!s.ok) continue;
                    int sx, sy;
                    origin(x, y, sx, sy);
                    blit_obj(s, sx + kHexBridgeAnchorDx + s.vx,
                             sy + kHexBridgeAnchorDy + s.vy);
                    Picked lg;
                    lg.lib = p.lib;
                    lg.frame = dir;
                    lg.sprite = &s;
                    const MapResource* r =
                        find_map_resource(res, bridge->resource);
                    log_hex("bridge", x, y, bridge->resource,
                            r != nullptr ? r->ilb : std::string(), lg);
                }
            }
        }
        // Units (AoWE.TArmyHS) are collected during this object sweep and blitted
        // afterwards (see below), so they always sit on top of the terrain,
        // objects, roads and bridges. An object on a hex drawn later in the
        // row-major pass must never cover a unit. `sprite`/`lib` point into the
        // shared library cache, whose node-based std::map storage keeps the
        // references valid for the whole render.
        struct PendingUnit {
            const Sprite* sprite;
            int ox;
            int oy;
            std::uint32_t x;
            std::uint32_t y;
            std::uint32_t resource;
            const Library* lib;
            std::size_t slot;
            // The unit's ground shadow (`AoWE.TUnitShadowHS`, drawn one
            // show-priority layer below the body - see blit_shadow_buffer).
            // `shadow` stays null for a unit whose gfx record ships no shadow
            // sequence, which is exactly when the engine creates no shadow
            // object either.
            const Sprite* shadow = nullptr;
            int shadow_ox = 0;
            int shadow_oy = 0;
            const Library* shadow_lib = nullptr;
            std::size_t shadow_slot = 0;
            // The army's size banner (`TArmyHS.Show` draws it after the body).
            ArmyBanner banner{};
        };
        std::vector<PendingUnit> pending_units;
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                const MapTile& t = lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                int sx, sy;
                origin(x, y, sx, sy);
                for (const MapHs& hs : t.objects) {
                    if (hs.tag == 50) continue;
                    if (hs.class_id == kRoadClassId) continue;   // drawn above
                    if (hs.class_id == kHexBridgeClassId) continue;  // drawn above

                    // An army (AoWE.TArmyHS) draws its *lead* unit's sprite at the
                    // hex (TUnitHS.Show: HXtoHP + sequence offset + sprite base/
                    // shift). It carries no Release.hss map resource (its rw tag 1
                    // is not a TMapObjectResource id), so the generic object path
                    // below would drop it.
                    if (hs.class_id == kMapArmyHsClassId && !hs.units.empty()) {
                        // `no_units` leaves the armies (and their shadows) to
                        // `draw_unit_overlays`: a live front-end composites them
                        // per map frame, on top of everything the level carries.
                        if (opt.no_units) continue;
                        // The walk animation hides the army on the hex it is
                        // moving onto (the moving sprite stands in for it).
                        if (opt.hide_army_level == opt.level &&
                            opt.hide_army_x == static_cast<int>(x) &&
                            opt.hide_army_y == static_cast<int>(y)) {
                            continue;
                        }
                        const Library* ulib = nullptr;
                        std::size_t uslot = 0;
                        int uxoff = kUnitSeqDx, uyoff = kUnitSeqDy;
                        if (unit_stand(hs.units.front(),
                                       static_cast<int>(hs.army_direction),
                                       &ulib, uslot, uxoff, uyoff)) {
                            const Sprite& s = ulib->frames[uslot];
                            // Deferred: blitted after the whole object sweep so
                            // the unit is never covered by a later hex's object.
                            PendingUnit pu{&s, sx + uxoff + s.vx,
                                           sy + uyoff + s.vy, x, y,
                                           static_cast<std::uint32_t>(
                                               hs.units.front().resource),
                                           ulib, uslot};
                            // The army's ground shadow, at the very same anchor
                            // (the sequence's xoff/yoff + the sprite's
                            // base+shift) - TUnitHS.ShowShadow@0x55784504 draws
                            // the body's own footprint.
                            const Library* shlib = nullptr;
                            std::size_t shslot = 0;
                            int shxoff = kUnitSeqDx, shyoff = kUnitSeqDy;
                            if (unit_shadow(hs.units.front(),
                                            static_cast<int>(hs.army_direction),
                                            opt.clock,
                                            object_anim_phase(
                                                static_cast<int>(x),
                                                static_cast<int>(y)),
                                            &shlib, shslot, shxoff, shyoff)) {
                                const Sprite& sh = shlib->frames[shslot];
                                pu.shadow = &sh;
                                pu.shadow_ox = sx + shxoff + sh.vx;
                                pu.shadow_oy = sy + shyoff + sh.vy;
                                pu.shadow_lib = shlib;
                                pu.shadow_slot = shslot;
                            }
                            // The army size shield + leader crown, at HXtoHP (not
                            // the body's -16/-24 sequence cell): TArmyHS.Show.
                            resolve_army_banner(hs, player_races, race_ilbs,
                                                resolver, cache, sx, sy,
                                                pu.banner);
                            pending_units.push_back(pu);
                        }
                        continue;
                    }

                    const MapResource* r = find_map_resource(res, hs.resource);

                    // A city (City.TCity) is not a single sprite: draw it the way
                    // AoWE.TCity.Show -> AoWE.TCityResource.ShowCity does - the
                    // back wall layer (city+0x68), the body (city+0x70, race
                    // specific) and the front wall layer (city+0x6c), all at
                    // HXtoHP + resource dx_main/dy_main. Its hill/mound is the
                    // separate Pad.TPad object already blitted beneath it.
                    if (hs.class_id == kCityClassId && r != nullptr) {
                        // The engine's TCity.Show draws the owner's flag FIRST,
                        // then the walls/body on top of it - the pole is planted
                        // behind the city.
                        blit_flag(hs, r, x, y, sx, sy);
                        draw_city(hs, r, x, y, sx, sy,
                                  terrain_of_hex[static_cast<std::size_t>(y) *
                                                     lv.width + x]);
                        continue;
                    }

                    Picked p = pick(hs.resource, false,
                                    object_anim_phase(static_cast<int>(x),
                                                      static_cast<int>(y)));
                    p.race = hs.race;
                    log_hex("object", x, y, hs.resource,
                            p.ilb.empty() && r != nullptr ? r->ilb : p.ilb, p);
                    if (p.sprite == nullptr) continue;
                    // An object that resolved to a *body sequence* is a real
                    // structure (e.g. a Shipyard -> PRO_SYRD.ILB) even when the
                    // resource's first named library is an additive effect; only
                    // drop objects that really drew an effect library.
                    if (r != nullptr && !p.by_seq &&
                        is_transient_effect(
                            lower(fs::path(r->ilb).stem().string()))) {
                        continue;  // additive effect: no opaque blit
                    }
                    // Engine object anchor (re/exports/hsepack/
                    // HSEngine.TMapObject.Show.c + HSEngine.TMultiHexMO.Show.c): the
                    // sprite's visual box lands at `anchor + base + shift`, where the
                    // decoded base+shift is s.vx/s.vy and the anchor is
                    // HXtoHP - 8 (single-hex) or HXtoHP + dXmainP/dYmainP (multi-hex).
                    // See object_sprite_origin.
                    const Sprite& s = *p.sprite;
                    int ox = 0, oy = 0;
                    if (p.by_seq) {
                        // HSEngine.TMultiHexMO.Show's ShowComplexImage branch: the
                        // anchor is HXtoHP (WITHOUT the multi-hex dXmainP/dYmainP),
                        // then the sequence adds its own xoff/yoff, then the
                        // sprite adds base+shift; + kHexFieldMargin is our canvas
                        // margin, which cancels the -8 the engine applies to
                        // single-hex draws.
                        ox = sx + p.seq_dx + s.vx + kHexFieldMargin;
                        oy = sy + p.seq_dy + s.vy;
                    } else {
                        object_sprite_origin(sx, sy, r, s.vx, s.vy, ox, oy);
                    }
                    // TPlayerStructure.Show draws the owner's flag BEFORE the
                    // structure's own sprite, so the pole is planted behind the
                    // building (docs/19_STRUCTURE_FLAGS.md).
                    blit_flag(hs, r, x, y, sx, sy);
                    // A live front-end leaves the ANIMATED frames out (it
                    // composites them per map frame with draw_structure_overlays,
                    // which is cheaper than re-rendering the level).
                    if (!(opt.no_animated_overlays && p.body_mode != 0)) {
                        blit_obj(s, ox, oy);
                    }
                    // The body's chained sequences (ShowComplexImage draws them
                    // over the body): the animated part of a building.
                    for (const Picked::Layer& layer : p.layers) {
                        if (layer.sprite == nullptr) continue;
                        if (opt.no_animated_overlays && layer.mode != 0) continue;
                        const Sprite& ls = *layer.sprite;
                        const int lx = sx + layer.dx + ls.vx + kHexFieldMargin;
                        const int ly = sy + layer.dy + ls.vy;
                        blit_obj(ls, lx, ly);
                        Picked lp;
                        lp.lib = layer.lib;
                        lp.frame = layer.frame;
                        lp.sprite = &ls;
                        lp.by_seq = true;
                        log_hex("overlay", x, y, hs.resource, p.ilb, lp);
                    }
                }
            }
        }

        // Units last: composite every army's lead-unit sprite on top of the
        // terrain, objects, roads and bridges collected above. Row-major order
        // is preserved, so overlapping units still stack as before.
        //
        // The ground shadows come first: `TUnitShadowHS.GetShowPriority` returns
        // the unit's own priority with the layer replaced by `0x64000000`
        // (`(prio & 0x3ffffff) + 0x64000000`) against the unit's `0x68000000`,
        // so in the engine's sorted scene every shadow is drawn before every
        // body - two passes with the same row-major order, not one interleaved
        // pass. The blend is the engine's `TRLESprite16.ShowTransparent`
        // 50 % average (see blit_shadow_buffer), not the body's alpha blit.
        for (const PendingUnit& u : pending_units) {
            if (u.shadow == nullptr) continue;
            blit_shadow(*u.shadow, u.shadow_ox, u.shadow_oy);
            Picked lg;
            lg.lib = u.shadow_lib;
            lg.frame = u.shadow_slot;
            lg.sprite = u.shadow;
            log_hex("shadow", u.x, u.y, u.resource, std::string("UNITS"), lg);
        }
        for (const PendingUnit& u : pending_units) {
            blit_obj(*u.sprite, u.ox, u.oy);
            Picked lg;
            lg.lib = u.lib;
            lg.frame = u.slot;
            lg.sprite = u.sprite;
            log_hex("unit", u.x, u.y, u.resource, std::string("UNITS"), lg);
            // TArmyHS.Show: the size shield, then the leader crown, before the
            // next army's body (so banners stack with their own army).
            if (u.banner.shield != nullptr) {
                blit_banner(*u.banner.shield, u.banner.shield_x,
                            u.banner.shield_y);
                Picked lb;
                lb.lib = u.banner.shield_lib;
                lb.frame = u.banner.shield_slot;
                lb.sprite = u.banner.shield;
                log_hex("banner", u.x, u.y, u.resource,
                        std::string(u.banner.neutral ? "FLAGS" : "RACES"), lb);
            }
            if (u.banner.crown != nullptr) {
                blit_banner(*u.banner.crown, u.banner.crown_x,
                            u.banner.crown_y);
                Picked lc;
                lc.lib = u.banner.crown_lib;
                lc.frame = u.banner.crown_slot;
                lc.sprite = u.banner.crown;
                log_hex("crown", u.x, u.y, u.resource, std::string("FLAGS"),
                        lc);
            }
        }
    }

    // The downscale samples the full-res canvas; the split layer samples with the
    // same factor so the two stay in register.
    auto shrink = [&](std::vector<std::uint8_t>& src, int w, int h, int f,
                      std::vector<std::uint8_t>& out, int& ow, int& oh) {
        ow = w / f;
        oh = h / f;
        out.assign(static_cast<std::size_t>(ow) * oh * 4, 0);
        for (int y = 0; y < oh; ++y) {
            for (int x = 0; x < ow; ++x) {
                const std::size_t si =
                    (static_cast<std::size_t>(y * f) * w + x * f) * 4;
                const std::size_t di = (static_cast<std::size_t>(y) * ow + x) * 4;
                for (int k = 0; k < 4; ++k) out[di + k] = src[si + k];
            }
        }
    };
    if (opt.max_dim > 0 && dw > opt.max_dim) {
        const int f = (dw + opt.max_dim - 1) / opt.max_dim;
        int nw = 0, nh = 0;
        std::vector<std::uint8_t> small;
        shrink(canvas, dw, dh, f, small, nw, nh);
        rgba.swap(small);
        out_w = static_cast<std::uint32_t>(nw);
        out_h = static_cast<std::uint32_t>(nh);
        if (contents != nullptr) {
            int cw2 = 0, ch2 = 0;
            std::vector<std::uint8_t> csmall;
            shrink(contents_canvas, dw, dh, f, csmall, cw2, ch2);
            contents->swap(csmall);
        }
        return true;
    }
    rgba.swap(canvas);
    out_w = static_cast<std::uint32_t>(dw);
    out_h = static_cast<std::uint32_t>(dh);
    if (contents != nullptr) contents->swap(contents_canvas);
    return true;
}

bool render_map_png(const Map& m, const std::vector<MapResource>& res,
                    const std::string& ilb_root, const MapVizOptions& opt,
                    const std::string& out_path, std::string& error) {
    std::vector<std::uint8_t> rgba;
    std::uint32_t w = 0, h = 0;
    if (!render_map_rgba(m, res, ilb_root, opt, rgba, w, h, error)) return false;
    return write_png_rgba(out_path, w, h, rgba, error);
}

// ---------------------------------------------------------------------------
// Movement overlay (aow/mapviz.hpp)
// ---------------------------------------------------------------------------
void hex_origin(int x, int y, int& sx, int& sy) {
    // HSEngine.HXtoHP@5560e3b4: `px = x*0x20 + 8`, `py = y*0x20 + (x & 1)*0x10`,
    // which is exactly the render loop's hex origin.
    sx = kHexFieldMargin + x * kHexPitchX;
    sy = y * kHexPitchY + (x & 1) * kHexStagger;
}

namespace {

// Alpha-composites `s` into `rgba`, anchored at `hex_origin(hex) - 8 + base+shift`
// - the engine's ShowMovePaths blit (`HXtoHP - camera + (-8,-8)` plus the image's
// own base+shift).
void blit_overlay_sprite(std::vector<std::uint8_t>& rgba, int w, int h,
                         const Sprite& s, int hex_x, int hex_y, bool bgra,
                         int scale) {
    if (!s.ok) return;
    if (scale < 1) scale = 1;
    int hx = 0, hy = 0;
    hex_origin(hex_x, hex_y, hx, hy);
    // The engine's anchor is `HXtoHP - 8 + base+shift`; `scale` grows the sprite
    // around that point (the viewer pre-scales the UI overlay so it keeps its
    // 1:1 on-screen size when the map itself is zoomed out).
    const int ox = hx - 8 + s.vx -
                   (scale - 1) * static_cast<int>(s.w) / 2;
    const int oy = hy - 8 + s.vy -
                   (scale - 1) * static_cast<int>(s.h) / 2;
    // Channel map: the sprite is RGBA; the canvas may be BGRA (GDI order).
    const int c0 = bgra ? 2 : 0;
    const int c2 = bgra ? 0 : 2;
    for (std::uint32_t y = 0; y < s.h; ++y) {
        for (std::uint32_t x = 0; x < s.w; ++x) {
            const std::size_t si = (static_cast<std::size_t>(y) * s.w + x) * 4;
            const unsigned a = s.rgba[si + 3];
            if (a == 0) continue;
            for (int sy = 0; sy < scale; ++sy) {
                const int dy = oy + static_cast<int>(y) * scale + sy;
                if (dy < 0 || dy >= h) continue;
                for (int sx = 0; sx < scale; ++sx) {
                    const int dx = ox + static_cast<int>(x) * scale + sx;
                    if (dx < 0 || dx >= w) continue;
                    const std::size_t di =
                        (static_cast<std::size_t>(dy) *
                             static_cast<std::size_t>(w) +
                         static_cast<std::size_t>(dx)) * 4;
                    if (a == 255) {
                        rgba[di + static_cast<std::size_t>(c0)] = s.rgba[si];
                        rgba[di + 1] = s.rgba[si + 1];
                        rgba[di + static_cast<std::size_t>(c2)] = s.rgba[si + 2];
                        rgba[di + 3] = 255;
                    } else {
                        const unsigned inv = 255u - a;
                        for (int c = 0; c < 3; ++c) {
                            const int sc = (c == 0) ? c0 : (c == 2 ? c2 : 1);
                            rgba[di + static_cast<std::size_t>(c)] =
                                static_cast<std::uint8_t>(
                                    (s.rgba[si + static_cast<std::size_t>(sc)] *
                                         a +
                                     rgba[di + static_cast<std::size_t>(c)] *
                                         inv) /
                                    255);
                        }
                        rgba[di + 3] = 255;
                    }
                }
            }
        }
    }
}

}  // namespace

// The overlay is redrawn on every hover step, so it must not rebuild anything:
// the shared library cache and resolver (above) already hold `Cursors.ilb` and
// the game-root index once the level has been rendered.
// Loads (and caches) the overlay sprites so the first trail does not pay for
// them. The engine links Cursors.ilb at start-up anyway
// (TAoWWHSSet.Create -> TILManagerList.LinkToIL(manager, 1, "CURSORS.ILB")), so
// warming it when a level is loaded is what the game itself does. The very first
// un-warmed overlay cost ~50 ms; warm, it is ~0.3 ms.
bool prepare_move_overlay(const std::string& ilb_root, std::string& error) {
    error.clear();
    const std::string file = shared_resolver(ilb_root).resolve("CURSORS.ILB");
    if (file.empty()) {
        error = "Cursors.ilb not found under " + ilb_root;
        return false;
    }
    get_library(shared_libraries(), file);
    return true;
}

bool draw_move_path_overlay(std::vector<std::uint8_t>& rgba, int w, int h,
                            const std::string& ilb_root, const MovePath& path,
                            int budget, int per_turn, std::string& error,
                            bool bgra, int sprite_scale) {
    error.clear();
    if (path.empty()) return true;  // CalculatePathEx cleared the list
    const std::string file = shared_resolver(ilb_root).resolve("CURSORS.ILB");
    if (file.empty()) {
        error = "Cursors.ilb not found under " + ilb_root;
        return false;
    }
    const Library& lib = get_library(shared_libraries(), file);
    auto frame_of = [&](int frame) -> const Sprite* {
        if (frame < 0) return nullptr;
        const std::size_t i = static_cast<std::size_t>(frame);
        if (i >= lib.frames.size()) return nullptr;
        return lib.frames[i].ok ? &lib.frames[i] : nullptr;
    };

    // One arrow per step. `ShowMovePaths` blits the arrow for the step
    // `list[i] -> list[i+1]` AT `list[i]`, and its list is DESTINATION-first, so
    // the trail lands on every hex EXCEPT the source - the destination included
    // (the X is then drawn on top of it). Our `steps` are SOURCE-first, so the
    // hex to draw for the step `steps[i-1] -> steps[i]` is `steps[i]`: start at
    // index 1 (skip the hero's hex) and never leave the trail one hex short of
    // the destination.
    for (std::size_t i = 1; i < path.steps.size(); ++i) {
        const int dir = path_arrow_dir(path, i - 1);
        if (dir == 0) continue;
        // ShowMovePaths keeps a running `local_34` that starts at the path total
        // and subtracts each entry's stored cost; because an entry stores
        // `mp(pos[i]) - mp(pos[i+1])`, `local_34` is exactly `mp(pos[i])` - the
        // accumulated move points from the source to pos[i]. The arrow is bright
        // while `budget >= local_34`, i.e. while this hex is reachable THIS turn,
        // and dim beyond it. (Our `steps` are source-first, so `steps[i].cost` IS
        // that accumulated cost.)
        const Sprite* s = frame_of(path_arrow_frame(dir, budget >= path.steps[i].cost));
        if (s != nullptr) {
            blit_overlay_sprite(rgba, w, h, *s, path.steps[i].x, path.steps[i].y,
                                bgra, sprite_scale);
        }
    }
    // The destination X: bright when the whole path fits this turn.
    if (const Sprite* s = frame_of(path_target_frame(budget >= path.total_cost))) {
        const MoveStep& d = path.steps.back();
        blit_overlay_sprite(rgba, w, h, *s, d.x, d.y, bgra, sprite_scale);
    }
    // When the path does not fit this turn, ShowMovePaths@0x55763f80 also marks
    // the hex where this turn's move points run out with a second, bright X:
    // `pos = TMovepath.MovePointsToPos(path, budget)` while `0 < pos < count-1`.
    // It supersedes that hex's (dim) arrow.
    const int stop = path_budget_stop_index(path, budget);
    if (stop >= 1 && stop < static_cast<int>(path.steps.size()) - 1) {
        if (const Sprite* s = frame_of(kTargetFrameBright)) {
            const MoveStep& m = path.steps[static_cast<std::size_t>(stop)];
            blit_overlay_sprite(rgba, w, h, *s, m.x, m.y, bgra, sprite_scale);
        }
    }
    (void)per_turn;
    return true;
}

void blit_sprite_bgra(std::vector<std::uint8_t>& bgra, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy) {
    if (sw <= 0 || sh <= 0 || sprite.size() <
            static_cast<std::size_t>(sw) * static_cast<std::size_t>(sh) * 4) {
        return;
    }
    for (int y = 0; y < sh; ++y) {
        const int dy = oy + y;
        if (dy < 0 || dy >= ch) continue;
        for (int x = 0; x < sw; ++x) {
            const int dx = ox + x;
            if (dx < 0 || dx >= cw) continue;
            const std::size_t si =
                (static_cast<std::size_t>(y) * static_cast<std::size_t>(sw) +
                 static_cast<std::size_t>(x)) * 4;
            const unsigned a = sprite[si + 3];
            if (a == 0) continue;
            const std::size_t di =
                (static_cast<std::size_t>(dy) * static_cast<std::size_t>(cw) +
                 static_cast<std::size_t>(dx)) * 4;
            if (a == 255) {
                bgra[di] = sprite[si + 2];      // B
                bgra[di + 1] = sprite[si + 1];  // G
                bgra[di + 2] = sprite[si];      // R
            } else {
                const unsigned inv = 255u - a;
                bgra[di] = static_cast<std::uint8_t>((sprite[si + 2] * a +
                                                      bgra[di] * inv) / 255);
                bgra[di + 1] = static_cast<std::uint8_t>(
                    (sprite[si + 1] * a + bgra[di + 1] * inv) / 255);
                bgra[di + 2] = static_cast<std::uint8_t>(
                    (sprite[si] * a + bgra[di + 2] * inv) / 255);
            }
            bgra[di + 3] = 0;
        }
    }
}

// Alpha-composites `sprite` (8-bit RGBA) into an RGBA canvas: the same rule as
// blit_sprite_bgra with the byte order the map renderer emits (R,G,B,A).
namespace {
void blit_sprite_buffer(std::vector<std::uint8_t>& canvas, int cw, int ch,
                        const std::vector<std::uint8_t>& sprite, int sw, int sh,
                        int ox, int oy, bool bgra) {
    if (sw <= 0 || sh <= 0 ||
        sprite.size() < static_cast<std::size_t>(sw) * static_cast<std::size_t>(sh) * 4) {
        return;
    }
    const int r0 = bgra ? 2 : 0;
    const int b0 = bgra ? 0 : 2;
    for (int y = 0; y < sh; ++y) {
        const int dy = oy + y;
        if (dy < 0 || dy >= ch) continue;
        for (int x = 0; x < sw; ++x) {
            const int dx = ox + x;
            if (dx < 0 || dx >= cw) continue;
            const std::size_t si =
                (static_cast<std::size_t>(y) * static_cast<std::size_t>(sw) +
                 static_cast<std::size_t>(x)) * 4;
            const unsigned a = sprite[si + 3];
            if (a == 0) continue;
            const std::size_t di =
                (static_cast<std::size_t>(dy) * static_cast<std::size_t>(cw) +
                 static_cast<std::size_t>(dx)) * 4;
            if (a == 255) {
                canvas[di] = sprite[si + r0];
                canvas[di + 1] = sprite[si + 1];
                canvas[di + 2] = sprite[si + b0];
            } else {
                const unsigned inv = 255u - a;
                canvas[di] = static_cast<std::uint8_t>(
                    (sprite[si + r0] * a + canvas[di] * inv) / 255);
                canvas[di + 1] = static_cast<std::uint8_t>(
                    (sprite[si + 1] * a + canvas[di + 1] * inv) / 255);
                canvas[di + 2] = static_cast<std::uint8_t>(
                    (sprite[si + b0] * a + canvas[di + 2] * inv) / 255);
            }
            canvas[di + 3] = 0;
        }
    }
}
}  // namespace

// The brighten variant of `blit_sprite_buffer`: a class-0x16 (TSprite16)
// sequence overlay is composited with the GFXE brighten table - `dst =
// max(dst, src)` per channel - exactly like `blit_brighten` above. `sprite` is
// RGBA; `bgra` selects the canvas byte order.
namespace {
void blit_sprite_brighten_buffer(std::vector<std::uint8_t>& canvas, int cw,
                                 int ch,
                                 const std::vector<std::uint8_t>& sprite, int sw,
                                 int sh, int ox, int oy, bool bgra) {
    if (sw <= 0 || sh <= 0 ||
        sprite.size() < static_cast<std::size_t>(sw) * static_cast<std::size_t>(sh) * 4) {
        return;
    }
    const int r0 = bgra ? 2 : 0;
    const int b0 = bgra ? 0 : 2;
    for (int y = 0; y < sh; ++y) {
        const int dy = oy + y;
        if (dy < 0 || dy >= ch) continue;
        for (int x = 0; x < sw; ++x) {
            const int dx = ox + x;
            if (dx < 0 || dx >= cw) continue;
            const std::size_t si =
                (static_cast<std::size_t>(y) * static_cast<std::size_t>(sw) +
                 static_cast<std::size_t>(x)) * 4;
            if (sprite[si + 3] == 0) continue;
            const std::size_t di =
                (static_cast<std::size_t>(dy) * static_cast<std::size_t>(cw) +
                 static_cast<std::size_t>(dx)) * 4;
            for (int k = 0; k < 3; ++k) {
                const int s_k = sprite[si + (k == 0 ? r0 : (k == 1 ? 1 : b0))];
                const std::size_t d_k = di + static_cast<std::size_t>(k);
                if (s_k > canvas[d_k]) canvas[d_k] = static_cast<std::uint8_t>(s_k);
            }
            canvas[di + 3] = 0;
        }
    }
}
}  // namespace

void blit_sprite_rgba(std::vector<std::uint8_t>& rgba, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy) {
    blit_sprite_buffer(rgba, cw, ch, sprite, sw, sh, ox, oy, false);
}

void blit_shadow_bgra(std::vector<std::uint8_t>& bgra, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy) {
    blit_shadow_buffer(bgra, cw, ch, sprite, sw, sh, ox, oy, true);
}

void blit_shadow_rgba(std::vector<std::uint8_t>& rgba, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy) {
    blit_shadow_buffer(rgba, cw, ch, sprite, sw, sh, ox, oy, false);
}

bool army_sprite_rgba(const Map& m, const std::vector<MapResource>& res,
                      const std::string& ilb_root, std::size_t level,
                      std::uint32_t x, std::uint32_t y,
                      std::vector<std::uint8_t>& rgba, int& out_w, int& out_h,
                      int& vx, int& vy, std::string& error) {
    rgba.clear();
    out_w = out_h = 0;
    vx = vy = 0;
    if (level >= m.levels.size()) {
        error = "level out of range";
        return false;
    }
    const MapLevel& lv = m.levels[level];
    if (y >= lv.height || x >= lv.width) {
        error = "hex out of range";
        return false;
    }
    const MapTile& tile = lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
    const MapHs* army = nullptr;
    for (const MapHs& hs : tile.objects) {
        if (hs.class_id == kMapArmyHsClassId && !hs.units.empty()) {
            army = &hs;
            break;
        }
    }
    if (army == nullptr) {
        error = "no army on this hex";
        return false;
    }
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    UnitCatalog cat;
    std::string uerr;
    if (!load_unit_catalog(ilb_root, cat, uerr) || cat.units.empty()) {
        error = uerr.empty() ? "unit catalog unavailable" : uerr;
        return false;
    }
    // The army's stored facing (TUnitHS rw tag 9) selects its standing pose, so
    // an army that just walked keeps facing the way it arrived.
    const UnitPose pose =
        resolve_unit_pose(&cat, resolver, cache, army->units.front(),
                          static_cast<int>(army->army_direction), false, 0, 0);
    if (!pose.ok) {
        error = "unit sprite did not resolve";
        return false;
    }
    const Sprite& s = pose.lib->frames[pose.slot];
    if (!s.ok) {
        error = "unit sprite did not decode";
        return false;
    }
    rgba = s.rgba;
    out_w = static_cast<int>(s.w);
    out_h = static_cast<int>(s.h);
    vx = pose.xoff + s.vx;
    vy = pose.yoff + s.vy;
    (void)res;
    return true;
}

bool army_banner_rgba(const Map& m, const MapHs& army,
                      const std::string& ilb_root, int sx, int sy,
                      std::vector<std::uint8_t>& rgba, int& out_w, int& out_h,
                      int& vx, int& vy, std::string& error) {
    rgba.clear();
    out_w = out_h = 0;
    vx = vy = 0;
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    const std::vector<std::uint8_t> player_races = map_player_races(m);
    const std::vector<std::string> race_libraries;
    ArmyBanner banner;
    if (!resolve_army_banner(army, player_races, race_libraries, resolver,
                             cache, sx, sy, banner)) {
        error = "army banner did not resolve";
        return false;
    }
    // The union of the two parts, relative to the hex anchor. The shield's own
    // base+shift is already folded into `shield_x/y`; undo `sx/sy` to get the
    // relative placement inside the composite.
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    bool have = false;
    auto add_box = [&](const Sprite* s, int ox, int oy) {
        const int rx = ox - sx;
        const int ry = oy - sy;
        if (!have) {
            x0 = rx;
            y0 = ry;
            x1 = rx + static_cast<int>(s->w);
            y1 = ry + static_cast<int>(s->h);
            have = true;
        } else {
            x0 = std::min(x0, rx);
            y0 = std::min(y0, ry);
            x1 = std::max(x1, rx + static_cast<int>(s->w));
            y1 = std::max(y1, ry + static_cast<int>(s->h));
        }
    };
    if (banner.shield != nullptr) {
        add_box(banner.shield, banner.shield_x, banner.shield_y);
    }
    if (banner.crown != nullptr) {
        add_box(banner.crown, banner.crown_x, banner.crown_y);
    }
    out_w = x1 - x0;
    out_h = y1 - y0;
    vx = x0;
    vy = y0;
    if (out_w <= 0 || out_h <= 0) {
        error = "empty army banner";
        return false;
    }
    rgba.assign(static_cast<std::size_t>(out_w) *
                    static_cast<std::size_t>(out_h) * 4,
                0);
    // Straight source-over: the art is colour-keyed (alpha 0 or 255), and the
    // order is TArmyHS.Show's - shield first, then the crown above it.
    auto composite = [&](const Sprite* s, int ox, int oy) {
        if (s == nullptr) return;
        const int rx = ox - sx - x0;
        const int ry = oy - sy - y0;
        for (std::uint32_t y = 0; y < s->h; ++y) {
            for (std::uint32_t x = 0; x < s->w; ++x) {
                const std::size_t si =
                    (static_cast<std::size_t>(y) * s->w + x) * 4;
                const std::uint8_t a = s->rgba[si + 3];
                if (a == 0) continue;
                const std::size_t di =
                    (static_cast<std::size_t>(ry + y) *
                         static_cast<std::size_t>(out_w) +
                     static_cast<std::size_t>(rx + x)) *
                    4;
                if (a == 255) {
                    rgba[di] = s->rgba[si];
                    rgba[di + 1] = s->rgba[si + 1];
                    rgba[di + 2] = s->rgba[si + 2];
                } else {
                    const int inv = 255 - a;
                    for (int c = 0; c < 3; ++c) {
                        rgba[di + c] = static_cast<std::uint8_t>(
                            (s->rgba[si + c] * a + rgba[di + c] * inv) / 255);
                    }
                }
                rgba[di + 3] = 255;
            }
        }
    };
    composite(banner.shield, banner.shield_x, banner.shield_y);
    composite(banner.crown, banner.crown_x, banner.crown_y);
    return true;
}

bool unit_pose_rgba(const UnitCatalog& cat, const std::string& ilb_root,
                    const MapUnit& unit, int direction, bool moving,
                    int clock, int phase, std::vector<std::uint8_t>& rgba,
                    int& out_w, int& out_h, int& vx, int& vy,
                    std::string& error) {
    rgba.clear();
    out_w = out_h = 0;
    vx = vy = 0;
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    const UnitPose pose =
        resolve_unit_pose(&cat, resolver, cache, unit, direction, moving, clock,
                          phase);
    if (!pose.ok) {
        error = "unit pose did not resolve";
        return false;
    }
    const Sprite& spr = pose.lib->frames[pose.slot];
    if (!spr.ok) {
        error = "unit pose did not decode";
        return false;
    }
    rgba = spr.rgba;
    out_w = static_cast<int>(spr.w);
    out_h = static_cast<int>(spr.h);
    vx = pose.xoff + spr.vx;
    vy = pose.yoff + spr.vy;
    return true;
}

// See mapviz.hpp. The engine swaps the walking sprite for the standing one in a
// single frame; a front-end can instead draw the two cross-faded over the last
// few animation frames of the move, at one anchor and one facing, so the unit
// settles into the idle sprite. Both poses are resolved exactly the way
// `unit_pose_rgba` / the level render place them: a sprite's visual box lands at
// `HXtoHP(hex) + seq.xoff/yoff + base+shift`.
bool unit_settle_pose_rgba(const UnitCatalog& cat, const std::string& ilb_root,
                           const MapUnit& unit, int direction, int clock,
                           int phase, double settle,
                           std::vector<std::uint8_t>& rgba, int& out_w,
                           int& out_h, int& vx, int& vy, std::string& error) {
    rgba.clear();
    out_w = out_h = 0;
    vx = vy = 0;
    // The ends of the ramp are the two poses the front-end would draw anyway.
    if (settle <= 0.0) {
        return unit_pose_rgba(cat, ilb_root, unit, direction, true, clock, phase,
                              rgba, out_w, out_h, vx, vy, error);
    }
    if (settle >= 1.0) {
        return unit_pose_rgba(cat, ilb_root, unit, direction, false, 0, 0, rgba,
                              out_w, out_h, vx, vy, error);
    }
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    const UnitPose walk = resolve_unit_pose(&cat, resolver, cache, unit,
                                            direction, true, clock, phase);
    if (!walk.ok || walk.slot >= walk.lib->frames.size() ||
        !walk.lib->frames[walk.slot].ok) {
        error = "unit walk pose did not resolve";
        return false;
    }
    const UnitPose stand = resolve_unit_pose(&cat, resolver, cache, unit,
                                             direction, false, 0, 0);
    if (!stand.ok || stand.slot >= stand.lib->frames.size() ||
        !stand.lib->frames[stand.slot].ok) {
        error = "unit standing pose did not resolve";
        return false;
    }
    const Sprite& w = walk.lib->frames[walk.slot];
    const Sprite& s = stand.lib->frames[stand.slot];
    const int wvx = walk.xoff + w.vx;
    const int wvy = walk.yoff + w.vy;
    const int svx = stand.xoff + s.vx;
    const int svy = stand.yoff + s.vy;
    const int x0 = std::min(wvx, svx);
    const int y0 = std::min(wvy, svy);
    const int x1 = std::max(wvx + static_cast<int>(w.w),
                            svx + static_cast<int>(s.w));
    const int y1 = std::max(wvy + static_cast<int>(w.h),
                            svy + static_cast<int>(s.h));
    out_w = x1 - x0;
    out_h = y1 - y0;
    vx = x0;
    vy = y0;
    if (out_w <= 0 || out_h <= 0) {
        error = "empty settle sprite";
        return false;
    }
    rgba.assign(static_cast<std::size_t>(out_w) * static_cast<std::size_t>(out_h) *
                    4,
                0);
    // Premultiplied accumulation: the colours are averaged by the weight times
    // each sprite's own alpha (the unit libraries' colour key is decoded to
    // alpha), then un-premultiplied once at the end.
    std::vector<double> acc(rgba.size(), 0.0);
    auto add = [&](const Sprite& spr, int ox, int oy, double weight) {
        for (std::uint32_t y = 0; y < spr.h; ++y) {
            for (std::uint32_t x = 0; x < spr.w; ++x) {
                const std::size_t si =
                    (static_cast<std::size_t>(y) * spr.w + x) * 4;
                const double a = spr.rgba[si + 3] / 255.0 * weight;
                if (a <= 0.0) continue;
                const std::size_t di =
                    (static_cast<std::size_t>(oy + y) *
                         static_cast<std::size_t>(out_w) +
                     static_cast<std::size_t>(ox + x)) *
                    4;
                for (int c = 0; c < 3; ++c) {
                    acc[di + c] += spr.rgba[si + c] / 255.0 * a;
                }
                acc[di + 3] += a;
            }
        }
    };
    add(w, wvx - x0, wvy - y0, 1.0 - settle);
    add(s, svx - x0, svy - y0, settle);
    for (std::size_t i = 0; i < acc.size(); i += 4) {
        const double a = std::min(acc[i + 3], 1.0);
        rgba[i + 3] = static_cast<std::uint8_t>(a * 255.0 + 0.5);
        for (int c = 0; c < 3; ++c) {
            const double v = a > 0.0 ? acc[i + c] / a : 0.0;
            rgba[i + c] =
                static_cast<std::uint8_t>(std::min(v * 255.0 + 0.5, 255.0));
        }
    }
    return true;
}

bool unit_shadow_rgba(const UnitCatalog& cat, const std::string& ilb_root,
                      const MapUnit& unit, int direction, int clock, int phase,
                      std::vector<std::uint8_t>& rgba, int& out_w, int& out_h,
                      int& vx, int& vy, std::string& error) {
    rgba.clear();
    out_w = out_h = 0;
    vx = vy = 0;
    error.clear();
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    const UnitPose pose = resolve_unit_shadow(&cat, resolver, cache, unit,
                                              direction, clock, phase);
    if (!pose.ok) {
        error = "unit shadow did not resolve";
        return false;
    }
    const Sprite& spr = pose.lib->frames[pose.slot];
    rgba = spr.rgba;
    out_w = static_cast<int>(spr.w);
    out_h = static_cast<int>(spr.h);
    vx = pose.xoff + spr.vx;
    vy = pose.yoff + spr.vy;
    return true;
}

namespace {

// One hexagon's water-animation result (aow/map_resources.hpp).
struct WaterDraw {
    bool drew = false;
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    void cover(int ax0, int ay0, int ax1, int ay1) {
        if (!drew) {
            x0 = ax0; y0 = ay0; x1 = ax1; y1 = ay1;
            return;
        }
        if (ax0 < x0) x0 = ax0;
        if (ay0 < y0) y0 = ay0;
        if (ax1 > x1) x1 = ax1;
        if (ay1 > y1) y1 = ay1;
    }
};

// Blits one water sequence frame, leaving the destination's alpha channel alone
// (the level canvas is opaque, the split `contents` layer transparent, and the
// live front-ends paint their canvas themselves). A class-0x16 (TSprite16)
// sprite is *brightened* - `ImageLib.TSprite16.ShowTransparent@0x55211ea0` ->
// `ShowBlended` with the GFXE brighten table, `dst = max(dst, src)` - which is
// what every WATERANM / SEFFECT sprite is; anything else alpha-composites.
void water_blit(std::vector<std::uint8_t>& dst, int cw, int ch,
                const Sprite& s, int ox, int oy, bool bgra) {
    if (!s.ok) return;
    const int ch_off[3] = {bgra ? 2 : 0, 1, bgra ? 0 : 2};
    for (std::uint32_t y = 0; y < s.h; ++y) {
        const int ty = oy + static_cast<int>(y);
        if (ty < 0 || ty >= ch) continue;
        for (std::uint32_t x = 0; x < s.w; ++x) {
            const int tx = ox + static_cast<int>(x);
            if (tx < 0 || tx >= cw) continue;
            const std::size_t si = (static_cast<std::size_t>(y) * s.w + x) * 4;
            const unsigned a = s.rgba[si + 3];
            if (a == 0) continue;
            const std::size_t di =
                (static_cast<std::size_t>(ty) * cw + tx) * 4;
            if (s.class_id == 0x16) {  // the blended (brighten) path
                for (int k = 0; k < 3; ++k) {
                    const unsigned sv =
                        s.rgba[si + static_cast<std::size_t>(ch_off[k])];
                    if (sv > dst[di + static_cast<std::size_t>(k)]) {
                        dst[di + static_cast<std::size_t>(k)] =
                            static_cast<std::uint8_t>(sv);
                    }
                }
            } else {
                const unsigned inv = 255u - a;
                for (int k = 0; k < 3; ++k) {
                    const unsigned sv =
                        s.rgba[si + static_cast<std::size_t>(ch_off[k])];
                    dst[di + static_cast<std::size_t>(k)] =
                        static_cast<std::uint8_t>(
                            (sv * a +
                             dst[di + static_cast<std::size_t>(k)] * inv) / 255);
                }
            }
        }
    }
}

// Draws (or just measures, with `do_blit == false`) what
// `AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4` draws for the sequence-list
// entry `index` at counter value `counter`: the entry's whole chain, every
// sequence framed by the one-shot resolver `image_sequence_unlooped_frame`
// (`GetUnloopedFrameIndex@0x552188a8`), at `HXtoHP + seq.xoff/yoff + sprite
// base+shift`. `sx`/`sy` are the hex's canvas origin, which IS HXtoHP; the extra
// `kHexFieldMargin` is the port's canvas margin (see object_sprite_origin).
WaterDraw water_hex_sequences(const MapResource& r, int index, int counter,
                              int sx, int sy, std::vector<std::uint8_t>& dst,
                              int cw, int ch, bool bgra, LibResolver& resolver,
                              std::map<std::string, Library>& cache,
                              bool do_blit, int clip_x0, int clip_y0,
                              int clip_x1, int clip_y1) {
    WaterDraw out;
    if (index < 0 || static_cast<std::size_t>(index) >= r.sequences.size()) {
        return out;
    }
    const std::vector<const ImageSequence*> chain =
        image_sequence_chain(r.sequences[static_cast<std::size_t>(index)]);
    for (const ImageSequence* s : chain) {
        if (s == nullptr || s->library.empty()) continue;
        const std::string file = resolver.resolve(s->library);
        if (file.empty()) continue;
        const Library& lib = get_library(cache, file);
        if (lib.frames.empty()) continue;
        const int f = image_sequence_unlooped_frame(
            *s, static_cast<int>(lib.frames.size()), counter);
        if (f < 0) continue;
        // The engine's `bVar2` ("this hexagon drew something" - which is what
        // decides whether its counter is parked afterwards) is set as soon as the
        // resolver accepts the tick, *before* it tests the image
        // (`ShowDynamic`: `if (frame != -1) { bVar2 = true; if (ImageExist(...))
        // ...`). That matters: WATERANM's `Water010` is a 0-byte placeholder -
        // the deliberately blank frame the ripple tables lead with - so a ripple
        // that "draws" nothing for its first 21 ticks still runs its counter on.
        out.drew = true;
        if (static_cast<std::size_t>(f) >= lib.frames.size()) continue;
        const Sprite& sp = lib.frames[static_cast<std::size_t>(f)];
        if (!sp.ok) continue;
        const int ox = sx + s->xoff + sp.vx + kHexFieldMargin;
        const int oy = sy + s->yoff + sp.vy;
        const bool visible = !(ox >= clip_x1 || oy >= clip_y1 ||
                               ox + static_cast<int>(sp.w) <= clip_x0 ||
                               oy + static_cast<int>(sp.h) <= clip_y0);
        if (do_blit && visible) water_blit(dst, cw, ch, sp, ox, oy, bgra);
        if (visible) {
            out.cover(ox, oy, ox + static_cast<int>(sp.w),
                      oy + static_cast<int>(sp.h));
        }
    }
    return out;
}

// The live front-ends hand `draw_water_overlays` an increasing map frame, so the
// hexagons' counters must survive between calls (the engine keeps them on its
// sprites). The key covers the game root, the level and a signature of the
// level's terrain, so loading a different map restarts from `Create`; a call
// with a *smaller* tick rewinds and replays, which keeps a one-shot render and a
// test deterministic whatever order they ask in.
struct WaterHexState {
    int steps = 0;  // ShowDynamic calls already replayed (0 = after Create)
    WaterHexAnim st;
};

std::map<std::string, std::vector<WaterHexState>>& water_state_cache() {
    static std::map<std::string, std::vector<WaterHexState>> c;
    return c;
}

}  // namespace

// See mapviz.hpp. `AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4` in full: per
// hexagon, per map frame,
//   1. advance the hexagon's own counter `+0x18` and, on the frame it reaches 0,
//      roll the sequence-list index `+0x19`
//      (`water_hex_advance` / `water_sequence_roll`);
//   2. draw every sequence of that entry's chain with
//      `GetUnloopedFrameIndex(seq, counter)` (`water_hex_sequences`);
//   3. if nothing was drawn, sleep the counter for `-(0x14|10) - RandInt(0x19)`
//      frames (`water_hex_finish`).
// The other half of ShowDynamic - the shoreline shelf - is static, and the base
// tile plus its foam belong to the static pass; both are already in the cached
// level.
bool draw_water_overlays(const Map& m, const std::vector<MapResource>& res,
                         const std::string& ilb_root, int level, int tick,
                         std::vector<std::uint8_t>& rgba, std::uint32_t w,
                         std::uint32_t h, bool bgra, int& dirty_x0,
                         int& dirty_y0, int& dirty_x1, int& dirty_y1,
                         std::string& error, const WaterAnimOptions& water,
                         int clip_x0, int clip_y0, int clip_x1, int clip_y1) {
    dirty_x0 = dirty_y0 = dirty_x1 = dirty_y1 = 0;
    error.clear();
    if (level < 0 || static_cast<std::size_t>(level) >= m.levels.size()) {
        error = "level out of range";
        return false;
    }
    const int cw = static_cast<int>(w);
    const int ch = static_cast<int>(h);
    if (cw <= 0 || ch <= 0 ||
        rgba.size() < static_cast<std::size_t>(cw) *
                          static_cast<std::size_t>(ch) * 4) {
        error = "canvas size mismatch";
        return false;
    }
    const MapLevel& lv = m.levels[static_cast<std::size_t>(level)];
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    // The terrain number of every hex (its tag-50 resource's TTerrainList entry
    // 0, exactly like mapviz's `terrain_of_hex`) plus a signature of the level so
    // the state cache below is never shared between two different maps.
    std::vector<int> terr(static_cast<std::size_t>(lv.width) * lv.height, -1);
    std::uint32_t sig = 2166136261u;
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const std::size_t i = static_cast<std::size_t>(y) * lv.width + x;
            for (const MapHs& hs : lv.tiles[i].objects) {
                if (hs.tag != 50) continue;
                const MapResource* r = find_map_resource(res, hs.resource);
                if (r != nullptr && r->has_terrain) terr[i] = r->terrain;
                sig = (sig ^ hs.resource) * 16777619u;
                break;
            }
        }
    }
    const std::string key = ilb_root + "|" + std::to_string(level) + "|" +
                            std::to_string(lv.width) + "x" +
                            std::to_string(lv.height) + "|" +
                            std::to_string(sig);
    std::vector<WaterHexState>& states = water_state_cache()[key];
    if (states.size() != static_cast<std::size_t>(lv.width) * lv.height) {
        states.assign(static_cast<std::size_t>(lv.width) * lv.height,
                      WaterHexState{});
        for (WaterHexState& s : states) s.steps = -1;  // not created yet
    }
    if (clip_x1 <= clip_x0 || clip_y1 <= clip_y0) {  // "no clip": the whole canvas
        clip_x0 = 0;
        clip_y0 = 0;
        clip_x1 = cw;
        clip_y1 = ch;
    }
    // dir 0..5 = N/NE/E/S/W/NW (HSEngine.HNtoHX + TranslateHX column stagger).
    static const int dx6[6] = {0, 1, 1, 0, -1, -1};
    static const int dy6[6] = {-1, -1, 0, 1, 0, -1};
    bool any = false;
    auto cover = [&](int x0, int y0, int x1, int y1) {
        if (!any) {
            dirty_x0 = x0; dirty_y0 = y0; dirty_x1 = x1; dirty_y1 = y1;
            any = true;
            return;
        }
        if (x0 < dirty_x0) dirty_x0 = x0;
        if (y0 < dirty_y0) dirty_y0 = y0;
        if (x1 > dirty_x1) dirty_x1 = x1;
        if (y1 > dirty_y1) dirty_y1 = y1;
    };
    auto terrain_at = [&](std::uint32_t hx,
                          std::uint32_t hy) -> const MapResource* {
        const std::size_t i = static_cast<std::size_t>(hy) * lv.width + hx;
        for (const MapHs& hs : lv.tiles[i].objects) {
            if (hs.tag != 50) continue;
            return find_map_resource(res, hs.resource);
        }
        return nullptr;
    };
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const std::size_t idx = static_cast<std::size_t>(y) * lv.width + x;
            const MapResource* ownr = terrain_at(x, y);
            if (ownr == nullptr ||
                ownr->class_id != kWaterHexagonResourceClassId) {
                continue;
            }
            if (!ownr->has_seq || ownr->sequences.empty()) continue;
            // The two ShowDynamic inputs: `sprite+0x14` (the flat-land mask; 0
            // means open water, which is the 1/700 fish roll's precondition) and
            // `sprite+0x1a` (the N edge's variant byte, 0x28 for a snow neighbour
            // and 0x50 for wasteland - set by UpdateTransitions@5579ad98).
            int mask = 0;
            bool north_variant = false;
            for (int d = 0; d < 6; ++d) {
                int nx = static_cast<int>(x) + dx6[d];
                int ny = static_cast<int>(y) + dy6[d];
                if ((x & 1u) != 0 && (dx6[d] & 1) != 0) ++ny;
                if (nx < 0 || ny < 0 || nx >= static_cast<int>(lv.width) ||
                    ny >= static_cast<int>(lv.height)) {
                    continue;
                }
                const int nt = terr[static_cast<std::size_t>(ny) * lv.width + nx];
                if (nt >= 1 && nt <= 5) {
                    mask |= 1 << d;
                    if (d == 0 && (nt == 3 || nt == 5)) north_variant = true;
                }
            }
            const bool open_water = (mask == 0);
            WaterHexState& hs = states[idx];
            const int hx = static_cast<int>(x);
            const int hy = static_cast<int>(y);
            if (hs.steps < 0 || hs.steps > tick) {  // Create / rewind
                hs.st = water_hex_create(hx, hy);
                hs.steps = 0;
            }
            if (!water_hex_animates(water, north_variant)) continue;
            if (hs.steps == tick) continue;  // already composited this frame
            int sx = 0, sy = 0;
            hex_origin(hx, hy, sx, sy);
            while (hs.steps < tick) {
                const bool last = (hs.steps + 1 == tick);
                const WaterTick tk = water_hex_advance(
                    hs.st, hx, hy, open_water, north_variant, water);
                WaterDraw d;
                if (tk.index >= 0) {
                    d = water_hex_sequences(*ownr, tk.index, tk.counter, sx, sy,
                                            rgba, cw, ch, bgra, resolver, cache,
                                            last, clip_x0, clip_y0, clip_x1,
                                            clip_y1);
                }
                water_hex_finish(hs.st, hx, hy, d.drew, water);
                ++hs.steps;
                if (last && d.drew) cover(d.x0, d.y0, d.x1, d.y1);
            }
        }
    }
    if (!any) dirty_x0 = dirty_y0 = dirty_x1 = dirty_y1 = 0;
    return true;
}

// See mapviz.hpp. The engine re-draws its whole scene every map frame
// (`HSEngine.THSMap.NewFrame@5560d758` bumps the clock and the scene is shown
// again); a level render here costs ~28 ms, so a live front-end keeps the static
// level cached (rendered with `MapVizOptions::no_animated_overlays`) and
// re-composites only the animated building sequences, with the map clock and the
// per-hex phase. This walks exactly what
// `HSEngine.TMapObjectResource.ShowComplexImage@5560a134` draws: the sequence
// chain from the object's cell image, every sequence whose `GetFrameIndex` is
// not -1, at `HXtoHP + seq.xoff/yoff + sprite base+shift`.
// `AoWE.TPlayerStructure.UpdateFlagID@55760ed8` (and `TCity.UpdateFlagID
// @557ac8b0`): the flag *byte* the draw reads - 0x3f = "no flag", 0x20 = the
// neutral flag, otherwise the owner's race id (| `(city level - 1) << 6` for a
// city, which the draw turns back into the flagpole variant). See
// aow/mapviz.hpp and docs/19_STRUCTURE_FLAGS.md.
bool structure_flag_byte(const MapResource* res, std::uint32_t object_class_id,
                         std::uint8_t owner, std::uint8_t city_level,
                         std::uint8_t city_flags,
                         const std::vector<std::uint8_t>& player_races,
                         std::uint8_t& out) {
    if (res == nullptr) return false;
    if (!is_player_structure_resource(res->class_id)) return false;
    if (!is_player_structure_class(object_class_id)) return false;
    if (object_class_id == kCityClassId) {
        // `TCity.UpdateFlagID`'s gate: a razed city (or one whose bit 0 is set)
        // draws nothing. `GetBuilding` is false for a map-placed structure:
        // `TStructure.Create` leaves the build counter at -1 and no shipped map
        // writes rw tag 0xff.
        if ((city_flags & 0x03) != 0) return false;
        // The size byte at +0x4d (1..4) occupies the flag byte's top two bits -
        // `(level - 1) * 0x40` evaluated in the engine's *byte* arithmetic, so an
        // absent byte (0) wraps to pole variant 3.
        const std::uint8_t pole =
            static_cast<std::uint8_t>((city_level - 1) & 0x03) * 0x40;
        if (owner >= 1 && static_cast<std::size_t>(owner) < player_races.size()) {
            out = static_cast<std::uint8_t>(player_races[owner] + pole);
            return true;
        }
        // An unowned city: `0x20 + (level-1)*0x40`. TCity.Show additionally
        // requires `resource+0x64 != 0` for a neutral flag, which every
        // TCityResource has (see kPsrClasses).
        if (!res->flag_neutral_when_unowned) return false;
        out = static_cast<std::uint8_t>(0x20 + pole);
        return true;
    }
    if (owner >= 1 && static_cast<std::size_t>(owner) < player_races.size()) {
        out = player_races[owner];
        return true;
    }
    // `TPlayerStructure.UpdateFlagID`'s `owner < 1` branch: the neutral flag only
    // when the resource wants one (cities; every other subclass clears +0x64).
    if (!res->flag_neutral_when_unowned) return false;
    out = 0x20;
    return true;
}

bool resolve_structure_flag(const MapResource* res, std::uint32_t object_class_id,
                            std::uint8_t owner, std::uint8_t city_level,
                            std::uint8_t city_flags,
                            const std::vector<std::uint8_t>& player_races,
                            const std::vector<std::string>& race_libraries,
                            int clock, int hex_pixel_x, StructureFlag& out) {
    out = StructureFlag{};
    std::uint8_t flag = 0;
    if (!structure_flag_byte(res, object_class_id, owner, city_level, city_flags,
                             player_races, flag)) {
        return false;
    }
    // `Show` picks the flag *style* from the resource's pad index (+0x46): a
    // structure that covers a single hex draws `TFlagControl.ShowFlag`, anything
    // covering 2+ hexes with a terrain draws `ShowStructureFlag`. A city's pad
    // index is its size (City1..City4 -> 1..4), so a village uses the fixed
    // single-hex pole (Flags.ILB frame 4) and a larger city the pole variant the
    // flag byte selects.
    const bool city_style = structure_pad_index(*res) < 2;
    const int step = structure_flag_step(clock, hex_pixel_x, city_style);
    player_structure_flag_anchor(*res, out.dx, out.dy);

    if (flag & 0x20) {
        // The neutral flag lives in Flags.ILB, right after the poles.
        out.pole_frame = city_style ? kFlagPoleFrameCity
                                    : static_cast<std::size_t>(flag >> 6);
        out.flag_library = "Flags.ILB";
        out.flag_frame = (city_style ? kNeutralFlagFrameCity
                                     : kNeutralFlagFrameStructure) +
                         static_cast<std::size_t>(step);
        return true;
    }
    // Owned: the flag is the *owner's race* flag, out of that race's library -
    // the same `RACES\R*.ILB` a city of that race draws its body from
    // (`TFlagControl.ShowFlag` -> `TRaceResourceList.GetRaceResource` -> +0x1c).
    const std::size_t race = static_cast<std::size_t>(flag & 0x1f);
    if (race >= race_libraries.size() || race_libraries[race].empty()) {
        return false;  // unknown owner race -> the engine would draw nothing
    }
    out.pole_frame = city_style ? kFlagPoleFrameCity
                                : static_cast<std::size_t>(flag >> 6);
    out.flag_library = race_libraries[race];
    out.flag_frame = (city_style ? kOwnedFlagFrameCity
                                 : kOwnedFlagFrameStructure) +
                     static_cast<std::size_t>(step);
    return true;
}

bool draw_structure_overlays(const Map& m, const std::vector<MapResource>& res,
                             const std::string& ilb_root, int level, int clock,
                             std::vector<std::uint8_t>& rgba, std::uint32_t w,
                             std::uint32_t h, bool bgra, int& dirty_x0,
                             int& dirty_y0, int& dirty_x1, int& dirty_y1,
                             std::string& error) {
    dirty_x0 = dirty_y0 = dirty_x1 = dirty_y1 = 0;
    error.clear();
    if (level < 0 || static_cast<std::size_t>(level) >= m.levels.size()) {
        error = "level out of range";
        return false;
    }
    const int cw = static_cast<int>(w);
    const int ch = static_cast<int>(h);
    if (cw <= 0 || ch <= 0 ||
        rgba.size() < static_cast<std::size_t>(cw) *
                          static_cast<std::size_t>(ch) * 4) {
        error = "canvas size mismatch";
        return false;
    }
    const MapLevel& lv = m.levels[static_cast<std::size_t>(level)];
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    // The owner flags are the owning player's *race* flags, so resolve the
    // roster (TPlayer rw tag 2) and the race image libraries once per call.
    const std::vector<std::uint8_t> races = map_player_races(m);
    const std::vector<std::string> race_ilbs = race_image_libraries(res);
    bool any = false;
    auto cover = [&](int x0, int y0, int x1, int y1) {
        if (!any) {
            dirty_x0 = x0;
            dirty_y0 = y0;
            dirty_x1 = x1;
            dirty_y1 = y1;
            any = true;
            return;
        }
        if (x0 < dirty_x0) dirty_x0 = x0;
        if (y0 < dirty_y0) dirty_y0 = y0;
        if (x1 > dirty_x1) dirty_x1 = x1;
        if (y1 > dirty_y1) dirty_y1 = y1;
    };
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const MapTile& t =
                lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
            for (const MapHs& hs : t.objects) {
                if (hs.tag == 50) continue;
                // An army draws through TUnitHS.Show and a road/bridge through
                // its own resource - none of them here.
                if (hs.class_id == kRoadClassId ||
                    hs.class_id == kHexBridgeClassId ||
                    hs.class_id == kMapArmyHsClassId) {
                    continue;
                }
                const MapResource* r = find_map_resource(res, hs.resource);
                int sx = 0, sy = 0;
                hex_origin(static_cast<int>(x), static_cast<int>(y), sx, sy);
                // The owner flag a takeable structure draws (docs/19). It is
                // animated off the map clock, so a live front-end composites it
                // here even for a city, whose body is already in the cached
                // level (drawn through TCityResource.ShowCity, NOT this path).
                {
                    FlagSprites fs;
                    if (structure_flag_sprites(hs, r, sx, sy, clock, races,
                                               race_ilbs, resolver, cache, fs)) {
                        const Sprite* parts[2] = {fs.pole, fs.flag};
                        const int px[2] = {fs.pole_x, fs.flag_x};
                        const int py[2] = {fs.pole_y, fs.flag_y};
                        for (int i = 0; i < 2; ++i) {
                            if (parts[i] == nullptr) continue;
                            const Sprite& sp = *parts[i];
                            // A class-0x16 (TSprite16) sprite is blended with the
                            // GFXE brighten table, every other class alpha-shows
                            // (the same rule the sequence overlays below use).
                            if (sp.class_id == 0x16) {
                                blit_sprite_brighten_buffer(
                                    rgba, cw, ch, sp.rgba,
                                    static_cast<int>(sp.w),
                                    static_cast<int>(sp.h), px[i], py[i], bgra);
                            } else {
                                blit_sprite_buffer(rgba, cw, ch, sp.rgba,
                                                   static_cast<int>(sp.w),
                                                   static_cast<int>(sp.h), px[i],
                                                   py[i], bgra);
                            }
                            cover(px[i], py[i], px[i] + static_cast<int>(sp.w),
                                  py[i] + static_cast<int>(sp.h));
                        }
                    }
                }
                // A city's *body* is the three ShowCity layers, not the
                // resource's sequence list - nothing more to do here.
                if (hs.class_id == kCityClassId) continue;
                if (r == nullptr || !r->has_seq) continue;
                const ImageSequence* body = body_sequence(*r);
                if (body == nullptr) continue;
                const int phase = object_anim_phase(static_cast<int>(x),
                                                    static_cast<int>(y));
                const std::vector<const ImageSequence*> chain =
                    image_sequence_chain(*body);
                for (std::size_t i = 0; i < chain.size(); ++i) {
                    const ImageSequence* s = chain[i];
                    if (s->library.empty() || is_effect_library(s->library)) {
                        continue;
                    }
                    // Only an ANIMATED body (mode != 0) is left out of the cached
                    // level; a static body is already baked into it.
                    if (i == 0 && (s->mode & 0x7f) == 0) continue;
                    const std::string file = resolver.resolve(s->library);
                    if (file.empty()) continue;
                    const Library& lib = get_library(cache, file);
                    if (lib.frames.empty()) continue;
                    const int f = image_sequence_frame(
                        *s, static_cast<int>(lib.frames.size()), clock, phase);
                    if (f < 0 ||
                        static_cast<std::size_t>(f) >= lib.frames.size()) {
                        continue;
                    }
                    const Sprite& sp = lib.frames[static_cast<std::size_t>(f)];
                    if (!sp.ok) continue;
                    const int ox = sx + s->xoff + sp.vx + kHexFieldMargin;
                    const int oy = sy + s->yoff + sp.vy;
                    // A class-0x16 (TSprite16) sequence overlay is blended with
                    // the GFXE brighten table, every other class alpha-shows.
                    if (sp.class_id == 0x16) {
                        blit_sprite_brighten_buffer(rgba, cw, ch, sp.rgba,
                                                    static_cast<int>(sp.w),
                                                    static_cast<int>(sp.h), ox,
                                                    oy, bgra);
                    } else {
                        blit_sprite_buffer(rgba, cw, ch, sp.rgba,
                                           static_cast<int>(sp.w),
                                           static_cast<int>(sp.h), ox, oy, bgra);
                    }
                    cover(ox, oy, ox + static_cast<int>(sp.w),
                          oy + static_cast<int>(sp.h));
                }
            }
        }
    }
    if (!any) dirty_x0 = dirty_y0 = dirty_x1 = dirty_y1 = 0;
    return true;
}

// See mapviz.hpp. The engine shows a placed army as an ordinary hexagon sprite,
// but the port composites every army LAST: `render_map_rgba` collects them during
// its object sweep and blits them after the whole sweep (`pending_units`), and a
// live front-end - which keeps them out of its cached level with
// `MapVizOptions::no_units` - re-composites them here, after the buildings'
// animated overlays and the water. That is the order the engine's sorted scene
// produces (`AoWE.TUnitHS.CalcShowPriority@0x557840c8` = `0x68000000 | position`,
// above the structures' layer), so a unit standing next to a building is never
// covered by the building's glow, its owner flag or a ripple. The only layer
// meant to sit *above* the units is the map's spell gfx (`AoWE.TWorldFXHS`),
// which the port does not have yet.
bool draw_unit_overlays(const Map& m, const std::string& ilb_root, int level,
                        int clock, std::vector<std::uint8_t>& rgba,
                        std::uint32_t w, std::uint32_t h, bool bgra,
                        int& dirty_x0, int& dirty_y0, int& dirty_x1,
                        int& dirty_y1, std::string& error, int hide_level,
                        int hide_x, int hide_y, int clip_x0, int clip_y0,
                        int clip_x1, int clip_y1) {
    dirty_x0 = dirty_y0 = dirty_x1 = dirty_y1 = 0;
    error.clear();
    if (level < 0 || static_cast<std::size_t>(level) >= m.levels.size()) {
        error = "level out of range";
        return false;
    }
    const int cw = static_cast<int>(w);
    const int ch = static_cast<int>(h);
    if (cw <= 0 || ch <= 0 ||
        rgba.size() < static_cast<std::size_t>(cw) *
                          static_cast<std::size_t>(ch) * 4) {
        error = "canvas size mismatch";
        return false;
    }
    const MapLevel& lv = m.levels[static_cast<std::size_t>(level)];
    LibResolver& resolver = shared_resolver(ilb_root);
    std::map<std::string, Library>& cache = shared_libraries();
    // An army's sprite resolves through the game's unit roster - the same
    // process-wide catalog the level render uses (`shared_unit_catalog`, so the
    // per-frame call does not re-parse the .PFS files). A map without one - or a
    // missing Release/ - simply draws no armies, exactly like the level pass.
    const UnitCatalog* cat = shared_unit_catalog(ilb_root);
    // The army banner needs the owner's race. The live path has no Release.hss
    // table, so `resolve_army_banner` falls back to the shipped faction order
    // (the same RACES\R*.ILB names the resource table yields).
    const std::vector<std::uint8_t> player_races = map_player_races(m);
    const std::vector<std::string> race_libraries;
    const bool clip = clip_x1 > clip_x0 && clip_y1 > clip_y0;
    // One army's resolved body + ground shadow. `sprite`/`shadow` point into the
    // shared library cache, whose node-based std::map storage keeps them valid
    // for the whole call (exactly like the level pass's `pending_units`).
    struct PendingUnit {
        const Sprite* sprite = nullptr;
        int ox = 0;
        int oy = 0;
        const Sprite* shadow = nullptr;
        int shadow_ox = 0;
        int shadow_oy = 0;
        ArmyBanner banner{};
    };
    std::vector<PendingUnit> units;
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const MapTile& t =
                lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
            for (const MapHs& hs : t.objects) {
                if (hs.class_id != kMapArmyHsClassId || hs.units.empty()) {
                    continue;
                }
                // The walk animation hides the army on the hex it is moving onto:
                // the moving sprite stands in for it.
                if (hide_level == level && hide_x == static_cast<int>(x) &&
                    hide_y == static_cast<int>(y)) {
                    continue;
                }
                int sx = 0, sy = 0;
                hex_origin(static_cast<int>(x), static_cast<int>(y), sx, sy);
                // Cheap reject before the (much dearer) sprite resolution: a
                // unit's cell is at most 64x64 and its anchor is
                // `hex_origin + seq.xoff/yoff (-16,-24) + base+shift`, so a hex
                // further than `kUnitClipMargin` from the clip cannot put a
                // pixel inside it. This keeps a live front-end's per-frame cost
                // proportional to the *visible* armies, not to the map's.
                if (clip && (sx + kUnitClipMargin < clip_x0 ||
                             sx - kUnitClipMargin >= clip_x1 ||
                             sy + kUnitClipMargin < clip_y0 ||
                             sy - kUnitClipMargin >= clip_y1)) {
                    continue;
                }
                const int dir = static_cast<int>(hs.army_direction);
                // The standing pose is a mode-0 sequence, so it does not depend on
                // the map clock - the same resolution the level pass uses.
                const UnitPose pose =
                    resolve_unit_pose(cat, resolver, cache, hs.units.front(),
                                      dir, /*moving=*/false, 0, 0);
                if (!pose.ok) continue;
                const Sprite& s = pose.lib->frames[pose.slot];
                if (!s.ok) continue;
                PendingUnit pu;
                pu.sprite = &s;
                pu.ox = sx + pose.xoff + s.vx;
                pu.oy = sy + pose.yoff + s.vy;
                const UnitPose sh = resolve_unit_shadow(
                    cat, resolver, cache, hs.units.front(), dir, clock,
                    object_anim_phase(static_cast<int>(x),
                                      static_cast<int>(y)));
                if (sh.ok) {
                    const Sprite& ss = sh.lib->frames[sh.slot];
                    if (ss.ok) {
                        pu.shadow = &ss;
                        pu.shadow_ox = sx + sh.xoff + ss.vx;
                        pu.shadow_oy = sy + sh.yoff + ss.vy;
                    }
                }
                resolve_army_banner(hs, player_races, race_libraries, resolver,
                                    cache, sx, sy, pu.banner);
                units.push_back(pu);
            }
        }
    }
    bool any = false;
    auto cover = [&](int x0, int y0, int x1, int y1) {
        if (!any) {
            dirty_x0 = x0;
            dirty_y0 = y0;
            dirty_x1 = x1;
            dirty_y1 = y1;
            any = true;
            return;
        }
        if (x0 < dirty_x0) dirty_x0 = x0;
        if (y0 < dirty_y0) dirty_y0 = y0;
        if (x1 > dirty_x1) dirty_x1 = x1;
        if (y1 > dirty_y1) dirty_y1 = y1;
    };
    auto visible = [&](int ox, int oy, int sw, int sh) {
        if (!clip) return true;
        return !(ox >= clip_x1 || oy >= clip_y1 || ox + sw <= clip_x0 ||
                 oy + sh <= clip_y0);
    };
    // Pass 1: every shadow. `TUnitShadowHS.GetShowPriority@0x55783d64` is the
    // unit's priority with the layer dropped to 0x64000000, so the engine's
    // sorted scene composites *all* shadows before *all* bodies - two row-major
    // passes, not one interleaved pass. The blend is the 50 % average
    // (blit_shadow_buffer), not the body's alpha blit.
    for (const PendingUnit& u : units) {
        if (u.shadow == nullptr) continue;
        const int sw = static_cast<int>(u.shadow->w);
        const int sh = static_cast<int>(u.shadow->h);
        if (!visible(u.shadow_ox, u.shadow_oy, sw, sh)) continue;
        blit_shadow_buffer(rgba, cw, ch, u.shadow->rgba, sw, sh, u.shadow_ox,
                           u.shadow_oy, bgra);
        cover(u.shadow_ox, u.shadow_oy, u.shadow_ox + sw, u.shadow_oy + sh);
    }
    // Pass 2: every body, in the same row-major order. A class-0x16 sprite
    // (TSprite16) is *blended* - the GFXE brighten table, `dst = max(dst, src)` -
    // and every other class alpha-composites, exactly like the level pass.
    for (const PendingUnit& u : units) {
        const Sprite& s = *u.sprite;
        const int sw = static_cast<int>(s.w);
        const int sh = static_cast<int>(s.h);
        if (visible(u.ox, u.oy, sw, sh)) {
            if (s.class_id == 0x16) {
                blit_sprite_brighten_buffer(rgba, cw, ch, s.rgba, sw, sh, u.ox,
                                            u.oy, bgra);
            } else {
                blit_sprite_buffer(rgba, cw, ch, s.rgba, sw, sh, u.ox, u.oy,
                                   bgra);
            }
            cover(u.ox, u.oy, u.ox + sw, u.oy + sh);
        }
        // The army's size shield + leader crown, after its body and before the
        // next army - `TArmyHS.Show` order. Colour-keyed art: opaque alpha blit.
        if (u.banner.shield != nullptr) {
            const Sprite& b = *u.banner.shield;
            const int bw = static_cast<int>(b.w);
            const int bh = static_cast<int>(b.h);
            if (visible(u.banner.shield_x, u.banner.shield_y, bw, bh)) {
                blit_sprite_buffer(rgba, cw, ch, b.rgba, bw, bh,
                                   u.banner.shield_x, u.banner.shield_y, bgra);
                cover(u.banner.shield_x, u.banner.shield_y,
                      u.banner.shield_x + bw, u.banner.shield_y + bh);
            }
        }
        if (u.banner.crown != nullptr) {
            const Sprite& c = *u.banner.crown;
            const int cw2 = static_cast<int>(c.w);
            const int ch2 = static_cast<int>(c.h);
            if (visible(u.banner.crown_x, u.banner.crown_y, cw2, ch2)) {
                blit_sprite_buffer(rgba, cw, ch, c.rgba, cw2, ch2,
                                   u.banner.crown_x, u.banner.crown_y, bgra);
                cover(u.banner.crown_x, u.banner.crown_y,
                      u.banner.crown_x + cw2, u.banner.crown_y + ch2);
            }
        }
    }
    if (!any) dirty_x0 = dirty_y0 = dirty_x1 = dirty_y1 = 0;
    return true;
}

}  // namespace aow

