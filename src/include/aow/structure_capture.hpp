// structure_capture.hpp - how a world-map building changes hands (Stage 2.7).
//
// This is the 1:1 recovery of AoW1's *building acquisition*: which structures
// can be owned, what "moving an army onto one" does, who ends up owning it after
// a battle, what happens to its income/flag/crops/production, and how razing and
// rebuilding clear or restore the owner.
//
// The recovery is from AoWEPACK.dpl (image base 0x55700000). The vtable slot
// numbers below are the *class reference* offsets - this Borland compiler stores
// the class reference 12 bytes before the address tools/vmt_scan.py reports.
// Verified with tmp_gr/dump_vmt.py (TPlayerStructure class = 0x55714340):
//
//   +0x150 GetRazed          TStructure.GetRazed@5575ede4
//   +0x154 GetRazeable       TStructure.GetRazeable@5575eddc
//   +0x168 NewDay            TPlayerStructure.NewDay@55760a90
//   +0x174 ArmyPlaced        TPlayerStructure.ArmyPlaced@55760f24
//   +0x178 ArmyRemoved       TStructure.ArmyRemoved@5575f364
//   +0x190 BuildingDone      TPlayerStructure.BuildingDone@557608f4
//   +0x194 ExecuteRebuild    TPlayerStructure.ExecuteRebuild@55760928
//   +0x198 ExecuteBuild      TPlayerStructure.ExecuteBuild@55760954
//   +0x19c GenerateRazeDefenders / +0x1a0 PlaceRazeDefenders
//   +0x1a4 ExecuteRaze       TStructure.ExecuteRaze@5575ffc8
//   +0x1ec CanRaze           TStructure.CanRaze@5575fb80
//   +0x1f0 SetPlayer         TPlayerStructure.SetPlayer@55760d08
//   +0x1f4 GetIncome / +0x1f8 GetPower
//   +0x1fc UpdateFlagID      TPlayerStructure.UpdateFlagID@55760ed8
//   +0x204 UpdateUnfog       +0x208 UpdateExploration
//   +0x20c FinalizeCombat    +0x210 GetCombatEventText
//   +0x214 StructureCaptured +0x218 GetMoveAction
//   +0x220 UpdatePlayer      TPlayerStructure.UpdatePlayer@55760980
//
// The four ways a structure changes owner:
//
//  1. **Walking in (auto-capture).** `TArmyHS.PlaceOnMap@557909f4` sends map
//     field message 0x1102 to the field; `TStructure.MapFieldMsgProc@5575f730`
//     forwards it to the structure's `ArmyPlaced` (+0x174). TPlayerStructure.
//     ArmyPlaced -> UpdatePlayer (+0x220), which walks every hex the structure
//     covers, reads the army on each (`TMapField.GetObject(0x20217)`), and:
//        * one owner (or none)  -> SetPlayer(that owner)
//        * two different owners  -> SetPlayer(-1)  (a contested multi-hex city)
//        * no army at all        -> leave the owner untouched
//     That is why a neutral mine with no defender is simply claimed by stepping
//     on it, while a city owned by the independents still answers move-action 1
//     (`TCity.GetMoveAction@557ad0d4`: owner 0 + city resource wants a neutral
//     flag -> attack, unless `JoinAmount` says the city will join).
//  2. **Winning a battle.** `TPlayerStructure.FinalizeCombat@55760cd4` /
//     `City.TCity.FinalizeCombat@557acefc`: when the combat result (+0x14) is
//     3 (`won`) or 6 (`retreated`/defender gave up) the *attacker* player
//     (combat data +0x24) becomes the owner. Result 4 (`lost`), 2 (`cannot
//     inflict damage`) and 8 (`cannot breach wall`) change nothing.
//  3. **Negotiation.** `TCity.IncommingNegotiateOffer@557ad300` hands the city
//     to the offering player and transfers its independent garrison.
//  4. **Rebuilding.** `TPlayerStructure.ExecuteBuild/ExecuteRebuild@55760954/
//     @55760928` call `SetPlayer(builder)` when the construction finishes, after
//     `SetRazed(false)` cleared the razed bit.
//
// `TPlayerStructure.SetPlayer@55760d08` is the single funnel. In order it:
//   * refuses when `GetRazed()` and not `GetBuilding()` (a ruin cannot change
//     hands until it is rebuilt);
//   * clears +0x31 (seen marker), unregisters the old owner's income/power
//     source, logs `StructureCaptured` when the old owner was a real player
//     (old != 0 - capturing an independent structure is silent);
//   * writes the owner byte (+0x30), runs UpdateUnfog / UpdatePlayerSeen /
//     UpdateExploration, recomputes the flag byte (+0x32/TCity+0x54), then
//     registers the new owner's income/power source, checks the players' capture
//     locations and finally calls UpdatePlayer (crop/farm ownership).
//
// `City.TCity.SetPlayer@557aa2c8` wraps that with the city-only state:
// `TProductionControl.PlayerChanged@5575cef0` (keep the active item, drop the
// queued ones; drop everything when idle), `ResetEnchantments`, the player's
// city list move, the migrate action removal and the AI action revalidation.
//
// `TCity.SetRazed@557ab47c` / the subclass overrides all call SetPlayer(-1) and
// then set the class's razed bit; a city also loses its wall and its production.
// `TCity.GetRebuildInfo@557ab50c` = size*25 gold and size+1 turns.
//
// Evidence: port/re/exports/capture/ holds all decompiles (via GhidraMCP) and
// docs/29_BUILDING_ACQUISITION.md narrates them. The fidelity checks live in
// src/tests/game_state_test.cpp.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "aow/map_types.hpp"

namespace aow {

struct MapResource;  // aow/map_resources.hpp

// `TStructure.GetRazed` / `GetBuilding` (the map parser already resolves the
// class-specific razed bytes into `MapHs::razed`).
bool structure_razed(const MapHs& hs);
bool structure_building(const MapHs& hs);

// Flag-byte sentinels (`UpdateFlagID`).
constexpr std::uint8_t kStructureFlagNone = 0x3f;
constexpr std::uint8_t kStructureFlagNeutral = 0x20;

// `TPlayerStructure.UpdateFlagID@55760ed8` + `TCity.UpdateFlagID@557ac8b0`.
// `player_races[p]` is the owning player's race id, exactly the byte
// `TPlayer+0xa5` holds (aow::map_player_races). 0x3f = no flag, 0x20 (+ pole
// bits) = neutral, otherwise the race id (+ `(city_level-1) << 6` for a city).
std::uint8_t structure_flag_id(const MapHs& hs,
                               const std::vector<std::uint8_t>& player_races);

// `TPlayerStructure.GetMoveAction@55761620` / `TCity.GetMoveAction@557ad0d4`:
//   0 = walk on (nothing hostile)
//   1 = attack
//   2 = the independents would join (a neutral city, which the port cannot
//       decide without `TCity.JoinAmount`; it reports 1 = attack instead)
std::uint8_t structure_move_action(const MapHs& hs, int player);

// A placed ownable structure: `level`, its *home* hex and its index in that
// tile's `MapTile::objects` vector.
struct StructureRef {
    int level = -1;
    int x = -1;
    int y = -1;
    std::size_t object = 0;
    bool valid() const { return level >= 0 && x >= 0 && y >= 0; }
};

// Every ownable structure that covers `(x, y)`: the topmost object stored on the
// tile first, then the multi-hex buildings whose `TTerrainList` footprint covers
// it (nearest home hex first). This is `TMapField.GetObject` /
// `TMultiHexMO` registration order, the same walk `menu.cpp`'s
// `structure_on_hex` uses for the Structure floater.
std::vector<StructureRef> structures_at(const Map& map, std::size_t level,
                                        int x, int y,
                                        const std::vector<MapResource>& res);

// Topmost structure covering `(x, y)`; returns an invalid ref when there is none.
StructureRef structure_at(const Map& map, std::size_t level, int x, int y,
                          const std::vector<MapResource>& res);

MapHs* structure_object(Map& map, const StructureRef& ref);
const MapHs* structure_object(const Map& map, const StructureRef& ref);

// `TCity.GetSize@557aa938`: how many `TTerrainList` entries of the resource have
// a terrain (>= 0 as a signed char); that is the city's hex size (1..4).
int structure_size_hexes(const MapResource* res);

// Human name used for the event log line (a city's TCity rw tag 0xc name, or the
// class's English name for the other structures).
std::string structure_display_name(const MapHs& hs, const MapResource* res);

// The combat result codes `TCombat` stores at +0x14 (TCombat.GetCombatResultText
// @5572836c maps them to Battle Won / Lost / Drawn / Cannot Breach Wall /
// Cannot Inflict Damage):
enum class StructureCombatOutcome : std::uint8_t {
    drawn = 0,
    cannot_inflict_damage = 2,
    won = 3,
    lost = 4,
    retreated = 6,  // the defender yields: the attacker still takes it
    cannot_breach_wall = 8,
};

struct StructureChange {
    bool changed = false;
    bool refused_razed = false;   // SetPlayer's GetRazed && !GetBuilding gate
    bool capture_logged = false;  // the StructureCaptured event log fired
    int old_owner = -1;
    int new_owner = -1;
    std::string text;  // the event log line ("... has been captured")
};

// `TPlayerStructure.SetPlayer@55760d08` with all its side effects the port can
// model: the rebuild gate, the owner byte, the flag byte, the explored bit for
// the new owner, the city production reset and the final UpdatePlayer pass.
// Income/power lists are recomputed from the map by aow::compute_economy, and the
// event log is returned in `StructureChange::text` (the port has no global
// event-log window yet).
StructureChange structure_set_player(Map& map, const StructureRef& ref,
                                     int new_owner,
                                     const std::vector<std::uint8_t>& player_races,
                                     const std::vector<MapResource>& res);

// `TPlayerStructure.UpdatePlayer@55760980` (also `ArmyPlaced`/`ArmyRemoved`):
// look at the armies on the structure's own hexes and make the sole army owner
// the structure owner; two different owners make it player -1 (contested).
// Does nothing when no army stands on the structure.
StructureChange structure_update_player(Map& map, const StructureRef& ref,
                                        const std::vector<std::uint8_t>& player_races,
                                        const std::vector<MapResource>& res);

// True when any army not owned by `player` stands on a hex the structure covers.
// The original never places an attacker before its combat resolved
// (`TArmyCombatMoveTE.ExecuteCombat` removes the defender first), so a caller
// that has no combat model must skip the ArmyPlaced pass while this is true.
bool structure_has_foreign_army(const Map& map, const StructureRef& ref,
                                int player,
                                const std::vector<MapResource>& res);

// `TPlayerStructure.FinalizeCombat` / `TCity.FinalizeCombat`: result 3 or 6 hands
// the structure to `attacker`; every other result leaves it alone.
StructureChange structure_finalize_combat(
    Map& map, const StructureRef& ref, StructureCombatOutcome outcome, int attacker,
    const std::vector<std::uint8_t>& player_races,
    const std::vector<MapResource>& res);

// `TStructure.SetRazed` as its subclasses implement it: razing clears the owner
// (SetPlayer(-1), which the army standing there may immediately override - the
// engine does the same), sets the class's razed bit, a city also loses its wall
// and its production. Clearing the bit does not restore an owner.
StructureChange structure_set_razed(Map& map, const StructureRef& ref, bool razed,
                                    const std::vector<std::uint8_t>& player_races,
                                    const std::vector<MapResource>& res);

// `TStructure.GetRebuildInfo@5575f18c` / `TCity.GetRebuildInfo@557ab50c`.
struct RebuildInfo {
    int gold = 0;
    int turns = 0;
};
RebuildInfo structure_get_rebuild_info(const MapHs& hs, const MapResource* res);

// `TPlayerStructure.ExecuteBuild/ExecuteRebuild@55760954/@55760928` on the turn
// the builder finishes: SetRazed(false), clear GetBuilding, then SetPlayer(builder)
// (which re-registers the income and re-runs UpdatePlayer).
StructureChange structure_complete_rebuild(
    Map& map, const StructureRef& ref, int player,
    const std::vector<std::uint8_t>& player_races,
    const std::vector<MapResource>& res);

// `TCity.CheckRebellion@557ab534`: `civil_status` is `TCity.GetCivilStatus`
// (0 = Unruly, 1 = Unrest, >= 2 = no rebellion) and `roll` is the map's
// `Random(100)` draw (0..99; the engine rebels on `roll <= 50` for status 0 and
// `<= 10` for status 1). A successful rebellion hands the city to the
// independents (player 0).
bool structure_city_check_rebellion(Map& map, const StructureRef& ref,
                                    int civil_status, int roll,
                                    const std::vector<std::uint8_t>& player_races,
                                    const std::vector<MapResource>& res);

}  // namespace aow
