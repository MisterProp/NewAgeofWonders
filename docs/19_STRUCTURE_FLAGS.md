# 19 - The owner flag of a "takeable" (ownable) structure

This document records the reverse-engineering of **the flag a player-owned
world-map building flies** - which buildings have one, whose flag it is, where
the pole and the flag are anchored, and how the 12-frame flag animates - and its
1:1 reconstruction in `src/engine/map_resources.cpp`
(`is_player_structure_resource`, `structure_pad_index`,
`player_structure_flag_anchor`, `structure_flag_step`), `src/engine/map.cpp`
(`is_player_structure_class`, the `structure_owner` / `city_level` /
`city_flags` tags), `src/engine/mapviz.cpp` (`StructureFlag`,
`resolve_structure_flag`, `structure_flag_sprites` and the two blit sites) and
the tests in `src/tests/mapres_test.cpp`.

Everything was recovered with **GhidraMCP** (`tools/ghidra/`) from
`AoWEPACK.dpl` (image base `0x55700000`); the decompiles live under
`re/exports/flags*/`. The class hierarchy came out of the module's own Delphi
VMTs - `tools/vmt_scan.py` walks them:

```
python tools/vmt_scan.py AoWEPACK.dpl --children TPlayerStructure
python tools/vmt_scan.py AoWEPACK.dpl TMine TCity TTower
```

## 1. Which buildings are ownable

The *object class* decides: only an `AoWE.TPlayerStructure` subclass draws a
flag, because only those inherit `AoWE.TPlayerStructure.Show@55760f94`. The
subtree (VMT-walked; ClassIDs from each class's decompiled `ClassID` getter):

```
AoWE.TPlayerStructure              0x2034c   AoWE.TStructure 0x2034a
  Crops.TPlayerCropStructure       0x2034e
    City.TCity                     0x20108
    Farm.TFarm                     0x2036e
  PrdPlace.TProductionPlace        0x203ba
    Shipyard.TShipyard             0x204be     BldGuild.TBuildersGuild 0x2092e
    WaterNode 0x204b3  DeathNode 0x204ae  EarthNode 0x204ce
    FireNode  0x203de  LifeNode  0x20371  AirNode   0x2094e
    RandomNode.TRandomNode         0x20822
  Mine.TMine                       0x2037a
  Tower.TTower                     0x2043e
  PowerNode.TPowerNode             0x2019a
  ReflectingPool.TReflectingPool   0x2063e
  Altar.TAltar                     0x203f1
    AltLife 0x20233  AltDeath 0x20235  AltFire 0x2023a
    AltLight 0x2023d  AltIce 0x2024b  AltPest 0x2030c
```

Everything else that *looks* like a structure is a plain `AoWE.TStructure`
(Wizard's Tower `0x2089e`, Cave, Ruin, Pyramid, Crypt, Dungeon, Monsterlair,
Arena, MagicSanctuary, Sign, Teleport, Spring of Life, Blood Shrine,
Holy/Unholy Ground) or an `ILTer.TILTerrainMO` (Crops, Gems, Mana Crystals) and
**never** draws a flag: `0x2089f TWizardsTowerResource`, `0x2037f
TCaveResource` and `0x203af TGemsResource` all sit outside the subtree (the
Wizard's Tower is an underground *entrance*, not a possession).

The resource side mirrors it: `AoWE.TPlayerStructureResource` `0x2034d` and its
subclasses (`kPsrClasses` in `src/engine/map_resources.cpp`), reached by
`TMapObjectResource.ResourceUserClassID` - so `MapResource::is_player_structure`
is set from the resource class id alone.

## 2. Whose flag is it: the flag byte

`AoWE.TPlayerStructure.UpdateFlagID@55760ed8` (and
`City.TCity.UpdateFlagID@557ac8b0`) turn the owner into one byte the draw reads:

```c
// TPlayerStructure.UpdateFlagID
if ((this+0xc & 4) == 0) {                 // a runtime-only status byte
    if (this+0x30 < 1) {                   // nobody owns it
        if (resource+0x64 == 0) { this+0x32 = 0x3f; return; }  // 0x3f = no flag
        this+0x32 = 0x20;                  // the neutral flag
    } else {
        this+0x32 = player(this+0x30)+0xa5;   // the *owner's race id*
    }
} else this+0x32 = 0x3f;

// TCity.UpdateFlagID
if ((this+0xc & 4) == 0 &&
    (((this+0x44 & 2) == 0 || GetBuilding(this)) && (this+0x44 & 1) == 0)) {
    if (owner > 0) { this+0x54 = race + (this+0x4d - 1) * 0x40; return; }
    this+0x54 = 0x20 + (this+0x4d - 1) * 0x40;   // 0x20 = neutral
    return;
}
this+0x54 = 0x3f;
```

Reading it out:

* **The flag is the owner's *race* flag**, not a per-player colour:
  `player+0xa5` is the race id (the byte `TPlayer.ReadWrite` stores at rw tag 2
  and `RaceNames` is indexed with), and the draw hands `flagbyte & 0x1f` to
  `TRaceResourceList.GetRaceResource` to pick the `RACES\R*.ILB` library. A
  Goblin city flies the Goblin flag.
* **0x20 = the neutral flag** (`Flags.ILB`), drawn only by a resource whose
  `+0x64` is set. `TStructureResource.Create@5576046c` sets `+0x44 = 1`,
  `TPlayerStructureResource.Create@55761974` sets `+0x64 = 1`, and every
  subclass *except* `City.TCityResource` clears `+0x64` again (via
  `TProductionPlaceResource.Create@557bf400` or its own `Create`). So an unowned
  **city** keeps a flag (the grey neutral one) while an unowned mine, tower,
  node, shipyard and altar fly nothing - what `kPsrClasses` in
  `src/engine/map_resources.cpp` records, and what Tutorial level 0 shows.
* **A city packs its size into the flag byte's top two bits**
  (`(this+0x4d - 1) * 0x40`; `TCity.ReadWrite` rw tag 0x10 -> +0x4d). The draw
  reads them back as `flagbyte >> 6` to choose the flagpole sprite, so a bigger
  city flies a bigger pole.
* **A razed city flies nothing**: `TCity.GetRazed@557ab474` is
  `(this+0x44 & 2) != 0` (rw tag 0x16) and `UpdateFlagID` only allows the flag
  while `TStructure.GetBuilding` (`this+0x24 != -1`, the build counter) is true.
  `TStructure.Create` leaves that counter at -1 and **no shipped map writes
  rw tag 0xff**, so for a scenario-placed structure it is always false and the
  gate is simply "not razed". 58 of the 1989 cities in the shipped maps carry
  tag 0x16 = 2.

The port models this in `structure_flag_byte` (`src/engine/mapviz.cpp`); the two
runtime-only gates - `this+0xc & 4` and the per-player fog terms of `Show`
(`map+0xc8`, `this+0x2c`) - are documented as "visible", because a scenario file
never stores the first and the port has no current player to be fogged for.

## 3. Where the pole and the flag are anchored

As in `ShowComplexImage`: the flag control receives
`HXtoHP + (resource+0x68, resource+0x6c)` and then

* the **pole** is blitted at that anchor, and
* the **flag** at `anchor + (-0x15, -0x10)`,

each with its own ILB `base+shift` added inside the image's `Show`
(`TFlagControl.ShowFlag@5575a1e8` / `ShowStructureFlag@5575a34c`):

```c
Get(control+0x10 /* Flags.ILB */, pole_frame)->Show(x, y);
Get(flag_lib, flag_frame)->Show(x - 0x15, y - 0x10);
```

`resource+0x68/+0x6c` start as `TPlayerStructureResource.Create@55761974`'s
`(0x14, -0x23)` and are **overwritten by `GenerateFlagPositions@557619e8`** -
but only while `resource+0x44` is non-zero:

| `resource+0x46` (pad index) | 1 | 2 | 3 | 4 |
|---|---|---|---|---|
| anchor `(+0x68, +0x6c)` | `(0x14,-0x1e)` | `(0x23,-0x32)` | `(0x05,-0x2d)` | `(0x1e,-0x3c)` |

The pad index is `CalculatePadIndex@55760524`: the number of `TTerrainList`
entries whose terrain byte is `>= 0` as a signed char
(`TTerrainList.GetTerrain@55605054` returns the entry's low byte; a hexagon with
no terrain ships `0xFE`). In other words **the flag sits on the pad the
structure covers**, and for a city the pad index *is* its size - the shipped
`City1..City4.ILB` resources return 1/2/3/4, which is exactly the four rows
above (`mapres_test` prints them).

The `+0x44` gate is per class and real: `Mine.TMineResource.Create` and
`TPowerNodeResource.Create` clear it (a mine keeps `(20,-35)`), while
`TTowerResource.Create`, `TBuildersGuildResource.Create` and
`ReflectingPoolResource.Create` set it (they take the pad row). The port bakes
each class's `Create`-chain value into `MapResource::flag_pad_anchored`
(`kPsrClasses`).

## 4. The sprites and the timing

`TFlagControl.Create@5575a10c` links **`Flags.ILB`** into `control+0x10`
(`Images/Flags.ilb`), while `control+0xc` is the game's `TRaceResourceList` - the
same list a city's body uses, so `RACES\RHUMAN.ILB` carries both the Human city
art *and* the Human flag. The slots:

| library | frames | what |
|---|---|---|
| `Flags.ILB` | 0..3 | the four multi-hex flagpole variants (bigger structures/cities) |
| `Flags.ILB` | 4 | the single-hex flagpole (`ShowFlag`) |
| `RACES\R*.ILB` | 0..11 | that race's structure flag, 12 frames |
| `RACES\R*.ILB` | 12..23 | that race's city flag, 12 frames |
| `Flags.ILB` | 10..21 | the neutral structure flag, 12 frames |
| `Flags.ILB` | 22..33 | the neutral city flag, 12 frames |

`Show` picks the variant from the *resource's* pad index
(`if (resource[0x46] < 2) ShowFlag else ShowStructureFlag`): a structure
covering one hex (and a `City1`) uses the fixed pole 4 and the race frames
12..23, a bigger one the pole `flagbyte>>6` and frames 0..11.

The frame is driven by the map clock `AoWHSMap+0x40` **plus the hexagon's
*pixel* x** (`TPlayerStructure.Show` passes `HXtoHP.x + map+0x40` - a different
per-hex phase from the building animation's `x + y*0x10`):

```
ShowStructureFlag:  frame = ((clock + HXtoHP.x) >> 1) % 12   (a step every 2 ticks)
ShowFlag:           frame = ((clock + HXtoHP.x) >> 2) % 12   (a step every 4 ticks)
```

`aow::structure_flag_step` implements exactly those two, on the same
`MapVizOptions::clock` the building overlays already use
(`docs/17_BUILDING_ANIM.md`), so a live front-end that advances `game_clock`
once per 30 fps tick animates flags and buildings in one time base.

## 5. How the port draws it

* The **object pass** of `render_map_rgba` blits the pole and the flag (in that
  order) at `hex_origin(x,y) + (dx, dy) + sprite base+shift`, adding the same
  `kHexFieldMargin` in x as a structure's sequence frames - the engine reaches
  the flag control by the identical `image->Show(anchor + offset, ...)` call, so
  the flag stays glued to the building it belongs to.
* `TPlayerStructure.Show` / `TCity.Show` draw the flag **before** the structure
  itself, so the pole is planted *behind* the building; the port does the same
  (`blit_flag` runs before `draw_city` / the body blit).
* Because the frame depends on the clock, the flag belongs to the *animated*
  pass: with `MapVizOptions::no_animated_overlays` the cached level omits it and
  a live front-end composites it per map frame with `draw_structure_overlays`
  (which now covers cities too, whose bodies stay static in the cached level).
  A one-shot render bakes it at `opt.clock` like every other animation. The
  **armies** are composited *after* the flag (`draw_unit_overlays`,
  `docs/13_UNITS_ON_MAP.md`), so a unit standing on a structure's own hex -
  Tutorial L0's Orc hero at (32,21) - is drawn over the pole, not under it.
* The owner roster is read with `aow::map_player_races(map)` - the light half of
  `load_game_state` (no map sweep), because `draw_structure_overlays` runs once
  per map frame.

## 6. Tests and out of scope

`mapres_test` (`+16` checks) locks: both class tables and their shape, a mine's
`(0x14,-0x23)` anchor, the shipped City1..City4 pad indices and anchors,
`TTowerResource`'s pad re-anchor, an owned mine flying `RACES\R*.ILB` on
`Flags.ILB`'s pole with the flag sprite at `(-0x15,-0x10)`, an unowned mine /
cave / razed city flying nothing, an unowned city flying the neutral flag, and
the two timing rules - plus a render-level check that Tutorial level 0's Goblin
city at hex (32,21) gains pixels when the overlays are composited and that those
pixels change as the map clock advances. `aow_view --debug-csv` writes the
resolution as `flag,<x>,<y>,<resource>,<library>,<frame>,<entry>,<w>,<h>,<ok>`
rows; measured on Tutorial level 0 at `--clock 0`:

```
flag,32,21,22,Flags.ILB,4,flags.bmp,2,45,1          # Goblin city pole (City1)
flag,32,21,22,RACES\RGOBLIN.ILB,18,browns07.bmp,7,11,1
flag,16,25,58,Flags.ILB,0,flags.bmp,3,50,1          # unowned Halfling City3: neutral
flag,16,25,58,Flags.ILB,18,greyb09.bmp,12,18,1
flag,36,38,25200465,Flags.ILB,4,flags.bmp,2,45,1    # player 2's reflecting pool
flag,36,38,25200465,RACES\RGOBLIN.ILB,14,browns03.bmp,9,11,1
```

Open `[TODO(RE)]`:

* **Fog of war.** `Show` gates the flag on `map+0xc8` / `this+0x2c` (per-player
  visibility); the port renders the whole map for nobody in particular and draws
  every flag.
* **`this+0xc & 4`.** A runtime-only status byte (`HSEngine.TMapObject.ReadWrite
  @55609824` touches a neighbouring bit of the same field but never serialises
  it) - the port treats it as 0.
* **`ShowFlagEx` / `ShowStructureFlagEx`.** The same draws with a "draw the pole
  first" flag, used by the engine's event/selection paths; a scenario map never
  asks for them.
* **`TRandomNode`.** `RandomNode.TRandomNode` (0x20822) is a multi-hex
  `TProductionPlace` whose resource picks its contents at random; it is flagged
  like any other ownable structure (and never appears in the shipped scenarios).



