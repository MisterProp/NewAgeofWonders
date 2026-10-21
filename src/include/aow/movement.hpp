// movement.hpp - world-map unit movement: the recovered AoW1 rules.
//
// Reconstructed 1:1 from AoWEPACK.dpl (image base 0x55700000) and HSEPack.dpl
// (0x55600000). Every rule below cites the decompiled routine it came from; the
// full write-up is docs/15_MOVEMENT.md.
//
//   moves     = TAbilityOwner.GetAbMoveTypesAll@0x5574f764
//   tables    = FUN_55744b2c (startup) from 8 source tables @0x558e84fc
//   per-unit  = TAbstractUnit.CreateMovePointTable@0x5577fdc4
//   cost      = TAbstractUnit.MovePointCost@0x55780848
//   per-army  = TArmy.CreateMovePointTable@0x5578dc40 + MaximizeMovePointTable
//               @0x55744d34
//   budget    = TArmy.MovePoints@0x5578d8d0 = min(0x32, min unit move points)
//   path      = TMoveControl.CalculatePathEx@0x557460b0 (a bounded flood fill
//               over accumulated move points, then a gradient descent from the
//               destination back to the source)
//   overlay   = TMoveSelector.ShowMovePaths@0x55763f80 (Cursors.ilb arrows + X)
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/move_cost_tables.hpp"

namespace aow {

// ---------------------------------------------------------------------------
// Movement modes.
//
// `TAbilityOwner.GetAbMoveTypesAll@0x5574f764` folds the unit's abilities into
// one byte:
//
//   bit 0 (0x01) = ability 0
//   bit 1 (0x02) = ability 1 | 0xa5 | 0x3b
//   bit 2 (0x04) = ability 2 | 0x17 | 0xa4 | 0xa6
//   bit 3 (0x08) = ability 3 | 0x9d
//   bit 4 (0x10) = ability 4 | 0x9d
//   bit 5 (0x20) = ability 5 | 0x9d
//   bit 6 (0x40) = ability 7 | 0xa3
//   bit 7 (0x80) = ability 0x2a
//
// Each bit selects one of the eight `kMovePointSourceTables` rows (in table
// order 0x01, 0x04, 0x02, 0x08, 0x10, 0x20, 0x40, 0x80 - see
// kMovePointSourceTableBits).
// ---------------------------------------------------------------------------
enum MoveTypeBit : std::uint8_t {
    kMoveWalk = 0x01,
    kMoveFly = 0x02,
    kMoveWater = 0x04,
    kMoveForest = 0x08,
    kMoveCave = 0x10,
    kMoveMountain = 0x20,
    kMoveBit6 = 0x40,
    kMoveBit7 = 0x80,
};

constexpr int kTerrainCount = 13;   // terrain codes 0..12
constexpr int kOverlayCount = 16;   // raw overlay codes -1..14 -> table columns 0..15
constexpr std::uint8_t kImpassable = 0xFF;

// One 256-byte move-point table: `[terrain*16 + column]`, 0xFF = impassable,
// where the column is the field's *raw* TTerrainList overlay code + 1 (so the
// code -1 = "none" / plains selects column 0 and the code 0 = mountains selects
// column 1). `idx = terrain*16 + overlay + 1` is confirmed by
// TAbstractUnit.MovePointCost@0x55780848 (its 256-byte table is the frame buffer
// written one byte earlier) and TMoveControl.DefaultMoveOnMovePointProc@0x557454d8
// (`*(char*)(mc + field->terrain*0x10 + 0x31 + field->overlay)`, the table at
// `mc + 0x30`) - see docs/15_MOVEMENT.md §2.
using MovePointTable = std::array<std::uint8_t, 256>;

// `TArmy.MovePoints@0x5578d8d0`: the army budget is capped at 0x32 (50).
constexpr std::uint8_t kArmyMovePointsCap = 0x32;

// The value of `TArmy.MovePoints` / `TMoveControlSettings.GetMovePoints` for a
// hex the search never reached.
constexpr int kUnreachableMovePoints = 0xFFFFFF;

// ---------------------------------------------------------------------------
// The 256 `MovePointTables` the engine builds once at startup
// (FUN_55744b2c). For each move-type byte the cell is the cheapest of the
// selected source tables (0xFF when none applies).
// ---------------------------------------------------------------------------
std::array<MovePointTable, 256> build_move_point_tables();

// `TAbilityOwner.GetAbMoveTypesAll@0x5574f764`. `has` is indexed by ability id.
std::uint8_t move_types_from_abilities(const std::array<bool, 256>& has);

// `TAbstractUnit.CreateMovePointTable@0x5577fdc4`, base part: copy
// `MovePointTables[move_types]`. A fresh table is all-0xFF when the unit has no
// movement ability at all.
MovePointTable unit_move_point_table(
    const std::array<MovePointTable, 256>& tables, std::uint8_t move_types);

// The per-player traits `CreateMovePointTable@0x5577fdc4` reads from
// `TPlayer+0x54`/`+0x55`. The port does not model `TPlayer`, so a caller that
// knows them passes them; the defaults leave the base table untouched (the
// common case - a normal-rules player).
struct PlayerMoveTraits {
    bool road_cost_2 = false;  // (TPlayer+0x54) & 0x02: the road column costs 2
    bool cost_flag_a = false;  // (TPlayer+0x54) & 0x04
    bool cost_flag_b = false;  // (TPlayer+0x55) & 0x04
};

// `TAbstractUnit.CreateMovePointTable@0x5577fdc4`, full. After the base copy the
// engine applies, in this order:
//   * `(TPlayer+0x54) & 2`     -> every non-impassable *road* cell (column 5,
//                                 i.e. raw overlay code 4) is forced to 2;
//   * ability 0x17 | 1 | 0xa5  -> a movement-cost trait: with
//                                 `!cost_flag_a && cost_flag_b` every positive
//                                 cost doubles; with `cost_flag_a && !cost_flag_b`
//                                 every cost >2 becomes `(v + 1) >> 1`;
//   * ability 0x98             -> every cost >2 drops by 1 (floored at 2);
//   * ability 0x84             -> every positive cost gains 2.
// `has` is the unit's ability set indexed by ability id.
MovePointTable unit_move_point_table(
    const std::array<MovePointTable, 256>& tables, std::uint8_t move_types,
    const std::array<bool, 256>& has, const PlayerMoveTraits& traits = {});

// `AoWE.MaximizeMovePointTable@0x55744d34`: `dst = max(dst, src)` where 0xFF
// (impassable) wins - i.e. the slowest unit, and any blocked unit blocks the
// whole army. A `dst` that is already 0xFF stays 0xFF.
void maximize_move_point_table(MovePointTable& dst, const MovePointTable& src);

// `TArmy.CreateMovePointTable@0x5578dc40`: seed every cell with 1, fold in each
// unit's table, and when there is no unit at all the whole table is 0xFF.
MovePointTable army_move_point_table(const std::vector<MovePointTable>& units);

// `TArmy.MovePoints@0x5578d8d0`: min(0x32, smallest move points of the units).
// An empty selection is 0.
std::uint8_t army_move_points(const std::vector<std::uint8_t>& unit_move_points);

// `TAbstractUnit.MovePointCost@0x55780848`: table[terrain*16 + overlay + 1].
// `overlay` is the field's raw TTerrainList code (-1 = none, 0 = mountains,
// 1 = forest, 2 = hill, 4 = road/city, 5 = bridge, ...); the cost table's column 0
// is "no overlay", hence the +1. 0xFF = impassable.
std::uint8_t move_point_cost(const MovePointTable& table, int terrain,
                            int overlay);

// An army's whole movement capability: the folded move-point table, the
// move-type union and the this-turn budget.
struct ArmyMove {
    MovePointTable table{};               // TArmy.CreateMovePointTable
    std::uint8_t move_types = 0;          // TArmy.MoveTypes (union of units)
    std::uint8_t budget = 0;              // TArmy.MovePoints = min(0x32, min)
    bool valid = false;                   // false when no unit was selected
};

// Folds `count` selected units (`TArmy.CreateMovePointTable@0x5578dc40`,
// `TArmy.MoveTypes@0x5578d9e8`, `TArmy.MovePoints@0x5578d8d0`).
ArmyMove army_move(const std::array<MovePointTable, 256>& tables,
                   const std::vector<std::uint8_t>& unit_move_types,
                   const std::vector<std::uint8_t>& unit_move_points);

// The same fold, but each unit's own ability set is fed to
// `CreateMovePointTable` too (its per-ability cost tweaks), and the army's
// player traits are applied to every unit's table. `unit_abilities` may be empty
// (then it is treated as all-false, matching the 3-argument overload).
ArmyMove army_move(const std::array<MovePointTable, 256>& tables,
                   const std::vector<std::uint8_t>& unit_move_types,
                   const std::vector<std::uint8_t>& unit_move_points,
                   const std::vector<std::array<bool, 256>>& unit_abilities,
                   const PlayerMoveTraits& traits = {});

// ---------------------------------------------------------------------------
// Combining two armies (a move onto a friendly army).
//
// `AoWE.MoveArmyEx@0x5574a318` picks the move transition from the destination
// field's `TAoWMapField.GetMoveAction` -> `TArmy.GetMoveAction@0x5578f98c`
// (3 = our own army), and `CreateArmyMoveTE@0x5574a1a0` then builds a
// `TArmyCombineMoveTE` for that code. Its `CanMoveNext@0x55748834` accepts the
// last step only when `TArmy.CanCombine@0x55790814` accepts the merge; the
// move transition's `CanMoveNext` then re-checks it through
// `TArmy.CanAddUnitSelection@0x5578e55c` and reports a failure with
// `TArmyCombineMoveTE.ShowCombineFailure@0x5574876c` - the resourcestrings
// "Cannot combine armies" (65435) and "Cannot combine two transporters"
// (65434). `TMoveArmyTE.FinishMove@0x55747fbc` is the merge itself:
// `TArmy.AddUnitSelection(destination, moving, 0xff)` appends the moving units
// in their original order and the destination `TArmyHS` becomes the selection.
//
// `TArmy.CanAddUnitSelection`'s return codes:
//   0 = the units fit, 1 = a unit refuses (`TAbstractUnit.CanAddToList`),
//   2 = the army is full, 3 = both armies carry a transporter.
// A normal army's `TArmy.MaxSize@0x5578def4` is 8, and the recovered test is
// `destination_count + incoming_count < 9` - so an eight-unit merge fits and a
// ninth unit does not. (A transporter replaces the 8 with its capacity + 1; the
// port does not model transporters yet - docs/15_MOVEMENT.md.)
// ---------------------------------------------------------------------------
constexpr std::size_t kArmyMaxUnits = 8;  // TArmy.MaxSize, no transporter

enum class ArmyAddStatus : std::uint8_t {
    ok = 0,
    unit_refused = 1,          // TAbstractUnit.CanAddToList returned false
    full = 2,                  // the merged army would hold more than 8
    transporter_conflict = 3,  // two transporters cannot be combined
};

// `TArmy.CanAddUnitSelection@0x5578e55c` for two transporter-free armies: the
// ports of the recovered 0/1/2/3 codes. `incoming_units` is the selection that
// travels (`TControlWin+0x178`, i.e. the party window's mask), not the whole
// origin army when some units are left behind.
ArmyAddStatus army_can_add_units(std::size_t dest_units,
                                 std::size_t incoming_units);

// ---------------------------------------------------------------------------
// Per-unit abilities.
// ---------------------------------------------------------------------------

// `TCustomAbilityList.GetAbSet@0x5574e0e0`: bit i of the bitmap (LSB-first) is
// ability i; ids >= `count` are "not present". `mask` is ceil(count/8) bytes.
std::array<bool, 256> decode_ability_bitmap(
    const std::vector<std::uint8_t>& mask, std::uint32_t count);

// A `Unitres.pfs` record's rw tag 25 (`UnitEntry::ability_ref`) is the unit
// *type*'s TCustomAbilityList serialised as a nested `[table][data]` EObject
// with tag 2 (u16 count) and tag 3 (the bitmap). Returns the bitmap.
std::array<bool, 256> decode_ability_list(
    const std::vector<std::uint8_t>& payload);

// `TAbstractUnit.GetMoveTypes@0x5577ffd0` = GetAbMoveTypesAll over the placed
// unit's own ability list (MapUnit::ability_mask).
std::uint8_t unit_move_types(const MapUnit& unit);

// The per-turn movement allowance of a placed unit. `TUnit.GetMovePoints@
// 0x5578276c` reads +0x3d, which `TAbstractUnit.NewTurn@0x55780d4c` refills from
// `GetMoves` (+0x2d) - the Unitres record's `moves` field (tag 18) for a plain
// unit, or the Herores record's `moves` (tag 19) for a hero/leader. Returns 0
// when the catalogue has no value for the unit.
std::uint8_t unit_move_points(const class UnitCatalog& catalog,
                              const MapUnit& unit);

// What the unit can still spend *this turn*: the engine's `GetMovePoints`
// (+0x3d) as it stands after moving, i.e. `unit_move_points` less whatever the
// unit has spent (`MapUnit::move_points_left`). The Party window's pill caption
// prints this, not the maximum allowance.
std::uint8_t unit_move_points_left(const class UnitCatalog& catalog,
                                   const MapUnit& unit);

// Charges `cost` move points to one travelling unit (`TAbstractUnit
// .MovePointCost@0x55780848` is applied step by step as the army walks, so every
// unit of the party pays the path's cost). Clamped to 0..allowance.
void spend_unit_move_points(const class UnitCatalog& catalog, MapUnit& unit,
                            int cost);

// `TUnit.SetMovePoints@0x55782794` clamps to 0..max; a unit's move points come
// from the rule table (`TUnit.GetMovePoints@0x5578276c` reads +0x3d).
std::uint8_t clamp_move_points(int value);

// ---------------------------------------------------------------------------
// Hex geometry.
//
// `HSEngine.HXtoHP@5560e3b4`: px = x*0x20 + 8, py = y*0x20 + (x & 1)*0x10 - an
// axis-aligned, column-staggered grid. `HSEngine.HNtoHX@5560e08c` gives the six
// neighbour offsets `(0,-1)/(1,-1)/(1,0)/(0,1)/(-1,0)/(-1,-1)` (dir 1..6),
// folded by `HSEngine.CenterHNtoHX@5560e17c` so an odd column adds one row for
// an odd dx.
// ---------------------------------------------------------------------------
struct Hex {
    int x = 0;
    int y = 0;
    int level = 0;
};

// `HSEngine.dHXtoHNfast@5560e338`: the direction (1..6) to step from (x0,y0) to
// an adjacent (x1,y1); 0 when the two hexes are identical, and unspecified when
// they are not adjacent.
int hex_direction(int x0, int y0, int x1, int y1);

// `HSEngine.dHXtoRad@5560dcf4`: the hex distance between two hexes.
int hex_distance(int x0, int y0, int x1, int y1);

// The (x,y) reached from (x,y) by a step in direction `dir` (1..6). False when
// `dir` is out of range.
bool hex_neighbour(int x, int y, int dir, int& nx, int& ny);

// The engine's hex-neighbourhood walk (`HSEngine.InitHNWalk@5560e5c8` +
// `WalkToNextHN@5560e610`, direction table `DAT_5562e250` = HSEPack file
// 0x02d050, indexed `[edge 0..5][x parity]`). Started with radius 0 at (x,y) it
// yields the object's own hex first and then the surrounding rings clockwise:
// `index` 0 = (x,y), 1..6 = the six neighbours N, NE, E, S, W, NW, 7..18 = the
// radius-2 ring, and so on (an odd column folds one row, exactly like
// `hex_neighbour`).
//
// This is the order `HSEngine.TMultiHexMO.OverrideTerrain@5560b2d4` pairs with a
// resource's `TTerrainList`: covered hex `index` carries `MapResource::terrain_list
// [index]` - the same pairing `Mountain.TMountainMO.CanPlace@556a2e54` builds
// through `HSEngine.CenterHNtoHX@5560e17c`. It is why a mountain stamps every hex
// of its footprint, not only the hex the object is stored on.
void hn_walk_hex(int x, int y, int index, int& hx, int& hy);

// ---------------------------------------------------------------------------
// Terrain resolution.
// ---------------------------------------------------------------------------

// Fills `MapTile::terrain` / `MapTile::overlay` for `map.levels[level]` from the
// tile's sprites: the tag-50 terrain-hexagon resource's rw tag 7 (`TTerrainList`)
// entry 0 is a u16 `terrain | overlay << 8` (`TTerrainList.GetTerrain@55605054` /
// `GetOverlay@556050c8`), and `TMultiHexMO.OverrideTerrain@5560b2d4` writes both
// bytes onto every hex an object covers - only when the byte is `>= 0` as a
// *signed char*, so 0x80..0xFF (a plain hexagon ships 0xFE) means "no opinion".
// The engine walks the field's sprites top-down (`UpdateTerrainType@55607df4` /
// `UpdateOverlayType@55607e48`) taking the first opinion for the terrain and -
// independently - the first for the overlay, so an object overrides the hexagon
// under it. `overlay` is left -1 (= no overlay) when nothing answers: the raw code
// 0 is *mountains*, not "plain" (see `move_point_cost`). Returns the number of
// tiles that got a terrain.
//
// A *multi-hex* object (`terrain_list.size() > 1` - a mountain, a big city) is
// stored on one hex but covers `terrain_list.size()` hexes; the same pass stamps
// its per-hex overlay onto each covered hex (`hn_walk_hex`, the engine's
// `TMultiHexMO.OverrideTerrain` walk), so every hex of a mountain becomes
// impassable - not only the one the object stands on.
//
// A **`Mountain.TMountainMO`** (class 0x20168) is special, because it *overrides*
// the overlay accessor: `Mountain.TMountainMO.GetOverlay@557a2dc8` ignores the
// covered-hex index and answers **0 (mountains)** whenever its own entry 0 has an
// overlay opinion, so *every* hex the object covers is a mountain - its "slope"
// entries (they ship `terrain | 1<<8` / `2<<8` = forest/hill) do not open it.
// `Terrain.TTerrainMO` (class 0x20166 - trees, rocks, the hill mounds) does NOT
// override the accessor, so a hill mound keeps answering hill and stays walkable
// even underneath a mountain. `Cave.TCave` and `Sign.TSign` override it to
// "-1 = no opinion". A road (4) or a bridge (5) still wins - it is placed on top
// of the mountain, so `UpdateOverlayType` reads it first (a mountain pass stays
// open). See docs/15_MOVEMENT.md §1.
std::size_t fill_tile_terrain(Map& map, std::size_t level,
                              const std::vector<MapResource>& resources);

// ---------------------------------------------------------------------------
// The path.
// ---------------------------------------------------------------------------

// One path hex. `cost` is the accumulated move points from the source to this
// hex (0 on the source itself) - `TMoveControlSettings.GetMovePoints`.
struct MoveStep {
    int x = 0;
    int y = 0;
    int cost = 0;
};

struct MovePath {
    // Source first, destination last. Empty when the destination could not be
    // reached at all - `CalculatePathEx` CLEARS the list when the destination
    // is unreachable.
    std::vector<MoveStep> steps;
    int total_cost = 0;    // accumulated move points of the whole path
    bool reached = false;  // the destination was reached within `bound`
    int turns = 0;         // whole turns the path needs (ShowMovePaths label)

    bool empty() const { return steps.size() < 2; }
    // The destination hex, or nullptr.
    const MoveStep* destination() const {
        return steps.empty() ? nullptr : &steps.back();
    }
};

// The per-hex move cost of the destination of a step, 0xFF when impassable.
using TileCostFn = std::function<std::uint8_t(int x, int y)>;

// A bounded cheapest-path search (`TMoveControl.CalculatePathEx`). The engine
// floods the move-point grid out from the source and then walks the gradient
// back from the destination; a bounded Dijkstra computes the identical
// accumulated costs, so that is what the port does.
//
// `bound` is the largest accumulated cost to explore (the engine's
// `TMoveControl+0x2c`). Cells whose accumulated cost exceeds it are not
// expanded. The result is source-first; `reached` says whether the destination
// was found within the bound.
MovePath find_move_path(int width, int height, int source_x, int source_y,
                        int dest_x, int dest_y, int bound,
                        const TileCostFn& cost);

// Convenience: the army's table + one map level's `terrain`/`overlay` (call
// `fill_tile_terrain` first).
MovePath find_move_path(const MovePointTable& table, const MapLevel& level,
                        int source_x, int source_y, int dest_x, int dest_y,
                        int bound = 0xFF);

// ---------------------------------------------------------------------------
// Stream helpers (used by the CLI/tests to print a unit's movement).
// ---------------------------------------------------------------------------
const char* move_type_name(std::uint8_t move_type);
std::string move_types_string(std::uint8_t move_types);

// ---------------------------------------------------------------------------
// The overlay sprites (`TMoveSelector.ShowMovePaths@0x55763f80`).
//
// The engine's path list is stored DESTINATION-FIRST (`CalculatePathEx` builds
// it while walking back from the destination), and for the step from list[i] to
// list[i+1] it blits a `Cursors.ilb` frame at
// `HXtoHP(list[i]) - 8` (plus the sprite's own base+shift):
//
//   dir   = dHXtoHNfast(list[i], list[i+1])
//   frame = dir + 0x22   while the army can still reach the end of the path
//                        from list[i] this turn (bright)
//   frame = dir + 0x29   otherwise (dim)
//
// Because the port stores the path SOURCE-FIRST, the direction to use for the
// step leaving `steps[i]` is `hex_direction(steps[i+1], steps[i])` - the
// reverse step, which is exactly what the engine's destination-first list
// yields. Verified against a real in-game capture: a NE-travelling path draws
// Cursors.ilb frame 0x27 (39) at every step (docs/15_MOVEMENT.md).
//
// The destination hex gets an X: frame 0x29 (bright) when the whole path fits
// in this turn's move points, otherwise 0x30 (dim) with the turn count label.
// ---------------------------------------------------------------------------
constexpr int kArrowFrameBrightAnchor = 0x22;  // + dir(1..6) -> 0x23..0x28
constexpr int kArrowFrameDimAnchor = 0x29;     // + dir(1..6) -> 0x2A..0x2F
constexpr int kTargetFrameBright = 0x29;
constexpr int kTargetFrameDim = 0x30;

// `dir` for the step leaving `path.steps[i]` - the reverse step direction.
int path_arrow_dir(const MovePath& path, std::size_t i);

// The arrow frame for a step leaving a hex in direction `dir`: 0x22 + dir while
// the army can still reach the end of the path from here, else 0x29 + dir.
int path_arrow_frame(int dir, bool within_budget);

// Whether the arrow leaving `path.steps[i]` is bright. `ShowMovePaths`' running
// `local_34` equals the accumulated move points from the SOURCE to `steps[i]`
// (see docs/15_MOVEMENT.md §6), so the arrow is bright while that accumulated
// cost is within `budget` - the near half of the trail is bright and the far half
// (toward the destination) is dim. False for the last step (which has no arrow).
bool path_arrow_bright(const MovePath& path, std::size_t i, int budget);

// The destination X frame: bright when the path fits this turn, else dim.
int path_target_frame(bool within_budget);

// How many whole turns the path needs (the `ShowMovePaths` label):
//   turns = ((total_cost - budget) - 1) / per_turn + 1
int path_turns(int total_cost, int budget, int per_turn);

// The index (source-first) of the hex where `budget` move points run out this
// turn - `AoWE.TMovepath.MovePointsToPos@0x557638d8`, the hex ShowMovePaths marks
// with a second (bright) X when the whole path does not fit the turn. Returns -1
// when the path fits `budget` (or is too short).
int path_budget_stop_index(const MovePath& path, int budget);

}  // namespace aow
