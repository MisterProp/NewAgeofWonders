# Handoff prompt: world-map unit movement (with the ORIGINAL sprites)

> Copy everything below the line into a fresh agent session. It is
> self-contained. The previous attempt at this feature was **reverted** (see
> "Current state"); this prompt tells you exactly what to build and, crucially,
> that the on-map **arrow/X graphics must be the game's own ILB sprites** - never
> hand-drawn.

---

## Mission

In the Age of Wonders 1 C++ port at `C:\GOG Games\Age of Wonders\port`, implement
**world-map unit movement** for the human player's armies, reproducing the
original engine **1:1**: click one of your armies, see the movement path drawn
with the **game's own arrow and X image libraries**, and click a hex to move the
army there (walk animation). Terrain passability, the per-terrain move cost
("malus"), the unit's movement allowance, and the army's move budget must all use
the recovered rules below.

**Non-negotiable:** the path arrows and the destination X are the original
`ILB` sprites, blitted with the recovered rule. Do **not** draw vector lines,
triangles, or boxes as a substitute. The previous attempt did that and it was
rejected. If you cannot pin the exact sprite/frame mapping, say so and render a
head-less proof PNG for confirmation instead of inventing art.

## Current state (start here - it is clean)

* Every file the previous attempt touched has been **reverted**. There is **no**
  movement code in the tree: `src/engine/movement.cpp`,
  `src/include/aow/movement.hpp`, `src/include/aow/move_cost_tables.hpp`,
  `src/tests/movement_test.cpp`, `docs/15_MOVEMENT.md` do **not** exist;
  `viewer.cpp`, `map_types.hpp`, `map_resources.*`, `CMakeLists.txt`,
  `build.ps1`, `README.md`, `docs/03_ROADMAP.md` are back to their original
  content. `./build.ps1 -Test` is fully green.
* `aow_view.exe` at the game root (`..\aow_view.exe`) is the **reverted**
  (no-movement) build.

Verify the clean state before you start:
`Get-ChildItem -Recurse -Include 'movement.*'` must return nothing, and
`Select-String -Path src\game\viewer.cpp -Pattern movement` must return nothing.

## Where things are

| what | path |
|------|------|
| Renderer / ILB cache / blit | `src/engine/mapviz.cpp`, `src/include/aow/mapviz.hpp` |
| ILB decoder | `src/engine/ilb.cpp`, `src/include/aow/ilb.hpp` (`aow::IlbFile`, `indexed by image-slot id`) |
| Map parser (fields/objects/armies) | `src/engine/map.cpp`, `src/include/aow/map_types.hpp` |
| Resource table (`Release.hss`) | `src/engine/map_resources.cpp`, `src/include/aow/map_resources.hpp` |
| Game state (player roster) | `src/engine/game_state.cpp`, `src/include/aow/game_state.hpp` |
| GUI map viewer | `src/game/viewer.cpp` (`aow_view.exe`) |
| CLI | `src/game/main.cpp` (`aow_tools.exe`) |
| Tests | `src/tests/` |
| Build | `build.ps1` (MSYS2 g++ 15.2, **no CMake**), `src/CMakeLists.txt` |
| Docs | `docs/01_FORMATS.md`, `docs/03_ROADMAP.md`, `docs/09_GAME_SCREEN.md`, `docs/13_UNITS_ON_MAP.md`, `docs/14_GAME_STATE.md` |
| Ghidra decompiles | `re/exports/{aowepack,hsepack,...}/` |

Read first: `port/README.md`, `docs/03_ROADMAP.md` (Working agreements),
`docs/04_DRIVING_CLINE.md`, `docs/13_UNITS_ON_MAP.md`, `docs/14_GAME_STATE.md`.

## Build & test (run these constantly)

```powershell
cd 'C:\GOG Games\Age of Wonders\port'
./build.ps1 -Test -Viewer     # builds everything, runs ALL suites, installs aow_view.exe next to the game
./build.ps1 -Test             # build + tests only
```

Head-less spawn/render (what CI and the smoke test use; `aow_view.exe` is a GUI
subsystem binary, so run it via `Start-Process -Wait`):

```powershell
# from C:\GOG Games\Age of Wonders   (game root; --render path must be absolute & space-free)
Start-Process -FilePath 'aow_view.exe' -Wait -PassThru -NoNewWindow `
  -ArgumentList @('"Scenario\Custom\Tutorial.hsm"','--render','"C:\tmp\out.png"')
```

Keep these suites green and compile with **zero new warnings**
(`-Wall -Wextra -Wpedantic`): pfs_types, unit, unit_catalog, map, game_state,
mapres, mapviz_golden, the viewer/menu/scenario/game smoke tests, audio,
playlist.

## Tooling (Ghidra MCP + helpers)

* AoW modules are Delphi/Borland PE32. Image bases: `AoWEPACK.dpl 0x55700000`,
  `AoW.exe 0x00400000`, `HSEPack.dpl 0x55600000`.
* **GhidraMCP HTTP bridge is at `http://127.0.0.1:8089`** (NOT 8080). Useful:
  * `python tools/ghidra/mcpq.py /AoWEPACK.dpl /decompile_function address=0x5574b9ec`
  * `python tools/ghidra/mcp_call.py /search_functions "name_pattern=Move"`
  * `python tools/ghidra/mcp.py out.json /read_memory address=0x558e84fc&length=256`
  * `python tools/ghidra/decompile.py /AoWEPACK.dpl re/exports/x --list addr.txt`
  * `/get_xrefs_to` returns nothing for Delphi string constants - use
    `python tools/find_ref.py <module> --va 0x...` (scans for the absolute dword).
* `python tools/disasm.py <module> 0xVA length` - Capstone disassembly.
* `python tools/hexdump_va.py <module> --va 0x... --len N`.
* `python tools/ascii.py <png>` / `python tools/sheet2.py ... --scale N` - view
  sprite art as ASCII / zoomed sheets (use these to *verify* which ILB frames
  are the arrows before coding).
* Decode an ILB entry to PNG: `bin/aow_tools.exe png "Images\Cursors.ilb" <idx> out.png`;
  list entries: `bin/aow_tools.exe ilb "Images\Cursors.ilb"`.

## The movement rules ALREADY RECOVERED (use these; verify, do not re-derive from scratch)

All addresses are `AoWEPACK.dpl` unless noted. The prior session's decompiles are
gone, so re-decompile as needed with the tools above.

> **Correction (docs/15_MOVEMENT.md §2).** The cost-table index is
> `terrain*16 + overlay + 1`: the field's overlay byte is the *raw*
> `TTerrainList` code (-1 = none/plains, 0 = mountains, 1 = forest, 2 = hill,
> 4 = road/city, 5 = bridge, ...) and column 0 of the table is "no overlay".
> Both lookups add the one (`MovePointCost@0x55780848` reads
> `acStack_106[i]` although the table was written one byte earlier at
> `local_107`; `DefaultMoveOnMovePointProc@0x557454d8` reads
> `mc + terrain*0x10 + 0x31 + overlay` with the table at `mc + 0x30`), so
> `0x80..0xFF` must be treated as **-1** and the spot values quoted below were
> the pre-fix reading - taken literally they make forests impassable and
> mountains cost 4.

1. **Terrain move-cost tables.** The engine builds `MovePointTables` (BSS at
   `0x558ea040`, 256 tables x 256 bytes) at startup in the routine at
   **`0x55744b2c`** (no symbol; disassemble it):
   - fill `0x10000` bytes with `0xFF`;
   - for each `move_type` byte `0..255`, for each cell
     `idx = terrain*16 + overlay` (`terrain 0..12`, `overlay 0..15`):
     `v = 0x7F`; for each set bit, `v = min(v, src[k][idx])` skipping `0xFF`;
     `MovePointTables[move_type][idx] = v`.
   - the eight **source tables** are at **`0x558e84fc + k*0x100`** (8 x 256 B),
     consumed in bit order **`{0x01 walk, 0x04 water, 0x02 fly, 0x08 forest,
     0x10 cave, 0x20 mountain, 0x40, 0x80}`**.
   - Extract the 8 tables with `/read_memory` (they are the ground truth - do not
     hand-author them). Spot values (walk table): grass 4, grass+overlay5 (road)
     3, grass+overlay2/3 (forest/hill) 6, grass+overlay12 (mountain) 8, water
     impassable.
2. **The two axes.** `HSEngine.TMapField` `+0x14` = terrain type, `+0x15` =
   overlay. They come from the tag-50 terrain hexagon resource's `TTerrainList`
   (rw tag 7) entry 0, a `u16 = terrain | overlay<<8`:
   `HSEngine.TTerrainList.GetTerrain@0x55605054` (low byte),
   `GetOverlay@0x556050c8` (high byte). **`TMultiHexMO.OverrideTerrain@0x5560b2d4`
   only writes `+0x15` when the byte is `>= 0` as a *signed char***, so
   `0x80..0xFF` means "no overlay" (a plain grass hex ships `0xFE`) - treat those
   as overlay 0. Terrain list: 0 water, 1 grass, 2 desert, 3 snow, 4 steppe,
   5 wasteland, 6 ice, 7 earth, 8 rock, 9 lava, 10 dirt, 11 upper-wall,
   12 upper-coast, 13 upper-ice, 14 coast, 15 border.
3. **Move types.** `TAbilityOwner.GetAbMoveTypesAll@0x5574f764`:
   `bit0 = ability 0`; `bit1 = 1|0xa5|0x3b`; `bit2 = 2|0x17|0xa4|0xa6`;
   `bit3 = 3|0x9d`; `bit4 = 4|0x9d`; `bit5 = 5|0x9d`; `bit6 = 7|0xa3`;
   `bit7 = 0x2a`. `TUnitResource.GetMoveTypes@0x55784d10` (owner at resource
   `+0x38`); `TArmy.MoveTypes@0x5578d9e8` (union of selected units).
   The per-unit ability bitmap is a `TCustomAbilityList` (tags 2 = `u16` count,
   3 = `ceil(count/8)` bytes) - for a unit resource it is the rw **tag 25**
   payload (`AoWE.TAbilityOwner.ReadWrite@0x5574f318`). NOTE: the exact tag-25
   bitmap decode was not fully confirmed; verify it (render the ability list) and
   document.
4. **Per-unit cost table.** `TAbstractUnit.CreateMovePointTable@0x5577fdc4`
   (copy `MovePointTables[move_types]`, then ability tweaks);
   `TAbstractUnit.MovePointCost@0x55780848` = `table[terrain*16 + overlay]`;
   `DefaultMoveOnMovePointProc@0x557454d8` / `ExploredOnly…@0x5574642c` read it.
5. **The army.** `TArmy.CreateMovePointTable@0x5578dc40` seeds the table with 1
   then `MaximizeMovePointTable@0x55744d34` folds each selected unit
   (`dst = max(dst, src)`, `0xFF` propagates - slowest unit / any-blocked blocks
   all). Budget `TArmy.MovePoints@0x5578d8d0` = `min(0x32, min unit move points)`.
   `TUnit.GetMovePoints@0x5578276c` is `+0x3d`; `SetMovePoints@0x55782794`
   clamps to `0..max`.
6. **The pathfinder.** `TMoveControl.CalculatePath@0x557463f8` ->
   `CalculatePathEx@0x557460b0` (a bounded flood-fill over an accumulated
   move-point grid, then a gradient descent from the destination back to the
   source; the path is CLEARED when the destination is unreachable).
   Helpers: `CalculateMovePointLine@0x557455b4`, `ProcessHexagon@0x557456f8`,
   `ValidPath@0x557468e8`, `TMoveControlSettings.{Get,Set}MovePoints@0x557452e4 /
   0x55745318`, `SetupMovePointData@0x557451c4`.
   The six hex neighbours: `HSEngine.HNtoHX@0x5560e08c` (dir 1..6 =
   `(0,-1)/(1,-1)/(1,0)/(0,1)/(-1,0)/(-1,-1)`) with the
   `HSEngine.CenterHNtoHX@0x5560e17c` column-stagger fold (odd column + odd `dx`
   => `+1` row). Equivalently, a bounded Dijkstra from the source gives the same
   result.
7. **Selection / click driving it.** `AoWE.TAoWHSMapMouse.Down@0x5577448c`;
   `AoWE.TAoWHSMap.Select@0x55777c5c`; and the player path:
   `AoWE.TSelectedArmy.SelectPath@0x55792b6c` -> `CalculateMovePath@0x55792900`
   -> `AoWE.TMoveSelector.{CalculateMovePath@0x55763d40, SelectPath@0x55763d48,
   ShowMovePaths@0x55763f80}`. `TSelectedArmy.ShowMovePaths@0x55792524` gates it.
   `TSelectedArmy`/`TSelectedArmy.GetArmyHS` supply the army; the human player is
   the `TPlayer` whose type is 0 (`docs/14_GAME_STATE.md`, `Player::type`).

## THE SPRITE RULE (this is what the previous attempt got wrong - use the real ILBs)

`AoWE.TMoveSelector.ShowMovePaths@0x55763f80` draws the path **one image per
step**, blitted at `HXtoHP(hex) - camera + (-8,-8)` (see the `-8`/`-8` at the
blit calls):

```
for each step 1..n-1 (from hex B[i] to hex B[i+1]):
    dir  = HSEngine.dHXtoHNfast(B[i], B[i+1])          # hex step direction
    last = (i == n-1)                                   # destination step
    frame = (step is within this turn's move points) ? dir + 0x29    # bright
                                                     : dir + 0x22    # dim
    img = TCustomImageLibrary.Get(lib, frame)
    img.Show( HXtoHP(B[i]) - camera + (-8,-8) )
# single-step / end cases use frame 0x29 (=41) and 0x30 (=48)
```

`lib` = `*(AoWHSSet + 0x4c)`. `AoWE.TAoWWHSSet.Create@0x5574b9ec` sets it via
`TImageLibraryManager.LinkToIL(manager, 1, "CURSORS.ILB")` -> the library is
**`Images\Cursors.ilb`**.

**IMPORTANT - verify before coding.** Dumped, `Images\Cursors.ilb` slots 34..48
are 49x49 `Pointers.BMP` frames and they *render* like the game's **cursors**
(pointer + terrain/status icon). Two candidate readings:
* (a) the path trail really is `Cursors.ilb` frames `dir+0x22` / `dir+0x29`; or
* (b) `ShowMovePaths` draws the *cursor* and the visible arrow-segment trail is
  `Int\IconMove.ILB` (20x 32x17 `IconMove.BMP` arrow segments; loaded by an
  `AoW.exe` DFM control, `FileName = Int\IconMove.ILB`, Left 0x3a8 / Top 0x130;
  string VA `0x00500c41` in `AoW.exe`).

**Decide this first, with evidence, before writing the renderer:** decode the
candidate frames to PNG (`bin/aow_tools.exe png ...`), view them with
`tools/ascii.py` / `tools/sheet2.py --scale 4`, and compare against a real
in-game screenshot (the user's shows small white/pink chevron arrows along the
path and a red **X** on an unreachable/beyond-turn target). Render a **head-less
proof PNG** of the map with the trail drawn from the chosen library/frames and
confirm it matches. Only then wire the interaction.

Also: **`Images\Targhex.ILB`** (3x 48x32 `TargetE.BMP`) is the target/target-hex
overlay - likely the X / destination marker; decode and confirm. The red X in the
screenshot is most probably one of `Cursors.ilb` 34..48 or an `IconMove`/`Targhex`
frame.

Use `aow::IlbFile` + the existing `mapviz` ILB cache/blit path (see how
`mapviz.cpp` loads libraries and composites), so the sprites come from the same
mechanism as every other map graphic.

## Deliverables

1. **`src/include/aow/movement.hpp` + `src/engine/movement.cpp`**: the cost
   tables (extracted, in a generated `aow/move_cost_tables.hpp`), the move-type
   map, the per-unit/army table + budget, and the bounded hex pathfinder
   (a `MovePath` with per-step cumulative cost + `reached` + `destination`).
2. **Terrain/overlay resolution**: add `overlay` to `MapResource` (the high byte,
   `0x80..0xFF` => 0) and `terrain`/`overlay` to `MapTile`, filled from the
   tag-50 resource (e.g. `fill_tile_terrain(level, res)`).
3. **The viewer interaction** (`src/game/viewer.cpp`): select the human player's
   army; on hover, render the trail with the **real ILB arrows** per the sprite
   rule; on click, move the army with a step animation (relocate the
   `AoWE.TArmyHS` between field tables, set `army_direction`); deselect.
4. **Tests** (`src/tests/movement_test.cpp`): lock the extracted table values,
   the min/max rules, the army budget, the neighbour offsets, and the pathfinder
   (reachable goal, partial path on a small budget, a water wall blocking a
   walker but not a flyer). Keep all existing suites green.
5. **Docs**: write `docs/15_MOVEMENT.md` (all findings + evidence); update
   `docs/03_ROADMAP.md` (new Stage) and `docs/01_FORMATS.md` (city/field terrain
   if relevant); add a `README.md` row.
6. **Build wiring**: add `engine/movement.cpp` to `build.ps1` (`$sources`, `$vsrc`)
   and `src/CMakeLists.txt` (the engine lib + a test target + `add_test`).

## Constraints (project conventions)

* **stdlib-only** (plus OS libs for the Win32 viewer). No third-party deps, no
  CMake requirement (CMake is not installed - use `build.ps1`).
* **C++17**, g++ `-Wall -Wextra -Wpedantic`, **zero new warnings**.
* Do not break the head-less contract used by the smoke test/ctest:
  `aow_view <map> --root <dir> [--level N] [--no-objects] [--max N] --render <out.png>`.
* **Working agreements** (`docs/03_ROADMAP.md`): every RE finding goes into a doc
  **and** becomes a parser field or a test - no orphan knowledge. Cite the
  decompiled address for each rule.

## Acceptance criteria

* `./build.ps1 -Test` fully green (all suites + smoke tests), no warnings.
* Opening `Tutorial.hsm` in `aow_view.exe`, clicking your army (the human player
  is `PlayerType::Human`, type 0; on `Tutorial.hsm` the roster is Independents /
  Player 1 / Player 2 and the human owns the hero at hex **(15,34)**), hovering
  shows the **original arrow sprites** along the path and the **original X** on
  an unreachable/beyond-turn hex; clicking a reachable hex walks the army there.
* A head-less proof PNG exists showing the real sprites (paste/compare it).

## Suggested order (do NOT skip step 1)

1. **Verify the sprite source.** Decode `Cursors.ilb` 34..48, `IconMove.ILB`,
   `Targhex.ILB`; view them; decide (a) vs (b) above with evidence; render a
   head-less proof of the trail over `Tutorial.hsm` and confirm it matches the
   real game. **Do not write the interaction until the sprites are proven.**
2. Extract the 8 cost tables (`/read_memory` 0x558e84fc) and implement
   `movement.cpp` + `fill_tile_terrain` + the move-type/army/pathfinder logic;
   add `movement_test.cpp`.
3. Wire the viewer: select / hover-preview (real sprites) / click-move + animation.
4. Docs, build wiring, README/roadmap; run `./build.ps1 -Test -Viewer`.

## Pitfalls / what the previous attempt did WRONG

* It drew the arrows and X as **vector GDI strokes** instead of the game's ILB
  sprites. **Never do that.** Use `Images\Cursors.ilb` / `Int\IconMove.ILB` /
  `Images\Targhex.ILB` through `aow::IlbFile`.
* It only allowed selecting the human player's exact army hex, so clicking the
  Giant/Orc/Goblin on `Tutorial.hsm` (owned by the independents/CPU) did nothing,
  and the user thought the feature was broken. Make the human's armies easy to
  find (e.g. auto-select + centre on open, mark army hexes, a key to cycle them)
  and give title-bar feedback.
* It defaulted every unit to "walk" (it never decoded `Unitres.pfs` abilities),
  so flyers/swimmers did not differ. Decode the per-unit ability bitmap
  (verify the tag-25 layout first) and feed `GetAbMoveTypesAll`.
* It modelled unit move points as a constant 8; the real value is the unit's
  `+0x3d` fed by the turn loop (`docs/14_GAME_STATE.md`).
* The drop of move points between turns, transporters/sub-moves, and the road
  "constructing" cost callout are out of scope - state this in `docs/15_MOVEMENT.md`.

## Revert note (if you need to undo)

The previous feature was fully removed; there is no movement code to build on.
If you need to inspect the *old* (rejected) approach for reference only, it drew
GDI vectors in `viewer.cpp` and used a `MovePointTable`/`find_move_path` engine -
but do not restore it; rebuild from the rules above with real sprites.



