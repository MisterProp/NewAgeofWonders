# 18 - The water hexagon's animation (the moving sea)

This document records the reverse-engineering of **how Age of Wonders 1 animates
the sea** - the sixteen wave frames of the base tile, the per-direction foam of
the static pass, and the per-hexagon *private* counter `ShowDynamic` uses to play
one-shot ripple / fish / sparkle image sequences - and its 1:1 reconstruction in

* `src/include/aow/map_resources.hpp` + `src/engine/map_resources.cpp`
  (`aow::WaterHexAnim` and its rules),
* `src/include/aow/unit_anim.hpp` + `src/engine/unit_anim.cpp`
  (`aow::image_sequence_unlooped_frame`),
* `src/engine/mapviz.cpp` (`aow::draw_water_overlays`, the base-tile and foam
  passes, `MapVizOptions::water` / `no_animated_overlays`),
* the live front-ends (`src/game/viewer.cpp`, `src/game/menu.cpp`),
* the tests in `src/tests/mapres_test.cpp` (37 new checks).

Everything was recovered with **GhidraMCP** (`tools/ghidra/`) from `AoWEPACK.dpl`
(image base `0x55700000`), `HSEPack.dpl` (`0x55600000`) and `ILPACK.dpl`
(`0x55200000`). The decompiles live under `re/exports/` (the annotated
`aowepack/AoWHex.TAoWWaterHexagon.{Create,Show,ShowDynamic,UpdateTransitions}.c`,
`hsepack/IsoHex.TLowerIsometricHexagon.ShowDynamic.c` and
`ilpack/ImageLib.TImageSequence.GetUnloopedFrameIndex.c`), and the resource's own
sequence list can be dumped with
`python tools/structseq_probe.py ..\Release\Release.hss 0x2073e`.

## 1. Two passes over one hexagon

A water (or ice) tile is a map object of class **`AoWHex.TAoWWaterHexagon`**
(0x2073d, `ClassID@5579ad90`) whose `TMapObjectResource` is class
**`AoWHex.TAoWWaterHexagonResource`** (0x2073e). The shipped pack has exactly two
of those resources:

| id | library | terrain |
|----|---------|---------|
| `0x007b134f` | `WATERHEX.ILB` | 0 water |
| `0x00af9569` | `ICEHEX.ILB`  | 6 ice |

The engine draws such a hexagon twice:

* **`AoWHex.TAoWWaterHexagon.Show@5579aeac`** - the *static* pass: the base tile
  plus up to three foam sprites (called when the scene is (re)built);
* **`AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4`** - the *dynamic* pass, once
  per rendered map frame: the ripple / fish / sparkle sequence, and then the
  shoreline shelf (`IsoHex.TLowerIsometricHexagon.ShowDynamic@5561a400`).

## 2. The static pass: the base tile and the foam

```
// Show@5579aeac
HXtoHP(field.x, field.y) -> (px, py);  px -= cam_x + 8;  py -= cam_y;
lib = GetImageSequence(resource +0x24 /* TImageSequenceList */, 0).library;  // +0x10
frame = ((field >> 3) + field.x + field.y*2 + (sprite+0x15 >> 6)) & 0xf + 0x8c;
Get(lib, frame).Show(px, py);
px -= 6; py -= 9;
if (sprite+0x15 & 0x01) Get(lib, 0x24).Show(px, py);   // T_Ic-Wa2
if (sprite+0x15 & 0x02) Get(lib, 0x25).Show(px, py);
if (sprite+0x15 & 0x20) Get(lib, 0x23).Show(px, py);
```

* **The base tile is one of sixteen `H_Wa2.BMP` wave frames** -
  `WATERHEX.ILB` slots `0x8c..0x9b` (140..155), 48x32 each, all distinct art.
  `frame` mixes three terms: `field >> 3` (**the hexagon object's address**), the
  hex's `x`, `2y`, and `sprite+0x15 >> 6` - the `RandInt(4)` `Create` stores
  (`Create@5579ad40`: `sprite+0x18 = -1 - RandInt(0x14)`, `sprite+0x15 |=
  RandInt(4) << 6`). So every water tile shows its own phase of the wave: the
  ocean is a *field of wave phases*, which is why it does not read as one
  repeated tile. It is **not** animated over time - the phase is fixed per
  hexagon for as long as the sprite lives.
* **The foam** is `T_Ic-Wa2` (slots 0x23/0x24/0x25; 60x50 nominal, only ~25x22
  of art) blitted at the base tile's `(-6,-9)`, where `sprite+0x15` bits 0/1/5
  are set by `UpdateTransitions@5579ad98` for a neighbour that carries an
  `IsoHex.TLowerIsometricHexagon` - this hexagon's own water/ice family - and
  whose terrain differs from ours. That is exactly the **water/ice boundary**:
  `T_Ic-Wa2` is the ice-water fringe, and on Icedale (which ships both) it
  appears from both sides, the ice hexagons drawing it out of `ICEHEX.ILB`,
  which carries the same 115-entry layout as `WATERHEX.ILB`. A *border* neighbour
  sets **neither** mask, so the map edge gets no foam and no tan shelf.

`UpdateTransitions@5579ad98` also computes the **shoreline shelf's** mask
(`sprite+0x14`: one bit per neighbour that carries *neither* of those two
classes, i.e. the flat land) and the per-direction variant bytes
`sprite + dir + 0x19`: `0x28` when the land neighbour's terrain is 3 (snow),
`0x50` when it is 5 (wasteland), 0 otherwise. Those are added to the shelf slot
numbers to reach the `t_wa-sn` / `t_wa-wl` runs (0x28.. / 0x50..); for
grass / desert / steppe they are 0. The shelf itself (the tan fringe) is
`aow::water_shore_pieces` - see `docs/ROADMAP_mapviewer_fidelity.md` Phase 4.

## 3. The dynamic pass: the counter, the roll and the sleep

```
// ShowDynamic@5579afb4 - once per rendered map frame
HXtoHP(field.x, field.y) -> (px, py);  px -= cam_x + 8;  py -= cam_y;
if (engine+0x6d != 0 && (sprite+0x1a != 0 || <<level flag>> == 0)) {
    if (++sprite+0x18 >= 0) {                       // a *signed* byte counter
        drew = false;
        if (sprite+0x18 == 0)  roll(sprite+0x19);   // see below
        list = resource +0x24;                      // TImageSequenceList
        if ((byte)sprite+0x19 < list.count)
            for (seq = list.items[sprite+0x19]; seq; seq = seq->next) {
                frame = GetUnloopedFrameIndex(seq, sprite+0x18);
                if (frame != -1) {
                    drew = true;                                      // (*)
                    if (ImageExist(seq->library, frame))
                        Get(seq->library, frame).Show(px + seq->xoff,
                                                      py + seq->yoff);
                }
            }
        if (!drew) sprite+0x18 = -(engine+0x6d == 1 ? 0x14 : 10) - RandInt(0x19);
    }
}
TLowerIsometricHexagon.ShowDynamic(this, ...);      // the shelf + its variant pass
```

`roll` - the eleven values `sprite+0x19` can take:

```
if (((map+0x188) -> +0x10) & 0x10) == 0 {                   // branch A
    if (open_water && RandInt(700) == 0 && (map+0x3c & 2) == 0)
        sprite+0x19 = 0x1e + RandInt(10);                   // the fish chain
    else
        sprite+0x19 = 0x14 + RandInt(10);                   // a ripple
} else {                                                    // branch B
    if (open_water && RandInt(300) == 0 && (map+0x3c & 2) == 0)
        sprite+0x19 = 0x1e + RandInt(10);
    else if (RandInt(10) < 3)
        sprite+0x19 = 0x32 + RandInt(9);                    // a SEFFECT sparkle
    else
        sprite+0x19 = 0x14 + RandInt(10);
}
```

`open_water` is `sprite+0x14 == 0` - a water hex with **no** flat-land
neighbour, so the 1-in-700 fish never spawns along a coast.

`GetUnloopedFrameIndex@0x552188a8` is the *one-shot* sibling of the
`GetFrameIndex` the buildings and units use (`docs/16_UNIT_ANIM.md`,
`docs/17_BUILDING_ANIM.md`):

| tick (`sprite+0x18`) | result |
|----------------------|--------|
| `< 0` | `-1` - the counter is still asleep, nothing is drawn |
| mode 0 | the stored base frame |
| mode 1 | `tick / (period+1)`, and `-1` once that reaches the library's frame count |
| mode 4 | `frame_table[tick / (period+1)]`, and `-1` once that reaches the table length |
| modes 2 / 3 / 5 | `-1` (a one-shot caller cannot use ping-pong or a random pick) |

(*) note **where** `drew` is set: the engine marks "this hexagon drew something"
*as soon as the resolver accepts the tick*, before it tests the image. That is
load-bearing - `WATERANM.ILB`'s `Water010` is a **0-byte placeholder**, the
deliberately blank frame the ripple tables lead with - so a ripple whose table
still names frames keeps its counter running even while it draws nothing.

### What the resource actually contains

`WATERHEX.ILB` is not just the base tile - it is a mixed library:

| slots | art | role |
|-------|-----|------|
| `0x00..0x1f` | `t_wa-gr.BMP` (86x84) | the grass-family shelf / coast overlays |
| `0x23..0x25` | `T_Ic-Wa2.BMP` | the foam (mask bits 0/1/5) |
| `0x28..0x47`, `0x50..0x6f` | `t_wa-sn.BMP`, `t_wa-wl.BMP` | the snow / wasteland shelves |
| `0x8c..0x9b` | `H_Wa2.BMP` (48x32) | **the sixteen base wave frames** |

and the resource's `TImageSequenceList` (59 entries, `structseq_probe`) is:

| entry | mode | period | library | what it is |
|-------|------|--------|---------|------------|
| 0 | 0 | - | `WATERHEX.ILB` | the static base + shelf library |
| 1..19 | - | - | - | NULL placeholders |
| 20..29 | 4 | 2 | `WATERANM.ILB` | the five wave/ripple frame tables, shipped twice |
| 30..40 | 4 | 0 / 1 | `WATERANM.ILB` | `Fish1-00..17` chained to `FishSplas1-00..04` chained to `FishSplas1-05..11` |
| 50..58 | 4 | 0 / 1 | `TCOMBAT\OBJECTS\SEFFECT.ILB` | the sparkle effects |

`WATERANM.ILB` holds `Water000..066` (slots 0..66), `Fish1-*` (100..117) and
`FishSplas1-*` (120..131). Every entry here is ILB class **0x16** (TSprite16),
so the draw goes through the same **brighten** blend as a building's glow
(`dst = max(dst, src)`; `docs/17_BUILDING_ANIM.md` section 5b) - a water overlay
can only light the sea up, never darken it. One shipped frame-table cell in the
SEFFECT tables names an out-of-range slot (2324); the engine's `ImageExist` test
and the port's bounds check both drop it.

### A timeline (entry 20, the first ripple)

`ft = [10 x7, 47..57]` with `period = 2`, so one table cell lasts `period+1 = 3`
ticks:

```
counter  -20..-1   asleep (no draw; the counter just counts up)
counter   0..20    7 blank cells -> frame 10 (the 0-byte placeholder)
counter  21..53    frames 47..57 -> the wave crests, 3 ticks each
counter  54        every sequence returns -1 -> park at -(0x14..0x2c)
                   ... and the cycle starts again
```

So a water hex shows ~1.1 s of moving wave crest after a ~0.7 s blank lead-in,
then sleeps 20..44 more map frames (0.67..1.47 s at the engine's 30 Hz map
frame). Entry 30 (the fish) is 30 ticks / 1 s: the fish for 18 ticks, its splash
overlapping from tick 6, the big splash from tick 16.

## 4. What drives it, and the flags that are still unknown

The counter is bumped by the *scene draw*, i.e. once per rendered map frame
(`HSEngine.THSMap.NewFrame@5560d758` bumps the map clock on the same frame).
Four flags gate it; three are not identified beyond what a running world map
does:

| flag | meaning | port |
|------|---------|------|
| `AoWEngine+0x6d` | the engine's mode byte; `1` is the world map (and selects the longer sleep, `0x14` vs `10`), the title screen reads `2`, `00455f58` writes `1`/`3` | `WaterAnimOptions::dynamic` / `engine_mode` (default `true` / 1) `[TODO(RE)]` |
| `((map+0x188) -> +0x10) & 0x10` | picks roll branch B, and with it the 30 % SEFFECT sparkle | `WaterAnimOptions::wide_range` (default false) `[TODO(RE)]` |
| `map+0x3c & 2` | clears the extra fish / sparkle rolls when set | `WaterAnimOptions::effects_ok` (default true) `[TODO(RE)]` |
| `sprite+0x1a` / `<<level flag>>` | the `ShowDynamic` gate itself | `variant_present` / `level_anim` |

A normal surface game is `dynamic = true, engine_mode = 1, wide_range = false,
effects_ok = true`, which is what the port defaults to: the *ripples* (the common
case, `0x14..0x1d`) are exact and the fish appears with the engine's
1-in-700-per-roll probability, while the sparkle branch is only reachable
through a flag the port does not model yet.

## 5. The port

* **The rules** - `src/engine/map_resources.cpp`: `water_hex_create`
  (`Create@5579ad40`), `water_hex_advance` (`ShowDynamic`'s first half:
  increment, roll at exactly 0, otherwise report the entry + counter),
  `water_sequence_roll`, `water_hex_finish` (the `if (!drew)` park),
  `water_hex_animates` (the gate) and the phase helpers (`water_base_frame`,
  `water_base_phase`, `water_hex_variant`, `water_shore_variant`).
* **The frame resolver** - `aow::image_sequence_unlooped_frame`
  (`src/engine/unit_anim.cpp`) is `GetUnloopedFrameIndex` verbatim. Note that the
  port's "drew" verdict is the *resolver's*, not "a sprite was blitted" - the
  `(*)` note in section 3 is why.
* **The renderer** - `aow::draw_water_overlays` (`src/engine/mapviz.cpp`) walks
  every water/ice hexagon, advances its counter to the requested map frame, and
  composites each sequence of the rolled entry's chain at
  `hex_origin(x,y) + seq.xoff/yoff + sprite base+shift` with `water_blit`
  (brighten for class 0x16, alpha composite otherwise, destination alpha
  preserved). The per-hex state is cached by game root + level + a signature of
  the level's terrain, and a call with a *smaller* frame rewinds it, so a
  one-shot render, a test and the live front-ends all get the same deterministic
  result.
* **The static half** - the terrain pass draws the base tile from
  `aow::water_base_frame(x, y)` (slots 0x8c..0x9b), the coast pass draws the
  `T_Ic-Wa2` foam for the new `foam` mask, and the shelf's phase is
  `aow::water_shore_variant(x, y)`.
* **Baking vs. live** - `MapVizOptions::clock` with `no_animated_overlays =
  false` bakes the water animation into a one-shot render at that map frame;
  frame 0 (the default) draws nothing, because every hexagon is still asleep
  right after `Create`. With `no_animated_overlays = true` a live front-end calls
  `draw_water_overlays` itself every map frame, passing its **viewport as a
  clip** so the rectangle it restores from its pristine canvas stays bounded
  (`src/game/viewer.cpp` `draw_structure_anim`, `src/game/menu.cpp`
  `mv_draw_structure_anim`). Every hexagon's counter is advanced regardless of
  the clip - measured at **0.70 ms** per clipped live frame on the 1560x1552
  Tutorial level (the test prints it), against ~28 ms for a level re-render.
  Water is composited **before the armies**: `draw_unit_overlays` runs last, so a
  ripple (or a sparkle) never covers a unit standing on the water's hex
  (`docs/13_UNITS_ON_MAP.md`).


## 6. Tests, tools and proofs

* `src/tests/mapres_test.cpp` (37 new checks):
  * the `GetUnloopedFrameIndex` table for every mode (0/1/4, the refused
    2/3/5, a negative tick);
  * the shipped water/ice resource records entry by entry: the 59 entries, entry
    0's static `WATERHEX.ILB`, entry 20's mode-4 `WATERANM` ripple at `(-2,-6)`
    and its 18-cell table, the five tables shipped twice, the fish chain's three
    sequences and frame ranges (`Fish1-00..17` -> `..124` -> `..131`), entry
    50's `SEFFECT.ILB`, and `ICEHEX.ILB`'s single static sequence;
  * the phase helpers' ranges, the `>= 12 of 16` wave-phase spread over a 16x16
    block, and the exact `((phase + x + y*2 + variant) & 0xf) + 0x8c` rule;
  * the roll: the narrow branch's ripple/fish-only outcome, "no fish on a hex
    with a land neighbour", and the wide branch's sparkles + fish;
  * the gate (`engine+0x6d == 0` freezes the counter);
  * the exact ripple lifetime (`3 x frame-table` ticks), the first ripple's
    start frame (`-counter0 - 1`) and the sleep range (`0x14 + RandInt(0x19)`);
  * determinism (a replay is bit-identical);
  * the integration: nothing composites at map frame 0, a ripple is up within 60
    frames, **0** darkened and **0** alpha-changed pixels (the brighten blend),
    a clip with no water draws nothing, and the live path's per-frame cost;
  * the terrain debug CSV: every water base tile is a `H_Wa2` frame in
    0x8c..0x9b, the ocean uses >= 12 of the 16 slots, Tutorial L0 (no ice) draws
    **no** foam at all, and Icedale L0's water/ice boundary draws 84 `T_Ic-Wa2`
    sprites (from `WATERHEX.ILB` *and* `ICEHEX.ILB`) while its 302 ice tiles all
    draw `H_Wa2`-style frames in 0x8c..0x9b.
* `aow_view --clock N --render out.png` bakes the animation at map frame `N`.
  `bin/WaterAnimView_c0.png` / `WaterAnimView_c24.png` are the full-level proofs
  and `bin/WaterAnim_c0.png` / `WaterAnim_c24.png` the 260x300 ocean crops of the
  same pair (map frame 0 vs 24: the wave-phase field is identical, the moving
  crests are not). `bin/WaterFoam_Icedale.png` is a 300x260 crop of Icedale L0's
  water/ice boundary - the shelf, the foam and the ice tiles side by side.
* `python tools/structseq_probe.py ..\Release\Release.hss 0x2073e` dumps the
  resource's sequence list - the source of the table in section 3.

## 7. Honest limitations

* **The roll's exact picks cannot be reproduced.** The engine seeds them from
  the map's own RNG (`System.RandInt`, seeded when the scenario starts and not
  stored in the `.HSM`), so *which* ripple a given tile plays and *when* differ
  from a real session. The port stands in a deterministic positional value for
  every `RandInt` and for the two address-derived phases (`field >> 3`,
  `this >> 8 & 3`) - the same substitution it already makes elsewhere. What is
  exact: the cadence (one counter step per rendered map frame), the ranges and
  probabilities, the sequence/frame resolution, the entry lifetimes, the anchors,
  the blend, the gate and the pause lengths.
* **`engine+0x6d`, `map+0x3c & 2` and the `map+0x188` flag are not identified**
  beyond "the world map has 1 / false / true". If a real session ran with
  `engine+0x6d == 0` the engine would animate nothing at all; the port models
  that (`WaterAnimOptions::dynamic = false`) but never chooses it.
* **The base tile's per-hex phase is positional, not address-derived**, so the
  *pattern* of wave phases over the ocean differs from the original even though
  the frame set and the rule are the same. The same applies to the shelf's
  `this >> 8 & 3` variant.
* **The shelf's snow/wasteland slot offset is not applied.** The variant bytes
  (`sprite+0x1a..0x1f`: 0x28 for a snow neighbour, 0x50 for wasteland) are
  computed, but the shelf is still drawn from the `t_wa-gr` slots rather than
  shifted into `t_wa-sn` / `t_wa-wl` - the port draws the one shelf layer whose
  slots the grass case selects.
* **The animation state is not serialised** in a scenario. The engine keeps the
  counters on the hexagon sprites, so a real saved game resumes mid-ripple; the
  port always starts every hexagon at `Create` (map frame 0 of the session, or
  the `MapVizOptions::clock` it is asked for).

