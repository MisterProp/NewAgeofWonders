// movement_test.cpp - fidelity tests for the AoW1 world-map movement rules.
//
// Locks the extracted move-cost tables, the raw overlay code -> cost-table column
// mapping (the reported "forest is impassable / mountains are walkable" bug -
// docs/15_MOVEMENT.md §2), the move-type/army fold, the hex geometry, the bounded
// pathfinder, the Cursors.ilb overlay frames and the terrain resolution of a
// shipped scenario, against the decompiled engine (docs/15_MOVEMENT.md).
//
//   aow_movement_tests [game-root]      (default: ..\..)
//
// Exits 0 when every check passes, 1 otherwise. stdlib only.
#include <cstdio>
#include <string>
#include <vector>

#include "aow/game_state.hpp"
#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/movement.hpp"
#include "aow/unit_catalog.hpp"

namespace {

int g_fail = 0;
int g_pass = 0;

void check(bool ok, const std::string& what) {
    if (ok) {
        ++g_pass;
    } else {
        ++g_fail;
        std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    }
}

// A flat level of one terrain/overlay, width x height (`overlay` is the raw
// TTerrainList code: -1 = none/plains).
aow::MapLevel flat_level(int w, int h, int terrain, int overlay) {
    aow::MapLevel lv;
    lv.width = static_cast<std::uint32_t>(w);
    lv.height = static_cast<std::uint32_t>(h);
    lv.tiles.resize(static_cast<std::size_t>(w) * h);
    for (aow::MapTile& t : lv.tiles) {
        t.terrain = terrain;
        t.overlay = overlay;
    }
    return lv;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..\\..";
    const std::array<aow::MovePointTable, 256> tables =
        aow::build_move_point_tables();
    // -- The extracted source tables (AoWEPACK.dpl 0x558e84fc). --------------
    // Table 0 = bit 0x01 (walk). The second argument is the field's *raw*
    // TTerrainList overlay code, which is one BELOW the cost table's column
    // (aow::move_point_cost adds 1), so a mountain is code 0 and a forest is
    // code 1 - not the other way round:
    //   -1 none 4 | 0 mountains impassable | 1 forest 6 | 2 hill 6 | 3 scrub 4
    //   4 road/city 3 | 5 bridge 3 (the only water crossing) | 6 rubble 6
    // Expected costs are the wiki's terrain table
    // (ageofwonders.fandom.com/wiki/Terrain): Grassland 4, Forest 6, Hill 6,
    // Mountains Impassable, Road 3, City 3, Water Impassable.
    const aow::MovePointTable walk =
        aow::unit_move_point_table(tables, aow::kMoveWalk);
    check(aow::move_point_cost(walk, 1, -1) == 4,
          "walk: grassland (no overlay) 4");
    check(aow::move_point_cost(walk, 1, 1) == 6, "walk: forest (code 1) 6");
    check(aow::move_point_cost(walk, 1, 2) == 6, "walk: hill (code 2) 6");
    check(aow::move_point_cost(walk, 1, 4) == 3, "walk: road/city (code 4) 3");
    check(aow::move_point_cost(walk, 1, 0) == 0xFF,
          "walk: mountains (code 0) impassable");
    check(aow::move_point_cost(walk, 0, -1) == 0xFF, "walk: water impassable");
    check(aow::move_point_cost(walk, 0, 5) == 3,
          "walk: the bridge (code 5) is the only water crossing");
    check(aow::move_point_cost(walk, -1, 0) == 0xFF, "cost: terrain out of range");
    check(aow::move_point_cost(walk, 1, 99) == 0xFF, "cost: overlay overflow");
    check(aow::move_point_cost(walk, 1, -2) == 0xFF, "cost: overlay underflow");
    check(aow::move_point_cost(walk, 1, -1) == aow::move_point_cost(walk, 3, -1),
          "walk: grass and snow share the cost row");
    // Mountaineering (bit 0x20): 8 on a mountain, 4 on a hill.
    const aow::MovePointTable mount =
        aow::unit_move_point_table(tables, aow::kMoveMountain);
    check(aow::move_point_cost(mount, 1, 0) == 8, "mountaineer: mountain 8");
    check(aow::move_point_cost(mount, 1, 2) == 4, "mountaineer: hill 4");
    // Forestry (bit 0x08): 4 in a forest instead of 6.
    const aow::MovePointTable forest =
        aow::unit_move_point_table(tables, aow::kMoveForest);
    check(aow::move_point_cost(forest, 1, 1) == 4, "forestry: forest 4");
    // Cave crawling (bit 0x10): 3 on dirt (terrain 12) instead of 4.
    const aow::MovePointTable cave =
        aow::unit_move_point_table(tables, aow::kMoveCave);
    check(aow::move_point_cost(cave, 12, -1) == 3, "cave crawling: dirt 3");

    // Fly (bit 0x02) crosses water; walk|fly takes the cheaper of the two.
    const aow::MovePointTable fly =
        aow::unit_move_point_table(tables, aow::kMoveFly);
    check(aow::move_point_cost(fly, 0, -1) == 4, "fly: water costs 4");
    const aow::MovePointTable wf =
        aow::unit_move_point_table(tables, aow::kMoveWalk | aow::kMoveFly);
    check(aow::move_point_cost(wf, 1, -1) == 4, "walk|fly: grassland 4");
    check(aow::move_point_cost(wf, 0, -1) == 4, "walk|fly: water 4 (fly wins)");
    // Flying over a mountain still costs 8 ("Mountains Impassable or 8 with
    // Floating, Flying, or Mountaineering"), so the fold keeps the 8.
    check(aow::move_point_cost(fly, 1, 0) == 8, "fly: mountain costs 8");
    check(aow::move_point_cost(wf, 1, 0) == 8, "walk|fly: mountain 8 (fly wins)");
    check(aow::move_point_cost(wf, 1, 1) == 4, "walk|fly: forest 4 (fly wins)");
    check(aow::move_point_cost(wf, 1, 4) == 3, "walk|fly: road 3 (walk wins)");
    const aow::MovePointTable none = aow::unit_move_point_table(tables, 0);
    check(aow::move_point_cost(none, 1, -1) == 0xFF, "no move type: impassable");
    // The wiki's own hex count for a 28-movement-point unit
    // (ageofwonders.fandom.com/wiki/Movement): 7 standard / 9 road /
    // 4 forest-or-hill hexes - the Tutorial army's budget is exactly 28.
    {
        const int mp = 28;
        check(mp / aow::move_point_cost(walk, 1, -1) == 7 &&
                  mp / aow::move_point_cost(walk, 1, 4) == 9 &&
                  mp / aow::move_point_cost(walk, 1, 1) == 4,
              "wiki: 28 mp = 7 standard / 9 road / 4 forest hexes");
    }

    // -- CreateMovePointTable's per-ability / per-player tweaks -------------
    // (TAbstractUnit.CreateMovePointTable@0x5577fdc4). The walk table's grass =
    // 4, forest = 6, road (raw overlay 4) = 3.
    std::array<bool, 256> noneAb{};
    {
        std::array<bool, 256> has98{};
        has98[0x98] = true;
        const aow::MovePointTable t =
            aow::unit_move_point_table(tables, aow::kMoveWalk, has98);
        check(aow::move_point_cost(t, 1, -1) == 3, "ability 0x98: grass 4 -> 3");
        check(aow::move_point_cost(t, 1, 1) == 5, "ability 0x98: forest 6 -> 5");
        check(aow::move_point_cost(t, 1, 4) == 2, "ability 0x98: road 3 -> 2");
    }
    {
        std::array<bool, 256> has84{};
        has84[0x84] = true;
        const aow::MovePointTable t =
            aow::unit_move_point_table(tables, aow::kMoveWalk, has84);
        check(aow::move_point_cost(t, 1, -1) == 6, "ability 0x84: grass 4 -> 6");
        check(aow::move_point_cost(t, 1, 4) == 5, "ability 0x84: road 3 -> 5");
        check(aow::move_point_cost(t, 1, 0) == 0xFF,
              "ability 0x84: impassable stays impassable");
    }
    {
        // abilities 0x17 | 1 | 0xa5 + a player trait:
        std::array<bool, 256> mover{};
        mover[1] = true;
        aow::PlayerMoveTraits slow;
        slow.cost_flag_a = true;  // && !cost_flag_b -> halve (v+1)>>1
        const aow::MovePointTable h =
            aow::unit_move_point_table(tables, aow::kMoveWalk, mover, slow);
        check(aow::move_point_cost(h, 1, -1) == 2, "cost trait halve: grass 4 -> 2");
        check(aow::move_point_cost(h, 1, 1) == 3, "cost trait halve: forest 6 -> 3");
        aow::PlayerMoveTraits fast;
        fast.cost_flag_b = true;  // !cost_flag_a && cost_flag_b -> double
        const aow::MovePointTable d =
            aow::unit_move_point_table(tables, aow::kMoveWalk, mover, fast);
        check(aow::move_point_cost(d, 1, -1) == 8, "cost trait double: grass 4 -> 8");
        check(aow::move_point_cost(d, 1, 4) == 6, "cost trait double: road 3 -> 6");
        // With neither flag set (the default) the ability changes nothing.
        const aow::MovePointTable u = aow::unit_move_point_table(
            tables, aow::kMoveWalk, mover, aow::PlayerMoveTraits{});
        check(aow::move_point_cost(u, 1, -1) == 4, "cost trait default: unchanged");
    }
    {
        // (TPlayer+0x54) & 2 forces the road column to 2.
        aow::PlayerMoveTraits roads;
        roads.road_cost_2 = true;
        const aow::MovePointTable t = aow::unit_move_point_table(
            tables, aow::kMoveWalk, noneAb, roads);
        check(aow::move_point_cost(t, 1, 4) == 2, "road trait: road 3 -> 2");
        check(aow::move_point_cost(t, 1, -1) == 4, "road trait: grass unchanged");
    }

    // -- GetAbMoveTypesAll ---------------------------------------------------
    std::array<bool, 256> has{};
    check(aow::move_types_from_abilities(has) == 0, "abilities: none -> 0");
    has[0] = true;  // walk
    has[1] = true;  // fly
    has[2] = true;  // water
    has[3] = true;  // forest
    has[4] = true;  // cave
    has[5] = true;  // mountain
    check(aow::move_types_from_abilities(has) == 0x3F,
          "abilities: walk..mountain = 0x3F");
    std::array<bool, 256> alt{};
    alt[0xa5] = true;  // fly alias
    check((aow::move_types_from_abilities(alt) & 0x02) != 0,
          "abilities: 0xa5 aliases fly");
    std::array<bool, 256> fire{};
    fire[7] = true;
    check((aow::move_types_from_abilities(fire) & 0x40) != 0,
          "abilities: 7 sets bit 6");

    // -- The ability bitmap (TCustomAbilityList tags 2/3) --------------------
    // Human Archer (Unitres.pfs record 0): count 23, mask {0x01,0x00,0x40}.
    const std::vector<std::uint8_t> archer = {0x01, 0x00, 0x40};
    const std::array<bool, 256> ab = aow::decode_ability_bitmap(archer, 23);
    check(ab[0] && ab[22], "bitmap: bits 0 and 22 set");
    check(!ab[1] && !ab[23], "bitmap: bit 1 and 23 clear");
    check(aow::move_types_from_abilities(ab) == aow::kMoveWalk,
          "bitmap: the archer walks");
    // The nested [table][data] payload a Unitres record's rw tag 25 carries.
    const std::vector<std::uint8_t> payload = {0x02, 0x02, 0x00, 0x03, 0x04,
                                               0x17, 0x00, 0x00, 0x00, 0x01,
                                               0x00, 0x40};
    const std::array<bool, 256> ab2 = aow::decode_ability_list(payload);
    check(ab2[0] && ab2[22], "ability_ref payload decodes the same bitmap");
    // -- Maximize / army fold -------------------------------------------------
    aow::MovePointTable dst;
    dst.fill(1);
    aow::MovePointTable slow;
    slow.fill(6);
    slow[16] = 0xFF;  // a blocked unit blocks the cell
    aow::maximize_move_point_table(dst, slow);
    check(dst[0] == 6, "maximize: dst = max(dst, src)");
    check(dst[16] == 0xFF, "maximize: 0xFF propagates");
    aow::MovePointTable fast;
    fast.fill(2);
    aow::maximize_move_point_table(dst, fast);
    check(dst[0] == 6, "maximize: the slowest unit wins");
    const aow::MovePointTable army = aow::army_move_point_table({slow, fast});
    check(army[0] == 6, "army table: folded to the slowest unit");
    const aow::MovePointTable empty_army = aow::army_move_point_table({});
    check(empty_army[0] == 0xFF, "army table: empty army is impassable");
    // The 5-argument army fold feeds each unit's abilities into
    // CreateMovePointTable: a walker (grass 4) + an ability-0x98 walker (grass 3)
    // folds to the SLOWER (4), and a lone ability-0x98 unit folds to 3.
    {
        std::array<bool, 256> ab98{};
        ab98[0x98] = true;
        const aow::ArmyMove a =
            aow::army_move(tables, {aow::kMoveWalk, aow::kMoveWalk}, {30, 30},
                           {std::array<bool, 256>{}, ab98});
        check(aow::move_point_cost(a.table, 1, -1) == 4,
              "army fold (abilities): the slower unit wins");
        const aow::ArmyMove b = aow::army_move(tables, {aow::kMoveWalk}, {30},
                                               {ab98});
        check(aow::move_point_cost(b.table, 1, -1) == 3,
              "army fold (abilities): a lone 0x98 unit gets the cheaper table");
    }

    // -- TArmy.MovePoints = min(0x32, min unit) ------------------------------
    check(aow::army_move_points({}) == 0, "budget: empty selection 0");
    check(aow::army_move_points({40, 26, 32}) == 26, "budget: the slowest unit");
    check(aow::army_move_points({200}) == 0x32, "budget: capped at 0x32");
    check(aow::clamp_move_points(-5) == 0, "clamp: negative -> 0");
    check(aow::clamp_move_points(300) == 0xFF, "clamp: >0xFF -> 0xFF");

    // -- Hex geometry (HXtoHP / HNtoHX / dHXtoHNfast / dHXtoRad) -------------
    // Round trip: for every column parity and direction the neighbour is one
    // step away and points back at the source.
    bool adjacent = true, back_ok = true;
    for (int x0 = 0; x0 < 2; ++x0) {
        for (int y0 = 0; y0 < 3; ++y0) {
            for (int dir = 1; dir <= 6; ++dir) {
                int nx = 0, ny = 0;
                if (!aow::hex_neighbour(x0, y0, dir, nx, ny)) back_ok = false;
                if (aow::hex_distance(x0, y0, nx, ny) != 1) adjacent = false;
                if (aow::hex_direction(nx, ny, x0, y0) == 0) back_ok = false;
            }
        }
    }
    check(adjacent, "neighbour: every direction is exactly one hex away");
    check(back_ok, "neighbour: every step has a direction back");
    {
        int nx = 0, ny = 0;
        aow::hex_neighbour(2, 4, 1, nx, ny);
        check(nx == 2 && ny == 3, "neighbour even: dir1 = (0,-1)");
        aow::hex_neighbour(2, 4, 4, nx, ny);
        check(nx == 2 && ny == 5, "neighbour even: dir4 = (0,1)");
        aow::hex_neighbour(3, 4, 2, nx, ny);
        check(nx == 4 && ny == 4, "neighbour odd: dir2 = (1,0)");
        aow::hex_neighbour(3, 4, 6, nx, ny);
        check(nx == 2 && ny == 4, "neighbour odd: dir6 = (-1,0)");
        check(!aow::hex_neighbour(3, 4, 0, nx, ny), "neighbour: dir 0 invalid");
        check(!aow::hex_neighbour(3, 4, 7, nx, ny), "neighbour: dir 7 invalid");
    }
    check(aow::hex_direction(4, 4, 4, 4) == 0, "direction: same hex -> 0");
    check(aow::hex_distance(4, 4, 4, 4) == 0, "distance: same hex -> 0");

    // -- The multi-hex object walk (HSEngine.InitHNWalk/WalkToNextHN) ---------
    // TMultiHexMO.OverrideTerrain walks a multi-hex object's covered hexes in this
    // order and pairs covered hex i with TTerrainList entry i, so a mountain marks
    // its whole footprint and not only the hex the object is stored on.
    {
        int hx = 0, hy = 0;
        aow::hn_walk_hex(2, 4, 0, hx, hy);
        check(hx == 2 && hy == 4, "walk: index 0 is the object's own hex");
        // Ring 1 is exactly the six `hex_neighbour` directions, N..NW, for both
        // column parities (the walk's direction table folds the odd column).
        bool ring_ok = true;
        for (int x0 = 2; x0 < 4 && ring_ok; ++x0) {
            for (int i = 1; i <= 6; ++i) {
                int wx = 0, wy = 0, nx = 0, ny = 0;
                aow::hn_walk_hex(x0, 4, i, wx, wy);
                aow::hex_neighbour(x0, 4, i, nx, ny);
                if (wx != nx || wy != ny) ring_ok = false;
            }
        }
        check(ring_ok, "walk: ring 1 = N, NE, E, S, W, NW on both parities");
        aow::hn_walk_hex(2, 4, 7, hx, hy);
        check(hx == 2 && hy == 2, "walk: index 7 opens ring 2 at N");
        aow::hn_walk_hex(2, 4, 18, hx, hy);
        check(hx == 1 && hy == 2, "walk: index 18 is the last hex of ring 2");
        // Rings 1 and 2 are 6 + 12 distinct covered hexes, 1..2 away from the
        // centre; index 19 opens ring 3.
        bool ring_ok2 = true;
        for (int i = 1; i <= 18; ++i) {
            aow::hn_walk_hex(2, 4, i, hx, hy);
            const int d = aow::hex_distance(2, 4, hx, hy);
            if (d < 1 || d > 2) ring_ok2 = false;
        }
        check(ring_ok2, "walk: indices 1..18 stay inside rings 1 and 2");
        aow::hn_walk_hex(2, 4, 19, hx, hy);
        check(hx == 2 && hy == 1, "walk: index 19 opens ring 3 at N");
    }

    // -- The pathfinder -------------------------------------------------------
    const aow::MapLevel grass = flat_level(8, 8, 1, -1);  // grass, no overlay
    {
        const aow::MovePath p = aow::find_move_path(walk, grass, 0, 0, 2, 0);
        check(p.reached && !p.empty(), "path: reachable across grass");
        check(p.total_cost == 8, "path: two grass steps cost 8");
        check(p.steps.size() == 3, "path: source + 2 steps");
        check(p.steps.front().x == 0 && p.steps.front().y == 0,
              "path: starts at the source");
        check(p.steps.back().x == 2 && p.steps.back().y == 0,
              "path: ends at the destination");
        bool monotonic = true, fours = true;
        for (std::size_t i = 0; i < p.steps.size(); ++i) {
            if (i > 0 && p.steps[i].cost < p.steps[i - 1].cost) monotonic = false;
            if (p.steps[i].cost != 4 * static_cast<int>(i)) fours = false;
        }
        check(monotonic, "path: cumulative cost is non-decreasing");
        check(fours, "path: cumulative costs are 4 per grass hex");
    }
    // A water wall the walker cannot cross, but the flyer can.
    aow::MapLevel wall = grass;
    for (std::uint32_t y = 0; y < wall.height; ++y) {
        wall.tiles[y * wall.width + 1].terrain = 0;  // column 1 = water
    }
    {
        const aow::MovePath w = aow::find_move_path(walk, wall, 0, 0, 3, 0);
        check(w.steps.empty() && !w.reached,
              "path: a water wall blocks a walker");
        const aow::MovePath f = aow::find_move_path(fly, wall, 0, 0, 3, 0);
        check(f.reached && f.total_cost == 12,
              "path: the flyer crosses the water wall (3 x 4)");
    }
    // The bound caps the explored cost.
    {
        const aow::MovePath p = aow::find_move_path(walk, grass, 0, 0, 7, 0, 8);
        check(!p.reached || p.total_cost <= 8,
              "path: the bound caps the explored cost");
        const aow::MovePath q = aow::find_move_path(walk, grass, 0, 0, 7, 0, 0x40);
        check(q.reached, "path: a bigger bound reaches the far hex");
    }
    // An unreachable destination yields no path at all (CalculatePathEx clears).
    {
        aow::MapLevel m = grass;
        m.tiles[0 * m.width + 4].terrain = 0;  // water
        const aow::MovePath p = aow::find_move_path(walk, m, 0, 0, 4, 0);
        check(p.steps.empty(), "path: unreachable destination -> empty");
    }
    // -- The overlay frames (TMoveSelector.ShowMovePaths) --------------------
    // A NE-travelling path (the real in-game capture in bin/MovementRef.png):
    // each step from (x,y) to (x+1,y-1) leaves the hex in the reverse direction
    // (SW = dir 5) and draws Cursors.ilb frame 0x22 + 5 = 0x27 (39).
    aow::MovePath ne;
    ne.steps.push_back(aow::MoveStep{10, 10, 0});
    ne.steps.push_back(aow::MoveStep{11, 9, 4});
    ne.steps.push_back(aow::MoveStep{12, 8, 8});
    ne.total_cost = 8;
    ne.reached = true;
    check(aow::path_arrow_dir(ne, 0) == 5, "overlay: NE step -> reverse dir 5");
    check(aow::path_arrow_frame(aow::path_arrow_dir(ne, 0), true) == 0x27,
          "overlay: within budget -> Cursors.ilb frame 39");
    check(aow::path_arrow_frame(aow::path_arrow_dir(ne, 0), false) == 0x2E,
          "overlay: beyond budget -> dim frame 46");
    check(aow::path_target_frame(true) == aow::kTargetFrameBright,
          "overlay: reachable target -> frame 0x29");
    check(aow::path_target_frame(false) == aow::kTargetFrameDim,
          "overlay: beyond-turn target -> frame 0x30");
    check(aow::path_turns(36, 28, 28) == 1, "turns: 36 cost on a 28 budget");
    check(aow::path_turns(60, 20, 20) == 2, "turns: 60 cost on a 20 budget");
    check(aow::path_turns(10, 20, 20) == 0, "turns: fits this turn -> 0");
    // The turn-boundary hex (TMovepath.MovePointsToPos): an 8-step, 4/hex path
    // (cost 32) with a 28 budget stops at the 7th step (cost 28).
    {
        aow::MovePath longp;
        for (int i = 0; i <= 8; ++i) {
            longp.steps.push_back(aow::MoveStep{i, 0, 4 * i});
        }
        longp.total_cost = 32;
        longp.reached = true;
        check(aow::path_budget_stop_index(longp, 28) == 7,
              "budget stop: 28 of 32 -> the 7th step");
        check(aow::path_budget_stop_index(longp, 32) == -1,
              "budget stop: a fitting path has none");
        check(aow::path_budget_stop_index(longp, 4) == 1,
              "budget stop: one step only");
    }
    // The arrow bright/dim rule follows the accumulated cost FROM THE SOURCE
    // (ShowMovePaths' running `local_34`), so the near half of the trail is
    // bright and the far half dim - not the reverse.
    {
        aow::MovePath p;
        for (int i = 0; i <= 3; ++i) {
            p.steps.push_back(aow::MoveStep{i, 0, 4 * i});
        }
        p.total_cost = 12;
        p.reached = true;
        check(aow::path_arrow_bright(p, 0, 8), "arrow 0 (cost 0) is bright");
        check(aow::path_arrow_bright(p, 1, 8), "arrow 1 (cost 4) is bright");
        check(aow::path_arrow_bright(p, 2, 8), "arrow 2 (cost 8) is bright");
        check(!aow::path_arrow_bright(p, 3, 8), "arrow 3 (cost 12) is dim");
        check(!aow::path_arrow_bright(p, 4, 8), "no arrow past the end");
    }
    // A vertical step: (0,-1) reverses to (0,+1) = dir 4.
    aow::MovePath up;
    up.steps.push_back(aow::MoveStep{5, 5, 0});
    up.steps.push_back(aow::MoveStep{5, 4, 4});
    check(aow::path_arrow_dir(up, 0) == 4, "overlay: up step -> reverse dir 4");
    check(aow::path_arrow_frame(4, true) == 0x26, "overlay: up arrow frame 0x26");

    // -- A shipped scenario --------------------------------------------------
    aow::Map map;
    std::string err;
    if (!aow::load_map(root + "\\Scenario\\Custom\\Tutorial.hsm", map, err)) {
        std::fprintf(stderr, "FAIL: load_map: %s\n", err.c_str());
        return 1;
    }
    std::vector<aow::MapResource> res;
    check(aow::load_map_resources(root + "\\Release\\Release.hss", res, err),
          "tutorial: Release.hss loads");
    const std::size_t filled = aow::fill_tile_terrain(map, 0, res);
    check(filled == map.levels[0].tiles.size(),
          "tutorial: every tile resolves a terrain");
    const aow::MapLevel& lv = map.levels[0];
    check(lv.width == 48 && lv.height == 48, "tutorial: 48x48 level 0");
    const aow::MapTile& hero_tile = lv.tiles[34 * lv.width + 15];
    check(hero_tile.terrain == 1, "tutorial: the hero's hex (15,34) is grass");
    bool any_overlay = false;
    for (const aow::MapTile& t : lv.tiles) {
        if (t.overlay > 0) any_overlay = true;   // an *object* overlay (not -1)
    }
    check(any_overlay, "tutorial: some hexes carry an overlay");

    // Regression for the decoration-terrain bug: a decoration object whose tag-7
    // reports terrain 0 (water) - e.g. Crops.bmp (TT=ff00) - must NOT override the
    // grass hexagon's terrain (TT=fe01). The engine gates an object's terrain via
    // its `TMapObject+0xc & 4` flag; `fill_tile_terrain` reproduces the result by
    // taking the terrain from the tag-50 base hexagon only.
    {
        const aow::MapResource* grass = nullptr;
        const aow::MapResource* crops = nullptr;
        for (const aow::MapResource& r : res) {
            if (r.ilb == "H_gr.BMP" && r.has_terrain && r.terrain == 1) grass = &r;
            if (r.ilb == "Crops.bmp" && r.has_terrain && r.terrain == 0) crops = &r;
        }
        check(grass != nullptr && crops != nullptr,
              "decor: H_gr.BMP and Crops.bmp are in Release.hss");
        if (grass != nullptr && crops != nullptr) {
            aow::Map tm;
            tm.levels.resize(1);
            tm.levels[0].width = 1;
            tm.levels[0].height = 1;
            tm.levels[0].tiles.resize(1);
            aow::MapHs g;
            g.tag = 50;
            g.resource = grass->id;
            aow::MapHs c;
            c.tag = 51;
            c.resource = crops->id;
            tm.levels[0].tiles[0].objects = {g, c};
            aow::fill_tile_terrain(tm, 0, res);
            check(tm.levels[0].tiles[0].terrain == 1,
                  "decor: crops must not turn a grass hex into water");
            check(aow::move_point_cost(walk, tm.levels[0].tiles[0].terrain,
                                       tm.levels[0].tiles[0].overlay) == 4,
                  "decor: the grass hex stays walkable");
        }
    }

    // A `Mountain.TMountainMO` (class 0x20168) *overrides* the overlay accessor:
    // `Mountain.TMountainMO.GetOverlay@557a2dc8` ignores the covered-hex index and
    // answers 0 (mountains) for every hex the object covers, so the entries that
    // ship forest/hill (its "slopes") do not open it. The very same resource
    // placed as a `Terrain.TTerrainMO` (0x20166 - trees, rocks, the hill mounds)
    // keeps its per-hex answer, which is why a hill mound stays walkable under a
    // mountain. (This was the reported bug: only the entries that declared 0
    // themselves were stamped, so a mountain's art-covered slopes - and its own
    // hex when entry 0 declared a hill - stayed walkable.)
    {
        aow::MapResource mnt;
        mnt.class_id = 0x20169;  // Mountain.TMountainMOR
        mnt.id = 0x1234;
        mnt.has_terrain = true;
        mnt.terrain = 1;
        mnt.overlay = 0;
        // own hex = mountain (1 | 0<<8), N neighbour = forest (1 | 1<<8).
        mnt.terrain_list = {0x0001, 0x0101};
        // A plain grass hexagon (tag 50) so the tiles resolve a terrain: the
        // port (like the engine's *field*, whose terrain an object's tag-7 byte
        // only overrides when it has an opinion) takes the terrain from the
        // tag-50 resource.
        aow::MapResource grass;
        grass.id = 0x99;
        grass.has_terrain = true;
        grass.terrain = 1;
        grass.overlay = -1;
        grass.terrain_list = {0xfe01};
        const std::vector<aow::MapResource> one{mnt, grass};
        for (int cls : {static_cast<int>(aow::kMountainMoClassId),
                        static_cast<int>(aow::kTerrainMoClassId)}) {
            aow::Map tm;
            tm.levels.resize(1);
            tm.levels[0].width = 1;
            tm.levels[0].height = 2;
            tm.levels[0].tiles.resize(2);
            for (std::size_t i = 0; i < 2; ++i) {
                aow::MapHs base;
                base.tag = 50;
                base.resource = grass.id;
                tm.levels[0].tiles[i].objects = {base};
            }
            aow::MapHs hs;
            hs.tag = 51;
            hs.class_id = static_cast<std::uint32_t>(cls);
            hs.resource = mnt.id;
            // (0,1) is the object's own hex; its entry 1 covers N = (0,0).
            tm.levels[0].tiles[1].objects.push_back(hs);
            aow::fill_tile_terrain(tm, 0, one);
            const aow::MapTile& own = tm.levels[0].tiles[1];
            const aow::MapTile& north = tm.levels[0].tiles[0];
            const std::string what =
                cls == static_cast<int>(aow::kMountainMoClassId) ? "mountain"
                                                                : "decor hill";
            check(own.overlay == 0,
                  what + " class: the object's own hex is a mountain");
            check(aow::move_point_cost(walk, own.terrain, own.overlay) ==
                      aow::kImpassable,
                  what + " class: the walker cannot enter the anchor hex");
            if (cls == static_cast<int>(aow::kMountainMoClassId)) {
                check(north.overlay == 0,
                      "mountain class: the forest entry becomes a mountain too");
                check(aow::move_point_cost(walk, north.terrain, north.overlay) ==
                          aow::kImpassable,
                      "mountain class: the walker cannot enter the slope hex");
            } else {
                check(north.overlay == 1,
                      "decor class: the forest entry keeps its own answer");
                check(aow::move_point_cost(walk, north.terrain, north.overlay) ==
                          6,
                      "decor class: the forest hex costs a walker 6");
            }
        }
    }

    aow::UnitCatalog cat;
    std::string cerr_;
    check(aow::load_unit_catalog(root, cat, cerr_), "tutorial: unit catalog");
    aow::GameState gs;
    check(aow::load_game_state(map, gs, err), "tutorial: roster");
    const aow::MapHs* human_army = nullptr;
    for (const aow::MapHs& hs : hero_tile.objects) {
        if (hs.class_id == aow::kMapArmyHsClassId) human_army = &hs;
    }
    check(human_army != nullptr, "tutorial: an army sits on (15,34)");
    if (human_army != nullptr && !human_army->units.empty()) {
        check(human_army->units.front().is_hero,
              "tutorial: the lead unit is a hero");
        check(human_army->units.front().resource == 41,
              "tutorial: the hero is Herores resource 41");
        check(human_army->units.front().ability_count > 0,
              "tutorial: the hero's ability list is serialised");
        const std::uint8_t mt = aow::unit_move_types(human_army->units.front());
        check((mt & aow::kMoveWalk) != 0, "tutorial: the hero can walk");
        const std::uint8_t mp =
            aow::unit_move_points(cat, human_army->units.front());
        check(mp == 32, "tutorial: the hero has 32 move points");
        std::vector<std::uint8_t> mts, mps;
        for (const aow::MapUnit& u : human_army->units) {
            mts.push_back(aow::unit_move_types(u));
            mps.push_back(aow::unit_move_points(cat, u));
        }
        const aow::ArmyMove am = aow::army_move(tables, mts, mps);
        check(am.valid, "tutorial: the army folds a move-point table");
        check(am.budget == 28, "tutorial: the army budget is 28 (slowest unit)");
        check((am.move_types & aow::kMoveWalk) != 0,
              "tutorial: the army can walk");
        check(aow::move_point_cost(am.table, 1, -1) == 4,
              "tutorial: army grassland cost 4");
        check(aow::move_point_cost(am.table, 0, -1) == 0xFF,
              "tutorial: the army cannot walk on water");
        // The reported bug: a forest hex (raw overlay code 1) was treated as
        // impassable because the code was used as the cost-table column, which
        // made a mountain (code 0) cost 4 instead. A grass forest must cost 6
        // (4 with Forestry/Flying), a grass mountain nothing (8 with
        // Mountaineering/Flying) - docs/15_MOVEMENT.md §2.
        check(aow::move_point_cost(am.table, 1, 1) == 6,
              "tutorial: the army pays 6 for a forest hex");
        check(aow::move_point_cost(am.table, 1, 0) == 0xFF,
              "tutorial: the army cannot walk onto a mountain hex");
        check(aow::move_point_cost(am.table, 1, 4) == 3,
              "tutorial: the army pays 3 for a road hex");
        // ...and the same holds on the real level: a tree hex is reachable, a
        // mountain hex is not.
        int forest_reachable = 0, forest_tiles = 0;
        for (std::uint32_t y = 0; y < lv.height && !forest_reachable; ++y) {
            for (std::uint32_t x = 0; x < lv.width && !forest_reachable; ++x) {
                const aow::MapTile& t = lv.tiles[y * lv.width + x];
                if (t.terrain != 1 || t.overlay != 1) continue;
                if (++forest_tiles > 40) break;
                const aow::MovePath pf = aow::find_move_path(
                    am.table, lv, 15, 34, static_cast<int>(x),
                    static_cast<int>(y));
                if (pf.reached && pf.steps.size() > 1) forest_reachable = 1;
            }
        }
        check(forest_tiles > 0 && forest_reachable == 1,
              "tutorial: the army can walk into a forest hex");
        int mountain_tiles = 0, mountain_reachable = 0;
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                const aow::MapTile& t = lv.tiles[y * lv.width + x];
                if (t.terrain != 1 || t.overlay != 0) continue;
                ++mountain_tiles;
                const aow::MovePath pm = aow::find_move_path(
                    am.table, lv, 15, 34, static_cast<int>(x),
                    static_cast<int>(y));
                if (pm.reached) ++mountain_reachable;
            }
        }
        check(mountain_tiles > 0 && mountain_reachable == 0,
              "tutorial: no mountain hex is reachable by the walker army");
        // A mountain is a *multi-hex* object: `TMultiHexMO.OverrideTerrain` stamps
        // TTerrainList entry i onto covered hex i (the `hn_walk_hex` order), so the
        // whole footprint is a mountain, not only the hex it is stored on. The
        // reported bug was that only the anchor was stamped, which left a mountain's
        // other peaks - the hexes its art covers - walkable.
        int covered_mountains = 0, walkable_mountain_hexes = 0;
        std::string first_walkable;
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                const aow::MapTile& s = lv.tiles[y * lv.width + x];
                for (const aow::MapHs& hs : s.objects) {
                    const aow::MapResource* r =
                        aow::find_map_resource(res, hs.resource);
                    if (r == nullptr || r->terrain_list.size() < 2) continue;
                    for (std::size_t k = 0; k < r->terrain_list.size(); ++k) {
                        if (((r->terrain_list[k] >> 8) & 0xff) != 0) continue;
                        int hx = 0, hy = 0;
                        aow::hn_walk_hex(static_cast<int>(x), static_cast<int>(y),
                                         static_cast<int>(k), hx, hy);
                        if (hx < 0 || hy < 0 || hx >= static_cast<int>(lv.width) ||
                            hy >= static_cast<int>(lv.height)) {
                            continue;
                        }
                        const aow::MapTile& d =
                            lv.tiles[static_cast<std::size_t>(hy) * lv.width +
                                     static_cast<std::size_t>(hx)];
                        ++covered_mountains;
                        if (aow::move_point_cost(am.table, d.terrain, d.overlay) !=
                            aow::kImpassable) {
                            ++walkable_mountain_hexes;
                            if (first_walkable.empty()) {
                                first_walkable = "src=" + std::to_string(x) + "," +
                                                 std::to_string(y) + " k=" +
                                                 std::to_string(k) + " -> " +
                                                 std::to_string(hx) + "," +
                                                 std::to_string(hy) + " terr=" +
                                                 std::to_string(d.terrain) +
                                                 " ov=" +
                                                 std::to_string(d.overlay);
                            }
                        }
                    }
                }
            }
        }
        // The level now carries many more mountain hexes than it has
        // Mountain.TMountainMO objects: before the footprint pass exactly one hex
        // per object was stamped, which left the rest of every range plain grass.
        int mountain_anchors = 0;
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                for (const aow::MapHs& hs : lv.tiles[y * lv.width + x].objects) {
                    if (hs.class_id == 0x20168) ++mountain_anchors;
                }
            }
        }
        check(covered_mountains >= mountain_anchors &&
                  mountain_tiles > mountain_anchors,
              "tutorial: the mountain footprint outnumbers its objects (" +
                  std::to_string(mountain_tiles) + " hexes vs " +
                  std::to_string(mountain_anchors) + " objects)");
        check(walkable_mountain_hexes == 0,
              "tutorial: no covered mountain hex is walkable by the army (" +
                  first_walkable + ")");
        // ...and concretely: the big grass mountain stored on (25,10) has its
        // N neighbour (25,9) marked impassable too.
        {
            const aow::MapTile& anchor = lv.tiles[10 * lv.width + 25];
            const aow::MapResource* mnt = nullptr;
            for (const aow::MapHs& hs : anchor.objects) {
                if (hs.class_id != 0x20168) continue;  // Mountain.TMountainMO
                const aow::MapResource* r =
                    aow::find_map_resource(res, hs.resource);
                if (r != nullptr && r->terrain_list.size() >= 7) mnt = r;
            }
            check(mnt != nullptr, "tutorial: a multi-hex mountain sits on (25,10)");
            if (mnt != nullptr) {
                check((mnt->terrain_list[1] >> 8) == 0 &&
                          (mnt->terrain_list[2] >> 8) == 0,
                      "tutorial: the mountain's entries 1/2 are mountains");
                int hx = 0, hy = 0;
                aow::hn_walk_hex(25, 10, 1, hx, hy);
                const aow::MapTile& t =
                    lv.tiles[static_cast<std::size_t>(hy) * lv.width +
                             static_cast<std::size_t>(hx)];
                check(t.overlay == 0,
                      "tutorial: the mountain's covered hex carries an overlay");
                check(aow::move_point_cost(am.table, t.terrain, t.overlay) ==
                          aow::kImpassable,
                      "tutorial: the army cannot walk onto that covered hex");
            }
        }
        // The class override in the wild. The big grass mountain's entries 3/4/5
        // ship forest/hill (its "slopes") and the port used to leave them
        // walkable, so the trail could thread *through* a range: with the
        // Mountain.TMountainMO override they are mountains too.
        {
            int slope_tiles = 0, slope_walkable = 0;
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const aow::MapTile& s =
                        lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                    for (const aow::MapHs& hs : s.objects) {
                        if (hs.class_id != 0x20168) continue;
                        const aow::MapResource* r =
                            aow::find_map_resource(res, hs.resource);
                        if (r == nullptr) continue;
                        for (std::size_t k = 3; k <= 5 && k < r->terrain_list.size();
                             ++k) {
                            const int eo = static_cast<std::int8_t>(
                                (r->terrain_list[k] >> 8) & 0xff);
                            if (eo <= 0) continue;  // 0 = mountain, <0 = none
                            int hx = 0, hy = 0;
                            aow::hn_walk_hex(static_cast<int>(x),
                                             static_cast<int>(y),
                                             static_cast<int>(k), hx, hy);
                            if (hx < 0 || hy < 0 ||
                                hx >= static_cast<int>(lv.width) ||
                                hy >= static_cast<int>(lv.height)) {
                                continue;
                            }
                            const aow::MapTile& d =
                                lv.tiles[static_cast<std::size_t>(hy) * lv.width +
                                         static_cast<std::size_t>(hx)];
                            ++slope_tiles;
                            if (aow::move_point_cost(am.table, d.terrain,
                                                     d.overlay) !=
                                aow::kImpassable) {
                                ++slope_walkable;
                            }
                        }
                    }
                }
            }
            check(slope_tiles > 0 && slope_walkable == 0,
                  "tutorial: a mountain's forest/hill slope entries are mountains "
                  "too (" + std::to_string(slope_tiles) + " hexes, " +
                      std::to_string(slope_walkable) + " walkable)");
            // A mountain whose *own* entry 0 declares a hill (0x0201) is still a
            // mountain (Mountain.TMountainMO.GetOverlay ignores the index).
            const aow::MapTile& hill_mo = lv.tiles[31 * lv.width + 17];
            check(hill_mo.overlay == 0 &&
                      aow::move_point_cost(am.table, hill_mo.terrain,
                                           hill_mo.overlay) == aow::kImpassable,
                  "tutorial: (17,31) - a mountain declaring a hill - is a mountain");
            // ...while a Terrain.TTerrainMO hill mound under the range keeps its
            // own answer: the walkable pass the tutorial's road uses.
            const aow::MapTile& mound = lv.tiles[32 * lv.width + 17];
            check(mound.overlay == 2 &&
                      aow::move_point_cost(am.table, mound.terrain,
                                           mound.overlay) == 6,
                  "tutorial: the hill mound on (17,32) stays a hill (cost 6)");
        }
        const aow::MovePath p = aow::find_move_path(am.table, lv, 15, 34, 18, 34);
        if (!p.steps.empty()) {
            check(p.steps.front().x == 15 && p.steps.front().y == 34,
                  "tutorial: path starts on the hero's hex");
            check(p.reached, "tutorial: a nearby hex is reachable");
            bool dir_ok = true;
            for (std::size_t i = 0; i + 1 < p.steps.size(); ++i) {
                if (aow::path_arrow_dir(p, i) < 1) dir_ok = false;
            }
            check(dir_ok, "tutorial: every step resolves an arrow direction");
        }
    }

    std::printf("movement tests: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
