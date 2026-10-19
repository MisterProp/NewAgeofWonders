# Roadmap — closing the fidelity gap in the map viewer (`aow_view` / `mapviz`)

This is the ordered, concrete plan to make `aow_view`'s output match the real
Age of Wonders world map (in-game and the official editor). It expands the
suggested order in `HANDOFF_faithful_mapviewer.md` into executable steps.

## Why the two renders differ (one paragraph)

The engine draws each hex as a *stack*: base terrain sprite → runtime-chosen
**transition/fringe** overlay (coastlines, terrain blends) → optional
**upper/elevation** hex (cliffs) → object sprites anchored by their sprite
`xshift`/`yshift`, composited in a fixed layer order. `render_map_rgba()`
(`src/engine/mapviz.cpp:281-318`) does only **two flat passes** — one base
terrain sprite per hex and one object sprite per hex — with no transitions, no
cliffs, no real anchoring, and flat-colour fallbacks
wherever a library cannot be resolved. Every phase below removes one of those
gaps.

## Progress

* **Phase 0 — done.** `src/tests/mapviz_golden.cpp` (`aow_mapviz_golden`) renders
  `Tutorial.hsm`, `Underground Titans.hsm` and `Dwarf1.CSM` level 0 to
  `re/golden/*.png`; `MapVizOptions::debug_csv` + `aow_view --debug-csv <out.csv>`
  dump every hex's resolved entry.
* **Phase 1 — done.** A solid base hexagon is laid under *every* hex before its
  sprite is blitted, so no backdrop shows through; the tile size is confirmed
  against the decoded art (`detect_tile_size`). The grid was also corrected to
  the engine's own: `HSEngine.HXtoHP@8E9B8701` places a hex top-left at
  `px = x*32 + 8`, `py = y*32 + (x & 1)*16` - axis-aligned, 32x32 pitch,
  column-staggered (odd columns down 16px), not a 45-degree iso diamond.
  `mapres_test` samples every hex centre with that formula and asserts none
  shows the backdrop.
* **Phase 2 — done.** Terrain hexes pick the library's `h_*` base entry
  (`terrain_frame`), so mixed libraries (`WATERHEX.ILB`) no longer draw a
  transition overlay. Verified: **0** transition entries across `Tutorial.hsm`
  (692 water tiles now use `h_wa2.bmp`).
* **Phase 3 — partially done.** The ILB descriptor's placement fields are
  recovered and parsed into `IlbEntry`: `base_x`/`base_y` (+0x28/+0x2c margins)
  and `xshift`/`yshift` (+0x50/+0x54 for class 0x11, +0x5c/+0x60 for 0x16),
  locked by `mapres_test` against `H_Gr.ILB` (base hexagon = 48x32 at shift 0)
  and `zgr_Tre.ILB`. **Anchor still open:** the engine's literal
  `content = anchor + base + shift` is validated only for terrain (shift 0);
  applying it to objects regresses them (the volcano leaves its scorched
  patch), so the real object anchor needs the map-object draw path, which is
  **blocked on Ghidra** (not installed here). The renderer keeps its validated
  visual-box placement. See the Phase 3 section.
* **Phase 2.5 — object sprites restored (done, no RE).** The resource table
  stores *logical* library names for terrain objects (`gr_tree.BMP`,
  `Rd_gr.BMP`, `gr_rocks.BMP`, ...) that match **no file or ILB entry** on
  disk; `object_library_alias()` now maps them to their abbreviated files
  (`gr_tree.BMP` -> `zgr_Tre.ILB`), so trees/rocks/dead-trees/bones/crops/
  willow render (Tutorial L0: **319 of 362** objects resolve, up from ~120).
  `MOUNTAIN.ILB`'s five terrain blocks are selected by `res_type`
  (`mountain_block()`) so a grass hex gets `gr_mnt2` instead of `mnt_de`.
  Transient additive effects (`TWINKGR`, `SMOKE`) are skipped instead of
  blitting an opaque square. Remaining: `Rd_gr.BMP` (roads, 39) still has no
  library on disk.
* **Phase 2.6 — map frame is ocean again (done, no RE).** `H_Border.BMP` is the
  outermost terrain of every map (class `0x20558 BorderHx.TBorderHexagonResource`,
  a 6-hex-thick frame: `48^2 - 36^2 = 1008` tiles on `Tutorial.hsm`), and no
  `H_Border.ILB` exists anywhere under the game root, so all 1008 frame hexes
  rendered as a flat `rgb(30,30,40)` band. Probing the original's screenshot
  (`HowItShouldBe.png`) shows the frame and the open ocean are the *same* colour
  (~`(41,57,90)`), so `object_library_alias()` now maps the logical name to the
  world-map water library (`waterhex` -> `H_Wa2.BMP`, the tile the frame abuts).
  The frame now renders at `(41,56,90)` and is continuous with the ocean;
  `mapres_test` asserts every frame hex resolves to a real sprite.
* **Phase 3 — multi-hex resource fields recovered (done, enables the anchor).**
  `HSEngine.TMultiHexMOR.ReadWrite@5560b4ec` reads four ints (+0x34..+0x40, tags
  12..15); `SetdXmainP/SetdYmainP` name +0x34/+0x38, and
  `HSEngine.TMultiHexMO.Show@5560aaec` draws at `HXtoHP(hex) + those fields`, so
  **tags 12/13 are the sprite's `dXmainP`/`dYmainP` pixel shifts** (e.g.
  `sn_tree.BMP` id 87 -> `(-14,-27)`, grass mountain `0x0a75015a` -> `(-48,-47)`,
  `0x0809390e` -> `(-77,-51)`; the mountain shifts grow with the size index).
  Parsed into `MapResource::dx_main`/`dy_main` and locked by `mapres_test`.
  Tags 14/15 are still unidentified. This also confirms mountains are genuinely
  **multi-hex** objects
  (`Mountain.TMountainMO.CanPlace@556a2e54` walks a hex neighbourhood and rejects
  mountain overlaps unless `ValidMountainOverlap` agrees).
* **Phase 2.7 — mountain terrain block fixed (done, no RE).** `mountain_block()`
  mapped the resource `res_type` to a `MOUNTAIN.ILB` block with a *guessed* table
  that had snow/wall/desert permuted, so a snow map drew **desert** mountains and
  a wall map drew **snow** ones - an entire terrain type rendered with the wrong
  art. Cross-referencing each map's mountain `res_type` mix against its terrain
  tiles (`tools/mtmap.ps1`, 14 maps) pins the real table: 13=wall, 14=snow,
  15=desert, 16=stone, 17=grass. After the fix Frosty Lagoon draws 921 `mnt_sn`
  (was `mnt_de`), Blackwood Forest draws `wl_mnt`/`gr_mnt2`/`mnt_st`, Tutorial is
  unchanged, and `mapres_test` locks it (a no-desert map must draw no `Mnt_de`).
* **Phase 2.8 — object anchor applied (done).** Tags 12/13 (`dx_main`/`dy_main`)
  are now used as the object draw anchor, exactly as
  `HSEngine.TMultiHexMO.Show@5560aaec` prescribes: an object whose resource
  carries a shift is blitted at `origin(hex) + (dx_main, dy_main)` instead of the
  previous ad-hoc `(tile_w - w)/2` centering. This is a large change for the
  multi-hex objects: a grass mountain (`(-48,-47)`) and the volcano
  (`WL_VOLCL` `(-79,-64)`) move ~30-40 px from where the centering put them, and
  consecutive hexes' sprites (32 px pitch) now overlap into a continuous ridge
  instead of isolated mounds. Sprites with no shift (plain hexagon libraries)
  keep the visual-box placement. `mapres_test` still green (37/37). NB: the
  earlier "regression" recorded in Phase 3 was the *ILB visual-box* anchor
  (`base_x/base_y + xshift/yshift`); the resource-level `dXmainP/dYmainP` is a
  separate, decompile-proven shift and is applied on top of the hex origin.
* **Phase 2.9 — mountain size corrected (empirical fit).** `MOUNTAIN.ILB` packs
  five frames per terrain block (`60x50, 93x66, 93x66, 125x78/101, 188x118/128`)
  but each terrain exposes only **three** mountain resources (`res_index` 0/1/2).
  Comparing Tutorial's original render (`HowItShouldBe.png`) with ours showed the
  mountains were drastically under-drawn: rock/brown/slate/tan was **~5 %** of
  pixels where the reference has **~29 %** (rock/land 17 % vs 49 %). Drawing the
  three resources as the block's three *largest* frames (`mountain_block + 2 + want%3`)
  moves our rock/land to **~45 %**, matching the reference much more closely.
  `tools/img_hist.ps1` made this measurable without viewing the PNGs. NB: the
  exact size→frame rule still lives in `TMapObjectResource.ShowComplexImage`
  [TODO(RE)]; the current mapping is a documented empirical fit.
* **Phase 3 — object anchor recovered & implemented (done).** The Phase-3 blocker
  ("apply base+shift literally regresses objects") is resolved by decompiling the
  map-object draw path. `HSEngine.TMapObject.Show@55609a00` draws a *single-hex*
  object at `HXtoHP(hex) - 8`; `HSEngine.TMultiHexMO.Show@5560aaec` draws a
  *multi-hex* object at `HXtoHP(hex) + dXmainP/dYmainP`. In **both** cases the
  image's own `Show` adds the ILB visual-box offset `base + shift`
  (`TSprite16.ShowOpaque` = `param + base + shift`; `TRLESprite16.GetVisualDimensions`).
  So the earlier regression was the *missing `dXmainP`/`dYmainP`*, not the
  base+shift. The port now places every object with
  `aow::object_sprite_origin(hex_origin, resource, sprite.vx, sprite.vy)` =
  `anchor + base + shift`, where the renderer's `kHexFieldMargin` (8px grid
  margin) cancels the single-hex `-8` and is added for multi-hex objects. Before/
  after pixel diff = **26.75 %** (a real correction; mountains now form ranges and
  objects sit on their hexes). New exports:
  `re/exports/hsepack/{HSEngine.TMapObject.Show, Hexagon.THexagon.Show,
  HSEngine.TMapObject.MainPlace, HSEngine.TMapObjectResource.ShowComplexImage,
  HSEngine.TMapObjectResource.Draw, HSEngine.TMapObjectResource.GetCellImage,
  IsoHex.TIsometricHexagon.Show}.c`. `mapres_test` grew to **41** checks.
* **Known remaining gaps [TODO(RE)].** (a) `ShowComplexImage` now decompiled but
  not yet reproduced: objects with an image-sequence list (`resource +0x24 != 0`)
  are drawn as a *chain* of images (each frame at `anchor + sequence+0x24/+0x28`),
  so the empirical mountain-size mapping is still a fit `[TODO(RE)]`. (b) Mountain `res_type`s 10/22
  (underground twins) are unmapped. (c) ~~**Roads are missing entirely**~~ -
  **closed by Phase 4b below**: roads are placed `Road.TRoad` objects whose
  `Road.TRoadResource` carries its `Rd_*.BMP` overlay library *inline*, so they
  were never absent from the data - only unresolvable by name. Still open on the
  colour front: our forests read too dark vs the reference (reference `black`/land
  ~13 % vs ours ~29 %).
* **Phase 4 — land terrain transitions + coastline IMPLEMENTED (coast detail
  partially open).** The whole path is decompiled + exported. The *land↔land*
  half is in `mapviz`: for each hex it now reads the **real terrain number** from
  the resource's `TTerrainList` (`MapResource::terrain`, rw tag 7 - so water=0,
  land=1..5, ice=6, coast=14, border=15 are known, not guessed from the library
  stem) and, for each of the three upper edges (NW/N/NE = HN {6,1,2}), blits the
  `HEXTRANS.ILB` overlay `own*16 + 6 + edge + variant*3` at `(-6,-9) + base/shift`
  where the neighbour's terrain differs. The **coastline** is a separate, now
  implemented pass: the *water* hexagon composites the tan shelf
  (`WATERHEX.ILB` `t_wa-gr` slots 0..0x1e) toward its flat-land neighbours
  (`aow::water_shore_pieces`, from `IsoHex.TLowerIsometricHexagon.ShowDynamic`)
  at `HXtoHP + (-0x13,-0x20/-0x1f)`. `mapres_test` grew to **84** checks
  (terrain numbers, the shore-mask frame rule, and Tutorial L0's coastline).
  Verified against `HowItShouldBe.png`: the land/water mask aligns at 94.1 %, and
  the aligned sand fraction rises from 11.6 % (no shelf) to 13.0 % (shelf) vs the
  reference 15.7 %. Phase 4c then closed the water hexagon itself - its sixteen
  `H_Wa2` base wave frames, the `T_Ic-Wa2` shore foam, and the whole
  `ShowDynamic` animation (the `WATERANM.ILB` ripples / fish and the
  `SEFFECT.ILB` sparkles; `docs/18_WATER_ANIM.md`). Still open: the
  per-direction snow/wasteland shelf *slot offset* (`t_wa-sn`/`t_wa-wl`), the
  water animation's unidentified engine flags, and the land↔ice `T_ic-wa`
  "long" branches of `AoWHex.TAoWHexagon.UpdateTransition`.
* **Phase 5 — raised terrain DONE (base variants).** The underground is built from
  `IsoHex.TUpperIsometricHexagon` hexes (class `0x20170`, `EARTHHEX/ROCKHEX`); the
  engine draws `Show@55619e24` slot `(sprite+0x15 >> 6) + 0x28` = `hu_er` tops with
  cliff edges at 0x1e/0x1f/0x20. This exposed and fixed a real bug: ILB libraries
  are indexed by **image-slot id**, not array position (EARTHHEX skips 33..39), so
  every raised hex had been drawn as slot 0 (a cliff sprite). `mapres_test` 46;
  Tutorial L1 before/after diff 53.3 %. Edges still open.
* **Cross-cutting — inline image libraries (wrong-object fix).** Resources whose
  stored string is only a logical label (`gr_tree.BMP`, `gr_rocks.BMP`,
  `st_bones.BMP`, `Crops.bmp`, ...) carry a **complete ILB image inline** in
  `Release.hss` (TMapObjectResource rw tag 8 = magic `04 'I' 'L' 'B'`, v4.0).
  `TILManagerList.LinkToIL` only resolves exact uppercased relative *paths*, so
  the engine never loads those from disk - which is why the alias table produced
  wrong species (a big 5-tree cluster where a small tree belongs). `mapviz` now
  decodes the inline library (`IlbFile::load_memory`) and draws its slot 0, falling
  back to the named-file/alias path if it is absent or unparseable. `mapres_test`
  51; Tutorial L0 A/B diff **53.7 %** (mountains keep their named library).
  Roadmap note: **still open** `Rd_gr.BMP` (roads) - no inline library, no file.
* **Cross-cutting — image-sequence lists (hill vs mountain fix).** A multi-hex
  resource's **rw tags 9/10** hold an image-sequence list
  (`TImageSequenceList.LoadFromStream` = `u32 count` + per entry `u8 flag` then
  `TImageSequence.LoadFromStream`: rwString library, u8, u8 period, u8 mode, u32
  **frame**, u32 xoff, u32 yoff, [frame table if mode 4/5], u8 chain). It names the
  library AND the frame, and `ShowComplexImage` draws it at `anchor + xoff/yoff`.
  `map_resources`/`mapviz` now use it (highest priority), which supersedes the old
  empirical `mountain_block()+2+want%3`: the three grass mountain resources are
  `MOUNTAIN.ILB` frames **26 / 8 / 9**, and 26 is the 60x50 **HILL** mound
  (idx 25..29 of the file are the per-terrain hills) - previously drawn as a
  93x66 mountain. `mapres_test` 56. `mountain_block` remains only as a fallback
  for resources with neither an inline library nor a sequence.
* **Phase 4b — roads IMPLEMENTED (done, RE).** Roads are *placed objects* (engine
  class `0x20164` `Road.TRoad`, matching resource class `0x20165`
  `Road.TRoadResource`). The resource's logical name `Rd_gr.BMP` matches no file or
  ILB entry, which is why the old renderer dropped all of them - but the resource
  carries its overlay library **inline** (rw tag 8), a 6688-byte ILB with 8 entries
  named `Rd_gr.BMP` at image slots **1..8** (60x50 cells). The decompiled path is:
  `Road.TRoad.Show@55616ed4` anchors the overlay at `HXtoHP(hex) + (-14,-9)`;
  `Road.TRoad.UpdateTransitions@55616f8c` recomputes a transition byte from the six
  neighbouring fields (bit `dir-1` set per adjacent road, then the E/W pair
  collapses to bit 6 and the NE/NW pair to bit 7); `Road.TRoadResource.Show@5561713c`
  draws the matching slots (1..6 = arms, 7/8 = the straights, 7 also the isolated
  tile). `AoWRoad.TAoWRoad.UpdateTransitions@5579b84c` is the loaded-game override -
  same algorithm plus a static-scene invalidation, **and** it counts a neighbour as
  road-connected when the tile answers the road query message `0x1131`
  (`AoWE.TStructure.MapFieldMsgProc@5575f730`, resource byte `+0x45`), so roads lead
  into cities/buildings too (`AoWE.TStructure.UpdateRoads` re-runs the neighbours
  when a structure is placed). `mapviz` reproduces all of it
  (`aow::road_transition_byte`, `aow::road_frames`, `aow::road_connectable_object`,
  the road pass before the object pass in `src/engine/mapviz.cpp`, gated by
  `--objects`), and `object_library_alias()` maps `Rd_<terrain>` ->
  `H_<Terrain>Dr.ILB` as a fallback. Tutorial level 0 draws 39 road hexes; 14 roads
  grew an arm toward an adjacent structure (cities, mine, tower, cave, wizard tower,
  teleporter, monster lair) once structures are counted. New exports:
  `re/exports/hsepack/{Road.TRoad.Show, Road.TRoad.UpdateTransitions,
  Road.TRoad.UpdateNeighbourTransitions, Road.TRoadResource.Show,
  Road.TRoadResource.ClassID, HSEngine.TAbstractRoadResource.MsgProc,
  HSEngine.HNtoHX, HSEngine.HXtoHP}.c`,
  `re/exports/aowepack/{AoWRoad.TAoWRoad.UpdateTransitions,
  AoWE.TStructure.MapFieldMsgProc, AoWE.TStructure.UpdateRoads}.c`. `mapres_test`
  grew to 82 checks (road resource/inline-library shape, the transition byte for
  isolated / E-W / NE-NW / N-S cases, the frame selection, structure
  road-connectability, the mine arms, and the Tutorial render).
* **Phase 4c — structure placement via the image sequence (done, RE).** A structure
  with an image-sequence list (`resource+0x24`, rw tags 9/10) is drawn by
  `AoWE.TStructureResource.Show@557605dc` through `ShowComplexImage` at
  `HXtoHP + sequence xoff/yoff + base + shift` - the *sequence offsets* are the
  authoritative placement, not the multi-hex `dXmainP/dYmainP` (tags 12/13), which
  is only a fallback for the single-image branch. The port was parsing the sequence
  name at a fixed offset, so it failed for every record whose tag-9 head is followed
  by **two** housekeeping bytes instead of one (City3 = one, Mine/Tower/
  WizardsTower = two) and silently fell back to `dXmainP`. The wizard tower is the
  clearest casualty: its 120px-tall sprite carries `xoff/yoff = (-10,-90)`, so the
  fallback (`(0,0)`, no tags 12/13) dropped it ~90px below its hex. `load_map_resources`
  now scans a few positions for a plausible length-prefixed library name, so the
  sequences parse for all of them: Mine `(-16,-22)` (≈ its old `dXmainP`, so it does
  not move), Tower `(-9,-48)` (was `(-11,-31)`), WizardsTower `(-10,-90)` - each now
  grounds on its hex. A first attempt at giving structures a *different* anchor
  (`HXtoHP`, no `dXmainP`) was wrong and reverted; the shift was right, the missing
  sequence was the bug. Evidence: `re/exports/aowepack/AoWE.TStructureResource.Show.c`.
  `mapres_test` grew to 84 checks (the three structure sequences + their offsets).
  **Still open `[TODO(RE)]`:** the structure frame index `MO+0x14` (the object's own
  cell image - e.g. the city race/wall variant), and the `map+0x40 & 6` branch flag.
* **Phase 4d — bridges (done).** A bridge is drawn by `HxBridge.THexagonBridge`
  (`AoWEPACK`, class `0x2087a`); its resource is `HxBridge.THexagonBridgeResource`
  (class `0x2087b`), a **single** 78x65 sprite with a **three-frame** library
  carried INLINE (rw tag 8): `Str_brdg.bmp` on the surface (2 records) and
  `U_brdg.bmp` underground (3 records), `res_index` 24..28. The scenario stores
  only the resource id, and `Create@5579973c` initialises the orientation byte
  `+0x11` to `0xff`, so the engine recomputes it on load from the terrain:
  `UpdateTransitions@55799fd8` calls `DirectionValid@55799c04` in the order
  **1, 2, 0**; `DirectionValid(d)` is true when the neighbours at HN `(d+1)` and
  HN `(d+4)` are both land. On the column-staggered grid
  (`HSEngine.CenterHNtoHX@5560e17c` / `HNtoHX@5560e08c`) those are the three
  opposite pairs **N/S, NE/SW, SE/NW** - exactly the three frames the art shows.
  `Show@55799a70` then draws `Get(resource+0x20, +0x11)` at
  `HXtoHP + (-0x17, -0x14)`. `mapviz` reproduces all of it:
  `aow::bridge_land` (`AoWE.Land@5575aea4` excludes 0/6/9/10/13),
  `aow::hex_bridge_direction` (order 1,2,0, `0xff` invalid) and the Phase 4d
  pass (frame = direction). Evidence:
  `re/exports/aowepack/{HxBridge.THexagonBridge.UpdateTransitions,
  HxBridge.THexagonBridge.DirectionValid, HxBridge.THexagonBridge.Show,
  HxBridge.THexagonBridge.ClassID, HxBridge.THexagonBridgeResource.ClassID,
  AoWE.Land}.c`, `re/exports/hsepack/HSEngine.CenterHNtoHX.c`. `mapres_test`
  grew by 17 checks (the 5 bridge resources + 3-frame inline library,
  `bridge_land`, the direction rule and one/two-pair priority, the anchor, and a
  Dwarf1 L1 integration render: all 6 bridges draw frame 0..2 from `U_brdg.bmp`).
  **Note:** the older `Bridge.TBridge`/`TBridgeResource` (HSEPack classes
  `0x20174`/`0x20175`, `Wa_bdge.BMP`/`Ula_bdge.BMP`, a 19-frame multi-piece
  overlay with its own `Bridge.TBridge.UpdateTransitions@55617b60`) is the base
  engine variant and is **not** placed by the shipped scenarios.
* **Phase 4e — cities: race bodies, walls and the pad "hill" (done, RE).** A city
  (`City.TCity`, class `0x20108`) is **not** one sprite and is **not** drawn by the
  generic object path. `AoWE.TCity.Show@557aca88` converts its hex with `HXtoHP`
  and hands the raw position to `AoWE.TCityResource.ShowCity@557ade3c`, which adds
  the resource `dXmainP/dYmainP` (`+0x34/+0x38`) and blits **three** layers:
  back wall (`city+0x68`), body (`city+0x70`), front wall (`city+0x6c`).
  `City.TCity.UpdateImages@557ac494` fills them:
  * the body is **race-specific**: `TRaceResource.GetCityImage@55759ebc` returns
    the race library's slot `terrain == 5 ? size + 0x3b : terrain*10 + size + 0x45`,
    else the fallback `size + (wallFlag == 0 ? 0x32 : 0x3c) - 1`, else nothing;
  * the walls come from the city *resource's own* library (`CityN.ILB`), block
    offset `T + wallType*2`; `T` is 0 (surface), `0x14` (snow) or `0x28`
    (wasteland/earth/rock/lava/upper);
  * wall types 0/3 mean no walls; a razed city clears all three.
  The **race** and **wall type** are serialised on the city (`TCity.ReadWrite`
  rw tags **0xd/0xf** → `+0x45`/`+0x4c`), and the race index selects one of the
  game pack's **12 race resources** (`TAoWWHSSet` rw **tag 10** → `+0x54`
  `TRaceResourceList`; each `TRaceResource`'s rw **tag 8** names `RACES\R*.ILB`,
  in faction order - Human 0, Orc 9, Goblin 10). A **race-less city** (rw tag 0xd
  = -1/0xff, what the editor writes for a city whose race was never picked - every
  city of a map built from scratch, e.g. `Scenario\Custom\test.hsm`) has no race
  of its own, so `GetCityImage` returns nothing and `UpdateImages` falls back to
  the resource's `C_random` "?" block; an **owned** city resolves to its owner's
  race instead (`TCity.UpdateFlagID@557ac8b0` builds the very same city's *flag*
  byte out of `TPlayer.GetRace`, `+0xa5`), which is what `aow::city_body_race`
  feeds to the body: the port's `Barondir Bay.HSM` Human town at (12,20) and the
  Human player's `test.hsm` city now draw the Human art under their Human flag,
  while an unowned race-less city keeps the "?" placeholder. The **hill** the
  city sits on is
  a *separate* object, `Pad.TPad` (class `0x20106`, resource `Pad.TPadResource`
  `0x20107`, a `TMultiHexMOR`): `PADn.ILB` slots 1..5 are the
  grass/desert/snow/stone/wall mounds and the map already carries one under every
  city, so the port only had to stop drawing the wrong (generic, unwalled) body on
  top of it. `mapviz` now reproduces all of the above (`draw_city`,
  `city_image_slot`, `city_image_fallback_slot`, `city_terrain_block`,
  `city_resource_size`, `city_body_race`, `race_image_libraries`). Evidence:
  `re/exports/aowepack/{AoWE.TCity.Show, AoWE.TCityResource.ShowCity,
  AoWE.TCity.UpdateImages, AoWE.TCity.{SetRace,SetWallType,ReadWrite,GetSize},
  AoWE.TCityResource.{ClassID,ResourceUserClassID,ActivateMsg},
  AoWE.TRaceResource.{GetCityImage,ReadWrite,LinkToIL,ClassID},
  AoWE.TAoWHSSet.ReadWrite, Pad.TPad.ClassID, Pad.TPadResource.{ClassID,
  ActivateMsg}, Pad.TPad.ForceTerrainType, Hill.THillMO.ClassID,
  Hill.THillMOR.{ClassID,CreateResourceUser}}.c`. `mapres_test` grew 17 checks
  (12 race libraries + order, the GetCityImage/block rules, the CityN sizes, and a
  Tutorial L0 integration: the walled Goblin town at (32,21) and the Halfling city
  at (16,25) both draw a `RACES\R*.ILB` body and each sits on a `Pad` object) plus
  11 for the race-less/owner rule (`city_body_race`'s eight cases and a `Barondir
  Bay.HSM` L0 render: the walled Human-owned town at (12,20) draws
  `RACES\RHUMAN.ILB` slot 63 `c_human.bmp` and no `c_random.bmp`, the unowned
  race-less village at (29,20) still draws `CITY1.ILB` slot 1 `c_random.bmp`).
  **Still open `[TODO(RE)]`:** the exact `TMapField+0x12` "wallFlag" byte (the port
  uses the city's wall type), the pad frame (always the grass mound here; the
  engine picks it through the sequence) and the razed / under-construction
  overlays (`TCityOverlayHS.Show`).
* **Phase 4f — a structure's body is the effect's *chain*, not the list's first
  name (done, RE; this is why PORTS were missing).** A map-object resource's
  image-sequence list (rw tags 9/10) may **open with an additive-effect
  sequence**: the `Shipyard.TShipyardResource` (class `0x204bf`, ids `0x3c`/`0x6b`)
  list's entry 1 is `SMOKE.ILB` (mode 4, 12-frame table, `xoff/yoff = (-2,-28)`)
  whose `.next` chain (`+0xc`) carries the dock itself - `PRO_SYRD.ILB`, mode 0,
  base frame 0, `xoff/yoff = (-19,-25)`.
  `HSEngine.TMapObjectResource.ShowComplexImage@5560a134` walks that chain (as does
  the sequence branch of `AoWE.TStructureResource.Show@557605dc`) and draws every
  sequence whose `ImageLib.TImageSequence.GetFrameIndex@0x552186ec` is not `-1`.
  The old loader scanned the blob for the first library *name*, so it recorded
  `SMOKE.ILB` as the Shipyard's library - and `mapviz`'s `is_transient_effect()`
  then discarded the whole object: **not one port/shipyard was ever drawn**.
  `load_map_resources` now parses the *whole* `TImageSequenceList` with
  `aow::parse_image_sequence_list` (so the NULL placeholder at entry 0 and the
  `.next` chains are reconstructed) and the new `aow::body_sequence()` selects the
  body - the first sequence, entries in list order and each entry followed by its
  chain, whose library is not an effect (`aow::is_effect_library`, the single
  definition the renderer's `is_transient_effect` now delegates to). `has_seq` /
  `seq_lib` / `seq_frame` / `seq_dx` / `seq_dy` describe that body, and the object
  pass only drops an object when the sequence it resolved really is an effect
  (`!Picked::by_seq`); the `--debug-csv` `ilb` column now reports the library
  actually drawn rather than the resource's raw first name. Verified on
  `Underground Titans` L0: the 4 `Shipyard.TShipyard` hexes now draw
  `PRO_SYRD.ILB` `pro_syrd2.bmp` (`re/…` crop checked) and the before/after CSV
  diff is **exactly those 4 rows** - every other object keeps its library and
  frame. Evidence: `re/exports/hsepack/{HSEngine.TMapObjectResource.
  ShowComplexImage, HSEngine.TMapObjectResource.GetCellImage, HSEngine.
  TMapObjectResource.Draw}.c`,
  `re/exports/anim/ILPACK_dpl/{TImageSequenceList,TImageSequence}.LoadFromStream.c`,
  `re/exports/aowepack/{AoWE.TStructureResource.Show, AoWE.TStructureResource.
  ReadWrite, PrdPlace.TProductionPlace.ReadWrite, Shipyard.TShipyardResource.
  Create}.c`. `mapres_test` grew 5 checks (the Shipyard's list opens with
  `SMOKE.ILB` + chain, `body_sequence` returns `PRO_SYRD.ILB`, its offsets, and
  `is_effect_library`).
  **Still open `[TODO(RE)]`:** the object's own **cell image** (`MO+0x14`, the
  sequence *start* index `ShowComplexImage` is handed, i.e. the built /
  under-construction / razing state) - the port uses "first non-effect sequence"
  as the body proxy, which matches `STR_MINE` / `STR_TOWR` / `STR_WTWR` /
  `PRO_SYRD`, and therefore does not yet draw a structure's razing state.
* **Phase 4g — building animation (done, RE).** `ShowComplexImage` draws the whole
  sequence chain, and the chain carries the moving part (a mine's mode-5 sparkle,
  a wizard tower's mode-4 orb, a teleporter's shimmer, an animated node/farm/cave
  body). `mapviz` now draws the body **and its chain**, each sequence framed by
  `ImageLib.TImageSequence.GetFrameIndex@0x552186ec` from the map clock
  (`AoWHSMap+0x40`) and the per-object phase `hex_x + hex_y*0x10`
  (`aow::object_anim_phase`; `AoWE.TStructureResource.Show@557605dc`). Two new
  options: `MapVizOptions::clock` (a one-shot render bakes the frame at that clock)
  and `no_animated_overlays` + `draw_structure_overlays` (a live front-end keeps
  the static level cached and re-composites only the animated sprites each map
  frame, since a level re-render is ~28 ms). A third pair, `no_units` +
  `draw_unit_overlays`, gives the placed armies the same treatment: they are
  composited **after** every structure overlay and the water, so a unit standing
  next to a building - or on its hex, Tutorial L0's Orc hero at the Goblin city
  (32,21) - is never covered by the building's glow or its owner flag. The viewer ticks its own map-clock
  timer; the in-game screen does it in `game_update`. CLI `aow_view --clock N`,
  `AoW.exe --game-clock N`. Proofs: `bin/BuildAnimView_c0/c12.png`,
  `bin/BuildAnimGame_c0/c12.png` (the build smoke test asserts the pairs differ);
  `mapres_test` +20 checks (incl. the per-building background audit: 127
  structures, 177 frames, 0 opaque). The animated frames are class-0x16
  `TSprite16`, always blitted colour-keyed (`TSprite16.ShowOpaque` ->
  `GFXE.BltColorKey16`); `IlbEntry::color_key` carries the key and `decode_rgb`
  turns it into alpha, so the buildings no longer draw their backgrounds
  (`tools/buildings_sheet.py` renders the checkerboard sheet). See
  `docs/17_BUILDING_ANIM.md`.
  **Still open `[TODO(RE)]`:** modes 3/5 pick a frame randomly (the Delphi RNG),
  so a mode-5 overlay draws but does not flicker; additive effect sequences
  (`SMOKE.ILB`/`TWINKGR.ILB`) are still skipped.
* **Phase 4h - the owner flag of a takeable structure (done).** The
  `TFlagControl.ShowFlag`/`ShowStructureFlag` draw is reproduced for *every*
  ownable structure (not just a city): a flagpole from `Flags.ILB` plus the
  **owning player's race flag** from `RACES\R*.ILB` (frames 0..11 as a one-hex
  structure, 12..23 as a bigger one/the neutral `Flags.ILB` pair otherwise),
  anchored at `HXtoHP + resource(+0x68,+0x6c)` with the flag `(-0x15,-0x10)`
  of the pole, animated off `(map_clock + HXtoHP.x) >> 1|2 % 12` and drawn
  *behind* the building. The ownable subtree was read out of the module's
  Delphi VMTs (new tool `tools/vmt_scan.py`), the per-class resource defaults
  out of each `Create` chain, and the owner out of the object's rw tag 5 (a
  city also reads its size tag 0x10 and razed tag 0x16). New front-end work:
  `draw_structure_overlays` now also re-composites the flags (cities included)
  every map frame. See `docs/19_STRUCTURE_FLAGS.md`; `mapres_test` +16 checks.
* Phases 6–8 — not started.

## How to validate every step (do not skip)

```powershell
cd 'C:\GOG Games\Age of Wonders\port'
./build.ps1 -Test -Viewer          # builds AoW.exe + aow_view.exe, all suites green
./bin/aow_view.exe '..\Scenario\Custom\Tutorial.hsm' `
    --root '..' --level 0 --render '.\bin\check.png'
```

Constraints that must hold after *every* phase:
* C++17, `g++ -std=c++17 -Wall -Wextra -Wpedantic`, **zero new warnings**.
* stdlib-only (plus Win32/GDI for the viewer). No third-party deps.
* The head-less contract stays intact: `aow_view <map> --root <dir> [--level N]
  [--no-objects] [--max N] --render <out.png>`.
* Keep the 4 suites green (`pfs_types_test`, `map_test`, `mapres_test`,
  `aow_view` smoke).

## Phase overview

| # | Phase | Needs RE? | Fidelity impact | Depends on |
|---|-------|-----------|-----------------|------------|
| 0 | Instrumentation / reference harness | no | — (measurement) | — |
| 1 | Kill backdrop gaps + verify tile geometry | no | High (removes black) | 0 |
| 2 | Correct base-tile selection in mixed libraries | no* | High (water) | 0 |
| 3 | Object sprite anchoring (`xshift`/`yshift`) | ~~yes (anchor)~~ **done** | High (objects) | 0 |
| 4 | Terrain transition/fringe frames | yes (RE done; **land↔land + roads implemented**) | **Highest** (coastlines) | 1,2 |
| 5 | Elevation / upper-hex (cliff) layer | yes (base **done**, edges open) | High (3-D look) | 4 |
| 6 | `THSMap` fields + player/race lists | **yes** | Medium (data) | — |
| 7 | Layer order / z-sort / clipping | no | Medium | 3,4,5 |
| 8 | Viewer UX parity (optional) | no | Cosmetic | 1-7 |

\* Phase 2 starts as a name heuristic; Phase 4 may supply the authoritative rule.

## Phase 0 — Instrumentation / reference harness (no visual change)

**Goal:** make progress measurable and regressions visible.

1. Add a head-less "render N maps at fixed options" helper (or a small
   `src/tests/mapviz_golden.cpp`) that renders `Tutorial.hsm` (level 0),
   `Dwarf1.CSM` and `Underground Titans` to PNGs under a fixed output dir.
2. Add a per-hex **diagnostic dump** to `mapviz.cpp` behind a flag/option:
   for each tile emit `x,y,resource_id,ilb,frame,sprite_w,sprite_h,fallback?`
   to a CSV. This is what tells you *which* hexes fall back and why.
3. Capture **reference screenshots** of the same maps from the real game /
   editor, stored under `port/docs/refs/` (or a noted external path) to eyeball
   against.

**Files:** `src/tests/mapres_test.cpp` (extend), new optional
`MapVizOptions::debug_csv`. **Acceptance:** the harness runs from
`./build.ps1 -Test` and produces deterministic PNGs.

## Phase 1 — Kill backdrop gaps + verify tile geometry (no RE)

**Goal:** no unexplained black/near-black regions; correct hexagon size/step.

* `mapviz.cpp:295-298` only fills a fallback diamond when the resource resolves
  to *some* library. Make the fallback **unconditional**: if `pick()` is null,
  draw a per-terrain-colour diamond even when `find_map_resource()` is null
  (currently it can leave the `(12,12,20)` backdrop). See the canvas pre-fill at
  `mapviz.cpp:255-256`.
* **Geometry - done.** `tile_w=48, tile_h=32` are confirmed against the decoded
  art (`detect_tile_size`); the grid is the engine's `HXtoHP` transform
  (`px = x*32 + 8`, `py = y*32 + (x & 1)*16`) - axis-aligned, 32x32 pitch,
  column-staggered - so neighbouring columns interlock over the 16px flat edge
  with no seam. The old 45-degree iso diamond (`step_x=24, step_y=16`) and the
  full-width `fill_diamond()` approximation were replaced by `fill_hex()`, whose
  45-degree flanks exactly match the 48x32 sprite footprint.

**Tests:** add a `mapres_test` assertion that the rendered level has **no
backdrop-colour pixels inside the hex field**.
**Acceptance:** `check.png` has no large black areas; tile edges meet cleanly.

## Phase 2 — Correct base-tile selection in mixed libraries (no RE to start)

**Goal:** water (and any other mixed library) shows its real base tile, not a
transition overlay.

* `WATERHEX.ILB` (`Images/Waterhex.ILB`) holds `t_wa-*` transition overlays
  **first**, then the `H_Wa2.BMP` base hexes. `pick()` indexes
  `frames[frame]` (`mapviz.cpp:275-277`), so water renders the `t_wa-gr` khaki
  lattice (`docs/01_FORMATS.md:531-536`).
* Fix: when resolving a terrain sprite, choose the **base entry by name** --
  skip `t_*`/transition entries and pick the `H_Wa*` (or class-appropriate)
  entry. `ilb_entry_names()` (`src/include/aow/ilb.hpp:90`) already enumerates
  entry names without touching pixel data; use it to build a name->index map per
  library and select the base index instead of `frame`.
* Generalise the rule for all terrain classes
  (`0x20153/0x20171/0x20558/0x2073e`).

**Tests:** a water tile resolves to an `H_Wa*` entry, **not** a `t_wa*` entry.
**Acceptance:** water reads as water; no khaki lattice.

## Phase 2.5 - Object sprite restoration (done, no RE)

**Goal:** the map's objects (trees, rocks, crops, ...) actually render.

* The retail resource table names the terrain-object libraries *logically*
  (`gr_tree.BMP`, `Rd_gr.BMP`, `gr_rocks.BMP`, `st_bones.BMP`, `Crops.bmp`,
  `gr_willow.BMP`, `sn_tree.BMP`, `wl_rocks.BMP`, ...). None of those names is a
  file or an ILB entry on disk - the sprites live in abbreviated files
  (`gr_tree.BMP` -> `Images/TCombat/Gr/zgr_Tre.ILB`, entries `zgr_Trees5.BMP`).
  `LibResolver::resolve` now falls back to `object_library_alias()`
  (`src/engine/mapviz.cpp`), a curated `logical -> file-stem` table covering the
  grass/stone/snow/desert/wall/underground families.
* `MOUNTAIN.ILB` packs five 5-frame terrain blocks (desert `Mnt_de`, grass
  `gr_mnt2`, snow `Mnt_sn`, stone `Mnt_st`, wall `wl_mnt`). The engine picks the
  block from the hex's terrain at runtime; the stored `res_type` selects it, so
  `mountain_block(type)` maps `14->0, 17->5, 13->10, 16->15, 15->20` and the
  frame is `block + res_index%5`. Verified: grass hexes draw `gr_mnt2`, stone
  hexes `Mnt_st`.
* Transient additive effects (`EFFECTS\TWINKGR.ILB` `GreenTwinkle`, `SMOKE.ILB`)
  are composited with a blend mode in the engine; a plain opaque blit left a
  dark square. `is_transient_effect()` skips them.

**Tests:** Tutorial level 0 resolves 319 of 362 objects (was ~120); the golden
render (`re/golden/Tutorial_L0.png`) now shows trees/rocks/crops.
**Acceptance:** the map is no longer bare, mountains match the terrain type,
and no opaque effect squares remain. Remaining: `Rd_gr.BMP` (roads, 39) has no
library on disk.


## Phase 3 — Object sprite anchoring: `xshift`/`yshift` (done) — fields + anchor recovered

**Goal:** mountains/forests/cities/units land exactly where the engine puts them.

* **Recovered (done).** The ILB descriptor carries a sprite's placement box:
  * `base_x`/`base_y` = descriptor +0x28/+0x2c — the base `TLibraryImage`
    margins (the allocated bitmap is `(+0x18 + base_x) x (+0x1c + base_y)`).
  * `xshift`/`yshift` = +0x50/+0x54 for class 0x11 (`TRLESprite16`), +0x5c/+0x60
    for 0x16 (`TSprite16`); the visual box is
    `(base_x + xshift, base_y + yshift, rle_w, rle_h)`.
  * Evidence: `re/exports/ilpack/TRLESprite16.GetVisualDimensions.c`,
    `TRLESprite16.ReadImageData.c` and `TSprite16.ShowOpaque.c`
    (`dest = param + base + shift`). Parsed into `IlbEntry`
    (`src/include/aow/ilb.hpp`, `src/engine/ilb.cpp`).
* **Calibration (done).** `H_Gr.ILB` base hexes are `48x32` at `base=(0,0)`,
  `shift=(0,0)` — terrain content is drawn exactly on the tile origin.
  `zgr_Tre.ILB` shows objects carry a non-zero shift (e.g. `55x60 @ (29,11)`).
  `mapres_test` asserts both.
* **Anchor — recovered (done).** The map-object draw path is
  `HSEngine.TMapObject.Show@55609a00` (single-hex: `anchor = HXtoHP - 8`) and
  `HSEngine.TMultiHexMO.Show@5560aaec` (multi-hex: `anchor = HXtoHP +
  dXmainP/dYmainP`); the sprite's own `Show` then adds `base + shift`
  (`TSprite16.ShowOpaque` = `param + base + shift`). The earlier "regression" was
  applying base+shift without the multi-hex `dXmainP/dYmainP`. `mapviz` now uses
  `aow::object_sprite_origin()` for every object; sprites with no resource shift
  (plain hexagon libraries) get the base+shift placement too, instead of the old
  ad-hoc centring. See the progress bullet above for the evidence and the 26.75 %
  before/after diff.

**Tests (done):** `mapres_test` asserts the `H_Gr` base hexagon is `48x32 @
(0,0)` and that `zgr_Tre` object sprites carry a non-zero shift.
**Acceptance:** object positions match the reference screenshot.

## Phase 4 — Terrain transition / fringe frames (RE) -- mechanism recovered, implementation re-scoped

**Goal:** coastlines, terrain boundaries and fringes appear; hexes of one type
stop looking identical.

**Status: the transition mechanism is now fully decompiled** (this session). The
roadmap's original assumption ("chosen at runtime from the neighbours in
`Hexagon.THexagon.UpdateTransitions`") was **wrong in detail**: `UpdateTransitions`
and `UpdateTransition` on the plain `Hexagon.THexagon` are empty; the bytes are
computed by the field/terrain layer and the boundaries are drawn by the *water*
hexagon. Key findings (all exported under `re/exports/`):

* **The overlay library is a constant.** `Hexagon.THexagonResource.MsgProc@55611e50`
  links `TImageLibraryManager.LinkToIL(..., 1, "HexTrans.ILB")` into
  `resource +0x34`. `HEXTRANS.ILB` (129 entries) holds the coast/fringe sets:
  `T_co-wa`, `T_gr_co`, `T_DE`, `T_GR`, `T_SN`, `T_ST`, `T_WL`, ...; the five
  land families `T_GR/T_DE/T_SN/T_ST/T_WL` occupy 9-frame blocks at idx
  22/38/54/70/86 (16 apart).
* **The per-hex transition bytes are NOT in the .HSM.** A tag-50 terrain record
  carries only tag 1 (its resource id) - verified by dumping `Tutorial.hsm`
  (border/land/water). The sprite bytes are set at runtime.
* **Where the terrain type lives.** `HSEngine.TMapField` keeps `terrain` at
  `+0x14` and `overlay` at `+0x15`; `TMultiHexMO.OverrideTerrain@5560b2d4` writes
  them from the terrain object (`GetTerrain`/`GetOverlay`, slots +0x4c/+0x58)
  then calls `TMapField.TerrainChanged@55607cb4`, which notifies each hexagon
  (slot +0x9c/+0xa0) and propagates to the 6 neighbours
  (`UpdateNeighbourTerrain@55607f9c` -> `NeighbourTerrainChanged@55607f34`, with
  the direction-code table `DAT_5562e248`).
* **The overlay draw path (land).** `Hexagon.THexagonResource.Show@55611fa4`
  draws the base at `(HXtoHP-8)` then up to 3 overlays from `resource+0x34`
  (`HexTrans.ILB`), selected by `sprite+0x13` bits 0/1/2 (edge A/B/C) with bits
  3/4/5 choosing short vs long offsets:
  A/B/C short `(-6,-9)`; long A `(-6,-41)`, B `(-38,-25)`, C `(26,-25)`. The
  overlay **frame is the neighbour's terrain index** stored in `sprite+0x10/11/12`.
* **The coastline (water) is its own path.** `AoWHex.TAoWWaterHexagon`:
  `UpdateTransitions@5579ad98` sets mask `+0x15` bits where a neighbour has a
  cliff (`TLowerIsometricHexagon`/`TBorderHexagon`) with a *different* terrain;
  `Show@5579aeac` then draws the 6 overlays from the water animation library at
  `(-6,-9)` with hard-coded frames `0x24`/`0x25`/`0x23` for mask bits 0/1/5, plus
  the animated base frame `(x + y*2 + phase) & 0xf + 0x8c`.

**The land transition rule is fully recovered (this session).** The empty
`Hexagon.THexagon.UpdateTransition@55611de8` (vtable slot +0x120) is only
HSEPack's *base*; the loaded game's land hexagon is AoWEPACK's
`AoWHex.TAoWHexagon`, whose `UpdateTransition@5579a9e0` computes the real thing,
once per edge (0/1/2). Per edge it maps the edge to a direction (`HN = {6,1,2}`
via `HSEngine.HNtoHX`; table `DAT_558e8dd8`), finds the neighbour, and compares
`ownterr = this->GetTerrain(0)` (the hexagon RESOURCE's `TTerrainList` entry 0 at
`resource+0x28` / rw tag 7 - `AoWHex.TAoWHexagonResource.MsgProc@5579ace0`) with
`nterr = neighbour.field+0x14`. When they differ (and neither is 7/8/0xF) the
overlay frame is **`ownterr*16 + 6 + edge + variant*3`** (variant ∈ {0,1,2}) for a
plain land↔land boundary, plus special cases for the water/coast terrain numbers
0/6/9/10/13 (full switch in
`re/exports/aowepack/AoWHex.TAoWHexagon.UpdateTransition.c`). It stores the frame
at `sprite+0x10/11/12`, sets `sprite+0x13 |= 1<<edge` and, for the "long"
variants, `|= 1<<(edge+3)`; `Hexagon.THexagonResource.Show@55611fa4` then blits
the overlays from `HexTrans.ILB` (`resource+0x34`) at `(-6,-9)` (long: A
`(-6,-41)`, B `(-38,-25)`, C `(26,-25)`). **This predicts `HEXTRANS.ILB`'s layout
exactly**: `T_GR`(22)/`T_DE`(38)/`T_SN`(54)/`T_ST`(70)/`T_WL`(86) are
`ownterr` 1..5, and each 9-frame block is edge (0..2) × variant (0..2).

**Water/coast:** `AoWHex.TAoWWaterHexagon.UpdateTransitions@5579ad98` sets mask
byte `+0x15` where a neighbour has a cliff/border with a different terrain, and
`Show@5579aeac` blits three **small** detail sprites from the water sequence
library at `(-6,-9)` with hard-coded frames `0x24/0x25/0x23` - their visual boxes
are only 13x8 / 22x18 / 20x16 in `WaterAnm.ILB`, i.e. shore foam, not full fringes.

**Implemented (land↔land).** `mapviz` now has a transition pass after the terrain
pass:
* terrain family from the base hexagon resource library stem
  (`terrain_type_of()`: `h_gr`→1, `h_de`→2, `h_sn`→3, `h_st`→4, `h_wl`→5), which
  matches `own*16 + 6` and the observed block order;
* three upper edges, offsets by column parity (even x: `(-1,-1)/(0,-1)/(1,-1)`;
  odd x: `(-1,0)/(0,-1)/(1,0)`) = NW/N/NE = HN {6,1,2};
* frame `own*16 + 6 + edge + variant*3` out of `HEXTRANS.ILB`, blitted at
  `(-6,-9) + base+shift`. The engine's `RandInt(3)` variant is replaced by a
  positional hash so the render stays deterministic;
* only the upper three edges are drawn: a hex sprite's upper edge is the one that
  lies over its northern neighbour, so the neighbour's lower edge is never needed.
* Evidence/tests: `mapres_test` (44) asserts Dwarf1 draws >100 transitions, all
  from `HEXTRANS.ILB`, every frame in 22..94; A/B pixel diff 0.83 % with
  `--no-objects`.

**Implemented (the water hexagon, Phase 4c — `docs/18_WATER_ANIM.md`).** The
coast/water path is now complete:
* the `T_Ic-Wa2` shore foam (slots 0x23/0x24/0x25, mask bits 0/1/5 of
  `sprite+0x15`) at the base tile's `(-6,-9)` — drawn where a neighbour of the
  hexagon's own water/ice family (`IsoHex.TLowerIsometricHexagon`) has a
  *different* terrain, i.e. on the water/ice boundary (originally "left out
  rather than guessed"; the box is ~25x22, not a full fringe). Icedale L0 draws
  84 of them, from `WATERHEX.ILB` and `ICEHEX.ILB` alike;
* the base tile is one of `WATERHEX.ILB`'s sixteen `H_Wa2` wave frames
  (`0x8c..0x9b`), picked per hex by `Show`'s
  `((field>>3) + x + y*2 + (sprite+0x15>>6)) & 0xf + 0x8c`;
* **the animation**: `AoWHex.TAoWWaterHexagon.ShowDynamic`'s private per-hex
  counter (`+0x18`), which plays one one-shot entry of the resource's sequence
  list (`+0x19`) — the ten `WATERANM.ILB` ripples, the `Fish1-*`/`FishSplas1-*`
  chains and the `TCOMBAT\OBJECTS\SEFFECT.ILB` sparkles — plus the sleep after it
  runs out. `aow::WaterHexAnim` + `aow::image_sequence_unlooped_frame` +
  `aow::draw_water_overlays`, with the live screens re-compositing their viewport
  once per map frame (0.70 ms) and the one-shot render baking it at
  `MapVizOptions::clock`.

**Still open:** the per-direction snow/wasteland coastline shelf *slot offset*
(the 0x28/0x50 variant bytes are computed, but the shelf is still drawn from the
`t_wa-gr` slots instead of shifted into `t_wa-sn`/`t_wa-wl`), the water
animation's unidentified engine flags (`engine+0x6d`, `map+0x3c & 2`,
`map+0x188`), the water/coast special branches of the *land* transition (terrain
0/6/9/10/13), and the "long" variants (flag bit `edge+3`, offsets
`(-6,-41)/(-38,-25)/(26,-25)`).

**Superseded note (kept for context):** the earlier reading - "plain land hexes
have NO transition" - was based on HSEPack's empty base method.
The trigger is real - `HSEngine.TAbstractHexagon.PlaceHX@5560a6d4` and
`ChangeTerrain` call vtable slot **+0x11c** (`UpdateTransitions`) on
place/terrain-change. `HSEngine.TMapField.ReadWrite@55607128` serialises only the
child sprites (tags >= 0x32), so **there is no `+0x14/+0x15` in the .HSM**: the
terrain type is a **resource** property (`HSEngine.TMapObject.GetTerrain@55609780`
= `TTerrainList.GetTerrain(resource+0x28 /* rw tag 7 */, hexIndex)`; list = `+0x04`
count, `+0x08` u16[] of `terrain | overlay<<8`;
`HSEngine.TTerrainList.LoadFromStream@55605354`, `GetTerrain@55605054`).
Water-sequence plumbing (resolved): `TImageSequenceList.GetImageSequence@5521942c`
= `*(*(list+4)+i*4)`; `TImageSequence.LinkToIL@55218944` resolves `seq+0x10` from
the sequence's library NAME (`seq+0x14`) via
`TImageLibraryManager.LinkToIL@55217794`.

**Tests (once implemented):** a coastline / terrain-boundary tile selects a
**non-zero** frame. **Acceptance:** `Tutorial.hsm` shows real coastlines and
blended terrain edges.

## Phase 5 — Elevation / upper-hex (cliff) layer — DONE (base variants)

**Goal:** raised terrain gets its 3-D cliff faces, matching the official render.

**What "elevation" turned out to be:** not a per-hex height field. The **whole
underground level** is built from `IsoHex.TUpperIsometricHexagon` hexes
(class `0x20170`) - 1262 of Tutorial level 1's 2304 tiles, 6076+5828 of Dwarf1's,
1705 of Underground Titans'. Their resource is `IsoHex.TUpperIsometricHexagonResource`
(class `0x20171`, only ids 1/2 = `EARTHHEX.ILB` / `ROCKHEX.ILB`).

**The rule** (`IsoHex.TUpperIsometricHexagon.Show@55619e24`): the base is drawn at
`(sprite+0x15 >> 6) + 0x28` - bits 6/7 of `+0x15` are a `RandInt(4)` variant, so
the base is library slots **0x28..0x2b**. In `EARTHHEX.ILB` those are `hu_er.BMP`
(48x32) - the hexagon *tops*; slots `0x1e/0x1f/0x20` are the cliff *edges*
(`tu_er-rk.BMP`), drawn at `(-6,-9)` for mask bits 0/1/5 of `+0x15`.

**The real bug this exposed:** ILB libraries are indexed by the record's
**image-slot id**, not by array position - `EARTHHEX.ILB` runs 0..32 then 40..43.
`get_library` indexed by position, so `frames[40]` was out of range and
`terrain_frame()` fell back to slot 0 (`tu_xx-er.BMP`, a cliff sprite) for **every**
raised hex. Fixed: `get_library` now indexes by `IlbEntry::index` (identical for
contiguous libraries, correct for gapped ones - and it also makes `HEXTRANS.ILB`'s
16-spaced terrain blocks line up for Phase 4).

**Implemented:** in the terrain pass, a hex of class `0x20170` draws
`EARTHHEX/ROCKHEX` slot `0x28 + hash(x,y)%4` (a deterministic stand-in for the
engine's `RandInt(4)`). Before/after pixel diff on Tutorial level 1: **53.3 %**.

**Tests:** `mapres_test` (46) asserts Tutorial level 1 is built from EARTHHEX hexes
and that every one draws an `hu_er` frame in 40..43.

**Still open:** the cliff *edges* (mask bits 0/1/5, frames 0x1e/0x1f/0x20) - they
only appear at underground terrain boundaries, which are rare; and whether the
surface level ever carries `0x20170` hexes for hills.

**Acceptance:** raised terrain shows rock/earth hexagon tops instead of a cliff
sprite patchwork.

## Phase 6 — `THSMap` fields + player/race lists (RE)

**Goal:** the remaining structural data the engine uses (elevation, owner,
setup).

* Recover `THSMap` tags 3/4/5/6 (bytes/ints at `THSMap+0x34/0x80/0x84/0x88`)
  (`docs/01_FORMATS.md:590-591`).
* Recover the player/setup lists (`tag 0x15` `TPlayerList`, `0x16` `TRaceList`)
  and expose typed structs on `Map` (players: name/race/colour/team; races:
  name/type) so the viewer can tint owned cities and label players.
* **Files:** `src/engine/map.cpp`, `src/include/aow/map_types.hpp`,
  possibly `map_resources.*`.

**Acceptance:** owner tinting/labels become possible; elevation is available to
Phase 5.

## Phase 7 — Layer order / z-sort / clipping

**Goal:** correct compositing so tall sprites overlap the way the engine does.

* Implement the engine's actual order: ground -> fringes/transitions -> object
  overlays, with per-hex clipping (`HANDOFF_faithful_mapviewer.md:139-142`).
* Z-sort objects within/among hexes as the engine does (row-major is not enough
  for tall sprites).
* **Phase 7a — the unit layer (done, partial).** The armies are the port's last
  world layer: `render_map_rgba` collects them during the object sweep and blits
  them after it, and a live front-end re-composites them per map frame with
  `draw_unit_overlays` (after the structure overlays and the water), so a unit is
  never covered by a building's animated sprite, its owner flag or a ripple - the
  position the engine's `TUnitHS.CalcShowPriority` (0x68000000) sorts them to.
  `docs/13_UNITS_ON_MAP.md`. **Still open `[TODO(RE)]`:** the engine's per-hex
  z-sort *between* rows (a tall object drawn on a lower hex must still occlude the
  row behind it) and the map's spell gfx (`AoWE.TWorldFXHS.Show@55797118`), the
  layer meant to sit above the units.

**Acceptance:** no sprite is occluded/ordered incorrectly vs the reference.

## Phase 8 — Viewer UX parity (optional / cosmetic)

* Overlays (grid, terrain highlight), a minimap, player-colour tinting (uses
  Phase 6), zoom-to-fit polish. Purely additive; no fidelity risk.

## Cross-cutting: docs, tests, build (every phase)

* Update `docs/01_FORMATS.md`: move items **out of** `TODO(RE)` as you close
  them, citing the new decompile + test.
* Update `docs/03_ROADMAP.md` Stage 2.2 and `HANDOFF_faithful_mapviewer.md`.
* Extend `src/tests/mapres_test.cpp` (and add `mapviz_golden.cpp` from Phase 0)
  with fidelity assertions per phase.
* New Ghidra exports under `re/exports/` for every function decompiled, with the
  standard header comment.
* `build.ps1` / `src/CMakeLists.txt` updated only if files/tests are added.

## Acceptance criteria (overall)

* `./build.ps1 -Test -Viewer` fully green, no warnings.
* Rendering `Underground Titans`, `Dwarf1.CSM` and `Tutorial.hsm` and comparing
  to real in-game screenshots shows: coastlines and terrain transitions present,
  forests/mountains/cities at the right positions, elevation/cliffs, and no
  large unexplained black regions.
* Every map-pipeline item previously in `docs/01_FORMATS.md` `TODO(RE)` is
  either closed with evidence or explicitly re-scoped with a written reason.

## Recommended execution order (quick wins first)

1. **Phase 0** (harness) -- so every later change is measurable.
2. **Phase 1** (backdrop gaps + geometry) -- huge visual win, no RE.
3. **Phase 2** (water/base-tile selection) -- huge visual win, no RE.
4. **Phase 3** (object anchoring) -- descriptor fields recovered; the object
   *anchor* needs the map draw path (Ghidra), see the Phase 3 section.
5. **Phase 4** (transitions) -- biggest fidelity win, needs RE.
6. **Phase 6** (THSMap fields) then **Phase 5** (elevation) -- data before cliffs.
7. **Phase 7** (layer order) and **Phase 8** (UX) last.


