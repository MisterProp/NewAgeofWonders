# Handoff prompt: make `aow_view` a high-fidelity AoW1 map renderer

> Copy everything below the line into a fresh agent session. It is written to be
> self-contained.

---

## Mission

You are working in the **Age of Wonders 1 re-implementation port** at
`C:\GOG Games\Age of Wonders\port` (Windows, PowerShell, MSYS2 `g++` 15.2, **no
CMake installed** — use `build.ps1`). The current map viewer (`aow_view.exe`,
`src/game/viewer.cpp`) renders a decoded `.HSM`/`.CSM` map, but it is a
**crude first-pass preview**, not a faithful reproduction. When you load e.g.
`Underground Titans`, the output looks nothing like the real game: large areas
are missing/black, there are no coastlines or terrain transitions, forests and
mountains sit at wrong offsets or vanish, and every hex of a terrain type looks
identical.

**Goal:** turn the renderer into a **high-fidelity** map viewer that reproduces
what the engine actually draws, and in doing so **close every `TODO(RE)` item in
the map pipeline** (listed below). Every RE finding must be written into
`docs/01_FORMATS.md` **and** encoded as a parser field or a test — no orphan
knowledge (see the "Working agreements" in `docs/03_ROADMAP.md`).

## Where things are

| what | path |
|------|------|
| Renderer | `src/engine/mapviz.cpp`, `src/include/aow/mapviz.hpp` |
| Map parser | `src/engine/map.cpp`, `src/include/aow/map_types.hpp` |
| Resource table | `src/engine/map_resources.cpp`, `src/include/aow/map_resources.hpp` |
| ILB decoder | `src/engine/ilb.cpp`, `src/include/aow/ilb.hpp` |
| GUI viewer | `src/game/viewer.cpp` |
| CLI | `src/game/main.cpp` |
| Tests | `src/tests/{map_test,mapres_test,pfs_types_test}.cpp` |
| Build | `build.ps1` (g++ only path is exercised) |
| Formats doc | `docs/01_FORMATS.md` (see the "HSM / CSM / HSS" + "Resource resolution + rendering" + "TODO(RE)" sections) |
| Roadmap | `docs/03_ROADMAP.md` (Stage 2.2) |
| **Ghidra exports** | `re/exports/{hsepack,aowepack,ilpack}/` — the decompiled evidence you must mine |

## Build & test (this is how you validate every step)

```powershell
cd 'C:\GOG Games\Age of Wonders\port'
./build.ps1 -Test -Viewer      # builds AoW.exe + aow_view.exe, runs ALL suites,
                               # installs aow_view.exe next to the game
```

Head-less render (what CI/ctest use) — run this constantly on a real map:

```powershell
./bin/aow_view.exe '..\Scenario\Custom\Tutorial.hsm' `
    --root '..' --level 0 --render '.\bin\check.png'
# then open bin/check.png and compare against a screenshot of the real game
```

The four suites that must stay green: `pfs_types_test` (42), `map_test` (17 /
144 maps, 2,761,039 tiles), `mapres_test` (8), and the `aow_view` smoke test.
Compile with **zero new warnings** (`-Wall -Wextra -Wpedantic`).

## Baseline (already done — do not regress)

* `.HSM`/`.CSM` fully decode (container → levels → row-major `TMapField`s; each
  field's table has **tag 50 = terrain hexagon**, **tags 51+ = objects**).
* `render_map_rgba()` splits pixel output from PNG writing; `render_map_png()`
  is a thin wrapper; `viewer.cpp` paints from `render_map_rgba`.
* Terrain/object sprites resolve via `MapHs::resource` → `Release.hss` tag 5
  (`TMapObjectResourceList`, 1142 records) → ILB library name → file by stem.
* The renderer currently: fills the canvas with RGB `(12,12,20)`, draws each
  tile's tag-50 sprite with **`ilb.entries().front()` (frame 0 only)** at a
  fixed 48×32 iso diamond, then draws non-tag-50 objects at
  `sx + (tile_w - s.w)/2, sy + tile_h - s.h + 8`. **The guessed tile geometry was
  a main source of the fidelity gap; the renderer now uses the engine's own**
  (`HSEngine.HXtoHP@8E9B8701`: `px = x*32 + 8`, `py = y*32 + (x & 1)*16` -
  axis-aligned, 32x32 pitch, column-staggered, *not* a 45-degree iso diamond).

## The TODOs to crack (all of them)

### 1. Terrain transition frame selection  ⟵ *the single biggest fidelity win*
`docs/01_FORMATS.md` TODO + `docs/03_ROADMAP.md`:
> "The **frame index** inside a terrain library (the terrain-transition variant)
> is chosen at runtime from the neighbouring hexes
> (`Hexagon.THexagon.UpdateTransitions`); `mapviz` uses frame 0 for now, so all
> tiles of one terrain type look identical."

* `Hexagon.THexagon.UpdateTransitions` is **not yet exported** — locate it in
  `HSEPack.dpl` (image base `0x55600000`) in Ghidra, decompile it, and export to
  `re/exports/hsepack/Hexagon.THexagon.UpdateTransitions.c`.
* Work out the frame-index rule (neighbour terrain class/edge mask → frame
  index in the 48×32 diamond library). The terrain hexagon resources are
  `0x20153 Hexagon.THexagonResource` (122), `0x20171
  IsoHex.TUpperIsometricHexagonResource` (2: `EARTHHEX.ILB`, `ROCKHEX.ILB`),
  `0x20558 BorderHx.TBorderHexagonResource` (12: `H_Border.BMP`), `0x2073e
  AoWHex.TAoWWaterHexagonResource` (2: `WATERHEX.ILB`, `ICEHEX.ILB`).
* Implement it: pick the correct frame per hex from the 8 (or 6) neighbours,
  instead of `entries().front()`. Add a `mapres_test` check that a coastline /
  terrain boundary tile selects a **non-zero** frame.

### 2. Object sprite anchoring (`xshift`/`yshift`)
`docs/01_FORMATS.md`:
> "Object sprites are anchored by their hex centre; the exact `xshift`/`yshift`
> fields (`TRLESprite16+0x5c/+0x60`) are not yet applied."

* Decompile the relevant sprite blit / `GetVisualDimensions` path
  (`re/exports/ilpack/TRLESprite16.*`) and the map-object draw code to recover
  how `xshift`/`yshift` (and the hex-centre anchor) position a sprite. The
  current `+8` nudge and centring are guesses.
* Apply the real offsets so mountains/forests/cities land correctly and tall
  sprites are not clipped.

### 3. `THSMap` tags 3/4/5/6
`docs/01_FORMATS.md` + `map_types.hpp`:
> "`THSMap` tags 3/4/5/6 (bytes/ints at `THSMap+0x34/0x80/0x84/0x88`) ... are
> decoded structurally only."

* `HSEngine.THSMap.ReadWrite` (`HSEPack 0x5560c4c4`) + `AoWE.TAoWHSMap.ReadWrite`
  (`AoWEPACK 0x55776d60`) are already exported. Recover what tag 3 (rwByte) and
  tags 4/5/6 (rwInteger, currently guessed as map pixel width/height) actually
  are — likely map dimensions / tile size. Expose them as typed `Map` fields and
  assert them against the known `width`/`height`.

### 4. Player / race lists (`TAoWHSMap` tag 0x15 / 0x16)
> "The player/setup lists (`tag 0x15` TPlayerList, `0x16` TRaceList) are located
> but their per-record tags are not yet mapped."

* Find `TPlayerList` / `TRaceList` `ReadWrite` in `AoWEPACK.dpl`, map their
  per-record tags, and expose typed structs on `Map` (players: name/race/colour/
  team; races: name/type). This is what later lets the viewer tint owned cities
  and label players.

### 5. Rendering-fidelity fixes discovered in review (not yet in the TODO list)
These directly explain "the map looks totally different":

* **Stop leaving dark gaps.** The canvas is pre-filled `(12,12,20)`; any hex
  whose tag-50 sprite fails to resolve/load just shows that near-black backdrop.
  Every hex must get *some* terrain (real sprite, else a per-terrain colour
  fallback diamond — `terrain_color()` already exists but is only used when the
  ILB name resolves). Make the fallback robust and add a test that the rendered
  level has **no fully-background pixels** inside the hex field.
* **Multi-frame / layered composition.** Objects and terrain currently use
  `entries().front()` only. Confirm which classes need which frame (some object
  ILBs are multi-frame/animated) and composite the engine's actual layer order
  (ground → fringes/transitions → object overlays).
* **Verify tile geometry - DONE (Phase 1).** The grid is the engine's own, not a
  45-degree iso diamond: `HSEngine.HXtoHP@8E9B8701` places a hex top-left at
  `px = x*32 + 8`, `py = y*32 + (x & 1)*16` - a **32x32 pitch, axis-aligned,
  column-staggered** grid (odd columns down 16px). The 48x32 sprite
  (`tile_w=48, tile_h=32`, whose 45-degree flanks interlock over the 16px flat
  edge) is confirmed against the decoded `H_Gr.ILB` base frame
  (`detect_tile_size`); `mapres_test` samples every hex centre with the formula
  and asserts none shows the backdrop.

## Deliverables

1. Updated `src/engine/mapviz.cpp` (+ `mapviz.hpp` if the options/API grow) with
   the transition-frame selection, correct object anchoring, robust per-hex
   fallback, and layered composition.
2. Updated `src/engine/map.cpp` / `map_types.hpp` (and, if needed,
   `map_resources.*`) with the recovered `THSMap` fields and player/race lists.
3. New/extended tests in `src/tests/` (keep the existing 4 suites green; add
   fidelity assertions, e.g. non-zero transition frames, no backdrop gaps).
4. New Ghidra export `.c` files under `re/exports/` for every function you
   decompile, with the standard header comment.
5. Doc updates: `docs/01_FORMATS.md` (move items **out of** `TODO(RE)` as you
   close them, with the evidence) and `docs/03_ROADMAP.md` Stage 2.2.
6. `build.ps1` / `src/CMakeLists.txt` updated only if you add files or tests.

## Constraints

* **stdlib-only** policy (plus OS libraries for the Win32 viewer): no third-party
  deps, no CMake requirement. The existing PNG writer emits *uncompressed*
  DEFLATE on purpose.
* C++17, `g++ -std=c++17 -Wall -Wextra -Wpedantic`, **no warnings**.
* Do not break the head-less contract used by the smoke test / ctest
  `viewer_render`: `aow_view <map> --root <dir> [--level N] [--no-objects]
  [--max N] --render <out.png>`.
* Keep the GUI double-clickable and painting from `render_map_rgba`.

## Acceptance criteria

* `./build.ps1 -Test -Viewer` is fully green (all suites + smoke test), no
  warnings.
* Rendering `Underground Titans` (and `Dwarf1.CSM`, `Tutorial.hsm`) to PNG and
  eyeballing it against a real in-game screenshot shows: coastlines and terrain
  transitions present, forests/mountains/cities at the right positions, and no
  large unexplained black regions.
* Every item that was in `docs/01_FORMATS.md` "TODO(RE)" for the map pipeline is
  either **closed with evidence** (decompile + test) or explicitly re-scoped
  with a written reason.

## Suggested order

1. Fix the backdrop/fallback gaps + confirm tile geometry (quick, huge visual
   win, no RE needed).
2. Recover & implement object `xshift`/`yshift` anchoring.
3. Recover & implement terrain transition-frame selection.
4. Recover `THSMap` tags 3/4/5/6, then the player/race lists.
5. Update docs + tests after each step; keep re-rendering a real map to check.