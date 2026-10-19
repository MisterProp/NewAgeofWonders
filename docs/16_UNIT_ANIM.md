# 16 - The unit sprite animation (the world-map walk)

This document records the reverse-engineering of **how Age of Wonders 1 animates a
unit sprite** - the image-sequence records inside `Unitgfx.pfs`, the direction and
frame rules `AoWE.TUnitHS` uses to draw a unit, and the sub-hex walk a moving army
plays - and its 1:1 reconstruction in `src/include/aow/unit_anim.hpp` +
`src/engine/unit_anim.cpp`, the `aow::unit_pose_rgba` / `aow::unit_settle_pose_rgba`
hooks in `src/engine/mapviz.cpp`, the walk animation in `src/game/viewer.cpp` and
`src/game/menu.cpp`, and the tests in `src/tests/unit_anim_test.cpp`. Section 10
covers the unit's **ground shadow** (`AoWE.TUnitShadowHS`) and its benchmark
against the original game's screenshot.

One place where the port deliberately goes further than the engine is the
walk -> standing hand-off (section 7): the engine swaps the two poses in a single
frame at whatever point of the cycle the clock happens to be, which reads as a
lurch - "right before arriving at the final position it stops a little and then it
dashes". The port aligns the walking cycle with the move so the stride finishes at
the destination, and settles into the standing pose over the last few frames.

Everything was recovered with **GhidraMCP** (`tools/ghidra/`) from `ILPACK.dpl`
(image base `0x55200000`) and `AoWEPACK.dpl` (`0x55700000`); the decompiles live
under `re/exports/anim/`. A head-less probe of the raw sequence blob lives in
`tools/unitseq_probe.py`, and the port exposes the parsed result as
`aow unitseq Release\Unitgfx.pfs [res_index|all]`.

## 1. The record: `ImageLib.TImageSequence`

A unit's sprite is an image library (`UNITS\<race>\<unit>.ILB`). The Unitgfx
record that names it (rw tag 6) also carries a **`TImageSequenceList`** at rw tag
5 - the animation poses. `ImageLib.TImageSequenceList.LoadFromStream@0x552190d8`
reads it as:

```
u32 count
then, count times:
    u8 flag          # 0 => a TImageSequence follows; anything else => a NULL slot
    (TImageSequence) # only when flag == 0
```

`ImageLib.TImageSequence.LoadFromStream@0x55218a60` reads one sequence:

| order | field | engine offset | meaning |
|-------|-------|---------------|---------|
| 1 | `rwString` library | `+0x14` | the ILB this pose draws from |
| 2 | `u8` | `+0x08` | (unused by the port) |
| 3 | `u8` period | `+0x18` | clock ticks between pose frames, **minus 1** |
| 4 | `u8` mode | `+0x19` | 0 static, 1 loop, 2 ping-pong, 3 random, 4 frame table, 5 random frame table |
| 5 | `u32` base frame | `+0x20` | the frame of a static pose |
| 6 | `i32` xoff | `+0x24` | added to the hex's `HXtoHP` |
| 7 | `i32` yoff | `+0x28` | added to the hex's `HXtoHP` |
| 8 | frame table | `+0x1c` | only for mode 4/5: `u32 seed`, `u32 count`, `count * i32` frames |
| 9 | `u8` has chain | - | when set, one more `TImageSequence` follows (`+0x0c`) |

(`FUN_5520d1e0` is the `rwString` reader: a `u32` byte length, then the bytes.)

The **world map uses the same record**: every `TMapObjectResource` carries a
`TImageSequenceList` in its rw tags 9/10, and
`HSEngine.TMapObjectResource.ShowComplexImage@5560a134` starts the chain at the
object's cell image and walks `.next` (as does the sequence branch of
`AoWE.TStructureResource.Show@557605dc`). There the entries are normally **NULL
placeholders** (`flag != 0`, indexed by the object's state) and the sprite lives in
one entry's chain: e.g. the Shipyard's entry 1 is `SMOKE.ILB` (an additive effect)
chained to its dock `PRO_SYRD.ILB`. `aow::body_sequence`
(`src/engine/map_resources.cpp`) walks entries-then-chains and skips effect
libraries, which is what made ports/shipyards render; see
`docs/ROADMAP_mapviewer_fidelity.md` Phase 4f.

## 2. The shipped 31-pose layout

Every shipped unit uses the same 31 slots. Parsed from `Release\Unitgfx.pfs`:

```
[ 0.. 5]  standing pose, facing 1..6     (mode 4, a one-frame table: the pose)
[ 6..11]  walking cycle, facing 1..6     (mode 4, an eight-frame table 1..8)
[12..17]  a second pose, facing 1..6     (mode 4)
[18..23]  a tail pose, facing 1..6       (mode 4)
[24..29]  the ground shadow (UNITS\USHADOW.ILB), facing 0..5
[30]      the corpse (UNITS\DEADUNIT.ILB)
```

The pose art is banked by **hundreds**: facing 1 -> `4xx`, facing 2 -> `3xx`,
facing 3 -> `2xx`, facing 4 -> `1xx`, facing 5 -> `6xx`, facing 6 -> `5xx`. For
`UNITS\HMN\HMNAR.ILB` (Human Archer, gfx_index 0):

```
seq 0  (stand dir 1) frames: 409
seq 3  (stand dir 4) frames: 109
seq 6  (walk  dir 1) frames: 401 402 403 404 405 406 407 408   period 1, xoff -16 yoff -24
seq 8  (walk  dir 3) frames: 201 202 203 204 205 206 207 208
seq 24 (shadow)      UNITS\USHADOW.ILB base 1
seq 30 (corpse)      UNITS\DEADUNIT.ILB frames 2 3
```

Across the 182 shipped records: 156 have 31 sequences, 13 have 30, 13 have 24;
every record's slots `0..11` exist and name the unit library, so every unit can
walk. `aow::parse_image_sequence_list` reproduces this (`unitseq` prints it), and
`unit_anim_test` locks the Human Archer block.

## 3. `AoWE.TUnitHS.Show` picks the pose

`re/exports/aowepack/AoWE.TUnitHS.Show.c` (0x55784614):

```
assert TUnitHS+0x14 < sequence-list count
HXtoHP(field.x, field.y) -> anchor
if TUnitHS+0x15 == -1:                      # standing
    seq = list[TUnitHS+0x14 - 1]
else:                                        # moving
    HNtoHP(TUnitHS+0x14) -> one-hex pixel delta
    if (flags & 0x10) == 0:                  # not a "no-offset" move
        anchor += round(delta * (TUnitHS+0x15 / GetMoveSteps()))
    if (flags & 0x08) == 0:                  # not paused
        seq = list[TUnitHS+0x14 + 5]
    else:
        seq = list[TUnitHS+0x14 - 1]
for s in seq and its chain (s+0x0c):
    frame = TImageSequence.GetFrameIndex(s, AoWHSMap+0x40, (ptr>>2)&0xffff)
    if frame != -1: image(s.library, frame).Show(anchor + s.xoff, anchor + s.yoff)
```

* **`TUnitHS+0x14`** is the facing. `TUnitHS.Create@0x55783da4` leaves it 3;
  `TUnitHS.SetDirection@0x5578480c` overwrites it (and cancels a running move when
  it changes). `AoWE.TMoveTE.GetDirection@0x5574771c` computes it from a path
  step with `HSEngine.dHXtoRad` + `HSEngine.dHXtoHNfast@0x5560e338` - exactly
  `aow::hex_direction`.
* **`TUnitHS+0x15`** is the move progress. `0xFF` = standing; otherwise it runs
  `0..GetMoveSteps`. `TUnitHS.SetMoveStatus@0x557848c0` sets it to
  `(GetMoveSteps * percent + 50) / 100`; `MoveFirst@0x55784830` resets it to 0;
  `MoveNext@0x55784910` adds 1 per animation frame; `CancelMove@0x55784978` puts
  it back to `0xFF`.
* **`GetMoveSteps`** (vtable `+0x120`, `0x55784988`) is the animation frames one
  hex takes, from the unit-gfx resource's rw tag 10 (`TUnitGFXResource+0x30`):
  `0 -> 12, 1 -> 10, 2 -> 8, 3 -> 6, else 8`. `aow::unit_move_steps`.
* **The shadow** is drawn by `TUnitHS.ShowShadow@0x55784504` from
  `list[dir + 0x17]` (dir 1..6) or `list[0x1b]` (dir 0) - slots 24..29, the
  shared `UNITS\USHADOW.ILB` art. The port draws it too (section 10).

`unit_sequence_index(dir, moving)` = `dir - 1` / `dir + 5`, defaulting to 3 for
an out-of-range facing.

## 4. `TImageSequence.GetFrameIndex`

`re/exports/anim/Ilpack_dpl/0x552186ec.c`. The engine keeps a per-sequence frame
counter (`+0x34`) that advances once every `period + 1` clock changes (a `+0x30`
countdown reloaded from `+0x18`), and reads:

| mode | result |
|------|--------|
| 0 | the stored base frame (`+0x20`) |
| 1 | `(counter + phase) % library_count` |
| 2 | `abs(((counter + phase) % ((N-1)*2)) - (N-2))` |
| 3 | a random frame every `period + 1` phases |
| 4 | `frame_table[(counter + phase) % table_count]` |
| 5 | a random frame-table entry |

`library_count` is the linked ILB's image count (`library + 0x44`); `phase` is
`(unit_ptr >> 2) & 0xffff`. With a fixed frame rate the counter is simply
`clock / (period + 1)`, which `aow::image_sequence_frame` uses (no mutable state).
The world-map poses are all **mode 4**: walking cycles of 8 frames with
`period 1` (the frame holds two clock ticks), standing poses of 1 frame.

## 5. The sub-hex walk

`HSEngine.HNtoHP@0x5560e448` = `HNtoHX(dir)` then `HXtoHP` then `x -= 8`, i.e. the
one-hex pixel delta of a direction. `TUnitHS.Show` offsets the anchor by
`round(delta * progress / GetMoveSteps)`, so the sprite slides across the hex as
`+0x15` climbs. `aow::WalkAnimation` reproduces the whole sequence:

* `start(steps, steps_per_hex)` - `MoveFirst`: facing = the first step's
  direction, progress 0;
* `tick()` - `MoveNext`: progress +1; on reaching `steps_per_hex` the next step
  begins with a fresh facing;
* `t()` - the `progress / steps_per_hex` interpolation used to place the sprite;
* `direction()` / `final_direction()` - the facing bytes.

The port places the sprite at `hex_origin(from) + (hex_origin(to) -
hex_origin(from)) * t`, which is exactly `HXtoHP` interpolation (`hex_origin` IS
`HXtoHP`), plus the sequence's `xoff/yoff` and the pose's own `base+shift`
(`aow::unit_pose_rgba`).

## 6. The front-ends

* **`aow_view.exe`** (`src/game/viewer.cpp`): a click relocates the army's
  `TArmyHS` immediately, re-renders with the destination hidden and starts a
  `WalkAnimation`. Its 33 ms timer advances `kAnimFramesPerTick` frames: it bumps the map
  clock and calls `tick()`, then `draw_anim_sprite` blits
  `unit_pose_rgba(dir, moving)` at the interpolated anchor. When the walk ends,
  `finish_move` re-renders (the army is drawn on its hex) and re-selects it.
* **The in-game screen** (`src/game/menu.cpp`, `AoW.exe`): `game_move_selected`
  relocates the army, hides it on the destination (`MapVizOptions::hide_army_*`),
  starts the same `WalkAnimation` and re-selects it (so the title/log already show
  the new hex). Each 30 fps timer tick advances one frame in `game_update`, which
  recomposites `game_draw` with the moving pose. `--game-anim N` advances N frames
  head-lessly so the walk's proof PNG can be taken mid-step.

The pose for a step uses the **unit's own** library and sequences, so every unit
walks with its personal art and faces the hex it is entering.

The cycle itself is framed off the *global* map clock, so which of its 8 poses is
on screen when a move ends has nothing to do with the move: a walk is normally
cut off mid-stride and the standing pose then replaces that pose in a single
frame (section 7). Both front-ends therefore *choose* the walk's phase
(`aow::walk_settle_phase`, aligned with the path so the stride finishes at the
destination) and fade the standing pose in over the last `aow::kSettleTicks`
ticks (`aow::walk_settle_weight` -> `aow::unit_settle_pose_rgba`).

One timer tick advances `aow::kAnimFramesPerTick` engine animation frames
(`aow/unit_anim.hpp`). A frame is *both* one `TUnitHS.MoveNext` sub-step (the
travel speed) and one `AoWHSMap+0x40` clock tick (the walk cycle's frame rate),
so this single multiplier scales travel and animation together. It is 2 (the
engine's own rate doubled); set it to 1 to reproduce the engine exactly.

When the walk ends, the move has already written the last step's direction into
the army's facing (`TUnitHS` rw tag 9, `MapHs::army_direction`), so the idle
render that follows draws the **standing pose of that same direction**
(`seq[dir-1]`) - the unit keeps facing the way it arrived instead of snapping
back to a default pose.

## 7. The settle phase: the walk -> standing hand-off

`TUnitHS.Show` frames the walking sequence off the **global** map clock
(`AoWHSMap+0x40`) and a per-unit offset, and the walk's *length* plays no part in
that. So a walk is normally cut off wherever the 8-pose cycle happens to be, and
when the move ends the very next frame draws the *standing* pose: the unit is
swapped from "legs in mid-swing" to "planted" in one frame - the visible "right
before arriving it stops a little and then it dashes" (the pose snaps, the unit
does not move).

Two facts make that fixable inside the port, without touching the travel:

* **The phase is already arbitrary.** The engine's own offset is
  `(unit_ptr >> 2) & 0xffff`, i.e. a per-unit nothing. A front-end that knows the
  whole move up front can simply *choose* it.
* **The art is authored for exactly one hand-off.** For every facing the image
  ids run `<d>01..<d>08` = the walking cycle (`seq[dir+5]`'s frame table) and
  `<d>09` = the standing pose (`seq[dir-1]`) - the standing pose is the image
  that *follows* the cycle's last frame (verified on the shipped
  `UNITS\HMN\HMNAR.ILB`: frames 401..408 walk facing 1, 409 its standing pose).
  Cycle frame 8 is therefore the pose a walk should stop on, and the two are
  placed by the same anchor rule, so the swap is only a pose change.

`aow::walk_settle_phase(total_substeps, frames_per_tick, cycle_length, spacing)`
returns the phase that lands the cycle's **last** pose on the last animation
frame the move draws (`counter = clock / spacing` is `image_sequence_frame`'s own
rule), and `aow::walk_settle_weight(remaining, frames_per_tick)` then ramps the
standing pose in over the walk's final `aow::kSettleTicks` ticks, so the sprite
the front-end swaps to when the walk ends - the level render's standing army - is
already the pose that is on screen. `aow::unit_settle_pose_rgba` (mapviz) draws
that cross-fade: both poses resolved like `unit_pose_rgba`, both at the walker's
interpolated anchor and the *same* facing (a move ends facing the direction of
its last step, `TUnitHS` rw tag 9 / `MapHs::army_direction`, so the blend cannot
drift), composited premultiplied with the weights `1 - settle` and `settle`.

The result is a walk that *finishes its stride* at the destination and settles
into the standing pose over the last ~100 ms instead of snapping. Measured on the
tutorial hero's 3-hex move (`tools/walk_settle_check.py`, which renders every
animation tick head-less and measures the sprite):

| | last walking frame | arrival frame | differing once the step is removed |
|---|---|---|---|
| engine's phase, hard swap | 382 px | 431 px | **101 px = 23.4 %** (a pose snap) |
| settle phase + fade | 431 px | 431 px | **0 px = 0.0 %** (a pure translation) |

The front-end reports what it chose, so a smoke test can lock it:

```
game_move=15,34->17,32 dir=1           # unchanged: the travel is untouched
game_walk=cycle=8 spacing=2 phase=4 settle=7
```

with `phase` the `aow::walk_settle_phase` result and `settle` the cycle index it
aims at (`cycle_length - 1`). Both halves are opt-out-able and independent:
`kSettleTicks = 0` restores the engine's single-frame swap (the stride still
finishes on the cycle's last pose), and a unit whose walking slot is not a
mode-4 frame table (the static poses of `Shadow`, `Incarnate`, `Sargon`, the
`Fire Elemental`/`Fire Sprite`) keeps the engine's own per-army phase.

## 8. Tests + tools

* `aow::parse_image_sequence_list` / `image_sequence_frame` / `unit_move_steps` /
  `WalkAnimation` / `walk_settle_phase` / `walk_settle_weight` /
  `unit_settle_pose_rgba` / `unit_walk_cycle` - `src/tests/unit_anim_test.cpp`
  (83 checks: the mapping, the frame rule per mode, a two-hex walk, the settle
  sweep over every path length and step count, the Human Archer block, the
  walking cycles of the whole shipped roster - every mode-4 walk settles - and
  every facing of a shipped army's walking pose decoding over `Tutorial.hsm`).
* `aow unitseq Release\Unitgfx.pfs 0` dumps the parsed poses of a unit;
  `tools/unitseq_probe.py <Unitgfx.pfs> <off> <size>` parses the raw blob.
* The walk itself is proved head-lessly from the in-game screen:
  `AoW.exe --screen game --game-click 513 141 --game-click 220 216 --game-click
  284 152 --game-anim N --game-clock 0 --no-cursor --render out.png` - `N`
  advances the animation N ticks, so `bin/WalkSettle_a*.png` are distinct frames
  of the tutorial hero's 3-hex move (`N = 0` the army still on its source hex,
  `N = 8` the last stride tick, `N = 9/10/11` the fade - 1/3, 2/3, 1 - and
  `N = 12` settled on the destination).
  `aow_view`'s head-less `--render` draws the path overlay, not the walk.
* `tools/walk_settle_check.py` drives that walk for every tick of the move and
  measures the hand-off from the frames alone (it must be a pure translation):
  it prints the sprite's per-tick travel and the pose change across the arrival,
  and exits non-zero when the unit does not arrive in its standing pose.

## 9. Out of scope / `TODO(RE)`

* Modes 3 and 5 (random poses) cycle their library / frame table deterministically
  at the same `period + 1` step the engine re-randomises on; the engine's
  `RandInt` (seeded by the map) cannot be reproduced. Modes 0/1/2/4 are exact.
  (The unit *shadow* is no longer on this list - section 10.)
* The idle map render draws the *standing* pose of the army's `army_direction`
  (`mapviz::resolve_unit_pose` -> `seq[dir-1]`), so an army that just walked keeps
  facing the way it arrived. A unit that has never moved uses the facing the
  scenario stored in `TUnitHS` rw tag 9 (default 3).
* The engine's exact animation frame rate is not recovered; the port advances
  `kAnimFramesPerTick` animation frames per frame-timer tick (33 ms in the viewer
  / in-game scene), currently 2 so both travel and playback run at double rate.
* The settle phase and fade (section 7) are deliberate departures from the
  engine, which swaps the two poses in one frame. The travel, the cycle's rate
  and the art are untouched; `kSettleTicks = 0` drops the fade, and
  `walk_settle_phase` only re-uses the engine's own (arbitrary) phase.

## 10. The ground shadow (`AoWE.TUnitShadowHS`)

Every army on the world map casts a shadow. It is **not** part of the body pose:
the engine gives the `TUnitHS`/`TArmyHS` a separate scene object,
`AoWE.TUnitShadowHS`, in `TUnitHS.CreateShadow@0x55783e20` (only when the map has
shadows enabled - `AoWHSMap+0x249 != 0`) and destroys it in
`DestroyShadow@0x55783e58`. `TUnitShadowHS.Show@0x55783d80` forwards to the
unit's own vtable `+0x114` slot, so the drawing is `TUnitHS.ShowShadow@0x55784504`
(or the army's `TArmyHS.ShowShadow@0x557912a8` override, which adds the fog /
concealment gates the body has).

```
anchor = HXtoHP(hex) - view
if TUnitHS+0x15 != -1 and (TUnitHS+0x8 & 0x10) == 0:      # moving
    anchor += round(HNtoHP(TUnitHS+0x14) * (TUnitHS+0x15 / GetMoveSteps()))
seq = list[TUnitHS+0x14 + 0x17]        # facing 1..6 -> slots 24..29
   or list[0x1b] when the facing is 0  # 27
for s in seq and its chain (s+0x0c):
    frame = TImageSequence.GetFrameIndex(s, AoWHSMap+0x40, (unit>>2)&0xffff)
    image(s.library, frame).Show(anchor + s.xoff, anchor + s.yoff, dst)
```

* **The anchor is the body's.** Both functions start from `HXtoHP(hex)` and add
  the same sub-hex offset (`HNtoHP(dir) * progress / GetMoveSteps`), so a moving
  army's shadow travels with it and the standing shadow is the body's own
  footprint. The per-sequence `xoff/yoff` and the image's own `base+shift` are
  added inside `Show`, exactly like the pose.
* **The sequence** comes from the same `Unitgfx.pfs` tag-5 list the body uses:
  `aow::unit_shadow_sequence_index(direction)` = `direction + 0x17` (24..29) or
  `0x1b` for a facing of 0. The slots are `UNITS\USHADOW.ILB` (ILB class 0x11,
  16-bit RLE, 27 different shadow sprites). Most shipped records make them
  **mode 0** (a stored base frame), but the flyers - the dragons, the drakes, the
  eagle rider, the wyvern, the giant bat, the goblin bomber, the demon, Astra, the
  fairy - ship a 4- or 6-frame **mode-4 table**, so a flying unit's shadow
  animates off the map clock like a building's overlay. Records with only 24
  sequences have no shadow slot at all, and the Incarnate's and the Black
  Dragon's slots are **NULL placeholders**: `GetImageSequence` returns 0 and the
  draw loop never runs, so those units cast no shadow.
* **The layer.** `TUnitShadowHS.GetShowPriority@0x55783d64` is
  `(unit_priority & 0x3ffffff) + 0x64000000` against the unit's `0x68000000`
  (`TUnitHS.CalcShowPriority@0x557840c8`), i.e. the *same* position with the layer
  dropped from 26 to 25: every shadow is drawn before every body, so the army
  (and the size banner `TArmyHS.Show` draws after its sprite) always sits on top
  of its own shadow. `mapviz` therefore blits all armies' shadows in one pass and
  then all bodies - in `render_map_rgba`'s final pass (the armies are the port's
  **last** world layer, `docs/13_UNITS_ON_MAP.md`) and again per map frame in a
  live front-end (`aow::draw_unit_overlays`, clipped to its viewport).
* **The blend is a 50 % average, not an opaque sprite.** `ShowShadow` uses the
  image's vtable `+0x84` slot; for a class-0x11 image that is
  `ImageLib.TRLESprite16.ShowTransparent@0x55210474`, whose DirectDraw blitter
  (`FUN_55210ec4` for 565, `FUN_55211248` for 555) skips the RLE escape runs and
  writes, for every other 16-bit pixel,

  ```
  dst = ((src & 0xF7DF) + (dst & 0xF7DF)) >> 1
  ```

  i.e. the average of the shadow art and what is already under it (`0xF7DF`
  clears the red/green low bits of both operands so the add cannot carry between
  the 565 fields). `aow::blit_shadow_rgba` / `blit_shadow_bgra` reproduce it on
  the port's 8-bit canvas by narrowing both operands to 565 and widening the
  result back with the ILB decoder's bit replication. It is *not* the `TShadow`
  class (0x14, `GenerateShadowBlendTable`, `dst = 3/4 dst`) - that is the mask
  the **structure** shadows use.
* **`mapviz`** resolves the shadow next to the body (`resolve_unit_shadow`) and
  logs it to `--debug-csv` as a `shadow,<x>,<y>,<unit resource>,UNITS,<frame>,
  <entry>,<w>,<h>,1` row; the walk overlays (`game/viewer.cpp`,
  `game/menu.cpp`) resolve it every animation tick and composite it before the
  moving pose, at the walker's interpolated anchor.

### The benchmark (`bin/ShadowRef.png`)

`bin/ShadowRef.png` is a 66x58 crop of the **original game** on
`Bloody Marauders.hsm` level 0: the Human leader army (the "Human Hero Leader"
gfx record, `UNITS\HMN\HMNHE.ILB`) after it walked one hex **south**, from its
scenario hex (31,45) to (31,46). Facing 4 is therefore the standing pose `seq[3]`
= HMNHE frame 109 (the reference's figure matches it with a mean |delta| of 5.7
over its 490 opaque pixels) and the shadow is `seq[27]` = USHADOW frame 26,
anchored at `HXtoHP + (-1,18)` (a 17x14 sprite - the reference puts its darkest
blob exactly there).

`tools/shadow_bench.py` drives the port's in-game screen into that same state
(`--start-map` + the two clicks that walk the leader south), reads the camera the
port reports, crops the reference's window out of the render and scores the
17x14 shadow footprint:

| crop of the reference's shadow rectangle | mean &#124;delta&#124; |
|---|---|
| no shadow at all (un-shadowed terrain) | 61.0 |
| the shadow art blitted opaque | 33.9 |
| an 8-bit 50 % average | 9.3 |
| **the engine's 565 50 % average** | **7.2** |
| the port's own render (bench; whole crop 22.1) | 12.3 |

The last two differ because the benchmark's crop carries the port's **pre-existing
8 px terrain/object offset** (`Hexagon.THexagon.Show@0x55611d60` and
`HSEngine.TMapObject.Show@0x55609a00` draw at `HXtoHP.x - 8`, while `TUnitHS.Show`
and the port's units use `HXtoHP` itself), so the terrain *under* the shadow is
sampled from a different column; the 7.2 row samples the terrain at the port's own
offset, isolating the blend. The reference crop itself predates the size-banner
work, but the banner is now implemented (`docs/21_ARMY_COUNTER.md`) and sits
above the shadow rectangle, so the shadow measurement is unaffected.

`unit_anim_test` locks the mapping (`unit_shadow_sequence_index`), the shipped
shadow slots (every record's library, the mode-4 flyers, the null and missing
slots) and the reference's own numbers (slot 27 = USHADOW 26 at
`HXtoHP + (-1,18)`, 17x14); `mapres_test` locks the level render (`shadow` CSV
rows for Tutorial L0 and the leader's
`shadow,31,45,36,UNITS,7,ushadow.bmp,26,13,1`).
