// map_resources.cpp - loads the world map-object/terrain resource table from
// Release.hss (see aow/map_resources.hpp for the format + evidence).
#include "aow/map_resources.hpp"

#include <cctype>
#include <fstream>
#include <iterator>

#include "aow/eobject.hpp"
#include "aow/endian.hpp"

namespace aow {
namespace {

// Extracts every printable u32-length-prefixed string in [buf, buf+n).
void collect_u32_strings(const std::uint8_t* buf, std::size_t n,
                         std::vector<std::string>& out) {
    std::size_t i = 0;
    while (i + 4 <= n) {
        const std::uint32_t len = read_u32(buf + i);
        if (len >= 3 && len <= 64 && i + 4 + len <= n) {
            bool printable = true;
            for (std::uint32_t k = 0; k < len; ++k) {
                const std::uint8_t c = buf[i + 4 + k];
                if (c < 32 || c >= 127) { printable = false; break; }
            }
            if (printable) {
                out.emplace_back(reinterpret_cast<const char*>(buf + i + 4), len);
                i += 4 + len;
                continue;
            }
        }
        ++i;
    }
}

}  // namespace

// One `TPlayerStructureResource` subclass: its engine class id plus the two
// runtime flag bytes its `Create` ends up with. `pad_anchored` is resource+0x44
// and `neutral` resource+0x64; both come from the chain of `Create` calls (Ghidra
// decompiles in re/exports/flags, flags2, flags3):
//
//   TStructureResource.Create@5576046c          +0x44 = 1, +0x45 = 1
//   TPlayerStructureResource.Create@55761974    +0x64 = 1, +0x68/+0x6c = (0x14,-0x23)
//   TProductionPlaceResource.Create@557bf400    +0x58 = 1, +0x64 = 0
//
// so a class that never mentions +0x44 keeps 1, and one whose chain sets
// +0x64 = 0 (every ProductionPlace descendant plus Farm, Mine, Tower, PowerNode,
// ReflectingPool and the altars) drops the neutral flag. `City.TCityResource`
// keeps `+0x64 = 1` - only a city that nobody owns still shows the neutral flag.
struct PlayerStructureResourceInfo {
    std::uint32_t class_id;
    bool pad_anchored;  // resource+0x44 (GenerateFlagPositions re-anchors)
    bool neutral;       // resource+0x64 (draw the neutral flag while unowned)
};

namespace {
const PlayerStructureResourceInfo* psr_info(std::uint32_t class_id);
}

bool load_map_resources(const std::string& path, std::vector<MapResource>& out,
                        std::string& error) {
    out.clear();
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "cannot open " + path;
        return false;
    }
    std::vector<std::uint8_t> file((std::istreambuf_iterator<char>(in)),
                                   std::istreambuf_iterator<char>());
    if (file.size() < 16) {
        error = "file too small: " + path;
        return false;
    }
    const std::size_t region_off = read_u32(file.data());
    if (region_off < 16 || region_off >= file.size()) {
        error = "bad region offset: " + path;
        return false;
    }
    const std::uint8_t* R = file.data() + region_off;
    const std::size_t n = file.size() - region_off;

    EObjectTable outer;
    if (!read_eobject_table(R, n, 0, outer)) {
        error = "cannot read header table: " + path;
        return false;
    }
    const std::size_t t0 = eobject_find(outer, 0);
    if (t0 == static_cast<std::size_t>(-1)) {
        error = "no top object: " + path;
        return false;
    }
    EObjectTable top;
    if (!read_eobject_table(R, n, outer.base + t0, top)) {
        error = "cannot read top object: " + path;
        return false;
    }
    // tag 5 = THSSet+0x24 = TMapObjectResourceList.
    const std::size_t t5 = eobject_find(top, 5);
    if (t5 == static_cast<std::size_t>(-1)) {
        error = "no TMapObjectResourceList (tag 5): " + path;
        return false;
    }
    EObjectTable list;
    if (!read_eobject_table(R, n, top.base + t5, list)) {
        error = "cannot read resource list: " + path;
        return false;
    }
    const std::size_t c1 = eobject_find(list, 1);
    if (c1 == static_cast<std::size_t>(-1)) {
        error = "resource list has no children: " + path;
        return false;
    }
    EObjectTable ch;
    if (!read_eobject_table(R, n, list.base + c1, ch)) {
        error = "cannot read resource children: " + path;
        return false;
    }

    out.reserve(ch.entries.size());
    for (std::size_t i = 0; i < ch.entries.size(); ++i) {
        const std::size_t a = ch.base + ch.entries[i].second;
        if (a + 4 > n) continue;
        const std::size_t next = (i + 1 < ch.entries.size())
            ? ch.base + ch.entries[i + 1].second
            : n;
        MapResource r;
        r.offset = a;
        r.class_id = read_u32(R + a);
        EObjectTable sub;
        if (!read_eobject_table(R, n, a + 4, sub)) continue;
        const std::size_t o1 = eobject_find(sub, 1);
        if (o1 == static_cast<std::size_t>(-1)) continue;
        r.id = read_u32(R + sub.base + o1);
        const std::size_t o2 = eobject_find(sub, 2);
        if (o2 != static_cast<std::size_t>(-1)) r.res_type = read_u32(R + sub.base + o2);
        const std::size_t o3 = eobject_find(sub, 3);
        if (o3 != static_cast<std::size_t>(-1)) r.res_index = read_u32(R + sub.base + o3);
        // tag 7 = TTerrainList (see aow/map_resources.hpp). Serialised by
        // HSEngine.TTerrainList.LoadFromStream as `u32 count` then `count` u16
        // entries; GetTerrain(list, i) returns entry i's low byte and
        // GetOverlay(list, i) its high byte (HSEngine.TTerrainList.GetOverlay
        // @556050c8). Entry 0 is the hexagon resource's own
        // `terrain | overlay << 8`: terrain 0 water, 1..5 land families, 6 ice.
        // An overlay byte >= 0x80 is "no overlay" (TMapObject.GetOverlay@556097b8 /
        // TMultiHexMO.OverrideTerrain@5560b2d4 only write it when it is >= 0 as a
        // signed char), so it is stored as -1 - the raw code 0 is mountains, not
        // "plain" (see aow::move_point_cost).
        const std::size_t o7 = eobject_find(sub, 7);
        if (o7 != static_cast<std::size_t>(-1)) {
            const std::size_t o = sub.base + o7;
            if (o + 4 <= n) {
                const std::uint32_t cnt = read_u32(R + o);
                if (cnt >= 1 && cnt <= 4096 && o + 4 + static_cast<std::size_t>(cnt) * 2 <= n) {
                    r.terrain = R[o + 4];
                    const std::uint8_t ov = R[o + 5];
                    r.overlay = (ov >= 0x80) ? -1 : ov;
                    r.has_terrain = true;
                    r.terrain_list.resize(cnt);
                    for (std::uint32_t k = 0; k < cnt; ++k) {
                        r.terrain_list[k] = static_cast<std::uint16_t>(
                            R[o + 4 + k * 2] | (R[o + 5 + k * 2] << 8));
                    }
                }
            }
        }
        // TMultiHexMOR placement (tags 12/13 = dXmainP/dYmainP); see the header
        // for the evidence (HSEngine.TMultiHexMOR.ReadWrite, TMultiHexMO.Show).
        const std::size_t o12 = eobject_find(sub, 12);
        const std::size_t o13 = eobject_find(sub, 13);
        if (o12 != static_cast<std::size_t>(-1) &&
            o13 != static_cast<std::size_t>(-1)) {
            r.dx_main = read_i32(R + sub.base + o12);
            r.dy_main = read_i32(R + sub.base + o13);
            r.has_shift = true;
        }
        // tag 8 = the resource's image library, serialized inline (a complete ILB
        // image). See aow/map_resources.hpp.
        const std::size_t o8 = eobject_find(sub, 8);
        if (o8 != static_cast<std::size_t>(-1)) {
            const std::size_t o = sub.base + o8;
            if (o + 0x18 <= n && R[o] == 0x04 && R[o + 1] == 'I' && R[o + 2] == 'L' &&
                R[o + 3] == 'B') {
                const std::uint32_t total = read_u32(R + o + 0x14);
                if (total >= 0x18 && o + total <= n) {
                    r.ilb_data.assign(R + o, R + o + total);
                }
            }
        }
        // tags 9/10 = the image-sequence list (a `TImageSequenceList`; see
        // aow/map_resources.hpp and docs/16_UNIT_ANIM.md). Parsed in FULL with
        // the same reader the unit animation uses, so the renderer can pick the
        // sequence `ShowComplexImage` would draw. The earlier fixed-offset scan
        // only recovered the *first* name in the blob, which for a Shipyard is
        // `SMOKE.ILB` (an additive effect) rather than its body `PRO_SYRD.ILB` -
        // so the renderer dropped the whole structure.
        const std::size_t o9 = eobject_find(sub, 9);
        const std::size_t o9b = (o9 != static_cast<std::size_t>(-1))
                                    ? o9
                                    : eobject_find(sub, 10);
        if (o9b != static_cast<std::size_t>(-1)) {
            const std::size_t head = sub.base + o9b;
            if (head < n) {
                std::string serr;
                if (parse_image_sequence_list(R + head, n - head, r.sequences,
                                              serr)) {
                    // `body_sequence` (map_resources.cpp) is the sequence the
                    // engine's sequence chain shows as the structure's body.
                    const ImageSequence* body = body_sequence(r);
                    if (body != nullptr) {
                        r.has_seq = true;
                        r.seq_lib = body->library;
                        r.seq_frame = static_cast<std::size_t>(
                            body->base_frame < 0 ? 0 : body->base_frame);
                        r.seq_dx = body->xoff;
                        r.seq_dy = body->yoff;
                    }
                } else {
                    r.sequences.clear();
                }
            }
        }
        if (next > a) collect_u32_strings(R + a, next - a, r.ilbs);
        if (!r.ilbs.empty()) r.ilb = r.ilbs.front();

        out.push_back(std::move(r));
    }

    // Race resources: TAoWWHSSet rw **tag 10** (`+0x54`) is the
    // TRaceResourceList. Evidence: AoWE.TAoWHSSet.ReadWrite@5574c528 reads tag
    // 10 (decimal; Ghidra prints it without an 0x prefix) into +0x54 and
    // Create@5574b9ec installs a TRaceResourceList there.
    // Each child is an AoWE.TRaceResource (class 0x20201) whose rw **tag 8** is
    // the race image library name (`RACES\RHUMAN.ILB`, ...). The list order IS
    // the race/faction index a city's rw tag 0xd refers to (index 9 = Orc,
    // 10 = Goblin - docs/01_FORMATS.md). Appended with is_race=true so
    // race_image_libraries() can recover them in order.
    const std::size_t t10 = eobject_find(top, 10);
    if (t10 != static_cast<std::size_t>(-1)) {
        EObjectTable rl;
        if (read_eobject_table(R, n, top.base + t10, rl)) {
            const std::size_t rc = eobject_find(rl, 1);
            if (rc != static_cast<std::size_t>(-1)) {
                EObjectTable rch;
                if (read_eobject_table(R, n, rl.base + rc, rch)) {
                    for (std::size_t i = 0; i < rch.entries.size(); ++i) {
                        const std::size_t a = rch.base + rch.entries[i].second;
                        const std::size_t next =
                            (i + 1 < rch.entries.size())
                                ? rch.base + rch.entries[i + 1].second
                                : n;
                        if (a + 4 > n || read_u32(R + a) != 0x20201) continue;
                        MapResource r;
                        r.is_race = true;
                        r.class_id = 0x20201;
                        r.res_index = static_cast<std::uint32_t>(i);
                        EObjectTable sub;
                        if (read_eobject_table(R, n, a + 4, sub)) {
                            const std::size_t o1 = eobject_find(sub, 1);
                            if (o1 != static_cast<std::size_t>(-1) &&
                                sub.base + o1 + 4 <= n) {
                                r.id = read_u32(R + sub.base + o1);
                            }
                            // tag 8 = the race image library, a `rwString`
                            // (u8 length prefix, 0xFF => u32) - see
                            // TRaceResource.ReadWrite (+0x20) and LinkToIL.
                            const std::size_t o8 = eobject_find(sub, 8);
                            if (o8 != static_cast<std::size_t>(-1)) {
                                r.ilb = eobject_short_string(R, n, sub.base + o8);
                            }
                        }
                        if (r.ilb.empty()) {
                            const std::size_t he =
                                (next > a && next <= n) ? next : n;
                            collect_u32_strings(R + a, he - a, r.ilbs);
                            if (!r.ilbs.empty()) r.ilb = r.ilbs.front();
                        } else {
                            r.ilbs.push_back(r.ilb);
                        }
                        out.push_back(std::move(r));
                    }
                }
            }
        }
    }

    // Bake in each TPlayerStructureResource subclass's `Create` defaults (the
    // flag bytes the engine keeps at resource+0x44 / +0x64, docs/19).
    for (MapResource& r : out) {
        const PlayerStructureResourceInfo* info = psr_info(r.class_id);
        if (info == nullptr) continue;
        r.is_player_structure = true;
        r.flag_pad_anchored = info->pad_anchored;
        r.flag_neutral_when_unowned = info->neutral;
    }
    return true;
}

const MapResource* find_map_resource(const std::vector<MapResource>& list,
                                     std::uint32_t id) {
    for (const MapResource& r : list) {
        if (!r.is_race && r.id == id) return &r;
    }
    return nullptr;
}

bool is_effect_library(const std::string& library) {
    // Fold to the lowercased basename without extension, so `SMOKE.ILB`,
    // `smoke.ilb` and `EFFECTS\TWINKGR.ILB` all match.
    std::string s = library;
    for (char& c : s) {
        if (c == '/') c = '\\';
        else c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    const std::size_t slash = s.find_last_of('\\');
    std::string base = (slash == std::string::npos) ? s : s.substr(slash + 1);
    const std::size_t dot = base.find_last_of('.');
    if (dot != std::string::npos) base = base.substr(0, dot);
    return base == "smoke" || base == "twinkgr";
}

const ImageSequence* body_sequence(const MapResource& r) {
    for (const ImageSequence& entry : r.sequences) {
        for (const ImageSequence* s : image_sequence_chain(entry)) {
            if (s->library.empty()) continue;
            if (is_effect_library(s->library)) continue;
            return s;
        }
    }
    return nullptr;
}

int object_anim_phase(int x, int y) {
    // AoWE.TStructureResource.Show@557605dc / HSEngine.TMapObject.Show@55609a00:
    // `(int)*(char *)(field+0x10) + *(char *)(field+0x11) * 0x10`.
    return x + y * 0x10;
}

// ---------------------------------------------------------------------------
// The water/ice hexagon's animation (aow/map_resources.hpp)
// ---------------------------------------------------------------------------
namespace {

// The engine takes three of a water hexagon's phases from object *addresses*
// (`field >> 3` in Show@5579aeac, `this >> 8` in the shelf draw) or from the
// map's RNG (`RandInt` at Create / on the roll), so they carry the runtime's
// allocation and seed - neither is in the .HSM. This mixes the hex coordinates
// into a stable 32-bit value instead: deterministically the same on every run
// and every machine, spatially decorrelated (so neighbouring tiles differ the
// way the engine's addresses do), and it never repeats per hex the way a plain
// `x + y` would.
std::uint32_t water_hash(int x, int y, std::uint32_t salt) {
    std::uint32_t h = static_cast<std::uint32_t>(x) * 0x9e3779b1u ^
                      static_cast<std::uint32_t>(y) * 0x85ebca6bu ^
                      salt * 0xc2b2ae35u;
    h ^= h >> 15;
    h *= 0x2545f491u;
    h ^= h >> 13;
    return h;
}

int water_rand(int x, int y, std::uint32_t salt, int n) {
    if (n <= 0) return 0;
    return static_cast<int>(water_hash(x, y, salt) %
                            static_cast<std::uint32_t>(n));
}

}  // namespace

bool water_hex_animates(const WaterAnimOptions& opt, bool variant_present) {
    // `if (engine+0x6d != 0 && (sprite+0x1a != 0 || <level flag> == 0))`.
    return opt.dynamic && (variant_present || opt.level_anim);
}

WaterHexAnim water_hex_create(int x, int y) {
    // AoWHex.TAoWWaterHexagon.Create@5579ad40:
    //   sprite+0x18 = -1 - RandInt(0x14);   sprite+0x15 |= RandInt(4) << 6
    WaterHexAnim st;
    st.variant = water_hex_variant(x, y);
    st.counter = -1 - water_rand(x, y, 0x4004u, 0x14);
    st.index = 0;
    st.rolls = 0;
    return st;
}

int water_sequence_roll(int x, int y, bool open_water, int roll_salt,
                        const WaterAnimOptions& opt) {
    // ShowDynamic@5579afb4's roll. Each `RandInt` call site gets its own salt so
    // the stand-in values are independent, and the salt carries the hex's roll
    // ordinal so successive rolls on one hex differ.
    const std::uint32_t s = 0x5000u + static_cast<std::uint32_t>(roll_salt % 64) * 8u;
    if (!opt.wide_range) {
        if (open_water && water_rand(x, y, s + 1u, 700) == 0 && opt.effects_ok) {
            return 0x1e + water_rand(x, y, s + 2u, 10);
        }
        return 0x14 + water_rand(x, y, s + 3u, 10);
    }
    if (open_water && water_rand(x, y, s + 4u, 300) == 0 && opt.effects_ok) {
        return 0x1e + water_rand(x, y, s + 5u, 10);
    }
    if (water_rand(x, y, s + 6u, 10) < 3) {
        return 0x32 + water_rand(x, y, s + 7u, 9);
    }
    return 0x14 + water_rand(x, y, s + 3u, 10);
}

WaterTick water_hex_advance(WaterHexAnim& st, int x, int y, bool open_water,
                            bool variant_present, const WaterAnimOptions& opt) {
    WaterTick out;
    if (!water_hex_animates(opt, variant_present)) return out;
    // `sprite+0x18 = sprite+0x18 + 1; if (-1 < sprite+0x18) { ... }`
    ++st.counter;
    if (st.counter < 0) return out;
    if (st.counter == 0) {
        // The index is re-rolled on the frame the counter reaches exactly 0.
        st.index = water_sequence_roll(x, y, open_water, st.rolls, opt);
        ++st.rolls;
    }
    out.index = st.index;
    out.counter = st.counter;
    return out;
}

void water_hex_finish(WaterHexAnim& st, int x, int y, bool drew,
                      const WaterAnimOptions& opt) {
    if (drew) return;
    if (st.counter < 0) return;  // still asleep - the engine never resets here
    // `sprite+0x18 = -(engine+0x6d == 1 ? 0x14 : 10) - RandInt(0x19)`
    const int base = (opt.engine_mode == 1) ? 0x14 : 10;
    const std::uint32_t salt =
        0x6000u + static_cast<std::uint32_t>(st.rolls % 64) * 4u;
    st.counter = -(base + water_rand(x, y, salt, 0x19));
}

int water_hex_variant(int x, int y) {
    return water_rand(x, y, 0x1001u, 4);
}

int water_base_phase(int x, int y) {
    return water_rand(x, y, 0x2002u, 0x10);
}

int water_shore_variant(int x, int y) {
    // `uVar6 = this >> 8 & 3; if (uVar6 == 3) uVar6 = 0;`
    const int v = water_rand(x, y, 0x3003u, 4);
    return v == 3 ? 0 : v;
}

int water_base_frame(int x, int y) {
    // Show@5579aeac:
    //   frame = ((field>>3) + x + y*2 + (sprite+0x15>>6)) & 0xf + 0x8c
    const int f = (water_base_phase(x, y) + x + y * 2 + water_hex_variant(x, y));
    return (f & 0xf) + 0x8c;
}

std::vector<std::string> race_image_libraries(
    const std::vector<MapResource>& list) {
    std::vector<std::size_t> idx;
    for (const MapResource& r : list) {
        if (r.is_race) idx.push_back(r.res_index);
    }
    std::size_t maxi = 0;
    for (std::size_t i : idx) {
        if (i > maxi) maxi = i;
    }
    std::vector<std::string> out;
    if (idx.empty()) return out;
    out.assign(maxi + 1, std::string());
    for (const MapResource& r : list) {
        if (!r.is_race) continue;
        if (r.res_index < out.size()) out[r.res_index] = r.ilb;
    }
    return out;
}

// ---------------------------------------------------------------------------
// The owner flag of a takeable structure (docs/19_STRUCTURE_FLAGS.md).
// ---------------------------------------------------------------------------

namespace {

const PlayerStructureResourceInfo kPsrClasses[] = {
    // The abstract bases (never serialised themselves, kept for completeness).
    {0x2034d, true,  true},   // AoWE.TPlayerStructureResource
    {0x2034f, true,  true},   // Crops.TPlayerCropStructureResource
    {0x203bb, true,  false},  // PrdPlace.TProductionPlaceResource
    {0x20823, true,  false},  // RandomNode.TRandomNodeResource
    // Concrete resources.
    {0x20109, true,  true},   // City.TCityResource            (keeps +0x64)
    {0x2036f, true,  false},  // Farm.TFarmResource
    {0x2037b, false, false},  // Mine.TMineResource            +0x44 = 0
    {0x2043f, true,  false},  // Tower.TTowerResource          +0x44 = 1
    {0x2019b, false, false},  // PowerNode.TPowerNodeResource  +0x44 = 0
    {0x2063f, true,  false},  // ReflectingPool.TReflectingPoolResource +0x44 = 1
    {0x204bf, false, false},  // Shipyard.TShipyardResource    +0x44 = 0
    {0x2092f, true,  false},  // BldGuild.TBuildersGuildResource +0x44 = 1
    {0x204b4, false, false},  // WaterNode.TWaterNodeResource  +0x44 = 0
    {0x204af, false, false},  // DeathNode.TDeathNodeResource  +0x44 = 0
    {0x204cf, true,  false},  // EarthNode.TEarthNodeResource  (no override)
    {0x203df, true,  false},  // FireNode.TFireNodeResource    (no override)
    {0x20372, false, false},  // LifeNode.TLifeNodeResource    +0x44 = 0
    {0x2094f, false, false},  // AirNode.TAirNodeResource      +0x44 = 0
    {0x203f2, true,  false},  // Altar.TAltarResource
    {0x20234, true,  false},  // AltLife.TAltarOfLifeResource
    {0x20236, true,  false},  // AltDeath.TAltarOfDeathResource
    {0x2023b, true,  false},  // AltFire.TAltarOfFireResource
    {0x2023e, true,  false},  // AltLight.TAltarOfLightningResource
    {0x2024c, true,  false},  // AltIce.TAltarOfIceResource
    {0x2030d, true,  false},  // AltPest.TAltarOfPestilenceResource
};

const PlayerStructureResourceInfo* psr_info(std::uint32_t class_id) {
    for (const PlayerStructureResourceInfo& i : kPsrClasses) {
        if (i.class_id == class_id) return &i;
    }
    return nullptr;
}

}  // namespace

bool is_player_structure_resource(std::uint32_t class_id) {
    return psr_info(class_id) != nullptr;
}

int structure_pad_index(const MapResource& r) {
    // CalculatePadIndex@55760524: `for i in 0..count-1: if GetTerrain(list, i)
    // >= 0 (signed) then result++`. GetTerrain returns the entry's low byte.
    int n = 0;
    for (std::uint16_t e : r.terrain_list) {
        if (static_cast<std::int8_t>(e & 0xff) >= 0) ++n;
    }
    return n;
}

void player_structure_flag_anchor(const MapResource& r, int& dx, int& dy) {
    // TPlayerStructureResource.Create@55761974's defaults.
    dx = 0x14;
    dy = -0x23;
    if (!r.flag_pad_anchored) return;  // GenerateFlagPositions does nothing
    switch (structure_pad_index(r)) {
        case 1: dx = 0x14; dy = -0x1e; break;
        case 2: dx = 0x23; dy = -0x32; break;
        case 3: dx = 0x05; dy = -0x2d; break;
        case 4: dx = 0x1e; dy = -0x3c; break;
        default: break;  // 0 (and anything >4) keeps the Create default
    }
}

int structure_flag_step(int clock, int hex_pixel_x, bool city_style) {
    const int v = clock + hex_pixel_x;
    if (city_style) return (v >> 2) % 12;
    return (v >> 1) % 12;
}


}  // namespace aow
