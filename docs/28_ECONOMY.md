# 28 - Gold income and upkeep (the per-turn economy)

This document records the 1:1 recovery of AoW1's **gold economy**: which
buildings pay gold every turn, how much, how the city/farm crop ownership is
decided, and what a player pays in unit upkeep. The implementation is
`aow::compute_economy` + `GameState::update_income` + `GameState::start_new_map`
in `src/engine/game_state.cpp` / `src/include/aow/game_state.hpp`; the tests are
in `src/tests/game_state_test.cpp`.

Everything was read from `AoWEPACK.dpl` (image base `0x55700000`) with
GhidraMCP. The decompiles used here are under `re/income/`.

## 1. The pipeline

```
TAoWHSMap.StartNewMap@55777d68
    gold(+0xc4) = start_gold(+0xac)              // SetGems is the setter
    CPU King  (type 6): gold += 250
    CPU Emperor (type 7): gold += 500
    (and mana = magic.starting_mana, with the same AI bonuses)

player turn (day != 1), TPlayerControl.NewTurn@557553c4:
    gold = gold + income - upkeep                // upkeep = army(+0xc8) + magic(+0xcc)
    deficit: gold = 0, food_penalty -= 5 (floor -50); otherwise food_penalty = 0

TPlayer.UpdateIncome@55751f88:
    income(+0xd0) = base_income(+0xb0) + Σ income_source.Income()
    then the CPU multiplier (section 1 and docs/14_GAME_STATE.md)
```

`TPlayer.GetUpkeep@557520c4` is exactly `army_upkeep(+0xc8) + magic_upkeep(+0xcc)`.
The live gold field is `+0xc4`; the scenario's `TPlayer` rw **tag 0x5** (`+0xac`)
is the *starting* gold. The historical setter name `TPlayer.SetGems` is
misleading: AoW1 has no separate crystal currency, and `Gems.TGems.ArmyEntered`
adds gold to `+0xc4`.

## 2. Income sources: one `TIncomeSource` per owned structure

When a `TPlayerStructure` is activated it registers its income source with the
owner (`TPlayer.RegisterIncomeSource@55752068`):

| structure | source registration | `Income()` |
|---|---|---|
| `Crops.TPlayerCropStructure` (city, farm) | `Activate@557a62e4` registers `+0x40` | `TPlayerCropStructure.GetIncome@557a6100` = the cached `+0x3c` byte |
| `Mine.TMine` | `Activate@557b45d8` registers `+0x40` | `TMine.GetIncome@557b43e4` = flat **10** |
| `PrdPlace.TProductionPlace` | `Activate@557bee9c` registers `+0x10` when its `GetMagicSphere` is 8 | `TProductionPlace.GetIncome@557be500` |

A `TProductionPlace` whose `GetMagicSphere` is not 8 (the six magic nodes and
`RandomNode`) instead creates a `TPlayerStructurePowerSource`; it produces
**mana**, not gold. `Shipyard.TShipyard.GetBaseIncome@557c75d0` overrides the
base to **0**. `BuildersGuild` inherits the flat 10.

`TPlayerStructureIncomeSource.Income@55761af8` simply calls the structure's
vtable slot `+0x1f4`, which is the subclass's `GetIncome`. The base
`TPlayerStructure.GetIncome@55760f3c` returns 0.

### 2.1 City income - `TCity.UpdateIncome@557abb94`

```
if (GetRazed() || GetActiveAction() == 4 /* migrate */ || GetBuilding())
    income = 0
else {
    size  = TCity.GetSize()            // TCityResource+0x78, see 2.2
    crops = GetCropCount()             // see 2.3
    income = size*5 + crops*2
    if (GetActiveAction() < 2)         // 0 idle or 1 produce merchandise
        income += round(income * 0.25) // float 0.25 at 0x557abc2c
    if (city+0x5c & 2)                 // Gold Rush city enchantment
        income *= 2
}
```

* 25% is **produce merchandise**, the default city action. It is *not* paid
  while producing a unit/installing/upgrading/fortifying/looting/razing.
* `GetActiveAction@557a96a8` only looks at the queue when the production
  control's `+0x20` state byte is 2. It maps the first queue item's production
  id through `City.ProductionIDToCityAction@557a7ce4`:
  `1 -> 8` (produce unit), `2 -> 7` (install), `10 -> 1` (merchandise),
  `11 -> 2` (upgrade), `12 -> 3` (fortify), `13 -> 4` (migrate),
  `14 -> 5` (loot).
* The wiki's "Total Income" table (5/10/15/20 base plus 2 per crop field) is
  the base+crop part **before** the 25% merchandise bonus.
* `System.ROUND` (AoWEPACK `0x55701048`) rounds halves to even.

### 2.2 City size - `TCityResource+0x78`

`TCity.GetSize@557aa938` returns `resource+0x78`. `TCityResource.ActivateMsg@557adee8`
builds that byte as the number of `TTerrainList` entries whose terrain byte is
>= 0. On the shipped `Release.hss` resources:

| resource id | library | valid terrain entries | size |
|---:|---|---:|---:|
| 22 | `CITY1.ILB` | 1 | 1 |
| 57 | `CITY2.ILB` | 2 | 2 |
| 58 | `CITY3.ILB` | 3 | 3 |
| 59 | `CITY4.ILB` | 4 | 4 |
| 61..64 | the second (terrain) block | 1..4 | 1..4 |

The city's rw **tag 0x10** (`+0x4d`) is the **upgrade level**, not the size.

### 2.3 Crop ownership and `GetCropCount@557a6480`

A city or farm resource has a `TTerrainList` (`MapResource::terrain_list`). Its
valid entries are the structure's own hexes. `TTerrainList.CreateSurroundingHNTable@556051e8`
turns those into the set of adjacent HN indices that are **outside** the
footprint; `TPlayerCropStructure.GetCropCount` then walks that table and counts
the `Crops.TCrop` objects whose owner pointer is this structure.

The engine rebuilds the owner pointers on activation:

1. `TCrop.Activate@557a5e2c` -> `LinkToCropOwner@557a5dac` links the crop to a
   `TPlayerCropStructure` in the adjacent ring.
2. `TPlayerCropStructure.Activate`/`LinkToCrops@557a686c` walks the structure's
   own surrounding-HN table and overwrites every crop there to point at the
   structure.

The port reproduces both halves: it builds the surrounding-HN table from the
resource, iterates structures in map order (the engine's activation order), and
the last structure claims an overlapping crop. `hn_walk` is the same spiral as
`HSEngine.InitHNWalk@5560e5c8` / `WalkToNextHN@5560e610`; `hn_neighbour` is
`HNtoHX` folded by the column parity of `CenterHNtoHX@5560e17c`.

### 2.4 Farm income - `TPlayerCropStructure.UpdateIncome@557a6230`

```
if (!GetRazed() && !GetBuilding() && CropsEnabled())
    income = GetCropCount() * 2
else
    income = 0
```

`Farm.TFarm.CropsEnabled@557b2c88` additionally rejects terrain 5.
`Farm.TFarm.ReadWrite@557b2c64` stores its razed bit in rw tag 0x14 (+0x44 bit
0); `Mine.TMine.ReadWrite@557b4690` uses rw tag 0x14 (+0x3c bit 0); a
`TProductionPlace` stores 1 at rw tag 0x0a (+0x48) when razed. The map parser
exposes these as `MapHs::structure_status` / `MapHs::razed`.

### 2.5 Production-place income - `TProductionPlace.GetIncome@557be500`

```
base = GetBaseIncome()             // 10, or 0 for Shipyard / razed / building
if (GetMagicSphere() == 8) {       // a gold place, not a magic node
    if (production state == 2 && first queue item != 10 /* merchandise */)
        return base;               // no 25% while producing something else
    return base + round(base * 0.25);
}
```

## 3. Unit upkeep - `TArmy.UpdateUpkeep@5578cc08`

Armies maintain their own cached `TArmy+0x18` byte and adjust the owner's
`+0xc8` by the delta with `TPlayer.SetArmyUpkeep@557520d4`. The per-unit
`GetUpkeep` (vtable slot `+0x9c`) is:

| unit | upkeep | evidence |
|---|---|---|
| `TLeader` | 0 | `TLeader.GetUpkeep@5578b758` |
| `THero` | `5 + 2 * experience_level` | `THero.GetUpkeep@557872c0` |
| `TUnit` | `(resource level + 1) * 2`; **0** when ability `0x9b` is present | `TAbstractUnit.GetUpkeep@5577fc64`, `TUnit.GetUnitLevel@55782b8c` |

The hero experience level is `THero.ExperienceToLevel@557876d4` over the
threshold table `0x558e84b8` (`15, 15, 20, 20, 25, 25` per five-level band,
max 30). A plain `TUnit`'s level lives in the `Unitres.pfs` record (+0x2f), so
the port looks the unit id up in `UnitCatalog`; the map itself does not store
it. The ability bitmap is LSB-first (`GetAbSet@5574e0e0`), ability `0x9b` is
bit 155 (byte 19, bit 3).

The leader has no upkeep, which is why a starting army can cost less than its
visible unit count suggests.

## 3.1 Mana income (the bar's other read-out)

The bar's right panel is the same shape for magic, so the port also fills it:

* `TPlayerMagicControl.ReadWrite@5577c748` rw **tag 0x1b** -> `+0x18` is the
  scenario's starting mana. `TAoWHSMap.StartNewMap` seeds the live mana
  (`+0x24`) from it and gives CPU King/Emperor `+250`/`+500`.
* `TLeader.ReadWrite@5578b470` rw **tag 0x2e** -> `+0xa0` is the leader's sphere
  pick list (one wrapper byte, u32 count, then the sphere ids).
  `TLeader.GetSpherePicks@5578b17c` counts the picks in one sphere.
* `TPlayerMagicControl.GetPower@5577ca00` sums every registered power source:
  `PowerNode.GetPower@557d03c8` is flat 10; a magic node's
  `TProductionPlace.GetPower@557be468` is `picks*5 + 10` for the owner's picks
  in that node's sphere, and 0 when the owner has no picks. Gold production
  places (sphere 8) give no power.
* `GetManaIncome@5577c95c` = `GetNetPowerToMana@5577cee4`:
  `net_power - round(net_power * power_to_research / 100)`, then the CPU
  multipliers 1.25 / 1.5 / 1.75 / 2.0 (CPU Knight/Lord/King/Emperor).

The sphere ids are Life 1, Death 2, Earth 3, Air 4, Fire 5, Water 6. The
scenario maps store the picks and starting mana directly, e.g. Tutorial's
Human leader has `[1,5,1]` (two Life, one Fire) and 100 starting mana.

### 3.2 When the mana is credited

`TPlayerControl.NewTurn@557553c4` posts its turn-start message `0x20020002`
(with the player index in `+0x18`) **outside** the `day != 1` gold branch, and
`TPlayerMagicControl.MsgProc@5577d264` answers it with
`TPlayerMagicControl.NewTurn@5577cc2c` when that index matches. The routine
ends with:

```
mana = mana + GetManaIncome() - GetManaUpkeep()
if mana < 0: OutOfMana(-mana); mana = 0
```

So - unlike the gold step - **mana income is credited on day 1 too**, and a
negative balance is clamped at 0 (`OutOfMana`). `GetManaUpkeep@5577c964` is the
sum of every active enchantment's `+0x10` plus
`TGlobalMagicControl.GetEnchantmentUpkeep` for the player; the port does not
parse enchantment lists yet, so `PlayerMagic::mana_upkeep` stays 0.

The bar's green `+N` is `GetManaIncome`, i.e. exactly the number now credited.
(The research share is a separate sink in the original: with no research
selected, `NewTurn` also adds `GetNetPowerToResearch` to mana. The port has no
research UI yet and credits the displayed `mana_income` only.)

## 4. What the port exposes

* `Player::start_gold` - scenario rw tag 0x5 (+0xac).
* `Player::gold` - live treasury rw tag 0x10 (+0xc4), seeded by `start_new_map`.
* `Player::building_income` - Σ owned structures' `Income()`.
* `Player::income` - `update_income` result (base + buildings, CPU multiplier).
* `Player::army_upkeep` / `magic_upkeep` / `upkeep()`.
* `Player::magic.starting_mana`, `magic.power_income`, `magic.mana_income`
  (the right-hand bar read-out) and `magic.mana_upkeep`, plus
  `Leader::sphere_picks`.
* `aow::load_game_state(map, resources, units, gs, error)` evaluates the map
  economy; the short overload `load_game_state(map, gs, error)` keeps the light
  roster-only path.
* `aow::compute_economy(map, resources, units, gs)` can be called again after a
  capture/razing/building change.

The port's `begin_player_turn` calls `update_income` before crediting
`gold + income - upkeep`, exactly the `TPlayerControl.NewTurn` arithmetic, and
then credits `magic.mana += magic.mana_income - magic.mana_upkeep` (clamped at
0) for the `TPlayerMagicControl.NewTurn` step - on day 1 as well.

The bar's green number is the **net** value, not the gross income:
`Scene::draw_readout` is handed `Player::income - Player::upkeep()` on the gold
side (and the already-net `magic.mana_income` on the mana side). A negative net -
"only costs, no gains" - is drawn as red `-N` with Delphi's `clRed`; a
non-negative value is green `+N` (`0x77FF77`).

## 5. Tests

`aow_game_state_tests <game-root>` (111 checks) includes the Tutorial economy:
independents 39 gold/turn (a size-3 city with 4 crops = 29 plus a mine = 10),
Borok 14 building gold -> 28 after the CPU Lord x2 multiplier, and army upkeeps
28 / 22 / 41 for the three seats. It also locks the leader sphere picks
(`[1,5,1]`), starting mana (100 / 50), the independents' power-node 10 -> mana
income 5, and the AI multiplier/start-gold branches.

## 6. Known limits

* Gold Rush (city enchantment, doubles income) is not parsed yet - the
  scenario maps used by the port start without city enchantments.
* Unit/global enchantments contribute to `PlayerMagic::mana_upkeep`, not the
  gold `magic_upkeep` field; both stay 0 until enchantment lists are parsed.
* The research share of the power (`net_power_to_research`) is not simulated:
  the port credits the displayed `mana_income` every turn, while the original
  routes that share into research progress (and spills the remainder into mana
  when research completes).
* Crop growth/trampling and building/razing during play are not simulated; the
  map's serialized crops and structure states are the economy snapshot.

## 7. Gold piles - `Gems.TGems` pickups

A gold pile on the world map is `Gems.TGems` (class **0x203ae**,
`Gems.TGemsResource` 0x203af). It is an `ILTer.TILTerrainMO`, **not** a
`TPlayerStructure`: it never flies an owner flag, it cannot be owned, and it is
collected by walking onto its hex. The implementation in the port is
`aow::generate_gems_amounts` / `aow::generate_gems_amount` /
`aow::gems_army_entered` (`src/engine/game_state.cpp`) with the map fields
`MapHs::gems_gold` / `MapHs::gems_size` (`src/include/aow/map_types.hpp`,
parsed in `src/engine/map.cpp`), and the UI hook
`Scene::game_gems_army_update` in `src/game/menu.cpp`. The tests are the
`gems:` set in `src/tests/game_state_test.cpp`.

Everything below is from AoWEPACK.dpl (image base `0x55700000`) via GhidraMCP;
the decompiles are `re/income/Gems_*.c` and the fresh dumps used for this
section.

### 7.1 The object and its two fields

```
Gems.TGems.Create@557c4f9c            +0x20 (size class) := 2
                                      +0x1c (amount)     := 0
Gems.TGems.ReadWrite@557c4fdc         base TMultiHexMO.ReadWrite
                                      rwInteger(+0x1c, tag 5)   the amount
                                      rwData(+0x20, 1, tag 6)   the size class
```

Shipped scenarios store only **tag 6** (the size class): Barondir Bay and
Valley Elf have five / thirteen piles of size 2, Heartwood Forest all sizes
1..3, and none writes tag 5. The amount is generated at game start.

### 7.2 Generating the amount - `TGems.GenerateGemsAmount@557c5010`

```
if size == 4 then                       // the editor's "random"
    size := TAoWHSMap.Random(3) + 1     // 1..3, stored back into +0x20
case size of
  1: amount := TAoWHSMap.Random(25) + 25     //  25.. 49
  2: amount := TAoWHSMap.Random(25) + 50     //  50.. 74
  3: amount := TAoWHSMap.Random(50) + 100    // 100..149
otherwise: the amount is left untouched
```

It runs when the map receives message **0x20020001** and its day counter
(`map+0x174`) is **1** (`Gems.TGems.MsgProc@557c50c4`). That message is sent by
`TPlayerControl.NewDay@55754da4` after the day becomes 1 - i.e. the first day
of a fresh scenario - and reaches every level/tile/object in container order.
A saved game (day > 1) does not regenerate.

`TAoWHSMap.Random@5577827c` swaps the map's own RNG state (`map+0x230`, rw
**tag 0x1c**) into Delphi's `RandSeed` around `System.Random(max)`. The shipped
maps do carry the seed (Tutorial `0x18e2d3b6`, Barondir Bay `0x9b75d3e0`), and
Borland's `System._RandInt` is:

```
RandSeed := RandSeed * $08088405 + 1
result   := high dword(RandSeed * range)
```

so the port replays the exact sequence (`aow::delphi_random`) and reproduces
the original amounts, not just their ranges. The map's day tag (rw 0x13) is
absent from a fresh scenario (`rwInteger` then stores 0); the port parses both
tags but keeps its turn loop starting at day 1 (docs/14).

### 7.3 Collecting it - `TGems.ArmyEntered@557c50f8`

`TArmyHS.PlaceOnMap@557909f4` sends the map field message **0x1102**
(ArmyPlaced). `Gems.TGems.MapFieldMsgProc@557c52a0` answers it with
`ArmyEntered(map_object, army_hs)`:

```
owner := army_hs+0x1c (TArmy) + 0x12
if owner == 0 then return                      // the independents never collect

if (map+0xa5 == owner) or TAoWHSMap.Watching(hex, radius 0) then
    Sound.PlaySample('SFX\EVENTS\GOLD.WAV')

player := TPlayerList.GetPlayers(map+0x140, owner)
TPlayer.SetGems(player, player+0xc4 + self+0x1c)

event := TLocationTextEventLog.Create
event.player := owner
event.text   := Format(AoWE.GoldFoundRStr, [IntToStr(self+0x1c)])
event.location := (self.x, self.y, self.level)
player+0xd8.AddEvent(event); event.Execute

self.Destroy                                 // class reference's -4 slot (vmtDestroy)
```

* `TPlayer.SetGems@55751f38` writes `+0xc4` (the live gold treasury - AoW1 has
  no separate "gems" currency), fires the player's GemsChanged notify event and
  checks the **gold victory** condition (`map+0x14d == 2` and
  `map+0x154 <= gold` -> `TAoWHSMap.GameOver(player, 1)`). The port has no
  victory conditions yet.
* `AoWE.GoldFoundRStr` is `"%s gold pieces found"` (the English string in
  `Dict\ResStr.mld`), so a 60-gold pile logs "60 gold pieces found".
* `TAoWHSMap.Watching@55775b88` at the radius `ArmyEntered` passes (0) is the
  seated player's view of that single hex: `TMapField+0x1a`/`map+0xc8`,
  `GetExplored(field, map+0xa5)` with `map+0xc9`, and `THSMap.Viewing`. The
  port plays the sound when the collecting army is the seated player's or the
  hex carries the seated player's explored bit.
* The pile is destroyed **after** paying out, even when the amount is 0.

### 7.4 What the port does

* `Map::rand_seed` / `Map::day` - TAoWHSMap rw tags 0x1c / 0x13.
* `MapHs::gems_gold` / `gems_size` - TGems rw tags 5 / 6.
* `aow::generate_gems_amounts(map)` runs in `Scene::mv_load` right after
  `load_game_state` (a scenario start is the first NewDay; the map is only
  loaded from a fresh `.HSM`/`.CSM`, as the port has no save loader). It walks
  level / tile / placement order, leaves the advanced seed on `Map::rand_seed`
  and returns how many piles it filled.
* `Scene::game_gems_army_update(x, y)` is called from `mv_finish_arrival` (a
  plain walk) and `mv_finish_merge` (a merge's arriving party), both at the
  moment `TMoveArmyTE.FinishMove` runs - the same instant the original sends
  0x1102. It adds the gold, prints `game_gold=<x>,<y> player=<p>
  amount=<n> total=<n> gold=<treasury>`, shows "%n gold pieces found" in the
  event strip for the seated player, plays `SFX\EVENTS\GOLD.WAV`, and removes
  the object (the caller re-renders the level).
* Head-less proof: `--game-gold X Y` runs the same handler without scripting a
  walk (`game_gold_hook=<x>,<y> ok=<0|1>`).

Verified live on Heartwood Forest (human = Elves, `--scen-human 4`): the leader
at (27,28) walks to the pile at (29,23), the log shows
`game_gold=29,23 player=1 amount=60 total=60 gold=310` (250 -> 310), and a
second 0x1102 on the same hex answers `game_gold_hook=29,23 ok=0` - the pile is
gone. Barondir Bay's five size-2 piles generate `60, 67, 52, 51, 65` from its
own seed, which is what the unit tests lock.

### 7.5 Remaining differences

* The original shows the pickup in the **event logbook** (a
  `TLocationTextEventLog` with a location). The port has no logbook window, so
  it uses the status line the capture path already uses.
* Gold-victory (`SetGems`'s GameOver check) is not implemented.
* The generation pass runs at map load; the original runs it on the first
  NewDay. All 31 shipped `.HSM` scenarios use `turn_order_mode` 0, which draws
  nothing before that message, so the RNG position (and the amounts) is
  identical. A mode 1/2 map would regenerate its turn order first, and the
  port's turn-order shuffle still uses its own xorshift (docs/14, known
  limits).
* `TGems.UpdateAITarget@557c508c` (the pile's AI target priority is
  `amount/5 + 15`) is not used: the port does not move AI armies.
* Only the human seat moves in the port, so only its armies can collect a
  pile; the rule itself (and the independents' refusal) is in
  `gems_army_entered` and is player-generic.

