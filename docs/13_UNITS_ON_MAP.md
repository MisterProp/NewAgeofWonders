# 13 - The units placed on a scenario map

This document records the reverse-engineering of how a `.HSM` / `.CSM` scenario
stores the **armies and units** on its map, and the C++ reconstruction in
`src/include/aow/map_types.hpp` (`MapUnit`, `MapHs::units`) + `src/engine/map.cpp`
(`parse_army`) with its rendering in `src/engine/mapviz.cpp` and the tests in
`src/tests/{map_test,mapres_test}.cpp`.

It was recovered with **GhidraMCP** (`tools/ghidra/`) from `AoWEPACK.dpl` (image
base `0x55700000`); the decompiles live under `re/exports/aowepack/`
(`AoWE.TArmyHS.*`, `AoWE.TUnitHS.*`, `AoWE.TUnit.*`, `AoWE.THero.*`,
`AoWE.TArmy.ReadWrite`). No separate palette is needed: every `UNITS\*.ILB`
encodes its own palette (see `docs/01_FORMATS.md`, ILB section), so a unit drawn
from its library is already 1:1.

## The object chain

A `TMapField`'s table lists its hexagon sprites under tags >= 50 (see
`docs/01_FORMATS.md`). One of those, engine class **`0x20217` `AoWE.TArmyHS`**,
is an army:

```
TMapField                         (tags >= 50 = hexagon sprites)
  tag 50   terrain hexagon
  tag 51+  objects, one of which is ...
    AoWE.TArmyHS         class 0x20217   (extends AoWE.TUnitHS)
      tag  9  rwByte    +0x14 = TUnitHS facing / image-sequence index
      tag 10  rwEObject the embedded AoWE.TArmy  (a bare table, NO class id)
        tag  1  rwChildren -> the unit objects, in order:
          AoWE.TUnit    class 0x20213   (a normal unit)
          AoWE.THero    class 0x20230   (a hero)
          AoWE.TLeader  class 0x20232   (a wizard leader; a THero subclass)
```

Evidence:

* `re/exports/aowepack/AoWE.TArmyHS.ReadWrite.c` (`0x55790d88`) calls
  `TUnitHS.ReadWrite` then writes `*(param_1+0x1c)` at **tag 10** through the
  stream's slot `+0x0c` = **rwEObject** (an embedded EObject - *no* class id; see
  the stream vtable in `docs/01_FORMATS.md`).
* `AoWE.TUnitHS.ReadWrite.c` (`0x55783ed4`) writes `+0x14` at **tag 9**.
* `AoWE.TArmy.ReadWrite.c` (`0x5578e8ec`) is a `TEObject` (its own tags 4..0xd);
  its **tag 1** is the children table (`rwChildren`), which is where the units
  live.

## The units

Each unit child carries its own tag table (a `[u32 class id][table][data]`
record, like every children table). The tags a placed unit uses:

| class | tag | meaning |
|-------|-----|---------|
| `TUnit` `0x20213` | rw tag 10 (rwWord) | the TUnitResource record-list tag (the unit *type*) |
| `TUnit` / `THero` / `TLeader` | rw tag 5 (rwIntegerEx, def -1) | `TAbstractUnit.id` (per-map id) |
| `TUnit` / `THero` / `TLeader` | rw tag 6 (rwByte) | `TAbstractUnit` morale (+0x26) |
| `THero` `0x20230` | rw tag 8 (rwInteger) | the THeroResource index (the hero *type*) |

The plain **`TUnit` tag 10** value matches the `Unitres.pfs` record-list tag -
exactly the `id` field of `aow::UnitEntry` (see `docs/11_UNIT_CATALOG.md`). So
`load_unit_catalog` + `UnitCatalog::find_gfx` resolve it to the `UNITS\*.ILB`
sprite folder directly. `TUnitResource.UnitResourceIndex` (`0x55784d90`) is what
the engine writes there (`AoWE.TUnit.ReadWrite.c`, tag 10).

A **hero/leader** (tag 8) resolves through `Herores.pfs` instead: the tag is a
`THeroResource` record-list tag, and `THero.GetUnitGFXResourceIndex`
(`0x557875bc`) returns the resource's `+0x18` (`gfx_index`) - the same key the
Unitgfx list uses. `UnitCatalog::hero_gfx` / `find_hero_gfx` cover this.

### Heroes (`AoWE.THero` / `TLeader`)

A `THero` record carries far more than a sprite. `AoWE.THero.ReadWrite`
(`0x55788880`) serialises it; the fields the port loads into `MapUnit`:

| tag | type | field | getter |
|-----|------|-------|--------|
| 5  | rwIntegerEx | `id` | `TAbstractUnit` |
| 6  | rwByte | morale | `TAbstractUnit` |
| 7  | rwInteger | `face_index` (+0x3c) | `GetFace` `0x557887dc` |
| 8  | rwInteger | `hero_resource_index` (+0x44) | `SetHeroResourceIndex` `0x55788840` |
| 10 | rwString | `name` (+0x60) | `GetName` `0x55786fa4` |
| 11 | rwString | `nickname` (+0x64) | `GetNickname` `0x55786ab0` |
| 15 | rwData(1) | `gender` (+0x69) | `GetGender` `0x55786c34` |
| 22 | rwByte | `unit_level` (+0x4c) | `GetUnitLevel` `0x55786a7c` |
| 24 | rwInteger | `experience` (+0x48) | `GetExperience` `0x55787670` |
| 38 | rwString | `library_name` (+0x94) | `GetLibraryName` `0x55786980` |

`TLeader` (`0x20232`) is a `THero` subclass (`TLeader.ReadWrite` `0x5578b470`
adds tags 0x2d/0x2e), so it loads through the same path.

**Unset hero resource.** `THero.Create` (`0x55786848`) leaves `+0x44 = -1`, and
the storage stream writes a field only when it differs from that Create default -
so a hero whose resource index is `-1` simply **omits tag 8** (16 of the 437
shipped heroes). `parse_army` keeps `resource = -1` for those, and the renderer
falls back to the first hero resource (`Herores.pfs` tag 0 = Human) - e.g.
Lizardman2.csm's "Esmeralda the Healer" (tag 8 absent) draws `UNITS\HMN\HMNHX.ILB`.

> **Zero-valued fields are omitted by the storage stream.** A fresh unit's
> `experience` (TUnit tag 7) is absent, and a unit/hero whose resource tag would
> be `0` also drops the tag - so a **missing tag 10 / tag 8 means the value is
> 0** (the first unit / hero resource), not "no resource". 0xffff is the explicit
> "no resource". `parse_army` applies this.

## The rendering (1:1)

`re/exports/aowepack/AoWE.TArmyHS.UpdateUnitGFXResource.c` (`0x55790c1c`) picks
the army's **lead** unit - index 0, or the first non-concealed one - and sets the
army sprite's `TUnitGFXResource` from that unit. So an army on the map draws
**one sprite: its lead unit's**.

`re/exports/aowepack/AoWE.TUnitHS.Show.c` (`0x55784614`) is the draw:

```
HXtoHP(hex.x, hex.y)  ->  (px, py)                      # the hex top-left
anchor = (px - scrollx, py - scrolly)
for each image sequence the gfx resource lists:
    frame = seq.GetFrameIndex(clock, unit)              # current animation frame
    if frame != -1:
        image = library(seq.lib).Get(frame)
        image.Show(anchor.x + seq.xoff, anchor.y + seq.yoff, dst)  # dest = that + base + shift
```

Every `UNITS\*.ILB` image sequence in the Unitgfx tag-5 `TImageSequenceList`
stores the same `xoff = -16`, `yoff = -24`, so the 64x64 cell is centred up-left
of the hex and the figure stands on it. `mapviz` reproduces the anchor exactly
(the sequence's `xoff/yoff` + the sprite's own `base+shift`), and draws the
**standing pose of the army's stored facing**: `TUnitHS.Show` draws `seq[dir-1]`
when idle, so `mapviz::resolve_unit_pose` resolves `MapHs::army_direction`
(rw tag 9) through the same sequence table the walk uses. An army that has just
walked therefore keeps facing the direction it arrived from (docs/16_UNIT_ANIM.md
section 7: the port also aligns the walking cycle with the move and settles it
into that standing pose, so the swap on arrival is not a pose snap).

## Result

* `parse_army` fills `MapHs::units` (lead first) + `MapHs::army_direction`.
* `mapviz` draws each `AoWE.TArmyHS`'s lead-unit sprite at its hex, resolved
  through the unit catalog / Herores table, plus the army's **ground shadow**
  (`AoWE.TUnitShadowHS` -> `TUnitHS.ShowShadow@0x55784504`), which is a separate
  scene object one show-priority layer below the body - so all shadows are
  composited before all bodies (docs/16_UNIT_ANIM.md section 10).
* **The army's size banner is drawn** (`docs/21_ARMY_COUNTER.md`).
  `TArmyHS.Show@0x557914d8` draws, after the lead unit's sprite, the owner race's
  `PartyShield` carrying the army's unit count
  (`TRaceResourceList.GetRaceResource(army+0x16 /* the owner's race */)`
  library, frame `(GetUnitCount() - ConcealedUnitsCount()) + 0x1d`) at the hex
  anchor `+ (-5,-0xc)`, with a crown overlay from `AoWHSSet+0x78+0x10` (the
  `TFlagControl`'s `Flags.ILB`) at frame `race + 0x50` when the army carries a
  `TLeader` (`TArmy+0x10 & 0x20`, set by `UpdateConcealment`), and the `0x20`
  (ownerless) variant from `Flags.ILB` at frame `count + 0x27`. The owner race is
  `TPlayer.GetRace` (`+0xa5`, i.e. `aow::map_player_races`) for owner > 0 and
  `0x20` for the independents (owner 0) - exactly `TArmyHS.UpdateFlagID`.
  `aow::army_banner_rgba` / the `PendingUnit::banner` pass reproduce this for the
  one-shot render, the live `draw_unit_overlays` and the walking sprite; the
  art/placement match `bin/counterRef.png` pixel-for-pixel
  (`bin/CounterBench.png`, `bin/CounterDiff.png`).
* **Units are composited last, on top of the terrain, objects, roads and bridges.**
  The engine shows them as ordinary hexagon sprites, so a unit could be covered
  by an object on a hex drawn later in `mapviz`'s row-major sweep; instead
  `render_map_rgba` collects every army during that sweep and blits the sprites
  in a final pass (`pending_units`), keeping the relative row-major order of the
  units.
* **A live front-end composites them per map frame, after everything else.** The
  cached level is rendered with `MapVizOptions::no_units`, which leaves the armies
  (bodies *and* ground shadows) out; the viewer's `draw_structure_anim`
  (`src/game/viewer.cpp`) and the in-game screen's `mv_draw_structure_anim`
  (`src/game/menu.cpp`) then re-run `aow::draw_unit_overlays` every map frame -
  **after** `draw_structure_overlays` and `draw_water_overlays`, so a unit
  standing next to a building is never covered by the building's animated sparkle
  / glowing orb, by its **owner flag** (Tutorial L0's Orc hero stands on the
  Goblin city hex (32,21)!) or by a water ripple. The engine's sorted scene draws
  them in that order too - `TUnitHS.CalcShowPriority@0x557840c8` returns
  `0x68000000 | position`, above a map object's layer. The only layer meant to
  sit *above* the units is the map's **spell gfx**: `AoWE.TWorldFXHS`
  (`Create`/`Activate`/`NewFrame`/`Show@55797118`), which the port does not
  implement yet (`[TODO(RE)]` its `GetShowPriority@557960a8`).
* `draw_unit_overlays` walks the level row-major, resolves every army's standing
  pose + shadow (the standing pose is a mode-0 sequence, so it is clock
  independent; the flyers' shadow tables are framed from the map clock) and
  composites **all shadows first, then all bodies** - `TUnitShadowHS
  .GetShowPriority@0x55783d64` is `(unit & 0x3ffffff) + 0x64000000`, one layer
  below its unit. It is clipped to the caller's viewport like
  `draw_water_overlays` (the armies are spread over the whole level, so its dirty
  rectangle would otherwise be the map), and it must be applied to the *pristine*
  canvas of the frame: the shadow's 50 % average is not idempotent.
* **The scanner's minimap keeps the cached level**, which no longer carries the
  army sprites; the engine's `TScanner` picks its armies from the `ArmyCheck`
  toggle instead (`docs/09_GAME_SCREEN.md` section 7).
* Only the interface (drawn by `game/menu.cpp`) and the interactive movement
  overlay (`draw_move_path_overlay`, the walk sprite) sit above them.
* **Tutorial.hsm** (level 0) carries 4 armies and their sprites all resolve
  (rows from `aow mapviz --debug-csv`, locked by `mapres_test`):

  ```
  unit,33,16,113,UNITS,609,giant609.bmp,32,39,1     # Giant (dir 5 -> seq 4)
  banner,33,16,113,FLAGS,43,partyshield.bmp,11,14,1 # ownerless 4 (+0x27)
  unit,32,21,  9,UNITS,309,orchx309.bmp,31,48,1     # Orc hero (Herores tag 9, dir 2)
  banner,32,21,  9,RACES,33,partyshield.bmp,13,13,1 # Orc 4 (+0x1d)
  unit,15,34, 41,UNITS,309,hlfhe309.bmp,24,42,1     # Halfling hero (dir 2)
  banner,15,34, 41,RACES,33,partyshield.bmp,11,12,1 # Halfling 4
  crown,15,34, 41,FLAGS,85,partyshield.bmp,15,6,1   # TLeader race 5 + 0x50
  unit,36,38, 46,UNITS,209,gbnhe209.bmp,27,37,1     # Goblin hero (dir 3)
  banner,36,38, 46,RACES,32,partyshield.bmp,13,13,1 # Goblin 3
  crown,36,38, 46,FLAGS,90,partyshield.bmp,15,7,1   # TLeader race 10 + 0x50
  ```

  The frame id is the *standing* pose of each army's `army_direction`: facing 1
  -> `4xx`, 2 -> `3xx`, 3 -> `2xx`, 4 -> `1xx`, 5 -> `6xx`, 6 -> `5xx`.

  Across all 72 shipped maps `map_test` parses 6456 placed units (6019 `TUnit` +
  437 heroes/leaders); every hero loads a name and no plain unit fails to
  resolve a sprite tag. The hero-led armies of Tutorial level 0 load as
  `Doktok the Fat` (Orc, `THeroResource` 9 → `UNITS\ORC\ORCHX.ILB`),
  `Trodsen` (Halfling leader, 41) and `Borok` (Goblin leader, 46).

## Tools

`aow mapunits <map.HSM> [--root <game-dir>] [--level N]` lists every army a map
places and the units/heroes it carries (name, class, resource, level, resolved
sprite):

```
aow mapunits Scenario/Custom/Tutorial.hsm --root .. --level 0
```

`tools/eobj_dump.py` prints the raw EObject graph, including each army's unit
list, without rebuilding the port:

```
python tools/eobj_dump.py Scenario/Custom/Tutorial.hsm --class 0x20217
python tools/eobj_dump.py Scenario/Campaign/Dwarf1.CSM --tile 30,32
```
