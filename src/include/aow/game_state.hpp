// game_state.hpp - the Age of Wonders 1 game state: the player roster and the
// turn/day loop that sits on top of a loaded scenario map.
//
// Reconstructed 1:1 from the Delphi game module AoWEPACK.dpl (image base
// 0x55700000). Every field and every rule below cites the decompiled method it
// was recovered from.
//
// Class map (Delphi -> here):
//
//   Engine.TEControlStyleNode
//     AoWE.TPlayerList        the roster (map tag 0x15); children = TPlayer
//   Engine.TEObject
//     AoWE.TPlayer            class 0x20020   one player / wizard
//     AoWE.TPlayerMagicControl                mana, power, research (TPlayer+0x54)
//     AoWE.TPlayerControl                     the turn/day driver
//
// On-disk: `AoWE.TAoWHSMap.ReadWrite` (0x55776d60) writes the roster at tag
// **0x15** as a `rwEObject`; `TPlayerList` is a `TEControlStyleNode`, so its own
// tag 1 is the children table, and each child is a `[u32 class id][table][data]`
// record with class id 0x20020. The per-record tags come from
// `AoWE.TPlayer.ReadWrite` (0x5575314c); the magic sub-object from
// `AoWE.TPlayerMagicControl.ReadWrite` (0x5577c748). See docs/14_GAME_STATE.md.
//
// Turn loop (recovered from `AoWE.TPlayerControl.NewDay` 0x55754da4,
// `NewTurn` 0x557553c4, `TTurnPlayerControl.EndTurn` 0x55756db8,
// `AoWE.TAoWHSMap.GenerateNewTurnOrder` 0x557782fc):
//
//   * a "day" (turn) counter lives on the map; every round every active player
//     gets a turn, in the order [independents(0)] + a random permutation of
//     1..N-1 (regenerated each day when the map's turn-order mode is 2);
//   * beginning a player's turn (from day 2 on) adds the player's income minus
//     upkeep to their gold (clamped at 0, adding a starvation penalty), clears
//     that player's end-turn flag, and always bumps the player's turn count;
//   * a turn only passes when its owner has ended it (`TPlayer.SetEndTurn`
//     +0x2c): `TTurnPlayerControl.EndTurn` walks the turn order from the player
//     who just ended, skipping seats that are not playing (+0x45 != 1), starts
//     and ends those in the same pass, and only then hands the turn on. When
//     the order wraps, `TAoWHSMap.NewDay` starts a new day - so a day can only
//     advance once every player has ended their turn.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "aow/map_types.hpp"

namespace aow {

// AoWE.TPlayer.ClassID @ 0x557516c8.
constexpr std::uint32_t kPlayerClassId = 0x20020;

// ---------------------------------------------------------------------------
// TPlayer.ReadWrite (AoWEPACK 0x5575314c) tag ids. `off` is the byte offset in
// the original 32-bit object; only the tags confirmed against shipped maps are
// named.
// ---------------------------------------------------------------------------
enum PlayerRwTag : std::uint32_t {
    // +0xa5  u8  race id. Read/written by `TEReadStorageStream.rwByte`
    // (Enginep vtable +0x30), which assigns **0** to the field when the tag is
    // absent - so a map that never stored a race is Humans (0), NOT 0xff.
    // 0xff is only ever seen when it is written explicitly (the independents'
    // record, and the pre-load `TPlayer.Create` 0x55750ea8 sentinel).
    kPlayerTagRace = 0x2,
    // +0xa7  u8  player type (`TSetupControl.hPickMap` 0x557e20c8 reads
    // TPlayer+0xa7 and stores it as the player's type; `TPlayer.Create`
    // 0x55750ea8 leaves it 0). Verified against the 9 campaign maps: the
    // independents carry 4, the human 0 and the AIs 1/2/3/6/7.
    kPlayerTagType = 0x3,
    kPlayerTagGold = 0x5,         // +0xac  i32 current gold (default 250 = 0xfa)
    kPlayerTagBaseIncome = 0x6,   // +0xb0  i32 base income
    kPlayerTagUnknownA = 0xa,     // +0xa9?  u8
    kPlayerTagUnknownB = 0xb,     // +0xbc  object
    kPlayerTagGems = 0x10,        // +0xc4  i32 crystals (SetGems)
    kPlayerTagColor = 0x14,       // +0x2d..+0x2f  u8[3] player colour
    kPlayerTagLogbook = 0x17,     // +0xd8  object  TEventLogbook
    kPlayerTagName = 0x18,        // +0x30  string
    kPlayerTagGameOver = 0x19,    // +0x44  u8  game-over status (2 = out)
    kPlayerTagFoodPenalty = 0x1b, // +0x50  i8  starvation counter
    kPlayerTagLeader = 0x1e,      // +0x58/+0xd4  object  TLeader
    kPlayerTagMagic = 0x1f,       // +0x54  object  TPlayerMagicControl
    kPlayerTagIndex = 0x12,       // +0xa6  u8  player index/id
    kPlayerTagTurnCount = 0x22,   // +0x5c  i32 turns this player has taken
};

// THero.ReadWrite (AoWEPACK 0x55788880) tag ids shared by a placed hero and the
// player's TLeader (tag 0x1e) - the same tags src/engine/map.cpp reads for the
// heroes standing on the map.
enum HeroRwTag : std::uint32_t {
    kHeroTagFaceIndex = 0x7,          // THero+0x3c  the face index
    kHeroTagHeroResourceIndex = 0x8,  // THero+0x44  the THeroResource index
    kHeroTagName = 0xa,               // THero+0x60  the hero's name
    kHeroTagNickname = 0xb,           // THero+0x64  the nickname
};

// TPlayerMagicControl.ReadWrite (AoWEPACK 0x5577c748) tag ids.
enum MagicRwTag : std::uint32_t {
    kMagicTagMana = 0x14,             // +0x24 i32
    kMagicTagPowerToResearch = 0x15,  // +0x2c i32 % of power to research (def 50)
    kMagicTagResearchSpells = 0x16,   // +0x30 TIntegerList (spell ids)
    kMagicTagCurrentResearch = 0x17,  // +0x34 i32
    kMagicTagResearchProgress = 0x18, // +0x38 i32
    kMagicTagEnchantments = 0x19,     // +0x3c object  TEnchantmentList
    kMagicTagByte = 0x1a,             // +0x44 u8
};

// TPlayer+0xa7 player behaviour/control type (map rw tag 0x3). 0 = local human;
// the AI types are the difficulty levels - `TPlayer.UpdateIncome` (0x55751f88)
// gives types 2, 3, 6 and 0xa an income bonus and `TPlayerControl.Setup`
// (0x55754b94) treats 0 as a connected human. `TSetupControl.ListAvailablePlayers`
// (0x557e1660) names the seven a host game offers, in the order
// 0,1,2,3,6,7,4 = Human, CPU Squire, CPU Knight, CPU Lord, CPU King, CPU
// Emperor, Independent (see docs/08_SCENARIO_SCREEN.md). The numeric values are
// verified; the AI enum names are the difficulty levels' placeholders.
enum class PlayerType : std::uint8_t {
    Human       = 0,   // "Human"        (the local, connected player)
    AI1         = 1,   // "CPU Squire"
    AI2         = 2,   // "CPU Knight"
    AI3         = 3,   // "CPU Lord"
    Independent = 4,   // "Independent"  (the monsters; also entry 0's forced type)
    AI5         = 5,   // "None"         (an unassigned slot; not offered in SP)
    AI6         = 6,   // "CPU King"
    AI7         = 7,   // "CPU Emperor"
    AI8         = 8,
    AI9         = 9,
    AI10        = 10,
};

// ---------------------------------------------------------------------------
// AoWE.TPlayerMagicControl - a player's magic (TPlayer+0x54).
// ---------------------------------------------------------------------------
struct PlayerMagic {
    std::int32_t mana = 0;                 // +0x24 tag 0x14
    std::int32_t power_to_research = 50;   // +0x2c tag 0x15 (0..100, default 50)
    std::int32_t current_research = 0;     // +0x34 tag 0x17
    std::int32_t research_progress = 0;    // +0x38 tag 0x18
    std::vector<std::int32_t> research_spells;  // +0x30 tag 0x16
    bool present = false;                  // the map carried a tag-0x1f object

    // `TPlayerMagicControl.GetNetPowerToResearch` 0x5577cf80:
    //   round(net_power * power_to_research / 100)
    int net_power_to_research(int net_power) const {
        return static_cast<int>(
            (static_cast<long long>(net_power) * power_to_research + 50) / 100);
    }
};

// ---------------------------------------------------------------------------
// AoWE.TLeader (class 0x20232, a THero subclass) - the player's wizard-leader.
//
// On-disk: `AoWE.TPlayer.ReadWrite` (0x5575314c) stores the leader at rw tag
// **0x1e** - `rwObject(TPlayer+0x58, 0x1e)` in the older maps and
// `rwObject(TPlayer+0xd4, 0x1e)` when the map's `+0xf` carries bit 1 - and the
// record itself is a THero, serialised inline (a bare property table, no class
// id): `TLeader.ReadWrite` (0x5578b470) = `THero.ReadWrite` (0x55788880) +
// tags 0x2d/0x2e. The fields the banner needs are the THero tags the placed
// heroes use too (`src/include/aow/map_types.hpp`):
//
//   tag 7  rwInteger  THero+0x3c  the *face* index   (SetFaceIndex 0x5578883c)
//   tag 8  rwInteger  THero+0x44  the THeroResource index (SetHeroResourceIndex
//                                 0x55788840 -> THeroResourceList.GetHeroResource)
//   tag 10 rwString   THero+0x60  the hero's name    (GetName 0x55786fa4)
//   tag 11 rwString   THero+0x64  the nickname       (GetNickname 0x55786ab0)
//
// `TPlayer.GetFace` (0x5575235c) is what the Events banner's `FaceFr` draws, and
// `TPlayer.GetName` (0x55752104) what its `Player` label shows.
// ---------------------------------------------------------------------------
struct Leader {
    bool present = false;          // the map stored a tag-0x1e object
    std::int32_t face_index = 0;   // tag 7  - an FACERES.PFS record tag
    std::int32_t hero_resource_index = -1;  // tag 8 (THero.Create leaves -1)
    std::string name;              // tag 10
    std::string nickname;          // tag 11
};

// ---------------------------------------------------------------------------
// AoWE.TPlayer - one player/wizard (class 0x20020).
// ---------------------------------------------------------------------------
struct Player {
    std::uint8_t index = 0;  // +0xa6 tag 0x12 (0 = independents/monsters)
    // +0xa5 tag 0x2. `rwByte` defaults an absent tag to 0, so a race that the
    // map never stored is Humans (0); 0xff only appears when it was written
    // (the independents' record). See PlayerRwTag::kPlayerTagRace.
    std::uint8_t race = 0;
    PlayerType type = PlayerType::Human;  // +0xa7 tag 0x3 (0 = human, 4 = independents)
    std::uint8_t color[3] = {0, 0, 0};  // +0x2d.. tags 0x14..0x16 (rwByte default 0)

    std::int32_t gold = 250;         // +0xac tag 0x5  (Create default 0xfa)
    std::int32_t base_income = 0;    // +0xb0 tag 0x6
    std::int32_t income = 0;         // +0xd0 (UpdateIncome result)
    std::int32_t gems = 0;           // +0xc4 tag 0x10 (crystals; SetGems)
    std::int32_t army_upkeep = 0;    // +0xc8 (SetArmyUpkeep)
    std::int32_t magic_upkeep = 0;   // +0xcc (SetMagicUpkeep)
    std::int8_t  food_penalty = 0;   // +0x50 tag 0x1b (starvation counter)
    std::int32_t turn_count = 0;     // +0x5c tag 0x22
    std::uint8_t game_over_status = 0;  // +0x44 tag 0x19 (2 = eliminated)
    std::string name;                // +0x30 tag 0x18 (often empty in scenarios)

    // +0x2c - has this player ended the current turn? `AoWE.TPlayer.SetEndTurn`
    // (0x55752d00) stores it and fires the map's end-turn notify event
    // (`TAoWHSMap.TriggerEndTurnEvent` 0x55776474). The turn may not pass while
    // it is clear - `TTurnPlayerControl.EndTurn` (0x55756db8) only hands the
    // turn on after this player's own `TPlayerControl.EndTurn` set it, and
    // `TPlayerControl.NewTurn` (day != 1) clears it again when the turn returns.
    bool end_turn = false;

    // `AoWE.TPlayer.GetBusy` (0x557516d0) - a player in the middle of a
    // transition (a combat, a modal window) refuses to end the turn. Runtime
    // only; the port keeps it false unless a transition is in flight.
    bool busy = false;

    // +0x45 - the seat takes part in the turn order. `TPlayerList.Update`
    // (0x55754370) files every player whose +0x45 is 1 into its active lists,
    // and `TTurnPlayerControl.EndTurn` skips the others (it starts and ends
    // their turn in the same pass). `TPlayer.Validate` (0x55751c00) sets it.
    bool playing = true;

    // +0xd4 tag 0x1e - the player's TLeader (see Leader above).
    Leader leader;

    PlayerMagic magic;

    // Troops: counted from the scenario's placed armies (see GameState).
    int armies = 0;
    int units = 0;

    // `TPlayer.GetName` @ 0x55752104: independents -> "Independents"; a player
    // with a leader -> the leader's name (THero+0x60), with the nickname
    // (THero+0x64) appended when it is set (the routine concatenates the three
    // strings through `System.LStrCatN`, separated by the " " literal at
    // AoWEPACK 0x55702214); otherwise the player's own stored name (+0x30), and
    // "Player N" when that is empty too.
    std::string display_name() const {
        if (index == 0) return "Independents";
        if (leader.present && !leader.name.empty()) {
            if (leader.nickname.empty()) return leader.name;
            return leader.name + " " + leader.nickname;
        }
        if (!name.empty()) return name;
        return "Player " + std::to_string(static_cast<int>(index));
    }

    // `TPlayer.GetUpkeep` @ 0x557520c4: army + magic upkeep.
    int upkeep() const { return army_upkeep + magic_upkeep; }

    // Still in the game? (`TPlayerList.GetActivePlayers` 0x5575434c counts
    // players whose +0x44 != 2.)
    bool active() const { return game_over_status != 2; }
};

// ---------------------------------------------------------------------------
// The game state: the map's player roster plus the day/turn loop.
// ---------------------------------------------------------------------------
struct GameState {
    std::vector<Player> players;   // index 0 = independents, 1..N-1 = players
    int day = 1;                   // map +0x174 (global day/turn counter)
    int turn_limit = 0;            // map +0x158 (0 = no limit)
    int turn_order_mode = 0;       // map +0x16c (2 = regenerate order each day)

    std::vector<std::uint8_t> turn_order;  // map +0x170 (TByteList)
    std::size_t turn_in_order = 0;         // position within turn_order
    // map +0xa4 - the player whose turn it is (`TAbstractAoWHSMap
    // .SetCurrentPlayer` 0x55772a80; 0xff = none). The *seated* player
    // (`map+0xa5`, the local seat in a network/hotseat game) is the same
    // player in the port's single-player games.
    std::uint8_t current_player = 0xff;

    // map +0x139 / *(map + 0x1d4). `TAoWHSMap.EndTurn` (0x5577838c) refuses to
    // post the end-turn event while the event log is being executed and still
    // holds entries (the turn is mid-animation).
    bool event_log_busy = false;
    int event_log_count = 0;
    // The RNG seed `GenerateNewTurnOrder` draws with (the map's own `Random`
    // in the original, whose seed the port does not carry).
    std::uint32_t rng_seed = 0x1234567u;

    bool loaded = false;

    // The number of real players (excludes the independents at index 0);
    // `TPlayerList.GetNumberOfPlayers` @ 0x55754674.
    int player_count() const {
        return players.empty() ? 0 : static_cast<int>(players.size()) - 1;
    }

    // Rebuild `income` from `base_income` (mirrors TPlayer.UpdateIncome
    // 0x55751f88; the AI-difficulty multipliers are TODO(RE) - the two literal
    // bonuses +50/+100 for types 6/0xa are applied).
    void update_income(std::size_t player_index);

    // `TPlayerControl.NewDay` (0x55754da4) + `TAoWHSMap.GenerateNewTurnOrder`
    // (0x557782fc): advance the day and (re)create the turn order.
    void new_day(std::uint32_t rng_seed);

    // Build the turn order for the *current* day (no day increment) - the
    // scenario's first day, before any NewDay.
    void begin_day(std::uint32_t rng_seed);

    // `TPlayerControl.NewTurn` (0x557553c4): run one player's turn start - credit
    // income minus upkeep, clamp gold, bump the player's turn count. The day must
    // already be advanced (the original skips the gold step on day 1).
    void begin_player_turn(std::size_t player_index);

    // The player whose turn is current (turn_order[turn_in_order], or 0xff).
    std::uint8_t current() const;

    // Advance to the next player; when the order wraps, run `new_day`. Returns
    // the player now on turn.
    std::uint8_t advance_turn(std::uint32_t rng_seed);

    // Starts the turn loop on `seated` (the local player): puts the turn-order
    // cursor on that seat and runs its turn start. A map's own first turn would
    // otherwise belong to the independents (order entry 0).
    void begin_first_turn(std::uint8_t seated);

    // `TPlayerList.GetPlayers(..)+0x45`: does this seat take part in the order?
    bool player_playing(std::uint8_t player_index) const;

    // `AoWE.TPlayer.SetEndTurn` (0x55752d00): store the flag and fire the map's
    // end-turn notify event when it actually changed. Returns true on a change.
    bool set_end_turn(std::size_t player_index, bool value);

    // `AoWE.TAoWHSMap.EndTurn` (0x5577838c) -> `FUN_5574757c` ->
    // `TPlayerControl.EndTurn` (0x5575530c) -> `TTurnPlayerControl.EndTurn`
    // (0x55756db8): end `who`'s turn. The turn only advances when `who` is the
    // player on turn, so a turn can pass only once its owner has ended it.
    // Returns false when the map refuses (no such seat, the seat is busy, the
    // seat already ended, or the event log is still running).
    bool end_turn(std::uint8_t who);

    // `AoWE.TAoWHSMap.CancelEndTurn` (0x557783c8) + `TEndTurnTE.Execute`
    // (0x557473c4, cancel branch): withdraw `who`'s end-turn flag.
    bool cancel_end_turn(std::uint8_t who);

    // Position of a seat in the roster (players.size() when there is none).
    std::size_t index_of(std::uint8_t player_index) const;
};

// Parses the player roster (TAoWHSMap tag 0x15) out of `map.region` and fills
// `out`. `map` must have been loaded by load_map(). Also tallies each player's
// placed armies/units from `map`. Returns false and sets `error` on a malformed
// roster (a map with no roster still succeeds with zero players).
bool load_game_state(const Map& map, GameState& out, std::string& error);

// The race id of every roster entry, in roster order (entry 0 is the
// independents) - `AoWE.TPlayer` rw **tag 2** -> +0xa5, the byte
// `AoWE.TPlayerStructure.UpdateFlagID@55760ed8` copies into a structure's flag
// so its owner flag is the *owning player's race* flag. This is the light half of
// load_game_state (no map sweep), for the renderer, which asks once per map frame
// to re-composite the animated structure flags. Empty when the map carries no
// roster. See docs/19_STRUCTURE_FLAGS.md.
std::vector<std::uint8_t> map_player_races(const Map& map);

// `TAoWHSMap.GenerateNewTurnOrder` (0x557782fc): turn order = [0] followed by a
// random permutation of 1..player_count(), shuffled with a small deterministic
// PRNG seeded from `rng_seed` (the original uses the map's own RNG).
std::vector<std::uint8_t> generate_turn_order(int player_count,
                                              std::uint32_t rng_seed);

// `TSetupControl.hPickMap` (0x557e20c8) + `TSetupControl.SetupMap` (0x557e0b64):
// apply the scenario screen's per-race player types to a loaded roster.
//
// `race_type` holds one type byte per race id (index 0..race_count-1, e.g. the
// screen's `R*PlayerCB` choices); a player whose race is in range takes that
// type and every other player keeps the map's own. This is exactly what
// `SetupMap` does last - it only ever rewrites the *type*
// (`player[+0xa7] = settings[+8]`), never the race: a scenario's races are fixed
// and choosing which one is Human is how you choose whose cities and armies (its
// units) you play.
//
// Only one player may be Human. `hPickMap`'s local_5 pass keeps the *first*
// Human slot and demotes every later one to AI1 (CPU Squire);
// `ResetPlayerSettings` (0x557e1378) does the same for a reset default-human
// slot. This reproduces that pass so the result is identical whether the caller
// pre-deduped the array or handed in several Humans.
//
// Returns the roster index of the human player (0xff when the roster holds none),
// i.e. the index the caller should treat as the local player.
std::uint8_t apply_setup_player_types(GameState& gs,
                                      const std::uint8_t* race_type,
                                      std::size_t race_count);

}  // namespace aow