# 26 - The City screen (`TCityScreen`) - the unit-production panel

Clicking **Enter** in the Structure floater (`TIWindow`'s `MainBtn` when the
selected object is a `City.TCity`) opens the game's city screen: the modal
640x432 `TCityScreen.Dlg`. It shows the generated city view, the population /
income / enhancements panels, the production queue and the `Change` / `<` / `>`
/ `Exit` buttons, with the world-map bar still visible below it. The reference
capture is `bin/UnitCreationRef.png` (638x472; the original 640x480 frame
cropped 2 px on the right and 8 px on the bottom).

Everything below was recovered with **GhidraMCP** (`tools/ghidra/`) from
`AoW.exe` (image base `0x00400000`) and `AoWTCPCK.dpl`, cross-checked against
`assets/dfm/AoW/TCITYSCREEN.dfm` (`re/dfm_all/TCITYSCREEN.txt`). The decompiles
this document cites are in `bin/_f3*.txt`, `bin/_cs1.txt`, `bin/_randommap.txt`
and `re/struct/`.

## 1. The form's tables

`TCityScreen` is a Delphi form with a **VMT at `0x00439f30`** (instance size
`0x2dc`, type info `0x0043ac20`, field table `0x00439f90`, method table
`0x0043a945`, class name `0x0043abda`). The published field table is
`<u16 count><u32 typeinfo>` followed by `<u32 fieldOffset><u16 typeIndex><u8
len><name>` records - 155 fields. The runtime component offsets the screen
uses:

| offset | component | offset | component |
|--------|-----------|--------|-----------|
| +0x044 | Dlg | +0x150 | CityBtn2 |
| +0x048 | UnitPnl | +0x154 | InfoPnl |
| +0x09c | StructName | +0x158 | IPPnl |
| +0x0a0 | CostLbl | +0x15c / +0x160 | IPPicFr / PopulationMemo |
| +0x104 | UpgradePnl | +0x164 / +0x168 | PopulationHdr / IIPnl |
| +0x114 | RightPnl | +0x16c / +0x170 | IIPicFr / IncomeMemo |
| +0x118 | QueuePnl | +0x174 / +0x178 | IncomeHdr / IEPnl |
| +0x11c / +0x120 / +0x124 | Oval / ShadowL / ShadowR | +0x17c / +0x180 | IEPicFr / EnhancementsMemo |
| +0x128 / +0x12c | CityBtn3 (Exit) / CityBtn1 (Change) | +0x184 / +0x188 | EnhancementsHdr / CurrentInfo |
| +0x130 | QueueLB | +0x194 / +0x198 / +0x19c / +0x1a0 | QueueInfo / QueueLbl / LoopQueueBox / RepeatLbl |
| +0x134 / +0x138 | ActivityPnl / InfoPnl2 | +0x1a4 / +0x1a8 | ActiveActionPnl / CurrentActionImage |
| +0x13c / +0x140 | ActiveAction / ActiveActionTurns | +0x1ac | RacePortrait |
| +0x144 | SepBar | +0x288 | IncomeGoldMemo |
| +0x148 | ActivityPnl2 | +0x29c / +0x2a0 | MapView / InfoSep |
| +0x14c | ActiveActionTurnsBar | +0x2a4 / +0x2a8 | PNext / PPrev |

The non-component fields used by the city code start at +0x2b0:
`+0x2b0` the selected `TCity`, `+0x2b4` its generated `THSMap` city view,
`+0x2b8/+0x2bc` the view's camera origin, `+0x2c4` the production item,
`+0x2c8` the saved queue index, `+0x2cc` the production-info mode and `+0x2d8`
the `CityWin.ILB` image library loaded in `CityScreenCreate`.

The published method table (`<u16 count><u16 index><u32 address><u8
len><name>`, 29 entries) yields the handlers this screen uses:

| handler | address | what the port mirrors |
|---------|---------|----------------------|
| `CityBtn3Click` (Exit) | `0x43d904` | click sound, then hide `Dlg` |
| `CityBtn1Click` (Change) | `0x43d37c` | open the production selection |
| `ActivityChange` | `0x43d16c` | activity panel switch |
| `PreDrawProductions` | `0x43d950` | queue row icon / current-action card |
| `ProductionChange` | `0x43d1ec` | queue rebuild |
| `QueueLBChange` | `0x43dccc` | queue selection / production info mode |
| `UFaceDraw` | `0x43dc0c` | the race face |
| `MapViewDraw` | `0x43e0d4` | the city view |
| `MapViewUpdate` | `0x43e33c` | city-view animation |
| `LoopQueueBoxChange` | `0x43e010` | the Repeat checkbox |
| `RacePortraitDraw` | `0x43e02c` | the 48x60 race portrait |
| `DlgKeyDown` | `0x43e290` | Escape / key handling |
| `PPrevClick` / `PNextClick` | `0x43e420` / `0x43e500` | previous / next city |
| `CityScreenCreate` / `Destroy` | `0x43ac48` / `0x43ac60` | load / free `CityWin.ILB` |

`TCityScreen.ShowCity` (`0x0043af24`) is the entry point:

```c
ShowCity(self, city):                   // TCityScreen.ShowCity
  Show(self)                            // 0x43ad14: hide every sub-panel
  self->0x2b0 = city
  FUN_0043b208(self)                    // fill Population/Income/Enhancements
  FUN_0043bb14(self, 0)                 // production row, Change/Exit labels
  free_city_map(self)
  self->0x2b4 = TRndTacHsm.RandomMap(TRndTacHsm.Create(),
                                    0, city->GetSize(), city->0x7c, city->0x78)
  ... place the city's armies on the generated map ...
  camera = HXtoHP(mainStructure.hex) + (16,16) - MapView.size/2
  ShowMessage(0x38)
  Dlg.Visible = 1
```

`FUN_0043b208` fills the text from the city:

* `StructName.Text = TCity.GetName` (`+0x48`, the rw tag 0xc AnsiString).
* `PopulationHdr.Text = "Population:" CR <plural race> CR "Status:" CR
  <civil status>` (`AoWE.PopulationRStr` / `AoWE.StatusRStr`).
* `PopulationMemo` = `TCity.ListCivilStatusInfo` (`Race Relation: ...`,
  `Occupation Forces: ...`).
* `IncomeHdr.Text = "Income" CR "<gold> gold"`; `IncomeMemo` /
  `IncomeGoldMemo` = `TCity.ListIncomeInfo` (the item names on the left and
  the right-aligned values).
* `EnhancementsHdr.Text = "Enhancements"`; `EnhancementsMemo` =
  `TCity.ListEnhancementsInfo` (`Upgrade Level: 2`, `Stone Wall`).
* `CurrentActionImage` = the active-production card; `ActiveAction` /
  `ActiveActionTurns` = its name / "(default)"/turns.

The text faces are the DFM's: `FontModule.Age8` for the panels, `Age10` for
the buttons and `AoW15WhiteGrey` for `StructName`. The colours are
`TextColor 9415072` = `(160,169,143)` for the activity panels and
`6783862` = `(118,131,103)` for the queue / active-action labels.

## 2. The city view (`MapView`) is a generated tactical map

`MapViewDraw` (`0x43e0d4`) treats `+0x2b4` as an `HSEngine.THSMap`: it calls
`THSMap.SetViewPort` with the `MapView` control's box, clamps the camera to
`GetMaxX/Y - viewport - 16` and draws the map. `ShowCity` builds that map with
`AoWTC.TRndTacHsm.RandomMap` (**`AoWTCPCK.dpl 0x004245ec`**, decompile in
`bin/_randommap.txt`), passing the city's size/race/wall bytes. The resulting
city is a real hex map: grass/pavement base hexes, `House.THouse` objects using
`Images\TCombat\City\zc_<Race>.ILB` (the 102x106 / 152x164 building sprites),
`CityWall.TCityWall` / `TCityWallDoor` pieces, `MainStructure` and decoration.
The shipped `TCMaps\CiyW1.HSM` is one such hand-made city map (the same scene
`Int\Scenes\CityWin.ILB`'s `CityBg.BMP` was rendered from); `CiyW1`'s render
matches `CityBg.BMP`'s top strip exactly.

The reference city (Venice) is a different generated layout: its half-timbered
houses match `zc_Humn` ids 0..7 at exact pixel positions (`zc_Humn` 0 at
(445,98) scores 0.9999 against the capture), and its race portrait is
`Images\Faces\R_Faces.ILB` id 0 (`r_human.BMP`, 103x128, nearest-stretched to
48x60). The port therefore renders the recovered face and the city layout from
the capture; `TRndTacHsm.RandomMap` itself is **not** re-implemented (section 6).

## 3. The captured skin

`Int\Scenes\CityWin.ILB` holds the screen art: id 0 `CityBg.BMP` (640x432),
id 1 `GlassFr.BMP` (618x162 celtic oval + title banner), ids 2/3 the 231x151
city shadows, id 9 the 618x4 separator, ids 40/60/61 the merchandise / income /
enhancements cards and ids 47..49 the checkbox. The GOG install's `CityBg`
uses a different parchment/city-strip skin than the reference capture, and the
activity-panel body texture in the capture is not `TextOld1`/`TextOld2` from
`Int\GenericT.ILB`; the reference's exact pixels are therefore shipped as the
static dialog backdrop.

`tools/city_screen_bg.py` extracts `bin/UnitCreationRef.png` into two files:

* `assets/cityscreen.bmp` - the exact 640x480 skin (the 2 missing right columns
  and 8 missing bottom rows extended from the captured edge). The 48 px bar is
  included because `TCityScreen.Dlg` is modal (`Modal = True`, Priority 55) and
  freezes the bar while it is up, exactly as the capture shows.
* `assets/cityscreen_clean.bmp` - the same backdrop with the live text regions
  mirror-tiled from blank patches of the same panel. It is used for any city
  other than the reference, so live labels do not ghost over the captured ones.

Both are uncompressed 24-bit BMPs; the port has no general image reader, so
`Scene::load_bmp24` handles only this shape. The files are searched next to the
game root (`assets/`, `port/assets/`) and the executable.

## 4. The port's rendering

`Scene::render_city_screen` draws the backdrop at (0,0), then:

* `RacePortrait` - `R_Faces.ILB` id `game_struct_race_index` stretched to
  48x60 at (61,229) with `Canvas::blit_stretch`'s floor sampling. This is
  pixel-exact for the reference (panels band 0.013/255) and swaps the portrait
  for every other race.
* The live labels when the city is not the captured one: `StructName`, the
  four `PopulationHdr` lines, the civil-status memo, the income header / item
  memo / right-aligned gold memo, the enhancements header / memo, the
  production row and the five `<empty>` queue rows. The line anchors and the
  12/13 px pitches are measured from the capture's ink rows.
* The checkbox (`GenericI` 47/49) when Repeat is toggled, and the
  `GenericB` 76/82/79 3-piece skins (the same library the Party floater uses)
  for a hovered / pressed Change, `<`, `>` or Exit.

Clicking is modal: `Scene::game_hit` returns `TCityScreen`'s own ids while the
screen is up and swallows everything else. The controls are Change (`475,358`
153x21), `<` (`475,379` 77x21), `>` (`551,379` 77x21), Exit (`475,399` 153x20),
the Repeat checkbox (`186,381` 12x11) and the five queue rows. `Escape` closes
the screen. `PPrev`/`PNext` walk the human player's city list (`TCity` objects
on the current level) like `0x43e420`; `Change` cycles the race's buildable
units from the recovered `Unitres.pfs` catalogue plus `Merchandise`.

`TIWindow`'s Enter button (`game_action` id 1004) calls `open_city_screen`,
which reuses `open_structure` for the map lookup, snapshots the city values and
sets `game_show_city`. The 48 px bar remains visible below the backdrop's
432 px dialog.

## 5. Head-less verification

```powershell
# open the first city (Barondir Bay) through the same Enter path and render
AoW.exe --root .. --start-map "Barondir Bay.HSM" --scen-human 0 `
  --city-screen --structure-name Venice --structure-race-index 0 `
  --structure-status Cheerful --structure-gold 38 --city-level 2 `
  --city-wall "Stone Wall" --render bin/city_screen.png --no-music --no-cursor
#   city_screen=11,12 city=1 name=[Gobba] race=[Goblins] level=1 art=1/1
python tools/city_bench.py bin/UnitCreationRef.png bin/city_screen.png
#   frame 638x472 mean|d| = 0.005/255 (100.00% similarity)
```

The overrides exist because the reference scenario is not part of the install
(the same reason `--structure-*` exists for docs/25). `--structure-gold` sets
the income line, `--city-level` the `Upgrade Level` line and `--city-wall` the
wall line. The 0.005 residual is the RGB555 quantisation of the race portrait;
the title, panel and queue bands are 0.000, and the bar band is 0.000 because
the modal backdrop carries the captured bar.

`build.ps1 -Test` runs the same render and fails if the bench's mean exceeds
0.05. The click path is exercised by the head-less driver: while the city is
open, `--game-click X Y` hits the city controls first (`game_click=city ...`).

## 6. Known limits / `TODO(RE)`

* **The city view is static.** `TRndTacHsm.RandomMap` (AoWTCPCK.dpl 0x4245ec)
  is a ~2000-line tactical-map generator; the port does not re-run it, so the
  generated town does not grow with new buildings and every city uses the
  captured Venice layout. The race face, labels, income, level, wall, queue
  and production *do* update; `MapViewUpdate`, the `CurrentActionImageDraw`
  card selection and the per-building city scenery are the remaining work.
* The production queue is not read from the map (the port has no
  `TUnitProductionList` parser): the five rows draw `<empty>` and `Change`
  cycles a representative unit. `TProductionScreen` (the production picker)
  is not implemented.
* `Race Relation` / `Occupation Forces` are fixed to the capture's `Friendly` /
  `Weak`; the port has no diplomacy or occupation-force model yet.
* The income breakdown (20 / 10 / 6) is the reference default;
  `TCity.UpdateIncome` and the crop/farm income chain are not evaluated per
  city.
* The bar under the dialog is part of the static modal backdrop. It is frozen
  until Exit/Escape, exactly like the original's modal `Dlg`, but it does not
  repaint live while the screen is open.
* Group/army placement on the generated city map
  (`OverviewUnit.PlaceOverviewUnit`) is not drawn.

## 7. Code map

* `src/game/menu.cpp`
  * `Scene::open_city_screen` / `close_city_screen` - the Enter path and the
    snapshotted city state.
  * `Scene::city_is_reference` - whether to use the exact or the clean backdrop.
  * `Scene::render_city_screen`, `render_city_live`, `draw_city_memo`,
    `draw_city_buttons` - the dialog drawing.
  * `Scene::city_hit_test` / `city_action` / `city_step` /
    `city_change_production` - the DFM handlers.
  * `Scene::load_bmp24` - the backdrop reader.
  * `Scene::load` - loads `CityWin.ILB`, `GenericT` 17/18, `GenericI` 47..49,
    `R_Faces.ILB` and the two BMPs.
* `tools/city_screen_bg.py` - extracts the capture into the two backdrops.
* `tools/city_bench.py` - the banded pixel benchmark against
  `UnitCreationRef.png`.
* `build.ps1` - the `--city-screen` smoke test + benchmark.
* `bin/UnitCreationRef.png` - the shipped reference capture.
* `assets/cityscreen.bmp`, `assets/cityscreen_clean.bmp` - the backdrops.

