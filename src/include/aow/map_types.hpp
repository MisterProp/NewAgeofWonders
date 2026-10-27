// map_types.hpp - typed views over the AoW1 scenario/campaign map format
// (`.HSM` / `.CSM` / `.HSS`). Stage 2.1.
//
// Container (see docs/01_FORMATS.md, "HSM / CSM / HSS"):
//   `.HSM`/`.CSM` : 'CFS\0' + u8 version + zlib stream  -> the region below
//   `.HSS`        : uncompressed (Release.hss, the game pack)
//
// The region is the generic engine EObject graph (same machinery as `.PFS`):
//   u8 count, entries...  -> an outer table with one entry (tag 0 -> offset 0);
//   at base+0 begins the top object's own `[table][data]`.
//
// The top object is `AoWE.TAoWHSMap` (class id 0x20011). Its tag map comes from
// the decompiled `AoWE.TAoWHSMap.ReadWrite` (AoWEPACK 0x55776d60) and
// `HSEngine.THSMap.ReadWrite` (HSEPack 0x5560c4c4):
//
//   tag 2  rwString   HSS file name (e.g. "RELEASE\release.hss")
//   tag 3  rwByte     (THSMap+0x34)  [TODO(RE)]
//   tag 4  rwInteger  map pixel width?  (THSMap+0x80)
//   tag 5  rwInteger  map pixel height? (THSMap+0x84)
//   tag 6  rwInteger  (THSMap+0x88)
//   tag 1  children   a single child: the TMapContainer (class 0x10103)
//
// TMapContainer (class 0x10103, `ReadWrite` HSEPack 0x55608c44):
//   tag 3  rwByte  width  (in hexes)
//   tag 4  rwByte  height (in hexes)
//   tag 5  rwByte  number of levels
//   tag 1  children -> the levels (`AoWE.TAoWMapLevel`, class 0x20013)
//
// TMapLevel (class 0x10102/0x20013, `ReadWrite` HSEPack 0x556084d0):
//   tag 3  rwByte  width
//   tag 4  rwByte  height
//   tag 5/6 rwByte level index / marker
//   tag 6  field data: a sequence of `u32 length` + one field EObject per hex,
//          row-major, exactly width*height records.
//
// TMapField (class 0x10101, `ReadWrite` HSEPack 0x55607128) stores its hexagon
// sprites as children with table tags >= 50 (rwCreateEObject, each preceded by a
// u32 Engine class id). Tag 50 is the terrain hexagon; tags 51+ are the map
// objects (units/cities/items/...). TAoWMapField adds tag 4 (rwWord).
//
// Reference decompiles live in re/exports/aowepack/ and re/exports/hsepack/.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace aow {

// Engine TEObject class ids of the map classes (from the ClassID getters).
constexpr std::uint32_t kAoWHSMapClassId = 0x20011;    // AoWE.TAoWHSMap
constexpr std::uint32_t kMapContainerClassId = 0x10103;  // HSEngine.TMapContainer
constexpr std::uint32_t kMapLevelClassId = 0x10102;    // HSEngine.TMapLevel
constexpr std::uint32_t kAoWMapLevelClassId = 0x20013;  // AoWE.TAoWMapLevel
// AoWTC.TAoWCombatMapLevel / TAoWCombatMapField (AoWTCPCK.dpl): the tactical
// battle map variants of the scenario level/field. ClassID getters at
// AoWTCPCK.dpl 0x41b350 / 0x41b2f4 return these. They inherit TAoWMapLevel /
// TAoWMapField, so the field/level serialisation is identical; only the
// registered class ids differ. The shipped TCMaps\*.HSM files use them.
constexpr std::uint32_t kAoWCombatMapLevelClassId = 0x220102;
constexpr std::uint32_t kAoWCombatMapFieldClassId = 0x220101;
constexpr std::uint32_t kMapFieldClassId = 0x10101;    // HSEngine.TMapField
constexpr std::uint32_t kIsoUpperHexagonClassId = 0x20170;  // IsoHex.TUpperIsometricHexagon
constexpr std::uint32_t kPadClassId = 0x20106;          // Pad.TPad (city hill)
constexpr std::uint32_t kCityClassId = 0x20108;         // City.TCity
constexpr std::uint32_t kCityResourceClassId = 0x20109;  // City.TCityResource
// Terrain.TTerrainMO (trees, rocks, the hill mounds, ...) - a *decor* terrain
// object: it answers `GetOverlay` with its resource's per-hex entry.
constexpr std::uint32_t kTerrainMoClassId = 0x20166;      // Terrain.TTerrainMO
// Mountain.TMountainMO - the only terrain object that *overrides* `GetOverlay`
// to answer 0 (mountains) for every hex it covers, whatever the per-hex entry
// says (`Mountain.TMountainMO.GetOverlay@557a2dc8`; see
// `aow::fill_tile_terrain` in aow/movement.hpp). Its resources are the
// `Mountain.TMountainMOR` records (class 0x20169) of Release.hss.
constexpr std::uint32_t kMountainMoClassId = 0x20168;      // Mountain.TMountainMO
// Crops.TCrop (0x2036a) - the field tile a TPlayerCropStructure plants around
// itself. It is a TILTerrainMO whose resource is Crops.TCropResource (0x2036b,
// the single Release.hss record id 41). `GetTerrainTypeImage` selects the sprite
// by the *hex's terrain number* (plus 0x1e for an Undead owner), so the frame is
// not a per-resource variant - see aow::crop_image_index.
constexpr std::uint32_t kCropClassId = 0x2036a;            // Crops.TCrop
constexpr std::uint32_t kCropResourceClassId = 0x2036b;    // Crops.TCropResource
// Gems.TGems (0x203ae) - the gold pile. It is an `ILTer.TILTerrainMO` (not a
// TPlayerStructure): an army walking onto its hex collects it.
//   `Gems.TGems.Create@557c4f9c`       +0x20 (size class) = 2, +0x1c (amount) = 0
//   `Gems.TGems.ReadWrite@557c4fdc`    rw tag 5 -> +0x1c amount (rwInteger)
//                                      rw tag 6 -> +0x20 size class (1-byte blob)
//   `Gems.TGems.GenerateGemsAmount@557c5010` recomputes the amount from the
//                                      size class at the start of a scenario
//   Size classes 1, 2, 3 are the small/medium/large piles; 4 means "random"
//   (drawn once and replaced by 1..3). Shipped scenarios store only tag 6.
constexpr std::uint32_t kGemsClassId = 0x203ae;            // Gems.TGems
// Farm.TFarm (0x2036e) - the only shipped TPlayerCropStructure subclass. It is
// the structure that owns/links the crop tiles around it (TCrop.LinkToCropOwner /
// TPlayerCropStructure.LinkToCrops); its rw tag 5 is the farm's owner.
constexpr std::uint32_t kFarmClassId = 0x2036e;            // Farm.TFarm
// Mine.TMine (0x2037a) - a TPlayerStructure that pays a flat 10 gold a turn
// (`Mine.TMine.GetIncome@557b43e4`). Its rw tag 0x14 (+0x3c) stores a status
// byte whose bit 0 is `GetRazed@557b4640`.
constexpr std::uint32_t kMineClassId = 0x2037a;            // Mine.TMine
// PowerNode.TPowerNode (0x2019a) - flat 10 power (mana income) and rw tag 0x14
// -> +0x3c bit 0 razed, exactly like a mine.
constexpr std::uint32_t kPowerNodeClassId = 0x2019a;
// PrdPlace.TProductionPlace (0x203ba) and its gold-paying subclasses. A
// production place whose `GetMagicSphere` is not 8 is a magic node and uses a
// TPlayerStructurePowerSource instead of a gold income source
// (`PrdPlace.TProductionPlace.Create@557be10c`).
constexpr std::uint32_t kProductionPlaceClassId = 0x203ba;  // PrdPlace.TProductionPlace
constexpr std::uint32_t kShipyardClassId = 0x204be;         // Shipyard.TShipyard (base income 0)
constexpr std::uint32_t kBuildersGuildClassId = 0x2092e;    // BldGuild.TBuildersGuild (10 gold)

// The map-object classes that carry the placed units. A `TMapField`'s
// `AoWE.TArmyHS` (0x20217) holds one army: its rw tag 10 is the `AoWE.TArmy`
// (an embedded EObject, no class id), whose children (tag 1) are the unit
// objects. See docs/13_UNITS_ON_MAP.md.
constexpr std::uint32_t kMapArmyHsClassId = 0x20217;  // AoWE.TArmyHS
constexpr std::uint32_t kMapUnitClassId = 0x20213;    // AoWE.TUnit
constexpr std::uint32_t kMapHeroClassId = 0x20230;    // AoWE.THero
constexpr std::uint32_t kMapLeaderClassId = 0x20232;  // AoWE.TLeader

// One unit inside an army (`AoWE.TUnit` / `THero` / `TLeader`). A unit's sprite
// is looked up through a *different* tag depending on the class:
//   TUnit   (0x20213)  rw tag 10 (rwWord)  - the TUnitResource's Unitres.pfs
//                                           record-list tag (== UnitEntry::id)
//   THero   (0x20230)  rw tag 8  (rwInteger) - the THeroResource index
//   TLeader (0x20232)  idem (a THero subclass)
// Evidence: re/exports/aowepack/AoWE.TUnit.ReadWrite.c (tag 10 =
// TUnitResource.UnitResourceIndex), AoWE.THero.ReadWrite.c (tag 8 =
// SetHeroResourceIndex), AoWE.TArmyHS.{ReadWrite,UpdateUnitGFXResource}.c and
// AoWE.TUnitHS.Show.c (the army draws its *lead* unit's sprite).
struct MapUnit {
    std::uint32_t class_id = 0;  // Engine class id of the unit object
    std::int32_t id = -1;        // TAbstractUnit rw tag 5 (rwIntegerEx, def -1)
    std::uint8_t morale = 0;     // TAbstractUnit rw tag 6 (rwByte, +0x26)
    // The unit's sprite source (see the tag table above): the TUnitResource
    // record-list tag for a TUnit, or the THeroResource index for a hero. The
    // stream omits a value equal to the object's Create default, so -1 means
    // "unset" (a hero's THero.Create leaves +0x44 = -1).
    std::int32_t resource = 0;
    // True when `resource` is a THeroResource index (THero/TLeader) rather than
    // a TUnitResource tag (TUnit).
    bool is_hero = false;

    // The unit's ability list (`TAbilityOwner` rw tags 2/3, serialised for every
    // placed unit by TAbstractUnit.ReadWrite@0x557820e4 -> TAbilityOwner.
    // ReadWrite@0x5574f318 -> TCustomAbilityList.ReadWrite@0x5574e2bc): tag 2 is
    // a u16 bit count and tag 3 `ceil(count/8)` bitmap bytes, LSB-first
    // (GetAbSet@0x5574e0e0). This is what `GetAbMoveTypesAll` folds into the
    // move-type byte (docs/15_MOVEMENT.md).
    std::uint32_t ability_count = 0;
    std::vector<std::uint8_t> ability_mask;

    // --- Hero/leader-only fields (empty/0 for a plain TUnit) ---------------
    // Evidence: AoWE.THero.ReadWrite@0x55788880 and the getters it feeds -
    // GetName@0x55786fa4 (+0x60), GetNickname@0x55786ab0 (+0x64),
    // GetFace@0x557887dc (+0x3c), GetGender@0x55786c34 (+0x69),
    // GetExperience@0x55787670 (+0x48), GetUnitLevel@0x55786a7c (+0x4c),
    // GetLibraryName@0x55786980 (+0x94).
    std::string name;              // tag 10 rwString
    std::string nickname;          // tag 11 rwString
    std::int32_t face_index = 0;   // tag 7  rwInteger
    std::uint8_t gender = 0;       // tag 15 rwData(1)
    std::uint32_t experience = 0;  // tag 24 rwInteger
    std::uint8_t unit_level = 0;   // tag 22 rwByte
    std::string library_name;      // tag 38 rwString

    // --- Runtime state (not serialised by the map file) --------------------
    // Move points left this turn (`TUnit.GetMovePoints@0x5578276c` reads +0x3d,
    // which `TAbstractUnit.NewTurn@0x55780d4c` refills from `GetMoves` when the
    // owner's turn begins). A move charges every travelling unit the path's
    // cost (`TAbstractUnit.MovePointCost`). -1 = the unit has not spent any yet,
    // so its allowance (`aow::unit_move_points`) is still full - the same
    // convention `MapHs::army_mp_left` uses at army level.
    std::int16_t move_points_left = -1;
};

// One entry of a city's `AoWE.TUnitProductionList` (what the production cards
// at `TCityScreen.P1CB..P12CB` offer).
//
// Evidence: `AoWE.TUnitProductionList.Add@0x5575c60c` stores its two arguments
// at `TUnitProduction+8` (the Unitres record id) and `+0xc` (a boolean flag),
// and `AoWE.TUnitProduction.ReadWrite@0x5575c5b0` streams them as rw tag 0x14
// (`rwInteger`) and rw tag 0x15 (`rwBool` - a *presence* flag, no payload).
// The list itself is serialised by `TENode`/`TECustomNode.ReadWriteChildren`
// as tag 1: a child table whose entries are `rwCreateEObject` records, each a
// u32 `AoWE.TUnitProduction.ClassID` = 0x20299 followed by its own rw table.
struct CityProductionUnit {
    std::int32_t unit_id = 0;  // Unitres record tag (`TUnitProduction+8`)
    bool flag = false;         // `TUnitProduction+0xc` (rw tag 0x15 presence)
};

// One item of a city's `AoWE.TProductionQueue` (the `QueueLB` rows).
//
// `AoWE.TProductionItem.ReadWrite@0x5575c6f0` streams `+8/+0xc/+0x10` as rw
// tags 0x14/0x15/0x16. `TProductionItem.GetName`/`TProductionControl.
// GetProductionName@0x5575de04` show what they mean: `+8` is the kind
// (1 = produce, 2 = install/research), `+0xc` is the index into the city's
// `TUnitProductionList`, and `+0x10` is the extra value the engine carries
// for a queued item. The queue is a `TNode` child list too (tag 1), whose
// records are `AoWE.TProductionItem.ClassID` = 0x20297 + a rw table.
struct CityQueueItem {
    std::int32_t kind = 0;   // `TProductionItem+8`  (rw tag 0x14)
    std::int32_t index = 0;  // `TProductionItem+0xc` (rw tag 0x15)
    std::int32_t extra = 0;  // `TProductionItem+0x10` (rw tag 0x16)
};

// One hexagon sprite (an `HSEngine.THexagonSprite` subclass) placed on a tile.
struct MapHs {
    std::uint32_t tag = 0;       // field-table tag: 50 = terrain, 51+ = objects
    std::uint32_t class_id = 0;  // Engine class id of the sprite
    std::uint32_t resource = 0;  // the sprite's tag 1 (resource/variant id)
    std::size_t offset = 0;      // region offset of the sprite (evidence)
    // City.TCity (class 0x20108) state from its flattened rw table (see
    // re/exports/aowepack/City.TCity.ReadWrite.c): tag 0xd = race/faction
    // (`TCity.GetRace` reads +0x45; -1 = the race was left unset, which the
    // editor writes for its "Random" entry and which makes `TCity.UpdateImages`
    // draw the city resource's `C_random` "?" block - an *owned* city draws its
    // owner's race instead, `aow::city_body_race`) and tag 0xf = wall type
    // (`TCity.SetWallType` writes +0x4c). Other classes leave these -1.
    std::int8_t race = -1;
    std::int8_t wall_type = -1;
    // `Gems.TGems` (class 0x203ae) only: the pile's gold value (+0x1c, rw tag
    // 5) and its size class (+0x20, rw tag 6). `TGems.Create` leaves 0 gold and
    // size 2; the amount is (re)generated from the size when a scenario starts
    // (`aow::generate_gems_amounts`), so a shipped map only stores the size.
    std::int32_t gems_gold = 0;
    std::uint8_t gems_size = 2;
    // `AoWE.TArmyHS` (class 0x20217) only: the army's units, in child order
    // (the lead unit - the one the engine draws - is `units.front()`), plus the
    // TUnitHS facing byte (rw tag 9). Empty for every other object class.
    std::uint8_t army_direction = 3;  // TArmyHS/TUnitHS rw tag 9 (rwByte)
    // `AoWE.TArmyHS` only: the owning player's index (`AoWE.TArmy` rw tag 4 =
    // +0x12; `TArmy.GetPlayer` @ AoWEPACK 0x5578d504). A missing tag means owner
    // 0 (the independents/monsters). See docs/14_GAME_STATE.md.
    std::uint8_t owner = 0;
    // `AoWE.TPlayerStructure` (class 0x2034c) and its subclasses only: the
    // owning player's index. `AoWE.TPlayerStructure.ReadWrite@55760f44` rw
    // **tag 5** writes +0x30 as a `rwByte`, and `TEReadStorageStream.rwByte`
    // stores 0 when the tag is absent - so a structure nobody owns is player 0
    // (the independents) and `AoWE.TPlayerStructure.UpdateFlagID@55760ed8`
    // treats `owner < 1` as "draw the neutral flag or none at all". See
    // docs/19_STRUCTURE_FLAGS.md.
    std::uint8_t structure_owner = 0;
    // `City.TCity` (class 0x20108) only: `City.TCity.ReadWrite@557acbcc` rw
    // **tag 0x16** writes +0x44, a status byte. `TCity.GetRazed@557ab474` reads
    // its bit 1, and `TCity.UpdateFlagID@557ac8b0` refuses to draw the owner flag
    // while it is set (unless the city is being built, which a scenario file
    // never stores - `TStructure.Create` leaves the build counter at -1 and no
    // shipped map writes rw tag 0xff). Bit 0 also suppresses the flag.
    std::uint8_t city_flags = 0;
    // `AoWE.TPlayerStructure` subclasses that carry their own status byte:
    //   Farm.TFarm   rw tag 0x14 -> +0x44  bit 0 = GetRazed@557b2b80
    //   Mine.TMine   rw tag 0x14 -> +0x3c  bit 0 = GetRazed@557b4640
    //   PowerNode.TPowerNode rw tag 0x14 -> +0x3c bit 0
    //   TProductionPlace rw tag 0x0a -> +0x48 (1 = razed, GetRazed@557be558)
    // `razed` is the resolved result. A razed structure is deactivated and
    // unregistered from the owner's income list, so it pays nothing.
    std::uint8_t structure_status = 0;
    bool razed = false;
    // --- runtime ownership state (never read from the map file) ------------
    // `TPlayerStructure+0x32` (and `TCity+0x54`) - the flag *byte*
    // `UpdateFlagID` computes and `Show` reads: 0x3f = draw no flag, 0x20 (plus
    // the city pole bits) = the neutral flag, otherwise the owner's race id.
    // See aow/structure_capture.hpp (`structure_flag_id`).
    std::uint8_t flag_id = 0x3f;
    // `TPlayerStructure+0x31` - the "seen by a player" marker SetPlayer clears
    // before it re-runs `UpdatePlayerSeen`.
    std::uint8_t seen_state = 0;
    // `TStructure.GetBuilding` - true while a builder is constructing/rebuilding
    // the structure. A scenario never stores one (`TStructure.Create` leaves the
    // build counter at -1), so map-placed structures start false.
    bool building = false;
    // `City.TCity` (class 0x20108) only: rw **tag 0xc** -> +0x48 is the city's
    // name, an AnsiString. `TCity.ReadWrite@557acbcc` streams it through
    // `TEReadStorageStream.rwString` (one length byte, 0xff => a u32 length,
    // then the bytes) and `City.TCity.GetName@557ac918` simply copies +0x48 -
    // the string the Structure floater's StructNameLbl shows (TIWindow.ShowInfo
    // -> the city panel at 0x443318), so the name really is in the map. Empty
    // means the tag was absent (`rwString` omits empty strings and the reader
    // clears the field); the original then shows a blank label. Every shipped
    // map names its cities ("Venice" in Bloody Marauders).
    std::string city_name;
    // `City.TCity` (class 0x20108) only: `City.TCity.ReadWrite@557acbcc` rw
    // **tag 0x10** writes +0x4d, the city's **upgrade level** (1..size, the
    // `TCity.SetUpgradeLevel@557ac75c` field). It is NOT the city's hex size:
    // that is `TCityResource+0x78`, built by `TCityResource.ActivateMsg` as the
    // number of valid terrain-list entries (see docs/26_CITY_SCREEN.md).
    // `TCity.UpdateFlagID@557ac8b0` packs this level into the flag byte's
    // pole-select bits: `race + (level - 1) * 0x40`, and the flag draw reads
    // those bits back out (`flagbyte >> 6`) to pick the flagpole sprite.
    std::uint8_t city_level = 0;
    // `City.TCity` rw **tag 0x12** is the city's `AoWE.TProductionControl`
    // object (`TCity.ReadWrite@557acbcc` calls `rwEObject`). Its own rw tags
    // (`AoWE.TProductionControl.ReadWrite@5575dc18`):
    //   0x14 -> +0x14 TProductionQueue     (the QueueLB rows)
    //   0x15 -> +0x1c TUnitProductionList  (the P1CB..P12CB production cards)
    //   0x16 -> +0x24 TProductionSettings  (the item currently being produced)
    //   0x17 -> +0x20 `rwByte`  the production state (0 idle, 1 waiting for
    //                           gold, 2 active; `NewTurn@5575dd00`)
    //   0x18 -> +0x18 `rwBool`  the queue's LoopQueueBox
    // The two child lists are serialised as tag 1 (rwChildren). The parser in
    // map.cpp fills the fields below; they stay empty for non-city objects and
    // for maps whose city has no production control.
    std::vector<CityProductionUnit> production_units;
    std::vector<CityQueueItem> production_queue;
    // Runtime mirror of the settings object's TProductionItem-shaped triple
    // (+8 paid gold, +0xc paid mana, +0x10 turns left). The original object is
    // a `TProductionSettings` streamed with tags 0x0a..0x13
    // (`TProductionSettings.ReadWrite@5575c8c4`); `parse_production` reads the
    // meaningful ones into the scalar fields below and copies this triple.
    CityQueueItem production_current;
    bool production_current_present = false;
    std::uint8_t production_state = 0;  // +0x20, tag 0x17
    bool loop_queue = false;            // +0x18, tag 0x18
    // --- runtime production progress -----------------------------------------
    // `TProductionControl+0x24` is the object `ExecuteProduction@5575d118`
    // fills while an item runs: +8 gold paid, +0xc mana paid, +0x10 turns left
    // (a *signed byte*), +0x1c total turns, +0x1d owner player.
    // `TProductionControl.NewTurn@5575dd00` decrements +0x10 once at the start
    // of the owner's turn and `FUN_0043f574` reads that same +0x10 every time
    // it refreshes the overview's ActiveActionTurns, so these fields are the
    // single source of truth for the number the city screen shows. For a save
    // loaded mid-production `parse_production` seeds them from rw tag 0x16.
    std::int32_t production_turns_left = 0;   // +0x10, tag 0x0c
    std::int32_t production_turns_total = 0;  // +0x1c, tag 0x0d
    std::int32_t production_gold_paid = 0;    // +8,   tag 0x0a
    std::int32_t production_mana_paid = 0;    // +0xc, tag 0x0b
    std::vector<MapUnit> units;
    std::vector<std::uint8_t> payload;  // raw sprite bytes

    // --- runtime army state (never read from the map file) -----------------
    // `AoWE.TArmy+0x28`: bit 0 is the Guard stance (`PGuardChange`, the party
    // window's Guard), and `TAoWHSMap.SelectNextArmy` (0x55775fa0) only offers
    // an army whose `army_flags & 3` is 0. Cleared when the army moves.
    std::uint8_t army_flags = 0;
    // Remaining move points this turn (`TArmy.MovePoints` after the army has
    // moved; `TAbstractUnit.NewTurn` refills them when its owner's turn
    // begins). -1 = the army has not moved yet, so its units' allowance is
    // still full. `SelectNextArmy` requires at least 2.
    std::int16_t army_mp_left = -1;
};

// One map hex (a `TMapField`). `objects` is in placement order.
struct MapTile {
    std::uint32_t tag = 0;  // field-table tag (index within the level)
    // `TAoWMapField.ReadWrite@0x55771c28` adds **one** tag to TMapField's: tag 4,
    // a 4-byte blob at +0x18 - the *explored* bitmask (`TAoWMapField.
    // GetExplored@0x55771bf4` returns bit `(player & 0x1f)` of that u32, so bit i
    // = "player i has explored this hex"). 0 for a hex nobody has seen.
    std::uint32_t explored = 0;
    // The hex's `TMapField` terrain (+0x14) and overlay (+0x15), resolved from
    // the tile's sprites by `aow::fill_tile_terrain` (aow/movement.hpp) - see
    // `aow::move_point_cost` for the overlay codes. `terrain` is -1 when the tile
    // has no terrain hexagon; `overlay` is the raw TTerrainList code of the
    // topmost sprite that has one (-1 = none/plains, 0 = mountains, 1 = forest,
    // 2 = hill, 4 = road/city, 5 = bridge, ...).
    int terrain = -1;
    int overlay = 0;
    std::vector<MapHs> objects;
};

struct MapLevel {
    std::size_t index = 0;      // level number (0 = surface)
    std::uint32_t width = 0;    // in hexes
    std::uint32_t height = 0;   // in hexes
    std::vector<MapTile> tiles;  // width*height, row-major
};

struct Map {
    std::uint32_t class_id = 0;   // top object class id (0x20011 = TAoWHSMap)
    bool compressed = false;      // true when the file was a 'CFS\0' zlib stream
    std::uint32_t width = 0;      // container width (hexes)
    std::uint32_t height = 0;     // container height (hexes)
    std::size_t level_count = 0;  // container level count
    std::string hss_file;         // TAoWHSMap tag 2 (HSS file name)
    // TAoWHSMap rw **tag 0x13** -> +0x174: the scenario's day counter
    // (`TPlayerControl.NewDay@55754da4` increments it). A fresh `.HSM`/`.CSM`
    // omits the tag, which `rwInteger` reads back as 0; only a saved game
    // carries the live day. The port's turn loop starts at day 1 (docs/14).
    std::int32_t day = 0;
    // TAoWHSMap rw **tag 0x1c** -> +0x230: the map's RNG state. `TAoWHSMap.
    // Random@5577827c` copies it into Delphi's `RandSeed`, calls `System.Random`
    // and stores the advanced value back; every random map decision (turn
    // order, `Gems.TGems.GenerateGemsAmount`, city rebellions, ...) draws from
    // this one sequence. Shipped maps do carry it (e.g. Tutorial 0x18e2d3b6),
    // so the port replays the exact Borland `_RandInt` stream.
    std::uint32_t rand_seed = 0;
    // The scenario's music playlist. TAoWHSMap holds two `Sound.TSonglist`
    // objects, each serialised as an EObject whose tag 10 is the
    // `TStringList.Text` (a u32 byte length followed by CRLF-separated song
    // file names, with a trailing CRLF):
    //   tag 0x1d -> AoWE.TStartupSongList (+0x110)  the scenario's "original" songs
    //   tag 0x3c -> Sound.TSonglist      (+0x114)  the (possibly edited) song list
    // AoW.exe FUN_00454d2c plays tag 0x3c when it has entries, otherwise tag
    // 0x1d (skipping any song whose title is the "FileNotFound" resource, i.e.
    // a missing file), otherwise the default "Songs\Heartwoo.it". The player
    // then walks the list in order, wrapping to the first song at the end.
    // See docs/09_GAME_SCREEN.md (in-game music).
    std::vector<std::string> songs_startup;  // TAoWHSMap tag 0x1d
    std::vector<std::string> songs_list;     // TAoWHSMap tag 0x3c
    std::vector<MapLevel> levels;
    std::vector<std::uint8_t> region;  // raw (inflated) EObject region
};

// Returns a stable human-readable name for a map-related Engine class id, or
// "class_0xXXXXX" when unknown. Names come from the decompiled ClassID getters.
const char* map_class_name(std::uint32_t class_id);

// True when `class_id` is `AoWE.TPlayerStructure` (0x2034c) or one of its
// subclasses - i.e. a map object the engine's class hierarchy calls an *ownable*
// (a "takeable") structure, whose `Show` is `AoWE.TPlayerStructure.Show@55760f94`
// and therefore draws the owner's flag.
//
// The list is not guessed: it was read straight out of the module's Delphi VMTs
// (every class carries `vmtParent` at `vmt-36` holding `parentVmt-76`) and
// cross-checked against each class's decompiled `ClassID` getter. The subtree is
//
//   TPlayerStructure            0x2034c   (base)
//     TPlayerCropStructure      0x2034e  -> TCity 0x20108, Farm.TFarm 0x2036e
//     TProductionPlace          0x203ba  -> Shipyard 0x204be, BldGuild 0x2092e,
//                                          Water/Death/Earth/Fire/Life/AirNode
//                                          0x204b3/0x204ae/0x204ce/0x203de/
//                                          0x20371/0x2094e, RandomNode 0x20822
//     Mine.TMine                0x2037a
//     Tower.TTower              0x2043e
//     PowerNode.TPowerNode      0x2019a
//     ReflectingPool            0x2063e
//     Altar.TAltar              0x203f1  -> the six TAltarOf* (Life 0x20233,
//                                          Death 0x20235, Fire 0x2023a,
//                                          Lightning 0x2023d, Ice 0x2024b,
//                                          Pestilence 0x2030c)
//
// Everything else that looks like a structure is a plain `TStructure`
// (Wizard's Tower, Cave, Ruin, Pyramid, Crypt, Dungeon, Monsterlair, Arena,
// MagicSanctuary, Sign, Teleport, Spring of Life, Blood Shrine, Holy/Unholy
// Ground, Gems/ManaCrystals/Crops - the latter three are `ILTer.TILTerrainMO`)
// and never draws a flag. See docs/19_STRUCTURE_FLAGS.md.
bool is_player_structure_class(std::uint32_t class_id);

// True for `PrdPlace.TProductionPlace` and every subclass (Shipyard,
// BuildersGuild, the six magic nodes, RandomNode). These are the structures
// that register a gold income source (sphere 8) or a magic power source (a
// node) when activated - see `PrdPlace.TProductionPlace.Create@557be10c`.
bool is_production_place_class(std::uint32_t class_id);

// Loads a `.HSM` / `.CSM` (or `.HSS`) file. Returns false and sets `error`.
bool load_map(const std::string& path, Map& out, std::string& error);

}  // namespace aow
