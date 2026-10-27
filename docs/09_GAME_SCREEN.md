# 09 - The in-game screen (TWorldMap), recovered and reproduced

After the Scenario screen's map is chosen the game drops into the main play
screen: the rendered map under three pieces of interface -

* the **bottom bar** (`TTBWindow` in `AoW.exe`), carrying the five task tabs
  (World Map / Realm / Magic / Relations / Events), the start-menu button, the
  next-turn / turn buttons and the gold / mana read-outs;
* the **World Map floater** (`TSWindow`) - the minimap with its Surface /
  Caverns tabs and three view toggles;
* the **Events floater** (`TPInfoW`) - the leader banner and the event history.

This stage reproduces all three, drawn over the port's existing map renderer
(`src/engine/mapviz.cpp`), with every graphic taken from the entry its form DFM
names and every position derived from the DFM plus the engine's layout routine.

All `AoW.exe` addresses are image base `0x00400000`; the UI toolkit addresses
are `AoWInterface.dpl` (base `0x00400000`). Evidence was gathered with GhidraMCP
(`tools/ghidra/`) - see the "functions mined" list at the end.

## 1. The layout engine: `TAoWComponent.ReAlignComp`

A form DFM stores each component's design-time `WinLeft` / `WinTop` / `WinWidth`
/ `WinHeight` **and** a block of `Alignment.*` fields. At runtime the window
manager recomputes every component's box from those fields, so the `WinLeft`
values are *not* the shipped positions. The routine that does it is

```
AoWInterface.dpl  0x00401f2c  AoWComp.TAoWComponent.ReAlignComp
AoWInterface.dpl  0x00402af4  AoWWinControl.TAOWWindowControl.ReAlignComp (calls it)
```

Decompiled (`re/exports/aowiface/TAoWComponent.ReAlignComp.c`), with
`parent_w/height = [parent+0x78/+0x7c]`, the component's `w/h/x/y =
[+0x78/+0x7c/+0x80/+0x84]`, `WidthPercent = +0xac`, `HeightPercent = +0xad`,
`LeftOff % = +0xb0`, `RightOff % = +0xb4`, `TopOff % = +0xb8`,
`BottomOff % = +0xbc`, `LeftOffset = +0xc0`, `RightOffset = +0xc4`,
`TopOffset = +0xc8`, `BottomOffset = +0xcc`:

```
AlignWidth(A) = 0 = awNone  : (leave x/w)
                1 = awLeft  : x = parent_w * LeftOffPercent / 100 + LeftOffset
                              w = WidthPercent ? WidthPercent * parent_w / 100
                2 = awRight : w = WidthPercent ? WidthPercent * parent_w / 100
                              x = parent_w - RightOffset - w
                                  - parent_w * RightOffPercent / 100
                3 = awBoth  : x = parent_w * LeftOffPercent / 100 + LeftOffset
                              w = parent_w - x - RightOffset
                                  - parent_w * RightOffPercent / 100
                4 = awCenter: x = parent_w / 2 - w / 2
```

and the vertical pair is identical with `Top/Bottom/Height`. The replica encodes
exactly this (`Scene::render_game` and the `kTb*` / `kFloat*` / `kEvent*`
constants), evaluated at the design size 640x480 and then scaled uniformly to
the window.

### 1.1 The map window (`TMWindow.MapWindow`) and its frame

The main map is `TMWindow.MapWindow`, a `TAOWWindow` anchored `awLeft`/`ahTop` at
(0,0). Its DFM gives `WinWidth 440` / `WinHeight 432`, `BorderWidth 11` /
`BorderHeight 11`, `UseManagerBorders` and `ILIndexFrame 8`; at the design size
the 200 px floater column to its right (440 + 200 = 640) and the 48 px bar below
(432 + 48 = 480) tile the screen, which is why the two floaters sit at x = 440
(sections 3 / 4). The replica sizes the window to the area left of the floater
column and above the bar (`Scene::game_layout`, `L.map_window`), so at 640x480 it
is that same 440x432.

`ILIndexFrame 8` opens `Int\GenericF.ILB`'s **`MapWindow.BMP`** set: a 9-slice of
12x12 corners with 419x12 top / bottom and 12x408 side edges, ordered
TL, top, TR, left, right, BL, bottom, BR - the same layout the floater 9-slice
(`MainFloaterSetR`, ids 0..7) uses. `Scene::render_game` draws it with
`blit_frame_box`, which *tiles* the edge art 1:1 across the span between the
corners (never stretches it - the engine's `TAOWWindow.Draw@0x00407654` repeats
its ILFrame art) and then blits the corners on top. The scene is inset to the
window's client box by `w - 0x18` / `h - 0x18` (`AoW.exe 0x00451268`), i.e.
exactly the frame's 12 px, so the corners are the 12x12 art verbatim and the
client box (the camera viewport, section 6) is 416x408.

Layout and texture were checked pixel-for-pixel against `bin/CornersRef.png` (the
shipped 440x432 window captured at a (+1,-1) offset): the 12 px frame ring matches
to a mean per-channel absolute difference of ~2.3 - the capture's own compression
noise - with all four 12x12 corners and every edge row / column aligned.

## 2. The bottom bar (`TTBWindow`, `assets/dfm/AoW/TTBWINDOW.dfm`)

`TTBWindow.TaskWindow` is a `TAOWWindow` 640x48 anchored `awBoth`/`ahBottom`
(so at y = 480-48 = 432). Its frame/texture are assigned in code - the 3-piece
`Int\TBWin.ILB` **BarSet.BMP** (49 + 436 + 155 = 640 x 48) is that body.

| component | DFM `WinLeft,WinTop` | size | AOWWindow | art (DFM `IL*`) |
|-----------|----------------------|------|-----------|-----------------|
| `StartMenuBar` | 4,7 | 40x38 | TaskWindow | `TBWin` DragonButton, up 11 / down 8 (3-piece) |
| `WMButton` "World Map" | 72,0 | 70x18 | TaskPanel | `TBWin` BarButtonLong 14 / lit 20 / down 17 |
| `OVButton` "Realm" | 145,0 | 70x18 | TaskPanel | idem |
| `OV2Btn` "Magic" | 218,0 | 70x18 | TaskPanel | idem |
| `RelationsBtn` "Relations" | 0,0* | 70x18 | TaskPanel | idem (*placed by `LeftOffPercent` 42) |
| `InfoBtn` "Events" | 291,0 | 70x18 | TaskPanel | idem (56%) |
| `ConButton` / `StructButton` / `ChatBtn` | - | 70x18 | TaskPanel | hidden in single-player |
| `TBNextButton` | 568,7 | 26x38 | TaskWindow | `TBWin` NextButton **3** (idle) / **4** (held) - one picture each, not a 3-piece |
| `TurnButton` | 596,7 | 41x38 | TaskWindow | `IntGfxMod.TurnButton` = `Int\TBTurnBt.ILB`, frame `11 + 7*race` (see docs/22) |
| `WGoldPnl` | 423,26 | 69x21 | TBWorld | `TBWin` BarSet2 **51**, `awRight` RightOffset 148 |
| `WManaPnl` | 495,26 | 69x21 | TBWorld | `TBWin` BarSet2 **52**, RightOffset 76 |
| `TCTimerImg` | 385,28 | 33x17 | TBWorld | `TBWin` BarSet2 **28** (clock body), RightOffset 222 |
| `TCTimeImg` | 371,29 | 11x14 | TBWorld | `IntGfxMod.LBIcons` = `Int\LBIcon.ILB` **2** (hourglass), RightOffset 258 |
| `TCTimeLbl` | 388,32 | - | TBWorld | FontModule.TBarNumber, clBlack, RightOffset 225 |
| `PInd` | 290,28 | 75x17 | TBWorld | `TBWin` PlyrIndL **29/30/31** (2+5+2 x17 3-piece), RightOffset 275 |

`TBTurnBt.ILB` holds 102 frames: frames 0..10 are the swirling "turn changing"
glow and frames `11+7*r .. 17+7*r` one block per **race** r (0..12, 12 = no
seat), whose last frame is the block's transition swirl. `TAOWTurnButton`
(`AOWTools.dpl`, unit `TurnButton`) draws `AnimStart + Frame` - `11 + 7*race`
at rest - and plays a state-machine animation when the seat changes. The whole
class, the two buttons' handlers and the turn loop they drive are documented in
**`docs/22_TURN_BUTTONS.md`**; the proof sheet is `bin/TurnButtons.png`.


`PInd` is painted by the form's `TBWorld.PIndDraw` (not yet decompiled): it lays
one **PlyrIndL** (2 + 5 + 2 x 17) pill per game player, right-aligned in the
strip. The replica draws the reviewed count (5) at the reviewed pitch (6 px);
the pill colours follow the scenario's player list, so they are not fixed.

`TBWorld` also carries `TCTimerImg`/`TCTimeImg`/`TCTimeLbl` (the turn clock, on
the right) and the `PInd` player-indicator strip, plus `WGoldPnl`/`WManaPnl` and
their labels. The world screen shows the `W*`/`TC*`/`PInd` set; the `T*`
(`TGoldPnl`/`TManaPnl`/...) twins belong to the combat bar (`TBCombat`) and are
not drawn here.

`TaskPanel` is a `TAOWPanel` at `WinLeft = LeftOffset = 48`, width 506
(`RightOffset = 86`) inside the 640-wide bar. Each tab is `awLeft` with
`WidthPercent = 14` and `LeftOffPercent` 0/14/28/42/56, so `ReAlignComp` puts
them at `48 + 506*pct/100 + LeftOffset` with width `70`:

```
World Map 48   Realm 120   Magic 193   Relations 266   Events 339
```

The gold / mana panels carry `WGTLbl`/`WMTLbl` (the totals, FontModule
**TBarNumber** = `Int\Fonts\Age10tb.ILB`, clBlack, `awRight` RightOffset 23 on
the panel), `WGSign`/`WMTSign` (the "+", FontModule **Age8** =
`Int\Fonts\Age8.ILB`, RightOffset 16) and `WGILbl`/`WMILbl` (the per-turn
income, Age8, centred in the 16 px `WGIPnl`/`WMIPnl` inset RightOffset 1). Both
the sign and the income use the DFM's green `TextColor` 0x77FF77 =
(119,255,119). Tab captions are `FontModule.Age10` (`Int\Fonts\Age10.ILB`),
`TextColor = clBlack`.

## 3. The World Map floater (`TSWindow`, `assets/dfm/AoW/TSWINDOW.dfm`)

`TSWindow.ScannerWindow` is a `TAOWWindow` 200x216, `awRight`/`ahTop` (so at
x = 440, y = 0). `ILFrame = IntGfxMod.GenericF` (id 0) and
`ILTexture = IntGfxMod.GenericT` (id 2). GenericF 0..7 is **MainFloaterSetR**, a
9-slice whose top corners are 6x20 (the title bar) and bottom corners 6x6;
GenericT 2 is **MainFloaterSet.BMP** (190x192, the muted olive body).

| component | WinLeft,WinTop | size | art |
|-----------|----------------|------|-----|
| `STitleIcon` | 2,2 | 19x16 | `IntGfxMod.TitleIcons` = `Int\WinTIcon.ilb` id 0 |
| `STitleLabel` "World Map" | 25,4 | - | FontModule.Age10, clBlack |
| `Help` | 165,1 | 15x16 | `GenericI` 59 / lit 61 / down 60 |
| `SMinIcon` | 180,1 | 15x16 | `GenericI` 3 / lit 5 / down 4 |
| `ScannerFrame` | inset | - | `IntGfxMod.ScanWin` = `Int\ScanWin.ILB` 0..7 (9-slice) |
| `Scanner` | 30,32 | 131x152 | `ScanWin` 20 (the minimap itself) |
| `BuildingCheck` | 160,37 | 15x15 | `ScanWin` ScanIcon 8 / lit 0 / down 9 |
| `ArmyCheck` | 160,52 | 15x15 | idem |
| `NaturalCheck` | 160,67 | 15x15 | idem |
| `ScannerTab` | 31,182 | 145x15 | `ScanWin` ParchClm 14 / down 17 (3-piece) |

The tab captions are `Surface` / `Caverns` (the third level, Depths, appears on
maps that have one - the replica keeps the two the reviewed screen shows).

## 4. The Events floater (`TPInfoW`, `assets/dfm/AoW/TPINFOW.dfm`)

`TPInfoW.PInfoWin` is a `TAOWWindow` 200x216, `awRight`/`ahBottom` with
`BottomOffset = 48`, so it sits at x = 440, y = 216 (directly above the bar).
Same `GenericF` 0 / `GenericT` 2 frame and body.

| component | WinLeft,WinTop | size | AOWWindow | art |
|-----------|----------------|------|-----------|-----|
| `SIcon` | 2,2 | 19x16 | PInfoWin | `TitleIcons` id 1 |
| `IWTitle` "Events" | 25,4 | - | PInfoWin | FontModule.Age10, clBlack |
| `Help` / `Min` | 153,1 / 181,1 | 15x16 | PInfoWin | `GenericI` 59/61/60 and 3/5/4 |
| `PInfoPnl` | 6,19 | 188x66 | PInfoWin | `GenericF` 105..112 + `GenericT` 17 |
| `FaceFr` (leader portrait) | 1,1 | 52x64 | PInfoPnl | `GenericF` 113..120 + `GenericT` 17 |
| `ShieldFr` (race crest) | 136,1 | 52x64 | PInfoPnl | `GenericF` 113..120 + `IntGfxMod.CityWin` 5 |
| `RShield` | 6,6 | 40x53 | ShieldFr | `IntGfxMod.Shields` = `Int\ShieldsM.ILB` id 8 |
| `InfoFr` | 52,0 | 84x46 | PInfoPnl | `GenericF` 113..120 + `GenericT` 17 |
| `TurnFr` | 52,45 | 84x20 | PInfoPnl | idem |
| `Player` | 70,6 | - | PInfoPnl | FontModule.Age8, TextColor 9415072 |
| `LeadL` "of the" | 80,18 | - | PInfoPnl | idem |
| `Race` | 69,31 | - | PInfoPnl | idem |
| `Turn` "Day N" | 77,50 | - | PInfoPnl | idem |
| `EventLbl` "Event History" | 69,88 | - | PInfoWin | FontModule.Age8, clBlack |
| `EventFr` (the list) | 6,100 | 188x110 | PInfoWin | `GenericF` 105..112 |
| `EventList` | 1,1 | 187x108 | EventFr | `TAOWListBox`: `GenericF` 113 frame, `GenericT` 21 / `IntGfxMod.Menus` 8 textures, ItemHeight 17 |
| `EventSB` (scrollbar) | 171,1 | 17x108 | EventFr | `GenericS` 94/96/98..101, hidden while short |

`TextColor` 9415072 is `TColor` `$008FA9A0` -> RGB **(160,169,143)** (pale sage),
the same value `docs/08_SCENARIO_SCREEN.md` reads off the dialog captions.

### 4.1 The leader banner (`FUN_00423170` + `FaceFrDraw` `FUN_00423770`)

The `PInfoPnl` banner is not static art: the floater's update routine
(`AoW.exe 0x00423170`) fills it from the **seated player** - the roster entry
`TPlayerList.GetPlayers(AoWHSMap+0x140, AoWHSMap+0xa5)` names - and `FaceFr`'s
own `OnDraw` (`0x00423770`) paints the portrait:

```
FUN_00423170:
    p = TPlayerList.GetPlayers(aowhsmap+0x140, aowhsmap+0xa5)
    Player.SetGText(TPlayer.GetName(p))                 # 0x55752104
    if p.index != 0:
        Race.SetGText(TranslateRStr(RaceNamesPluralRStr[p.race]))  # +0xa5
    Turn.SetGText(DayRStr + IntToStr(aowhsmap+0x174))    # "Day N"
    RShield.SetCurrent((p.race == -1 ? 0xc : p.race) + 0xd)
FUN_00423770 (FaceFr.OnDraw):
    if aowhsmap+0xa5 >= 0:                              # a player is seated
        rect = FaceFr's box inset by 3 px                # (x+3, y+3, w-6, h-6)
        face = TPlayer.GetFace()                        # 0x5575235c
        if face: face.Draw(rect)

TPlayer.GetFace (0x5575235c): p+0xd4 (TLeader) -> TLeader's vtable+0xf4
THero.GetFace   (0x557887dc): TFaceResourceList[AoWHSSet+0x74][THero+0x3c]
                              -> TFaceResource.GetFaceImage (0x5574d52c)
```

and the pieces it reads:

* **the name** - `TPlayer.GetName` (`0x55752104`): the independents are
  "Independents"; otherwise the tag-0x1e `TLeader`'s `THero` name (`+0x60`, rw
  tag 10) with the nickname (`+0x64`, tag 11) appended when set; else the
  player's stored name (`+0x30`, tag 0x18), else `"Player N"`;
* **the race** - `AoWE.RaceNamesPluralRStr[player+0xa5]`, the *plural*
  resourcestrings in `AoWEPACK.dpl`'s RT_STRING table: 65256 "Humans", 65258
  "Azracs", 65260 "Lizards", 65262 "Frostlings", 65232 "Elves", 65234
  "Halflings", 65236 "Dwarves", 65238 "Highmen", 65240 "Dark Elves", 65242
  "Orcs", 65244 "Goblins", 65246 "Undead(plural)" (the shipped placeholder);
* **the face** - the leader's *face index* (the tag-0x1e record's rw tag 7 =
  `THero+0x3c`) is a tag of **`Release\FACERES.PFS`** (`AoWE.TFaceResource`,
  class 0x20270), whose records carry the image library (`FACES\L_FACES.ILB` for
  leaders, `H_FACES.ILB` for heroes) and the image id inside it (its tag 6).
  Thinreed Lake's Human seat (Orcs) stores face index 270 ->
  `L_FACES.ILB`'s id 45 = `L-orc01.BMP` (103x128). The face draw is a
  nearest-neighbour stretch of that image into the 46x58 inset rect;
* **the crest** - `RShield`'s `CurrentImage = race + 13` selects one of
  `Int\ShieldsM.ILB`'s ids 13..25 (40x53, class 0x12). The sprite is drawn 1:1
  at `ShieldFr + (6,6)` (the DFM's `awCenter`/`ahCenter` box) plus its own
  `base + shift`, and it is **blended**: the class-0x12 crest is averaged with
  what it covers, so the cell body's hatch shows through it.

Measured against `bin/ProfileLook1.png` (the Thinreed Lake screen playing as the
Orcs = `Scenario\Custom\Thinreed Lake.hsm`, player 1), with the reference's crop
origin at the port render's `+443+220` (the floater is at 440,216):

| region | mean abs delta / 255 |
|--------|----------------------|
| leader portrait (`FaceFr` inset by 3) | **0.83** |
| face cell (frame incl.) | 1.76 |
| race crest (`RShield`) | 6.46 |
| crest placement, best of a +-3 px search | (0,0) |
| name / race text (`InfoFr`) | 6.66 |
| "Day N" (`TurnFr`) | 7.70 |
| whole banner crop (193x77) | 6.89 |

(the residue is the port's bitmap-font rasterisation and the panel body texture,
not the banner's content: the crest's *alignment* fit is exactly (0,0) and the
portrait is pixel-exact).

## 5. Reproducing it in the port (the exact art)

`src/game/menu.cpp` gains a fourth screen (`screen == 3`, "game"), driven by

```powershell
./build.ps1                       # builds port/bin/AoW.exe
./bin/AoW.exe --root ".." --screen game
./bin/AoW.exe --root ".." --screen game --render game.png
./bin/AoW.exe --root ".." --screen game --map "Scenario\Custom\Tutorial.hsm"
./bin/AoW.exe --root ".." --screen game --width 1280 --height 960 --render big.png
```

* the map is loaded and rendered once through `aow::render_map_rgba` (the same
  primitive `aow_view` uses) into an RGBA buffer; the main view and the scanner
  both scale that one render, so a frame is cheap;
* the layout is resolved every frame from the DFM `Alignment.*` blocks through
  `aow_realign` (a direct port of `ReAlignComp`), so the interface **reflows**
  rather than stretches - see §5.2;
* every sprite is the `ILIndex*` entry its DFM names (see sections 2-4), decoded
  through `aow::IlbFile`; the fonts are `FontModule.Age10` / `Age8` /
  `TBarNumber`, loaded the way the Scenario screen loads `Age10s`.

### 5.1 Getting there: Scenario -> select a map -> **Start!**

The in-game screen is reached through the normal UI flow, exactly as the engine
does it:

```
title (orb 0) -> Game Type (Single Computer) -> Scenario -> Start! -> in-game
```

* On the **Scenario** screen the map picker's **Select** (`scen_action -2000`)
  calls `select_map(name, true)` - it fills the info strip and lights the races,
  as `FUN_00411460` does;
* the **Start!** button (`TScenSetup.StartBtnClick` `0x00412568`) calls
  `Scene::start_game`, which loads `Scenario\Custom\<name>` through
  `load_game_map` (the same mapviz render the viewer uses) and sets
  `screen = 3`. If no map is chosen it reports `no scenario map selected` and
  stays put.
* **Esc** on the in-game screen returns to the Scenario screen (the engine opens
  its own game menu there; returning to `TScenSetup` keeps the port navigable).

Inside the in-game screen the recovered controls are live:

| click | action |
|-------|--------|
| bar **World Map** tab (`WMButtonClick`) | toggles the World Map floater (the tab reads "lit" while it is up) |
| bar **Events** tab (`InfoBtnClick`) | toggles the Events floater |
| bar **Realm** / **Magic** / **Relations** | reported; their windows are not ported yet |
| scanner **Surface** / **Caverns** tabs (`ScannerTabClick`) | switch `game_level` and re-render the map at that level |
| scanner **minimize** (`SMinIcon`) | hides the World Map floater |
| **next-turn** arrow (`TBNextButton`) | `TBNextButtonClick`: `Sfx\Int\Btn1.wav` + `TAoWHSMap.SelectNextArmy` - selects the next of your armies that still has move points and is not on guard, or clears the selection (docs/22) |
| **turn** button (`TurnButton`) | `TurnButtonChange` (on mouse-DOWN): ends your turn (or withdraws it), and the server-side loop hands the turn on once every player has ended theirs; the button plays its turn-change animation (docs/22) |
| click on the **world map** | selects the hex under the cursor; the view does **not** move (section 6) |
| click / drag on the **scanner minimap** | centres the camera on the clicked hex (section 6) |
| `C` | centres the camera on the selected hex (the engine's `TSelectedArmy.Center`) |
| `F1` | toggles the frames-per-second read-out in the frame's top-left corner (`FpsMeter`) |
| pointer against a **map edge** | edge-scrolls the view 16 px per update (section 6) |

The whole flow is exercised head-less by `--start-map <file.HSM>`, which runs the
same `select_map(..., true)` + `start_game()` calls and renders the in-game
screen:

```powershell
./bin/AoW.exe --root ".." --start-map "Tutorial.hsm" --render flow.png
# flow=scenario-select-start map=Tutorial.hsm
# game_leader=15,34                     <- the human player's army (lead unit = hero)
# screen=game map=Tutorial.hsm level=0 day=1 disp=640x480 bar=640x48 ...
# game_cam=296,916+416x408 level=0 map=48x48 maxhp=1552x1552 sel=-1,-1 ...
```

Starting a map opens the view **centred on the human player's leader** - what the
engine's `TAoWE.TAoWHSMap.CenterOnLeader` (`0x55778e1c`) does when a map is
opened. `Scene::start_game` arms `game_center_on_start`; the first
`game_update()` then centres on the human player's army whose lead unit is the
hero (via `Scene::game_center_leader`) once the map window's viewport size is
resolved. Unlike the `L` key, the start is **not** selected by default: the leader
is left unselected so the player starts with a clean map. On `Tutorial.hsm` the
human (Halflings) hero sits at (15,34), so the camera lands at `296,916`.

### The `F1` frame-rate read-out

`F1` is a debugging aid that belongs to no screen: `FpsMeter` counts the frames
the window actually **presented** (one per `WM_PAINT`) and averages them over a
one-second window (`kFpsWindowMs`), so the number is the rate the player really
sees, not what the form asked for.

The form's `FrameRate = 30` is real because the frame clock is a 33 ms
**waitable timer** (`start_frame_clock` - a high-resolution waitable timer where
the OS has one, a plain one otherwise) that the message loop waits on next to
its input queue. That matters: the first implementation used the form's own
`SetTimer(33)`, and `WM_TIMER` is serviced off the 15.6 ms system clock, so the
request rounded up to three ticks = 46.9 ms and the whole game ran at **~21 fps**
(measured 47.12 ms over 40 ticks with an empty message loop - no rendering
involved - and `timeBeginPeriod(1)` did not change it, 47.5 ms). The same 33 ms
period on the other APIs is on time on the same box: waitable timer 33.07 ms
(30.2 fps), `timeSetEvent(33, 1)` 33.04 ms (30.3 fps). With the waitable timer
the live title screen and the live Tutorial map both report **30 fps**;
`WM_TIMER` remains the fallback tick for systems where no waitable timer can be
created (the game then still runs, just at the coarse rate). A frame that
overruns the 33 ms budget is not caught up - the clock does not accumulate
ticks - so it lowers the read-out instead.

`draw_fps_overlay` composites the read-out over the finished frame in
`blit_to_window`, whatever screen is up, so it never disturbs a screen's own
composition: a translucent black plate (`kFpsPlateAlpha`) sized from the shipped
face's metrics, and the game's own `FontModule.Age8s` face (`glyphs8`, the
`GameLbl` font every screen already loads) in white - `glyph_ink` keeps the
glyph boxes' palette entry 0 black so the text sits seamlessly on the plate.
The anchor is `kFpsX`/`kFpsY` = (4,4), the frame's top-left corner.

`--fps N` bakes the read-out into a head-less render (`fps=FPS N x=4 y=4` in the
log), which is how the build's smoke test pins the overlay and its placement;
the live capture is `bin/FpsCounterLive.png`, the head-less one
`bin/FpsCounterHeadless.png`.

### 5.2 Resizing: the interface reflows, it does not stretch

Every element is drawn at **its own pixel size** into the box `ReAlignComp`
resolves for the current window, so growing the window (or `--width/--height`)
moves and re-proportions the interface the way the engine does - it never
magnifies the 640x480 composition. Concretely, from the DFM alignment blocks:

* the bar (`TaskWindow`, `WidthPercent 100`, `HeightPercent 0`) spans the new
  width but **keeps its 48 px height**;
* the five task tabs (`awLeft`, `WidthPercent 14`, `LeftOffPercent`
  0/14/28/42/56 on the `awBoth` TaskPanel) **widen and spread** across it;
* the World Map / Events floaters (`WidthPercent 0`) **keep their 200x216 pixel
  size** and only ride the right edge (`awRight`, the events floater also
  `ahBottom` with `BottomOffset 48`);
* the start button, next-turn / turn buttons and the gold / mana read-outs keep
  their art size and pin to their edges;
* all text stays at the game's own bitmap-font size (`Age10` / `Age8` /
  `TBarNumber`);
* the **map viewport is drawn 1:1** at the level's native tile scale - the world
  map has no zoom in AoW1 - and is a *crop* of the level through the engine's
  camera (section 6), clipped to the map window's client box (section 1.1); a
  bigger window simply shows more of the level instead of scaling the tiles up.

Measured (the smoke test pins both):

| window | bar | tab 0 | tab 4 | World Map floater | Events floater | gold |
|--------|-----|-------|-------|-------------------|----------------|------|
| 640x480 | 640x48 | 70x18 @48 | 70x18 @339 | 200x216 @440,0 | 200x216 @440,216 | 69x21 @423,458 |
| 960x720 | 960x48 | 115x18 @48 | 115x18 @518 | 200x216 @760,0 | 200x216 @760,456 | 69x21 @743,698 |
| 1349x749 | 1349x48 | 170x18 @48 | 170x18 @736 | 200x216 @1149,0 | 200x216 @1149,485 | 69x21 @1132,727 |
| 1920x1080 | 1920x48 | 250x18 @48 | 250x18 @1082 | 200x216 @1720,0 | 200x216 @1720,816 | 69x21 @1703,1058 |

`--screen game` also takes the flags shared with the other screens
(`--render`, `--width`, `--height`, `--no-music`, `--no-cursor`, `--mouse`,
`--crop`). The head-less run prints one summary line the build's smoke test
pins:

```
screen=game map=Tutorial.hsm level=0 day=1 disp=640x480 bar=640x48
scan=200x216@440,0 event=200x216@440,216 tab0=70x18@48,439
tab4=70x18@339,439 gold=69x21@423,458 turn=41x38@596,439
map_px=1560x1552 font8_ok=1 fonttb_ok=1
```

## 6. The world-map camera (`THSMapViewer` + `THSMapMouse`)

The map above the bar is not a static picture: it is a **camera** - a rectangle
of the level, in map pixels, moved by the mouse, the keyboard and the scanner.
All of it is `HSEPack.dpl` (`HSEngine`) plus the map window's handlers in
`AoW.exe`; the port transcribes them 1:1 (`Scene::cam_*` / `Scene::game_*`).

### 6.1 The camera state (`HSEngine.THSMapViewer`)

| field | offset | meaning |
|-------|--------|---------|
| `SceneX` / `SceneY` | `+0x30` / `+0x34` | the view's top-left, in **map pixels** |
| `SceneWidth` / `SceneHeight` | `+0x28` / `+0x2c` | the viewport size, in pixels |
| `SceneL` | `+0x38` | the level being viewed |
| `FHSMap` | `+0x48` | the `THSMap` being shown |

The grid is the engine's own, `HSEngine.HXtoHP@0x5560e3b4`:

```
HXtoHP(x, y) = (x*32 + 8, y*32 + (x & 1)*16)      hex -> map pixel
HPtoHX(px, py) = ((px - 8) >> 5, (py - (hx & 1)*16) >> 5)   map pixel -> hex
```

so a level of `w x h` hexes is `32*w + 16` by `32*h + 16` map pixels
(`THSMap.GetMaxXhp@0x5560c3ec` / `GetMaxYhp@0x5560c414`). **There is no zoom**:
the map is always drawn at that native scale.

### 6.2 The clamps (`SetSceneX` / `SetSceneY` / `SetSceneL`)

```
SetSceneX(0x5560edf0): x = max(x, 16)
                       if maxXhp < SceneWidth  + x + 16 -> x = maxXhp - SceneWidth  - 16
SetSceneY(0x5560ee38): y = max(y, 16)
                       if maxYhp - 16 < SceneHeight + y -> y = maxYhp - SceneHeight - 16
SetSceneWidth(0x5560ee80) / SetSceneHeight(0x5560ee98): store, then re-clamp that axis
SetSceneL(0x5560edd0):  l = clamp(l, 0, levelCount - 1)
```

The viewport itself is the **map window's client box**: `AoW.exe 0x00451268`
sizes the scene from the window as `w - 0x18` / `h - 0x18` (the `MapWindow` DFM's
12 px border on each side), `MapWindowPreDrawSurface` shows it at `+12, +12`, and
the hit test (`0x00450de8`) uses the same inset. The replica draws the same box:
the window's 12 px `MapWindow.BMP` frame rings it (section 1.1), so at 640x480 the
window is 440x432 and the camera's viewport - the client box the scene is insetted
to - is 416x408.

### 6.3 Centring on a hex (`CenterScene`)

`THSMapViewer.CenterScene@0x5560ed20` is the single "move the view onto this
hex" entry point:

```
CenterScene(x, y, level):
    (px, py) = HXtoHP(x, y)
    SetSceneX(px + 16 - SceneWidth  / 2)     # the hex's *centre* (HXtoHP + 16)
    SetSceneY(py + 16 - SceneHeight / 2)
    SetSceneL(level)
```

`THSMapViewer.SetHSMap@0x5560eda4` hooks the map's `OnCenterViewTo` to exactly
this function, so everything that moves the view onto a hex funnels through it:

| caller | what triggers it |
|--------|------------------|
| `AoWE.TAoWHSMap.Select@0x55777c5c` (via `THSMap.CenterViewTo@0x5560c178`) | clicking a structure/hero in the Realm / Relations power-source lists |
| `AoWE.TAoWHSMap.CenterOnLeader@0x55778e1c` | the `L` key |
| `AoWE.TSelectedArmy.Center@0x55793c84` | the `C` key |

### 6.4 The mouse (`HSEngine.THSMapMouse` + the map window)

`AoW.exe`'s map window forwards its mouse to `AoWE.TAoWHSMapMouse`, which is a
`THSMapMouse`: `MapWindowMouseDown` (`0x00450e30`), `MouseUp` (`0x00450e88`) and
`MouseMove` (`0x00450ef8`) convert screen pixels to the window's client box and
call the map mouse's `Down` / `Up` / `Move` (`0x5560b838` / `0x5560b8a0` /
`0x5560b8f8`). The map mouse keeps the pointer in client pixels (`+0x8/+0xc`,
with `0x80000000` as its "outside" sentinel) and resolves the hex with
`GetXhx@0x5560b6b0` / `GetYhx@0x5560b718`, which are `HPtoHX` plus a
`[0, w) x [0, h)` bounds test. A mouse-down selects the hex through
`THexagonSpriteSelection.SelectLocation`.

### 6.5 The edge scroll (`AoW.exe 0x00452cc0`)

`MapWindowUpdate` (`FUN_00452e50`) runs every frame and calls `FUN_00452cc0` with
the mouse in client pixels:

```
if mouseX < 2                 -> SetSceneX(SceneX - 16)
if mouseX >= clientWidth - 1  -> SetSceneX(SceneX + 16)
if mouseY < 2                 -> SetSceneY(SceneY - 16)
if mouseY >= clientHeight - 1 -> SetSceneY(SceneY + 16)
```

so the view steps **16 px per frame** while the pointer rests within **2 px** of
an edge. The whole routine is gated on the application being active and on no
modal window being visible (`FUN_00455eac`). The port runs the same body from its
30 fps timer (the form's `FrameRate`), which is what keeps the view scrolling
with a stationary pointer.

### 6.6 The scanner's view rectangle (`AoW.exe 0x00450f8c`)

`MapViewerSceneChanged` pushes the camera into `TScanner` - in *hex* units:

```
SetTWidth (SceneWidth  >> 5)     SetTHeight (SceneHeight >> 5)
SetTLeft  ((SceneX >> 5) - 6)    SetTTop   ((SceneY >> 5) - 6)
```

The `- 6` is the scanner's own map-origin offset (the `Scanner` unit lives in
`AOWTools.dpl`, which is **not** in the Ghidra project, so its consumer could not
be read); in the scanner's drawn coordinates the rectangle sits at
`(SceneX >> 5, SceneY >> 5)`, which is where the replica outlines it.

### 6.7 What moves the camera in the replica

| input | engine | replica |
|-------|--------|---------|
| a map is chosen (Scenario `Start!`) | the map opens centred on the player's leader (`CenterOnLeader`) | `Scene::start_game` arms `game_center_on_start`; the first `game_update` centres on the human player's army whose lead unit is the hero, but leaves it **unselected** |
| pointer within 2 px of a map edge | edge scroll, 16 px/frame | same |
| click on the world map | `THSMapMouse.Down` selects the hex; the game's selection centres **only when it carries an army/structure** (`Select(HS, center)`) | selects the hex **and the army standing on it**; a click on another hex walks the selected army there when this turn's move points cover the path (docs/15_MOVEMENT.md); the view does **not** move |
| click / drag on the scanner | `TScanner` moves its view rect | centres on the clicked hex (`CenterScene`) |
| `C` | `TSelectedArmy.Center` | centres on the selected hex |
| `L` | `CenterOnLeader` | centres on the human player's army whose lead unit is the hero, and selects it |

Head-less, `--game-click X Y` runs one update tick and then clicks (repeatable -
each click is applied in order), `--game-right-click X Y` right-clicks instead
(clearing the selection), `--game-key K` presses one map key, and the run prints
the camera, the selected army and its hovered path so the build's smoke test pins
them. A world-map click selects the hex but leaves the camera where it was:

```
# game_cam=16,16+416x408 level=0 map=48x48 maxhp=1552x1552 sel=-1,-1 hover=0,0 mouse=0,0
# --game-click 400 400  ->  game_cam=16,16+416x408 ... sel=12,12 hover=12,12 mouse=388,388
# --mouse 427 419       ->  game_cam=32,32+416x408 ...          (the edge scroll)
```

(The click is converted from display pixels to the window's client box first -
the 12 px frame subtracted - so (400,400) is client (388,388), hence the selected
hex; the edge scroll's trigger pixel is the client box's last pixel (415,407),
i.e. display (427,419).)

### 6.8 The pointer: the plain arrow and the hover sparkle

The map window paints its own pointer, so the map mouse carries the
`Images\Cursors.ilb` image **id** in `THSMapMouse+0x30` (`GetCursor` /
`SetCursor`, HSEPack.dpl `0x5560b630` / `0x5560b634`) and the **state request**
the map asks for in `+0x34`:

| `+0x34` | meaning | image |
|---------|---------|-------|
| `0` | nothing special | id `0` - the plain arrow |
| `0x101` | the hex under the pointer holds something **selectable** | ids `0x34..0x37`, the animated sparkle |
| `0x100` | one of your armies is selected and you are choosing where to move it | the walk / sail / fly icon (`0x40` / `0x42` / `0x44`) or its blocked twin (`0x41` / `0x43` / `0x45`) - section 6.9 |
| `0x102` / `0x103` | the move target holds an army that is not yours / one that would join it | the crossed swords (`0x31`/`0x32`) / the twin banners (`0x3a..0x3c`) |

`AoWE.TAoWHSMap.MouseMoved` (`0x55777a58`) re-arms the request on every mouse
move. With the map mouse's selection empty (`THSMapMouse+0x28`) and the pointer
on the map, it asks the hexagon selection whether anything on that hex can be
selected - `HSEngine.THexagonSpriteSelection.CanSelectLocation@0x5560bb54`, which
walks the field's sprites *last placed first* and takes the first `CanSelect`
(`+0xf8`) that answers yes - and asks for `0x101` if so, else `0`:

```
if mouse.SelectedSprite == nil and THSMapMouse.GetXhx != -1:
    if not TAoWMapField.GetExplored(hex, seated player): return   # the fog gate
    if AoWHSMap+200 == 0 or field.has_object:                     # "show objects"
        state = 0x101 if CanSelectLocation(...) and not rect-selecting else 0
```

`AoWE.TArmyHS.CanSelect` (`0x55790b20`) -> `AoWE.TUnitHS.CanSelect`
(`0x55784078`) answers yes for an army that is standing (`TUnitHS.Moving` false,
i.e. not mid-step), that is on the hex under the pointer, and that is not
carrying the `TArmy+0x10 & 0x10` flag (concealed). It is deliberately **not** an
ownership test: the game asks for the sparkle over *any* selectable sprite, an
enemy army included - clicking one is how you attack it. `AoWE.TStructure.
CanSelect` (`0x5575e994`) does the same for a structure (a city, a tower, ...)
out of the terrain its record carries. The replica does not parse structures, so
an army is the only hover that arms the sparkle today.

`AoWE.TAoWHSMap.NewFrame` (`0x55777ce8`) then resolves the request into the image
id **every frame** (`AoWE.TAoWHSMap.UpdateCursor@0x55777b20`) - which is what
makes the hover pointer animate:

```
state 0     -> SetCursor(0)
state 0x101 -> SetCursor(0x34 + ((AoWHSMap+0x40 >> 2) & 3))
state 0x102 -> SetCursor(0x31 + ((AoWHSMap+0x40 >> 2) & 1))
state 0x103 -> SetCursor(0x3a + ((AoWHSMap+0x40 >> 2) % 3))
```

so the hover pointer is the four 49x49 `Pointers.BMP` sparkle frames
(`Cursors.ilb` ids `52..55` = `0x34..0x37`), **one frame per four ticks of the
map clock** `AoWHSMap+0x40`, which `HSEngine.THSMap.NewFrame@0x5560d758` bumps by
one per map frame (wrapping at `0x100000`) - the same 30 fps clock the units'
image sequences animate from (docs/16_UNIT_ANIM.md), so a pointer pulse and the
unit playback stay locked together. Whether the resolved id is non-zero is
mirrored into `THSMapMouse+0x3a` ("a custom pointer is up", i.e. the window's own
arrow is not the pointer).

**Placement.** Every pointer in `Cursors.ilb` is a 49x49 (or 56x55) *slot* whose
glyph sits at the entry's `base + shift`, and the engine paints the slot
**centred** on the mouse. The arrow at id 0 carries shift `(23,22)`, which is
exactly what puts its tip on the slot's centre = the hotspot; the sparkle frames
are centred glyphs (`shift == (slot - rle) / 2`: 25x25 at (12,12), 23x23 at
(13,13), ...). A pointer's visual box therefore lands at
`mouse - slot/2 + base + shift`, which is what `Sprite::pointer_dx/pointer_dy`
carries for the sprites decoded out of `Cursors.ilb` (the menu screens keep
their own `(0,0)` anchor).

The replica implements this in the in-game screen: `Scene::game_update` ticks
`game_clock` (`AoWHSMap+0x40`), `Scene::game_resolve_hover` runs the `MouseMoved`
request (`Scene::game_mouse_moved` / `game_can_select_here`),
`Scene::game_update_cursor` resolves the id, and `render_game` paints
`Scene::game_pointer()` at the pointer - the arrow, or the sparkle while the
pointer rests on a hex with an army. The request's fog / "show objects" gates are
not modelled (the replica draws each level unfogged with every object), and the
*selected*-army states are left to a later pass - so where the engine would leave
a stale request (an army selected, or the pointer off the map onto the bar or a
floater) the replica deliberately settles on the plain arrow, since the sprites
those states want are not decoded yet. Head-less:

```
# --game-click 513 141 --game-anim 5 --mouse 220 216
# game_cam=296,916+416x408 ... sel=-1,-1 hover=15,34
# game_cursor=257 id=53 clock=7 selectable=1     <- 0x101, id = 0x34 + ((7 >> 2) & 3)
# --mouse 300 300                                <- empty terrain
# game_cursor=0 id=0 clock=1 selectable=0        <- the plain arrow
```

`build.ps1` pins both, including the `id == 0x34 + ((clock >> 2) & 3)` frame
rule, so the sparkle's timing cannot drift.

### 6.9 Choosing a destination: the move pointer and the blue hexagon

Selecting one of your armies (a click on it) starts the game's **move selection**:
`AoWE.TSelectedArmy.Select@0x55793b70` -> `TMoveSelector.Activate@0x55763e74`,
which points the mouse's capture at the move selector (`THSMapMouse.SetCapture`),
registers it for the mouse-move / down / up event lists, sets the cursor state to
**`0x100`** (the "choose where to walk" icon) and calls
`TAoWE.TAbstractAoWHSMap.ShowHexagonSelector@0x55773268`: the map's `+0xd6` flag
goes up and the **two hexagon-selector sprites** (map `+0xcc`, `+0xd0`) are added
to its scene list, then positioned by `UpdateHexagonSelector@0x55773424` - run
every frame from `TAoWHSMap.NewFrame` while the flag is set - on the mouse's hex
(`+0xd4`/`+0xd5`, 0xFF when the pointer is off the map, which makes both halves
draw nothing). `TMoveSelector.Deactivate@0x55763f04` (run by
`TSelectedArmy.ExecuteMove` when the move starts, and by `Unselect`) takes them
back out again.

#### The marker

Both halves are `Images\General.ILB` entries **6 and 7**, both named `3dHex.BMP` -
`AoWWHSSet+0x48` is `TImageLibraryManager.LinkToIL("GENERAL.ILB")`
(`TAoWHSSet.Create@0x5574b9ec`). They are the two halves of one blue 3D hexagon:

| | art | drawn at |
|---|-----|----------|
| **back** | id 7 - the upper rim, its box `shift` `(1,0)` | `HXtoHP(hex) + (-8, -0x1e)`, so its visual box at hex + `(-7, -30)` |
| **front** | id 6 - the lower rim, `shift` `(0,15)` | the same anchor, box at hex + `(-8, -15)` |

Together they span 63 px of a `48x63` slot: the hexagon (48x32) plus a 30 px
"wall" above it. `ShowBack` / `ShowFront` are the two sprites' own `Show` methods -
they only paint while the mouse's capture is the move selector - and
`UpdateHexagonSelector` gives the front half the scene key *just past the hex's
topmost sprite* while the back half keeps the hex's own key, so the back half
sorts **behind** the units on that hex and the front half **in front of** them.

The art is a **filled** hexagon (a dark blue gradient with a bright rim) whose
surroundings are the magic key colour: class `0x16` is `ImageLib.TSprite16`
(`TSprite16.ClassID@0x552115fc` = `0x16`). The key is the descriptor's u16 right
after the shift fields (`IlbEntry::color_key`; see docs/01_FORMATS.md and
docs/17_BUILDING_ANIM.md section 5), and `decode_rgb` now turns it into real
alpha, because the map canvas is written by a blitter that honours alpha only.
The menu's own `key_to_alpha` / `set_key` border heuristic is kept as a fallback
for an entry without a stored key.

The two halves are painted with a **brighten** blend, not alpha and not opaque:
the engine shows the scene sprites through `GFXE.GenerateBrightenBlendTable`
(the `GFXE.BlendColorKey565@0x5500b3f0` rasteriser indexes that per-channel
table), which makes the result `max(dst, src)` per channel. The dark-blue
gradient fill therefore never darkens the tile - it only *lifts* the channels
where its own pixel is brighter (its blue and the bright rim) - so the marker
becomes the glassy hexagon of `BMRef.png`: the hovered hex's terrain stays fully
visible inside it, with a blue haze where the gradient is bright and glowing
edges, and a unit standing on the hex is genuinely split by it, its lower part
behind the front half. The replica reproduces it with `Scene::brighten_px` in
`mv_paint_selector`.

The replica composites that over the **flat level canvas** (unchanged - it is the
port's verified row-major render): for the marker's own pixels it draws the *far
wall* - brightened - and then puts the hex's contents back over it, so a city, an
object or a unit standing on the hex is drawn at full strength on top of the far
wall (the back half keeps the hex's own scene key, so the engine sorts it behind
those sprites), and finally the *near wall* - brightened - over everything, which
is the `Scene::mv_draw_hex_selector` / `mv_paint_selector` pair. The contents come
from a `contents` layer that mapviz produces on request while it draws the level
(see `render_map_rgba` in `aow/mapviz.hpp`); the terrain layer it also returns
there is local, because only the contents have to be put back over the far wall.
The halves' class-0x16 key is turned into real alpha once, at load
(`key_to_alpha`), so the keyed surroundings drop out. The trail is composited
after both, as the engine draws `ShowMovePaths` after the hexagon-sprite scene,
and the dirty-rect refresh restores the marker's rectangle from the flat canvas,
so a hover stays cheap (`--game-bench`).

One ordering nuance: the contents layer carries the *whole* level's objects and
units, so within the marker's rectangle a hex *below* the hovered one also comes
over the far wall (its scene key is larger - which is what the engine does) while a
hex *above* is covered by the wall (also what the engine does). Only a *neighbour
above* with art reaching down into the marker's rectangle would, strictly, be
drawn behind the wall by the engine and in front of it here; the difference is a
few pixels at the top of the marker.

The unit of the marker's own hex comes out of that contents layer: the in-game
screen's cached level leaves the armies out (`MapVizOptions::no_units`,
`docs/13_UNITS_ON_MAP.md`) and re-composites them per map frame, but the marker's
patch is applied *after* that composite, so `mv_build_contents` is the one level
render that keeps the armies - otherwise the marker hex's own unit (or the army
being ordered to move) would be left behind the far wall.

#### The pointer

`AoWE.TSelectedArmy.UpdateMoveCursor@0x55793440` re-runs on every mouse move while
the army is selected (it returns immediately unless it *is* the mouse's selection)
and resolves the icon:

```
if the selection is empty, the hex is unexplored, or the field has no object:   # "no target"
    state = 0x100 if the move-settings list is a single army else 0
else
    action = TAoWMapField.GetMoveAction(field, selected_army.player)            # a 0x1101 message
    state  = 0x102 if action == 1 -> the crossed swords     (a foreign army)
             0x103 if action == 2 -> the twin banners       (an army that would join)
             0x100 otherwise                                (our own army, or nothing)
if state == 0x100:
    icon = 0x40 if move_types & 1 (walk) else 0x42 if & 4 (water) else 0x44 if & 2 (fly)
    if the move-point cell at the hex's terrain/overlay is non-zero:
        if exploration is off, or the hex is explored: keep `icon`              # allowed
    icon |= 1                                                                   # -> the blocked twin
    SetCursor(icon)
```

`TArmy.CreateMovePointTable@0x5578dc40` seeds that cell with 1 and folds each
unit's costs in (0xFF = impassable), so **a zero never occurs** and the test
reduces to the exploration gate: the blocked twin (`0x41` / `0x43` / `0x45`, the
red "no entry" art) means *exploration is on and this player has not explored the
hex* - which is also why the path search treats such a hex as impassable
(`ExploredOnlyMoveOnMovePointProc@0x5574642c`). Actions `1` / `2` come from
`TArmy.GetMoveAction@0x5578f98c`, and the `2` case needs
`TArmy.GetJoinSelection@0x5578fa74` -> `TAbstractUnit.CanJoin@0x557821ac` (race
alignment + diplomatic relations); the replica models neither, so a foreign army
always answers `1` - what the engine does for an army that cannot join.

The icon is the army's own: `TArmy.MoveTypes@0x5578d9e8` (the union of the units'
move types - the replica's `mv_army.move_types`), tested walk -> water -> fly in
that order (`Scene::game_update_move_cursor`). The `0x102` / `0x103` states are
symbolic and animated by `UpdateCursor` itself (two and three frames).

#### The scenario's starting vision

That exploration gate reads `TAoWMapField.GetExplored@0x55771bf4` - **bit `player`
of the field's explored mask**, the 4-byte `TAoWMapField` tag 4 at `+0x18` (the
replica's `MapTile::explored`). A shipped scenario only carries whatever a saved
game left there - the retail maps ship zeros - so the game *fills the mask in at
map start*: every army/structure explores the hexes within its vision
(`TArmy.UpdateExploration@0x5578e1a8` -> `TAoWMapLevel.ExploreArea@0x5577212c`,
radius `TAbstractUnit.VisibilityRange@0x55780ef8` = **4**, or `2 * the unit's
vision ability + 4`; `TArmy.NewDay` refreshes it every turn). The replica runs
that pass for the seated player's armies when a map loads and again whenever one
of them arrives after a move (`Scene::mv_explore_for_human`, radius 4, always
adding to the mask), which is what makes the walk icon show over the ground around
your army, the blocked twin over the unexplored distance, and the ground around a
destination become explored once you walk there - as in the game. TODO(fog): the
per-step vision *during* a walk, the per-turn refresh of `TArmy.NewDay`, other
players' bits and the fog rendering itself (`TAoWE.TAbstractAoWHSMap.DrawFog@
0x55772eb8`) are a separate pass (section 7), as is the trail search's own
explored-only rule.

Head-less (`--game-click 513 141 --game-click 220 216` selects the hero army at
(15,34); the hex coordinates follow from the camera the scanner click sets):

```
# --mouse 236 200  ->  hover=16,34  game_cursor=256 id=64 ... explored=1 selector=1  (the walk icon)
# --mouse 400 380  ->  hover=21,39  game_cursor=256 id=65 ... explored=0 selector=1  (the blocked twin)
```

`build.ps1` pins both, plus the marker's presence in each case.

## 7. Known deviations / open items

* **Minimap contents.** The `TScanner` draws the level's *explored* terrain over
  a parchment background, with the units/structures the three toggles select.
  The replica has no fog-of-war state, so it shows the whole rendered level
  scaled into the scanner rect (the toggles are drawn but do not filter yet).
  The level it scales is the *cached* one, which no longer carries the army
  sprites (`MapVizOptions::no_units`: the in-game screen composites the armies
  per map frame on top of the map view, `docs/13_UNITS_ON_MAP.md`), so the
  minimap does not show them - the engine draws its own army dots from
  `ArmyCheck`, which is part of the `Scanner.TScanner.Update` scan below.
  TODO(RE): recover `Scanner.TScanner.Update`'s tile/unit scan.
* **Leader portrait.** Done - section 4.1: `FaceFr`'s portrait is the seated
  player's leader face (`TPlayer.GetFace` -> the tag-0x1e `TLeader`'s tag-7 face
  index through `FACERES.PFS`), the name/race labels come from the roster, and
  `RShield` shows `race + 13`'s crest. All four are read from the loaded map.
* **Banner cell body texture.** The `PInfoPnl`/`InfoFr`/`EventFr` bodies keep
  their hatch, but the replica *stretches* `GenericT` 17 over the box where the
  engine tiles the panel's texture, so the hatch's spacing differs from
  `bin/ProfileLook1.png` (the banner's own content matches - section 4.1).
  TODO(RE): `TAOWPanel.Draw`'s texture pass (tile vs stretch and the anchor).
* **Banner title/min/help art.** The `SIcon`/`Min`/`Help` sprites differ from
  the reviewed crop by a pixel or two (they sit at the crop's edge); the labels,
  portrait and crest do not.
* **Interactive state.** The World Map / Events tabs, the scanner's level tabs,
  its minimize button and the bar's two turn buttons are live (section 5.1 and
  docs/22). The Realm / Magic / Relations tabs, the scanner's three view toggles
  and the help buttons are drawn in their resting frames and only reported on
  click, since their windows are not ported yet.
* **Floater sizing.** The engine sizes the floaters from the window manager
  (`MinWidth/MinHeight` 200x216; the events floater is `Resizable`). The replica
  fixes them at 200x216 and, because their `WidthPercent`/`HeightPercent` are 0,
  they keep that size at every window size and only ride their edges - which is
  what the reviewed wide capture shows.
* **Bar art.** `TaskWindow`'s `ILFrame`/`ILTexture` are assigned in code (the
  DFM leaves them empty); the replica uses `Int\TBWin.ILB` BarSet 49/436/155,
  which sums exactly to the 640x48 bar. TODO(RE): confirm from
  `TTBWindow.TBWindowCreate`.
* **Map-window frame.** Done - the `TMWindow.MapWindow` window and its 12 px
  `MapWindow.BMP` frame (GenericF 8..15) are drawn and pinned against
  `bin/CornersRef.png` (section 1.1). The DFM's `BorderWidth`/`BorderHeight` are
  11, but the engine insets the scene / hit-tests / converts the mouse through
  12 px (`w - 0x18`, `+0xc`), which is also the frame art's own thickness, so the
  replica uses 12 throughout.
* **`PInd` pill colours.** `TBWorld.PIndDraw` is not decompiled; the replica
  draws the reviewed count/pitch of `PlyrIndL` pills but not the per-player
  tinting, which follows the loaded scenario's player list.
* **`EventLbl` colour.** The DFM declares `TextColor = clBlack` for
  "Event History" even though it sits on the olive body; the replica keeps the
  declared colour rather than the pale sage the other banner labels use.
* **Gold / mana numbers.** `WGoldPnl`/`WManaPnl` show the seated player's live
  `Player::gold`, `Player::magic.mana` and the **net per-turn** value
  (`Scene::game_live_stats`). The gold net is `income - upkeep` (army + magic),
  the mana net is `magic.mana_income` (already net of the research share). A
  non-negative net draws the DFM's green `+N`; a negative net draws red `-N`.
  The mana total is credited by the recovered `TPlayerMagicControl.NewTurn`
  step in `GameState::begin_player_turn` (day 1 included), so the green number
  is actually added to `Player::magic.mana` every turn. The `140`/`10` and
  `250`/`10` seeds are only the pre-load fallbacks when no roster is seated.
  The economy behind the figures is recovered in `docs/28_ECONOMY.md`.
* **Scanner click.** The `Scanner` unit is in `AOWTools.dpl`, which is **not** in
  the Ghidra project, so `TScanner`'s own mouse handling could not be read. The
  replica's minimap click uses the recovered camera side (the engine's
  `CenterScene`); whether the engine centres or drags the view rectangle there is
  inferred from the `TLeft/TTop` sync formula. TODO(RE): open `AOWTools.dpl` and
  decompile `Scanner.TScanner.CheckMouseDown`.
* **Scanner view rectangle art.** The engine paints the rectangle with the
  scanner's own art; the replica outlines it in 1 px white.
* **Hexagon cursor.** Done for the move selection: the blue 3D hex the game rings
  the hovered hex with is `Images\General.ILB` `3dHex.BMP` (ids 6/7), shown from
  `TMoveSelector.Activate` until Deactivate, and the replica draws both halves in
  the level canvas with the engine's anchor and z-order (section 6.9), brightened so
  the tile shows through. Without a
  move selection the engine draws no such marker - clicking a hex only selects it.
* **The join pointer.** `TArmy.GetMoveAction`'s `2` (an independent army whose
  units would join you) needs `TArmy.GetJoinSelection` ->
  `TAbstractUnit.CanJoin`, which weighs race alignment and diplomatic relations;
  the replica does not model those, so a foreign army always answers `1` and gets
  the crossed-swords pointer instead of the twin banners. TODO(next): lift
  `TAbstractUnit.CanJoin` (0x557821ac) and the relation tables.
* **Fog of war.** The replica has no fog: it renders every level in full and only
  runs the scenario-start exploration pass (section 6.9) for the seated player's
  bit of each field's mask - the one thing the pointer and the move search read.
  A live vision pass (units/structure vision updating as they move,
  `TArmy.UpdateExploration`, `TArmy.NewDay`) and the fog rendering itself
  (`TAoWE.TAbstractAoWHSMap.DrawFog@0x55772eb8`) are the next stage. Until then
  the blocked move icon can sit over terrain the game would have fogged, and the
  trail search does not stop at the explored boundary
  (`ExploredOnlyMoveOnMovePointProc@0x5574642c`).
* **World-map click: selecting and walking an army.** In the engine a map click
  selects the hex under the cursor (`THSMapMouse.Down` ->
  `THexagonSpriteSelection.SelectLocation`) and hands it to `TAoWHSMap.Select`,
  which picks the army standing there; the view moves onto a hex only when the
  game's selection carries an army/structure (`TAoWHSMap.Select(HS, center)`), or
  on the `C`/`L` keys. The replica now matches that, and goes one step further
  (docs/15_MOVEMENT.md):

  * `Scene::load_game_map` keeps the parsed map + `Release.hss` resources and runs
    `Scene::mv_load` (the `MovePointTables`, the roster via
    `aow::load_game_state`, the unit catalogue and the per-level terrain), which
    also records the `PlayerType::Human` player;
  * a click on one of *your* armies selects it (`Scene::mv_select` folds its units
    with `aow::army_move`); a click on another hex walks it there when this turn's
    move points cover the path (`Scene::game_move_selected` relocates the
    `AoWE.TArmyHS`, writes its `army_direction` and re-renders the level), else
    reports `destination unreachable` / `not enough move points this turn`;
  * a click on *another army of yours* while one is selected is a **move onto
    its hex**, exactly the engine's `TMoveArmyTE.CanMoveNext@0x5574830c`
    combine step: when the merged force is eight units or fewer
    (`TArmy.MaxSize@0x5578def4`,
    `TArmy.CanAddUnitSelection@0x5578e55c`) the incoming army walks to it and
    only on arrival does `TMoveArmyTE.FinishMove@0x55747fbc` /
    `TArmy.AddUnitSelection@0x5578e6b4` append its units. The destination army
    therefore stays visible throughout the walk and the Party window follows
    the incoming party until the merge commits; otherwise the destination is
    refused with the game's own "Cannot combine armies" resource string and the
    selected army stays selected;
  * hovering with a selection composites the game's own `Cursors.ilb` arrows / X
    over the level render (`Scene::game_update_path` + `mv_refresh_draw`), so the
    trail previews exactly as the engine's `TMoveSelector.ShowMovePaths` draws it;
  * clicking the selected army again, or right-clicking anywhere on the map
    (`Scene::game_right_click`, the window procedure's `WM_RBUTTONDOWN`),
    clears the selection - the engine removes the party floater on a map
    right-click and closing it deselects the party; the camera still does not
    follow a click - the view is moved by the World Map scanner (which always
    centres) and the `C` key.
* **Walk animation.** The in-game screen walks the moved `TArmyHS`'s sprite hex
  by hex with the unit's own walking sequence and hands over to its standing
  pose (`aow::WalkAnimation`, docs/16_UNIT_ANIM.md), exactly like the viewer.
  The port's feedback for a refused move is `game_status` + a `game_msg=` line
  on stdout (the engine shows a message box, whose art is not recovered).
* **Interactive cost.** The level render and the trail overlay are cheap because
  `src/engine/mapviz.cpp` caches the ILB index/resolver and the decoded libraries
  for the life of the process and the trail is composited with a dirty rectangle;
  `--game-bench N` prints the per-step timings (docs/15_MOVEMENT.md,
  "Interactive cost"). The build's `game performance smoke test` guards it.
* **Camera with a viewport larger than the map.** `SetSceneX/SetSceneY` clamp the
  low edge to 16 first and the high edge second (the engine's order), so a window
  wider than the level's `32*w + 16` px can push `SceneX` below 16. The replica
  reproduces that exactly rather than inventing a third clamp.

## 8. In-game music (the scenario playlist)

The title screen plays `Songs\TITLE.IT`; when a map starts the game **stops it
and plays the map's own playlist**, and on the way back out it returns to the
title theme. The whole thing is the global `Sound.SongPlayer`, a `TSongPlayer`
(`SoundP.dpl`), that `AoW.exe` drives.

### The playlist lives in the map

`AoWE.TAoWHSMap` holds two `Sound.TSonglist` objects (`TAoWHSMap.Create`):

```
TAoWHSMap +0x110  AoWE.TStartupSongList  <- map tag 0x1d   the "original" songs
TAoWHSMap +0x114  Sound.TSonglist        <- map tag 0x3c   the (edited) song list
```

`Sound.TSonglist.ReadWrite` (`SoundP.dpl` 0x5540572c) writes its song list at
object tag 10, which the storage stream serialises as a `u32` byte length
followed by the `TStringList.Text`: the file names joined by CRLF, with a
trailing CRLF (verified on all 72 shipped maps). So in a `.HSM`/`.CSM`:

```
tag 0x1d -> EObject -> tag 10 -> u32 len + "SONGS\CLOUDS.IT\r\nSONGS\REMARCH.IT\r\n"
```

`aow::load_map` reads both lists into `Map::songs_startup` (tag 0x1d) and
`Map::songs_list` (tag 0x3c); the `aow map` CLI prints the effective list.

### The list the game actually plays (`FUN_00454d2c`)

`AoW.exe 0x00454d2c` runs when the in-game screen opens. It clears the song
player's list and refills it, in this order:

1. **tag 0x3c** (`+0x114`) wins when it has entries - copied verbatim
   (`TStrings.Assign`);
2. otherwise **tag 0x1d** (`+0x110`) is copied entry by entry, **skipping any
   song whose title is the "FileNotFound" resource** (i.e. a missing file);
3. if neither yields a song the default **`Songs\Heartwoo.it`** is added.

It then calls `TSongPlayer.Play(0)` and `Sound.SetMusicVolume(registry)`. The
companion `FUN_00454b70` stops the title theme and fades the volume out while
the map loads, and `FUN_0045530c` stops the map music and re-shows the title
screen (`TitleSceneShow`, which plays `Songs\TITLE.IT` if it is not already
playing) on the way out.

`aow::scenario_playlist(map, game_root)` implements exactly this (the names are
relative to the game root, so each is resolved against it).

### How the list is walked (`Sound.TSongPlayer.Update`)

`TSongPlayer.Update` (`SoundP.dpl` 0x55405bf0) is called every sound tick:

```
if (sound active and not paused and the current song has stopped)
    index++
    if (index >= count) index = 0        ; wrap back to the first song
    if (index < count)  Play(index)      ; else stop
```

and `Play` (0x55405d48) decodes the song **once** (no per-song looping). So the
playlist is played in order and repeats from the top after the last song - which
is also why the one-song title list simply loops. `aow::PlaylistPlayer`
(`src/engine/playlist.cpp`) is the port of that player; the Win32 audio thread in
`src/game/menu.cpp` feeds it `Songs\TITLE.IT` on the menus and the map's list
once `Scene::start_game` runs, and swaps back to the title theme on Esc.

`aow_playlist_tests` covers both halves against the shipped data (12 checks: the
tag-0x3c-vs-0x1d choice, the missing-file drop, the default, and the in-order
walk that wraps).

## 9. Functions & forms mined

Decompiled (GhidraMCP, `tools/ghidra/mcp_call.py` / `tools/ghidra/decompile.py`):

| module | address | symbol |
|--------|---------|--------|
| AoWInterface.dpl | 0x00401f2c | `AoWComp.TAoWComponent.ReAlignComp` (the layout formula) |
| AoWInterface.dpl | 0x00402af4 | `AoWWinControl.TAOWWindowControl.ReAlignComp` |
| AoWInterface.dpl | 0x00407654 | `AoWWindow.TAOWWindow.Draw` |
| HSEPack.dpl | 0x5560e3b4 / 0x5560e3d8 | `HSEngine.HXtoHP` / `HPtoHX` (the hex grid) |
| HSEPack.dpl | 0x5560c3ec / 0x5560c414 | `THSMap.GetMaxXhp` / `GetMaxYhp` (the camera's extents) |
| HSEPack.dpl | 0x5560ed20 | `THSMapViewer.CenterScene` (the centring) |
| HSEPack.dpl | 0x5560eda4 | `THSMapViewer.SetHSMap` (hooks `OnCenterViewTo` -> `CenterScene`) |
| HSEPack.dpl | 0x5560edd0 / 0x5560edf0 / 0x5560ee38 / 0x5560ee80 / 0x5560ee98 | `THSMapViewer.SetSceneL` / `SetSceneX` / `SetSceneY` / `SetSceneWidth` / `SetSceneHeight` |
| HSEPack.dpl | 0x5560ecc4 / 0x5560f000 / 0x5560efec | `THSMapViewer.ChangeScene` / `ShowScene` / `ResetStaticSurface` |
| HSEPack.dpl | 0x5560c178 / 0x5560c194 / 0x5560cfe0 | `THSMap.CenterViewTo` / `ChangeViewTo` / `GetViewCenter` |
| HSEPack.dpl | 0x5560d01c / 0x5560d158 | `THSMap.SetViewPort` / `ClearViewPort` |
| HSEPack.dpl | 0x5560b53c / 0x5560b7d0 / 0x5560b838 / 0x5560b8a0 / 0x5560b8f8 | `THSMapMouse.Create` / `MouseDown` / `Down` / `Up` / `Move` |
| HSEPack.dpl | 0x5560b6b0 / 0x5560b718 | `THSMapMouse.GetXhx` / `GetYhx` (pixel -> hex + bounds) |
| AoW.exe | 0x00450db4 / 0x00450e30 / 0x00450e88 / 0x00450ef8 | `TMWindow.MWindowCreate` / `MapWindowMouseDown` / `MouseUp` / `MouseMove` |
| AoW.exe | 0x00450de8 / 0x00450f8c / 0x00451268 | the map window's hit test / `MapViewerSceneChanged` / `MapWindowPreDrawSurface` |
| AoW.exe | 0x00452e50 / 0x00452cc0 / 0x00455eac | the map window's update loop / the edge scroll / its modal-window gate |
| AoWEPACK.dpl | 0x55777c5c | `AoWE.TAoWHSMap.Select` (select + centre) |
| AoWEPACK.dpl | 0x55778e1c / 0x55793c84 | `AoWE.TAoWHSMap.CenterOnLeader` / `TSelectedArmy.Center` |
| AoWEPACK.dpl | 0x55773b08 | `AoWE.TAoWHSMapKeyboard.KeyDown` (the `C` / `L` camera keys) |
| AoWEPACK.dpl | 0x5577448c / 0x557745a8 / 0x557745f4 | `AoWE.TAoWHSMapMouse.Down` / `Up` / `Move` |
| AoWEPACK.dpl | 0x5577225c / 0x557748a4 | `AoWE.TAbstractAoWHSMap.Create` / `TAoWHSMap.Create` |
| AoWEPACK.dpl | 0x5577478c / 0x557747c8 | `AoWE.TStartupSongList.Create` / `OriginalSongsChanged` (the `+0x110` list) |
| AoWEPACK.dpl | 0x557748a4 / 0x55776d60 | `AoWE.TAoWHSMap.Create` (creates the `+0x110`/`+0x114` lists) / `ReadWrite` (tags 0x1d / 0x3c) |
| SoundP.dpl | 0x5540572c / 0x5540568c | `Sound.TSonglist.ReadWrite` (song list at tag 10) / `Create` |
| SoundP.dpl | 0x55405bf0 / 0x55405d48 / 0x55405c5c / 0x55405e2c | `Sound.TSongPlayer.Update` / `Play` / `PlayEx` / `Stop` (the playlist walk) |
| AoW.exe | 0x00454d2c / 0x00454b70 / 0x0045530c | the in-game music setup / the map-load stop+fade / the return-to-title stop |
| AoW.exe | 0x0041a150 | `TitleSceneShow` (plays `Songs\TITLE.IT`) |
| AoW.exe | 0x00423170 | `FUN_00423170` - `TPInfoW`'s update: the banner's name / "of the" / race / "Day N" labels and `RShield`'s `race + 13` (section 4.1) |
| AoW.exe | 0x00423770 / 0x0042383c | `FaceFrDraw` (the leader portrait) / `FaceFrMouseDown` | 
| AoWEPACK.dpl | 0x55752104 / 0x5575235c | `AoWE.TPlayer.GetName` / `TPlayer.GetFace` (the banner's label and portrait) |
| AoWEPACK.dpl | 0x557887dc / 0x5578883c / 0x55788840 | `AoWE.THero.GetFace` / `SetFaceIndex` (`+0x3c`) / `SetHeroResourceIndex` (`+0x44`) |
| AoWEPACK.dpl | 0x5574d608 / 0x5574d52c | `AoWE.TFaceResourceList.GetFaceResource` / `TFaceResource.GetFaceImage` (FACERES.PFS -> the image) |
| AoWEPACK.dpl | 0x5578b470 | `AoWE.TLeader.ReadWrite` (the THero tags + 0x2d/0x2e the tag-0x1e record carries) |

The `TMWindow` handlers are reached through the DFM's published-method names,
which live in the form's RTTI method table: each entry is
`<u16 size><u32 code><u8 len><name>`, so scanning the binary for the name string
gives the code address directly (`tools/ghidra/find_str_va.py`).

Method tables read for the UI classes (`re/funcs_AoWInterface_dpl.txt`,
`re/funcs_aowInt_dpl.txt`): `AoWBaseWin.TAOWBaseWindow.{DrawSurface,DrawSurfDesc,
SetILFrame,SetILTexture}`, `AoWWindow.TAOWWindow.{Draw,SetSize,SetILFrame,
SetILTexture}`, `AoWTabPanel.TAOWTabPanel.{Draw,SetIndex}`,
`AOWMenuBar.TAOWMenuBar.{Draw,SetFStrings}`, `AoWPanel.TAOWPanel`,
`AoWFrame.TAoWFrame`, `AoWImage.TAOWImage`, `AoWLabel.TAOWLabel`,
`AoWButton.TAOWBaseButton`, `AoWListBox.TAOWListBox`, `AoWVScroll.TAOWScrollBar`,
`AoWCheckBox`, `AoWTurnButton`, `Scanner.TScanner`, `AOWWinManager.TAOWWinManager`.

Forms decoded (`tools/dfm_dump.py`): `TTBWINDOW.dfm`, `TSWINDOW.dfm`,
`TPINFOW.dfm`, `TINTGFXMOD.dfm` (the `IntGfxMod` library map), `TMWINDOW.dfm`
(the `TMWindow` map window), `TMAINFORM.dfm` (the 640x480 `TDisplay`).



