# 20 - The Party window (TControlWin), recovered and reproduced

Clicking a party on the World Map opens the game's **Party** floater: the
`ControlWindow` of `TControlWin`, a 200x216 `TAOWWindow` anchored to the right
edge over the map. It lists the eight unit slots of the selected army in two
rows of four - each slot a portrait cell carrying the unit's own sprite, its
hit-point bar and, under it, the "leave behind" pill with the unit's
movement-type icon and its move points - plus the party's move points and
status, and the `PPrev` / `PGuard` / `PMove` / `PNext` button row.

Everything below was recovered with **GhidraMCP** (`tools/ghidra/`) from
`AoW.exe` (image base `0x00400000`; the Ghidra project holds the *original*
`AoW.exe.orig`, whose pooled DFM strings sit at `0x00404xxx`). The form itself
is `assets/dfm/AoW/TCONTROLWIN.dfm` (`re/dfm_all/TCONTROLWIN.txt`), and the
decompiles this document cites are in `re/partywin/`.

## 1. The form's tables

Delphi does not export a form's own methods, so the class's tables had to be
found by hand. They are pooled in the code segment with the form's DFM strings,
and each entry is `<u16><u32 addr>` followed by `<len><name>` - i.e. the
**address precedes its name**.

### 1.1 Published components (their field offsets)

The component table starts at `0x00404944` with `ControlWindow`:
`<len><name><u16 field><u32 ...>`. The recorded field is the published one; the
real object field is `published - 4` (the first component, `ControlWindow`, is
at `+0x44`, which `FUN_0040a344` confirms by handing it to
`TAOWGenericWindow.SetCursor`):

| field | component |
|-------|-----------|
| `+0x44` | `ControlWindow` |
| `+0x48` / `+0x4c` | `CWTitle` / `CWMinIcon` |
| `+0x50`..`+0x6c` | `Unit1Frame`..`Unit8Frame` |
| `+0x70` | `CTitleIcon` |
| `+0x74` / `+0x78` | `MovesLbl` / `PartyStatusLbl` |
| `+0x7c` | `Close` |
| `+0x80`..`+0x9c` | `U1MIcon`..`U8MIcon` |
| `+0xa0` / `+0xa4` | `MoraleMask` / `MoveIcon` |
| `+0xa8`..`+0xb8` | `PartyT1Pnl`, `PartyBPnl`, `PartyT2Pnl`, `PartyT3Pnl`, `PartyT4Pnl` |
| `+0xbc`..`+0xd8` | `Unit1Gge`..`Unit8Gge` |
| `+0xdc`..`+0xf8` | `Unit1Chk`..`Unit8Chk` |
| `+0x13c`..`+0x174` | `PartyIPnl`, `PartyILbl`, `Center`, `Help`, `PPrev`, `PMove`, `PNext`, `PControlPnl`, `PGuard`, `AoWFrame2`, `PartyB2Pbl`, `AoWFrame1`, `Translator`, `PartyActionLbl1`, `PartyActionLbl2` |

`+0x178` is the class's first non-component field - the **travelling selection
mask** (section 3.1). The `Unit1Chk`..`Unit8Chk` block is *not* in slot order
(`Unit4Chk` is declared after `Unit7Chk`), which is why the refresh reads them
as `0xdc, 0xe0, 0xe4, 0xf4, 0xe8, 0xec, 0xf0, 0xf8`.

### 1.2 Published methods (the handlers)

The method table follows at `0x00404dcf`. Its addresses, each verified against
the decompiled body:

| handler | address | what it does |
|---------|---------|--------------|
| `CWMinIconClick` | `0x00405fc4` | `FUN_00455dfc(0)` (the click sound) + hide the window |
| `CloseClick` | `0x00405fe4` | hide the window (which deselects the party) |
| `ControlWindowResize` | `0x00406018` | the window's own drag/resize offsets |
| `Unit1FrameMouseDown` | `0x00406040` | focus the clicked cell's unit (`TUnitList.GetUnit` by the cell's `Tag` -> window data `+0xc0`) |
| `Unit1ChkChange` | `0x004060d8` | rebuild the mask, `TSelectedArmy.SetSelection`, refresh |
| `CheckStart` | `0x00406130` | rebuild the mask from the pills (section 3.1) |
| `CheckEnd` | `0x004062a0` | **the window's refresh** (section 3.2) |
| `Unit1ChkMouseDown` | `0x004066bc` | on the *right* button: invert the mask |
| `CenterClick` | `0x004066f4` | `TAoWHSMap` vtable `+0xbc` (CenterViewTo) + `PlayCenterAnimation` |
| `Unit1FramePreDraw` | `0x0040674c` | the cell's pre-draw -> `FUN_00405d98` |
| `HelpClick` | `0x00406808` | `FUN_0040471c(0x1f7)` - this window's help |
| `PPrevClick` | `0x00406814` | `TAoWHSMap.SelectPrevArmy` |
| `PNextClick` | `0x00406824` | the selected-army list's next entry |
| `PMoveClick` | `0x00406834` | the selected army's move |
| `PGuardChange` | `0x00406848` | `TArmy+0x28` bit 0 - "removes the party from the next-party loop" |

`FUN_00455dfc(party, mode)` is the shared UI click sound (`0` =
`Sfx\Int\Btn1.wav`, `1` = `Btn5`, `2` = `Btn6`, `3` = `Btn3`).

## 2. The pieces the window draws

The DFM's `TIntGfxMod` names the libraries (`assets/dfm/AoW/TINTGFXMOD.dfm`):

| DFM name | file | used for |
|----------|------|----------|
| `IntGfxMod.Units` | `Int\Units.ILB` | the cells and the pills |
| `IntGfxMod.MoveIcons` | `Int\IconMove.ILB` | the movement-type icons |
| `IntGfxMod.UnitBGs` | `Int\UnitBgs.ILB` | the cell wash (section 4.2) |
| `IntGfxMod.GenericF` | `Int\GenericF.ILB` | the floater's 9-slice (ids 0..7) |
| `IntGfxMod.GenericT` | `Int\GenericT.ilb` | the floater body (id 2) |
| `IntGfxMod.GenericI` | `Int\GenericI.ILB` | the title-bar buttons |
| `IntGfxMod.GenericB` | `Int\GenericB.ILB` | the foot buttons |
| `IntGfxMod.TitleIcons` | `Int\WinTIcon.ilb` | `CTitleIcon` (id 4) |

`Int\Units.ILB` groups (all `PartyFr.BMP`):

| ids | shape | use |
|-----|-------|-----|
| `0..7` | 9-slice, `16x16 / 26x16 / 16x26` | the **empty** cell (`Emp{n}Fr`, a `frFilledBox`: the frame over its hatch) |
| `8` | `100x100` | the empty cell's hatch (`RedText.BMP`) |
| `9..11` | 3-piece `16x18 / 32x18 / 16x18` | the **foot panels'** olive-grey pill (`PartyBPnl`, `PartyB2Pbl`, `PControlPnl`) |
| `12..19` | 9-slice | a travelling unit's cell (`Unit{n}Frame`, `frHollowBox`) |
| `20..22` / `23..25` | 3-piece | the pill: **up** (travelling) / **down** (left behind) |
| `26..33` | 9-slice | a left-behind unit's cell (the refresh writes `ILIndexFrame 0x1a`) |

The title-bar buttons are `GenericI` `0/2/1` (close), `3/5/4` (minimise),
`6/8/7` (centre), `59/61/60` (help) - up / lit / down. The foot buttons are
`GenericB` `76` (up) / `82` (lit) / `79` (down), three pieces each.


## 3. The window's own logic

### 3.1 The travelling mask (`TControlWin+0x178`)

The party holds at most `TArmy.MaxSize` = 8 units, and the window's eight slots
are those units in order. Which of them march is one byte:

```
CheckStart  FUN_00406130     sel = 0
  for each slot s:           # a pill is CHECKED while its unit is left behind
      if !check[s].Checked && slot_unit[s] != -1:  sel |= 1 << slot_unit[s]

Unit1ChkChange FUN_004060d8  CheckStart(self);
                             TSelectedArmy.SetSelection(sel);
                             CheckEnd(self);
```

while `CheckEnd` (the refresh) sets every pill's `Checked` from the mask the
engine already holds:

```
sel = TSelectedArmy.GetSelection(map + 0xd8)          # -> form + 0x178
for each slot s:
    check[s].Checked      = !(slot_unit[s] == -1 || (sel >> slot_unit[s]) & 1)
    frame[s].ILIndexFrame = check[s].Checked ? 0x1a : 0x0c
```

*Checked* therefore means "stays behind"; the pill's down art (ids 23..25) and
the red-tinged cell frame (26..33) both follow from that one bit.

### 3.2 `CheckEnd` (FUN_004062a0) - the whole window in one pass

```
sel = TSelectedArmy.GetSelection(hsmap + 0xd8); form + 0x178 = sel
for s in 0..7:  check[s].Checked = ...                 (section 3.1)
for s in 0..7:  frame[s].ILFIndex = checked ? 0x1a : 0x0c
MoveIcon.CurrentImage = FUN_004541b0(TArmy.GetPartyMoves(army, sel))
MovesLbl.Text         = IntToStr(TArmy.MovePoints(army, sel))
PartyStatusLbl.Text   = TranslateRStr(TPartyStatusRStr[
                            TArmy.GetPartyStatus(armyHS + 0x1c, sel)])
```

`FUN_004541b0` is the party icon's frame chooser: the move-type byte's bits are
tested in a fixed order (the walking boot, tree, pickaxe, water, their
combinations, then the wings for flight) and the *IconMove* id it returns is
always even.

### 3.3 The numbers the window shows

| read-out | engine | port |
|----------|--------|------|
| a unit's move points (the pill's caption) | `TUnit.GetMovePoints` (+0x3d, refilled from `Unitres`/`Herores` at turn start) | `aow::unit_move_points(mv_cat, unit)` |
| the party's move points (`MovesLbl`) | `TArmy.MovePoints@0x5578d8d0` = **min over the travelling units**, capped at `0x32`, `0` when none travels | `Scene::party_move_points()` |
| the party's icon | `TArmy.GetPartyMoves@0x5578da60` = the **AND** of the travelling units' move-type bytes, OR'd with `1 << kind` from its little state machine | `Scene::party_moves_byte()` + `party_icon_frame()` |
| the party's status | `TArmy.GetPartyStatus@0x5578f584`: a battle predictor first, then the party's lowest *morale value*: `no unit, or < 61` -> `psStable`, `< 81` -> `psContent`, else `psCheerful` | `Scene::party_status()` (the predictor's own cases answer `psUnruly` / `psUnrest`, which the world map has no equivalent of) |

The status names are the enum's own, in declaration order - dumped from
`TPartyStatus`'s RTTI in `AoWEPACK.dpl`:

```
psUnruly    0   MoraleValueToMorale@0x5575ad64:  value <21     -> 0
psUnrest    1                                    value 21..40  -> 1
psStable    2                                    value 41..60  -> 2
psContent   3                                    value 61..80  -> 3
psCheerful  4                                    value >=81    -> 4
```

`MoraleToMoraleValue@0x5575ace0` maps a class back to its band midpoint
(0->10, 1->30, 2->50, 3->70, 4->90), and
`TAbstractUnit.GetMorale@0x5577f178` is exactly
`MoraleValueToMorale(*(char*)(unit + 0x26))` - so the byte a map file stores
(`TAbstractUnit` rw tag 6) is the unit's **morale value**, and reading the
window needs no in-game morale recomputation. The Tutorial party's values
(50/50/50/90) therefore answer `psStable` - the "Stable" `bin/partyRef.png`
shows.


## 4. Drawing the window 1:1

### 4.1 The layout

Every component's box is the DFM's `Alignment.*` block run through the same
`AoWComp.TAoWComponent.ReAlignComp` replica the rest of the screen uses
(`aow_realign`), so the window keeps its 200x216 design size and only rides the
right edge (`awRight`/`ahTop`, `TopOffset 216`). The interesting rows at that
size:

```
ControlWindow  440,216  200x216     PartyT1Pnl     4,22  192x47  (units 1..4)
CWTitle         25,4 (label)        PartyT2Pnl     4,73  192x18  (their pills)
CTitleIcon      2,2     19x16       PartyT3Pnl     4,95  192x47  (units 5..8)
Close/Min/Ctr/Help   awRight rox 4/18/32/46, y 1, 15x16
                                    PartyT4Pnl     4,147 192x18  (their pills)
Unit{n}Frame    lo/ro +/- 25% per slot, 46x48
                                    PartyBPnl      4,171  96x18
Unit{n}Gge      the same slots, 24x2, BottomOffset 3
                                    PartyB2Pbl   100,171  96x18
Unit{n}Chk      46x18 in T2 / T4    PControlPnl    3,192 194x20
U{n}MIcon       slot% + 3, 32x17
```

`PartyIPnl` (its "Party Action:" labels) is hidden in this window state, and
the empty-slot overlays (`Emp{n}Fr` / `Emp{n}Chk`) sit on top of the cells at
priority 50.

### 4.2 A cell's contents (`FUN_0040674c` -> `FUN_00405d98`)

```
slot  = sender.Tag                      # Unit1Frame Tag 0 .. Unit8Frame Tag 7
unit  = the army's unit `slot_unit[slot]`   # -1 = an empty slot -> nothing extra
rect  = (cell.x + 4, cell.y + 3) .. (cell.x + w - 3, cell.y + h - 7)
wash  = ImageLib.Get(UnitBGs, unit & 0x7f)      # drawn only while < 8 entries
        ShowClipped at the rect's centre (the 100x100 image, 1:1, clipped)
bar   = ImageLib.Get(<the library at IntGfxMod+0x64>, unit & 0x7f)
        ShowClipped centred on the gauge
img   = the unit's own image (vtable +0x100)
        ShowClipped centred in the rect
```

The port pins `UnitBGs` id 1 (the teal-green cell every unit of the shipped
Tutorial party answers); the exact source field of that `& 0x7f` read is still
open (`TODO(RE) TAbstractUnit vtable+0xfc`), and the id can be swept with the
`AOW_PARTY_BG` dev knob.

## 5. The port

`src/game/menu.cpp`:

* `Scene::party_layout()` - the boxes above through `aow_realign`;
  `Scene::party_hit_test()` / `Scene::party_action()` share them with the
  renderer, so a control cannot be drawn and hit-tested at different places.
* `Scene::party_army()` / `party_unit(slot)` / `party_travels(slot)` -
  `TSelectedArmy.GetArmyHS` and the slot -> unit mapping.
* `Scene::party_moves_byte()` / `party_status()` / `party_move_points()` -
  the three read-outs of section 3.3.
* `Scene::party_refresh_portraits()` - the per-slot standing sprite
  (`aow::unit_pose_rgba`, the same call `mapviz` draws the map armies with),
  cached while the selection stands still.
* `Scene::draw_game_party()` - the DFM's declaration order: the floater body +
  frame, the title bar, the foot panels, the cells (frame, wash, hit-point bar,
  portrait, pills), the foot read-outs, the button row.
* The window opens on `mv_select` (with every unit travelling) and closes on
  `mv_reset` (a right-click on the map, or `CloseClick`); `game_update` resolves
  its hover so the title-bar and foot buttons light up, and the window procedure
  gives it the click before the map and the scanner.
* `Scene::game_select_army(+/-1)` backs `PPrevClick` / `PNextClick` (the port
  has one seated player, so it walks that player's armies on the level).
* **Leaving a unit behind**: `game_move_selected` splits the army before it
  walks - the units whose bit is clear in `party_sel` are pushed back onto the
  origin hex as their own `TArmyHS`, which is the "another group" the game forms
  (`game_split=<x>,<y> stay=<n> go=<n>` on stdout).
* **Merging two parties**: clicking another of your armies while a party is
  selected moves it onto that hex and combines the two through the engine's
  combine move (`TMoveArmyTE.FinishMove@0x55747fbc` /
  `TArmy.AddUnitSelection@0x5578e6b4`). Only a merged force of **8 units or
  fewer** is accepted (`TArmy.CanAddUnitSelection@0x5578e55c`,
  `TArmy.MaxSize@0x5578def4`); a ninth unit is refused with the game's own
  "Cannot combine armies" resourcestring. The merge is committed on **arrival**:
  the destination army and the incoming party both stay visible during the
  walk (the window follows the incoming party), and the destination becomes the
  selection with every merged unit travelling only when the walk ends.


## 6. Fidelity - the pixel comparison

`bin/partyRef.png` is the shipped window over the Tutorial map's hero party
(`15,34`: `Trodsen`, a shadow unit, a Centaur and a Halfling Pony Rider). The
port's own frame (`bin/AoW.exe --root . --screen game --no-music --no-cursor
--render bin/PartyWindow.png --game-click 513 141 --game-click 220 216`) is
cropped at `(440,216) 200x216` and compared with the reference by
`tools/refcmp.ps1`:

```
best alignment dx=1 dy=-2   mean|d| = 16.6/255   (6.5 % per channel)
```

`tools/refsbs.ps1` writes the band-by-band side-by-side (`bin/PartyDiff.png`).
The alignment quantifies a known offset of the *capture* itself -
`partyRef.png` is 197x219, not 200x216, so the screenshot's own crop sits 1 px
left and 2 px up - and the per-row profile of the comparison is 0.7/255 on the
empty rows and 2-8/255 on the window body. The residual is dominated by the
four portraits: the cells carry the same units, but the reference caught a
different frame of their idle animation than `unit_pose_rgba`'s `clock = 0`.
Verified 1:1 against the capture: the floater frame and body, the title bar and
its four buttons, the eight cells and their wash, the two pill rows (up / down),
the hit-point bars, the two foot panels, the move-point and status read-outs and
the four foot buttons.

Known deviations / open items:

* the unit-box wash id is pinned to `UnitBGs` 1 (`TODO(RE)` the exact
  `TAbstractUnit` field behind it);
* the portraits use the standing pose of the army's own facing, and the idle
  animation's *frame* may differ from a given capture (see above);
* the port has no damage model, so every hit-point gauge draws full;
* `GetPartyStatus`'s predictor cases (a party facing a fight answers
  `psUnruly` / `psUnrest`) need the engine's combat predictor;
* the pill's per-unit move-point caption is drawn by the port from
  `unit_move_points` (the engine's own setter for that caption was not located -
  the value, the glyphs and the position match the capture);
* the window is fixed to `(disp_w - 200, 216)`; the engine's `ControlWindow` is
  draggable, and its free-drag behaviour is `TODO(RE)`.

## 7. The tests

`build.ps1 -Test` runs a head-less party-window smoke test: it selects the
party, clicks the pill under unit 3 (`game_msg=unit 3 is left behind`), walks
the rest of the party away (`game_split=15,34 stay=1 go=3`), toggles Guard,
pings `PPrev` and closes the window - the same clicks a human would make, with
the window's own id in every log line (`game_click=party <x>,<y> id=<n>`).

