// mapviz.hpp - renders a decoded map to a PNG (Stage 2.2).
//
// Each tile's hexagon sprites carry a resource id (see aow/map_resources.hpp);
// the matching `TMapObjectResource` names an ILB library (e.g. `H_gr.BMP` ->
// `Images/TCombat/Base_hex/H_Gr.ILB`). The tiles are 48x32 hexagons (45-degree
// flanks, 16px flat top) drawn on the engine's axis-aligned, column-staggered
// hex grid - HSEngine.HXtoHP: `px = x*32 + 8`, `py = y*32 + (x & 1)*16`. The
// sprites are ILB class 0x11, 16-bit RLE - decoded by src/engine/ilb.cpp.
//
// `render_map_png` resolves every sprite, blits the terrain hexagons, then
// overlays the object sprites. Sprites whose ILB cannot be located fall back to
// a flat colour keyed by terrain type.
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/movement.hpp"
#include "aow/unit_catalog.hpp"

namespace aow {

// The engine's hex grid (HSEngine.HXtoHP@5560e3b4): one hex step is 32 px right,
// 32 px down, and an odd *column* starts 16 px lower. `hex_origin` maps a hex to
// its sprite-box top-left inside a rendered canvas (which already carries the
// engine's +8 field margin, see kHexFieldMargin).
constexpr int kHexPitchX = 32;
constexpr int kHexPitchY = 32;
constexpr int kHexStagger = 16;
void hex_origin(int x, int y, int& sx, int& sy);

// mapviz draws a hexagon's top-left at its `origin()` (see render_map_rgba:
// `field_x + x * 32`), which is exactly HXtoHP. The engine's single-hex paths
// subtract another 8 (`Hexagon.THexagon.Show@55611d60`,
// HSEngine.TMapObject.Show@55609a00) while its unit draws use HXtoHP itself, so
// the port's terrain/objects sit `kHexFieldMargin` px right of the engine's
// *relative to the units* - a known pre-existing offset, quantified against the
// original game in docs/16_UNIT_ANIM.md section 10, not a cancellation.
constexpr int kHexFieldMargin = 8;

// A road overlay's draw anchor. Road.TRoad.Show@55616ed4 converts the road's
// hex with HXtoHP and blits the overlay at `(HXtoHP.x - 14, HXtoHP.y - 9)`; the
// sprite's own base+shift is then added inside the image's Show, exactly like a
// normal object. mapviz's hex origin IS HXtoHP (HSEngine.HXtoHP@5560e3b4 returns
// `x*0x20 + 8`, `y*0x20 + (x & 1)*0x10`), so a road sprite's top-left =
// `origin + (kRoadAnchorDx + vx, kRoadAnchorDy + vy)`.
constexpr int kRoadAnchorDx = -14;
constexpr int kRoadAnchorDy = -9;

// Engine class id of a placed road object (Road.TRoad.ClassID@55616ecc returns
// 0x20164). The matching *resource* is class 0x20165 Road.TRoadResource, whose
// rw tag 8 carries the road sprite library INLINE (entries named `Rd_*.BMP`).
constexpr std::uint32_t kRoadClassId = 0x20164;

// The road-overlay transition byte for the road on hex (x,y), exactly as
// re/exports/hsepack/Road.TRoad.UpdateTransitions.c computes it: bit i (i=0..5)
// is set when the neighbour reached by direction i+1 is *road-connected*, then
// the E/W pair (bits 2|4) and the NE/NW pair (bits 1|5) collapse onto the
// dedicated "straight" bits 6 (0x40) and 7 (0x80). Direction 1..6 -> (dx,dy)
// comes from HSEngine.HNtoHX@5560e08c with the column-stagger fold of
// HSEngine.TranslateHX@509C8955 (an odd column with odd dx moves one row up).
//
// `connected` is a `width * height` grid, non-zero where a road object sits OR a
// road-connectable *structure* sits. The engine's `Road.TRoad.UpdateTransitions`
// sets a neighbour's bit when `FindOwnedHS(nfield, TAbstractRoad)` finds a road,
// and the loaded game (AoWRoad.TAoWRoad.UpdateTransitions@5579b84c) *also* counts
// a neighbour whose field answers the road query message 0x1131 - which
// `AoWE.TStructure.MapFieldMsgProc` answers for a structure whose resource byte
// `+0x45` is set. That is what makes a road lead into a city/building. Exposed so
// mapres_test can lock the rule.
std::uint8_t road_transition_byte(std::uint32_t x, std::uint32_t y,
                                  std::uint32_t width, std::uint32_t height,
                                  const std::vector<std::uint8_t>& connected);

// True when a placed object of engine class `class_id` participates in the road
// network, i.e. a road can connect toward its hex. Roads themselves are handled
// by the caller; this covers the *structures* that answer the road query
// (message 0x1131, AoWE.TStructure.MapFieldMsgProc@5575f730). The engine gates
// that on the structure resource's runtime byte `+0x45`, which is not
// serialised, so the port uses the class instead: any placed object that is not
// a road, a terrain hexagon/decoration, a mountain, a crop, a unit/army, an item,
// a fire or a transient cloud is treated as a structure [TODO(RE) exact +0x45].
bool road_connectable_object(std::uint32_t class_id);

// The overlay frames Road.TRoadResource.Show@5561713c draws for a transition
// byte, as indices into the road resource's 8-entry (`Rd_*.BMP`, slots 1..8)
// library. Frame 1..6 are the six single-direction arms, 7/8 the two straights
// (and 7 also the isolated tile); frame 0 is never drawn.
void road_frames(std::uint8_t byte, std::vector<std::size_t>& out);

// Engine class id of a placed hexagon-bridge object. HxBridge.THexagonBridge.
// ClassID@557997c0 (AoWEPACK) returns 0x2087a; its resource is class 0x2087b
// HxBridge.THexagonBridgeResource, a *single* sprite with a three-frame library
// (`Str_brdg.bmp` on the surface, `U_brdg.bmp` underground) carried INLINE in
// rw tag 8 (scene evidence: Dwarf1.CSM levels 1/2 place 6 each). This is the
// bridge the shipped scenarios use - the older `Bridge.TBridge`/`TBridgeResource`
// (HSEPack, classes 0x20174/0x20175, `Wa_bdge.BMP`/`Ula_bdge.BMP`) is the
// 19-frame multi-piece variant and is not placed by them.
constexpr std::uint32_t kHexBridgeClassId = 0x2087a;

// AoWHex.TAoWWaterHexagon.ClassID@5579ad90 returns 0x2073d: the map object class
// of a water *and* an ice tile (their resources are 0x2073e records, WATERHEX.ILB
// and ICEHEX.ILB). A water hexagon draws its own base tile plus, per map frame,
// the one-shot ripple/fish/sparkle sequences of its resource list - see
// aow/map_resources.hpp and docs/18_WATER_ANIM.md.
constexpr std::uint32_t kWaterHexagonClassId = 0x2073d;
// ... and its *resource* class, `AoWHex.TAoWWaterHexagonResource.ClassID
// @5579b584` = 0x2073e (`WATERHEX.ILB` id 0x007b134f, `ICEHEX.ILB` id
// 0x00af9569). The map's tag-50 hexagon carries an *object* class id and the
// rule resources carry their own, so both are needed.
constexpr std::uint32_t kWaterHexagonResourceClassId = 0x2073e;

// HxBridge.THexagonBridge.Show@55799a70 blits the bridge sprite at
// `HXtoHP(hex) + (-0x17, -0x14)` (the image's own base+shift is added inside
// Show). mapviz's hex origin IS HXtoHP, so the sprite box top-left is
// `origin + (kHexBridgeAnchorDx + vx, kHexBridgeAnchorDy + vy)`.
constexpr int kHexBridgeAnchorDx = -0x17;  // -23
constexpr int kHexBridgeAnchorDy = -0x14;  // -20

// True when a `TTerrainList` terrain code counts as *land* for the bridge
// orientation test, i.e. `AoWE.Land@5575aea4` returns non-zero. It excludes 0
// (water), 6 (ice), 9 (lava), 10 (dirt) and 13 (upper-ice); every other code
// (grass/desert/snow/steppe/wasteland/earth/rock/upper-wall/coast/border) is
// land. Exposed so mapres_test can lock the rule.
bool bridge_land(int terrain);

// The orientation byte an `HxBridge.THexagonBridge` stores at `+0x11` and uses
// as its library slot (Show does `Get(resource+0x20, direction)`).
// `HxBridge.THexagonBridge.UpdateTransitions@55799fd8` recomputes it from the
// terrain (the scenario stores only the resource id, and Create initialises it
// to 0xff), testing in the priority order 1, 2, 0:
//
//   direction 0 -> the HN 1 (N)  and HN 4 (S)  neighbours are both land
//   direction 1 -> the HN 2 (NE) and HN 5 (SW) neighbours are both land
//   direction 2 -> the HN 3 (SE) and HN 6 (NW) neighbours are both land
//
// because `DirectionValid@55799c04(d)` is true when the neighbours reached by
// `CenterHNtoHX(d+1)` AND `CenterHNtoHX(d+4)` are land. (On the column-staggered
// grid HSEngine.HNtoHX@5560e08c numbers the six neighbours N, NE, SE, S, SW, NW
// - the +-32px columns always carry a +-16px row offset - so d+1/d+4 are the
// three opposite pairs.) The returned value is exactly the library frame index
// the engine draws, which its own art confirms: frame 0 is a N/S deck, frame 1
// a NE/SW deck and frame 2 a SE/NW deck. `0xff` means no opposite land pair
// (an invalid bridge).
//
// `land` is a `width * height` grid, non-zero where the hex is land terrain.
// Exposed so mapres_test can lock the rule.
std::uint8_t hex_bridge_direction(std::uint32_t x, std::uint32_t y,
                                  std::uint32_t width, std::uint32_t height,
                                  const std::vector<std::uint8_t>& land);

// One shoreline shelf sprite a water/ice hexagon draws: `frame` is an image slot
// of its own library (WATERHEX.ILB `t_wa-gr` slot 0..0x1e, ICEHEX.ILB), `dy` the
// vertical anchor. See water_shore_pieces.
struct ShorePiece {
    int frame;
    int dy;
};

// The shoreline/fringe sprites a water hexagon composites toward its flat-land
// neighbours. `mask` has one bit per direction (bit i = direction i+1 =
// N/NE/E/S/W/NW, HSEngine.HNtoHX + TranslateHX); `variant` is the engine's
// per-hex 0..2 phase (sprite +0x15 >> 6). Evidence:
// AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4 ->
// IsoHex.TLowerIsometricHexagon.ShowDynamic@5561a400, which blits the matching
// library slots at x = HXtoHP - 0x13, y = HXtoHP.y - 0x20 (most) or - 0x1f (the
// "corner" frames 0x16/0x13/0x1e/0x12/0x1c). Exposed so mapres_test can lock it.
void water_shore_pieces(int mask, int variant, std::vector<ShorePiece>& out);

// The canvas top-left at which the engine draws an object's sprite visual box.
// `hex_sx`/`hex_sy` are the hex origin from mapviz's grid (HXtoHP +
// kHexFieldMargin); `vis_x`/`vis_y` are the sprite's base_x+xshift / base_y+yshift
// (the ILB visual-box offset, `Sprite::vx/vy`). `resource` supplies the
// TMultiHexMOR dXmainP/dYmainP (rw tags 12/13). The rules come from the
// decompiled draw path: a single-hex object's anchor is `HXtoHP - 8`
// (HSEngine.TMapObject.Show.c) and a multi-hex object's is
// `HXtoHP + dXmainP/dYmainP` (HSEngine.TMultiHexMO.Show.c); the sprite's own
// base+shift is added on top in both cases. Exposed so mapres_test can lock it.
void object_sprite_origin(int hex_sx, int hex_sy, const MapResource* resource,
                          int vis_x, int vis_y, int& ox, int& oy);

// ---------------------------------------------------------------- city sprites
// A city (City.TCity, class 0x20108) is drawn on top of its "pad" (Pad.TPad,
// class 0x20106 - the grassy/rocky mound the structure sits on, a separate map
// object) as up to three sprites, in this order: the back wall layer
// (`city+0x68`), the body (`city+0x70`), and the front wall layer (`city+0x6c`),
// all at `HXtoHP + resource dx_main/dy_main + sprite base+shift`. Evidence:
// re/exports/aowepack/AoWE.TCity.Show.c (-> AoWE.TCityResource.ShowCity.c) and
// City.TCity.UpdateImages.c, City.TCity.SetRace.c, AoWE.TRaceResource.GetCityImage.c.
//
// The body is race-specific: the size comes from the city *resource* (`+0x78`;
// City1..City4 -> 1..4) and the sprite from the owning race's TRaceResource
// image library (`RACES\R*.ILB`, indexed by the city's rw tag 0xd), slot:
//
//   GetCityImage(size, terrain, wallFlag)@55759ebc:
//     slot = (terrain == 5) ? size + 0x3b : terrain*10 + size + 0x45;
//     if that slot does not exist -> size + (wallFlag == 0 ? 0x32 : 0x3c) - 1;
//     if that also does not exist -> no image (the caller then draws the
//     resource's own block sprite).
//
// A city whose race byte is negative has no race of its own: `GetCityImage`
// gives the caller nothing and `UpdateImages` falls back to the city resource's
// own block - `block[T+1]`, the `C_random` "?" placeholder. An *owned* city
// resolves to its owner's race instead (see `city_body_race`), the same race the
// engine's `TCity.UpdateFlagID` puts on the city's flag; only a city with no
// usable owner draws the placeholder.
//
// `city_terrain_block` is the offset of the terrain's block inside the city
// resource's own library (`iVar5` in UpdateImages): 0x14 for snow (terrain 3),
// 0x28 for the wasteland/earth/rock/lava/upper families (5/7/8/9/12/13), else 0.
int city_terrain_block(int terrain);
std::size_t city_image_slot(int size, int terrain);
std::size_t city_image_fallback_slot(int size, int wall_flag);
// City size (`resource+0x78`, City1..City4 -> 1..4) from the resource's library
// name. Returns 0 when the name does not identify a city resource.
int city_resource_size(const MapResource& resource);

// The race a city's *body* is drawn for. `TCity.UpdateImages` hands the city's
// own race byte (rw tag 0xd, `+0x45`) to `GetCityImage`, and a negative byte -
// the "Random"/unset city race the editor writes (0xff) - makes the engine draw
// the city resource's `C_random` block instead. An *owned* city does not have to
// look like that placeholder: the engine derives an owned city's identity from
// its owner - `TCity.UpdateFlagID@557ac8b0` builds the city's flag byte from
// `TPlayer.GetRace` (`+0xa5`, i.e. `aow::map_player_races`) and the port already
// flies exactly that race's flag - so the body is looked up in that same race's
// library. Without it, the Human player's own city in `Scenario\Custom\test.hsm`
// (and the walled Human town at (12,20) of `Barondir Bay.HSM`) drew the "?"
// placeholder under a Human flag.
//
//   `city_race`     `MapHs::race` (rw tag 0xd; -1 = the race was not set)
//   `owner`         `MapHs::structure_owner` (rw tag 5; 0 = unowned)
//   `player_races`  the roster, `aow::map_player_races`
//   `race_count`    the number of race resources (== length of
//                   `aow::race_image_libraries`), so an unset (0xff) owner race
//                   is rejected instead of indexing the library list out of range
//
// Returns the race id to look up, or -1 when there is none (a city with no usable
// owner keeps the engine's own `C_random` fallback).
int city_body_race(int city_race, unsigned owner,
                   const std::vector<std::uint8_t>& player_races,
                   std::size_t race_count);

// ---------------------------------------------------------------------------
// The owner flag of a placed "takeable" (ownable) structure - `AoWE.TPlayerStructure`.
// See docs/19_STRUCTURE_FLAGS.md.
// ---------------------------------------------------------------------------

// What one flag draw needs. `TFlagControl` owns `Flags.ILB`: the *pole* comes from
// there, and the 12-frame flag itself either from the owner's race library
// (`RACES\R*.ILB`, frames 12..23 in the single-hex/city style and 0..11 in the
// multi-hex style) or - for a structure nobody owns - from `Flags.ILB` itself
// (frames 22..33 / 10..21).
struct StructureFlag {
    std::string pole_library = "Flags.ILB";
    std::size_t pole_frame = 0;  // Flags.ILB slot (4, or flagbyte>>6)
    std::string flag_library;    // the owner's race ILB, or "Flags.ILB"
    std::size_t flag_frame = 0;  // slot inside `flag_library`
    // Anchor = HXtoHP + (dx, dy) (`resource+0x68` / `+0x6c`).
    int dx = 0;
    int dy = 0;
    // The flag sprite itself is blitted at anchor + (sprite_dx, sprite_dy) +
    // its own base+shift (`kStructureFlagSpriteDx/Dy` = -0x15 / -0x10).
    int sprite_dx = kStructureFlagSpriteDx;
    int sprite_dy = kStructureFlagSpriteDy;
};

// Resolves the owner flag the engine would draw for the structure at a hex, or
// returns false when there is none (`AoWE.TPlayerStructure.UpdateFlagID
// @55760ed8` / `TCity.UpdateFlagID@557ac8b0` yield the "no flag" id 0x3f, or the
// resource is not a `TPlayerStructureResource`, or the owner's race library is
// unknown).
//
//   `res`             the structure's map resource (the flag geometry lives on it)
//   `object_class_id` the placed object's engine class (0x20108 = City.TCity
//                     takes the city flag-byte formula and its razed gate; any
//                     other TPlayerStructure subclass uses the plain formula)
//   `owner`           the structure's `structure_owner` (rw tag 5; 0 = unowned)
//   `city_level`      a city's rw tag 0x10 (1..4), ignored otherwise
//   `city_flags`      a city's rw tag 0x16 status byte, ignored otherwise
//   `player_races`    `aow::map_player_races(map)` - race per roster index
//   `race_libraries`  `aow::race_image_libraries(resources)` - ILB per race
//   `clock`           the map clock (`MapVizOptions::clock`)
//   `hex_pixel_x`     the hex's HXtoHP x - the engine adds it to the clock, so
//                     neighbouring columns animate out of phase
//
// The port has no live game state, so two engine conditions are modelled as
// "visible": the object-state gate `this+0xc & 4` (a runtime only status byte - a
// scenario file never stores it) and the per-player fog terms of `Show` (the port
// renders the whole map for nobody in particular). Both are noted in
// docs/19_STRUCTURE_FLAGS.md.
bool resolve_structure_flag(const MapResource* res, std::uint32_t object_class_id,
                            std::uint8_t owner, std::uint8_t city_level,
                            std::uint8_t city_flags,
                            const std::vector<std::uint8_t>& player_races,
                            const std::vector<std::string>& race_libraries,
                            int clock, int hex_pixel_x, StructureFlag& out);

// ---------------------------------------------------------------------------
// Crops - `Crops.TCrop` (class 0x2036a, resource `Crops.TCropResource` 0x2036b).
// ---------------------------------------------------------------------------
//
// A crop is a `TILTerrainMO` overlay placed on a field. Its `Show` is the
// ILTerrain variant (`ILTer.TILTerrainMO.Show@0x55618940`): it draws
// `resource.ImageLibrary.Get(image_index)` at
// `HXtoHP + resource.dXmainP/dYmainP` + the sprite's own base+shift - exactly the
// object anchor the renderer already uses (`object_sprite_origin`). The image
// index comes from `Crops.TCrop.GetTerrainTypeImage@0x557a5f14`:
//
//     index = field_terrain;
//     if (owner.GetRace() == 0x0b) index += 0x1e;   // Undead crop art
//
// `TCrop.UpdateSettings@0x557a5cc8` stores that index and frees the crop when
// `ImageExist(resource.ImageLibrary, index)` is false, so a terrain with no crop
// frame draws nothing. The shipped inline ILB has the sparse ids
// 1,2,3,4 (grass/desert/snow/steppe), 12 (upper/underground) and 31..35,41 (the
// Undead variants); `fill_library` already indexes frames by image-slot id, so
// the index can be used directly.
//
// `owner_race` is the race of the `TPlayerCropStructure` that owns the crop
// (`TCrop.GetTerrainTypeImage` calls vtable +0x238 = `GetRace`, whose vtable
// slot is 0x557a586c in AoWEPACK.dpl). Pass -1 when the crop has no owner (a
// decorative crop, or no farm nearby); it then uses the normal terrain art.
// Returns -1 for an unknown terrain (-1).
int crop_image_index(int terrain, int owner_race);

struct MapVizOptions {
    int level = 0;         // level to render (0 = surface)
    bool objects = true;   // overlay object sprites
    int max_dim = 0;       // >0: downscale so the output fits max_dim px
    // When non-empty, every hex's resolution is written to this path as CSV
    // (`layer,x,y,resource,ilb,frame,entry,w,h,ok`). Diagnostic (Phase 0) for
    // finding hexes that fall back instead of drawing a real sprite.
    std::string debug_csv;
    // When >= 0, the army standing on this hex of `level` is NOT drawn. The
    // viewer's walk animation uses it: the army object is relocated in the map
    // first, then the destination army is hidden while the moving sprite is
    // composited on top, so the army is never drawn twice.
    int hide_army_level = -1;
    int hide_army_x = -1;
    int hide_army_y = -1;
    // When >= 0 and `hide_object_level == level`, the object whose map-file
    // region starts at this offset is left out of the render. Used to build the
    // "terrain + every other object" background the selected structure's 50%
    // blink fades against (`Scene::mv_prepare_structure_fade`;
    // bin/Screen Selection.gif). `MapHs::offset` is unique per placed object:
    // `MapHs::tag` is only the field-table slot within a hex record and is
    // reused on every tile, so it cannot identify a single building.
    int hide_object_level = -1;
    std::size_t hide_object_offset = static_cast<std::size_t>(-1);
    // The map animation clock `AoWHSMap+0x40` (`HSEngine.THSMap.NewFrame@5560d758`
    // adds one per map frame, wrapping at 0x100000). Every image sequence's frame
    // is resolved from it with `ImageLib.TImageSequence.GetFrameIndex@0x552186ec`
    // and the per-object phase `aow::object_anim_phase(hex_x, hex_y)` - exactly
    // what `HSEngine.TMapObjectResource.ShowComplexImage@5560a134` does. A
    // one-shot render bakes the frame at this clock; 0 is the deterministic
    // default (so a head-less render stays reproducible).
    int clock = 0;
    // When true, the renderer leaves out the ANIMATED sequence frames - the body
    // or `.next` chain sequences whose mode is not 0 (a mine's sparkle, a wizard
    // tower's glowing orb, a node's shimmer) *and* the water hexagons' one-shot
    // ripple/fish/sparkle sequences - so a live front-end can composite them
    // itself every map frame with `draw_structure_overlays` /
    // `draw_water_overlays`. The engine re-draws its whole scene each frame;
    // re-rendering a level costs ~28 ms, so the live screens keep the static
    // level cached and only re-composite the animated sprites. Default false: a
    // one-shot render bakes them in at `clock`, which is the head-less proof
    // path.
    bool no_animated_overlays = false;
    // When true, the placed armies (their bodies *and* their ground shadows) are
    // left out of the render, so a live front-end can composite them itself every
    // map frame with `draw_unit_overlays` - as the LAST world layer, which is
    // where the engine sorts them (`AoWE.TUnitHS.CalcShowPriority@0x557840c8`
    // returns `0x68000000 | position`, above the structures' layer) and what
    // keeps a building's animated sprite or the water from covering a unit
    // standing next to it. `draw_structure_overlays` skips the armies for the
    // same reason; the only layer meant to sit *above* a unit is the map's spell
    // gfx (`AoWE.TWorldFXHS`, not implemented yet). Default false: a one-shot
    // render bakes them in, which is the head-less proof path.
    bool no_units = false;
    // The per-hex water-hexagon animation state is seeded from the map's RNG and
    // advanced once per map frame in the engine; a one-shot render instead
    // replays it deterministically from the hexagon's `Create` up to `clock`
    // (see aow/map_resources.hpp). `clock` 0 is therefore the state right after
    // Create - every ripple still asleep - which keeps the golden renders stable.
    WaterAnimOptions water = WaterAnimOptions{};
};

// ---------------------------------------------------------------------------
// World-map movement overlay (see aow/movement.hpp and docs/15_MOVEMENT.md).
// ---------------------------------------------------------------------------

// Draws the movement path's overlay - the game's own `Cursors.ilb` arrow and X
// sprites - straight into an existing RGBA canvas (the one `render_map_rgba`
// produced). `path` is source-first (`aow::MovePath`); `budget` is the army's
// move points this turn and `per_turn` the move points it gets each later turn
// (`path_turns` needs it for the "N turns" label logic).
//
// One image per step, blitted at `hex_origin(hex) - 8 + (sprite base+shift)`,
// exactly as `TMoveSelector.ShowMovePaths@55763f80` does; the destination hex
// gets the X. Nothing is drawn for an empty path. Returns false and sets
// `error` when `Cursors.ilb` cannot be resolved.
// `bgra` selects the canvas channel order: false = RGBA (what render_map_rgba
// returns), true = BGRA (GDI's 32bpp BI_RGB, what the viewer paints from).
// `sprite_scale` grows every overlay sprite by whole pixels around its hex (1 =
// the engine's 1:1 blit). The viewer uses it to keep the arrows' on-screen size
// when it has zoomed the *map* out to fit a large map - the game itself always
// draws them 1:1, so this restores that look instead of shrinking the arrows to
// a few pixels.
bool draw_move_path_overlay(std::vector<std::uint8_t>& rgba, int w, int h,
                            const std::string& ilb_root, const MovePath& path,
                            int budget, int per_turn, std::string& error,
                            bool bgra = false, int sprite_scale = 1);

// Warms the movement overlay's own sprite library (`Images\Cursors.ilb`) into the
// shared library cache, so the first hover of a session does not decode it. The
// engine links it at start-up, so the port does the same when a level loads.
// Returns false (with `error`) when it cannot be resolved.
bool prepare_move_overlay(const std::string& ilb_root, std::string& error);

// Alpha-composites an RGBA sprite (`sprite`, sw x sh) into a BGRA canvas at
// (ox, oy). The viewer's walk animation uses it to draw the moving army over the
// map it already rendered.
void blit_sprite_bgra(std::vector<std::uint8_t>& bgra, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy);

// The same composite into an RGBA canvas (the in-game screen's `game_draw` is
// RGBA, not GDI's BGRA), so its walk animation draws the moving army over the
// level it already rendered.
void blit_sprite_rgba(std::vector<std::uint8_t>& rgba, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy);

// Composites a unit's ground shadow - `AoWE.TUnitShadowHS.Show` -> the unit's
// own `ShowShadow@0x55784504` - into a canvas, the way the engine's
// `ImageLib.TRLESprite16.ShowTransparent` (the vtable+0x84 slot every sprite
// draw uses) does it for the class-0x11 shadow art:
//
//     dst = ((src & 0xF7DF) + (dst & 0xF7DF)) >> 1      (per 16-bit 565 pixel)
//
// i.e. a **50 % average of the shadow sprite and what is under it**, computed in
// the engine's 565 pixel format (the mask clears the red/green low bits of both
// operands before the add, exactly as `FUN_55210ec4` does). `sprite` is the RGBA
// image `unit_shadow_rgba` returned; a transparent pixel (the RLE escape word)
// leaves the destination untouched. `bgra` selects the canvas channel order.
void blit_shadow_bgra(std::vector<std::uint8_t>& bgra, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy);
void blit_shadow_rgba(std::vector<std::uint8_t>& rgba, int cw, int ch,
                      const std::vector<std::uint8_t>& sprite, int sw, int sh,
                      int ox, int oy);

// Resolves the sprite of the army standing on `(x,y)` of `level` (its lead
// unit's standing pose, see docs/13_UNITS_ON_MAP.md) into a standalone RGBA
// image. `vx`/`vy` are the placement offsets relative to `hex_origin(x,y)` -
// the moving-sprite overlay blits at `hex_origin(x,y) + (vx,vy)`. Returns false
// and sets `error` when there is no army or its sprite does not resolve.
bool army_sprite_rgba(const Map& m, const std::vector<MapResource>& res,
                      const std::string& ilb_root, std::size_t level,
                      std::uint32_t x, std::uint32_t y,
                      std::vector<std::uint8_t>& rgba, int& out_w, int& out_h,
                      int& vx, int& vy, std::string& error);

// The **size banner** of an army - `AoWE.TArmyHS.Show@0x557914d8` draws it
// after its lead unit's sprite: the owner race's PartyShield with the army's
// (non-concealed) unit count, and a crown above it when the army carries a
// `TLeader` (`TArmy.UpdateConcealment` sets army+0x10 bit 0x20):
//   race == 0x20 (owner 0, the independents):
//       Flags.ILB      frame count + 0x27
//   else:
//       RACES\R*.ILB   frame count + 0x1d
//   crown (owned race): Flags.ILB frame race + 0x50
// The shield is blitted at `HXtoHP(hex) + (-5,-12) + sprite base+shift`, the
// crown at `HXtoHP(hex) + (-5,-14) + sprite base+shift`, both colour-keyed
// (the reference `bin/counterRef.png` matches the decoded art pixel-for-pixel).
//
// `sx`/`sy` are the caller's hex origin (mapviz's `hex_origin`, or the walk's
// interpolated anchor). The result is the shield + crown composited in that
// order into one RGBA image; `vx`/`vy` are its placement offsets relative to
// `sx`/`sy` (negative: it sits above-left of the hex). Returns false (with
// `error`) when the army has no resolvable banner.
bool army_banner_rgba(const Map& m, const MapHs& army,
                      const std::string& ilb_root, int sx, int sy,
                      std::vector<std::uint8_t>& rgba, int& out_w, int& out_h,
                      int& vx, int& vy, std::string& error);

// The Unitgfx record a placed unit draws with (a plain TUnit looks its Unitres
// record up by its rw tag 10; a hero/leader uses the Herores table). nullptr when
// unresolved. Used by the walk animation to read the unit's move class / poses.
const UnitGraphics* find_unit_graphics(const UnitCatalog& cat,
                                       const MapUnit& u);

// The walking cycle's frame-table length and frame spacing (`period + 1`) for a
// unit's `direction` pose - the two numbers `aow::walk_settle_phase`
// (aow/unit_anim.hpp) needs to align a move's walk with the standing pose it
// hands off to. Reads the walking sequence (`seq[dir + 5]`,
// `aow::unit_sequence_index`), so it resolves against the same records
// `unit_pose_rgba` draws from. Returns false - leaving the caller free to use
// the engine's per-unit phase - when the unit ships no *frame-table* walking
// cycle for that facing (`mode 4`; the shipped world-map walks): a static or
// random pose has no stride to finish.
bool unit_walk_cycle(const UnitGraphics& g, int direction, int& cycle_length,
                     int& spacing);
bool unit_walk_cycle(const UnitCatalog& cat, const MapUnit& unit, int direction,
                     int& cycle_length, int& spacing);

// The *animated* pose of one placed unit - the world-map walk (aow/unit_anim.hpp).
// `direction` is the hex facing 1..6 and `moving` selects the walking sequence
// (`TUnitHS.Show@0x55784614`: standing = seq[dir-1], walking = seq[dir+5]);
// `clock` is the map animation clock (`AoWHSMap+0x40`) and `phase` the per-unit
// offset. Resolves the unit through `cat` (the caller's already-loaded roster, so
// a per-frame call stays cheap), decodes the pose's frame from the sequence's own
// library and returns its RGBA. `vx`/`vy` are the placement offsets relative to
// `hex_origin(x,y)`: the sequence's xoff/yoff plus the sprite's base+shift (the
// engine's `HXtoHP(hex) + seq.xoff/yoff` anchor). Returns false (with `error`)
// when the unit, its sequence or the frame cannot be resolved.
bool unit_pose_rgba(const UnitCatalog& cat, const std::string& ilb_root,
                    const MapUnit& unit, int direction, bool moving,
                    int clock, int phase, std::vector<std::uint8_t>& rgba,
                    int& out_w, int& out_h, int& vx, int& vy,
                    std::string& error);

// The moving sprite at a walk's hand-off: the walking pose cross-faded into the
// standing pose it hands over to (`settle` = the standing pose's weight, 0 =
// exactly `unit_pose_rgba`'s walking pose, 1 = exactly the standing pose). Both
// poses are placed at the same anchor and use the same facing - a move ends
// facing the direction of its last step (`TUnitHS` rw tag 9), which is the
// direction the walk drew with - so the only thing that changes is the pose
// itself. Without it the frame the front-end swaps the moving sprite for the
// standing army is a one-frame pose snap (the "stops a little and then dashes"
// on arrival); with it the unit settles into the idle sprite. `vx`/`vy` are the
// placement offsets relative to `hex_origin(hex)`, as for `unit_pose_rgba`.
// Returns false (with `error`) when either pose cannot be resolved.
bool unit_settle_pose_rgba(const UnitCatalog& cat, const std::string& ilb_root,
                           const MapUnit& unit, int direction, int clock,
                           int phase, double settle,
                           std::vector<std::uint8_t>& rgba, int& out_w,
                           int& out_h, int& vx, int& vy, std::string& error);

// The **ground shadow** of one placed unit - the `AoWE.TUnitShadowHS` scene
// object the engine creates for it (`TUnitHS.CreateShadow@0x55783e20`, only when
// the map has shadows enabled: `AoWHSMap+0x249 != 0`). `TUnitHS.ShowShadow@
// 0x55784504` (the army's override `TArmyHS.ShowShadow@0x557912a8`) resolves
// `aow::unit_shadow_sequence_index(direction)` from the *same* Unitgfx sequence
// list the body uses, frames it with `ImageLib.TImageSequence.GetFrameIndex`
// (`clock` / `phase`, exactly like the pose), and blits the library image at
//   HXtoHP(hex) + seq.xoff/yoff + sprite base+shift
// - the same anchor rule as the body, so the shadow is the body's own footprint.
// A moving unit adds the sub-hex offset to the anchor (`round(HNtoHP(dir) *
// progress / GetMoveSteps())`, i.e. the front-end's interpolated anchor - the
// engine computes it twice, once for the body and once for the shadow).
//
// Unlike the body, the shadow has **no fallback**: a unit whose gfx record ships
// fewer than 24 sequences has no shadow slot at all, and the engine then draws
// nothing. Returns false (with `error`) in that case, or when the sequence's
// library/frame does not resolve. `vx`/`vy` are the placement offsets relative to
// `hex_origin(x,y)`, as for `unit_pose_rgba`; the caller composites the result
// with `blit_shadow_rgba` / `blit_shadow_bgra` (the 50 % blend), *before* the
// body.
bool unit_shadow_rgba(const UnitCatalog& cat, const std::string& ilb_root,
                      const MapUnit& unit, int direction, int clock, int phase,
                      std::vector<std::uint8_t>& rgba, int& out_w, int& out_h,
                      int& vx, int& vy, std::string& error);

// Maps every map-object/terrain resource id to the frame index *within its
// image library*. The scenario stores a distinct resource id per terrain /
// object *variant*; within one `(res_type, library-stem)` group those ids carry
// a contiguous `res_index` counter and each library occupies one contiguous
// run, so the frame is the offset from the run's first id (a new run starts
// whenever `res_index` is not the previous id + 1). Evidence (docs/01_FORMATS.md,
// "Resource resolution + rendering"): terrain `res_type 11` block 9..20 == the 12
// frames of `H_Gr.ILB` (12 entries); `H_Border.BMP` block 78..89 == 12 frames.
// Ids absent from the table map to frame 0.
std::map<std::uint32_t, std::size_t> map_resource_frames(
    const std::vector<MapResource>& res);

// Renders `m`'s level `opt.level` into `rgba` (8-bit RGBA, row-major, 4 bytes
// per pixel, no padding). `out_w` / `out_h` receive the pixel size, which
// `opt.max_dim` may reduce. `res` is the resource list from Release.hss;
// `ilb_root` is the game root scanned for *.ILB files.
//
// This is the primitive the GUI viewer (`game/viewer.cpp`) paints from: it
// needs the pixels, not a file.
//
// When `contents` is non-null the level is **split while it is drawn**: `rgba`
// receives only the *terrain* - the tag-50 hexagons with their coast shelves and
// edge transitions, plus the flat fallback - and `*contents` receives everything
// the sweep draws over them (decorations, roads, bridges, cities and the armies)
// on its own transparent background, at the same size. The split is for
// re-compositing a *local* patch of one hex: **it is not the same order as the
// flat render**, which interleaves each hex's terrain with its objects row by row
// (so a tall object is still clipped by the terrain of the hexes below it, as the
// engine's scene keys do), while the split draws every object over every terrain.
// The in-game screen keeps the flat canvas as its level image and uses the
// contents layer only to put a hex's own art back over the move-selection
// marker's far wall (docs/09_GAME_SCREEN.md 6.9); every other caller passes
// nullptr (the default) and gets the usual flat render.
bool render_map_rgba(const Map& m, const std::vector<MapResource>& res,
                     const std::string& ilb_root, const MapVizOptions& opt,
                     std::vector<std::uint8_t>& rgba, std::uint32_t& out_w,
                     std::uint32_t& out_h, std::string& error,
                     std::vector<std::uint8_t>* contents = nullptr);

// Composites the ANIMATED building overlays of `level` at `clock` into `rgba` -
// the canvas `render_map_rgba` produced with `no_animated_overlays = true`. For
// every structure the object pass draws it walks the image-sequence chain
// `HSEngine.TMapObjectResource.ShowComplexImage@5560a134` would draw and blits
// each sequence whose mode is not 0 (a mine's sparkle, a wizard tower's glowing
// orb, a node's shimmer) at
//   hex_origin(x,y) + seq.xoff/yoff + sprite base+shift,
// with the frame from `ImageLib.TImageSequence.GetFrameIndex@0x552186ec` and the
// per-hex phase `object_anim_phase`. This is the port's equivalent of the engine
// re-drawing its scene every map frame: the caller restores the previous frame's
// rectangle and runs this again. `bgra` selects `rgba`'s byte order (the viewer's
// canvas is BGRA, the in-game screen's is RGBA). The bounding box of everything
// drawn is returned in `dirty_x0..dirty_y1` (`x1 <= x0` when nothing was drawn),
// so the caller can grow its dirty rectangle and restore it next frame. Returns
// false (with `error`) only when the level is out of range.
bool draw_structure_overlays(const Map& m, const std::vector<MapResource>& res,
                             const std::string& ilb_root, int level, int clock,
                             std::vector<std::uint8_t>& rgba, std::uint32_t w,
                             std::uint32_t h, bool bgra, int& dirty_x0,
                             int& dirty_y0, int& dirty_x1, int& dirty_y1,
                             std::string& error);

// Composites the WATER hexagons' animated overlays at map frame `tick` into
// `rgba`. The engine's water hexagon has no map-clock-driven sequence like a
// building: `AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4` keeps a *private*
// counter on every hexagon sprite (`+0x18`), bumped once per rendered map
// frame, and plays one one-shot entry of the resource's `TImageSequenceList`
// (`sprite+0x19`) through it - the WATERANM.ILB ripples, the chained
// `Fish1-*`/`FishSplas1-*` and the `TCOMBAT\OBJECTS\SEFFECT.ILB` sparkles -
// drawn at `hex_origin(x,y) + seq.xoff/yoff + sprite base+shift` and composited
// with the class-0x16 brighten blend, exactly like a structure's overlays. The
// shelf the same function draws is already in the cached level (it is static).
//
// With `no_animated_overlays` the level image has none of them, so a live
// front-end calls this every map frame with an increasing `tick`; a one-shot
// render replays the same rule from `Create` to `MapVizOptions::clock`. The
// per-hex state is cached inside the renderer and rewound when `tick` goes
// backwards, so calls are order-independent and deterministic.
//
// `dirty_x0..dirty_y1` receive the bounding box of everything drawn (`x1 <= x0`
// when nothing was drawn). Returns false (with `error`) only when the level is
// out of range.
//
// `clip_x0..clip_y1` restrict which sprites are composited: a live front-end
// passes its viewport so the rectangle it restores from its pristine canvas
// stays bounded by the screen. Every hexagon's counter is advanced regardless
// (so the phases stay in step with the engine's) and only the *blitted* sprites
// are reported as dirty. `clip_x1 <= clip_x0` or `clip_y1 <= clip_y0` means "the
// whole canvas", which is what a one-shot render wants.
bool draw_water_overlays(const Map& m, const std::vector<MapResource>& res,
                         const std::string& ilb_root, int level, int tick,
                         std::vector<std::uint8_t>& rgba, std::uint32_t w,
                         std::uint32_t h, bool bgra, int& dirty_x0,
                         int& dirty_y0, int& dirty_x1, int& dirty_y1,
                         std::string& error,
                         const WaterAnimOptions& water = WaterAnimOptions{},
                         int clip_x0 = 0, int clip_y0 = 0, int clip_x1 = 0,
                         int clip_y1 = 0);

// Composites the ARMIES of `level` at map `clock` into `rgba` - the canvas
// `render_map_rgba` produced with `no_units = true`. Every placed army
// (`AoWE.TArmyHS`) draws its lead unit's *standing* pose (`TUnitHS.Show@0x55784614`:
// the sequence's own xoff/yoff plus the sprite's base+shift at `HXtoHP(hex)`),
// picked from the army's stored facing (rw tag 9) like the walk's hand-off does,
// plus its ground shadow (`TUnitHS.ShowShadow@0x55784504`), which is framed from
// `clock` and the per-hex phase `object_anim_phase` (the flyers ship 4/6-frame
// shadow tables; every other unit's shadow is static).
//
// This is the port's **last** world layer: call it AFTER
// `draw_structure_overlays` and `draw_water_overlays`, so a unit is never covered
// by a building's animated sprite, its owner flag or a water ripple. The engine's
// sorted scene draws them in the same order - `TUnitHS.CalcShowPriority` returns
// `0x68000000 | position` while a structure/water sprite sits on a lower layer.
// The only layer meant to sit *above* the units is the map's spell gfx
// (`AoWE.TWorldFXHS`, `Create`/`Activate`/`NewFrame`/`Show@55797118`), which the
// port does not implement yet.
//
// The two passes mirror `render_map_rgba`'s final pass exactly: every shadow
// first, then every body, both in row-major hex order - `TUnitShadowHS
// .GetShowPriority@0x55783d64` puts a shadow one layer below its unit
// (`(unit & 0x3ffffff) + 0x64000000`), so the engine composites all shadows
// before all bodies. The shadow blend is the 50 % average `blit_shadow_buffer`
// reproduces, which is NOT idempotent: `rgba` must be the *pristine* canvas of
// the frame (the live screens restore the previous frame's dirty rectangle from
// their cached level first), never a canvas a shadow was already composited into.
//
// `hide_level/x/y` mirror `MapVizOptions::hide_army_*`: the walk animation hides
// the army on the hex it is moving onto, because the moving sprite stands in for
// it. `clip_x0..clip_y1` restrict which sprites are composited, exactly like
// `draw_water_overlays` - the armies are spread over the whole map, so a live
// front-end passes its viewport to keep the rectangle it restores from its cached
// level bounded by the screen; `clip_x1 <= clip_x0` means "the whole canvas".
//
// `dirty_x0..dirty_y1` receive the bounding box of everything drawn (`x1 <= x0`
// when nothing was drawn). Returns false (with `error`) only when the level is
// out of range. There is no `res` parameter: an army's sprite resolves through
// the unit catalogue (Unitres/Unitgfx/Herores), not through `Release.hss`.
bool draw_unit_overlays(const Map& m, const std::string& ilb_root, int level,
                        int clock, std::vector<std::uint8_t>& rgba,
                        std::uint32_t w, std::uint32_t h, bool bgra,
                        int& dirty_x0, int& dirty_y0, int& dirty_x1,
                        int& dirty_y1, std::string& error, int hide_level = -1,
                        int hide_x = -1, int hide_y = -1, int clip_x0 = 0,
                        int clip_y0 = 0, int clip_x1 = 0, int clip_y1 = 0);

// ---------------------------------------------------------------------------
// Tactical battle maps (TCMaps\*.HSM)
// ---------------------------------------------------------------------------
// The interior of a tactical map is built from AoWTCPCK.dpl's combat classes:
//   TCombatHexagon        0x220500  base terrain hex (rw draw: base + 3 edge
//                                   transitions from TCombat\Base_Hex\CTransHx)
//   TWaterCombatHexagon   0x220597  water variant of the same
//   TSingleCombatHexagon  0x22051e  one-layer floor hex (dungeons/expl)
//   TIsometricCombatHexagon 0x22050a elevated/stacked hex
//   TCombatRoad           0x220502  road overlay (base + six edge pieces)
//   TCombatILTerrain      0x220136  trees/rocks/crops (TMapObject.Show)
//   TCombatTerrain        0x220166  combat terrain object
//   TCombatObstacle       0x22052a / THouse 0x220512 / CityWall 0x220504 /
//   TMainStructure 0x22051c / TFleeLocation 0x220528
//   TUnitPositionHS       0x220103  editor/deployment marker (not drawn)
//   BorderHx.TBorderHexagon 0x20557 the frame around the 65x59 playfield
constexpr std::uint32_t kBorderHexagonClassId = 0x20557;
constexpr std::uint32_t kCombatHexagonClassId = 0x220500;
constexpr std::uint32_t kCombatRoadClassId = 0x220502;
constexpr std::uint32_t kCombatILTerrainClassId = 0x220136;
constexpr std::uint32_t kCombatTerrainClassId = 0x220166;
constexpr std::uint32_t kWaterCombatHexagonClassId = 0x220597;
constexpr std::uint32_t kIsoCombatHexagonClassId = 0x22050a;
constexpr std::uint32_t kSingleCombatHexagonClassId = 0x22051e;
constexpr std::uint32_t kUnitPositionHSClassId = 0x220103;

struct TbMapVizOptions {
    int level = 0;
    bool no_objects = false;
    int clock = 0;  // frame counter for animated object sequences
};

// Renders a tactical battle map (`TCMaps\*.HSM`) at 1:1: the 65x59 staggered hex
// grid, the base terrain hexes with their CTransHx edge transitions, roads,
// obstacles and structures. `combat` is `load_combat_resources()`'s table
// (Release\comb_res.pfs); `world` is Release.hss and is only used for the
// BorderHx frame. The output is RGBA, the same shape `render_map_rgba` produces.
bool render_tb_map_rgba(const Map& m, const std::vector<MapResource>& combat,
                        const std::vector<MapResource>& world,
                        const std::string& ilb_root, const TbMapVizOptions& opt,
                        std::vector<std::uint8_t>& rgba, std::uint32_t& out_w,
                        std::uint32_t& out_h, std::string& error);

// PNG wrapper for render_tb_map_rgba.
bool render_tb_map_png(const Map& m, const std::vector<MapResource>& combat,
                       const std::vector<MapResource>& world,
                       const std::string& ilb_root, const TbMapVizOptions& opt,
                       const std::string& out_path, std::string& error);

// Renders `m`'s level `opt.level` to `out_path` (PNG).
bool render_map_png(const Map& m, const std::vector<MapResource>& res,
                    const std::string& ilb_root, const MapVizOptions& opt,
                    const std::string& out_path, std::string& error);

}  // namespace aow
