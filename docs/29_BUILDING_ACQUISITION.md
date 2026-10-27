# 29 - Building acquisition: how a world-map structure changes hands

This document records Phase 1-3 of the building-acquisition work: the rules from
the Age of Wonders wiki, the AoWEPACK.dpl functions recovered with **GhidraMCP**,
and their 1:1 implementation in the port. The code is
`src/include/aow/structure_capture.hpp` + the "Building acquisition" section of
`src/engine/game_state.cpp`; the UI hook is `Scene::game_structure_army_update`
in `src/game/menu.cpp`; the checks are the `capture:` set in
`src/tests/game_state_test.cpp`.

Every decompile cited here is under `port/re/exports/capture/`, dumped with
`tools/ghidra/mcp_call.py` (port 8089) from `AoWEPACK.dpl` (image base
`0x55700000`). `tmp_gr/dump_capture.py`, `dump_place.py`, `dump_move.py` and
`dump_city_capture.py` reproduce the dumps.

## 0. The vtable-offset correction (read this first)

The decompiles address virtual methods as `(**(code **)(*self + 0xNNN))()`.
`tools/vmt_scan.py` reports a class VMT 12 bytes *after* the reference this
Borland compiler actually stores in an object's field 0:

```
vmt_scan: TPlayerStructure vmt=0x5571434c
object->class   = 0x55714340   (= vmt_scan - 12)
class+0x1f0 = 0x55714530 -> 0x55760d08  TPlayerStructure.SetPlayer
class+0x1f4 = 0x55714534 -> 0x55760f3c  GetIncome
class+0x1fc = 0x5571453c -> 0x55760ed8  UpdateFlagID
class+0x208 = 0x55714548 -> 0x55761148  UpdateExploration
class+0x20c = 0x5571454c -> 0x55760cd4  FinalizeCombat
class+0x214 = 0x55714554 -> 0x55760ac4  StructureCaptured
class+0x220 = 0x55714560 -> 0x55760980  UpdatePlayer
```

`tmp_gr/dump_vmt.py` applies that `-12` and resolves every slot through
`/get_function_by_address`, which is how the table below was produced. Without
it, `+0x1f0` looks like `UpdateFlagID` and the ownership path makes no sense.

| slot | TPlayerStructure method | address |
|---|---|---|
| +0x150 | TStructure.GetRazed | 0x5575ede4 |
| +0x154 | TStructure.GetRazeable | 0x5575eddc |
| +0x174 | TPlayerStructure.ArmyPlaced | 0x55760f24 |
| +0x178 | TStructure.ArmyRemoved | 0x5575f364 |
| +0x190 | TPlayerStructure.BuildingDone | 0x557608f4 |
| +0x194 | TPlayerStructure.ExecuteRebuild | 0x55760928 |
| +0x198 | TPlayerStructure.ExecuteBuild | 0x55760954 |
| +0x19c | TStructure.GenerateRazeDefenders | 0x5575fd1c |
| +0x1a0 | TStructure.PlaceRazeDefenders | 0x5575fe84 |
| +0x1a4 | TStructure.ExecuteRaze | 0x5575ffc8 |
| +0x1ec | TStructure.CanRaze | 0x5575fb80 |
| +0x1f0 | **TPlayerStructure.SetPlayer** | **0x55760d08** |
| +0x1f4 | GetIncome | 0x55760f3c |
| +0x1f8 | GetPower | 0x55760f40 |
| +0x1fc | UpdateFlagID | 0x55760ed8 |
| +0x204 | UpdateUnfog | 0x55761228 |
| +0x208 | UpdateExploration | 0x55761148 |
| +0x20c | FinalizeCombat | 0x55760cd4 |
| +0x210 | GetCombatEventText | 0x55760c1c |
| +0x214 | StructureCaptured | 0x55760ac4 |
| +0x218 | GetMoveAction | 0x55761620 |
| +0x21c | GetDefaultAITargetPriority | 0x557615ac |
| +0x220 | **UpdatePlayer** | **0x55760980** |

`TCity` overrides the same slots: `SetPlayer@557aa2c8`, `UpdateFlagID@557ac8b0`,
`UpdatePlayer@557aa9fc`, `FinalizeCombat@557acefc`,
`GetCombatEventText@557acdb8`, `StructureCaptured@557aa0a4`,
`GetMoveAction@557ad0d4`. `TProductionPlace` overrides `SetPlayer@557befcc`
(income/power registration), `TMine` `@557b4564`, `TPowerNode@557d0584`,
`TAltar@557cf234`, `TReflectingPool@557d4f1c`, `TPlayerCropStructure@557a6278`.

## 1. Phase 1 - what the wiki says

The wiki is <https://ageofwonders.fandom.com>. Its HTML endpoints answer 403 to
the fetch tool, but the MediaWiki API serves the same text:
`api.php?action=parse&page=<name>&prop=wikitext&format=json` (and
`prop=extracts` is not enabled on this wiki, so the wikitext is used directly).

Sources:

* `Structure` - "a special location on the world map that can be conquered,
  explored, or otherwise interacted with ... All structures, whether
  controllable or not, behave like roads for the purpose of grounded units'
  movement across them."
* `City (Age of Wonders)` - city sizes/income, the per-turn city actions and
  their effects.
* `Node` - power nodes vs elemental nodes, mana per turn.
* `City status` - sentiment bands and the per-day rebellion chance.
* `Race relations` - the point bands and the modifiers a capture/raze/fortify
  applies.
* `Builder` - the Builder's Guild unit that builds roads/towers and repairs
  razed cities.

### 1.1 Which structures can be owned

| ownable (`TPlayerStructure` subtree) | not ownable (plain `TStructure`) |
|---|---|
| City, Farm | Wizard's Tower, Cave, Ruin, Pyramid, Crypt, Dungeon, Monsterlair |
| Mine, Tower, Power Node | Arena, Magic Sanctuary, Sign, Teleport |
| Shipyard, Builder's Guild | Spring of Life, Blood Shrine, Holy/Unholy Ground |
| the six elemental nodes, Random Node, Reflecting Pool, Altar and its six variants | Gems, Mana Crystals, Crops (`ILTer.TILTerrainMO`) |

The class table is in `docs/19_STRUCTURE_FLAGS.md` section 1; only the
`TPlayerStructure` subtree draws an owner flag.

### 1.2 City size, income and upgrades

| city size | base income | max crop fields | total income | max upgrade level |
|---|---:|---:|---:|---:|
| 1 hex | 5 gold | 6 | 17 | 1 |
| 2 hexes | 10 | 8 | 26 | 2 |
| 3 hexes | 15 | 9 | 33 | 3 |
| 4 hexes | 20 | 10 | 40 | 4 |

Each eligible free hex around the city becomes a crop worth 2 gold. A city can
never change its hex size, which caps its upgrade level; the upgrade level
determines which unit levels can be installed/trained.

### 1.3 Node mana

* Power Node: 10 mana per turn to whoever controls it, whatever spheres the
  leader has.
* Elemental node: 15 mana with one sphere of its element, +5 per additional
  sphere (0 spheres = 0). Each element's Mastery spell summons a level-4
  elemental at every matching node the player owns.

### 1.4 The per-turn city actions

Install unit, Produce unit, Produce merchandise (+25% income, the default),
Upgrade (+5 race relations), Fortify (wooden/stone wall, +5 race relations and
city status), Migrate (changes the population race; -15/+10 relations), Loot
(gradual destruction with gold), Raze (instant destruction, needs sufficient
forces). A razed city can be rebuilt by a **Builder**.

### 1.5 City status and rebellion

City morale = race relations (0..100) plus modifiers. For positive/neutral
relations the bands are Cheerful 80-100, Content 60-79, Stable 40-59 (0 %
rebellion). For negative relations: Enslaved 80-100, Oppressed 60-79, Stable
40-59 (all 0 %), Unrest 20-39 (10 %/day), Unruly 0-19 (50 %/day).

Modifiers include Hate -35, Wary -25, hostile terrain -10, wooden wall +5,
friendly terrain +10, stone wall +10, Polite +15, Friendly +25, weak/average/
strong occupation +25/+50/+75. The wiki notes that the occupation values in the
manual addendum do not match observation.

### 1.6 Race relations and capture

Relations are a 0..100 scale: Friendly 80-100, Polite 60-79, Neutral 40-59,
Wary 20-39, Hate 0-19. Looting/razing a city costs -30, migrating away -15,
fortifying/upgrading +5, migrating toward +10; relations drift ±1 per day
toward their initial value. Capturing a city is therefore not only a map change:
its status (and the chance it rebels back to the independents) follows from the
conquering leader's relation with that race.

## 2. Phase 2 - the decompiled ownership path

### 2.1 Walking in: `ArmyPlaced -> UpdatePlayer`

`TArmyHS.PlaceOnMap@557909f4` places an army and sends map-field message
`0x1102` to its field:

```c
local_28[0] = 0x1102;
local_20 = param_1;                     // the TArmyHS
HSEngine_TMapField_SendMapFieldMsg(field, local_28);
```

`TStructure.MapFieldMsgProc@5575f730` forwards it:

```c
else if (iVar1 == 0x1102)
    (**(code **)(*param_1 + 0x180))(param_1, param_2[2]);  // ArmyPlaced
```

`TPlayerStructure.ArmyPlaced@55760f24` is one line: `-> UpdatePlayer (+0x220)`.
`TCity.ArmyPlaced@557aab78` adds the Fountain-of-Life and fade-shell effects and
`TCity.ArmyRemoved@557aaba0` calls `UpdatePlayer` too.

`TPlayerStructure.UpdatePlayer@55760980` is the auto-capture rule:

```c
owner = -1; conflict = false;
for every hex the structure covers:
    field = map.GetField(x, y);
    army  = field.GetObject(0x20217);        // AoWE.TArmyHS
    if (army == NULL) continue;
    a = army.owner;                          // TArmy+0x12
    if (owner == -1) owner = a;
    else conflict = (a != owner);
if (owner == -1) return;                     // no army: keep the present owner
SetPlayer(conflict ? -1 : owner);
```

So **a structure is claimed by simply standing on it** when no other player's
army is on the structure. Two different owners on a multi-hex city make it
owner -1 (contested). A city with no army keeps its owner when the garrison
leaves.

`TPlayerStructure.GetMoveAction@55761620` decides whether the step is an attack:

```c
if (player != owner && !GetRazed() &&
    (owner != 0 || *(char *)(resource + 0x64) != 0))
    result = 1;                               // attack
```

`resource+0x64` is 1 only for `City.TCityResource` (the "neutral flag" flag), so
an **unowned mine/tower/node (owner 0, no neutral flag) answers 0: walk in**, and
the `ArmyPlaced` pass above claims it. An independent **city** answers 1
(attack). `TCity.GetMoveAction@557ad0d4` adds the `JoinAmount` branch: if the
independents would join, the action is 2 (peaceful join); the port has no
diplomacy and reports 1.

### 2.2 Winning a battle: `FinalizeCombat`

`TCombat.Finalize@55727e38` stores the result (+0x14) in the combat event log and
sends message `0x1110` to the battle's field; `TPlayerStructure.MapFieldMsgProc`
answers `0x1110` by calling `FinalizeCombat` (+0x20c):

```c
// TPlayerStructure.FinalizeCombat@55760cd4 / TCity.FinalizeCombat@557acefc
if (combat.result == 3 || combat.result == 6) {
    attacker = TCombatPlayerList.GetPlayers(combat.data + 0x38,
                                            combat.data + 0x24);  // attacker
    SetPlayer(this, attacker.player_index);
    UpdatePlayer(this);                    // TCity only, base SetPlayer did it
}
```

The result codes are named by `TCombat.GetCombatResultText@5572836c`:

| code | meaning |
|---:|---|
| 3 | Battle Won (attacker) -> capture |
| 4 | Battle Lost -> no change |
| 6 | defender yielded -> capture |
| 8 | Cannot Breach Wall -> no change |
| 2 | Cannot Inflict Damage -> no change |
| anything else | Battle Drawn -> no change |

`TCombat.Setup@55727310` stores the attacker at combat-data `+0x24` and the
defender at `+0x25`; FinalizeCombat reads `+0x24`, so 3 and 6 both mean the
attacker takes the structure.

### 2.3 `TPlayerStructure.SetPlayer@55760d08` in full

```c
if (new_owner != this.owner) {
    if (GetRazed() && !GetBuilding()) return;          // a ruin cannot be taken
    this+0x31 = 0;                                     // seen marker
    if (old_owner != -1)
        TPlayerList.GetPlayers(old_owner).RemoveStructure(this);
    if (map+0x174 > 0)
        StructureCaptured(old_owner, new_owner);        // +0x214
    this.owner = new_owner;
    UpdateUnfog();                                      // +0x204
    UpdatePlayerSeen();
    UpdateExploration();                                // +0x208
    ScannerIconsChangedRad(...);
    UpdateFlagID();                                     // +0x1fc
    FogChanged();
    NotifyPlayerChanged(...);
    if (new_owner != -1)
        TPlayerList.GetPlayers(new_owner).AddStructure(this);
    if (new_owner > 0 && map+0x14d == 1) {
        for every other player i:
            if (BitList.Get(this+0x34, i) &&
                (i == new_owner ||
                 (map+0x14b && diplomatic_relation(new_owner, i) == 2)))
                TPlayerList.GetPlayers(i).CheckCaptureLocations();
    }
    UpdatePlayer();                                     // +0x220
}
```

`StructureCaptured` (`0x55760ac4`, city override `0x557aa0a4`) only logs when the
*old* owner is not 0 (an independent structure's capture is silent), the map's
`+0x3c` bit 1 is clear and the structure has been revealed; it formats
`StructureCapturedRStr` with the structure/city name.

### 2.4 The flag byte: `UpdateFlagID`

`TPlayerStructure.UpdateFlagID@55760ed8`:

```c
if ((this+0xc & 4) == 0) {
    if (this+0x30 < 1) {
        if (field/resource + 0x64 == 0) { this+0x32 = 0x3f; return; }
        this+0x32 = 0x20;
    } else {
        this+0x32 = TPlayerList.GetPlayers(this+0x30).race;   // +0xa5
    }
} else {
    this+0x32 = 0x3f;
}
```

`TCity.UpdateFlagID@557ac8b0` replaces both branches:

```c
if ((this+0xc & 4) == 0 &&
    ((city_flags & 2) == 0 || GetBuilding()) &&
    (city_flags & 1) == 0) {
    if (owner > 0)
        this+0x54 = player(owner).race + (city_level - 1) * 0x40;
    else
        this+0x54 = (city_level - 1) * 0x40 + 0x20;
} else this+0x54 = 0x3f;
```

That is exactly what `mapviz`'s `structure_flag_byte` already implements for
drawing; `structure_flag_id` is the same rule for the runtime byte the ownership
code stores.

### 2.5 A captured city: `TCity.SetPlayer@557aa2c8`

```c
if (new_owner != owner) {
    productionControl.PlayerChanged();      // 0x5575cef0
    ResetEnchantments();
    if (old_owner != -1) player(old_owner).cities.Remove(productionControl);
    TPlayerCropStructure.SetPlayer(this, new_owner);   // -> base SetPlayer
    if (GetActiveAction() == 4 /* migrate */) {
        productionControl.Something();
        TProductionQueue.GetItem(queue, 0); TObject.Free();
    }
    ValidateAICityAction();
    UpdateAIMigration();
    if (new_owner != -1) player(new_owner).cities.Add(productionControl);
}
```

`TProductionControl.PlayerChanged@5575cef0` is the queue rule the port copies:

```c
for (i = count-1; i >= 1; --i) free(queue[i]);   // keep item 0
UnregisterGroupProduction();
loop_queue = false;
if (count != 0 && production_state != 2) { free(queue[0]); state = 0; }
```

So a captured city **keeps the item it is actively producing** (`state == 2`)
and loses the rest of the queue; an idle city loses everything.

### 2.6 Razing and rebuilding

`TStructure.ExecuteRaze@5575ffc8` formats `StructureRazedRStr`, draws the two
raze animations (sequence 0x14/0x15), then calls `SetRazed(1)` (+0x14c) and
`PlaceRazeDefenders(player)`:

```c
// TCity.SetRazed@557ab47c / TMine.SetRazed@557b4648 / TProductionPlace.SetRazed
if (razed != GetRazed()) {
    stop production / disable crops;
    if (razed) { SetPlayer(this, -1); set the class's razed bit; }
    else       { clear the class's razed bit; }
    this+0x14 = GetTerrainTypeImage(this, this+0x18);   // razed body sprite
}
// TCity.SetRazed also: ResetEnchantments, UpdateImages, SetWallType(0)
```

Because `SetRazed(1)` runs **before** the razed bit is written, the `SetPlayer(-1)`
inside it is allowed, and its trailing `UpdatePlayer` sees the razing army still
standing there and immediately hands the ruin back to that player. That is not a
bug in the port: the original leaves the ruin owned by the razer.

`TStructure.CanRaze@5575fb80` requires `!GetRazed() && GetRazeable()` and runs a
`TCombatPredictor` of the player's units against the structure's defenders: the
result must be 3, 4 or 6 with `GetSuperiority > 0x32` (50), otherwise
`NotEnoughForcesRStr` is returned.

`TCity.GetRebuildInfo@557ab50c` = `size * 0x19` gold and `size + 1` turns;
`TStructure.GetRebuildInfo@5575f18c` reads the structure resource's `+0x5a`
(gold) and `+0x5b` (turns). `TPlayerStructure.ExecuteBuild@55760954` /
`ExecuteRebuild@55760928` call the base construction TE and then
`SetPlayer(builder)` when it finishes.

### 2.7 Other ways a city changes hands

* `TCity.CheckRebellion@557ab534`: for an owned, non-razed city with
  `GetCivilStatus() < 2`, `Random(100) <= 50` (status 0) or `<= 10` (status 1)
  moves the armies out, logs `TCityRebellionEventLog`, calls `SetPlayer(0)` and
  spawns rebel units.
* `TCity.IncommingNegotiateOffer@557ad300`: when an accepted offer's
  `JoinAmount` is payable, it calls `SetPlayer(offering_player)`, transfers the
  city's independent garrison (`TArmy.SetPlayer`) and logs `XOverthrownRStr`.
* `TPlayerStructure.MsgProc@557614f4`: message `0x20020005` clamps an owner
  whose index exceeds the roster back to player 0.

## 3. Phase 3 - the port implementation

### 3.1 API (`src/include/aow/structure_capture.hpp`)

| port function | engine routine |
|---|---|
| `structure_razed` / `structure_building` | `GetRazed` / `GetBuilding` |
| `structure_flag_id` | `TPlayerStructure.UpdateFlagID` / `TCity.UpdateFlagID` |
| `structure_move_action` | `TPlayerStructure.GetMoveAction` / `TCity.GetMoveAction` |
| `structures_at` / `structure_at` / `structure_object` | `TMapField.GetObject` + `TMultiHexMO` footprint |
| `structure_size_hexes` | `TCity.GetSize` |
| `structure_display_name` | `GetName` / the structure resource name |
| `structure_set_player` | `TPlayerStructure.SetPlayer` (+ city queue reset) |
| `structure_update_player` | `TPlayerStructure.UpdatePlayer` |
| `structure_has_foreign_army` | the "combat has not removed the defender yet" guard |
| `structure_finalize_combat` | `TPlayerStructure/TCity.FinalizeCombat` |
| `structure_set_razed` | the `SetRazed` overrides |
| `structure_get_rebuild_info` | `TStructure/TCity.GetRebuildInfo` |
| `structure_complete_rebuild` | `ExecuteBuild/ExecuteRebuild -> SetPlayer` |
| `structure_city_check_rebellion` | `TCity.CheckRebellion` |

`MapHs` gained three runtime-only fields: `flag_id` (+0x32 / TCity+0x54),
`seen_state` (+0x31) and `building` (GetBuilding). A map file never stores them,
so parsing is unchanged.

The side effects the port can model are applied in the engine's order: the
rebuild gate, the city production `PlayerChanged`, the owner byte, the explored
bit for the new owner, the flag byte and the final `UpdatePlayer`. Income and
power registration are not stored twice - `aow::compute_economy` walks the map
and recomputes both, which is the port's equivalent of the
`TPlayer.RegisterIncomeSource` / `TPlayerMagicControl.RegisterPowerSource` lists.

### 3.2 The UI path

* `Scene::game_structure_army_update` is the `0x1102` message: after a party is
  placed on a hex it calls `structure_update_player` for every ownable structure
  covering it, recomputes the economy, refreshes an open Structure floater and
  re-renders the level. `--game-capture X Y` runs the same helper head-lessly
  and prints `game_capture_hook=X,Y ok=N`, `game_structure_owner=...` and
  `game_capture=...`.
* `game_move_selected` calls it on the source as the party leaves, but on the
  destination only when the walk *arrives*: the army's data is put on the hex at
  once (so its resting copy can be hidden while the sprite walks) and the hex is
  stored in `mv_arrive_pending`, so the building under it keeps its owner and
  flag for the whole transition. `game_update`'s walk tick ends with
  `TMoveArmyTE.FinishMove`, which runs `mv_finish_arrival` -> the same
  `ArmyPlaced` pass, and the level is re-rendered right after with the new
  owner's flag. A degenerate path (no walk) runs the pass straight away.
  `mv_finish_merge` does the same when an incoming party merges into one
  standing on the building (there the destination army never leaves the map, so
  nothing changes hands mid-walk either).
* `--game-anim N` is what plays a walk out head-lessly: the clicks only *order*
  the move, so a capture proof has to tick the transition.
* When a defender still stands on the building the port has no combat model, so
  the arriving pass is skipped (`structure_has_foreign_army`). The future combat
  path calls `structure_finalize_combat(..., won, attacker, ...)` directly.
* `Scene::game_move_action` now also asks the structure, so the crossed-swords
  cursor appears over an enemy city/mine (`TAoWMapField.GetMoveAction`'s 0x1101
  message goes to every sprite, not only armies).

Live check on the shipped Tutorial (the human hero starts at (15,34), the
independent Tower at (19,31) is 16 move points away). The clicks only *order*
the move, so the walk has to be ticked for the claim to happen - `--game-anim
200` runs the transition to its end:

```
AoW.exe --root ".." --screen game --no-music --no-cursor \
  --render bin/tmp_capture.png \
  --game-click 513 141 --game-click 220 216 --game-click 348 120 --game-anim 200
# game_msg=moved (15,34) -> (19,31)
# game_move=15,34->19,31 dir=1
# game_structure_owner=19,31 owner=1 old=0 flag=0x5
# game_capture=19,31 player=1
```

Without the `--game-anim` ticks the last three lines are absent: at that point
the walk is still running (`game_walk=...`, the arrival army hidden on its hex)
and the tower is still the independents'. The tower was owned by the
independents; the arriving party claims it with the player's race flag
(Halflings race 5 -> flag byte 0x05) at the end of the transition, and the
economy is recomputed before the frame is drawn.

### 3.3 Checks (`src/tests/game_state_test.cpp`, 58 new)

The synthetic 2-hex city locks: the neutral/owned/razed flag bytes (including
the `(level-1)<<6` pole bits), the walk-in vs attack move actions, silent
independent captures vs logged player captures, the `SetPlayer(-1) -> 0xff`
owner byte, the razed gate, `PlayerChanged` keeping only the running queue item,
the contested multi-hex result, the razing army keeping the ruin, rebuild
cost/turns and ownership, and the FinalizeCombat result set. The real Tutorial
map locks `structures_at` (Gobton/Kilia/the mine at 18,22), the city sizes
(City1 = 1, City3 = 3), and an actual `ArmyPlaced` claim of the independent mine.

### 3.4 Out of scope / `TODO(RE)`

* **Combat** is not simulated, so `structure_finalize_combat` is an API for the
  future battle system; the move path only performs the undefended claim.
* **Fog of war / visibility**: the explored bit is set for the covered hexes;
  the engine's per-player fog/seen lists (`UpdateUnfog`, `UpdatePlayerSeen`,
  `NotifyPlayerChanged`) are not modelled.
* **Victory locations**: `RegisterVictoryLocations` / `CheckCaptureLocations`
  have no port equivalent yet.
* **Diplomacy**: `TCity.JoinAmount` / `IncommingNegotiateOffer` are not modelled;
  a neutral city always answers move-action 1.
* **AI actions**: `ValidateAICityAction` / `UpdateAIMigration` are skipped; the
  port has no AI city planner.
* **Non-city rebuild costs**: `TStructure.GetRebuildInfo`'s resource `+0x5a` /
  `+0x5b` are not parsed, so `structure_get_rebuild_info` returns 0 for them.
* **Razing**: `CanRaze`'s `TCombatPredictor` superiority check and the raze
  animations are not implemented; only `SetRazed`/rebuild state is.
* **Enchantments**: `ResetEnchantments` on capture/raze is a no-op (no city-spell
  model yet).
