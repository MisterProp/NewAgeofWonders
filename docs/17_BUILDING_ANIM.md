# 17 - The building (structure) sprite animation

This document records the reverse-engineering of **how Age of Wonders 1 animates a
world-map building** - the image-sequence *chain* a structure resource draws, the
map clock and the per-object phase `ImageLib.TImageSequence.GetFrameIndex` is
handed - and its 1:1 reconstruction in `src/engine/mapviz.cpp`
(`MapVizOptions::clock`, `MapVizOptions::no_animated_overlays`,
`draw_structure_overlays`), `src/engine/map_resources.cpp`
(`aow::object_anim_phase`), the live front-ends (`src/game/viewer.cpp`,
`src/game/menu.cpp`) and the tests in `src/tests/mapres_test.cpp`.

Everything was recovered with **GhidraMCP** (`tools/ghidra/`) from
`HSEPack.dpl`, `AoWEPACK.dpl` (image base `0x55700000`) and `ILPACK.dpl`
(`0x55200000`); the decompiles live under `re/exports/`. The raw sequence blobs
of every shipped structure can be dumped with
`python tools/structseq_probe.py Release\Release.hss [class_hex]`.

## 1. A structure draws a sequence CHAIN, not one sprite

A structure (Mine, Tower, Wizard's Tower, Shipyard, Node, Farm, Teleporter, ...)
is an `AoWE.TStructure` (a `TMultiHexMO` subclass). Its `Show` converts the hex
with `HSEngine.HXtoHP` and hands the raw position to its *resource*'s Show
(`AoWE.TStructure.Show@5575f958` -> `AoWE.TStructureResource.Show@557605dc`):

```c
// AoWE.TStructureResource.Show@557605dc (normal branch)
HSEngine_TMapObjectResource_ShowComplexImage_23EDC2EF(
    resource, x, y,
    (int)*(char *)(MO.field + 0x10) + *(char *)(MO.field + 0x11) * 0x10,  // phase
    MO[5],                                                                  // cell image
    flags);
```

`HSEngine.TMapObjectResource.ShowComplexImage@5560a134` starts the chain at
`resource->seqlist.items[MO+0x14]` (the object's **cell image** - its state index)
and walks each sequence's `.next` (`+0xc`):

```c
iVar3 = seqlist.items[MO[5]];
for (; iVar3 != 0; iVar3 = *(int *)(iVar3 + 0xc)) {
    frame = ImageLib_TImageSequence_GetFrameIndex(iVar3, map->clock /*+0x40*/, phase);
    if (frame != -1)
        Get(seq->library /*+0x10*/, frame)->Show(anchor_x + seq->xoff /*+0x24*/,
                                                 anchor_y + seq->yoff /*+0x28*/, flags);
}
```

So a structure's sprite is **the whole chain**, not the first library named in
the resource blob: a static *body* plus one or more *animated* overlays. Two
examples from the shipped data (`tools/structseq_probe.py`):

```
Mine  (class 0x2037b, id 0x17)   [1] mode 0  base 0  (-16,-22) STR_MINE.ILB
                                     -> mode 5  10..15 (-16,-22) STR_MINE.ILB   (sparkle)
WizTower (0x2089f, id 0x5d311a5) [1] mode 0  base 0  (-10,-90) STR_WTWR.ILB
                                     -> mode 4  10..21 (-11,-89) STR_WTWR.ILB   (glowing orb)
Teleporter (0x2019b)             [0] mode 0 ..     + four mode-4 chains (shimmer)
Node / Farm / Elven cave / Grove [0] mode 4 (an animated BODY)
```

`aow::body_sequence` (`src/engine/map_resources.cpp`) picks the first non-effect
sequence of that list (the port's stand-in for `MO+0x14`), and
`aow::image_sequence_chain` returns the body followed by every `.next`
(`aow/unit_anim.hpp`). The renderer now draws **all** of them - that is what adds
the animation. (Phase 4f had already made the *body* render; the chain was
dropped, so a mine or wizard tower was drawn static.)

## 2. The frame: `TImageSequence.GetFrameIndex` + the map clock

`ImageLib.TImageSequence.GetFrameIndex@0x552186ec` resolves one sequence's frame
from the map clock and the phase (see docs/16_UNIT_ANIM.md for the record and the
per-mode rule; the port's `aow::image_sequence_frame` implements modes 0/1/2/4):

| mode | frame |
|------|-------|
| 0 | the stored base frame (static) |
| 1 | `(counter + phase) % library_count` |
| 2 | `abs(((counter + phase) % ((N-1)*2)) - (N-2))` |
| 3 | a random frame every `period + 1` phases |
| 4 | `frame_table[(counter + phase) % table_count]` |
| 5 | a random frame-table entry |

`counter` advances once every `period + 1` clock ticks; `library_count` is the
linked ILB's image count. The **map clock** is `AoWHSMap+0x40`:
`HSEngine.THSMap.NewFrame@0x5560d758` adds one per map frame (wrapping at
`0x100000`) - the same clock the unit poses and the hover pointer animate off.

The **phase** keeps neighbouring buildings out of lockstep. The engine computes it
from the object's hex:

* `AoWE.TStructureResource.Show@557605dc` and `HSEngine.TMapObject.Show@55609a00`
  pass `(int)*(char *)(field+0x10) + *(char *)(field+0x11) * 0x10` = **`x + y*0x10`**
  (`aow::object_anim_phase`);
* `HSEngine.TMultiHexMO.Show@5560aaec` passes the `HXtoHP` *pixel* sum
  (`px + py`) instead.

The anchor is `HXtoHP + seq.xoff/yoff + sprite base+shift` (the sequence offsets
are authoritative; the multi-hex `dXmainP/dYmainP` belongs to the *single-image*
branch only).

## 3. The port

`src/include/aow/mapviz.hpp`:

* `MapVizOptions::clock` - the map animation clock `AoWHSMap+0x40`. A one-shot
  render bakes each sequence's frame at this clock; **0** is the deterministic
  default (a head-less render stays reproducible).
* `MapVizOptions::no_animated_overlays` - when true the renderer leaves the
  **animated** sequence frames out (the body or chain sequences whose mode is not
  0), so a live front-end can composite them itself each map frame.
* `MapVizOptions::no_units` - when true the placed armies are left out too (bodies
  *and* ground shadows), because a live front-end composites them **last**, after
  every building overlay and the water: see `draw_unit_overlays` and
  `docs/13_UNITS_ON_MAP.md`. The engine sorts a unit above a map object
  (`TUnitHS.CalcShowPriority` = 0x68000000), so a unit standing next to a building
  must not be covered by it.
* `draw_structure_overlays(map, res, root, level, clock, rgba, w, h, bgra,
  dirty..., error)` - composites exactly what `ShowComplexImage` would draw for
  the level's structures at `clock`: the sequence chain from the body, every
  sequence whose frame index is valid, at
  `hex_origin + seq.xoff/yoff + sprite base+shift`. It returns the bounding box it
  painted, so the caller can grow its dirty rectangle and restore it next frame.

`src/engine/mapviz.cpp`:

* `pick()` resolves the body **and its chain** into `Picked::layers`, each layer
  framed by `image_sequence_frame` with `opt.clock` and
  `object_anim_phase(x, y)`; the object pass blits the body then the layers.
* the `overlay` rows of `--debug-csv` name each animated overlay's library/frame.

### Why a chain re-composite, not a re-render

The engine re-draws its whole scene every map frame. A level re-render here costs
**~28 ms** (`AoW.exe --game-bench N`), far over the 30 fps budget, so the live
screens keep the static level cached (rendered with `no_animated_overlays` +
`no_units`) and re-composite only the small sprites, in the engine's own layer
order - the buildings and their owner flags, then the water, then the armies:

* **`aow_view.exe`** (`src/game/viewer.cpp`): `rerender()` caches the static level
  in `g.mv.base`; a dedicated map-clock timer (`kMapTimer`, 33 ms) advances
  `g.mv.map_clock` and calls `redraw_overlay()`, which restores the previous dirty
  rectangle and re-runs `draw_structure_anim()` (`draw_structure_overlays` ->
  `draw_water_overlays` -> `draw_unit_overlays`).
* **The in-game screen** (`src/game/menu.cpp`, `AoW.exe`): the level renders
  (`mv_render_level` / `mv_build_contents` / `load_game_map`) set
  `no_animated_overlays` + `no_units`; `game_update()` advances `game_clock` once
  per 30 fps tick and `mv_refresh_draw()` re-composites the overlays into
  `game_draw` (`mv_draw_structure_anim`), exactly where it already re-composites
  the hover trail and the walk sprite.
  `mv_build_contents` is the one exception: it keeps the armies (no `no_units`),
  because the move-selection marker puts the selected hex's *contents* back over
  its far wall - the unit has to come out of that layer.

## 4. CLI + proofs

```
# head-less: bake the building animations at map clock N
aow_view.exe <map.HSM> --root <game-dir> --level 0 --clock 0  --render c0.png
aow_view.exe <map.HSM> --root <game-dir> --level 0 --clock 12 --render c12.png
# (c0.png != c12.png; --debug-csv names the animated overlay rows)

# the in-game screen: --game-anim N advances the map clock N ticks,
# --game-clock N forces it, --mouse drives the edge scroll to a structure
AoW.exe --root <game-dir> --screen game --no-music --no-cursor \
        --mouse 427 419 --game-anim 44 --game-clock 0  --render g0.png
AoW.exe --root <game-dir> --screen game --no-music --no-cursor \
        --mouse 427 419 --game-anim 44 --game-clock 12 --render g12.png
```

`build.ps1 -Test` runs a **building animation smoke test** that renders both views
at clock 0 and 12 and asserts the PNGs differ, keeping
`bin/BuildAnimView_c0/c12.png` and `bin/BuildAnimGame_c0/c12.png` as proofs, and
regenerates the checkerboard sheet `bin/buildings_sheet.png`.
`mapres_test` adds 20 checks: the phase formula, the mine's mode-5 sparkle chain,
the wizard tower's mode-4 orb chain, the overlay composite changing with the clock,
the `overlay` CSV rows, the class-0x16 colour keys, and the per-building audit
(127 structures, 177 frames, 0 opaque).

Verified on `Tutorial.hsm` level 0: at clock 0 the wizard tower's orb is
`STR_WTWR.ILB` `wtwr05.bmp` and at clock 12 it is `wtwr07.bmp`; the teleporter's
five shimmer sequences cycle `str_tele2.bmp` frames 11/8/7/6/7 -> 10/11/11/11/10.

## 5. The class-0x16 colour key (removing the backgrounds)

A building's *animated* frames are `TSprite16` (class `0x16`, raw RGB565): the
mine's sparkle is `STR_MINE.ILB` frames 10..15, the wizard tower's orb is
`STR_WTWR.ILB` frames 10..21, the gold glitter `MISC_OBJ.ILB` frames 10..15. A
`TSprite16` has **no mask**: the engine always blits it colour-keyed -
`ImageLib.TSprite16.ShowOpaque@0x5521193c` (the vtable+0x84 draw entry
`ShowComplexImage` uses) calls `GFXE.BltColorKey16@0x55202050`.

The key is the **u16 in the descriptor right after the shift fields** - the same
slot class 0x11/0x12 use for their RLE escape word, i.e. `c + 16` in
`ilb.cpp`'s parser (`ImageLib.TSprite16.ReadImageData@0x5521172c` reads width,
height, xshift, yshift and then that field). Verified against each sprite's
actual background:

| sprite | stored key | background |
|--------|-----------|------------|
| `Cursors.ilb` id 0 (`Pointers2.BMP`) | `0x418C` (65,48,98) | purple box |
| `General.ILB` `3dHex.BMP` | `0x05A9` (0,182,74) | green |
| `Str_mine.ILB` frame 10 | `0x0000` | black |
| `Str_wtwr.ILB` `Wtwr00.BMP` | `0x0000` | black |

`IlbEntry::color_key` / `has_color_key` carry it and `decode_rgb` writes alpha 0
for those pixels, so every consumer (the map renderer, the viewer, the in-game
screen, the menu) gets a background-free sprite. `0x0000` is a legitimate key, so
`has_color_key` (set for class `0x16`) gates it - unlike class `0x10`
(`TPicture16`), whose `ShowOpaque@0x5520ef4c` has no colour key and stays opaque.

**The stale-key fallback.** A few shipped frames - `MISC_OBJ.ILB`'s gold glitter
and `EPL_DNG2.ILB`'s dungeon mouth, the blended glows drawn over black - store a
key that does not occur in the bitmap at all, so keying it would remove nothing.
When the stored key removes zero pixels the sprite's dominant border-ring colour
is keyed instead (the rule the menu screens already used for a mask-less sprite);
a full-bleed picture has no dominant border and is left untouched. `mapres_test`
audits this per building:

```
building audit: 127 structures, 177 frames, 56 keyed, 0 opaque
```

`tools/buildings_sheet.py <game-root> <out.png>` lays every building sprite out on
a checkerboard, so a sprite that still carries its background is obvious at a
glance (`bin/buildings_sheet.png`).

## 5b. The glow BLEND (why an opaque blit looked wrong)

Keying the background was only half the fix. `ShowComplexImage` draws each chain
sequence through the image's **vtable+0x84** slot, and for a mask-less
`TSprite16` that slot is *not* the colour-key blit - it is
`ImageLib.TSprite16.ShowTransparent@0x55211ea0`, which fetches a GFXE blend table
(`GFXE.TLookupBlendTableCollection100.GetLookupTables`) and calls
`ShowBlended@0x55211b64` with it. That table is the per-channel **brighten**
table (`GFXE.GenerateBrightenBlendTable`, i.e. `dst = max(dst, src)` - the same
table the hexagon selector is painted with). A glow sprite therefore leaves the
terrain untouched where it is dark and *lights it up* where it is bright; an
opaque blit instead painted its dark surround over the map - the black discs.
(Only the `map+0x40 & 6 == 6` "cached static scene" branch uses the opaque
`+0x9c` slot, which the port does not model.)

`mapviz` now blits a class-0x16 sprite with `blit_brighten` (`max` per channel;
`blit_sprite_brighten_buffer` for the live front-ends) and every other class with
the normal alpha blit (their RLE mask is exact).

**Measured against the original game.** `docs/refs/glow_ref.png` is a 250x147
crop of the real AoW1 showing `Scenario\Custom\Bloody Marauders.hsm` level 0 - a
mine sparkle, a dungeon mouth (`EPL_DNG2`) and a river ford, all glows.
`tools/glow_bench.py` renders the same map, aligns the screenshot against the
render (offset 1572,1323) and reports the mean |delta|:

| blend of the class-0x16 overlays | glow pixels (mean \|delta\|) |
|-----------------------------------|------------------------------|
| opaque blit (before) | 33.1 |
| 50% lerp | 37.2 |
| additive | 57.8 |
| **brighten `max(dst,src)`** | **12.5** |

The screenshot's own colour conversion leaves a floor of ~5 even on pixels that
are pure terrain, so 12.5 is close to the achievable limit; the whole-crop
residual is 6.07. `build.ps1 -Test` runs the benchmark and fails if the glow
residual regresses past 16. The comparison sheet it writes is
`bin/glow_bench.png`; `mapres_test` asserts the property directly (the overlays
brighten more pixels than they darken, and the mean change is positive).

## 6. Out of scope / `TODO(RE)`

* **The object's cell image `MO+0x14`.** `ShowComplexImage` starts the chain at
  the object's *state* index (built / under-construction / razing), which is not
  serialised; the port still uses `body_sequence` = "the first non-effect
  sequence". That matches `STR_MINE` / `STR_TOWR` / `STR_WTWR` / `PRO_SYRD` but
  does not yet draw a structure's razing / building-up state.
* **Additive effects.** A chain that *opens* with an additive effect
  (`SMOKE.ILB`, `EFFECTS\TWINKGR.ILB`) is still skipped - a plain opaque blit of
  those leaves a dark square, and the port has no blend-mode pass. A producing
  mine's `SMOKE.ILB` puff is therefore not drawn (its in-library sparkle is).
* **Modes 3 and 5** pick a frame pseudo-randomly in the engine (GetFrameIndex
  cases 3/5 call `RandInt`, seeded by the map's own RNG, which the port does not
  model). The port *cycles* the library / frame table deterministically - the
  same `period + 1` step, every frame shown, just not shuffled (a frozen frame
  would not animate at all, and a random pick can never be reproduced).
* **The minimap** (`Scene::game_px`, the scanner's source) keeps the *static*
  level, so the scaled-down scanner does not show the animated overlays.
* **The city owner flag** (`AoWE.TCity.Show@557aca88` ->
  `AoWE.TFlagControl.ShowFlag@5575a1e8`, drawn at `resource+0x68/+0x6c` with a
  `map_clock + x` value) - **done, and it covers every ownable structure, not
  just a city**: see docs/19_STRUCTURE_FLAGS.md (the flag is the owner's *race*
  flag, blitted behind the building, animated off that same clock, and
  re-composited per map frame together with the overlays above).

