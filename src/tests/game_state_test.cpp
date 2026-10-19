// game_state_test.cpp - fidelity tests for the AoW1 game state (player roster +
// turn/day loop).
//
// Loads the shipped Tutorial.hsm and Dwarf1.CSM through aow::load_map +
// aow::load_game_state and checks the roster, the placed troops per player and
// the recovered turn loop, against values read from the same maps with the
// Ghidra + probe workflow (docs/14_GAME_STATE.md).
//
//   aow_game_state_tests [game-root]      (default: ..\\..)
//
// Exits 0 when every check passes, 1 otherwise. stdlib only.
#include <cstdio>
#include <string>
#include <vector>

#include "aow/game_state.hpp"
#include "aow/map_types.hpp"

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

aow::GameState load(const std::string& path, bool& ok) {
    aow::Map map;
    std::string err;
    ok = aow::load_map(path, map, err);
    if (!ok) {
        std::fprintf(stderr, "FAIL: load_map(%s): %s\n", path.c_str(), err.c_str());
        return {};
    }
    aow::GameState gs;
    ok = aow::load_game_state(map, gs, err);
    if (!ok) {
        std::fprintf(stderr, "FAIL: load_game_state(%s): %s\n", path.c_str(),
                     err.c_str());
        return {};
    }
    return gs;
}

// Total placed armies/units on the map (all owners).
void totals(const aow::GameState& gs, int& armies, int& units) {
    armies = 0;
    units = 0;
    for (const aow::Player& p : gs.players) {
        armies += p.armies;
        units += p.units;
    }
}

}  // namespace

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..\\..";

    // -- Tutorial.hsm: 3 players (independents + 2). --------------------------
    bool ok = false;
    const aow::GameState tut =
        load(root + "\\Scenario\\Custom\\Tutorial.hsm", ok);
    if (!ok) return 1;

    check(tut.loaded, "tutorial: roster loaded");
    check(tut.players.size() == 3, "tutorial: 3 roster entries");
    check(tut.player_count() == 2, "tutorial: 2 real players");
    if (tut.players.size() == 3) {
        check(tut.players[0].index == 0, "tutorial: entry 0 is independents");
        check(tut.players[0].race == 0xff, "tutorial: independents race 0xff");
        check(tut.players[0].gold == 250, "tutorial: independents gold 250");
        check(tut.players[0].display_name() == "Independents",
              "tutorial: independents display name");
        check(tut.players[1].index == 1, "tutorial: entry 1 index 1");
        check(tut.players[1].race == 5, "tutorial: player 1 race 5");
        check(tut.players[1].gold == 1000, "tutorial: player 1 gold 1000");
        check(tut.players[2].index == 2, "tutorial: entry 2 index 2");
        check(tut.players[2].race == 10, "tutorial: player 2 race 10");
        // The Goblin player carries a leader too, and `TPlayer.GetName` prefers
        // it over the (empty) stored name - see the leader checks below.
        check(tut.players[2].display_name() == "Borok",
              "tutorial: player 2 display name is its leader's");
        // TPlayer rw tag 0x3 (+0xa7) = the player type: the independents carry
        // 4, the local human 0 and the AIs their difficulty (see
        // TSetupControl.hPickMap 0x557e20c8).
        check(tut.players[0].type == aow::PlayerType::Independent,
              "tutorial: independents type = Independent");
        check(tut.players[1].type == aow::PlayerType::Human,
              "tutorial: player 1 type = Human");
        check(tut.players[2].type == aow::PlayerType::AI3,
              "tutorial: player 2 type = AI3 (CPU Lord)");
    }

    int armies = 0, units = 0;
    totals(tut, armies, units);
    check(armies == 4, "tutorial: 4 placed armies");
    check(units == 15, "tutorial: 15 placed units");
    if (tut.players.size() == 3) {
        check(tut.players[0].armies == 1 && tut.players[0].units == 4,
              "tutorial: independents own 1 army / 4 units");
        check(tut.players[1].armies == 1 && tut.players[1].units == 4,
              "tutorial: player 1 owns 1 army / 4 units");
        check(tut.players[2].armies == 2 && tut.players[2].units == 7,
              "tutorial: player 2 owns 2 armies / 7 units");
    }

    // -- The Events banner's leader (TPlayer rw tag 0x1e, docs/09 sec. 4.1). ----
    //    The record is a `THero` (TLeader) serialised inline; its tag 7 is the
    //    *face* index (an FACERES.PFS tag), tag 8 the THeroResource index, tag 10
    //    the name and tag 11 the nickname. `TPlayer.GetName` (0x55752104) is what
    //    the banner's `Player` label shows, and the independents (index 0, which
    //    never stores the tag) fall back to "Independents".
    if (tut.players.size() == 3) {
        check(!tut.players[0].leader.present,
              "tutorial: the independents carry no leader");
        check(tut.players[1].leader.present,
              "tutorial: player 1 carries a leader");
        check(tut.players[1].leader.name == "Trodsen",
              "tutorial: player 1 leader name");
        check(tut.players[1].leader.face_index == 150,
              "tutorial: player 1 leader face index 150");
        check(tut.players[1].leader.nickname.empty(),
              "tutorial: player 1 leader has no nickname");
        check(tut.players[1].leader.hero_resource_index >= 0,
              "tutorial: player 1 leader hero-resource index");
        check(tut.players[1].display_name() == "Trodsen",
              "tutorial: the name label is the leader's name");
    }

    // -- Thinreed Lake (the reference screen, docs/09 sec. 4.1): the Human seat
    //    is the Orc player and its leader is "Rasidan" with face index 270
    //    (FACERES.PFS tag 270 -> FACES\L_FACES.ILB id 45 = L-orc01.BMP). --------
    {
        const aow::GameState tr =
            load(root + "\\Scenario\\Custom\\Thinreed Lake.hsm", ok);
        if (!ok) return 1;
        check(tr.players.size() == 5, "thinreed: 5 roster entries");
        if (tr.players.size() == 5) {
            check(tr.players[1].race == 9, "thinreed: player 1 race 9 (Orcs)");
            check(tr.players[1].leader.present &&
                      tr.players[1].leader.name == "Rasidan",
                  "thinreed: player 1 leader is Rasidan");
            check(tr.players[1].leader.face_index == 270,
                  "thinreed: player 1 leader face index 270");
            check(tr.players[1].display_name() == "Rasidan",
                  "thinreed: the name label is Rasidan");
        }
    }

    // -- Dwarf1.CSM: 4 roster entries (independents + 3). ---------------------
    const aow::GameState dwf =
        load(root + "\\Scenario\\Campaign\\Dwarf1.CSM", ok);
    if (!ok) return 1;
    check(dwf.players.size() == 4, "dwarf1: 4 roster entries");
    check(dwf.player_count() == 3, "dwarf1: 3 real players");
    if (dwf.players.size() == 4) {
        check(dwf.players[1].gold == 125, "dwarf1: player 1 gold 125");
        check(dwf.players[1].race == 6, "dwarf1: player 1 race 6");
        check(dwf.players[2].gold == 250, "dwarf1: player 2 gold 250");
        check(dwf.players[3].gold == 275, "dwarf1: player 3 gold 275");
        // The map's own Human/CPU assignment: the played race (Dwarves, id 6) is
        // the human; the other two are CPU Squire / CPU Knight.
        check(dwf.players[1].race == 6 &&
                  dwf.players[1].type == aow::PlayerType::Human,
              "dwarf1: player 1 = Dwarves, Human");
        check(dwf.players[2].type == aow::PlayerType::AI1,
              "dwarf1: player 2 type = AI1 (CPU Squire)");
        check(dwf.players[3].type == aow::PlayerType::AI2,
              "dwarf1: player 3 type = AI2 (CPU Knight)");
    }
    totals(dwf, armies, units);
    check(armies == 53, "dwarf1: 53 placed armies");
    check(units == 154, "dwarf1: 154 placed units");

    // -- The rwByte default: an absent TPlayer race tag (0x2) is Humans. ------
    //    `TEReadStorageStream.rwByte` (vtable +0x30) writes **0** into the field
    //    when the tag is missing, so a map that never stored a race has Humans
    //    (0) - not the 0xff `TPlayer.Create` sentinel. This is the general rule
    //    behind the scenario screen's race cells (docs/08_SCENARIO_SCREEN.md
    //    sec. 8): these maps omit tag 0x2 for exactly one player, and the
    //    original lights that player's Humans cell. Every other race in these
    //    maps IS written explicitly, and the independents' 0xff is explicit.
    {
        const aow::GameState eu =
            load(root + "\\Scenario\\Custom\\AoW Europe Heaven Games.hsm", ok);
        if (!ok) return 1;
        check(eu.players.size() == 9, "europe: 9 roster entries");
        check(eu.players[0].race == 0xff, "europe: independents race 0xff");
        // Player 1's record carries no race tag -> Humans, and it is the Human.
        check(eu.players[1].race == 0, "europe: unraced player 1 = Humans");
        check(eu.players[1].type == aow::PlayerType::Human,
              "europe: player 1 = Human");
        check(eu.players[2].race == 4, "europe: player 2 race 4 (Elves)");

        const aow::GameState bb =
            load(root + "\\Scenario\\Custom\\Barondir Bay.HSM", ok);
        if (!ok) return 1;
        check(bb.players.size() == 5, "barondir: 5 roster entries");
        check(bb.players[3].race == 0, "barondir: unraced player 3 = Humans");
    }

    // -- TSetupControl.SetupMap (0x557e0b64) + hPickMap (0x557e20c8): the scenario
    //    screen's per-race player types are applied to the roster, and only the
    //    first Human survives (a later Human -> AI1, like ResetPlayerSettings
    //    0x557e1378). This is the race <-> played-units link. ------------------
    {
        aow::GameState s = tut;   // players: [0]=indie, [1] race 5, [2] race 10
        std::uint8_t rt[12] = {0};
        rt[5] = static_cast<std::uint8_t>(aow::PlayerType::AI1);    // Halfling CPU
        rt[10] = static_cast<std::uint8_t>(aow::PlayerType::Human);  // Goblin Human
        const std::uint8_t human = aow::apply_setup_player_types(s, rt, 12);
        check(human == 2, "setup: the chosen race's player is the human");
        check(s.players[1].race == 5 && s.players[1].type == aow::PlayerType::AI1,
              "setup: the previous human becomes CPU");
        check(s.players[2].race == 10 &&
                  s.players[2].type == aow::PlayerType::Human,
              "setup: the chosen race is Human");
        check(s.players[0].type == aow::PlayerType::Independent,
              "setup: the independents are untouched");
    }
    {
        // Two Human slots -> the first keeps it; the second drops to AI1.
        aow::GameState s = tut;
        std::uint8_t rt[12] = {0};
        rt[5] = 0;
        rt[10] = 0;
        const std::uint8_t human = aow::apply_setup_player_types(s, rt, 12);
        check(human == 1, "setup: the first Human slot stays the human");
        check(s.players[1].type == aow::PlayerType::Human,
              "setup: first Human stays Human");
        check(s.players[2].type == aow::PlayerType::AI1,
              "setup: a later Human becomes AI1");
        // A player whose race is out of range keeps the map's own type.
        aow::GameState s2 = tut;
        s2.players[1].race = 255;
        std::uint8_t rt2[12] = {0};
        const std::uint8_t h2 = aow::apply_setup_player_types(s2, rt2, 12);
        check(s2.players[1].type == aow::PlayerType::Human,
              "setup: an out-of-range race keeps the map's type");
        check(h2 == 1, "setup: an out-of-range-race human is still the human");
    }

    // -- Turn order: [0] + a permutation of 1..N. ----------------------------
    const std::vector<std::uint8_t> order =
        aow::generate_turn_order(3, 12345u);
    check(order.size() == 4, "turn order: independents + 3 players");
    check(!order.empty() && order[0] == 0, "turn order: independents first");
    for (std::uint8_t want = 1; want <= 3; ++want) {
        int seen = 0;
        for (std::uint8_t v : order) seen += (v == want) ? 1 : 0;
        check(seen == 1, "turn order: player " + std::to_string(want) +
                             " appears once");
    }
    check(aow::generate_turn_order(3, 12345u) == order,
          "turn order: deterministic for a seed");

    // -- The turn loop (TPlayerControl.NewTurn / NewDay). --------------------
    aow::GameState g = tut;
    // Day 1: no gold step, but the turn counter advances.
    g.day = 1;
    g.players[1].turn_count = 0;
    const std::int32_t gold0 = g.players[1].gold;
    g.begin_player_turn(1);
    check(g.players[1].gold == gold0, "day 1: gold unchanged");
    check(g.players[1].turn_count == 1, "day 1: turn count advanced");

    // Day 2: gold += income - upkeep; income = base (+AI bonus).
    g.players[1].base_income = 10;
    g.players[1].army_upkeep = 3;
    g.players[1].magic_upkeep = 1;
    g.players[1].gold = 100;
    g.day = 2;
    g.begin_player_turn(1);
    check(g.players[1].income == 10, "day 2: income = base (human)");
    check(g.players[1].gold == 100 + 10 - 4, "day 2: gold += income - upkeep");
    check(g.players[1].upkeep() == 4, "upkeep = army + magic");

    // A deficit clamps gold at 0 and lowers the food penalty by 5.
    g.players[1].gold = 2;
    g.players[1].base_income = 0;
    g.players[1].food_penalty = 0;
    g.begin_player_turn(1);
    check(g.players[1].gold == 0, "deficit: gold clamped to 0");
    check(g.players[1].food_penalty == -5, "deficit: food penalty -5");

    // AI types 6 / 0xa add the literal +50 / +100 (UpdateIncome).
    aow::GameState ai = tut;
    ai.players[1].base_income = 20;
    ai.players[1].type = aow::PlayerType::AI6;
    ai.update_income(1);
    check(ai.players[1].income == 70, "ai6 income = base + 50");
    ai.players[1].type = aow::PlayerType::AI10;
    ai.update_income(1);
    check(ai.players[1].income == 120, "ai10 income = base + 100");

    // A full round advances the day and rebuilds the order.
    aow::GameState loop = tut;
    loop.begin_day(9u);  // build day 1 order without advancing the day
    check(loop.day == 1, "begin_day: day stays 1");
    check(loop.turn_order.size() == 3, "begin_day: turn order built");
    loop.new_day(777u);  // day 1 -> 2, order rebuilt
    check(loop.day == 2, "new_day: day advanced to 2");
    check(loop.turn_order.size() == 3, "new_day: turn order built");
    // Each full round (one turn per player) must advance the day by exactly 1.
    bool exact = true;
    for (int round = 0; round < 3; ++round) {
        const int before = loop.day;
        const std::size_t n = loop.turn_order.size();
        for (std::size_t i = 0; i < n; ++i) loop.advance_turn(777u);
        if (loop.day != before + 1) exact = false;
    }
    check(exact, "full round: day advances exactly once per round");

    // Turn limit: reaching it eliminates every player.
    aow::GameState lim = tut;
    lim.turn_limit = 3;
    lim.new_day(1u);  // day 2
    lim.new_day(1u);  // day 3 == limit
    check(lim.day == 3, "turn limit: reached");
    bool all_out = true;
    for (const aow::Player& p : lim.players) all_out = all_out && !p.active();
    check(all_out, "turn limit: every player eliminated");

    std::printf("game_state tests: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}