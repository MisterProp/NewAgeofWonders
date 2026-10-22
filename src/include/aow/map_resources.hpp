// map_resources.hpp - the world map-object/terrain resource table (Stage 2.2).
//
// The world map's terrain and object *sprites* are looked up by a 32-bit
// resource id: `HSEngine.TMapObject.ReadWrite` (HSEPack 0x55609824) serialises
// the sprite's `tag 1` = `TMapObject.GetResourceID`, and on read it is resolved
// against the map's `TMapObjectResourceList` (raising `EMapObjectResourceMissing`
// when absent).
//
// That list lives in `Release.hss` (the game pack): the top object is a
// `TAoWWHSSet` whose **tag 5** is the `TMapObjectResourceList` (`THSSet+0x24`,
// see `THSSet.ReadWrite` HSEPack 0x55609280). Its child table holds ~1142
// `TMapObjectResource` subclasses, each `[u32 class id][table][data]` with
// **tag 1 = the resource id** and an embedded image-library name (e.g.
// `EARTHHEX.ILB`, `H_Border.BMP`, `WATERHEX.ILB`).
//
// A map tile's terrain hexagon (`MapHs::resource`) is exactly such an id, so
// `load_map_resources` lets the port resolve a tile to its ILB frame.
//
// Reference decompiles: re/exports/hsepack/HSEngine.TMapObject.ReadWrite.c,
// HSEngine.THSSet.ReadWrite.c, HSEngine.TMapObjectResource.ReadWrite.c.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "aow/unit_anim.hpp"  // ImageSequence (TImageSequence)

namespace aow {

// One `TMapObjectResource` from Release.hss's resource list.
//
// Placement fields (tags 12..15) exist only on the *multi-hex* subclasses
// (`HSEngine.TMultiHexMOR`, e.g. `Mountain.TMountainMOR` 0x20169 and
// `Terrain.TTerrainMOR` 0x20167). Evidence:
//   re/exports/hsepack/HSEngine.TMultiHexMOR.ReadWrite.c reads four ints at
//   +0x34/+0x38/+0x3c/+0x40 with tags 12/13/14/15; `TMultiHexMOR.SetdXmainP`
//   and `SetdYmainP` write +0x34/+0x38, and `HSEngine.TMultiHexMO.Show.c` draws
//   the sprite at `HXtoHP(hex) + resource[+0x34] / + resource[+0x38]` - i.e.
//   tags 12/13 are the sprite's own `dXmainP`/`dYmainP` pixel shifts (negative:
//   the sprite's visual top-left sits up/left of the hex the object is on).
//   The remaining two (tags 14/15) are not yet identified [TODO(RE)].
struct MapResource {
    std::uint32_t id = 0;        // tag 1 - the resource id (matched by sprites)
    std::uint32_t class_id = 0;  // Engine class id (e.g. 0x2073e water hexagon)
    std::uint32_t res_type = 0;  // tag 2 (TEResource type code)
    std::uint32_t res_index = 0;  // tag 3 (TEResource index)
    // tag 7 = TTerrainList (resource+0x28). Its first entry is a u16
    // `terrain | overlay << 8`; `GetTerrain(list, 0)` returns the low byte and
    // `GetOverlay(list, 0)` (`HSEngine.TTerrainList.GetOverlay@556050c8`) the
    // high byte. The terrain type the transition logic compares is 0 water,
    // 1..5 = grass/desert/snow/steppe/wasteland, 6 ice, ... (see
    // docs/15_MOVEMENT.md). `has_terrain` is false when the record carries no
    // tag 7 (then terrain = -1 and the caller falls back to guessing from the
    // library name). Evidence:
    // re/exports/move/HSEPack_dpl/0x55605054.c + 0x556050c8.c.
    int terrain = -1;
    // The overlay code (0 = mountains, 1 = forest/dense vegetation, 2 = hill,
    // 3 = scrub, 4 = road/city, 5 = bridge, 6 = rubble, 8 = bones, 9 = vegetation).
    // A plain hexagon ships 0xFE ("no overlay"): `TMapObject.GetOverlay@556097b8` /
    // `TMultiHexMO.OverrideTerrain@5560b2d4` treat the byte as a *signed char* and
    // only write it when it is `>= 0`, so 0x80..0xFF means "no opinion" and the
    // port stores -1. `aow::move_point_cost` maps the code to the cost table's
    // column `code + 1` (column 0 = no overlay) - see docs/15_MOVEMENT.md §2.
    int overlay = -1;
    bool has_terrain = false;
    // The raw TTerrainList entries (u16; low byte = terrain, high byte = overlay)
    // exactly as serialised in rw tag 7. Entry i is the terrain/overlay for the
    // object's i-th hex (`TMapObject.GetTerrain(i)` / GetOverlay(i)); entry 0 is
    // the object's own hex. Kept so callers can apply the engine's per-hex rule
    // (a multi-hex object covers several fields) instead of only entry 0.
    std::vector<std::uint16_t> terrain_list;
    std::int32_t dx_main = 0;    // tag 12 - TMultiHexMOR dXmainP (sprite x shift)
    std::int32_t dy_main = 0;    // tag 13 - TMultiHexMOR dYmainP (sprite y shift)
    bool has_shift = false;      // true when tags 12/13 were present
    std::string ilb;             // first embedded image-library name
    std::vector<std::string> ilbs;  // all embedded library names
    std::size_t offset = 0;      // region offset of the record (evidence)
    // Some resources carry their image library INLINE rather than naming a file:
    // TMapObjectResource rw **tag 8** holds a complete ILB image (magic
    // 04 'I' 'L' 'B', version 4.0) - e.g. the grass-tree resources whose only
    // string is the logical label "gr_tree.BMP" (no such file/entry exists on
    // disk, and TILManagerList.LinkToIL resolves names by exact path only, so the
    // engine cannot be loading those from a file). Decoded with IlbFile.
    std::vector<std::uint8_t> ilb_data;
    // Image-sequence list (rw tags 9/10, `ImageLib.TImageSequenceList.
    // LoadFromStream@0x552190d8`; rw tag 9 is its "has list" byte, tag 10 the
    // list). Every entry is a `TImageSequence` (library name, period, mode, base
    // frame, xoff/yoff, optional frame table) that may chain one further sequence
    // (`+0xc`). `HSEngine.TMapObjectResource.ShowComplexImage@5560a134` (and the
    // sequence branch of `AoWE.TStructureResource.Show@557605dc`) walks that chain
    // and draws every sequence whose `TImageSequence.GetFrameIndex@0x552186ec` is
    // valid, at `HXtoHP + seq xoff/yoff + sprite base+shift`.
    std::vector<ImageSequence> sequences;
    // The sequence the world-map renderer draws, i.e. `body_sequence(*this)` -
    // see below. A structure's list can *start* with an additive-effect sequence
    // (`SMOKE.ILB` for Shipyard.TShipyardResource, `EFFECTS\DESTR.ILB` for razing)
    // chained in front of the body; those cannot be composited by a static
    // renderer, so `body_sequence` skips them and `has_seq` / `seq_lib` /
    // `seq_frame` / `seq_dx` / `seq_dy` describe the *body* that is drawn. This is
    // how an object picks its own frame - e.g. the three grass "mountain"
    // resources are MOUNTAIN.ILB frames 26 / 8 / 9 (26 is the 60x50 HILL mound),
    // not the block+size rule the renderer used to apply.
    bool has_seq = false;
    std::string seq_lib;
    std::size_t seq_frame = 0;
    std::int32_t seq_dx = 0;
    std::int32_t seq_dy = 0;
    // True for an `AoWE.TRaceResource` (class 0x20201) read from the game pack's
    // TRaceResourceList (TAoWWHSSet rw tag 0x10, +0x54). Its `ilb` is the race's
    // image library (`RACES\RHUMAN.ILB`, ...) and `res_index` is the list
    // position, i.e. the race/faction index a city's rw tag 0xd refers to.
    // Evidence: re/exports/aowepack/AoWE.TAoWHSSet.{ReadWrite,Create}.c and
    // AoWE.TRaceResource.{ReadWrite,LinkToIL,GetCityImage}.c.
    bool is_race = false;

    // --- The owner flag a TPlayerStructure draws ---------------------------
    // `AoWE.TFlagControl` (AoWEPACK 0x5575a10c) is a small control that owns
    // `Flags.ILB`; a placed `AoWE.TPlayerStructure` draws its *pole* plus a
    // 12-frame flag from the owner's race library. The geometry and the
    // "which flag" decision come from fields on the *resource* that
    // `TStructureResource.Create` / `CalculatePadIndex` /
    // `TPlayerStructureResource.Create` / `GenerateFlagPositions` fill in at
    // runtime, so the port bakes each resource class's `Create` defaults in
    // here. See docs/19_STRUCTURE_FLAGS.md.
    bool is_player_structure = false;  // class is a TPlayerStructureResource subclass
    // resource+0x44: `TStructureResource.Create` sets 1 and `GenerateFlagPositions`
    // re-anchors the flag only while it is non-zero. The subclasses that clear it
    // (Mine, PowerNode, Shipyard, Water/Death/Life/AirNode) or set it
    // (BuildersGuild, Tower, ReflectingPool, and the rest by inheritance) are
    // tabulated in `kPsrClasses` (src/engine/map_resources.cpp).
    bool flag_pad_anchored = false;
    // resource+0x64: `TPlayerStructureResource.Create` sets 1 and every subclass
    // besides `City.TCityResource` clears it, so only a city that nobody owns
    // still draws the *neutral* flag (`flag id 0x20`); an unowned mine/tower/node
    // draws none (`0x3f`). `TPlayerStructure.UpdateFlagID@55760ed8`.
    bool flag_neutral_when_unowned = false;
};

// Loads the resource list from `Release.hss` (the game pack). Returns false and
// sets `error` on failure.
bool load_map_resources(const std::string& path,
                        std::vector<MapResource>& out, std::string& error);

// Loads the tactical-combat resource table from `Release\comb_res.pfs`. The map
// fields of a `TCMaps\*.HSM` reference these records by their rw tag-1 id; the
// record's image sequence list names the `Images\TCombat\...` ILB to draw.
// Returns false and sets `error` on failure.
bool load_combat_resources(const std::string& path,
                           std::vector<MapResource>& out, std::string& error);

// Looks up a resource id; returns nullptr when absent. Race resources
// (`is_race`) are skipped - their ids live in a separate namespace.
const MapResource* find_map_resource(const std::vector<MapResource>& list,
                                     std::uint32_t id);

// The race image libraries (`RACES\RHUMAN.ILB`, ...) ordered by faction index,
// taken from the `is_race` entries (res_index = position). Entry i is the
// library a city of race i draws its body from - the city's rw tag 0xd - see
// AoWE.TRaceResource.GetCityImage. Empty until load_map_resources has run.
std::vector<std::string> race_image_libraries(
    const std::vector<MapResource>& list);

// True when an image library names an *additive effect* the static world-map
// renderer cannot composite (the engine draws those through a blend mode, so an
// opaque blit leaves a dark square):
//   * `SMOKE.ILB` - the burning / production puff a structure's sequence chain
//     starts with (Shipyard, Tower, Wizard's Tower, ...);
//   * `EFFECTS\TWINKGR.ILB` - the "twinkle" a node/crop draws.
// A structure's body is the *next* sequence of the chain, which is exactly what
// `AoWE.TStructureResource.Show@557605dc` reaches once the effect's frames no
// longer apply - but a static render has no clock, so we skip them by name.
bool is_effect_library(const std::string& library);

// The `TImageSequence` the static world-map renderer draws for `r`: the first
// sequence in its image-sequence list - entries in list order, each entry
// followed by its `chain` - whose library is a real body (`!is_effect_library`).
// Returns nullptr when `r` has no sequence list or only effect sequences. The
// xoff/yoff of the returned sequence ground the sprite (`HXtoHP + xoff/yoff`).
const ImageSequence* body_sequence(const MapResource& r);

// ---------------------------------------------------------------------------
// The owner flag of a "takeable" (ownable) structure - `AoWE.TPlayerStructure`
// and its resource `TPlayerStructureResource`. See docs/19_STRUCTURE_FLAGS.md.
// ---------------------------------------------------------------------------

// True when `class_id` is `AoWE.TPlayerStructureResource` (0x2034d) or one of its
// subclasses. The tree was read out of the module's Delphi VMTs (`vmt-36` holds
// `parentVmt-76`) and cross-checked against each class's ClassID getter; see
// `kPsrClasses` in src/engine/map_resources.cpp. A resource that is not one of
// these belongs to a plain `TStructure` (Wizard's Tower, Cave, Ruin, ...) and
// never draws an owner flag.
bool is_player_structure_resource(std::uint32_t class_id);

// `AoWE.TStructureResource.CalculatePadIndex@55760524`: the value the engine keeps
// in resource **+0x46**. It walks the resource's `TTerrainList` and counts the
// entries whose terrain byte is `>= 0` as a signed char (`TTerrainList.GetTerrain
// @55605054` returns the entry's *low* byte, and a hexagon that has no terrain
// ships 0xFE). The value is therefore "how many hexes this resource covers with a
// terrain", and it doubles as
//   * the flag-position style `GenerateFlagPositions` switches on (1..4), and
//   * the flag-library switch: `Show` tests `resource[0x46] < 2` to choose the
//     single-hex *city-style* flag (a fixed flagpole, race frames 12..23) over
//     the multi-hex *structure-style* one (flagpole `flagbyte>>6`, frames 0..11).
int structure_pad_index(const MapResource& r);

// `AoWE.TPlayerStructureResource.GenerateFlagPositions@557619e8`, with the
// defaults `TPlayerStructureResource.Create@55761974` installs: the anchor the
// owner flag is drawn at, *relative to the object's HXtoHP* (`resource+0x68` /
// `+0x6c`).
//
//      default (Create)            (+0x14, -0x23)
//      pad index 1                 (+0x14, -0x1e)
//      pad index 2                 (+0x23, -0x32)
//      pad index 3                 (+0x05, -0x2d)
//      pad index 4                 (+0x1e, -0x3c)
//
// `GenerateFlagPositions` only runs when `resource+0x44` is non-zero, so a Mine
// (`+0x44 == 0`) keeps the Create default while a Tower (`+0x44 == 1`) takes the
// pad style above. The flag *sprite* is then drawn 0x15 px left / 0x10 px up of
// that anchor (see `kStructureFlagSpriteDx/Dy`).
void player_structure_flag_anchor(const MapResource& r, int& dx, int& dy);

// The flag sprite's offset from the pole anchor: `TFlagControl.ShowFlag` /
// `ShowStructureFlag` both blit the flag at `(x - 0x15, y - 0x10)` while the pole
// goes at `(x, y)`.
constexpr int kStructureFlagSpriteDx = -0x15;
constexpr int kStructureFlagSpriteDy = -0x10;

// The `Flags.ILB` frame a flagpole uses: `TFlagControl.ShowFlag@5575a1e8` always
// draws frame 4 (the single-hex/city pole), while `ShowStructureFlag@5575a34c`
// draws `flagbyte >> 6` (the pole variant the flag byte selects; 0..3).
constexpr std::size_t kFlagPoleFrameCity = 4;

// The `Flags.ILB` frame the *neutral* flag starts at: `ShowFlag` uses
// `frame + 0x16` and `ShowStructureFlag` `frame + 0x0a`, where `frame` is the
// 0..11 animation step below.
constexpr std::size_t kNeutralFlagFrameCity = 0x16;
constexpr std::size_t kNeutralFlagFrameStructure = 0x0a;

// The race library frame the *owned* flag starts at: `ShowFlag` uses `frame + 0xc`
// (12..23) and `ShowStructureFlag` `frame + 0` (0..11).
constexpr std::size_t kOwnedFlagFrameCity = 0x0c;
constexpr std::size_t kOwnedFlagFrameStructure = 0x00;

// The 0..11 animation step of a flag. `TPlayerStructure.Show@55760f94` /
// `TCity.Show@557aca88` hand the flag control `clock = map_clock + HXtoHP.x`
// (the map's own `+0x40` frame counter, `HSEngine.THSMap.NewFrame@5560d758`,
// added to the hex's *pixel* x - so neighbouring columns are out of phase), and
//   * `ShowFlag@5575a1e8`          resolves `(clock >> 2) % 12`;
//   * `ShowStructureFlag@5575a34c` resolves `(clock >> 1) % 12`.
// `city_style` selects between the two (resource+0x46 < 2 -> ShowFlag).
int structure_flag_step(int clock, int hex_pixel_x, bool city_style);


// The per-object animation phase `ImageLib.TImageSequence.GetFrameIndex`
// (`ILPACK.dpl 0x552186ec`) is handed for a world-map object. The engine computes
// it from the object's hex:
//   * `AoWE.TStructureResource.Show@557605dc` (the structure path) passes
//     `(int)*(char *)(MO.field+0x10) + *(char *)(MO.field+0x11) * 0x10`, i.e.
//     `hex_x + hex_y*0x10`;
//   * `HSEngine.TMapObject.Show@55609a00` computes exactly the same expression
//     for a plain object;
//   * `HSEngine.TMultiHexMO.Show@5560aaec` passes the `HXtoHP` *pixel* sum
//     (`px + py`) instead.
// Either way the value varies per hex, so neighbouring buildings do not animate
// in lockstep. `phase` is added to the sequence's own frame counter inside
// `GetFrameIndex` (and to the mode-4/5 table index).
int object_anim_phase(int x, int y);

// ---------------------------------------------------------------------------
// The water/ice hexagon's own animation (`AoWHex.TAoWWaterHexagon`, the map
// object class 0x2073d / resource 0x2073e).
//
// A water hexagon is *two* things at once. The static pass
// (`AoWHex.TAoWWaterHexagon.Show@5579aeac`) draws its base tile and, where a
// raised/border neighbour has a different terrain, three `T_Ic-Wa2` foam
// sprites. The dynamic pass (`ShowDynamic@5579afb4`), which the engine runs
// once per map frame, drives a per-hex counter that plays one *one-shot*
// image-sequence of the resource's list - the water ripples, the fish and the
// sparkle effects - and then draws the shoreline shelf
// (`IsoHex.TLowerIsometricHexagon.ShowDynamic@5561a400`).
//
// The resource's list (id 0x007b134f, 59 entries; `tools/structseq_probe.py`):
//   [ 0] mode 0 static, WATERHEX.ILB      entry 0's library is the hexagon's
//                                         base + shelf art
//   [ 1..19] NULL placeholders
//   [20..29] mode 4 per 2, WATERANM.ILB   the five wave/ripple frame tables,
//                                         each shipped twice
//   [30..40] mode 4 per 0/1 chains, WATERANM.ILB   a fish (`Fish1-*`) chained to
//                                         its splashes (`FishSplas1-*`)
//   [50..58] mode 4 per 0/1, TCOMBAT\OBJECTS\SEFFECT.ILB   the sparkles
//
// `Create@5579ad40` seeds the hexagon: the counter `+0x18` starts just below
// zero (so the first ripple is delayed) and `+0x15` bits 6/7 get `RandInt(4)`
// (the base tile's and the shelf's phase).
//
// The engine seeds those values from the map's own RNG and phases them off the
// sprites' *addresses* (`field >> 3` in Show, `this >> 8` in the shelf draw) -
// neither is recoverable from the map file (the .HSM does not store the RNG
// seed, and an address is an allocation artefact). The port therefore stands in
// a deterministic positional value for each of them, exactly as it already does
// for the engine's `RandInt` in the terrain/building animation. Everything else
// - the counter's cadence, the ranges rolled, the sequence/frame resolution,
// the anchors and the blend - is the engine's rule verbatim.
// ---------------------------------------------------------------------------

// The flags `ShowDynamic` tests before it animates anything.
//
//   if (engine+0x6d != 0 && (sprite+0x1a != 0 ||
//                            *(char *)(*(int *)(*(int *)(field+4)+0x1c)+200) == 0)) {
//
// `engine+0x6d` is the engine's mode byte (the title screen reads 2 for its orb
// overlay and the layout routine writes 1/3), so a running world map is 1:
// that is also the value that selects the *longer* sleep. The two remaining
// terms are not identified [TODO(RE)]: `sprite+0x1a` is the per-direction
// shoreline variant byte (0x28 for a snow neighbour, 0x50 for wasteland - see
// the water resource's `UpdateTransitions@5579ad98`), and the other is a byte
// on the level object. Both are modelled as explicit inputs with the values a
// normal surface map produces.
struct WaterAnimOptions {
    bool dynamic = true;   // engine+0x6d != 0 - the dynamic pass is running
    int engine_mode = 1;   // engine+0x6d (1 = the world map -> the longer sleep)
    // ((map[+0x188] -> +0x10) & 0x10) != 0: takes the second roll branch, which
    // also rolls the 30 % TCOMBAT\OBJECTS\SEFFECT.ILB sparkle.
    bool wide_range = false;
    bool effects_ok = true;  // (map+0x3c & 2) == 0 (the extra rolls are allowed)
    // The `<<level flag>> == 0` term of the gate above [TODO(RE)].
    bool level_anim = true;
};

// The state the engine keeps on the hexagon sprite.
struct WaterHexAnim {
    int variant = 0;  // +0x15 >> 6  the RandInt(4) set at Create (0..3)
    int counter = 0;  // +0x18       +1 per map frame, negative while asleep
    int index = 0;    // +0x19       the sequence-list entry being played
    // The port's own roll ordinal, used only to decorrelate the successive
    // stand-ins for the engine's `RandInt` calls on one hex.
    int rolls = 0;
};

// `ShowDynamic`'s gate. `variant_present` is `sprite+0x1a != 0`.
bool water_hex_animates(const WaterAnimOptions& opt, bool variant_present);

// `AoWHex.TAoWWaterHexagon.Create@5579ad40`:
//   counter = -1 - RandInt(0x14);  variant = RandInt(4)
WaterHexAnim water_hex_create(int x, int y);

// The `sprite+0x19` roll `ShowDynamic` makes on the frame the counter reaches 0.
// `open_water` is `sprite+0x14 == 0` (a water hex with no flat-land neighbour),
// `roll_salt` the hex's roll ordinal (see WaterHexAnim::rolls).
//
//   branch A: RandInt(700) == 0 && open_water && effects_ok -> 0x1e + RandInt(10)
//             otherwise                                     -> 0x14 + RandInt(10)
//   branch B: RandInt(300) == 0 && open_water && effects_ok -> 0x1e + RandInt(10)
//             RandInt(10) < 3                               -> 0x32 + RandInt(9)
//             otherwise                                     -> 0x14 + RandInt(10)
//
// 0x14..0x1d are the ten WATERANM ripple sequences, 0x1e..0x27 the fish chains
// and 0x32..0x3a the SEFFECT sparkles.
int water_sequence_roll(int x, int y, bool open_water, int roll_salt,
                        const WaterAnimOptions& opt);

// One map frame of `ShowDynamic`: advance the counter and report what to draw.
// `index` is -1 when this frame draws nothing (the counter is still negative).
// Otherwise `index` selects the entry of the resource's image-sequence list
// whose *whole chain* is drawn, each sequence framed by
// `image_sequence_unlooped_frame(seq, frames, counter)`.
struct WaterTick {
    int index = -1;
    int counter = 0;
};
WaterTick water_hex_advance(WaterHexAnim& st, int x, int y, bool open_water,
                            bool variant_present, const WaterAnimOptions& opt);

// The engine's `if (!bVar2) sprite+0x18 = -base - RandInt(0x19)` - the sleep
// after the sequence ran out (or the selected entry does not exist). `drew` is
// whether any sequence of the chain produced a frame this tick; the base is
// 0x14 when `engine_mode == 1` and 10 otherwise, so the ripple reappears after
// 20..44 (or 10..34) more map frames. Does nothing while the counter is still
// negative.
void water_hex_finish(WaterHexAnim& st, int x, int y, bool drew,
                      const WaterAnimOptions& opt);

// The `RandInt(4)` `Create` stores (0..3) - the base tile's phase as well as
// the shelf's.
int water_hex_variant(int x, int y);
// `Show`'s `field >> 3` term, 0..15.
int water_base_phase(int x, int y);
// The shelf draw's `this >> 8 & 3` with the engine's `3 -> 0`, so 0..2.
int water_shore_variant(int x, int y);
// `AoWHex.TAoWWaterHexagon.Show@5579aeac`: the base tile is
//   ((field >> 3) + hex_x + hex_y*2 + (sprite+0x15 >> 6)) & 0xf + 0x8c
// out of the hexagon's library - WATERHEX.ILB's sixteen `H_Wa2.BMP` wave
// frames. Returns the library slot id; the caller falls back to the stored
// frame when the library has no such run (ICEHEX.ILB).
int water_base_frame(int x, int y);

// The three `T_Ic-Wa2` foam slots `Show` blits where this hexagon borders a
// raised/border hexagon of a *different* terrain (mask bits 0, 1 and 0x20 of
// `sprite+0x15`, set by `UpdateTransitions@5579ad98`). The anchor is the base
// tile's + (kWaterFoamDx, kWaterFoamDy).
constexpr int kWaterFoamBit0Frame = 0x24;
constexpr int kWaterFoamBit1Frame = 0x25;
constexpr int kWaterFoamBit5Frame = 0x23;
constexpr int kWaterFoamDx = -6;
constexpr int kWaterFoamDy = -9;

}  // namespace aow
