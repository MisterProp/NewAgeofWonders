# 25 - The Structure floater (TIWindow / `IWindow`)

Clicking a building on the World Map opens the game's **Structure** window: the
`InfoWin` of `TIWindow` (form instance `IWindow`), a 200x216 `TAOWWindow`
anchored to the right edge exactly like the Party floater. This document records
the reverse engineering behind `Scene::open_structure`,
`Scene::render_structure_panel`, `Scene::structure_layout` and the new
`--structure-panel` capture hook in `src/game/menu.cpp`.

The reference capture is `bin/StructurePReference.png` (199x217). It is a crop
taken 3 px left and 2 px above the 200x216 window origin, so its first three
columns / two rows show the frame's outer edge and the frame itself starts at
(3,2); every component inside it is at its DFM box relative to that origin. The
screenshot is a 16-bit RGB555 surface capture (all channel values are in the
32-step RGB555-expanded set - see section 6).

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
  `+0x48`, the city's AnsiString). `TCity.ReadWrite@557acbcc` streams `+0x48`
  at rw **tag 0xc** through `rwString` - one length byte, `0xff` => a `u32`
  length follows, then the bytes (`TEReadStorageStream.rwString@555111a8` ->
  `FUN_5550f81c`; the writer is `Engine.TEWriteStorageStream.rwString` ->
  `FUN_5550f7a8`, which omits an empty string and makes the reader `LStrClr`
  the field). **The name really is in the map**: Bloody Marauders' Human
  capital stores `07 "Venice "` at its TCity record (its first city is
  `"Ricordia"`, and `"Confinia Minor"` proves multi-word names survive), and all
  2406 cities of the 72 shipped maps carry one. The port therefore reads tag 0xc
  with `eobject_short_string` (a u32 length misreads the first four name bytes
  and made every city show "City"). A city whose tag is absent has no name in
  the original either (the label is blank; `TCity.Create`'s `AoWE.NonameRStr` =
  resource string 64638 "Noname" only ever applies to a city the editor has not
  named yet).
* `CityRaceLbl.Text = "Race: %s"` - `AoWE.RaceX` (AoWEPACK resourcestring
  **64846**) formatted with `AoWE.RaceNamesRStr[city+0x45]`. The routine loads
  the table element first and then the format string:
  `MOVSX EAX, byte [city+0x45]` / `MOV EDX,[0x0045e000]` /
  `MOV EAX,[EDX+EAX*4]` / `TranslateRStr` / `LoadResString([0x0045e1c0])` /
  `Format`. So the index is the **signed** city race byte (`0xff` -> -1) and
  the name is the *shipped singular* string, not a plural and not the port's
  own spelling. `AoWE.RaceNamesRStr` is imported from `AoWEPACK.dpl` (the exe
  slot `0x0045e000` is the pre-fixup record `AoWE.RaceNamesRStr@B11890CE`), so
  the strings are that package's RT_STRING entries - the singular declared
  next to each race's plural (`AoWE.RaceNamesPluralRStr`, see
  `aow::race_label_plural`):

  | race | label | id | race | label | id |
  |------|-------|----|------|-------|----|
  | 0 Human | `Human` | 65255 | 6 Dwarf | `Dwarf` | 65235 |
  | 1 Azrac | `Azrac` | 65257 | 7 Highman | `Highman` | 65237 |
  | 2 Lizard | `Lizard` | 65259 | 8 DarkElf | `DarkElf` | 65239 |
  | 3 Frostling | `Frostling` | 65261 | 9 Orc | `Orc` | 65241 |
  | 4 Elf | `Elf` | 65263 | 10 Goblin | `Goblin` | 65243 |
  | 5 Halfling | `Halfling` | 65233 | 11 Undead | `Undead` | 65245 |

  `Highman`/`DarkElf` really are spelled without a space (their plurals are
  "Highmen"/"Dark Elves"), and Elf is the one race whose plural ("Elves",
  65232) is not adjacent to its singular in the table. The port exposes the
  table as `aow::race_label_singular` and shows it verbatim. Before that, the
  panel indexed a hand-written name list in a different order
  (`Human, Elf, Dwarf, Halfling, Orc, ...`), which renamed every race from id 4
  on - Heartwood Forest's Elves read "Orc", its Goblins "Azrac", its Orcs
  "Lizardman". An unset race (rw tag 0xd absent, or the editor's `0xff` =
  `C_random`) is resolved to the owner player's race, the same value
  `city_body_race` uses for the body art; when that also yields no race (the
  independents' roster entry carries 255) the label falls back to race 0, while
  the original indexes the raw byte and reads out of bounds. Heartwood Forest
  has 6 such cities.
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
| Celt frame edges | `Int\CeltDeco.ILB` | 0 (7x27 `ILIndexLine` knot) + FrameMain2 30..37 |
| RaceRelationIcon | `Int\Scenes\RelFr.ILB` | ids 11..18 (51x36 faces; 15 = friendly blue face); `CityPanel+32,86` |
| Income icon | `Int\LBIcon.ILB` | 31 (16x13) at `InfoPnl+10,2` |
| City level icon | `Int\LBIcon.ILB` | **1** (11x12 gold upgrade tree) at `InfoPnl+61,3` - 33 is the unrelated mana diamond |
| Name plaque | `Int\GenericT.ILB` | 14 `RedText` 100x100 parchment + the measured 3-line bevel |
| RazePnl | `Int\CeltDeco.ILB` | 74 (56x35) |
| RazeBtn | `Int\GenericI.ILB` | 53/55/54 (43x20 at `RazePnl+8,13`) |
| ProdOrb | `Int\ProdWin.ILB` | 4 (39x37) |

The city view is the RACES library's **id 42** (57x278). Its decoder buffer is
blitted at window (135,-88); the visual source (10,180) then lands on the
capture's (144,89), i.e. the visual banner box starts 3 px left of the window's
`CityBorderPnl`. The 80x194 panel clips it and the two CeltDeco arch pieces are
drawn over it (top at (119,19), id-9 tile at (119,77) and (119,135)).

The BCS city view is the one that makes the reference capture's right-hand
painting: the 57x278 race banner's lower rows are visible in the 80x194 panel.
The capture puts the visual banner box at (131,-93) (the window's title area
above the panel is clipped); the port blits the decoder buffer at (135,-88) so
the visual source (10,180) lands on the capture's (144,89) byte-for-byte. The
arch is drawn on top: CeltDeco 8 at (119,19) and the id-9 tile at (119,77) and
(119,135); below y~193 the city painting / window frame shows through.

## 4. Layout

`TIWINDOW.dfm` evaluates to (at the 200x216 design size):

```
InfoWin         (0,0 200x216)       Title ink    (25,4)   Age10 clBlack
SIcon           (2,1 19x16)         Help         (138,1 15x16)
Center          (153,1 15x16)       Min          (167,1 15x16)
Close           (181,1 15x16)       NamePnl      (4,17 136x27)
StructNameLbl   ink (52,27)         CityPanel    (4,45 114x138)
CityRaceLbl     ink centred, y=118  CityBorderPnl(120,18 80x194)
CivilStatusLbl  ink centred, y=171  MainBtn      (10,184 108x27)
InfoPnl+GoldIcn (20,96 16x13)       Income ink   (42,97)  Age10s white
+UgradeI        (71,97 11x12)       Level ink    (91,97)  Age10s white
RaceRelationIcon(42,131 37x36 vis)  Production   (12,68)  Age8 clBlack
RazePnl         (128,174 56x35)     RazeBtn      (136,187 43x20)
```

The floater is anchored `awRight`/`ahTop` at `TopOffset 216`, so in a 640x480
window it sits at `(440,216)` - the same place as the Party floater. The port
uses the same `kPartyTop` anchor and the DFM boxes, not a stretched bitmap.

Fonts (all decoded from `Int\Fonts`): Title and Enter are **Age10**;
StructNameLbl, CityIncomeLbl and CityLevelLbl are **Age10s** (their black
pixels are real shadow); CityRaceLbl/CivilStatusLbl are **Age8s** (thin face,
black pixels are the colour key); ActiveCityActionMemo is **Age8**. The
`glyph()` blitter keeps the glyph's own black shadow for the first four and
keys it for Age8s.

## 5. Input / selection

* `TAoWHSMap.Select` walks the field's sprites and asks each `CanSelect`; a
  building answers exactly like an army (`TStructure.CanSelect`). The map mouse
  then requests the animated hover pointer `Cursors.ilb` **0x34..0x37**
  (`kCursorStateHover`), which the existing cursor code already paints.
* A left click with no army selected opens `TIWindow.ShowInfo` for the building
  (`Scene::game_map_click` -> `Scene::open_structure`).
* **A large building answers on every tile its sprite occupies.** A multi-hex
  object is stored as *one* `MapHs` on its home hex, but
  `TMultiHexMO.UpdateMapFields@5560af6c` registers it on every `TTerrainList`
  entry that has an opinion, walking the engine's HN spiral (`hn_walk_hex`) from
  that home hex. The resource data therefore names the occupied tiles exactly:
  a size-1 `City.TCityResource` (CITY1.ILB) covers only its hex, CITY2 two,
  CITY3 three and CITY4 (a capital) four - entries 0/1/2/6 = home, N, NE, NW.
  `Scene::structure_on_hex` first checks the hovered/clicked tile's own object
  list, then searches the surrounding five-hex HN radius (the largest shipped
  `TTerrainList` has 65 entries, ring 5) for a structure whose footprint covers
  the tile, using `aow::object_covers_hex`. `open_structure` still records the
  object's *home* hex in `game_struct_hx/hy`, so the floater, its Center button
  and the selection pulse stay anchored to the building rather than the tile
  that was clicked. This makes the whole capital (four tiles) and every other
  large building selectable, not only its centre. The head-less
  `--structure-tile X Y` hook opens the structure selectable at a given tile
  (`build.ps1 -Test` uses Barondir Bay's size-4 city at (12,20): tiles
  (12,19), (13,19) and (11,19) all open it, (14,20) does not).
* Multiple objects on one hex page party -> structure -> party (the help text:
  "The first click will bring up the party floater, the second will bring up
  the structure floater"). The port keeps the party path unchanged and opens
  the structure when the hex has no army selection.
* Right-click closes the floater (the shared floater rule) before it clears an
  army selection.
* The selected building keeps pulsing its own opacity; `TStructure.Select`
  (`0x5575ea40`) also plays `Sfx\Int\Btn1.wav`. The port renders the level once
  more with only that map object hidden (`MapVizOptions::hide_object_offset` -
  the object's unique map-file region offset; `MapHs::tag` is only the
  field-table slot inside a hex record and is reused by every tile, so it would
  hide all buildings of that slot), takes the pixel bounding box of the
  difference, and every map frame re-blends the building over that background
  (`Scene::mv_prepare_structure_fade` / `mv_apply_structure_fade`). It is the
  building itself that fades, not a hexagon marker drawn over it.
* **The exact pulse is `bin/Screen Selection.gif`**, not a smooth sine: the
  capture is 13 consecutive map frames (30 ms/frame, i.e. the game's 30 fps
  clock). Frames 1-5 and 8-13 draw the building at 100%; only frames 6-7 drop the
  *body* to 50% over the terrain, then it is opaque again. The owner flag, the
  city wall and the armies on/next to the hex are separate scene objects and stay
  opaque throughout. `Scene::mv_apply_structure_fade` therefore uses a square
  pulse (`kStructurePulsePeriod = 13`, first dimmed frame 5 for two frames,
  alpha 128/255); its phase 0 is the map clock when the floater opens, so the
  first five frames after a click are opaque exactly like the capture.

## 6. Verification

`--structure-panel` opens the first city (or first structure) after
`--start-map`; the name, race, level and production come straight from the map
(`TCity` rw tags 0xc / 0xd / 0x10, the city race and the owning player), so a
plain run already reads e.g. `name=[Venice ]` for Bloody Marauders and the
first cities of Barondir Bay / Tutorial. Because the reference capture comes
from a scenario that is not part of the install, the hook also has
`--structure-name/-race/-race-index/-status/-gold/-mana` overrides. It prints
`structure_panel=<x>,<y> city=<0|1> name=[<name>] race=[<label>] race_id=<n>`,
so both the recovered name and the `CityRaceLbl` text can be asserted
head-lessly, and `--structure-tile X Y` opens a specific tile (used to prove
Venice answers on all four of its hexes).

```
# Heartwood Forest's first city is elven (TCity tag 0xd = 4) -> race=[Elf]
AoW.exe --root .. --start-map "Heartwood Forest.HSM" --structure-panel `
  --render bin/struct_heartwood.png --no-music --no-cursor
#   structure_panel=..,.. city=1 name=[...] race=[Elf] race_id=4
```

```
AoW.exe --render bin/struct_final.png --start-map "Barondir Bay.HSM" `
  --scen-human 0 --structure-panel --structure-name Venice --structure-race Human `
  --structure-race-index 0 --structure-gold 38 --structure-mana 2 `
  --structure-status Cheerful --structure-relation 4 --crop 437 214 199 217 `
  --no-music --no-cursor
python tools/structure_bench.py bin/StructurePReference.png bin/struct_final.png
```

`--structure-relation` selects the RelFr face band (11 + relation); the
reference's city owner is at relation 4, i.e. RelFr id 15, the blue laughing
face. The default is 4 as well (the port has no diplomacy model yet).

`build.ps1 -Test` also renders the first Barondir Bay city at map clocks 4-7 and
asserts the pulse's square shape: clocks 4 and 7 are byte-identical (opaque),
clocks 5 and 6 are byte-identical (the 50% dip), and 4 differs from 5.

`tools/structure_bench.py` writes `bin/structure_bench.png` (reference / port /
diff) and prints the mean per-channel absolute difference. The current result
is a mean of **~4.1/255** over the 199x217 panel (title 0.3, name 5.3,
left body 4.5, right city 4.2, footer 5.3); the build smoke test fails at 8.

All of the capture's *elements* now match: the frame, title bar and buttons,
the parchment and text pixels (byte-identical glyph masks), the RelFr relation
face, both InfoPnl icons, the BCS city painting, the two arch pieces, the raze
panel/button and the Enter button. The residual is the last few pixels of the
hand-drawn bevel around NamePnl/CityPanel, the celtic frame's corner knots
(TAoWFrame's rotated `ILIndexLine` tiling) and the 1 px frame overdraw at the
crop's left/top edge - i.e. sub-2 % per channel, not a layout or art-entry
difference. `Canvas::to_rgba(g.rgb555)` quantises the PNG through the engine's
16-bit RGB555 back surface when the capture hook is used (`--structure-panel`,
or `--rgb555` on another screen), which removed the palette delta that the
first pass (mean ~30.8) mistook for art differences.
