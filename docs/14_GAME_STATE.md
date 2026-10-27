# 14 - The game state: the player roster and the turn/day loop

This document records the reverse-engineering of the **Age of Wonders 1 game
state** that sits on top of a scenario map - the players, their resources and
troops, and the turn/day loop - and its C++ reconstruction in
`src/include/aow/game_state.hpp` + `src/engine/game_state.cpp`, with tests in
`src/tests/game_state_test.cpp`.

It closes the open item from `docs/HANDOFF_faithful_mapviewer.md` ("the
player/setup lists (tag 0x15 TPlayerList, 0x16 TRaceList) are located but their
per-record tags are not yet mapped"): the roster is now parsed and exposed as
typed `aow::Player` records, and the turn loop is implemented.

Everything was recovered with **GhidraMCP** (`tools/ghidra/`) from
`AoWEPACK.dpl` (image base `0x55700000`); the decompiles live under
`re/exports/aowepack_game/`. No separate palette or data file is needed - the
roster is part of the `.HSM`/`.CSM` object graph the map loader already decodes.

## The class map

| Delphi class | Class id | key methods (AoWEPACK) |
|--------------|----------|------------------------|
| `AoWE.TPlayerList` | (a `TEControlStyleNode`) | `Create` `0x55754014`, `GetPlayers` `0x557544d0`, `GetNumberOfPlayers` `0x55754674`, `SetNumberOfPlayers` `0x557546b0`, `Assign` `0x5575447c`, `GetActivePlayers` `0x5575434c`, `Update` `0x55754370` |
| `AoWE.TPlayer` | `0x20020` (`ClassID` `0x557516c8`) | `Create` `0x55750ea8`, `ReadWrite` `0x5575314c`, `GetName` `0x55752104`, `GetUpkeep` `0x557520c4`, `SetGems` `0x55751f38`, `UpdateIncome` `0x55751f88`, `GetUnitCount` `0x55751804`, `UpdateTurnInfo` `0x55752d70`, `SetEndTurn` `0x55752d00` |
| `AoWE.TPlayerMagicControl` | - | `Create` `0x5577c588`, `ReadWrite` `0x5577c748`, `GetPower` `0x5577ca00`, `GetManaIncome` `0x5577c95c`, `GetNetPower` `0x5577cec4`, `GetNetPowerToResearch` `0x5577cf80`, `SetMana` `0x5577c9b0`, `NewTurn` `0x5577cc2c` |
| `AoWE.TPlayerControl` | - | `NewDay` `0x55754da4`, `NewTurn` `0x557553c4`, `EndTurn` `0x5575530c`, `Setup` `0x55754b94`, `StartNewMap` `0x5575573c` |
| `AoWE.TAoWHSMap` | `0x20011` | `GenerateNewTurnOrder` `0x557782fc`, `EndTurn` `0x5577838c` |

## On-disk: the roster is TAoWHSMap tag 0x15

`AoWE.TAoWHSMap.ReadWrite` (`0x55776d60`) writes the roster at **tag 0x15** as a
`rwEObject`. `TPlayerList` is a `TEControlStyleNode`, so its own **tag 1** is the
children table, and each child is the usual `[u32 class id][table][data]` record
whose class id is **`0x20020`** (`AoWE.TPlayer`).

```
TAoWHSMap                          (top object; class 0x20011)
  tag 0x15  rwEObject -> TPlayerList
    tag 1   children -> one record per player, in index order:
      [u32 0x20020][TPlayer table]
```

Verified against the shipped maps (a probe of tag 0x15 on `Tutorial.hsm` and
`Dwarf1.CSM`): the children are exactly the roster entries, entry 0 being the
independents/monsters and 1..N the players.

### TPlayer.ReadWrite tags (`0x5575314c`)

The serializer omits fields equal to the value it would write anyway, so a
**missing tag means the stream's own default**, which is **0** for
`rwByte`/`rwInteger` (`TEReadStorageStream.rwByte` @ Enginep vtable `+0x30`
literally stores `0` when the tag is absent; `rwData` `+0x1c` leaves the field
untouched, so it keeps `TPlayer.Create`'s 0). The `TPlayer.Create`
(`0x55750ea8`) `0xff` sentinels therefore only survive when the tag *is*
written. Offsets are from the object pointer (the vtable).

| tag | off | type | field | evidence |
|-----|-----|------|-------|----------|
| `0x02` | `+0xa5` | `rwByte` | race id (`0xff` = independents; **absent = 0 = Humans**) | `SetRace` `0x55752cf8`; maps show 255/5/6/9/10, and omit the tag for a Humans player (Europe player 1, Barondir Bay player 3, ...) |
| `0x03` | `+0xa7` | `rwData(1)` | **player type** (0 human, AI levels, 4 independents) | `TSetupControl.hPickMap` `0x557e20c8` reads `+0xa7` into the race's `TSetupSettings`; verified against the 9 campaign maps (see below) |
| `0x05` | `+0xac` | `rwInteger` | **starting gold** (Create default 250) | map values 250/1000/125/275; `TAoWHSMap.StartNewMap@55777d68` copies it into `+0xc4` |
| `0x06` | `+0xb0` | `rwInteger` | base income | `UpdateIncome` reads `+0xb0` |
| `0x0a` | - | `rwByte` | `unknown` (present in maps) | - |
| `0x10` | `+0xc4` | `rwInteger` | **live gold treasury** | `SetGems` `0x55751f38` writes `+0xc4`; `TPlayerControl.NewTurn@557553c4`, `UpdateTurnInfo@55752d70` (TTurnInfoItem+8) and the bar all read it as gold. The historical method name is misleading - AoW1 has no separate crystal currency: Gems.TGems pickups add gold (`Gems.TGems.ArmyEntered@557c50f8`). |
| `0x12` | `+0xa6` | `rwByte` | **player index/id** (absent = 0 = independents) | `SetPlayerIndex` `0x55751838` |
| `0x14`/`0x15`/`0x16` | `+0x2d..+0x2f` | `rwByte` | player colour (RGB; absent = 0) | `ReadWrite` tags 0x14..0x16 |
| `0x17` | `+0xd8` | `rwEObject` | `TEventLogbook` | object (tag 10 inside) |
| `0x18` | `+0x30` | `rwString` | name | (empty in scenarios; `SetName` `0x557522dc` writes `+0x4c`/`leader+0x60`) |
| `0x19` | `+0x44` | `rwByte` | **game-over status** (`2` = out) | `GetActivePlayers` `0x5575434c` tests `+0x44 != 2` |
| `0x1b` | `+0x50` | `rwByte` | **food/starvation penalty** | `NewTurn` `0x557553c4` adjusts `+0x50` |
| `0x1e` | `+0x58`/`+0xd4` | `rwEObject` | the player's `TLeader` | `SetLeader` `0x55752cf0`; parsed - see below |
| `0x1f` | `+0x54` | `rwEObject` | `TPlayerMagicControl` | `Create` `0x55750ea8` |
| `0x22` | `+0x5c` | `rwInteger` | **turns this player has taken** | `NewTurn` increments `+0x5c` |

Non-serialised (runtime) fields filled by `Create`: `+0x28` army list,
`+0x34` name list, `+0x54` magic control, `+0x88` `TAIPlayerControl`,
`+0x8c` `TAIBudgetManager`, `+0x80` hero list, `+0xb4`/`+0xb8` `= -1`.
(`+0xa7` is the *player type*; it **is** serialised as rw tag `0x03`, see below.)

### The leader (`tag 0x1e`) and the Events banner

The tag-0x1e value is a `TLeader` - a `THero` subclass (`TLeader.ReadWrite`
`0x5578b470` = `THero.ReadWrite` `0x55788880` + tags 0x2d/0x2e) written
**inline**, i.e. as a bare property table with no class id, so the reader has to
know it is a hero. The fields the port reads are the ones a placed hero uses
too (`src/include/aow/map_types.hpp`):

| tag | field | meaning |
|-----|-------|---------|
| `0x7` | `THero+0x3c` | the **face index** (`SetFaceIndex` `0x5578883c`) - a tag of `Release\FACERES.PFS` |
| `0x8` | `THero+0x44` | the `THeroResource` index (`SetHeroResourceIndex` `0x55788840`; `-1` unset) |
| `0x0a` | `THero+0x60` | the hero's **name** (`GetName` `0x55786fa4`) |
| `0x0b` | `THero+0x64` | the **nickname** (`GetNickname` `0x55786ab0`) |

`TPlayer.GetName` (`0x55752104`) - what the Events floater's `Player` label shows
- is: "Independents" for index 0; else the leader's name plus its nickname when
set; else the player's own stored name (`+0x30`); else `"Player N"`. That makes
**every** seated player's label its *leader*'s name, which is why
`Scenario\Custom\Thinreed Lake.hsm` (the reviewed screen) reads
"Rasidan of the Orcs" while its player record stores no name at all.
`src/engine/game_state.cpp` parses the record into `Player::leader` and
`Player::display_name()` reproduces the routine; the banner that consumes it
(labels, face, race crest) is `docs/09_GAME_SCREEN.md` section 4.1.

### The player type (`+0xa7`, rw tag `0x03`)

`TPlayer.Create` leaves `+0xa7` at 0 ("human"); the map stores the real value.
Reading it back: `TSetupControl.hPickMap` (`0x557e20c8`, AoWEPACK) walks the
map's `TPlayerList`, and for every non-eliminated entry copies `TPlayer+0xa7`
into that race's `TSetupSettings` record (`+8` / `+0x10`). `TPlayer.ReadWrite`
ends by forcing `+0xa7 = 4` for entry 0 (the independents).

The values are the AI difficulty levels. `TSetupControl.ListAvailablePlayers`
(`0x557e1660`) offers a host the seven `AoWE.TPlayerTypeRStr` captions in the
order 0,1,2,3,6,7,4; `TScenSetup.R*PlayerCB` (`OnChange RacePlayerCBChange`
`0x00412a24`) shows exactly that list:

| value | caption | value | caption |
|-------|---------|-------|---------|
| 0 | Human | 6 | CPU King |
| 1 | CPU Squire | 7 | CPU Emperor |
| 2 | CPU Knight | 4 | Independent |
| 3 | CPU Lord | 5 | (unassigned "None"; not offered in SP) |

Verified against the nine campaign maps (whose titles name the played race) -
the human is always entry 1 with type **0**, and the played race is the map's
own: `Dwarf1.CSM` -> race 6 (Dwarves), `Orc1` -> 9, `Undead` -> 11, `Elf1` ->
4, `Lizardman1` -> 2, `Halfling1` -> 5, `Goblin1` -> 10, `Highman1` -> 7,
`Darkelf1` -> 8; the independents are always type 4 and the further players
"CPU Squire"/"CPU Knight" (types 1/2). This confirms the engine race enum is the
DFM's `R1..R12` order (0 Humans .. 11 The Undead). The Scenario screen uses it to
lit the races and pre-set each `R*PlayerCB` from the picked map - see
`docs/08_SCENARIO_SCREEN.md`.

### The race field (`+0xa5`, rw tag `0x02`)

`TPlayer.SetRace` (`0x55752cf8`) is a plain byte store, and rw tag `0x02` is a
plain `rwByte`: **a map that wrote no race tag means Humans (0)**, not the
`TPlayer.Create` `0xff` sentinel, because `TEReadStorageStream.rwByte` assigns
`0` to the field on the absent-tag branch. 0xff only appears when it *is*
written (the independents' record always carries it).

This matters because many shipped maps omit the tag for exactly one player -
`AoW Europe Heaven Games.hsm` (player 1), `Barondir Bay.HSM` (player 3),
`Blood Isles.hsm` (player 4), `Rise of the South`/`River of Life` (player 2),
the `Valley*.csm` campaign maps (player 5), and others - and the original treats
each as **Humans**. The scenario screen therefore lights that player's Humans
cell: on Europe the original shows eight races (Humans + the seven the map
stores), while a reader that mapped a missing tag to 0xff dropped the Humans
cell and showed only seven. `aow::Player::race` now defaults to 0, and
`parse_player` applies `TPlayer.ReadWrite`'s tail (`index == 0` -> type 4) -- so
the lit-race set is the roster's races *with the stream default applied*, for
every map, not a per-map special case.

### TPlayerMagicControl.ReadWrite tags (`0x5577c748`)

| tag | off | type | field |
|-----|-----|------|-------|
| `0x14` | `+0x24` | `rwInteger` | **mana** |
| `0x15` | `+0x2c` | `rwInteger` | **% of power to research** (Create default 50) |
| `0x16` | `+0x30` | list | researched-spell `TIntegerList` |
| `0x17` | `+0x34` | `rwInteger` | current research spell |
| `0x18` | `+0x38` | `rwInteger` | research progress |
| `0x19` | `+0x3c` | `rwEObject` | `TEnchantmentList` |
| `0x1a` | `+0x44` | `rwByte` | `unknown` |

`GetPower` (`0x5577ca00`) sums each power source's `Income` (`+0x50`);
`GetNetPower` (`0x5577cec4`) halves that when a global map flag is set;
`GetNetPowerToResearch` (`0x5577cf80`) is `round(net_power * power_to_research /
100)`; `GetNetPowerToMana` (`0x5577cee4`) is `net_power - research` (the AI
difficulty types 2/3/6/0xa round it). `GetManaIncome` is an alias of
`GetNetPowerToMana`.

## The troops

A player's troops are the armies the scenario places on the map
(`AoWE.TArmyHS`, class `0x20217`) that belong to them. An army's owner is
**`AoWE.TArmy` rw tag 4** at `+0x12` (`TArmy.GetPlayer` `0x5578d504` returns
`+0x12`; the stream omits a zero, so a **missing tag = owner 0**, the
independents). `src/engine/map.cpp`'s `parse_army` now reads that tag into
`MapHs::owner`; `load_game_state` tallies `Player::armies` / `Player::units`.

`TPlayer.GetUnitCount` (`0x55751804`) instead sums the sizes of the player's
runtime army list (`+0x28`); the port derives the same count from the placed
armies, since it does not yet build the runtime army list. Verified: Tutorial.hsm
= 4 armies / 15 units, owners `[0,2,1,2]`; Dwarf1.CSM = 53 armies / 154 units.

## The turn/day loop

The map keeps the global **day counter** and the **turn order**:

* day counter at `TAoWHSMap +0x174` (`NewDay` increments it; `NewTurn` skips the
  gold step when it is 1);
* turn limit at `TAoWHSMap +0x158`, turn-order mode at `+0x16c`, the order
  itself (a `TByteList`) at `+0x170`, current seated player at `+0xa5`.

`GenerateNewTurnOrder` (`0x557782fc`) builds the order as **[0]** (the
independents) followed by a random permutation of **1..N-1** (drawn one at a
time). `NewDay` (`0x55754da4`) increments the day, resets the position, and
regenerates the order only when the mode is 2; when the day reaches the turn
limit every player is put out.

`NewTurn` (`0x557553c4`) - one player's turn start:

```
if (day != 1) {
    gold = gold + income - upkeep     // upkeep = army(+0xc8) + magic(+0xcc)
    if (gold < 0) { gold = 0; food_penalty -= 5; if (food_penalty < -50) food_penalty = -50; }
    else          { food_penalty = 0; }
    set_end_turn(player, false)
}
turn_count += 1
```

`TAoWHSMap.StartNewMap` (`0x55777d68`) seeds the live treasury before the first
turn: `gold(+0xc4) = start_gold(+0xac)`, plus +250 for CPU King (type 6) and
+500 for CPU Emperor (type 7). (It also seeds mana from the magic control's
own starting-mana field with the same +250/+500 AI bonuses.)

`TPlayer.UpdateIncome` (`0x55751f88`) sets `income = base_income + Σ(income
source)`, then applies the CPU multiplier - the constants at AoWEPACK
`0x5575205c`/`60`/`64` are 1.0 / 2.0 / 2.5, and the Emperor branch is
`x*3 + 100`:

| player type | multiplier | flat bonus |
|---|---|---|
| 2 CPU Knight | `round(x * 1.0)` | - |
| 3 CPU Lord | `round(x * 2.0)` | - |
| 6 CPU King | `round(x * 2.5)` | +50 |
| 7 CPU Emperor | `round(x * 3.0)` | +100 |

`System.ROUND` is Delphi's round-to-nearest-even. The port reproduces all four
branches and the start-gold bonuses. The full **building income + army upkeep**
chain is recovered in `docs/28_ECONOMY.md` and implemented by
`aow::compute_economy`.

`TPlayerMagicControl.NewTurn` (`0x5577cc2c`) grows the research list, adds
`net_power_to_research` to research progress (granting a spell when it reaches
0), then `mana = mana + mana_income - mana_upkeep`, clamped at 0 (`OutOfMana`).
It is reached through `TPlayerMagicControl.MsgProc@5577d264`, which runs it for
message `0x20020002`; `TPlayerControl.NewTurn` posts that message outside its
`day != 1` gold branch, so mana income is credited on day 1 too. The port
reproduces that step in `GameState::begin_player_turn`
(`magic.mana += magic.mana_income - magic.mana_upkeep`, clamp 0) and exposes the
fields as `Player::magic` / `PlayerMagic::net_power_to_research`; research
progress itself is parsed but not simulated. See `docs/28_ECONOMY.md` 3.2.

### The end-turn rule (`TPlayer+0x2c`)

A turn may pass only once the player on turn has **ended** it:

* `TPlayer.SetEndTurn` (`0x55752d00`) stores `+0x2c` and fires
  `TAoWHSMap.TriggerEndTurnEvent` (`0x55776474`);
* `TAoWHSMap.EndTurn` (`0x5577838c`) refuses with no seated player, and while the
  execute-event log is running with entries; otherwise it posts the end-turn
  token (`FUN_5574757c`), whose execution calls `TPlayerControl.EndTurn`
  (`0x5575530c`, `SetEndTurn(player, 1)` for the seated player) and then
  `TTurnPlayerControl.EndTurn` (`0x55756db8`);
* `TTurnPlayerControl.EndTurn` hands the turn on **only** when the player who
  ended is the one on turn (`map+0xa4`): it walks the turn order from there,
  skipping seats whose `+0x45 != 1` (started *and* ended in the same pass), and
  calls `TAoWHSMap.NewDay` when the order wraps - so a day only advances when
  every player has ended a turn;
* `TAoWHSMap.CancelEndTurn` (`0x557783c8`) + the cancel branch of
  `TEndTurnTE.Execute` (`0x557473c4`) withdraw the flag again.

The port models this with `Player::end_turn` (`+0x2c`), `Player::playing`
(`+0x45`), `Player::busy` (`TPlayer.GetBusy`) and `GameState::event_log_busy` /
`event_log_count` (the `0x5577838c` gate). `GameState::advance_turn` is the loop
above; `GameState::end_turn(who)` / `cancel_end_turn(who)` are the map routines.
The bar's TurnButton is what drives them - see **`docs/22_TURN_BUTTONS.md`**.

## The C++ reconstruction

`src/engine/game_state.cpp`:

* `load_game_state(map, gs, err)` - parses `map.region` tag 0x15 into
  `GameState::players` (class id `0x20020` records) and tallies `armies`/`units`
  per player from `map.levels`.
* `GameState::begin_day` / `new_day` / `begin_player_turn` / `advance_turn` and
  `generate_turn_order` - the loop above.
* `apply_setup_player_types(gs, race_type, race_count)` - the scenario setup's
  race -> player-type pass (see below).

> The shuffle uses a small deterministic xorshift32 seeded by the caller; the
> original draws from the map's own RNG (`TAoWHSMap.Random`), whose seed the
> port does not model.

## Which race you play: the scenario setup (recovered 2024 pass)

A scenario's races are **fixed by the map** - the setup screen never changes a
player's race, only which of the map's races is the local **Human**. That choice
is the "race <-> units" link: it decides whose cities and armies you command.
The port used to ignore it and always play the map's own default Human (so
marking Elves as Human on the Scenario screen still handed you the Orc player's
units). The recovery, from `AoWEPACK.dpl`:

| method | address | what it does |
|--------|---------|--------------|
| `TSetupControl.hPickMap` | `0x557e20c8` | one `TSetupSettings` per map player: race `+0x18`, player index `+0x20`, **map default type `+0x10`**, **current type `+0x8`**. Its `local_5` flag starts set, so the first type-0 (Human) slot keeps Human and every later one is forced to `1` (CPU Squire). |
| AoW.exe `RacePlayerCBChange` | `0x00412a24` | a race cell's combo: `PlayerNameToPlayerID` -> `TSetupSettings.IndexOfRace(cell+0xc)` -> `hSetPlayerID`. |
| `TSetupControl.hSetPlayerID` | `0x557e1eb4` | stores the new current type; when it is Human it first resets every other current Human. |
| `TSetupControl.ResetPlayerSettings` | `0x557e1378` | puts a slot back to its map default, **except a default-human slot, which becomes `pidCPU1` = `1`**. |
| `TSetupControl.SetupMap` | `0x557e0b64` | last pass: `player[+0xa7] = settings[+8]` for each slot, then the map's turn/limitation fields, then `SetupPlayerControl`. |

So the port's `aow::apply_setup_player_types` (game_state.cpp) writes the
screen's per-race types onto the loaded roster and keeps only the first Human
(any later Human -> AI1), returning the human's roster index; `Scene::
scen_choose_type` (menu.cpp) mirrors `hSetPlayerID`/`ResetPlayerSettings` when
the drop-down changes a race, and `Scene::start_game` calls the apply pass right
after loading the map (the port's `SetupMap`) so `mv_human` and the leader
banner's race follow the chosen race.

```powershell
# head-less: mark race 9 (Orc) as Human on Heartwood Forest, then Start!
./bin/AoW.exe --root .. --start-map "Heartwood Forest.HSM" `
    --scen-human 9 --render out.png
#   scenario: Heartwood Forest.HSM roster [Elves=Human] [Halflings=CPU Squire] ...
#   game_human=4 race=Orcs
```

## Demo + tests

```
aow gamestate Scenario/Custom/Tutorial.hsm --turns 3
```

```
  day=1  turn_limit=0  turn_order_mode=0  players=2
  id  name           race  gold    income  upkeep  gems   mana   armies units
  0   Independents   255   250     0       0       0      0      1      4
  1   Player 1       5     1000    0       0       0      0      1      4
  2   Player 2       10    250     0       0       0      0      2      7
```

`aow_game_state_tests <game-root>` (82 checks) locks the roster of Tutorial.hsm,
Thinreed Lake, Dwarf1.CSM and two custom maps (counts, indices, races, **player
types**, gold), each seat's **leader** (the tag-0x1e record's face index, name
and nickname - what the Events banner shows), the troop tally,
the setup race -> player-type pass (only the first Human survives), the
turn-order construction, the day/round arithmetic, the income/upkeep
accounting and the deficit clamp. Run via `./build.ps1 -Test` or
`ctest -R game_state`.

## Open items (`TODO(RE)`)

* `TRaceList` (`TAoWHSMap` tag `0x16`) per-record tags - the race list is not
  parsed yet.
* `TPlayer` tag 0x0a; the remaining `TLeader` (`0x1e`) sub-record fields (the
  face index, hero-resource index, name, nickname and sphere picks - rw tag
  0x2e - are read; skill points, personality, ... are still unparsed).
* The magic-control researched-spell `TIntegerList` (tag 0x16) body.
* City enchantment state (`TCity+0x58`, rw tag 0x1d) - the Gold Rush flag
  (`TCity+0x5c` bit 1) is not parsed, so a scenario that starts with Gold Rush
  already cast is not doubled yet. The base income / merchandise rules are
  implemented in `docs/28_ECONOMY.md`.
* The map's own day value: `Map::day` parses TAoWHSMap rw tag 0x13 (absent = 0
  on a fresh scenario), but the turn loop still starts at day 1. The map's RNG
  state (`Map::rand_seed`, rw tag 0x1c) *is* parsed and drives the gold-pile
  generation with Borland's `System._RandInt` (`docs/28_ECONOMY.md` 7.2); the
  turn-order shuffle still uses the port's xorshift with the caller's seed.