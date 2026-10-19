# 03 - Roadmap

Guiding principle: **data -> systems -> presentation -> completeness.** Each
stage produces something runnable or a concrete artifact; never block the port
on "the whole game being decompiled".

Legend: [auto] can be scripted by the harness, [re] needs human/RE judgement,
[port] C++ work.

---

## Stage 0 - Harness (done)
* [auto] Binary inventory, import graph, exports (`dpl_index.py`).
* [auto] DFM form extraction (`dfm_extract.py`).
* [auto] ILB inventory (`ilb_tool.py`), strings (`strings_tool.py`).
* [port] Skeleton builds; `aow ilb <file>` lists entries.

## Stage 1 - Data layer (highest ROI)
1. [re] Fully map `.PFS` record layout - **done** (container + property-table
   object graph; see `docs/01_FORMATS.md`). All 12 files decode cleanly; the
   old TODO list (`Ability.pfs`, `Spells.pfs`, `ITEMS.pfs`, `HEROES.PFS`,
   `Unitres.pfs`) is covered, plus the rest.
   * Cross-reference descriptions in `strings_tool.py` output with record order.
2. [port] Typed structs for **all 11 non-empty `.PFS` tables** that load from the
   original files - **done** (`src/include/aow/pfs_types.hpp` +
   `src/engine/pfs_types.cpp`, loaders `aow::load_*`, CLI
   `aow abilities|spells|units|items|heroes|herores|unitgfx|faces|itemgfx|texts|terrain`
   / `aow pfs <file> --typed`). Verified counts (original AoW1):
   Ability 147, Spells 108, Unitres 142, ITEMS 83, HEROES 50, HERORES 36,
   Unitgfx 182, FACERES 89, ITEMGFX 123, TEXT 91, comb_res 581.
   * Field semantics recovered from the consumer getters: UnitStat 14..20 =
     attack/defense/damage/hit_points/moves/resistance/level, 13 = race id;
     Spell 13/14/15/16/17 = mana_cost/upkeep/research_cost/sphere/level;
     Item 20 = granted spell; Hero (name, face, resource index, skills,
     experience, casting/hit/move points, library, used skill points, morale).
     Remaining `TODO(RE)`: Spell none; UnitStat none; Item 17; Hero 4,5,9,12,
     14,25,26,27,31,34; comb_res body. See `docs/01_FORMATS.md`.
   * Rule-fidelity tests: `src/tests/pfs_types_test.cpp` (`build.ps1 -Test`
     or `ctest`) - locks record counts + known names/stats (42 checks).
3. [port] Rule-fidelity tests in `src/tests/` comparing parsed counts/values to
   values visible in-game.
4. [re] Decode `.ILB` entry records -> widths/heights/palette; export frames to
   PNG to validate against the shipped `ILBEditor`.

**Exit test:** a CLI that dumps every unit/spell/item with stats and a PNG of
any sprite, from the original data - no decompiler needed.

## Stage 2 - Core engine
5. [re] Reconstruct the object model from IDR/Ghidra: `Enginep.dpl` (base
   objects), then `HSEPack.dpl`, then `AoWEPACK.dpl` (10036 exports: the game).
   Build `re/symbols/renames.csv` mapping addresses -> names and apply it
   through `tools/ghidra/import_renames.py` on subsequent runs.
   * `HSEPack.dpl` is now imported into Ghidra (image base **0x55600000**) - it
     holds the map classes (`THSMap`, `TMapContainer`, `TMapLevel`, `TMapField`)
     and the terrain hexagons (`Hexagon.THexagon`, `IsoHex`, `BorderHx`).
6. [port] Deterministic fixed-tick simulation core (`src/engine/sim`): hex/tile
   grid, turn order, movement, combat resolution.
   * **Stage 2.3 - the game state + turn loop (done).** `src/engine/game_state.cpp`
     + `src/include/aow/game_state.hpp` load the scenario's player roster
     (`TAoWHSMap` tag 0x15 = `AoWE.TPlayerList`, class `0x20020`) into typed
     `aow::Player` records (race, gold, gems, income/upkeep, mana/power/research,
     per-player placed armies/units via `AoWE.TArmy` rw tag 4) and implement the
     recovered day/turn loop (`TPlayerControl.NewDay`/`NewTurn`,
     `TAoWHSMap.GenerateNewTurnOrder`). CLI `aow gamestate <map> [--turns N]`;
     `src/tests/game_state_test.cpp` (48 checks). See `docs/14_GAME_STATE.md`.
   * **Stage 2.4 - world-map unit movement (done).** `src/engine/movement.cpp` +
     `src/include/aow/movement.hpp` (+ the generated
     `src/include/aow/move_cost_tables.hpp`) implement the recovered movement
     rules: the engine's eight move-cost source tables (dumped from
     `AoWEPACK.dpl 0x558e84fc` with `tools/gen_move_cost_tables.py`), the
     ability -> move-type fold (`GetAbMoveTypesAll`, incl. the
     `TCustomAbilityList` tag 2/3 bitmap), the per-unit/per-army move-point table
     and the `min(0x32, min unit)` budget, the hex geometry
     (`dHXtoHNfast`/`dHXtoRad`/`HNtoHX`) and a bounded pathfinder
     (`TMoveControl.CalculatePathEx`). `aow::fill_tile_terrain` resolves each
     hex's `terrain`/`overlay` and `aow::draw_move_path_overlay` draws the
     path with the game's own `Images\Cursors.ilb` arrow/X sprites (frames
     `dir+0x22`/`dir+0x29`, X `0x29`/`0x30`). The viewer selects the human
     player's army, previews the trail on hover and walks the army on click.
     CLI `aow move <map> [--root <dir>] [--level N] [--to X,Y]`; head-less proof
     `aow_view <map> --render out.png --move X,Y`;
     `src/tests/movement_test.cpp` (118 checks). See `docs/15_MOVEMENT.md`.
   * **Stage 2.5 - unit sprite animation (done).** `src/engine/unit_anim.cpp` +
     `src/include/aow/unit_anim.hpp` implement the recovered animation: the
     `Unitgfx.pfs` tag-5 `TImageSequenceList` (library/mode/period/frame table),
     the 31-pose layout, `TUnitHS.Show`'s `seq[dir-1]` (stand) / `seq[dir+5]`
     (walk) selection, `TImageSequence.GetFrameIndex`, `TUnitHS.GetMoveSteps` and
     the sub-hex `WalkAnimation`. A move walks **each unit's own** sprites toward
     the hex it is entering, in `aow_view.exe` and the in-game `AoW.exe` screen.
     CLI `aow unitseq`; test `src/tests/unit_anim_test.cpp` (102 checks). See
     `docs/16_UNIT_ANIM.md`.
   * **Stage 2.5b - unit ground shadows (done).** `AoWE.TUnitShadowHS` /
     `TUnitHS.ShowShadow@0x55784504`: the shadow is a separate scene object one
     show-priority layer below the body, takes the sequence `dir + 0x17`
     (slots 24..29 of the same `Unitgfx` list, `UNITS\USHADOW.ILB`) at the body's
     own anchor, and is composited with the engine's `TRLESprite16.ShowTransparent`
     **50 % average** in 565 - not opaquely. Drawn by `render_map_rgba` (a
     `shadow` CSV row per army) and by both walk overlays; benchmarked against the
     original game by `tools/shadow_bench.py` (12.3 mean |delta| over the 17x14
     shadow footprint of `bin/ShadowRef.png`, where an un-shadowed crop scores
     61.0 and an opaque blit 33.9). See `docs/16_UNIT_ANIM.md` section 10.
   * **Stage 2.6 - building (structure) animation (done).** A structure draws the
     image-sequence **chain** `HSEngine.TMapObjectResource.ShowComplexImage` walks
     from its cell image: a static body plus animated `.next` sequences (a mine's
     `STR_MINE.ILB` mode-5 sparkle, a wizard tower's mode-4 orb, a teleporter's
     shimmer, an animated node/farm/cave body). Each frame comes from
     `ImageLib.TImageSequence.GetFrameIndex` with the map clock `AoWHSMap+0x40`
     (`HSEngine.THSMap.NewFrame`) and the per-object phase `x + y*0x10`
     (`aow::object_anim_phase`). `MapVizOptions::clock` bakes the frame at a chosen
     clock; `MapVizOptions::no_animated_overlays` + `draw_structure_overlays` let
     the live screens (the viewer's map-clock timer, `Scene::game_update`)
     re-composite only the animated sprites each frame, since a level re-render is
     ~28 ms, and `MapVizOptions::no_units` + `draw_unit_overlays` do the same for
     the placed armies - they are the **last** world layer, so a unit is drawn
     over a building's glow, its owner flag and the water (the engine sorts a unit
     at `TUnitHS.CalcShowPriority` 0x68000000; only the map's spell gfx,
     `AoWE.TWorldFXHS`, would go above it - not implemented). See
     `docs/13_UNITS_ON_MAP.md`. The animated frames are class-0x16 `TSprite16`, which the engine
     always blits colour-keyed (`TSprite16.ShowOpaque` -> `GFXE.BltColorKey16`);
     the key is the descriptor's u16 right after the shift fields
     (`IlbEntry::color_key`) and `decode_rgb` turns it into alpha, so no building
     keeps its background. CLI `aow_view --clock N` / `AoW.exe --game-clock N`;
     proof `bin/BuildAnim*_c0/c12.png`; sheet `tools/buildings_sheet.py`; test
     `mapres_test` (+20 checks, incl. the per-building audit). See
     `docs/17_BUILDING_ANIM.md`.
7. [port] Map loader for `.HSM`/`.CSM` (tiles + objects) -> render a static map
   - **done** (Stage 2.1). `src/engine/map.cpp` + `src/include/aow/map_types.hpp`
   decode the container, the `TAoWHSMap` top object, the `TMapContainer`/level
   grid and each hex's terrain + object hexagon sprites. CLI `aow map <file>
   [--preview]`. `src/engine/inflate.cpp` adds a stdlib-only zlib inflater.
   All 144 maps decode with 0 desyncs (`src/tests/map_test.cpp`).
   * **Stage 2.2 - sprites + visualiser (done).** The sprite `tag 1` is a
     `TMapObjectResource` id resolved against **`Release.hss` tag 5** (the
     `TMapObjectResourceList`, ~439 records on this data build, each naming an ILB library).
     `src/engine/map_resources.cpp` loads it, `src/engine/mapviz.cpp` renders a
     level to a PNG (48x32 hexagons on the engine's axis-aligned,
     column-staggered 32x32 grid - `HSEngine.HXtoHP` - + object sprites).
     CLI `aow mapres` / `aow map --res` / `aow mapviz`. Tests
     `src/tests/mapres_test.cpp`. See `docs/01_FORMATS.md`.
   * **Stage 2.2b - units on the map (done).** A field's `AoWE.TArmyHS`
     (class 0x20217) is parsed into its `AoWE.TArmy` unit list (`TUnit`/`THero`/
     `TLeader`), and each army draws its lead unit's `UNITS\*.ILB` sprite at the
     engine's anchor (`HXtoHP + (-16,-24) + base+shift`). `src/engine/map.cpp`
     (`parse_army`) + `src/include/aow/map_types.hpp` (`MapUnit`) +
     `src/engine/unit_catalog.*` (Herores.pfs) + `src/engine/mapviz.cpp`. Tests in
     `src/tests/{map_test,mapres_test}.cpp`. See `docs/13_UNITS_ON_MAP.md`.
   * **`aow_view.exe` - GUI map viewer (done).** `src/game/viewer.cpp` is a
     Win32/GDI front-end: double-clicking it opens the native *Open* dialog,
     auto-detects the game root (walks up to `Release/Release.hss`), renders the
     picked `.HSM`/`.CSM` and shows it in a scrollable, zoomable window with a
     File/View/Help menu (level selector, objects toggle, Save PNG, drag & drop,
     Ctrl+O/S/Q). It paints from `render_map_rgba` (added to `mapviz`), so no
     PNG round-trip. `aow_view <map> --render <out.png>` is the head-less mode
     used by the tests (`./build.ps1 -Test`, ctest `viewer_render`);
     `./build.ps1 -Viewer` also copies it next to the game. Windows-only and
     still stdlib-only apart from the OS libraries.
   * **Map-viewer fidelity (Phases 0-4b, done).** Golden harness
     (`src/tests/mapviz_golden.cpp` -> `re/golden/*.png`), `aow_view
     --debug-csv`, full hex coverage (no backdrop gaps; `detect_tile_size`),
     base-hex entry selection in mixed libraries (`terrain_frame` - water now
     draws `H_Wa2`, not the `t_wa-gr` overlay), the engine's
     **axis-aligned, column-staggered** hex grid (`HSEngine.HXtoHP`:
     `px = x*32 + 8`, `py = y*32 + (x & 1)*16`), land terrain-transition overlays
     (`HEXTRANS.ILB`), and **roads** (`Road.TRoad` objects drawing their inline
     border hexes fall back to a flat colour. Where the engine loads the border
     sprite from (assumed `HEXTRANS.ILB` / `CTransHx.ILB`) is unresolved.
     Remaining fidelity work (elevation edges, THSMap fields, z-order) is tracked
     in `docs/ROADMAP_mapviewer_fidelity.md`.
   * **The water animation is done** (`docs/18_WATER_ANIM.md`): the water/ice
     hexagon's sixteen `H_Wa2` base wave frames (one per hex, from `Show`'s
     `((field>>3) + x + y*2 + (sprite+0x15>>6)) & 0xf + 0x8c`), the `T_Ic-Wa2`
     foam where it borders ice or the map border, and `ShowDynamic`'s per-hex
     counter playing the `WATERANM.ILB` ripples / fish / `SEFFECT.ILB` sparkles
     (`aow::WaterHexAnim`, `aow::draw_water_overlays`, the live screen's clipped
     per-frame re-composite).
   * `TODO(RE)`: the per-direction snow/wasteland coastline shelf *slot offset*
     (the 0x28/0x50 variants are computed, the `t_wa-sn`/`t_wa-wl` runs are not
     selected); the `engine+0x6d` / `map+0x3c & 2` / `map+0x188` water-animation
     flags; the "long"
     transition variants and the land↔ice `T_ic-wa` branches of
     `AoWHex.TAoWHexagon.UpdateTransition`; the `THSMap` tag 3/4/5/6 semantics; the
     per-record tags of the player/race lists (`TAoWHSMap` tag 0x15/0x16). (The
     land↔land transition frame choice, the terrain type list (rw tag 7), the
     coastline shelf, the road overlay path and the bridge overlay path are done -
     see `docs/ROADMAP_mapviewer_fidelity.md`.)

**Exit test:** load a scenario, render the terrain, move a unit, resolve a melee.

## Stage 3 - Presentation & UI
8. [port] Platform layer (`src/platform`): SDL2 (or Win32/OpenGL) window, input,
   audio (miniaudio/SDL_mixer), and sprite blitting from decoded ILBs.
   * **The title screen is done** (`src/game/menu.cpp` -> `bin/AoW.exe`).
     It reproduces AoW.exe's own composition 1:1 from the shipped assets: the
     `ScrollBackg5` backdrop panning with wraparound, the `LogoTitleFinal` logo
     with its fade-in/fade-out timeline, the five `OrbFrame` menu orbs on the
     recovered 120 px pitch with their three states and hover captions, and
     `Songs\TITLE.IT` decoded by **libopenmpt** (`aow::openmpt::Player`; the
     reconstructed Galaxy player remains selectable with `--backend galaxy`).
     Every constant comes
     from `AoW.exe` 0x00419c84 / 0x00419488 / 0x0041a150 - see
     `docs/06_TITLE_SCREEN.md`, which also lists the (small) deliberate
     deviations. It is Win32 + GDI + WinMM + libopenmpt.
9. [port] Re-author the screens from `assets/dfm/*.dfm`: the DFM gives exact
   control positions/sizes/fonts, so layout is data-driven.
10. [port] Localization from `.MLD` (reuse `strings_tool.py` output as the
    translation table; ship EN at minimum).

**Exit test:** play a full single-player scenario start-to-finish.

## Stage 4 - Completeness
11. [re] AI, diplomacy, spell research, hero/item generation, event scripting.
12. [port] Networking (`AoWTCPCK.dpl` -> sockets; start with LAN play).
13. [port] Editor parity (from `AoWEd.exe` + `ILToolsP.dpl`).
14. [re] Verify balance constants against the community wiki and in-game
    observations rather than trusting decompiler float noise.

---

## Working agreements
* Every `[re]` finding is written down in `docs/` **and** encoded as a test or a
  parser field - no orphan knowledge.
* Symbol renames live in a versioned CSV (`re/symbols/`), never hand-edited into
  the Ghidra database only.
* Keep the port in `src/` clean-room-ish: original data + observed behaviour in,
  no copied Borland/VCL binaries.
