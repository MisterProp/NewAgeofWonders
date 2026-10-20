# 08 - The Scenario screen (TScenSetup), recovered and reproduced

Orb 0 of the title screen opens the Game Type screen (`docs/07_GAME_TYPE_SCREEN.md`).
Its **Single Computer** button opens the scenario setup screen - the "Scenario"
screen with the map picker and the race grid. This stage reproduces that screen,
its "Select a Map..." dialog and the leader-setup dropdowns from the game's own
assets and geometry.

All addresses below are `AoW.exe` (image base `0x00400000`), recovered with
GhidraMCP (`tools/ghidra/`) plus the local helpers (`tools/pe_va.py`,
`tools/blobfind.py`, `tools/ghidra/mcp_post.py` -> `/create_function`).

## 0. Fidelity passes against the reference captures

`bin/ScenarioMenuRef.png` is a 369x359 crop of the shipped 640x480 screen with
the "Select a Map..." picker open (the earlier, wider `SelectionMenuRef.png`
capture is superseded by it). The crop origin in the port render is **(134,37)**,
so the one-number comparison is:

```
python tools/pixmatch.py re/mw/final.png bin/ScenarioMenuRef.png \
    --render-origin 134 37                                       # one-number match
python tools/sheet2.py out.png 4 "Int/GenericE.ILB:3,4,5,..."     # zoomed contact sheet
```

The first pass scored 24.4% within 30 / mean 44.25 against the old capture.
Pass 2 (section 9) scores **91.4% within 30**, 84.6% within 20, 69.2% within
10 and 94.7% within 45 at mean 10.11 over the 369x359 crop.

Facts recovered from the capture (all measured, not guessed):

* **Backdrop.** The screen does **not** tint its backdrop - the corner pixels are
  `Backg03.BMP` (TitleM id 4) verbatim, e.g. `(20,470) = 31 20 20` and
  `(620,10) = 00 00 08`. The old deep-maroon fill was wrong and is gone.
* **Race cells.** An unavailable race keeps its `R*FogPnl` at full strength
  (`FUN_00410f2c`'s `param_2 == 0` branch) - the capture's cells are essentially
  black. The replica previously applied the fog only to *available* races.
* **Dialog skins.** `PathEdit` / `FileName` carry `ILIndex = 15`, i.e. the **dark**
  `NGEdit` face (GenericE 15..17); the combos use `NGCmb` (GenericE 21..23),
  whose 19 px right cap is the gold funnel. `EditBLight`/`ComboBLight` (3..8) are
  the pale variant and are not what ships.
* **Settings combo.** `PlayModeCB` names `ILFrame`/`ILEdit = IntGfxMod.TitleScreen`:
  TitleM's `ScenEBut` 26/27/28 (10/162/18 x 18), a grey field with a dark-green
  funnel - not a gold field.
* **Settings panel body.** The capture is **black above** the maze texture, i.e.
  the `ShadowOv` (GenericT 1) tile is anchored to the panel's *bottom* edge.
  Measured: bottom-anchored scores 29.2 over the panel vs 37.5 for the tiled
  version and 36.5 for plain black.
* **Floater frame inset.** The 380-wide `FileDialog` at x 130 draws its `DialogFr`
  frame (GenericF 32..39) inset ~13 px: the capture's frame spans x 143..492
  while the title bar and children keep their own positions. The body texture
  uses the same inset rect.
* **Floater shadow.** The modal window casts a large solid black shadow: the
  capture is pure black from x 10..490, y 396..466 where `Backg03` is maroon.
  TODO(RE): recover the shadow sprite/geometry from `AoW.exe`'s `TAOWWindow`.
* **Title bar.** It is the `DialogFr` top edge (GenericF 33, a gold bar) carrying
  `WinTIcon` id 8 at (22,3) and the Age10 title; the close box (WinLeft 345) is a
  gold disc with a dark cross.
* **Celtic bands.** The band above the buttons (`BCeltL`, `frHorizLine`,
  `ILLine = CeltDeco` 55) and the one under the title bar are a *repeating* knot
  tile. Measured against the capture, `CeltDeco`'s 27x27 knot piece (entry 4)
  scores 31.6, the 100x27 medallion at 56 scores 60.3 (it carries a gold oval).
* **Info strip.** `LSavedInfo` is a `TAOWPanel` (FrGoldD frame 113 over the
  `TextOld1` body 17) - without the body it showed the bright dialog texture.
  The three `NewInfo1..3` wells hold the size/towns/players glow icons, which the
  engine *adds* to the well (additive blit); their measured brightness centroids
  sit ~8-12 px left of each well's geometric centre.
* **List.** `FileLB` sits on the `TextOld1` body with the Age10 face in
  `TextColor 9415072` (RGB 160,169,143); the picker opens **unselected**, so the
  strip's values stay blank. The capture's row pitch is **17 px** (text tops at
  165, 182, 199, 216, ...), not 15.
* **Header.** `ScenarioLbl` is the screen's own title, centred on the big
  `AoW15WhiteGrey` face: the capture's glyph box is x 281..352, y 7..20.

### Still open (fine-grained, tracked here)

* The well glow icons are the closest-matching assets (`Int\Icons.ILB` 8/10 and
  TitleM's `SidePlyr` face) but the capture's globe/castle/face are brighter and
  slightly narrower; `TODO(RE)` the exact `TImageLibrary` indices the file dialog
  assigns to `LSizeI` / `LTownsI` / `LPlayersI`.
* The Settings panel's lower body and the three bottom `ScenRBut` buttons are a
  few levels brighter than the capture; the panel's `SettingsFogPnl` blend
  (`ILIndexTexture 22`) is not reproduced.
* Fonts are the game's own bitmaps, so residual error is anti-aliasing and glyph
  advance rounding, not wrong faces.

## 1. The flow

```
title orb 0  ->  FUN_0042abf0 (show TMultiWindow)
  SingleBtnClick = FUN_0042b588:
      hide(MultiPnl)
      TSetupControl.SetupHost(g_setup+0x48, 0, 0, 0)
      if form+0xec (a pre-chosen map) != 0: TSetupControl.hPickMap(form+0xec)
      FUN_00410bd8(g_scensetup, g_setup+0x48)      // show TScenSetup
```

`FUN_00410bd8` (5275 bytes) is `TScenSetup.Show`/refresh:

* stores the `TSetupControl` at `Self+0x33c`;
* clears the map-name edit (`+0x50`), sets the counters `+0x338 = 100` and
  `+0x340 = 15`;
* fills the turn-mode combo (`+0xd8` -> `TurnLbl`/`PlayModeCB`) with
  Classic / Simultaneous;
* routes on `TSetupControl.GetRemote`/`GetHost` to lock the host-only controls;
* starts `SONGS\TITLE.IT` (same as the other menu screens);
* calls `FUN_00411460` (the big refresh) which, per race, fills the grid.

`ScenSetupPnlUpdate = 0x004123c0` is the form's `OnUpdate`; it is only a
network ping/validate timer (send a ping every `0x340` frames, re-validate the
map every 100), not a visual animation.

## 2. The class and its methods

`TScenSetup` is a Delphi form in `AoW.exe`. Its RTTI lives in the code section:

```
VA 0x00410891..0x00410b6b   published-method records [u16 size][u32 addr][u8 len][name]
VA 0x00410b6c               class name "TScenSetup"   (TypeInfo tail)
VA 0x00410bb0               class record: "TScenSetup" -> field table 0x0040fbf8,
                            method table 0x0045d5f8, unit "AoWScenSetup"
```

Decoded method table (name -> address). The engine's `InitInheritedComponent`
binds the DFM's `On*` names to these addresses:

| Component | Handler | Address |
|-----------|---------|---------|
| ScenSetupPnl | OnUpdate | `0x004123c0` |
| PickScenarioBtn | OnClick | `0x00412460` -> `FUN_0041243c` (open map picker) |
| StartBtn | OnClick | `0x00412568` |
| ExitGame | OnClick | `0x00411f50` |
| EjectBtn | OnClick | `0x004125a4` |
| MakeHostBtn | OnClick | `0x004125e4` |
| RacePlayerCB | OnChange | `0x00412a24` |
| RaceBehaviorCB | OnChange | `0x00412ac8` |
| PreferredRaceCB | OnChange | `0x00412534` |
| TurnTimeSl | OnChange | `0x0041285c` |
| TurnsSl | OnChange | `0x004129a8` |
| PlayModeCB | OnChange | `0x004129e8` |
| CustomizeLeaders | OnChange | `0x00412b48` |
| LeadersOnMapCB | OnChange | `0x00412b04` |
| AlliedVictoryCB | OnChange | `0x00412b8c` |
| Exploration | OnChange | `0x00412bd0` |
| TacticalCombatCB | OnChange | `0x00412cb8` |
| AlwaysViewTCCB | OnChange | `0x00412c14` |
| SettingsClick | OnClick | `0x00412c2c` |
| RacePnlDrawBack | OnDrawBack | `0x00412c3c` |
| PlayerList | OnChange / OnDrawItem | `0x0041276c` / `0x00412114` |
| Chat (memo/edit) | OnReturn / OnUpdate | `0x00412058` / `0x00412c8c` |

`RacePnlDrawBack` (`0x00412c3c`) draws each race panel's background: it fetches
image `panel+0xc` from the interface image library and blits it at
`(panel+0x84, panel+0x88)+5` (the `+5` is the frame inset).

`FUN_00411460` (the refresh, 19 kB) walks `TSetupControl`'s player/settings
lists, fills the player list, the host/player/file/name labels and calls
`FUN_00410f2c` once per race panel with that race's `TSetupSettings` record.
`FUN_00410f2c` (8.9 kB) is the per-race routine: it sets availability, the
player combo, the behaviour combo, the "Human"/player-name label, the
behaviour/"Unavailable"/"Taken"/"Forced"/"Fixed" info label and the remote/host
visibility of each control.

## 3. Layout (TScenSetup DFM, 640x480 scene coordinates)

The form is flat: every component names its logical parent through the
`AOWWindow` property, and `WinLeft`/`WinTop` are relative to that window.
Absolute scene position = sum of the `WinLeft`/`WinTop` chain up to the form.
The form resource is `assets/dfm/AoW/TSCENSETUP.dfm`; `tools/dfm_dump.py` +
`tools/dfm_tree.py` turn it into the table below.

```
ScenSetupPnl  (0,0)  640x480  TitleM id 4 (Backg03.BMP)   the backdrop
SidesPnl      (10,39) 500x441 (no art; the race grid bounds)
SettingsPnl   (500,40) 134x430  GenericF[72] frame + GenericT[1] texture
```

Race grid: 12 cells of 156x66 at `SidesPnl + (col*164, row*70)`,
`col,row in 0..2,0..3`. Fill order and names:

```
 R1 Humans  (0,0) | R5  Elves     (164,0) | R9  Dark Elves (328,0)
 R2 Azracs  (0,70)| R6  Halflings (164,70)| R10 Orcs       (328,70)
 R3 Lizard  (0,140)|R7  Dwarves   (164,140)|R11 Goblins    (328,140)
 R4 Frostl. (0,210)|R8  High Men  (164,210)|R12 The Undead (328,210)
```

Per cell (relative to the cell's top-left):

```
 R*PlayerIcon   (0,20)  50x46   PlyrIcns (Int\Scenes\PlyrIcns.ILB)
 R*Lbl          top, centred over the cell   Age10s font, colour 14316400
 R*PlayerCB     (50,25) 98x18    GenericF frame + TitleScreen edit
 R*BehaviorCB   (50,45) 98x18    GenericF frame + TitleScreen edit
 R*InfoLbl      (50,59)
 R*FogPnl       cell-sized      drawn over locked cells
```

Settings panel (relative to `SettingsPnl`):

```
 TurnLbl      (10,10)   "Turns "
 PlayModeCB   (10,25) 114x18   Turns: Classic / Simultaneous
 DurationLbl  (10,55)   "Duration"
 DurFr        (10,70) 114x65    GenericF[64] frame (Duration group box)
 TurnTimeLbl  (15,75)   "Turn Time"
 TurnTimeSl   (15,87)  50x11    horizontal slider
 TurnTimeVal  (70,87)
 TurnsLbl     (15,105)  "Turns"
 TurnsSl      (15,116) 50x11
 TurnsVal     (70,117)
 MiscLbl      (10,142)  "Miscellaneous"
 LFr          (10,155) 114x70   GenericF[64] frame (Misc group box)
 LeadersOnMapCB  (15,162)+12x12  "Leaders on map"
 CustomizeLeaders(15,177)        "Customize leaders"
 AlliedVictoryCB (15,192)        "Allied victory"
 Exploration     (15,207)        "Exploration"
 PickScenarioBtn (10,345) 114x24  "Scenario"
 ExitGame        (10,372) 114x24  "Exit"
 StartBtn        (10,399) 114x24  "Start!"
```

## 4. Assets (IntGfxMod -> ILB)

`TIntGfxMod` (`assets/dfm/AoW/TINTGFXMOD.dfm`) names every interface library:

| DFM name | file |
|----------|------|
| TitleScreen | `Int\Scenes\TitleM.ILB` |
| GenericT | `Int\GenericT.ilb` |
| GenericF | `Int\GenericF.ILB` |
| GenericB | `Int\GenericB.ILB` |
| GenericS | `Int\GenericS.ilb` |
| PlyrIcns | `Int\Scenes\PlyrIcns.ILB` |
| LSetup | `Int\Scenes\LSetup.ILB` |

Relevant `TitleM.ILB` entries (id -> name/dims):

```
 1  SetupBg03      640x480   (game-type backdrop)
 4  Backg03        640x480   (Scenario backdrop, ScenSetupPnl ILIndexTexture=4)
20/21/22 TitleIcon 158x24    (game-type buttons)
26/27/28 ScenEBut   10/162/18x18   edit-box frame parts
29/30/31 SetupEdit  7/144/7x24     edit-box parts
41 ScenChk 12x12 unchecked, 42 checked
49..54 ScenCol (combobox parts)
55/56/57  58/59/60  61/62/63 SceneBtn 16/167/13x20  (3 buttons x 3 states)
70/71/72 HeatBtn 66x64       (race-cell "heat" glow frames)
256/0 SidePlyr 51x47         (player side icons)
```

`GenericF.ILB` entries 72..79 (`STGrFr`) and 64..71 (`ScenFr`) are the panel
frames; **32..39 (`DialogFr`, 22px)** is the ornate file-dialog frame and
**113..120 (`FrGoldD`, 3px)** the gold inner frames. `GenericT.ILB` id 1 is
`ShadowOv` (panel tint) and id 2 `MainFloaterSet` (the dialog body texture).

The green celtic buttons are `GenericB.ILB` **19..27 (`ButRCelt2`)** - three
states x three pieces (24/170/24 x27). `Int\WinTIcon.ilb` id 8 is the small
dialog title icon; `Int\Icons.ILB` holds the 51x47 `LoadIcons` used by the
size/towns/players strip; **`Int\ShieldsM.ILB`** (`ShieldsM.BMP` 40x53, ids
0..11) supplies the twelve race crests. All scenario text is `FontModule.Age10`/
`Age10s`, drawn through `TImageLibraryFont.SetColor` (this port tints the glyph
coverage directly).

`PlyrIcns.ILB` holds 6 51x47 `SidePlyr.BMP` icons (image + mask). The DFM
defaults `R*PlayerIcon` to it (`AnimStart=0, CurrentImage=0, AnimEnd=13`); the
per-race crest index is set at run time, so the exact race->icon table is a
`TODO(RE)` (see section 8).

## 5. The "Select a Map..." dialog (TFileDlgWindow)

`PickScenarioBtnClick` (`0x00412460`) stops the music and calls
`FUN_0041243c`, which shows the file dialog. The dialog form is
`assets/dfm/AoW/TFILEDLGWINDOW.dfm` / `TFileDlgWindow`:

```
FileDialog  (130,35) 380x363  TitleM ScenRBut/SetupEdit frame
  TopPnl / DriveFr / DriveL / DriveCmb / DriveSB   drive row
  PathL / PathFr / PathEdit                        path row
  FileLB        the *.HSM list (Scenario\Custom and Scenario\Campaign)
  FileName      the selected file
  NewInfo panel: SSizeI/STownsI/SPlayersI icons + SSize/STowns/SPlayers labels
  LSavedInfo / SavePnl / DateLB ...               the (save) mode's extra rows
  Openbut / CancelBut / UpDirBtn / Close
```

When a file is chosen the panel under the list shows the map's **size** (S/M/L),
**towns** and **players** - the coloured strip in the screenshot ("M | 23 | 4").
In this port those three values are read back from the selected `.HSM` with
`aow::load_map` (size from `Map.width`/`height`, towns = `City.TCity` sprites,
players = distinct city races).

The list itself is `FileLB` (a `TAOWListBox`): `ItemHeight` 17, `lh / 17` = 8
rows visible over the `Scenario\Custom\*.HSM` set (31 files here), so a click
lands on the row **under the cursor** - `Scene::scen_hit` and the render share
one geometry (`Scene::scen_sb`). Its `NSBar` scrollbar is live: the dec/inc
arrows step one row, a track click pages a screenful, the thumb drags, and the
wheel scrolls a row a notch. The bar follows the DFM's `VanishWhenFull` (it is
drawn and hit-tested only while the list is longer than 8 rows) and its thumb
tracks `scen_map_off` (hidden while it all fits). `Scene::scen_action` owns the
`-3000` up / `-3001` down / `-3002` page-up / `-3003` page-down / `-3004` thumb
codes; the thumb drag is tracked in `scen_sb_drag` between `WM_LBUTTONDOWN` and
`WM_LBUTTONUP` so the row hit test and the bar never disagree.

## 6. Reproducing it in the port (the exact art)

`src/game/menu.cpp` gains a third screen (`screen == 2`, "Scenario"). Every
graphic is the entry the form names, resolved through `TINTGFXMOD.dfm`
(`IntGfxMod.TitleScreen` = `Int\Scenes\TitleM.ILB`, `UnitIcons` =
`Int\Icons.ILB`, `TitleIcons` = `Int\WinTIcon.ilb`, `GenericT/F/B/E/S` = their
`Int\Generic*.ILB`, `Shields` = `Int\ShieldsM.ILB`). The first pass's flat
`fill()` rectangles are gone - this is a 1:1 asset map:

| element | form component | asset (DFM `ILIndex*`) |
|---------|----------------|------------------------|
| backdrop | `ScenSetupPnl` | `TitleM` 4 `Backg03.BMP` 640x480 |
| race cell | `R1..R12Pnl` (`TAOWPanel`) | `GenericF` 72..79 `STGrFr.BMP` 9-slice over `GenericT` 1 `ShadowOv.BMP`, blitted at `WinLeft+5, WinTop+5` - exactly what `RacePnlDrawBack` (0x00412c3c) does |
| race crest | `R*PlayerIcon` (0,20 50x46) | `Int\ShieldsM.ILB` ids 0..11 (see sec. 8) |
| race name | `R*Lbl` (48+its WinLeft, 9) | `FontModule.Age10s`, each race's own `TextColor` |
| race combos | `R*PlayerCB`/`R*BehaviorCB` (50,25)/(50,45) 98 wide | `GenericE` 6..8 `ComboBLight.BMP` + `Age8s` |
| "Unavailable" | `R*InfoLbl` (97,59) | `Age8s`, `TextColor` 14316400 |
| settings panel | `SettingsPnl` (`TAOWPanel`) | `GenericF` 72 `STGrFr` + `GenericT` 1 `ShadowOv` |
| Duration / Misc groups | `DurFr` / `LFr` (`TAoWFrame`) | `GenericF` 64..71 `ScenFr.BMP` |
| labels | `TurnLbl`/`DurationLbl`/`MiscLbl`/`TurnTimeLbl`/`TurnsLbl`/... | `Age8s`, `clSilver` (headings) / `clGray` (values) |
| sliders | `TurnTimeSl`/`TurnsSl` (`TAOWHScrollBar`) | `GenericS` 49..51 track + 52..54 thumb `ScenSlid.BMP` |
| checkboxes | `LeadersOnMapCB`/`CustomizeLeaders`/`AlliedVictoryCB`/`Exploration` | `TitleM` 41/42 `ScenChk.BMP` 12x12 |
| bottom buttons | `PickScenarioBtn`/`ExitGame`/`StartBtn` | `TitleM` 32/33/34 (up) 35/36/37 (down) 38/39/40 (lit) `ScenRBut.BMP`; caption `Age10s`, `TextColor` 15134446 |
| dialog frame | `FileDialog` (`TAOWWindow`) | `GenericF` 32..39 `DialogFr.BMP` over `GenericT` 2 `MainFloaterSet.BMP` |
| dialog title icon | `TitleIcon` | `WinTIcon` 8 `MainHeaderIcon.BMP` |
| dialog list | `FileLB` (`TAOWListBox`) | `GenericF` 113..120 `FrGoldD.BMP` over `GenericT` 21 `Text2Lb17.BMP` / `Menus` 8 `PopUpSet.BMP` |
| dialog edits | `DriveCmb` / `PathEdit` | `GenericE` 15..17 `NGEdit.BMP` / 21..23 `NGCmb.BMP` |
| info strip | `NewInfo` / `NewInfo1..3` | outer `GenericF` 105 `FrThinD` over `GenericT` 17 `TextOld1.BMP`; wells `GenericF` 113 `FrGoldD` over `GenericT` 17 |
| info icons | `SSizeI` / `STownsI` / `SPlayersI` | `Icons` id 7 / 8, `TitleScreen` id 10 (`TImageNode` glow + art layers) |
| Select / Cancel | `Openbut` / `CancelBut` | `GenericB` 19/22/25 `ButRCelt2.BMP`, `Age10`, `TextColor` clBlack |

* `--screen scenario` starts on it; clicking the Game Type screen's **Single
  Computer** button opens it;
* clicking **Scenario** opens the map picker overlay, which lists
  `Scenario\Custom\*.HSM` from disk and shows the size/towns/players strip;
* a race reads **"Unavailable"** until the chosen map is confirmed: the DFM's
  `R*InfoLbl` is only hidden by `FUN_00410f2c` when the setup control's player
  list holds that race, and the reviewed screen (picker still open) shows all
  twelve locked. Clicking **Select** loads the map and lights its races, which
  then fade in over the glow counter (`kScenGlowFrames`) - the analogue of the
  engine's per-race availability pass.

The build's scenario smoke test pins the recovered art
(`scen_art panel=2x2/256x256 mainbtn=170x18 dlg=18x22 body=190x192
list=50x50/32x32 info=213x143 slider=29x11 vsb=17x15 edit=90x18 combo=90x18
font10b_ok=1`), so a missing entry fails the build.

## 7. Running the replica

```powershell
./build.ps1                  # builds port/bin/AoW.exe
./bin/AoW.exe --root ".." --screen scenario
./bin/AoW.exe --root ".." --screen scenario --scen-frame 90 --render scen.png
```

Flags: `--screen title|gtype|scenario`, `--scen-frame N` (0 = all fogged,
`kScenGlowFrames` = fully lit), `--scen-maps` (open the "Select a Map..." picker
in the render), `--scen-pick <name>` (confirm a map: lights its races and sets
each `R*PlayerCB` from the map), `--scen-dd N` (open race cell N's PlayerCB
drop-down in the render), `--hover N` (0..2 = the three buttons).

## 8. Known deviations / open items

* ~~**Race crest icons.**~~ **Solved (see section 10).** `R*PlayerIcon` really is
  `IntGfxMod.PlyrIcns` (six `TImageNode`s, one per `TPlayerType`); the heraldic
  race crest comes from the *panel*: `R*Pnl.OnDrawBack = RacePnlDrawBack`
  (0x00412c3c) blits `IntGfxMod.Shields` (= `Int\ShieldsM.ILB`) at the panel's
  `(WinLeft + 5, WinTop + 5)` with the index the DFM stores in the panel's `Tag`
  (R1Pnl 0, R5Pnl 4, ... the 0/4/8/1/5/9/2/6/10/3/7/11 grid order).
* **Race-name colour.** `TAOWLabel.TextColor` is a `TColor` handed to
  `TImageLibraryFont.SetColor`, which maps it into the font's own colour palette
  (see `TFontModule`: Age10s defaults to `Color` 9236989 /
  `FontColorPaletteIndex` 2). The replica tints each glyph's coverage with the
  DFM `TextColor` directly, which reproduces the reviewed per-race tints
  (Humans 112,115,218 ... The Undead 146,125,118) but not the palette rounding.
  TODO(RE): decode `TImageLibraryFont.SetColor`'s palette search.
* **Dialog field text colour.** The `GenericE` edit/combo `TextColor` values in
  the DFM (11002593 for `FileName`/`DriveCmb`) are pale, which cannot be what the
  shipped screen shows on a pale field - another `SetColor`-palette effect. The
  replica draws the edit captions dark, matching the reviewed screen.
* ~~**Combo skin.**~~ **Solved (see section 10).** Every combo on the form names
  `ILEdit = IntGfxMod.TitleScreen` with `ILEIndex = 26` - i.e. `TitleM`'s
  `ScenEBut.BMP` 3-piece (10/162/18 x 18), whose right cap carries the dark-green
  drop-down funnel. The replica used `GenericE`'s `ComboBLight` before.
* **Map stats / race availability.** The size band (S/M/L) comes from the hex
  dimensions; towns = the `City.TCity` sprite count (Barondir Bay: 23, matching
  the reviewed screen); players = the exact roster size from the map's
  `TAoWHSMap` tag 0x15 `TPlayerList`. The lit-race set *is* the roster now: each
  player's `TPlayer` rw tag 0x2 is its race and **rw tag 0x3 its player type**
  (0 Human, 1/2/3/6/7 the CPU levels, 4 Independent - see
  `docs/14_GAME_STATE.md`), read by `FUN_00411460`'s per-race pass. The map may
  mark several slots type 0; the engine's `TSetupControl.hPickMap` (0x557e20c8)
  leaves only the **first** one Human and turns the rest into CPU Squire, and the
  replica now does the same, so a map whose stored types are all 0 shows one lit
  "Human" and three "CPU Squire" (it used to show four Humans). Only the picker's
  **Select** lights them (highlighting a row just fills the info strip), which is
  why the reviewed capture (picker open) shows all twelve "Unavailable".
* **A missing race tag means Humans.** `TPlayer`'s race (rw tag 0x2) is a plain
  `rwByte`, and `TEReadStorageStream.rwByte` (Enginep vtable +0x30) stores **0**
  into the field whenever the tag is absent - so a map that never wrote a race
  has Humans (0), *not* `TPlayer.Create`'s 0xff sentinel. Many shipped maps omit
  tag 0x2 for exactly one player (Europe player 1, Barondir Bay player 3, Blood
  Isles player 4, the `Valley*.csm` maps' player 5, ...) and the original lights
  their Humans cell: on `AoW Europe Heaven Games.hsm` the original shows **eight**
  races - Humans (Human) plus the seven the map stores - whereas reading the
  unraced player as 0xff skipped it and showed only seven. `parse_player`
  (game_state.cpp) now takes that 0 default (and applies `TPlayer.ReadWrite`'s
  tail, `index == 0` -> type 4), so the lit-race set is the roster's races with
  the stream default applied, for every map - no per-map special case.
* **Dropdowns.** The race cells' `R*PlayerCB` is interactive: clicking it opens
  the seven-entry player-type list (`TSetupControl.ListAvailablePlayers`
  0x557e1660 -> the order 0,1,2,3,6,7,4 = Human, CPU Squire, CPU Knight, CPU
  Lord, CPU King, CPU Emperor, Independent), drawn as the DFM's
  `ILTextureNormal` / `ILTextureSelected` (GenericT 4 / 16) at `ItemHeight` 15;
  choosing a row calls the recovered `RacePlayerCBChange` (0x00412a24) ->
  `hSetPlayerID` (0x557e1eb4) path. Marking a race **Human** resets the other
  Humans (`ResetPlayerSettings` 0x557e1378: a slot returns to its map default,
  and a default-human slot becomes CPU Squire), so exactly one race is Human.
  `Start!` then applies the choices to the roster exactly as
  `TSetupControl.SetupMap` (0x557e0b64) does (`player[+0xa7] = settings[+8]`),
  which is what makes the chosen race's cities and armies - its **units** - the
  ones you play (see docs/14_GAME_STATE.md, "Which race you play"). The behaviour
  combo (`R*BehaviorCB`, "Normal"), the other combo lists and the leader-setup
  (`TLeaderSetupWin`) sub-window are still drawn as resting frames.
* **Network rows.** The chat/edit/player-list widgets and `ScenSetupPnlUpdate`'s
  ping timer implement the single-player subset only.




## 9. Fidelity pass 2 - pixel parity with `bin/ScenarioMenuRef.png`

`bin/ScenarioMenuRef.png` is a 369x359 crop of the shipped picker; the matching
crop of the port render starts at **(134,37)** (the dialog itself is the
design-time 380x363 at (130,35), with the `DialogFr` frame drawn 13 px inset).
The final render is `bin/scenario_final.png`:

```
python tools/pixmatch.py re/mw/final.png bin/ScenarioMenuRef.png \
    --render-origin 134 37
mean_abs_diff_per_channel = 10.11
match_within_10/channel =  69.2%
match_within_20/channel =  84.6%
match_within_30/channel =  91.4%
match_within_45/channel =  94.7%
match_within_60/channel =  96.1%
```

The text runs are on the exact columns: title `x 38..121`, first list row
`x 56..162`, path `x 70..329`, Select/Cancel captions and every list/field
label. What remains is the 16-bit/dithered source colouring, not geometry.

Recovered with GhidraMCP for this pass (`AoWInterface.dpl`, `Ilpack.dpl`,
`AoW.exe`):

* **ILB `TImageNode` layers.** The UI sprite libraries interleave a node
  header per multi-layer sprite (`u32 reference, u32 id, u32 class=0x100,
  u32 class=0x16, descriptor, u32 0, u32 class=0x16, descriptor`). The index is
  the u32 before the `0x100`; the second layer starts with a 0 index. The
  loader now stamps that id on both layers. `Int\Icons.ILB` id 7 is the
  white-glow + **blue globe**, id 8 the **gold glow + white castle**;
  `Int\Scenes\TitleM.ILB` id 10 is the **lavender glow + bald leader face**.
  `TImageNode.Show` (`Ilpack.dpl` 0x5520d654) blits the layers in order.
  The port draws the glow layer additively and the art layer colour-keyed,
  which reproduces the reference globe's pixels exactly (e.g. 57,99,99 /
  41,90,41 / 107,115,214).
* **Info strip.** `S*I` name `UnitIcons` 7/8 and `TitleScreen` 10;
  `NewInfo1..3` are `GenericF` 113 (FrGoldD) over `GenericT` 17 (TextOld1) at
  x 1/105/207, widths 105/103/103, y +20, height 39. The outer `NewInfo`
  (and `NewLBFr`) frame is `GenericF` 105 (FrThinD); the shipped strip's body
  colours match `TextOld1` (17), not the darker save-mode `TextOld2` (18) -
  measured mean (34,40,21) vs (38,43,22) for 17 and (7,19,8) for 18.
* **List.** `FileLB` uses `ILTextureNormal = GenericT` 21 (`Text2Lb17`, the
  banded body), `ILTextureSelected = IntGfxMod.Menus` 8 (`PopUpSet`), frame
  `GenericF` 113, `ItemHeight` 17, `BorderWidth` 21 (the text starts at
  `lx + 25` in the port), 8 visible rows. The body colours now match the
  capture (35,38,21 vs 37,41,22).
* **9-slice edges are tiled, never stretched.** `TAOWPanel.Draw`
  (AoWInterface 0x0041f990), `TAOWEdit.Draw` (0x004108ec) and
  `TAOWButton.Draw` (0x0040b338) all loop `width / middle.width`; stretching
  the 190x22 `DialogFr` top edge and the 170x18 `ScenRBut` middle distorted the
  woven bands. `blit9_impl` / `blit_btn3` now tile with the target rect as clip.
* **Both celtic bands.** The flat `frHorizLine` is `CeltDeco` 55/56/57
  (7/100/7 x 27). The interlocking band under the title is `TCeltL` (the flat
  strip at (21,22), 338x27) plus `TCelt` tiling the 27x27 knot `CeltDeco` id 3
  at (28,22), 322x27 (`TAOWImage.Draw` loops over w/iw). The bottom band is
  `BCeltL` plus the three `CeltL/M/R` knots at x 27/177/326.
* **Close box / UpDirBtn.** `Close` is `GenericI` 0/1/2 (`MainHeaderBut`, 15x16)
  at (345,4); `UpDirBtn` is `GenericI` 56/57/58 (`NGButDirBack`, 32x21) at
  (313,78) - both were hand-drawn before.
* **Title.** `TitleIcon` is `WinTIcon` id 8 at (20,5); the title is Age10 at
  (42,6) with the font's CharacterDistance of 1 (`TImageLibraryFont.GetTextWidth`
  adds +0x50 per glyph). The title run ends on the reference's column 121;
  list/field/button runs are likewise exact.
* **Dialog body.** `MainFloaterSet` (GenericT 2) at the frame inset; the
  remaining difference is the source capture's 16-bit palette dithering.

Open items (unchanged): the modal shadow sprite/geometry, the network widgets,
and the leader-setup sub-window are not part of this pixel benchmark.

## 10. The Race Selector state - a 1:1 pass against `bin/RaceSelectorRef.png`
   and `bin/RaceSelectorRef2.png`

Both captures are 640x480 screenshots of the same screen: `RaceSelectorRef.png`
(635x479, origin **(1,1)** in the port's frame) is the state *before* a map is
chosen - every cell "Unavailable", the `Start!` button absent - and
`RaceSelectorRef2.png` (635x472, origin **(6,4)**) is the state after the
`Select a Map...` dialog confirmed **Blood Isles.hsm** (title "Scenario: Blood
Isles", four races lit, `Start!` present).

```
bin/AoW.exe --root .. --screen scenario --scen-pick "Blood Isles.hsm" \
    --scen-frame 90 --render re/rs_cur.png --no-music --no-cursor
python tools/pixmatch.py re/rs_cur.png bin/RaceSelectorRef2.png \
    --render-origin 6 4
mean_abs_diff_per_channel = 4.41
match_within_10/channel =  92.5%
match_within_20/channel =  96.4%
match_within_30/channel =  96.9%
match_within_45/channel =  97.6%
match_within_60/channel =  98.6%

bin/AoW.exe --root .. --screen scenario --scen-frame 90 \
    --render re/rs_none.png --no-music --no-cursor
python tools/pixmatch.py re/rs_none.png bin/RaceSelectorRef.png \
    --render-origin 1 1
mean_abs_diff_per_channel = 2.71
match_within_10/channel =  95.1%
match_within_30/channel =  98.6%
```

(Before this pass the same screen scored 30.6 / 24.6 % within 30.)

Facts recovered for this pass - `AoW.exe` through GhidraMCP
(`FUN_00410f2c` = the per-race refresh, `FUN_00411460` = the screen refresh,
`FUN_00412c3c` = `RacePnlDrawBack`, `FUN_00412a24` = `RacePlayerCBChange`), the
method table at `0x00410911` (25 published TScenSetup handlers, each
`<u16 idx><u32 addr><u8 len><name>`), `assets/dfm/AoW/TSCENSETUP.dfm`
and the shipped ILBs:

* **The race crest is the panel's background.** `RacePnlDrawBack` is
  `ImageLib.Get(<IntGfxMod.Shields>, panel-><race>, ...)->Show(WinLeft+5,
  WinTop+5)`; the race index is the DFM's per-panel `Tag`. A `TSprite16`
  anchors its visual box at `base + class shift`, so the crest lands 7 px right
  / 9 px down of the cell's corner (measured against the capture, template match
  mean 10.9). `R*PlayerIcon` is *not* the crest.
* **`R*PlayerIcon` = the player-type icon** (`IntGfxMod.PlyrIcns`,
  `Int\Scenes\PlyrIcns.ILB`): six `TImageNode`s, each an aura layer plus the
  emblem (skull / goblin face / crown ...). `FUN_00410f2c` picks the node with
  `TAOWImage.SetCurrent`: type 0 (Human) -> 0, 1/2/3 (CPU Squire/Knight/Lord) ->
  1/2/3, 6/7 (King/Emperor) -> 4/5; it is hidden for 4/5 (Independent/None).
  Draw order matches the capture: the **aura under** the crest, the **emblem
  over** it (the crest's lower tip stays clean, the emblem overlaps it). The
  glyph's pixels are reproduced exactly (template match mean 0.0).
* **Combos** (`R*PlayerCB` 50,25 and `R*BehaviorCB` 50,45, 98x18) are
  `TitleM`'s `ScenEBut` 3-piece (`ILEdit = TitleScreen`, `ILEIndex = 26`,
  `BorderWidth 6`, `ItemHeight 15/13`, `TextColor` = the race's own colour).
  `FUN_00410f2c` shows `R*BehaviorCB` only for the CPU types 1..3/6/7 - which is
  why the *Human* cell (Goblins on Blood Isles) has a single combo in the
  capture.
* **`R*InfoLbl`** ("Unavailable") is `awCenter / ahBottom` with `BottomOffset 7`
  inside `R*CenterPnl` (48,0 99x66), in the race's own `TextColor`: its ink sits
  at cell + (72..120, 48..55) in both captures.
* **The grid's fog.** An unavailable cell is black at alpha **208** over its
  contents (least-squares fit of the crest against `ShieldsM` id 4/0, RMS 4.0).
* **Lit races appear immediately.** `scen_glow` is what gates a *lit* cell's
  `R*FogPnl` (`FUN_00410f2c`: fog = `kScenGlowFrames - t`), but the interactive
  `WM_TIMER` handler used to leave it at 0 - only the head-less `--scen-frame`
  path set it - so in the game a confirmed map's available races stayed as dark
  as the unavailable ones. A second attempt stepped the counter once per 33 ms
  tick (a 3 s fade), which the original does not do: `FUN_00411460` drops every
  lit cell's fog the moment the map is loaded. The port now does the same -
  `Scene::select_map(confirm = true)` sets `scen_glow = kScenGlowFrames`, so the
  cells are bright on the frame right after `Select`, and there is no animation
  state left in `WM_TIMER`. Measured on the very first render:
  Humans crest (123,129,49) = the capture's value, Elves (15,22,15) = fogged.
* **The masked panel body.** The race cells and `ChatPnl` show *16 % of
  `Backg03`* (e.g. (460,230): backdrop (148,76,90) -> (24,8,16)) plus the
  `ShadowOv` tile at ~9 % - i.e. a black veil at 215 with the tile over it, not
  an opaque fill. The same veil reproduces `SettingsPnl`, whose body is the
  backdrop's brown ((560,300) = 16 % of (238,206,164) = (38,33,26), capture
  (49,41,33)); its `DurFr`/`LFr` groups are `frHollowBox` (no interior texture).
* **`ChatPnl`** (SidesPnl 0,286 -> abs (10,325) 484x145) is the bottom strip:
  `ChatMemo` (10,12) 299x93 (`ScenFr` frame + `blank1` body), `ChatEdit`
  (10,114) 299x21 (`TitleM` `ScenEd` 23/24/25), and `ConnectionPnl` (318,12)
  with the three column headers `Players` / `Ping` / `Pref. Race`
  (`TAOWButton` with `ILIndexUp = ILIndexDown = 52`, `ILIndexLit = 49` =
  `TitleM`'s `ScenCol` 2/52/2 x 15) over 70/25/61 x 78 `ScenFr`-framed lists.
  Everything inside the strip is veiled by `ChatFogPnl` (GenericT 1) at black
  alpha **194** - the "Players" caption's (238,238,230) reads (57,57,49) in the
  capture.
* **Checkboxes** are `IntGfxMod.GenericI`'s `NGCBox.BMP`: id 47 (empty, the
  gold-bevelled box) and id 48 (checked, the blue glow) - the DFM's
  `ILIndexUp/Down/Lit = 47/48/49`. `TitleScreen` 41/42 (`ScenChk.BMP`) is a
  *round* 12x12 ring and is not what ships. The setup defaults "Leaders on map"
  and "Exploration" to *on*.
* **Title.** `ScenarioLbl` is `awCenter` on the full-screen `ScenSetupPnl`
  (`TopOffset 8`), so the caption is centred on x = 320: "Scenario: Blood Isles"
  inks x 240..399, and the empty selection still carries the colon + space
  ("Scenario: " = 80 px), which is why its "Scenario:" ink starts on 281.
* **`Start!`** only exists once a map is confirmed; the pre-pick capture shows
  `Scenario` / `Exit` alone.

Remaining difference: the 16-bit palette dithering of the source captures and
the engine's font antialiasing (the port draws flat-ink glyphs, so a caption has
more full-bright pixels than the capture).

