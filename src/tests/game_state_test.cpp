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
#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/structure_capture.hpp"
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

aow::GameState load_with(const std::string& root, const std::string& path,
                         bool& ok) {
    aow::Map map;
    std::string err;
    ok = aow::load_map(path, map, err);
    if (!ok) {
        std::fprintf(stderr, "FAIL: load_map(%s): %s\n", path.c_str(), err.c_str());
        return {};
    }
    // The building/upkeep economy needs the map-object resource table (city
    // size + crop hex tables) and the unit catalog (TUnit resource level).
    std::vector<aow::MapResource> resources;
    aow::UnitCatalog catalog;
    aow::load_map_resources(root + "\\Release\\Release.hss", resources, err);
    aow::load_unit_catalog(root, catalog, err);
    aow::GameState gs;
    ok = aow::load_game_state(map, resources, &catalog, gs, err);
    if (!ok) {
        std::fprintf(stderr, "FAIL: load_game_state(%s): %s\n", path.c_str(),
                     err.c_str());
        return {};
    }
    return gs;
}

aow::GameState load(const std::string& root, const std::string& path,
                    bool& ok) {
    return load_with(root, path, ok);
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
        load(root, root + "\\Scenario\\Custom\\Tutorial.hsm", ok);
    if (!ok) return 1;

    check(tut.loaded, "tutorial: roster loaded");
    check(tut.players.size() == 3, "tutorial: 3 roster entries");
    check(tut.player_count() == 2, "tutorial: 2 real players");
    if (tut.players.size() == 3) {
        check(tut.players[0].index == 0, "tutorial: entry 0 is independents");
        check(tut.players[0].race == 0xff, "tutorial: independents race 0xff");
        check(tut.players[0].start_gold == 250 &&
                  tut.players[0].gold == 250,
              "tutorial: independents start/live gold 250");
        check(tut.players[0].display_name() == "Independents",
              "tutorial: independents display name");
        check(tut.players[1].index == 1, "tutorial: entry 1 index 1");
        check(tut.players[1].race == 5, "tutorial: player 1 race 5");
        check(tut.players[1].start_gold == 1000 &&
                  tut.players[1].gold == 1000,
              "tutorial: player 1 start/live gold 1000");
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
        // Per-turn economy (TPlayer.UpdateIncome + the structure income
        // sources):
        //   independents: the CITY3 resource -> size 3, 4 crops (15 + 8 = 23,
        //     +25% merchandise round(5.75)=6 -> 29) + a gold mine 10 = 39.
        //   player 2 (Borok): a size-1 city, 3 crops (5 + 6 = 11, +25%
        //     round(2.75)=3 -> 14).
        // Borok is CPU Lord (type 3), so TPlayer.UpdateIncome doubles it to 28.
        check(tut.players[0].building_income == 39,
              "tutorial: independents building income 39");
        check(tut.players[1].building_income == 0,
              "tutorial: player 1 has no income buildings");
        check(tut.players[2].building_income == 14,
              "tutorial: player 2 building income 14");
        check(tut.players[2].income == 28,
              "tutorial: player 2 CPU Lord income x2");
        // TArmy.UpdateUpkeep. Four/seven units, leaders cost 0, heroes
        // 5+2*level and TUnits (unit resource level + 1) * 2.
        check(tut.players[0].army_upkeep == 28,
              "tutorial: independents army upkeep 28");
        check(tut.players[1].army_upkeep == 22,
              "tutorial: player 1 army upkeep 22");
        check(tut.players[2].army_upkeep == 41,
              "tutorial: player 2 army upkeep 41");
        // Magic bar data: TLeader rw tag 0x2e sphere picks, magic rw tag 0x1b
        // starting mana (StartNewMap copies it to the live mana), and the
        // power-node / node-power income feeding GetManaIncome.
        check(tut.players[1].magic.starting_mana == 100 &&
                  tut.players[1].magic.mana == 100,
              "tutorial: player 1 starting/live mana 100");
        check(tut.players[1].leader.sphere_picks.size() == 3 &&
                  tut.players[1].leader.sphere_picks[0] == 1 &&
                  tut.players[1].leader.sphere_picks[1] == 5 &&
                  tut.players[1].leader.sphere_picks[2] == 1,
              "tutorial: player 1 leader sphere picks [1,5,1]");
        check(tut.players[2].magic.starting_mana == 50 &&
                  tut.players[2].magic.mana == 50,
              "tutorial: player 2 starting/live mana 50");
        check(tut.players[0].magic.power_income == 10 &&
                  tut.players[0].magic.mana_income == 5,
              "tutorial: independents power node 10 -> mana income 5");
        check(tut.players[1].magic.power_income == 0 &&
                  tut.players[1].magic.mana_income == 0,
              "tutorial: player 1 no power sources");
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
            load(root, root + "\\Scenario\\Custom\\Thinreed Lake.hsm", ok);
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
        load(root, root + "\\Scenario\\Campaign\\Dwarf1.CSM", ok);
    if (!ok) return 1;
    check(dwf.players.size() == 4, "dwarf1: 4 roster entries");
    check(dwf.player_count() == 3, "dwarf1: 3 real players");
    if (dwf.players.size() == 4) {
        check(dwf.players[1].start_gold == 125 &&
                  dwf.players[1].gold == 125,
              "dwarf1: player 1 start/live gold 125");
        check(dwf.players[1].race == 6, "dwarf1: player 1 race 6");
        check(dwf.players[2].start_gold == 250 &&
                  dwf.players[2].gold == 250,
              "dwarf1: player 2 start/live gold 250");
        check(dwf.players[3].start_gold == 275 &&
                  dwf.players[3].gold == 275,
              "dwarf1: player 3 start/live gold 275");
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
            load(root, root + "\\Scenario\\Custom\\AoW Europe Heaven Games.hsm", ok);
        if (!ok) return 1;
        check(eu.players.size() == 9, "europe: 9 roster entries");
        check(eu.players[0].race == 0xff, "europe: independents race 0xff");
        // Player 1's record carries no race tag -> Humans, and it is the Human.
        check(eu.players[1].race == 0, "europe: unraced player 1 = Humans");
        check(eu.players[1].type == aow::PlayerType::Human,
              "europe: player 1 = Human");
        check(eu.players[2].race == 4, "europe: player 2 race 4 (Elves)");

        const aow::GameState bb =
            load(root, root + "\\Scenario\\Custom\\Barondir Bay.HSM", ok);
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

    // Day 2: gold += income - upkeep; income = base + building income.
    g.players[1].base_income = 10;
    g.players[1].building_income = 0;
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
    g.players[1].building_income = 0;
    g.players[1].food_penalty = 0;
    g.begin_player_turn(1);
    check(g.players[1].gold == 0, "deficit: gold clamped to 0");
    check(g.players[1].food_penalty == -5, "deficit: food penalty -5");

    // TPlayerMagicControl.NewTurn (0x5577cc2c): the 0x20020002 turn-start
    // message is posted outside TPlayerControl.NewTurn's day!=1 gold branch, so
    // mana income is credited on day 1 too (unlike gold). Mana is clamped at 0
    // when the enchantment upkeep exceeds income (OutOfMana).
    aow::GameState mg = tut;
    mg.players[1].magic.mana = 7;
    mg.players[1].magic.mana_income = 5;
    mg.players[1].magic.mana_upkeep = 0;
    mg.day = 1;
    mg.begin_player_turn(1);
    check(mg.players[1].magic.mana == 12, "day 1: mana += mana income");
    mg.day = 2;
    mg.players[1].magic.mana = 7;
    mg.players[1].magic.mana_upkeep = 2;
    mg.begin_player_turn(1);
    check(mg.players[1].magic.mana == 10, "day 2: mana += income - upkeep");
    mg.players[1].magic.mana = 2;
    mg.players[1].magic.mana_income = 5;
    mg.players[1].magic.mana_upkeep = 9;
    mg.begin_player_turn(1);
    check(mg.players[1].magic.mana == 0, "mana deficit: clamped to 0");

    // AI multipliers from TPlayer.UpdateIncome (0x55751f88):
    // AI2 x1, AI3 x2, AI6 x2.5+50, AI7 x3+100.
    aow::GameState ai = tut;
    ai.players[1].building_income = 0;
    ai.players[1].base_income = 10;
    ai.players[1].type = aow::PlayerType::AI3;
    ai.update_income(1);
    check(ai.players[1].income == 20, "ai3 income = base * 2");
    ai.players[1].base_income = 0;
    ai.players[1].type = aow::PlayerType::AI6;
    ai.update_income(1);
    check(ai.players[1].income == 50, "ai6 income = +50 flat");
    ai.players[1].type = aow::PlayerType::AI7;
    ai.update_income(1);
    check(ai.players[1].income == 100, "ai7 income = +100 flat");
    // King/Emperor starting gold (TAoWHSMap.StartNewMap).
    ai.players[1].start_gold = 100;
    ai.start_new_map();
    check(ai.players[1].gold == 600, "ai7 start gold = start + 500");
    ai.players[1].type = aow::PlayerType::AI6;
    ai.start_new_map();
    check(ai.players[1].gold == 350, "ai6 start gold = start + 250");

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

    // -- The end-turn rule: a turn only passes once its owner has ended it ----
    // `TAoWHSMap.EndTurn` -> `TPlayerControl.EndTurn` -> `TTurnPlayerControl
    // .EndTurn`: the hand-over happens only when the player who ended *is* the
    // one on turn, and the day only advances when the order wraps.
    aow::GameState turn = tut;
    turn.begin_day(5u);
    const std::uint8_t first = turn.turn_order.front();
    turn.begin_first_turn(first);
    check(turn.current() == first, "end-turn: begin_first_turn puts the seat on turn");
    check(!turn.players[turn.index_of(first)].end_turn,
          "end-turn: a fresh turn starts with the flag down");
    // Ending a seat that is not on turn sets its flag but does not hand over.
    const std::uint8_t other = turn.turn_order.back();
    if (other != first) {
        check(turn.end_turn(other), "end-turn: a seat may still mark itself done");
        check(turn.current() == first, "end-turn: no hand-over for a seat off turn");
        check(turn.cancel_end_turn(other), "end-turn: cancel clears the flag");
    }
    // Ending the seat on turn hands the turn on...
    const std::uint8_t before_cur = turn.current();
    check(turn.end_turn(before_cur), "end-turn: the seat on turn ends its turn");
    check(turn.current() != before_cur, "end-turn: the turn moved on");
    // A second end-turn for the same seat is refused while its flag stands.
    check(!turn.end_turn(before_cur),
          "end-turn: an ended seat cannot end twice");
    // A seat's flag is cleared when its turn comes round again: run one full
    // round (the wrap starts day 2, whose NewTurn clears the flag).
    aow::GameState wrap = tut;
    wrap.begin_day(5u);
    wrap.begin_first_turn(wrap.turn_order.front());
    for (std::size_t i = 0; i < wrap.turn_order.size(); ++i) {
        const std::uint8_t c = wrap.current();
        if (c == 0xff) break;
        wrap.end_turn(c);
    }
    check(wrap.day == 2, "end-turn: a full round of endings advances the day");
    check(!wrap.players[wrap.index_of(wrap.turn_order.front())].end_turn,
          "end-turn: the new day clears the seat's end-turn flag");
    // The event log gate: while the log is executing, EndTurn refuses.
    aow::GameState busy = tut;
    busy.begin_day(5u);
    busy.begin_first_turn(busy.turn_order.front());
    busy.event_log_busy = true;
    busy.event_log_count = 3;
    check(!busy.end_turn(busy.current()), "end-turn: refused while the event log runs");
    busy.event_log_count = 0;
    check(busy.end_turn(busy.current()), "end-turn: allowed once the log is empty");

    // A day can only pass after *every* player ended a turn: three seats, three
    // hand-overs from a known start.
    aow::GameState round = tut;
    round.begin_day(11u);
    round.turn_in_order = 0;
    round.current_player = round.turn_order[0];
    round.players[round.index_of(round.turn_order[0])].end_turn = false;
    int handovers = 0;
    for (std::size_t i = 0; i < round.turn_order.size(); ++i) {
        const std::uint8_t c = round.current();
        if (c == 0xff) break;
        if (round.end_turn(c)) ++handovers;
    }
    check(handovers == static_cast<int>(round.turn_order.size()),
          "end-turn: one hand-over per seat in a round");
    check(round.day == 2, "end-turn: the day advances exactly when the order wraps");

    // Turn limit: reaching it eliminates every player.
    aow::GameState lim = tut;
    lim.turn_limit = 3;
    lim.new_day(1u);  // day 2
    lim.new_day(1u);  // day 3 == limit
    check(lim.day == 3, "turn limit: reached");
    bool all_out = true;
    for (const aow::Player& p : lim.players) all_out = all_out && !p.active();
    check(all_out, "turn limit: every player eliminated");

    // -- Building acquisition (docs/29_BUILDING_ACQUISITION.md) ---------------
    // A synthetic city with a 2-hex TTerrainList so UpdatePlayer can see a
    // second covered hex; no scenario file is needed for the rule checks.
    const std::vector<std::uint8_t> races{0xff, 5, 10};
    std::vector<aow::MapResource> sres(1);
    sres[0].id = 0x16;
    sres[0].class_id = aow::kCityResourceClassId;
    sres[0].terrain_list = {0x0101, 0x0101};  // home hex + hn index 1
    auto make_city = [&](aow::Map& m, aow::MapHs& city) {
        m.levels.resize(1);
        m.levels[0].width = 5;
        m.levels[0].height = 5;
        m.levels[0].tiles.assign(25, aow::MapTile{});
        city = aow::MapHs{};
        city.class_id = aow::kCityClassId;
        city.resource = 0x16;
        city.structure_owner = 0;
        city.city_level = 1;
        city.city_name = "Testville";
        m.levels[0].tiles[2 * 5 + 2].objects.push_back(city);
    };
    const aow::StructureRef kRef{0, 2, 2, 0};

    // `UpdateFlagID`: a neutral city flies Flags.ILB's neutral shield with the
    // level's pole bits; an owned city the owner's race (+ pole); a razed
    // structure nothing. A non-city has no neutral flag (+0x64 == 0).
    {
        aow::MapHs city;
        city.class_id = aow::kCityClassId;
        city.structure_owner = 0;
        city.city_level = 1;
        check(aow::structure_flag_id(city, races) == 0x20,
              "capture: unowned city flies the neutral flag");
        city.city_level = 3;
        city.structure_owner = 2;
        check(aow::structure_flag_id(city, races) == 10 + 2 * 0x40,
              "capture: owned city flag = race | (level-1)<<6");
        city.razed = true;
        check(aow::structure_flag_id(city, races) == 0x3f,
              "capture: a razed city flies nothing");
        aow::MapHs mine;
        mine.class_id = aow::kMineClassId;
        mine.structure_owner = 0;
        check(aow::structure_flag_id(mine, races) == 0x3f,
              "capture: an unowned mine flies nothing");
        mine.structure_owner = 1;
        check(aow::structure_flag_id(mine, races) == 5,
              "capture: an owned mine flies the owner's race flag");
    }

    // `GetMoveAction`: a hostile structure answers 1 (attack); an independent
    // non-city (owner 0, no neutral flag) answers 0 (walk in and claim it).
    {
        aow::MapHs mine;
        mine.class_id = aow::kMineClassId;
        mine.structure_owner = 0;
        check(aow::structure_move_action(mine, 1) == 0,
              "capture: an unowned mine is entered, not attacked");
        mine.structure_owner = 2;
        check(aow::structure_move_action(mine, 1) == 1,
              "capture: another player's mine is attacked");
        check(aow::structure_move_action(mine, 2) == 0,
              "capture: your own mine is entered");
        aow::MapHs city;
        city.class_id = aow::kCityClassId;
        city.structure_owner = 0;
        check(aow::structure_move_action(city, 1) == 1,
              "capture: an independent city is attacked");
        city.structure_owner = -1;  // 0xff
        check(aow::structure_move_action(city, 1) == 0,
              "capture: a city nobody owns is entered");
        city.structure_owner = 2;
        city.razed = true;
        check(aow::structure_move_action(city, 1) == 0,
              "capture: a razed city is not attacked");
    }

    // `SetPlayer`: neutral -> player 1 is silent, player 1 -> player 2 logs the
    // capture; the flag follows; the owner byte is 0xff for -1.
    {
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        aow::StructureChange ch =
            aow::structure_set_player(m, kRef, 1, races, sres);
        const aow::MapHs* c = aow::structure_object(m, kRef);
        check(ch.changed && ch.old_owner == 0 && ch.new_owner == 1,
              "capture: neutral city changes to player 1");
        check(!ch.capture_logged && ch.text.empty(),
              "capture: taking an independent city is silent");
        check(c != nullptr && c->structure_owner == 1 && c->flag_id == 5,
              "capture: owner byte + player 1's race flag");
        ch = aow::structure_set_player(m, kRef, 2, races, sres);
        c = aow::structure_object(m, kRef);
        check(ch.changed && ch.capture_logged && ch.old_owner == 1,
              "capture: taking a player's city is logged");
        check(!ch.text.empty(), "capture: the event log carries a line");
        check(c != nullptr && c->structure_owner == 2 && c->flag_id == 10,
              "capture: owner byte + player 2's race flag");
        ch = aow::structure_set_player(m, kRef, 2, races, sres);
        check(!ch.changed, "capture: setting the same owner is a no-op");
        ch = aow::structure_set_player(m, kRef, -1, races, sres);
        c = aow::structure_object(m, kRef);
        check(ch.changed && c != nullptr && c->structure_owner == 0xff,
              "capture: SetPlayer(-1) stores 0xff");
        check(c != nullptr && c->flag_id == 0x20,
              "capture: an ownerless city returns to the neutral flag");
    }

    // `SetPlayer`'s rebuild gate: a ruin only changes hands while a builder is
    // working on it (GetRazed && !GetBuilding -> return).
    {
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        aow::MapHs* c = aow::structure_object(m, kRef);
        if (c != nullptr) c->razed = true;
        const aow::StructureChange ch =
            aow::structure_set_player(m, kRef, 1, races, sres);
        check(!ch.changed && ch.refused_razed,
              "capture: a ruin refuses to change hands");
        if (c != nullptr) c->building = true;
        const aow::StructureChange built =
            aow::structure_set_player(m, kRef, 1, races, sres);
        check(built.changed, "capture: a ruin under construction can be taken");
    }

    // `TProductionControl.PlayerChanged`: an actively producing city keeps item
    // 0 only; an idle city loses the whole queue (and its current item).
    {
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        aow::MapHs* c = aow::structure_object(m, kRef);
        if (c != nullptr) {
            c->production_queue = {{1, 0, 0}, {1, 1, 0}, {2, 2, 0}};
            c->production_state = 2;
            c->production_current_present = true;
            c->loop_queue = true;
        }
        aow::structure_set_player(m, kRef, 1, races, sres);
        c = aow::structure_object(m, kRef);
        check(c != nullptr && c->production_queue.size() == 1 &&
                  c->production_queue.front().index == 0,
              "capture: an active city keeps only the running queue item");
        check(c != nullptr && c->production_state == 2 && !c->loop_queue,
              "capture: production keeps running but the loop clears");

        aow::Map m2;
        aow::MapHs city2;
        make_city(m2, city2);
        aow::MapHs* d = aow::structure_object(m2, kRef);
        if (d != nullptr) {
            d->production_queue = {{1, 0, 0}, {1, 1, 0}};
            d->production_state = 0;
            d->production_current_present = true;
            d->production_turns_left = 4;
        }
        aow::structure_set_player(m2, kRef, 1, races, sres);
        d = aow::structure_object(m2, kRef);
        check(d != nullptr && d->production_queue.empty() &&
                  !d->production_current_present,
              "capture: an idle city loses its queue and current item");
        check(d != nullptr && d->production_turns_left == 0,
              "capture: the dropped item's turn counter clears");
    }

    // `UpdatePlayer` / `ArmyPlaced`: an army standing on the structure claims it;
    // two different owners make the (multi-hex) structure contested (owner -1).
    {
        auto add_army = [](aow::Map& m, int x, int y, int owner) {
            aow::MapHs a;
            a.class_id = aow::kMapArmyHsClassId;
            a.owner = static_cast<std::uint8_t>(owner);
            a.units.push_back(aow::MapUnit{});
            m.levels[0].tiles[static_cast<std::size_t>(y) * 5 +
                                static_cast<std::size_t>(x)]
                .objects.push_back(a);
        };
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        add_army(m, 2, 2, 2);
        aow::StructureChange ch = aow::structure_update_player(m, kRef, races, sres);
        const aow::MapHs* c = aow::structure_object(m, kRef);
        check(ch.changed && c != nullptr && c->structure_owner == 2,
              "capture: a standing army claims the city");
        check(!ch.capture_logged && ch.text.empty(),
              "capture: claiming an independent city is silent");

        aow::Map m2;
        aow::MapHs city2;
        make_city(m2, city2);
        add_army(m2, 2, 2, 2);
        add_army(m2, 2, 1, 1);  // the city's second covered hex (hn index 1)
        ch = aow::structure_update_player(m2, kRef, races, sres);
        c = aow::structure_object(m2, kRef);
        check(ch.changed && c != nullptr &&
                  static_cast<std::int8_t>(c->structure_owner) == -1,
              "capture: two owners on a city's hexes make it contested");
        check(!ch.capture_logged,
              "capture: an independent city lost to a contest is silent");
        check(aow::structure_has_foreign_army(m, kRef, 2, sres) == false,
              "capture: a sole army is not foreign to its own player");
        check(aow::structure_has_foreign_army(m2, kRef, 2, sres),
              "capture: the second player's army is foreign to player 2");
    }

    // `SetRazed` clears the owner and the city's wall/production; an army on the
    // hex keeps the ruin (SetPlayer's UpdatePlayer pass), exactly like the game.
    {
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        aow::MapHs* c = aow::structure_object(m, kRef);
        if (c != nullptr) {
            c->structure_owner = 1;
            c->wall_type = 2;
            c->production_queue = {{1, 0, 0}};
            c->production_current_present = true;
        }
        aow::structure_set_razed(m, kRef, true, races, sres);
        c = aow::structure_object(m, kRef);
        check(c != nullptr && c->razed && (c->city_flags & 0x02) != 0,
              "capture: razing sets the city's razed bit");
        check(c != nullptr && c->wall_type == 0 && c->production_queue.empty(),
              "capture: razing drops the wall and the production");
        check(c != nullptr && static_cast<std::int8_t>(c->structure_owner) == -1,
              "capture: razing an empty city clears the owner");
        check(c != nullptr && c->flag_id == 0x3f,
              "capture: a ruin flies no flag");

        aow::Map m2;
        aow::MapHs city2;
        make_city(m2, city2);
        aow::MapHs* d = aow::structure_object(m2, kRef);
        if (d != nullptr) d->structure_owner = 1;
        aow::MapHs a;
        a.class_id = aow::kMapArmyHsClassId;
        a.owner = 1;
        a.units.push_back(aow::MapUnit{});
        m2.levels[0].tiles[2 * 5 + 2].objects.push_back(a);
        aow::structure_set_razed(m2, kRef, true, races, sres);
        d = aow::structure_object(m2, kRef);
        check(d != nullptr && d->razed && d->structure_owner == 1,
              "capture: the razing army keeps the ruin (UpdatePlayer)");
    }

    // `GetRebuildInfo` + `ExecuteBuild`: the builder clears the razed bit and
    // becomes the owner.
    {
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        const aow::RebuildInfo info =
            aow::structure_get_rebuild_info(city, &sres[0]);
        check(info.gold == 50 && info.turns == 3,
              "capture: a 2-hex city rebuilds for 50 gold / 3 turns");
        aow::structure_set_razed(m, kRef, true, races, sres);
        const aow::StructureChange ch =
            aow::structure_complete_rebuild(m, kRef, 2, races, sres);
        const aow::MapHs* c = aow::structure_object(m, kRef);
        check(ch.changed && c != nullptr && !c->razed && !c->building,
              "capture: a finished builder clears the razed state");
        check(c != nullptr && c->structure_owner == 2 && c->flag_id == 10,
              "capture: a rebuilt city belongs to its builder");
        check(!ch.text.empty(), "capture: the rebuild is logged");
    }

    // `TCity.CheckRebellion`: status 0 rebels on roll <= 50, status 1 on <= 10,
    // status >= 2 never; a rebellion hands the city to the independents (0).
    {
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        aow::MapHs* c = aow::structure_object(m, kRef);
        if (c != nullptr) c->structure_owner = 1;
        check(aow::structure_city_check_rebellion(m, kRef, 0, 50, races, sres),
              "capture: an unruly city rebels at roll 50");
        c = aow::structure_object(m, kRef);
        check(c != nullptr && c->structure_owner == 0,
              "capture: a rebellion returns the city to the independents");

        aow::Map calm;
        aow::MapHs calm_city;
        make_city(calm, calm_city);
        aow::MapHs* cc = aow::structure_object(calm, kRef);
        if (cc != nullptr) cc->structure_owner = 1;
        check(!aow::structure_city_check_rebellion(calm, kRef, 2, 0, races, sres),
              "capture: a stable city never rebels");

        aow::Map unrest;
        aow::MapHs unrest_city;
        make_city(unrest, unrest_city);
        aow::MapHs* uc = aow::structure_object(unrest, kRef);
        if (uc != nullptr) uc->structure_owner = 1;
        check(!aow::structure_city_check_rebellion(unrest, kRef, 1, 11, races, sres),
              "capture: an unrest city survives roll 11");
        check(aow::structure_city_check_rebellion(unrest, kRef, 1, 10, races, sres),
              "capture: an unrest city rebels at roll 10");
    }

    // `FinalizeCombat`: results 3 and 6 hand the structure to the attacker; a
    // loss (4) or a draw leaves it alone.
    {
        for (const aow::StructureCombatOutcome outcome :
             {aow::StructureCombatOutcome::won,
              aow::StructureCombatOutcome::retreated}) {
            aow::Map m;
            aow::MapHs city;
            make_city(m, city);
            aow::MapHs* c = aow::structure_object(m, kRef);
            if (c != nullptr) c->structure_owner = 1;
            const aow::StructureChange ch = aow::structure_finalize_combat(
                m, kRef, outcome, 2, races, sres);
            c = aow::structure_object(m, kRef);
            check(ch.changed && c != nullptr && c->structure_owner == 2,
                  "capture: a won/retreated battle takes the city");
            check(ch.capture_logged && !ch.text.empty(),
                  "capture: the battle capture is logged");
        }
        aow::Map m;
        aow::MapHs city;
        make_city(m, city);
        aow::MapHs* c = aow::structure_object(m, kRef);
        if (c != nullptr) c->structure_owner = 1;
        const aow::StructureChange ch = aow::structure_finalize_combat(
            m, kRef, aow::StructureCombatOutcome::lost, 2, races, sres);
        c = aow::structure_object(m, kRef);
        check(!ch.changed && c != nullptr && c->structure_owner == 1,
              "capture: a lost battle leaves the city with its owner");
    }

    // The shipped Tutorial: Gobton (32,21) is Borok's 2-hex city, Kilia (16,25)
    // and the mine at (18,22) belong to the independents. These pin
    // `structures_at`, `structure_size_hexes` and the auto-capture on a real map.
    {
        aow::Map tmap;
        std::vector<aow::MapResource> tres;
        std::string terr;
        if (aow::load_map(root + "\\Scenario\\Custom\\Tutorial.hsm", tmap, terr) &&
            aow::load_map_resources(root + "\\Release\\Release.hss", tres, terr)) {
            const auto gob =
                aow::structures_at(tmap, 0, 32, 21, tres);
            const aow::MapHs* g =
                gob.empty() ? nullptr : aow::structure_object(tmap, gob.front());
            check(g != nullptr && g->class_id == aow::kCityClassId &&
                      g->city_name == "Gobton" && g->structure_owner == 2,
                  "capture: Tutorial (32,21) is Borok's city Gobton");
            // `TCity.GetSize` counts the terrain-list entries that carry a
            // terrain; City1 (id 0x16) has one, City3 (id 0x3a) three.
            check(g != nullptr &&
                      aow::structure_size_hexes(
                          aow::find_map_resource(tres, g->resource)) == 1,
                  "capture: Gobton is a 1-hex City1");
            const auto kil = aow::structures_at(tmap, 0, 16, 25, tres);
            const aow::MapHs* k =
                kil.empty() ? nullptr : aow::structure_object(tmap, kil.front());
            check(k != nullptr && k->class_id == aow::kCityClassId &&
                      k->city_name == "Kilia" && k->structure_owner == 0,
                  "capture: Tutorial (16,25) is the independent city Kilia");
            check(k != nullptr &&
                      aow::structure_size_hexes(
                          aow::find_map_resource(tres, k->resource)) == 3,
                  "capture: Kilia is a 3-hex City3");
            const auto mine = aow::structures_at(tmap, 0, 18, 22, tres);
            const aow::MapHs* mi =
                mine.empty() ? nullptr : aow::structure_object(tmap, mine.front());
            check(mi != nullptr && mi->class_id == aow::kMineClassId &&
                      mi->structure_owner == 0,
                  "capture: Tutorial (18,22) is an independent mine");
            if (mi != nullptr) {
                check(aow::structure_move_action(*mi, 1) == 0,
                      "capture: that mine is entered rather than attacked");
                // `TArmyHS.PlaceOnMap` -> ArmyPlaced -> UpdatePlayer.
                aow::MapHs army;
                army.class_id = aow::kMapArmyHsClassId;
                army.owner = 1;
                army.units.push_back(aow::MapUnit{});
                tmap.levels[0].tiles[22 * tmap.levels[0].width + 18]
                    .objects.push_back(army);
                const aow::StructureChange ch =
                    aow::structure_update_player(tmap, mine.front(),
                                                 aow::map_player_races(tmap), tres);
                mi = aow::structure_object(tmap, mine.front());
                check(ch.changed && mi != nullptr && mi->structure_owner == 1,
                      "capture: stepping on the Tutorial mine claims it");
            }
        } else {
            check(false, "capture: Tutorial map/resources load");
        }
    }

    // -- Gold piles: Gems.TGems (docs/28_ECONOMY.md) --------------------------
    // `Gems.TGems.GenerateGemsAmount@557c5010` draws from `TAoWHSMap.Random`,
    // i.e. Borland's `System._RandInt`: RandSeed := RandSeed * $08088405 + 1,
    // result := high dword(RandSeed * range). The reference values below were
    // produced independently from that arithmetic with the Tutorial map's own
    // seed (rw tag 0x1c = 0x18e2d3b6), sizes 1, 2, 3, 4 in that order.
    {
        std::uint32_t seed = 0x18e2d3b6u;
        std::uint8_t size = 0;
        check(aow::generate_gems_amount(1, seed, size) == 43 && size == 1,
              "gems: size 1 -> Random(25) + 25 = 43");
        check(aow::generate_gems_amount(2, seed, size) == 53 && size == 2,
              "gems: size 2 -> Random(25) + 50 = 53");
        check(aow::generate_gems_amount(3, seed, size) == 111 && size == 3,
              "gems: size 3 -> Random(50) + 100 = 111");
        check(aow::generate_gems_amount(4, seed, size) == 135 && size == 3,
              "gems: size 4 draws Random(3) + 1 (3 here) and stores it back");
        check(seed == 3065659323u,
              "gems: the map RNG state advances exactly like Delphi's RandSeed");
        // An unknown size class leaves the amount untouched and draws nothing.
        size = 0;
        check(aow::generate_gems_amount(7, seed, size, 777) == 777 && size == 7 &&
                  seed == 3065659323u,
              "gems: an unknown size class leaves the amount alone");
    }

    // The size bands, over many draws.
    {
        std::uint32_t seed = 12345u;
        bool bands = true;
        for (int i = 0; i < 200; ++i) {
            std::uint8_t size = 0;
            const int a = aow::generate_gems_amount(1, seed, size);
            const int b = aow::generate_gems_amount(2, seed, size);
            const int c = aow::generate_gems_amount(3, seed, size);
            bands = bands && a >= 25 && a <= 49 && b >= 50 && b <= 74 &&
                    c >= 100 && c <= 149;
        }
        check(bands, "gems: amounts stay inside 25-49 / 50-74 / 100-149");
    }

    // A synthetic map: the generation pass walks level/tile/placement order, so
    // a non-gems object between two piles must not consume a draw, and the seed
    // advances once per known size class.
    {
        aow::Map gm;
        gm.levels.resize(1);
        gm.levels[0].width = 3;
        gm.levels[0].height = 1;
        gm.levels[0].tiles.assign(3, aow::MapTile{});
        gm.rand_seed = 0x18e2d3b6u;
        auto add = [&](int x, std::uint32_t cls, std::uint8_t size) {
            aow::MapHs o;
            o.class_id = cls;
            if (cls == aow::kGemsClassId) o.gems_size = size;
            gm.levels[0].tiles[static_cast<std::size_t>(x)].objects.push_back(o);
        };
        add(0, aow::kGemsClassId, 1);
        add(1, aow::kTerrainMoClassId, 0);  // a tree: no draw
        add(1, aow::kGemsClassId, 2);
        add(2, aow::kGemsClassId, 3);
        check(aow::generate_gems_amounts(gm) == 3, "gems: three piles generated");
        const aow::MapTile& t0 = gm.levels[0].tiles[0];
        const aow::MapTile& t1 = gm.levels[0].tiles[1];
        const aow::MapTile& t2 = gm.levels[0].tiles[2];
        check(t0.objects[0].gems_gold == 43 &&
                  t1.objects[1].gems_gold == 53 &&
                  t2.objects[0].gems_gold == 111,
              "gems: amounts follow the map's own RNG sequence (43/53/111)");
        check(gm.rand_seed == 1000428541u,
              "gems: the pass leaves the advanced seed on the map");
        // Non-gems objects keep their defaults.
        check(t1.objects[0].gems_gold == 0 && t1.objects[0].gems_size == 2,
              "gems: a tree is not touched by the generation pass");
    }

    // `ArmyEntered`: an owned army collects the pile and it is destroyed; the
    // independents (player 0) and an unknown player do not.
    {
        aow::Map gm;
        gm.levels.resize(1);
        gm.levels[0].width = 3;
        gm.levels[0].height = 1;
        gm.levels[0].tiles.assign(3, aow::MapTile{});
        aow::MapHs pile;
        pile.class_id = aow::kGemsClassId;
        pile.gems_size = 2;
        pile.gems_gold = 60;
        gm.levels[0].tiles[1].objects.push_back(pile);

        aow::GameState gs = tut;
        const std::size_t seat2 = gs.index_of(2);
        const int gold2 = gs.players[seat2].gold;

        const aow::GoldPickup none = aow::gems_army_entered(gm, gs, 0, 1, 0, 0);
        check(!none.picked() && !none.total(),
              "gems: the independents do not collect a pile");
        check(gm.levels[0].tiles[1].objects.size() == 1 &&
                  gs.players[seat2].gold == gold2,
              "gems: a skipped pile stays on the map");
        const aow::GoldPickup unknown =
            aow::gems_army_entered(gm, gs, 0, 1, 0, 9);
        check(!unknown.picked() && gm.levels[0].tiles[1].objects.size() == 1,
              "gems: an unknown player cannot collect");

        const aow::GoldPickup p2 = aow::gems_army_entered(gm, gs, 0, 1, 0, 2);
        check(p2.picked() && p2.amounts.size() == 1 && p2.amounts[0] == 60 &&
                  p2.total() == 60 && p2.player == 2,
              "gems: player 2 collects the 60 gold pile");
        check(gs.players[seat2].gold == gold2 + 60,
              "gems: the amount lands in the owner's gold treasury");
        check(gm.levels[0].tiles[1].objects.empty(),
              "gems: the collected pile is destroyed");
        const aow::GoldPickup again = aow::gems_army_entered(gm, gs, 0, 1, 0, 2);
        check(!again.picked() && gs.players[seat2].gold == gold2 + 60,
              "gems: a hex with no pile pays nothing the second time");

        // An empty pile is still destroyed (the original destroys it after the
        // payout whether or not the amount is 0).
        aow::MapHs empty;
        empty.class_id = aow::kGemsClassId;
        empty.gems_gold = 0;
        gm.levels[0].tiles[1].objects.push_back(empty);
        const aow::GoldPickup zero = aow::gems_army_entered(gm, gs, 0, 1, 0, 1);
        check(zero.picked() && zero.total() == 0 &&
                  gm.levels[0].tiles[1].objects.empty(),
              "gems: a 0-gold pile is collected and removed");
    }

    // A shipped map: Barondir Bay stores five size-2 piles and its own RNG seed
    // (rw tag 0x1c), so the generated amounts are the map's own sequence.
    {
        aow::Map bm;
        std::string berr;
        const bool bw = aow::load_map(
            root + "\\Scenario\\Custom\\Barondir Bay.HSM", bm, berr);
        check(bw, "gems: Barondir Bay loads");
        if (bw) {
            check(bm.rand_seed == 0x9b75d3e0u,
                  "gems: Barondir Bay's RNG seed comes from rw tag 0x1c");
            check(aow::generate_gems_amounts(bm) == 5,
                  "gems: Barondir Bay has five piles");
            std::vector<int> got;
            for (const aow::MapLevel& lv : bm.levels) {
                for (const aow::MapTile& t : lv.tiles) {
                    for (const aow::MapHs& o : t.objects) {
                        if (o.class_id == aow::kGemsClassId) {
                            got.push_back(o.gems_gold);
                        }
                    }
                }
            }
            const std::vector<int> want{60, 67, 52, 51, 65};
            check(got == want,
                  "gems: the five amounts replay the map's RNG (60/67/52/51/65)");
            check(bm.rand_seed == 2656503917u,
                  "gems: Barondir Bay's seed advanced five times");
        }
        // The map-level day tag (rw tag 0x13) is absent from a fresh scenario,
        // which `rwInteger` reads back as 0.
        check(bm.day == 0, "gems: a fresh scenario stores no day (0)");
    }

    std::printf("game_state tests: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}