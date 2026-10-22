# 25 - The Structure floater (TIWindow / `IWindow`)

Clicking a building on the World Map opens the game's **Structure** window: the
`InfoWin` of `TIWindow` (form instance `IWindow`), a 200x216 `TAOWWindow`
anchored to the right edge exactly like the Party floater. This document records
the reverse engineering behind `Scene::open_structure`,
`Scene::render_structure_panel`, `Scene::structure_layout` and the new
`--structure-panel` capture hook in `src/game/menu.cpp`.

The reference capture is `bin/StructurePReference.png` (199x217 - the 200x216
window with its right/bottom edge cropped by the capture, see section 6).

## 1. Where the window lives

`TIWindow` is one of the 79 `TPF0` DFM forms in the original `AoW.exe`; the
recovered form is `assets/dfm/AoW/TIWINDOW.dfm` (text dump:
`re/dfm_all/TIWINDOW.txt`). Its class VMT is at `0x0044193c` and its published
field table at `0x00441958` (91 fields). The field table is read as
`<u16 count><u32 typeinfo><u32 fieldOffset><u16 typeIndex><u8 nameLen><name>`;
that yields the component offsets used by the port:

| offset | component | offset | component |
|--------|-----------|--------|-----------|
| +0x44 | InfoWin | +0xb8 | Bar |
| +0x48 / +0x4c | Min / Close | +0xd8 | NotYoursPnl |
| +0x50 | Title | +0xfc | ConstructionPnl |
| +0x54 | SIcon | +0x10c | RazePnl |
| +0x58 | MainBtn | +0x110 | RazeBg |
| +0x5c | CeltFrame | +0x114 | RazeBtn |
| +0x60 / +0x64 | StructLine / StructNameLbl | +0x118 | CivilStatusLbl |
| +0x68 / +0x6c | ProductionPlacePnl / NamePnl | +0x11c | RaceRelationIcon |
| +0x70 | ActiveActionMemo | +0x120 | CityIncomeLbl |
| +0x74 / +0x78 | SignPnl / SignText | +0x124 / +0x128 | GoldIcn / UgradeI |
| +0x9c | CityPanel | +0x12c | CityLevelLbl |
| +0xa0 | CityRaceLbl | +0x140 | ActiveCityActionMemo |
| +0xa4 | CityBorderPnl | +0x158 | IncomePnl |
| +0xa8 | ProdOrb | +0x16c | IncomeMemo |
| +0xac / +0xb0 | SingleButPnl / SBmemo | +0x1a0 | Help |
| +0xb4 | AltarPnl | +0x1a4 / +0x1a8 | PopulationRaceLbl / PopulationLbl |

The published method table is at `0x00441f9f`; its handlers are
`InfoWinResize 0x4420d0`, `MinClick 0x4420f8`, `CloseClick 0x442118`,
`ProdOrb2Draw 0x44214c`, `ProdOrb1Draw 0x442150`, `CenterClick 0x442154`,
`RazeBtnClick 0x442278`, `MainBtnClick 0x442328`,
`ItembagLBDrawItem 0x4444b4`, `HelpClick 0x44453c`, `ESSBChange 0x444548`.
`TIWindow` has no exported methods (Delphi forms do not export them), so the
panel setup routines were located by their cross-references:

```
0x4425d8  TIWindow.ResetPanels        hide every sub-panel (Visible = 0/1 via vtable+0x6c)
0x44275c  TIWindow.ShowBase           StructLine + StructNameLbl + the object name
0x442804  show city (income, raze, activity)
0x4428b8  show city (income + raze)
0x44296c  production-place income-only panel
0x442a20  sign panel
0x442b64  item / production-orb panel
0x442c48  single-button panel (Wizard's Tower)
0x442d08  single-button panel (Enter)
0x442dc8  item-bag panel
0x442f08  exploration-site panel
0x443078  altar panel (charge gauge, gem/day labels)
0x443218  "not yours" single-button panel
0x443318  CITY panel (the reference capture)
0x44377c  city/altar variant with a production orb + active-action memo
0x443f88  ShowInfo dispatcher
```

`ShowInfo` tests the selected object's class and routes it to one of the
routines above. The class references are imported package class pointers
(`AoWEPACK.dpl`), so the port identifies the same cases from the map object's
class/resource class ids and the components each routine touches:
`+0x9c CityPanel`, `+0xa4 CityBorderPnl`, `+0xb4 AltarPnl`, `+0x74 SignPnl`,
`+0x170 ItemPnl`, `+0x18c ExplorationSitePnl`, `+0xfc ConstructionPnl`.

## 2. The City panel (`0x443318`)

The routine starts with `ResetPanels`, then `ShowBase`, then:

* `app+0xb0 = city` - the selected structure global.
* `CityPanel.Visible = 1`; `IncomePnl`/`RazePnl` etc. are hidden first.
* `StructNameLbl.Text = city.GetName` (`TCity.GetName@557ac918` returns
  `+0x48`, the rw **tag 0xc** name id; the Dict name table is external).
* `CityRaceLbl.Text = "Race: " + race_name[city+0x45]`
  (resource string, table indexed by the signed race byte).
* `CityLevelLbl.Text = IntToStr(city+0x4d)` (the size/level byte).
* `CityIncomeLbl.Text = IntToStr(city+0x3c)`.
* `CivilStatusLbl.Text = "Status: " + status_name[TCity.GetStatus(city) & 0x7f]`
  (`TCity.GetStatus` at `0x402abc`; the table at data `0x45e560`, resource
  strings `csCheerful`, `csContent`, ...).
* The active production object (`city+0x50`, `TProductionControl`) is drawn:
  * production type 1 -> the "Producing ..." text through `ActiveCityActionMemo`;
  * type 2 -> the item's name, else `ConstructionPnl` + the under-construction
    labels;
  * the income icon is `IncomeTypeImage` (ProdWin / ProdIcon), the value
    `IncomeAmountLbl`.
* `RazePnl.Visible = 1`.
* The city view (`CityBorderPnl`) and the leader portrait are drawn by the
  panel's own canvas pass; the port composes them from the same named art
  (section 3).

## 3. Art, exactly as TIWINDOW.dfm names it

| DFM property | Engine library | Entries used by the port |
|--------------|----------------|--------------------------|
| InfoWin body | `Int\GenericT.ilb` | 0 `TextBackGround` (200x216 stone), 2 `MainFloaterSet` (190x192 paper) |
| InfoWin frame | `Int\GenericF.ILB` | 0..7 `MainFloaterSetR` (6x20 / 190x20 / 6x192 / 6x6) |
| CeltFrame | `Int\CeltDeco.ILB` | 30..37 `FrameMain2` 9-slice |
| Min / Close | `Int\GenericI.ILB` | 3/5/4 and 0/2/1 |
| Center / Help | `Int\GenericI.ILB` | 6/8/7 and 59/61/60 |
| SIcon | `Int\WinTIcon.ilb` | 2 (19x16) |
| MainBtn | `Int\GenericB.ILB` | 19/25/22, 3 pieces (108x27) |
| CityBorderPnl | `Int\GenericF.ILB` | 115..122 `FrGoldD` (3x3) |
| City view | `Images\RACES\R*.ILB` | **42 `BCS_<race>.BMP`** (57x278), 45 `BCU_...` underground |
| Arch | `Int\CeltDeco.ILB` | 8 (76x58 top), 9 (76x58 border tile) |
| Name plaque lines | `Int\CeltDeco.ILB` | 17 (50x5), 22 (50x6) |
| RazePnl | `Int\CeltDeco.ILB` | 74 (56x35) |
| RazeBtn | `Int\GenericI.ILB` | 53/55/54 |
| GoldIcn | `Int\LBIcon.ILB` | 31 |
| Mana icon | `Int\LBIcon.ILB` | 33 |
| ProdOrb | `Int\ProdWin.ILB` | 4 (39x37) |

The BCS city view is the one that makes the reference capture's right-hand
painting: the 57x278 race banner's bottom 194 rows are visible in the 80x194
panel. The capture aligns it at `(134,-91)` (the window's title area above the
panel is clipped); the port draws it 1:1 with `Canvas::blit_clip` so its ILB
`base`/shift cannot move it. The arch (CeltDeco 8) is drawn on top, then the
left pillar strip and the id-9 border repeat every 58 px below it.

## 4. Layout

`TIWINDOW.dfm` evaluates to (at the 200x216 design size):

```
InfoWin         (0,0 200x216)       Title        (25,4 165x16)
SIcon           (2,1 19x16)         Help         (138,1 15x16)
Center          (153,1 15x16)       Min          (167,1 15x16)
Close           (181,1 15x16)       NamePnl      (4,17 136x27)
StructNameLbl   centred, y = 10     CityPanel    (4,45 114x138)
CityRaceLbl     centred, y = 73     CivilStatus  centred, bottom 1
CityBorderPnl   (120,18 80x194)     MainBtn      (10,184 108x27)
RazePnl         (128,174 56x35)     RazeBtn      (8,13 43x20 inside RazePnl)
```

The floater is anchored `awRight`/`ahTop` at `TopOffset 216`, so in a 640x480
window it sits at `(440,216)` - the same place as the Party floater. The port
uses the same `kPartyTop` anchor and the DFM boxes, not a stretched bitmap.

## 5. Input / selection

* `TAoWHSMap.Select` walks the field's sprites and asks each `CanSelect`; a
  building answers exactly like an army (`TStructure.CanSelect`). The map mouse
  then requests the animated hover pointer `Cursors.ilb` **0x34..0x37**
  (`kCursorStateHover`), which the existing cursor code already paints.
* A left click with no army selected opens `TIWindow.ShowInfo` for the building
  (`Scene::game_map_click` -> `Scene::open_structure`).
* Multiple objects on one hex page party -> structure -> party (the help text:
  "The first click will bring up the party floater, the second will bring up
  the structure floater"). The port keeps the party path unchanged and opens
  the structure when the hex has no army selection.
* Right-click closes the floater (the shared floater rule) before it clears an
  army selection.
* The selected building keeps a pulsing hexagon marker; `TStructure.Select`
  (`0x5575ea40`) also plays `Sfx\Int\Btn1.wav`. The port pulses the two
  `Images\General.ILB` selector halves off `AoWHSMap+0x40`
  (`Scene::render_structure_marker`).

## 6. Verification

`--structure-panel` opens the first city (or first structure) after
`--start-map`. Because the reference capture's city name and income live in a
scenario that is not part of the install, the hook also has
`--structure-name/-race/-race-index/-status/-gold/-mana` overrides.

```
AoW.exe --render bin/struct_final.png --start-map "Barondir Bay.HSM" `
  --scen-human 0 --structure-panel --structure-name Venice --structure-race Human `
  --structure-race-index 0 --structure-gold 38 --structure-mana 2 `
  --structure-status Cheerful --crop 440 216 199 217 --no-music
python tools/structure_bench.py bin/StructurePReference.png bin/struct_final.png
```

`tools/structure_bench.py` writes `bin/structure_bench.png` (reference / port /
diff) and prints the mean per-channel absolute difference. The current result
is a mean of **~30.8/255** over the 199x217 panel. The remaining differences
are the external Dict city name/portrait (the capture's "Venice" and blue
leader face), the 16-bit screenshot's palette dithering, and the city view
column/painting composition; every element, its DFM box and its original
library entry is reproduced.
