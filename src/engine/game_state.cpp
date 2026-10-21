// game_state.cpp - the AoW1 player roster + turn/day loop (Stage 2.3).
//
// See game_state.hpp for the format + the decompiled evidence. The roster lives
// in the scenario map's EObject graph at TAoWHSMap tag 0x15 (a TPlayerList); the
// reader reuses the shared property-table walk (aow/eobject.hpp) exactly like
// src/engine/map.cpp.
#include "aow/game_state.hpp"

#include "aow/endian.hpp"
#include "aow/eobject.hpp"

namespace aow {
namespace {

using Table = EObjectTable;

constexpr std::size_t kNpos = static_cast<std::size_t>(-1);

// TAoWHSMap.ReadWrite tag that holds the roster (a TPlayerList).
constexpr std::uint32_t kRosterTag = 0x15;

std::size_t find_off(const Table& t, std::uint32_t tag) {
    return eobject_find(t, tag);
}

// A small xorshift32 PRNG. The original shuffles with the map's own RNG
// (`TAoWHSMap.Random`); we only need a deterministic, well-defined permutation
// so the port's turn order is reproducible and testable.
std::uint32_t xs32(std::uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

// Parses a `TPlayerMagicControl` (TPlayer tag 0x1f).
void parse_magic(const std::uint8_t* R, std::size_t n, Table& ptab,
                 std::size_t off, PlayerMagic& out) {
    const std::size_t p = ptab.base + off;
    Table mt;
    if (!read_eobject_table(R, n, p, mt)) return;
    out.present = true;
    const std::size_t m = find_off(mt, kMagicTagMana);
    if (m != kNpos && mt.base + m + 4 <= n) {
        out.mana = read_i32(R + mt.base + m);
    }
    const std::size_t pr = find_off(mt, kMagicTagPowerToResearch);
    if (pr != kNpos && mt.base + pr + 4 <= n) {
        out.power_to_research = read_i32(R + mt.base + pr);
    }
    const std::size_t cr = find_off(mt, kMagicTagCurrentResearch);
    if (cr != kNpos && mt.base + cr + 4 <= n) {
        out.current_research = read_i32(R + mt.base + cr);
    }
    const std::size_t rp = find_off(mt, kMagicTagResearchProgress);
    if (rp != kNpos && mt.base + rp + 4 <= n) {
        out.research_progress = read_i32(R + mt.base + rp);
    }
    // tag 0x16 (the researched-spell IntegerList) is serialised through the
    // list's SaveToStream; its body is not decoded here (TODO(RE)) - the list
    // stays empty and `present` still records that the map carried one.
}

// Parses a player's TLeader (TPlayer rw tag 0x1e): a THero serialised inline
// (a bare property table - see the Leader comment in game_state.hpp). The tags
// are the THero ones the placed heroes use (src/engine/map.cpp): 7 = the face
// index, 8 = the THeroResource index, 10 = the name, 11 = the nickname.
void parse_leader(const std::uint8_t* R, std::size_t n, Table& ptab,
                  std::size_t off, Leader& out) {
    Table lt;
    if (!read_eobject_table(R, n, ptab.base + off, lt)) return;
    out.present = true;
    const std::size_t fi = find_off(lt, kHeroTagFaceIndex);
    if (fi != kNpos && lt.base + fi + 4 <= n) {
        out.face_index = read_i32(R + lt.base + fi);
    }
    const std::size_t hi = find_off(lt, kHeroTagHeroResourceIndex);
    if (hi != kNpos && lt.base + hi + 4 <= n) {
        out.hero_resource_index = read_i32(R + lt.base + hi);
    } else {
        out.hero_resource_index = -1;  // THero.Create's default
    }
    const std::size_t nm = find_off(lt, kHeroTagName);
    if (nm != kNpos) out.name = eobject_short_string(R, n, lt.base + nm);
    const std::size_t nk = find_off(lt, kHeroTagNickname);
    if (nk != kNpos) out.nickname = eobject_short_string(R, n, lt.base + nk);
}

void parse_player(const std::uint8_t* R, std::size_t n, Table& tab,
                  Player& out) {
    const std::size_t idx = find_off(tab, kPlayerTagIndex);
    if (idx != kNpos && tab.base + idx < n) out.index = R[tab.base + idx];
    // rw tag 0x2 = +0xa5 = the race. It is a plain `rwByte`
    // (TEReadStorageStream.rwByte, vtable +0x30), whose absent-tag branch
    // writes 0 - so a map that never stored a race means Humans (0), never
    // 0xff. Only an explicit tag (the independents' 0xff) differs. This is the
    // general rule that gives Europe's unraced player 1 the Humans cell.
    const std::size_t race = find_off(tab, kPlayerTagRace);
    if (race != kNpos && tab.base + race < n) out.race = R[tab.base + race];
    // TPlayer rw tag 0x3 = +0xa7 = the player type (`hPickMap` 0x557e20c8).
    const std::size_t ty = find_off(tab, kPlayerTagType);
    if (ty != kNpos && tab.base + ty < n) {
        out.type = static_cast<PlayerType>(R[tab.base + ty]);
    }
    const std::size_t gold = find_off(tab, kPlayerTagGold);
    if (gold != kNpos && tab.base + gold + 4 <= n) {
        out.gold = read_i32(R + tab.base + gold);
    }
    const std::size_t bi = find_off(tab, kPlayerTagBaseIncome);
    if (bi != kNpos && tab.base + bi + 4 <= n) {
        out.base_income = read_i32(R + tab.base + bi);
    }
    const std::size_t gems = find_off(tab, kPlayerTagGems);
    if (gems != kNpos && tab.base + gems + 4 <= n) {
        out.gems = read_i32(R + tab.base + gems);
    }
    const std::size_t tc = find_off(tab, kPlayerTagTurnCount);
    if (tc != kNpos && tab.base + tc + 4 <= n) {
        out.turn_count = read_i32(R + tab.base + tc);
    }
    const std::size_t fp = find_off(tab, kPlayerTagFoodPenalty);
    if (fp != kNpos && tab.base + fp < n) {
        out.food_penalty = static_cast<std::int8_t>(R[tab.base + fp]);
    }
    const std::size_t go = find_off(tab, kPlayerTagGameOver);
    if (go != kNpos && tab.base + go < n) out.game_over_status = R[tab.base + go];
    const std::size_t nm = find_off(tab, kPlayerTagName);
    if (nm != kNpos) out.name = eobject_short_string(R, n, tab.base + nm);
    const std::size_t c = find_off(tab, kPlayerTagColor);  // 0x14 color[3]
    if (c != kNpos && tab.base + c + 3 <= n) {
        out.color[0] = R[tab.base + c];
        out.color[1] = R[tab.base + c + 1];
        out.color[2] = R[tab.base + c + 2];
    }
    const std::size_t mg = find_off(tab, kPlayerTagMagic);
    if (mg != kNpos) parse_magic(R, n, tab, mg, out.magic);
    // TPlayer rw tag 0x1e = +0x58/+0xd4 = the player's TLeader (see Leader).
    const std::size_t ld = find_off(tab, kPlayerTagLeader);
    if (ld != kNpos) parse_leader(R, n, tab, ld, out.leader);
    // The tail of `TPlayer.ReadWrite` (0x5575314c): a record whose index byte
    // (+0xa6) is 0 *is* the independents, and the loader forces its type to 4
    // (`if index == 0 then +0xa7 = 4`). The index tag is itself a rwByte, so an
    // absent index is 0 too.
    if (out.index == 0) out.type = PlayerType::Independent;
}

// Walks map tag 0x15 (a TPlayerList): its tag 1 children table lists the
// `[u32 class id][table]` TPlayer records (class 0x20020).
void parse_roster(const std::uint8_t* R, std::size_t n, std::size_t list_off,
                  GameState& out) {
    Table pl;
    if (!read_eobject_table(R, n, list_off, pl)) return;
    const std::size_t c = find_off(pl, 1);
    if (c == kNpos) return;
    Table children;
    if (!read_eobject_table(R, n, pl.base + c, children)) return;
    for (const EObjectEntry& e : children.entries) {
        const std::size_t a = children.base + e.second;
        if (a + 4 > n) continue;
        if (read_u32(R + a) != kPlayerClassId) continue;
        Table pt;
        if (!read_eobject_table(R, n, a + 4, pt)) continue;
        Player p;
        parse_player(R, n, pt, p);
        out.players.push_back(std::move(p));
    }
}

// Counts the placed armies/units per player from the map (docs/13). An army's
// owner player is `AoWE.TArmy` rw tag 4 (`TArmy.GetPlayer` @ 0x5578d504 reads
// +0x12; a missing tag means owner 0 = the independents).
void tally_troops(const Map& map, GameState& out) {
    for (const MapLevel& lv : map.levels) {
        for (const MapTile& tile : lv.tiles) {
            for (const MapHs& hs : tile.objects) {
                if (hs.class_id != kMapArmyHsClassId) continue;
                const std::size_t who = hs.owner;
                if (who >= out.players.size()) continue;
                out.players[who].armies += 1;
                out.players[who].units += static_cast<int>(hs.units.size());
            }
        }
    }
}

}  // namespace

std::vector<std::uint8_t> generate_turn_order(int player_count,
                                              std::uint32_t rng_seed) {
    // TAoWHSMap.GenerateNewTurnOrder (0x557782fc): the pool is 1..N; the order
    // starts with the independents (0), then a random draw from the pool is
    // appended and removed until it is empty.
    std::vector<std::uint8_t> order;
    order.push_back(0);
    if (player_count < 1) return order;
    std::vector<std::uint8_t> pool;
    for (int i = 1; i <= player_count; ++i) {
        pool.push_back(static_cast<std::uint8_t>(i));
    }
    std::uint32_t state = rng_seed ? rng_seed : 0x1234567u;
    while (!pool.empty()) {
        const std::size_t pick = xs32(state) % pool.size();
        order.push_back(pool[pick]);
        pool.erase(pool.begin() + static_cast<std::ptrdiff_t>(pick));
    }
    return order;
}

void GameState::update_income(std::size_t player_index) {
    if (player_index >= players.size()) return;
    Player& p = players[player_index];
    // TPlayer.UpdateIncome (0x55751f88): income = base + sum(income sources).
    // We do not model cities/resource sites yet, so the sources contribute 0.
    std::int32_t v = p.base_income;
    // The AI-difficulty branches (type 2/3/6/0xa) round the value and add a flat
    // bonus for 6 (+0x32) and 0xa (+0x64). The exact multiplier for 2/3 is
    // TODO(RE); only the two literal bonuses are reproduced.
    switch (p.type) {
        case PlayerType::AI6:  v += 0x32; break;
        case PlayerType::AI10: v += 0x64; break;
        default: break;
    }
    p.income = v;
}

void GameState::begin_day(std::uint32_t rng_seed) {
    if (turn_order.empty()) {
        turn_order = generate_turn_order(player_count(), rng_seed);
        turn_in_order = 0;
    }
}

void GameState::new_day(std::uint32_t rng_seed) {
    // TPlayerControl.NewDay (0x55754da4): the day counter (map +0x174) advances,
    // then the turn position resets; the order list itself is regenerated only
    // when the map's turn-order mode is 2 (`GenerateNewTurnOrder`).
    ++day;
    if (turn_order_mode == 2 || turn_order.empty()) {
        turn_order = generate_turn_order(player_count(), rng_seed);
    }
    turn_in_order = 0;
    // Turn limit reached -> every player is out (TAoWHSMap_GameOver status 2).
    if (turn_limit > 0 && day >= turn_limit) {
        for (Player& p : players) p.game_over_status = 2;
    }
}

void GameState::begin_player_turn(std::size_t player_index) {
    if (player_index >= players.size()) return;
    Player& p = players[player_index];
    update_income(player_index);
    // TPlayerControl.NewTurn (0x557553c4): from day 2 on, credit income minus
    // upkeep; a deficit clamps gold at 0 and lowers the food penalty (-5, floor
    // -50), otherwise the penalty resets. The player's end-turn flag is cleared
    // in the same block (`TPlayer.SetEndTurn(player, false)`), so a seat always
    // starts its turn with the flag down.
    if (day != 1) {
        p.gold = p.gold + p.income - p.upkeep();
        if (p.gold < 0) {
            p.gold = 0;
            p.food_penalty = static_cast<std::int8_t>(p.food_penalty - 5);
            if (p.food_penalty < -50) p.food_penalty = -50;
        } else {
            p.food_penalty = 0;
        }
        set_end_turn(player_index, false);
    }
    // The player's turn counter always advances.
    p.turn_count += 1;
}

std::uint8_t GameState::current() const {
    if (turn_order.empty() || turn_in_order >= turn_order.size()) return 0xff;
    return turn_order[turn_in_order];
}

std::size_t GameState::index_of(std::uint8_t player_index) const {
    for (std::size_t i = 0; i < players.size(); ++i) {
        if (players[i].index == player_index) return i;
    }
    return players.size();
}

bool GameState::player_playing(std::uint8_t player_index) const {
    const std::size_t i = index_of(player_index);
    if (i >= players.size()) return false;
    // +0x45, the latch `TPlayerList.Update` files the active lists from and
    // `TTurnPlayerControl.EndTurn` tests. (A seat put out by the turn limit has
    // this cleared by `TPlayer.Validate`; `active()` is the separate +0x44.)
    return players[i].playing;
}

bool GameState::set_end_turn(std::size_t player_index, bool value) {
    if (player_index >= players.size()) return false;
    if (players[player_index].end_turn == value) return false;
    players[player_index].end_turn = value;
    // TAoWHSMap.TriggerEndTurnEvent (0x55776474): notify the map's event list.
    // The port has no listeners yet (the Events floater reads the seat itself),
    // so the flag is the whole observable effect.
    return true;
}

std::uint8_t GameState::advance_turn(std::uint32_t seed) {
    if (turn_order.empty()) {
        new_day(seed);
        if (turn_order.empty()) {
            current_player = 0xff;
            return 0xff;
        }
    }
    // TTurnPlayerControl.EndTurn (0x55756db8): walk the order from the player
    // who just ended. A seat that is not playing (+0x45 != 1) is started and
    // ended in the same pass (`TPlayerControl.NewTurn` + `EndTurn`), and the
    // first seat that *is* playing becomes the current player. Running off the
    // end of the order wraps into `NewDay` - the round only rolls over once
    // every player has had (and ended) their turn.
    std::size_t idx = turn_in_order;
    for (std::size_t guard = 0; guard <= turn_order.size(); ++guard) {
        ++idx;
        if (idx >= turn_order.size()) {
            new_day(seed);
            idx = 0;
            if (turn_order.empty()) {
                current_player = 0xff;
                return 0xff;
            }
        }
        const std::uint8_t c = turn_order[idx];
        const std::size_t pi = index_of(c);
        if (pi >= players.size() || !player_playing(c)) {
            if (pi < players.size()) {
                begin_player_turn(pi);
                set_end_turn(pi, true);
            }
            continue;
        }
        turn_in_order = idx;
        current_player = c;
        begin_player_turn(pi);
        return c;
    }
    current_player = 0xff;
    return 0xff;
}

void GameState::begin_first_turn(std::uint8_t seated) {
    if (turn_order.empty()) return;
    std::size_t pos = 0;
    for (std::size_t i = 0; i < turn_order.size(); ++i) {
        if (turn_order[i] == seated) {
            pos = i;
            break;
        }
    }
    turn_in_order = pos;
    current_player = turn_order[pos];
    const std::size_t pi = index_of(current_player);
    if (pi < players.size()) begin_player_turn(pi);
}

bool GameState::end_turn(std::uint8_t who) {
    // TAoWHSMap.EndTurn (0x5577838c): no seat -> refuse; while the event log is
    // executing and still holds entries -> refuse.
    if (who == 0xff) return false;
    if (event_log_busy && event_log_count != 0) return false;
    // FUN_5574757c: a busy player, or one that already ended, refuses.
    const std::size_t pi = index_of(who);
    if (pi >= players.size()) return false;
    if (players[pi].busy || players[pi].end_turn) return false;
    // TPlayerControl.EndTurn (0x5575530c): TPlayer.SetEndTurn(player, 1).
    set_end_turn(pi, true);
    // TTurnPlayerControl.EndTurn (0x55756db8): the hand-over happens only when
    // the player who ended *is* the one on turn.
    if (current() == who) advance_turn(rng_seed);
    return true;
}

bool GameState::cancel_end_turn(std::uint8_t who) {
    // TAoWHSMap.CancelEndTurn (0x557783c8) posts the end-turn token with the
    // cancel flag; TEndTurnTE.Execute (0x557473c4) then clears the flag. The
    // cancel is refused for a busy player or one that has not ended.
    if (who == 0xff) return false;
    const std::size_t pi = index_of(who);
    if (pi >= players.size()) return false;
    if (players[pi].busy || !players[pi].end_turn) return false;
    set_end_turn(pi, false);
    return true;
}

std::uint8_t apply_setup_player_types(GameState& gs,
                                      const std::uint8_t* race_type,
                                      std::size_t race_count) {
    // TSetupControl.SetupMap (0x557e0b64): for every settings record (one per map
    // player) it writes the screen's current type back into the roster -
    // `*(char *)(iVar4 + 0xa7) = *(char *)(iVar3 + 8)`. Entry 0 (the
    // independents) is never touched.
    for (Player& p : gs.players) {
        if (p.index == 0) continue;
        if (race_type != nullptr && static_cast<std::size_t>(p.race) < race_count) {
            p.type = static_cast<PlayerType>(race_type[p.race]);
        }
    }
    // TSetupControl.hPickMap (0x557e20c8): its local_5 flag starts set, so the
    // first Human slot keeps type 0 and every later Human gets AI1 (CPU Squire).
    // (ResetPlayerSettings 0x557e1378 reaches the same state: a reset slot whose
    // map default is Human becomes pidCPU1.)
    bool seen_human = false;
    std::uint8_t human = 0xff;
    for (Player& p : gs.players) {
        if (p.index == 0) continue;
        if (p.type != PlayerType::Human) continue;
        if (seen_human) {
            p.type = PlayerType::AI1;
        } else {
            seen_human = true;
            human = p.index;
        }
    }
    return human;
}

bool load_game_state(const Map& map, GameState& out, std::string& error) {
    out = GameState();
    if (map.region.empty()) {
        error = "map has no decoded region (load it with load_map first)";
        return false;
    }
    const std::uint8_t* R = map.region.data();
    const std::size_t n = map.region.size();

    Table outer;
    if (!read_eobject_table(R, n, 0, outer)) {
        error = "cannot read map header table";
        return false;
    }
    const std::size_t t0 = find_off(outer, 0);
    if (t0 == kNpos) {
        error = "map header has no top object";
        return false;
    }
    Table top;
    if (!read_eobject_table(R, n, outer.base + t0, top)) {
        error = "cannot read the map's top object";
        return false;
    }

    const std::size_t roster = find_off(top, kRosterTag);
    if (roster != kNpos) {
        parse_roster(R, n, top.base + roster, out);
    }
    tally_troops(map, out);
    out.loaded = true;
    return true;
}

std::vector<std::uint8_t> map_player_races(const Map& map) {
    std::vector<std::uint8_t> races;
    if (map.region.empty()) return races;
    const std::uint8_t* R = map.region.data();
    const std::size_t n = map.region.size();
    EObjectTable outer;
    if (!read_eobject_table(R, n, 0, outer)) return races;
    const std::size_t t0 = eobject_find(outer, 0);
    if (t0 == kNpos) return races;
    EObjectTable top;
    if (!read_eobject_table(R, n, outer.base + t0, top)) return races;
    const std::size_t roster = eobject_find(top, kRosterTag);
    if (roster == kNpos) return races;
    GameState gs;
    parse_roster(R, n, top.base + roster, gs);
    races.reserve(gs.players.size());
    for (const Player& p : gs.players) races.push_back(p.race);
    return races;
}

}  // namespace aow