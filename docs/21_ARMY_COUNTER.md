# 21 - The unit counter on top of an army

This document records the reverse-engineering of the size banner the original
game draws over every army on the world map, and its 1:1 reconstruction in
`src/engine/mapviz.cpp` / `src/include/aow/mapviz.hpp`.

It was recovered with **GhidraMCP** from `AoWEPACK.dpl` (image base
`0x55700000`):

| function | address | what it establishes |
|----------|---------|---------------------|
| `AoWE.TArmyHS.Show` | `0x557914d8` | the whole draw order: lead sprite -> size shield -> crown |
| `AoWE.TArmyHS.UpdateFlagID` | `0x55790be8` | army race byte `+0x16`: `TPlayer.GetRace` for owner > 0, `0x20` for owner 0 |
| `AoWE.TArmy.UpdateConcealment` | `0x5578c98c` | sets army `+0x10` bit `0x20` when `TArmy.Leader` finds a `TLeader` (class `0x20232`) |
| `AoWE.TArmy.Leader` | `0x5578dfbc` | the `TLeader` search |
| `AoWE.TArmy.ConcealedUnitsCount` | `0x5578cb84` | the `+0x2a` concealment mask the count subtracts |
| `AoWE.TFlagControl.Create` | `0x5575a10c` | the crown library is `Flags.ILB` (control `+0x10`) |
| `AoWE.TRaceResource.LinkToIL` | `0x55759cdc` | a race resource `+0x1c` is its `RACES\R*.ILB` library |
| `AoWE.TRaceResourceList.GetRaceResource` | `0x5575a0c4` | race index -> `TRaceResource` |

## The layout

`TArmyHS.Show` first computes the screen anchor from `HXtoHP(hex)` and then
draws the lead unit (`TUnitHS.Show`) before the banner. The banner part is:

```
race = army+0x16                     // TArmyHS.UpdateFlagID
count = TArmy.GetUnitCount() - TArmy.ConcealedUnitsCount()

if race == 0x20:                     // owner 0, the independents
    library = Flags.ILB
    frame   = count + 0x27
else:
    library = TRaceResourceList.GetRaceResource(race)+0x1c
    frame   = count + 0x1d

shield at (anchor.x - 5, anchor.y - 0xc) + sprite base+shift

if army+0x10 & 0x20:                 // a visible TLeader
    library = TFlagControl+0x10      // Flags.ILB
    frame   = race + 0x50
    crown at (anchor.x - 5, anchor.y - 0xe) + sprite base+shift
```

`anchor` is `HXtoHP(hex)`, i.e. the same origin the unit body uses **before**
its image-sequence offset `(-16,-24)`. The banner therefore sits above-left of
the figure, not on the body's 64x64 cell.

## The art

The twelve `Images\Races\R*.ILB` libraries each carry eleven `PartyShield.BMP`
frames at slots `0x1c..0x26`:

| slot | meaning |
|------|---------|
| `0x1d` (29) | blank shield (count 0) |
| `0x1e..0x26` (30..38) | count 1..9 |

`Images\Flags.ILB` carries the same shape as class `0x11` for the ownerless
armies at slots `0x27..0x31` (39 blank, 40..48 = 1..9), and the twelve crowns
at slots `0x50..0x5b` (80..91, race index). Their placement fields:

| art | class | base+shift | anchor offset |
|-----|-------|------------|---------------|
| race shield | `0x16` | `(5,3)` | `(0,-9)` |
| ownerless shield | `0x11` | `(5,3)` | `(0,-9)` |
| crown | `0x11` | `(3,0)` | `(-2,-14)` |

`TArmyHS.Show` uses the image's vtable `+0x84` Show slot. For this art it is a
colour-keyed, **opaque** blit: the sprite's palette values land unchanged on the
map (the reference crop matches them exactly). It is *not* the class-0x16
brighten blend used by the map-object/sequence path, so the banner has its own
blit path (`blit_banner` in `render_map_rgba`,
`blit_sprite_buffer` in `draw_unit_overlays`).

## The port

* `resolve_army_banner` (internal, `mapviz.cpp`) implements the formulas above.
  `MapHs::owner` gives the roster index; `aow::map_player_races(map)` gives the
  race; owner 0 (or an unusable race) selects the `0x20` `Flags.ILB` shield.
  When the caller has no `Release.hss` race list, the shipped faction order from
  `docs/11_UNIT_CATALOG.md` is used (the live per-frame path).
* `render_map_rgba` attaches a `PendingUnit::banner` to every army and draws
  body -> shield -> crown per army, so banners stack with their own army and the
  class-0x16 body blend stays untouched.
* `draw_unit_overlays` (the live per-map-frame composite used by `aow_view` and
  the in-game screen) resolves and draws the same banner in the same order. The
  existing `no_units` level + overlay contract stays pixel-identical.
* `army_banner_rgba` returns the shield+crown as one RGBA sprite; the viewer's
  and the in-game screen's `draw_anim_sprite` blit it at the walk's interpolated
  anchor, so the banner travels with a party while it moves (the destination
  army is hidden, exactly as before).
* `MapVizOptions::debug_csv` writes `banner,...` and `crown,...` rows naming the
  library/frame/size (see `docs/13_UNITS_ON_MAP.md`); `mapres_test` locks the
  Tutorial's 4 shields and 2 crowns.

## Benchmark

`bin/counterRef.png` is the original game's crop: a Human army of **1** on the
left and a Human army of **5** with a crown on the right. The decoded
`Images\Races\RHUMAN.ILB` frames 30 (count 1) and 34 (count 5), and
`Images\Flags.ILB` frame 80 (Human crown), match the reference's banner pixels
**exactly** (100 % of opaque pixels) at the reference positions:

```
reference shield 1 frame 30 at (15,13)
reference shield 5 frame 34 at (47,29)
reference crown   frame 80 at (45,24)
```

A port render of `Scenario\Custom\River of Life.hsm` level 0 (the Human
`TLeader` Cintra with 5 units at hex (28,37)) draws the same frame 34 + frame 80.
Aligning that banner to the reference position and comparing only the opaque
banner art gives:

```
shield5  opaque=120  changed>8=0  mean|d|=0.994/255  max=5
crown    opaque= 60  changed>8=0  mean|d|=0.894/255  max=7
```

(the residual is the ILB palette's 565 rounding, not a layout or blend error).

The same holds for the **live 640x480 in-game screen**:

```powershell
AoW.exe --root <game> --start-map "River of Life.hsm" --scen-human 0 `
        --render bin\CounterGameFull.png --no-music --no-cursor
```

its Human army draws frame 34 at (204,191) and the aligned banner/crown metric
is identical (`0.994` / `0.894` mean |d| per channel). Evidence images:
`bin/CounterBench.png` (reference art / port art / diff x8), `bin/CounterDiff.png`
and the in-game crop `bin/CounterGame.png`.
