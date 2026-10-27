# 27 - The Change page (`TCityScreen` mode 1) and `TProductionScreen`

Clicking **Change** in the city screen (`CityBtn1`) does not open a second
window: it switches the same `TCityScreen` form from the overview (mode 0) to
its **production picker** (mode 1, `+0x2cc = 1`). That page is the one in
`bin/cityscreen/01.png` .. `06.png`, with the six activity cells at the bottom
(`B1CB` .. `B6CB`): *Produce Merchandise*, *Upgrade*, *Fortify*, *Migrate*,
*Loot* and *Raze*.

The repository also has the separate `TProductionScreen` form
(`assets/dfm/AoW/TPRODUCTIONSCREEN.dfm`). It shares most of its control names
with `TCityScreen`, but the city's Change button never instantiates it; it is a
standalone production page whose `City` (`+0x1e8`) is set by its own caller.
This document covers the mode-1 code path first (what the port implements),
then records `TProductionScreen`'s own tables/behaviour for reference.

Everything below was recovered with **GhidraMCP** (`tools/ghidra/`, port 8089)
from the 32-bit game image (`AoWCompat.exe`, image base `0x00400000`; the
Ghidra project's "AoW.exe" is that 32-bit program - the installed 64-bit
`AoW.exe` is GOG's launcher) and cross-checked against
`re/dfm_all/TCITYSCREEN.txt` / `TPRODUCTIONSCREEN.txt`.

## 1. The class tables

`TCityScreen` published method table (`AoWCompat.exe` file `0x39d45`, 29
entries; `re/dfm_all/TCITYSCREEN.txt` names them):

| handler | address | role |
|---------|---------|------|
| `CityBtn3Click` (Exit) | `0x43d904` | mode 0: hide `Dlg`; mode 1: back to overview |
| `CityBtn1Click` (Change) | `0x43d37c` | mode 0: `FUN_0043bb14(param,1)` + `FUN_0043c414`; mode 1: run the selected activity |
| `ActivityChange` | `0x43d16c` | `FUN_0043c2ec` + `FUN_0043c43c(param, tag+1)` |
| `ActivityPreDraw` | `0x43d30c` | draw the activity icon (see `FUN_0043d290`) |
| `ActivityDoubleClick` | `0x43d1b0` | select the activity then `CityBtn1Click` |
| `PreDrawProductions` | `0x43d950` | draw one `P1CB..P12CB` production icon |
| `ProductionChange` | `0x43d1ec` | `FUN_0043c2ec` + `FUN_0043c290(param, tag)` |
| `ProductionsUpdate` | `0x43d264` | enable the cell when `TUnitProductionList.GetItem(tag)` exists |
| `ProductionDoubleClick` | `0x43d230` | select the cell then `CityBtn1Click` |
| `QueueLBChange` | `0x43dccc` | queue selection -> activity/queue edit mode |
| `MapViewDraw` | `0x43e0d4` | the generated city panorama |
| `MapViewUpdate` | `0x43e33c` | its animation tick |
| `CityScreenCreate` | `0x43ac48` | form create |
| `CityScreenDestroy` | `0x43ac60` | form destroy |
| `PPrevClick` / `PNextClick` | `0x43e420` / `0x43e500` | previous / next city |

`CityBtn1Click` decompiled (`tmp_citydec/dec_0043d37c.c`):

* `mode 0` (`+0x2cc == 0`): `FUN_0043bb14(param,1)` rebuilds the 12 production
  cells from `TCity+0x50` (`TUnitProductionList`, item `+0x1c`), then
  `FUN_0043c414(param)` shows `UpgradePnl`, `InfoPnl` and `ActivityPnl2`, and
  the tutorial fires message `0x39`. The form is now in mode 1.
* `mode 1` (`+0x2cc == 1`): a switch on the selected activity (`+0x2c0`) runs
  the matching `TCity` method: `1` Trade, `2` Upgrade, `3` Fortify, `4`
  Migrate, `5` Loot, `6` Raze (and `7`/`8` the two scroll/sort helpers). On
  success `FUN_0043bb14(param,0)` rebuilds the rows and returns to mode 0.
* `mode 2` (`+0x2cc == 2`): the queue editor - `MainBtn1` deletes the selected
  queue item, `MainBtn2` retries execution.

The six activity cells are `B1CB`..`B6CB` (`Tag 0..5`) inside
`ActivityPnl2` (`+0x148`, 167,357, 307x65). Their `OnPreDraw` calls
`FUN_0043d290(tag+1)` to pick a `CityWin.ILB` frame:

| tag | activity | frame |
|-----|----------|-------|
| 0 | Produce Merchandise | 40 (`Is-Merch`) |
| 1 | Upgrade | 43 |
| 2 | Fortify | 41 (wood) / 42 (stone, when `city+0x4c`) |
| 3 | Migrate | 44 |
| 4 | Loot | 45 |
| 5 | Raze | 46 |

The cell itself is 52x52 at `168 + 51*i`, `y=357..422`; `ActivityPreDraw`
blits the 48x60 card at the cell `+2,+2`.

`ActivityChange` -> `FUN_0043c43c(param, tag+1)` fills the page. The relevant
component offsets (full field table in `re/dfm_all/TCITYSCREEN.txt`, 155
fields; `tmp_city_fields.txt` has the parsed list):

| offset | control | role on the page |
|--------|---------|------------------|
| `+0x104` | `UpgradePnl` | the activity panel (title, picture, memo) |
| `+0x108` | `UpgradeLbl` | title, `AoW15WhiteGrey`, centred at (320, ~266) |
| `+0x10c` / `+0x110` | `UpgradeMemoFr` / `UpgradeMemo` | description, `Age10s` |
| `+0x1d4` / `+0x1d8` | `UpgPicPnl` / `UpgradePic` | the 90x? activity picture |
| `+0x1cc` / `+0x1d0` | `CostFr` / `TurnsFr` | the two right-hand boxes |
| `+0x0a0` / `+0x0a4` | `CostLbl` / `UCost` | "Cost" / value |
| `+0x0ac` / `+0x0b0` | `TurnsLbl` / `UTurns` | "Turns" / value |
| `+0x26c` / `+0x270` | `UpgBtm` / `UpgradeInfo` | relation footer |
| `+0x274` | `RelIcn` | relation crest (hidden for activities 0/3) |
| `+0x0b4..0x0e0` | `P1CB..P12CB` | 12 production cards, 52x52 at `13+51*i`, y=164..216 (`ActivityPnl` WinTop 164 + `P1CB` WinTop 0) |
| `+0x12c` / `+0x150` / `+0x128` | `CityBtn1` / `CityBtn2` / `CityBtn3` | action (or Delete) / Queue (or Execute) / Done |
| `+0x130` | `QueueLB` | 157x63 queue list at (11,358), ItemHeight 11 |
| `+0x2c4` | selected production item | index into `TUnitProductionList` (`TProductionItem+0xc` when queued) |
| `+0x2cc` | mode | 0 overview / 1 picker / 2 queue edit |
| `+0x2c8` | saved queue index | restored by `QueueLBChange` |

The page's six reference captures give the exact titles, descriptions,
cost/turn values and relation lines:

| page | title | action button | queue | cost / turns | relation |
|------|-------|---------------|-------|--------------|----------|
| 01 | Produce Merchandise | Produce Merchandise | yes | - / - | - |
| 02 | Upgrade | Upgrade | yes | 250 / 3 | Human Relation +5 |
| 03 | Fortify | Fortify | yes | - / - | Human Relation +5 |
| 04 | Migrate | (none) | no | - / - | - |
| 05 | Loot | Loot | no | - / - | Human Relation -30 |
| 06 | Raze | Raze | no | - / - | Human Relation -30 |

`Done` is always present. The descriptions (recovered from the captures):

* 01: *When producing merchandise, a city's income increases by 25%.*
* 02: *Upgrades the infrastructure of the town with the technology to buy more
  powerful units.*
* 03: *Improves town defenses. Ground based attackers must breach the wall to
  enter town. First level of fortification is a wooden palisade, the second
  level is a stone wall.*
* 05: *Pillages the town, taking 1 turn per city hex, after which the town is
  turned to ruins. The amount of gold depends on the city size. Looting
  decreases your relation with the population's race.*
* 06: *Destroys the town instantly by burning it to the ground. Razing is
  faster as the Loot option, but doesn't deliver any gold. Razing decreases
  your relation with the population's race.*

Page 04 additionally uses `MigratePnl` (`+0x1fc`): `NewRaceLbl`, the three
`MigrationRaceLB` / `MigrationRelationLB` / `MigrationTurnsLB` lists
(`+0x230/+0x234/+0x238`, headers `T3RaceLbl`/`T3RelLbl`) and the current-race
panel (`CurrentRacePnl`). `MigrationChange` (`0x43dbb4`) fills them.

## 2. The separate `TProductionScreen` form

`TProductionScreen`'s DFM is at `assets/dfm/AoW/TPRODUCTIONSCREEN.dfm`
(`re/dfm_all/TPRODUCTIONSCREEN.txt`, 3041 lines). Its published method table is
in `AoWCompat.exe` at file `0x3e0c8` (20 entries):

| method | address |
|--------|---------|
| `ProductionScreenCreate` | `0x43eef0` |
| `ProductionScreenDestroy` | `0x43ef08` |
| `ActivityChange` | `0x440854` |
| `ActivityDoubleClick` | `0x440898` |
| `ActivityPreDraw` | `0x4408cc` |
| `MainBtn1Click` | `0x440b9c` |
| `MainBtn2Click` | `0x440da8` |
| `MainBtn3Click` | `0x440f2c` |
| `UFaceDraw` | `0x440f60` |
| `QueueLBChange` | `0x441020` |
| `B4CBUpdate` | `0x4411f8` |
| `LoopQueueBoxChange` | `0x441230` |
| `CurrentActionImageDraw` | `0x44124c` |
| `MapViewDraw` | `0x441394` |
| `DlgKeyDown` | `0x441550` |
| `MapViewUpdate` | `0x4415fc` |
| `DlgHide` | `0x441640` |
| `UAbilitiesLBMouseDown` | `0x441648` |
| `PPrevClick` / `PNextClick` | `0x4416e0` / `0x4417cc` |

The helpers immediately before the form (`0x43ef14`, `0x43ef60`, `0x43efa0`,
`0x43f1d8`, `0x43f184`, `0x43f1f4`, `0x43f4a4`, `0x43f50c`, `0x43f534`,
`0x43f574`, `0x43fd4c`) are retained in `re/struct/all/` and in `tmp_proddec/`.
`FUN_0043f1f4` is the show/execute entry: it stores the `TCity` at `+0x1e8`,
calls `FUN_0043f574` (build the 12 production cards) and `FUN_0043fd4c(param,0)`
(mode 0), copies the queue into `QueueLB`, generates the tactical city view
with `TRndTacHsm.RandomMap`, places the garrison
(`OverviewUnit.PlaceOverviewUnit`) and finally shows `Dlg`.
`ProductionScreenCreate` only allocates its `TIntegerList` (`+0x1fc`) that
`FUN_0043ef14` drains when the production cards are rebuilt;
`ProductionScreenDestroy` frees it.

`MainBtn1Click` / `MainBtn2Click` are the same two-mode handlers as the city
form: mode 1 runs `TProductionPlace.CanProduce` / `Produce` (or
`CanResearch`/`Research`, `CanTrade`/`Trade`) and mode 2 deletes/retries the
queue item. `ActivityChange`/`ActivityDoubleClick` call the same
`FUN_0043f4a4` + `FUN_0043ffa8(tag)` pair that fills the activity page. The
form's `ActivityPreDraw` (`0x4408cc`) is the `PreDrawProductions` twin; it
looks up the `TUnitProductionList` item and blits its unit art with the same
`CityWin.ILB` frame-5/4 overlays.

## 3. Port implementation

### 3.1 The production model (recovered from the map)

`City.TCity.ReadWrite@557acbcc` serialises `TCity+0x50` (`TProductionControl`)
with **rw tag 0x12**. `TProductionControl.ReadWrite@5575dc18` then writes:

| tag | field | serialisation |
|-----|-------|---------------|
| `0x14` | `TProductionQueue` | rwEObject; tag 1 is a child list of `TProductionItem` (class `0x20297`) |
| `0x15` | `TUnitProductionList` | rwEObject; tag 1 is a child list of `TUnitProduction` (class `0x20299`) |
| `0x16` | `TProductionSettings` (`+0x24`) | rwEObject; see the field table below |
| `0x17` | production **state** (`+0x20`; 0 idle, 1 waiting, 2 executing) | rwData(1) |
| `0x18` | `LoopQueueBox.Checked` (`+0x18`) | rwBool |

`TUnitProduction.ReadWrite@5575c5b0` streams `+8` as rw tag `0x14` (the
Unitres record id) and `+0xc` as rw tag `0x15`, which is a **rwBool**: the
tag's presence is the value (no payload). `TProductionItem.ReadWrite@5575c6f0`
streams a **queue row's** `+8/+0xc/+0x10` as tags `0x14/0x15/0x16`;
`TProductionItem.GetName` and `TProductionControl.GetProductionName@5575de04`
read them as *kind* (`1` = Produce, `2` = Install/Research) and *index into
the unit list*.

Tag `0x16` is **not** a `TProductionItem`, it is the
`TProductionSettings` object `ExecuteProduction@5575d118` fills.
`TProductionSettings.ReadWrite@5575c8c4` streams (`+0x24` is a nested
produced-unit object, not part of the timer):

| tag | field | type |
|-----|-------|------|
| `0x0a` | `+8` **paid gold** | rwInteger |
| `0x0b` | `+0xc` **paid mana** | rwInteger |
| `0x0c` | `+0x10` **turns left** | rwByte (signed) |
| `0x0d` | `+0x1c` **total turns** | rwByte |
| `0x0e` | `+0x1d` owner player | rwByte |
| `0x0f` | `+0x20` group production id | rwInteger |
| `0x10` | `+0x28` run id | rwInteger |
| `0x11` | `+0x24` produced-unit object | rwEObject |
| `0x12` / `0x13` | `+0x14` / `+0x18` production upkeep | rwInteger |

So a save that is mid-production really carries the live timer: the shipped
`Save/autosave.asg` has Gobton at state 2 with `+8 = 12` gold paid,
`+0x10 = 2` turns left and `+0x1c = 2` total. Reading tag `0x16` as a
`TProductionItem` (tags `0x14/0x15/0x16`) yields all zeroes and made the city
screen show the *total* instead of the remaining turns.

`aow::MapHs` now carries `production_units` / `production_queue` /
`production_current` / `loop_queue`, filled by `parse_production()` in
`src/engine/map.cpp`. Venice (Bloody Marauders) is the reference city in
`bin/QueueRef.png`: its list is `0, 3, 250, 4, 1, 2, 251, 254` (Human Archer,
Swordsman, Battering Ram, Pikeman, Priest, Medium Cavalry, Catapult, Ballista),
the first three have the rw `0x15` flag set (Produce), the remaining five are
Install, and the queue starts empty.

### 3.2 The form's controls

* `Scene::city_change` is `TCityScreen+0x2cc` (mode 0/1/2);
  `Scene::city_queue_edit` is the original's `+0x2cc == 2`.
* `Scene::city_activity` is the checked `B1CB..B6CB` tag; `city_prod_sel` is
  `+0x2c4` (the checked `P1CB..P12CB` card).
* `Scene::city_change_production` is `CityBtn1Click` mode 0: it switches to
  mode 1 and keeps the city's own production list/queue.
* `Scene::city_hit_test` uses the DFM's **actual** DFS WinLeft/WinTop values:
  * `167 + 51*i, 358, 52x64` = `B1CB..B6CB` (`ActivityPnl2` 167,357 + cell
    WinTop 1);
  * `13 + 51*i, 164, 52x52` = `P1CB..P12CB` (`ActivityPnl` 11,164 + P1CB
    WinLeft 2, WinTop 0) - the old `y=203..255` rectangle hit the page text
    below the cards and missed their visible top 39 px;
  * `2001` `CityBtn1`, `2002` `CityBtn2`, `2004` `CityBtn3`;
  * `2010+i` `QueueLB` rows, but only rows that exist in the queue (the
    original's `count <= Selected` guard).
* `Scene::city_activity_select` + `Scene::city_action` switch pages and run
  the action. `CityBtn1Click` mode 1 queues the selected activity
  (`city_action_to_production_id`: Produce Merchandise 10, Upgrade 11,
  Fortify 12, Migrate 13, Loot 14) and returns to the overview; Migrate has no
  `CityBtn1`, and Raze (case 6) is the only immediate action
  (`Scene::city_activity_apply`). The activity's completion is applied by
  `TCityProductionControl.NewTurn`/`ProductionDone`. Done (`CityBtn3Click`
  mode 1) returns to the overview; Escape is `DlgKeyDown` and closes the whole
  modal screen from either mode.
* **Mode 2** (queue editor) follows `QueueLBChange@0x43dccc`:
  * selecting a row saves it in `+0x2c8`, maps `TProductionItem+8` through
    `ProductionIDToCityAction` (`1/2 -> 8/7`, `10..14 -> 1..5`), fills the page
    and enters mode 2;
  * `CityBtn1` becomes **Delete** (deletes the selected row); deleting row 0
    of an executing item cancels it and refunds the paid cost times the turns
    left / total (`ExecuteCancelProduction@5575cd64`);
  * `CityBtn2` becomes **Execute** only for row 0 while the control is
    *waiting* (`TProductionControl+0x20 == 1`, i.e. `CanExecuteProduction`
    refused for lack of gold) and calls `RetryProductionExecution@5575dad8`;
    otherwise it is hidden (`FUN_00441020`);
  * `CityBtn3` stays **Done**.
* `Queue` in mode 1 runs the same `Produce`/`Research`/city action with the
  caller flag 1 (the main button passes 0): it appends the item and leaves the
  page up. It is shown when `GetQueueLocked@5575cd20` is false: the queue is
  not full (five rows) and no queued Migrate/Loot locks it.

### 3.3 Rendering

* `Scene::render_city_change` blits `assets/cityprod1..6.bmp` (one per
  activity page) clipped at y=432, so the live world-map bar below the dialog
  remains visible and unchanged, and draws the live hover/held button skin.
  For any city other than the reference, the clean overview backdrop supplies
  the banner area and the live city name is drawn at the DFM's `StructName`
  anchor.
* The **reference page with no selection** is still an exact capture blit;
  that is what `bin/QueueRef.png` benchmarks. Live overlays are used only when
  the capture cannot carry the state:
  * a non-reference city redraws the 12 cards from `TUnitProductionList`:
    RelFr `42`/`43` as the TAOWCheckBox frame, CityWin `4` (Produce) / `5`
    (Install) / `62` (Is-Empty) at the cell +2,+2 and the unit's standing pose
    over it, plus the 45 % "not installed" wash - geometry, facings and blend
    in §3.3.1;
  * a checked card draws RelFr `43` over the face and changes `CityBtn1` to
    `Produce`/`Install`;
  * a non-empty queue draws the five rows over
    `assets/cityqueue.bmp` (the QueueLB art with the capture's `<empty>` text
    inpainted out by `tools/city_prod_bg.py`) and highlights the mode-2
    selection with Menus `8` + black text;
  * mode 2 redraws Delete/Execute/Done; a hidden button restores the clean
    RightPnl background from the Migrate page (`cityprod4.bmp`).
* **The unit detail page is live** (`FUN_0043c43c` cases 7/8). Checking a
  `P1CB..P12CB` card (or selecting a unit row in the queue editor) draws
  `UnitPnl` over the activity capture's page area:
  * the green page body is `GenericT 2` (MainFloaterSet) tiled from (11,162);
  * `UPicturePnl` (GenericT 18 + FrThinD 105) and `UPictureFramePnl` (GenericT
    19 + FrGoldD 113) frame the unit's face, `TUnit.GetFace` ->
    `TUnitGFXResource.GetFaceImage` (images\Faces\<lib>.ILB at the Unitgfx
    record's face index), stretched into `UFaceDraw`'s 83x103 rect. `UFaceDraw`
    (`0x43dc0c`) resolves the production item of the card checked *at draw
    time* (`TCityScreen+0x2b0` -> city `+0x50` GetItem(`+0x2c4`)) on every
    draw, so the port keys its decoded-face cache on
    `Scene::city_unit_face_id` - the unit id the sprite belongs to. Keying it
    on `city_unit_page.unit_id` kept the first unit's portrait after a second
    card was checked, because that field already names the new card when the
    face is loaded. The face library is a reused `IlbFile`, so its loader also
    resets the directory on every load (`docs/01_FORMATS.md`): switching from a
    `FACES\UNITS.ILB` unit (machines/monsters) to a `FACES\R_FACES.ILB` race
    face used to keep the old library's entries and decode the new file at
    their blob offsets, which sheared the portrait and mixed in slivers of the
    neighbouring pictures;
  * `UStatPnl` (TextOld1 + FrThinD) holds `UStatFr1`/`UStatFr2` (FrGoldD) with
    the six `TAOWImage` cells - `Int\Icons.ILB` 0..4 (sword / shield / star /
    ankh / heart) and `UMovesIcon`'s `IconMove.ILB` frame picked by
    `FUN_004541b0(GetMoveTypes)`;
  * UAttack..UHits are Age10s, `UAlignment`/`ULevel`/the ability
    rows/Cost/Turns labels are Age8, and `UnitLbl`/`UCost`/`UTurns` are
    AoW15WhiteGrey. All Age8 captions use the DFM's CharacterDistance 0
    (`re/dfm_all/TFONTMODULE.txt`) - `text_at(..., tracking = 0)` at the draw
    sites - and the page's TextColor 9415072;
  * `AbilPnl` (TextOld1 + FrThinD) plus the `UAbilitiesLB` cell (FrGoldD) list
    `TAbilityOwner.ListAbilities`' names with ItemHeight 16. The names come from
    the concrete `T*Ability.Create` constructors (`this+8`, RT_STRING captions
    in AoWEPACK.dpl) and the level comes from the unit's tag-25 TAbilityData
    objects (`this+0xc`, written at tag 10). `TTurnUndeadAbility.GetName` /
    `TDispelMagicAbility.GetName` / Marksmanship / Vision append " I".." IV";
    `TTransportAbility.GetName` prints "Transport (N units)". The list body is
    88 px tall with ItemHeight 16, so five rows are visible; its
    `VScrollBar T2SBar` (GenericS 86..93 `NGVsb2.BMP`, hidden while the list
    fits) is drawn for longer lists and the mouse wheel scrolls it one row;
  * Cost/Turns come from `UnitProductionGold`/`UnitProductionTime` for the
    card's kind (1 Produce / 2 Install), Upkeep is
    `TAbstractUnit.GetUpkeep` (`(level+1)*2`, 0 with ability 0x9b), and the
    alignment is `TAlignmentRStr[alignment & 0x7f]`.

  The page's controls are the DFM's ReAlignComp geometry with ActivityPnl at
  (11,162) (the shipped 640x480 frame draws the mode-1 band two pixels above
  the DFM's WinTop; the page texture phase measured on
  `assets/cityprod1.bmp` confirms it). The frame lines of
  `bin/UnitsSelectionRef.png` land exactly on those rects after undoing the
  capture's (3,-1) crop: every icon cell, the six stat values, both footer
  lines, the ability rows and Cost/Turns labels are pixel-identical to the
  capture, and the whole UnitPnl band benchmarks at mean |d| 0.89/255
  (`tools/unit_sel_bench.py`). The residual is the AoW15WhiteGrey ink's edge
  shading (the capture's font antialiasing sits a few levels brighter). The
  bench's *frame* number (6.78) is not a page metric: the reference's city
  panorama is a different `TRndTacHsm.RandomMap` draw and its card row shows
  six installed units, while the shipped Venice list carries eight - both are
  game-state differences, not page graphics.

The artwork is captured for the same reason as the overview backdrop
(`docs/26_CITY_SCREEN.md`): the original renders the page from `CityWin.ILB`
over a runtime-generated tactical city map. `tools/city_prod_bg.py` registers
the six captures onto the 640x480 frame (their crop origins differ by 1..3
pixels, found by matching the static bottom bar) and writes plain 24-bit BMPs,
which `Scene::load` reads with `load_bmp24`.

#### 3.3.1 The production cards (`PreDrawProductions@0x43d950`)

`bin/UnitsORREF.png` is the row's own reference: a 412x70 crop (page origin
`(8,161)`) of the original's Bloody Marauders capital (Venice, hex 32,45,
`Human Archer`..`Ballista`). Every card is three blits plus - on the second kind
- a blended fourth, and the decompiled handler pins all four.

```c
PreDrawProductions(self, cb, info):
  rect := Rect(ActivityPnl.LWin + cb.LWin, ActivityPnl.WinTop + cb.WinTop,
               + cb.Width, + cb.Height)                     // 0x43d98d..0x43d9bc
  item := city.ProdControl.UnitProductionList.GetItem(cb.Tag)
  if item = nil:                                   ImageLib.Get(62).Draw(rect+2,+2)
  else:
    art := city.ProdControl.vmt[0x9c](cb.Tag)      // the item's unit-art object
    if item^[+0xc] = 0:                            // Install
      art[+0xc] := 5                               // 0x43da4f  facing 5
      ImageLib.Get(5).Draw(rect + (2,2))
      art.vmt[0x1a4](rect.Left - 4, rect.Top - 4, @art[+0xc], @info)
      ImageLib.Get(5).ShowBlended(rect + (2,2),
          GFXE.AlphaBlendTable.GetLookupTables(0x2d)) // 45 %
    else:                                          // Produce
      art[+0xc] := 3                               // 0x43d9f5  facing 3
      ImageLib.Get(4).Draw(rect + (2,2))
      art.vmt[0x1a4](rect.Left - 4, rect.Top - 4, @art[+0xc], @info)
```

* **The rect.** `ActivityPnl` is anchored two pixels above the DFM's `WinTop`
  on this page - the same `(11,162)` the unit page measured (§3.3) - so a
  `P1CB..P12CB` cell is `(13 + 51*i, 162)` and the *art* is not clipped to the
  52x52 control: RelFr 42 (52x64) reaches `y = 226` and the 48x60 face
  `y = 224`, which is exactly what the capture shows (rows 53..62 of the
  reference band carry the cards' lower frame). The port used to draw on
  `(13 + 51*i, 164)` clipped to 52 px, which put every card two pixels low and
  cut that frame off.
* **The two faces.** `CityWin 4` (`CityFr`) for a `TUnitProduction+0xc <> 0`
  item and `CityWin 5` for the other kind, both 48x60 with no own offset, so
  they land on `cell + (2,2)`; `CityWin 62` (`Is-Empty`) marks a null item.
  `cw4` and `cw5` differ in 227 pixels (the inner frame's shading), which is
  how the capture tells them apart.
* **The unit art's facing.** The first byte of the draw-info record is the
  facing the art sequence resolves: **3** for Produce, **5** for Install - the
  same 3 `AoWE.TUnitHS.Create@0x55783da4` leaves in `TUnitHS+0x14`, i.e. the
  pose that faces the viewer. The port drew direction 1 for every card, which
  is the away-facing pose: all eight cards showed the units' backs. Matching the
  dumped standing frames against the capture is unambiguous - `HMNAR209`
  (facing 3) lands on the archer at mean |d| 2.5/255 while the other five
  facings stay above 30.
* **The unit art's anchor.** `(rect.Left - 4, rect.Top - 4)` = `cell + (-4,-6)`,
  with the frame's *own* `base_x/xshift` only: `HMNAR209`'s shift is (25,25), so
  its visual box lands on `cell + (21,19)`, which is where the capture has it.
  The sequence's `xoff/yoff` (-16,-24 for the shipped units) are *not* applied
  here - that anchor is the map's (`HXtoHP`), not this page's. The port
  therefore decodes the card art through `aow::unit_pose_sprite_rgba`
  (mapviz) instead of `aow::unit_pose_rgba`.
* **The washed-out kind.** The Install branch draws the frame-5 face a second
  time through `TSprite16.ShowBlended` with the alpha table at index 0x2d.
  `GenerateLookupTables` maps index *i* to table `i*count/100` and
  `GenerateAlphaBlendTable` fills every 5-bit entry with
  `round(src*p + dst*(1-p))`, so this is a **45 %** lerp towards the face -
  where the sprite is not, `src == dst` and nothing changes, so exactly the unit
  art is faded. Fitting `R = (1-a)*face + a*unfaded` over the capture's sprite
  pixels gives a = 0.45 for all five Install cards (the sweep bottoms out at 45
  on cards 3..7 and rises on both sides), i.e. the port's `Canvas::blit(face,
  ..., 45, ...)` reproduces the engine's table.
* **Grading.** `tools/unit_or_bench.py` renders the same page through the live
  card path (`Bloody Marauders.hsm` hex (32,45), a non-reference city name so
  `draw_city_production_cards` runs instead of the capture blit) and scores the
  eight cells against `bin/UnitsORREF.png`:

  ```
  card 0  mean|d| = 0.000/255     card 4  mean|d| = 0.115/255
  card 1  mean|d| = 0.000/255     card 5  mean|d| = 0.248/255
  card 2  mean|d| = 0.000/255     card 6  mean|d| = 0.426/255
  card 3  mean|d| = 0.109/255     card 7  mean|d| = 0.389/255
  worst card 0.426/255 (gate 1.0)  |  whole band 3.264/255
  ```

  The three Produce cards are pixel-identical; the Install cards' remainder is
  the 5-bit lookup-table rounding of the fade. The band figure also covers the
  2 px gutters, which a non-reference city repaints from `city_bg_clean` - a
  capture artifact, which is why the bench gates on the cards.

### 3.4 The production mechanic (queue, produce, install)

This is the part the wiki calls **Install unit** and **Produce unit**:

* Select an *uninstalled* card (`TUnitProduction+0xc == 0`) -> `FUN_0043c43c`
  opens page 7 and `CityBtn1` becomes **Install**. `TCity.Research@557aaf58`
  -> `TProductionPlaceControl.Research` (`+0xf4`) ->
  `TProductionControl.ResearchUnit@5575d870`, which queues a `TProductionItem`
  with **kind 2**.
* Select an *installed* card -> page 8, **Produce**. `TCity.Produce@557aaf18`
  -> `+0xe0` -> `ProduceUnit@5575d764` -> `ProduceUnitEx@5575d670`, kind **1**.
* Both calls append through `ExecuteAddProduction@5575d324`; if the queue now
  holds exactly one row it immediately calls `ExecuteProduction@5575d118`.
  The city override `TCityProductionControl.ExecuteProduction@557a7f0c`
  performs the actual charge/setup (`ExecuteProduction` itself only charges
  `CanProduceUnit`/`CanExecuteProduction`-validated gold/mana and sets the
  timer: `+8` paid gold, `+0xc` paid mana, `+0x10` turns left, `+0x1c` total).
* The cost is the Unitres record's **+0x4c**, i.e. tag 27
  (`TUnitResource.GetObtainValue@55784d58`; `UnitProductionGold` and
  `UnitResearchGold` both return it). A city pays **gold**:
  `TProductionPlaceControl.UnitProductionGold/Mana` route the value to the
  gold slot for `GetType() == 8` (a city) and to the mana slot for the magic
  production places. `CanExecuteProduction@5575cf4c` refuses when the player's
  treasury is short; the row then waits in state 1 and is retried by
  `NewTurn@5575dd00` every turn.
* Timers: install = the unit's level (`UnitResearchTime@5575d7b8`); produce =
  `level - City.GetSize()/2 + 1`, clamped to 1
  (`TCityProductionControl.UnitProductionTime@557a913c`). The other city
  actions (Trade 10, Upgrade 11, Fortify 12, Migrate 13, Loot 14) get 1, 3, 2,
  MigrationTime and `GetSize()` respectively (`557a7f0c`); Upgrade costs 250
  gold and Fortify `size*20` while the wall is below stone.
* `TCity.NewTurn@557ab814` -> `TProductionControl.NewTurn@5575dd00` ticks
  every city the seated player owns at the start of that player's turn
  (`Scene::mv_city_production_new_turn`, called from `game_turn_changed`):
  * state 1 (waiting) with the settings' owner == the seated player: an empty
    queue goes back to state 0, otherwise `ExecuteProduction(GetItem(0))`
    retries the payment;
  * state 2 with the settings' owner == the seated player: **if `+0x10 > 0`
    decrement `+0x10`**, then if `+0x10 == 0` complete the front item
    (`UnitProduced`/`UnitResearched`, the city-action override for kinds
    10..14) and run `ProductionDone` (which starts the next row).
  * `FUN_0043f574` refreshes the overview from that same `+0x10`: a stored
    **-1** hides `ActiveActionTurns`/`ActiveActionTurnsBar`; any other value
    goes through `AoWE.FmtTurnsLeftStr@5575a5c4`, where **only 1** is special
    ("1 turn left") and every other value - including **0** - is
    `IntToStr(n) + " turns left"`. The gauge's max is `+0x1c` and its position
    is `+0x1c - +0x10`.
* Completion:
  * **Install** -> `UnitResearched@5575d774` sets `TUnitProduction+0xc = 1`.
    The card now opens the Produce page and its TAOWCheckBox uses the Produce
    frame. The port logs `city_unit_installed=index=N unit=M`.
  * **Produce** -> `TCityProductionControl.UnitProduced@557a8f10` tries the
    city's covered hexes (the `TTerrainList` entries of its city resource) and
    places the new `TUnit` with `TAbstractUnit.Place@557810fc`: join the
    `TArmyHS` on that hex (`TArmy.AddUnit`, max 8 via `TArmy.MaxSize@5578def4`)
    or create a new army. The port derives the covered hexes with the same
    `hn_walk_hex` spiral and logs
    `city_unit_produced=unit=N at=X,Y army=K` (or `army=new`). If no footprint
    hex can take it (every army full and every hex another owner's) the item
    stays at the head of the queue in state 2 with 0 turns left, exactly like
    `NewTurn`: the next turn retries `UnitProduced` instead of losing the
    unit (the overview label then says "0 turns left").
  * The created `TUnit` copies the Unitres record's **tag 25** ability list
    (`TUnitResource` +0x38, `TUnit.SetUnitResource`). `GetMoveTypes` reads
    ability 0 (walk) / 1 (fly) / 2 (water) from it; a unit without the list
    has move-type 0, `MovePointTables[0]` is all-impassable, and
    `TArmy.CreateMovePointTable` folds that into the whole army - the produced
    unit would freeze every stack it joined. `aow::UnitEntry::ability_mask`
    now carries the decoded list and `city_place_unit_at` copies it.
* `ProductionDone@5575dc7c` removes the finished row and starts the next one.
  With `LoopQueueBox` checked it moves a **loopable** row to the end instead:
  `LoopableProduction@5575ddfc` + the city override allow only kind 1
  (produce unit) and kind 10 (Produce Merchandise); install and the
  Upgrade/Fortify/Migrate/Loot actions do not repeat. The settings object
  keeps its `+8/+0xc/+0x1c` values (an idle city in the shipped autosave still
  carries 250/3); only `+0x10` stays at 0.
* Deleting row 0 of an executing item (`CityBtn1Click` mode 2 ->
  `ExecuteCancelProduction@5575cd64`) refunds
  `ROUND(turns_left * paid_gold / total_turns)` (`CancelProductionReturnGold
  @5575cc90`); later rows have not been paid yet.
* The port keeps the live `TProductionControl+0x18/+0x20/+0x24` fields in
  `aow::MapHs` (`production_state`, `production_turns_*`,
  `production_gold_paid`/`production_mana_paid`), seeded from rw tag 0x16 when
  a save is loaded (`parse_production`), and writes the queue/unit list/loop
  back into the loaded `TCity` object (`Scene::city_store_to_map`), so closing
  and re-opening the city keeps the queue, the installed flags and the running
  timer. `production_turns_left` is the single source of truth: `NewTurn`
  ticks it, `city_sync_settings_mirror` keeps the serialised `+8/+0xc/+0x10`
  triple equal to it, `city_turns_left_label` formats it at draw time and
  `city_refresh_open_from_map` copies it into the open screen after a turn.
  This is in-memory only: the original's save file (`TCity.ReadWrite` tag
  0x12) is not written by the port yet.

`build.ps1 -Test` drives this from the GUI: `Change -> card -> Produce/
Install -> Done -> TurnButton`, then `--game-turns N` plays whole days until
the item finishes (`--game-turns` is a head-less test hook that ends the
human's turn and runs the AI seats). Barondir Bay's capital (40,25) is the
reference: card 0 (Lizardman Archer) is installed (Produce, 15 gold, one turn
after the size-4 city's discount) and card 3 (Giant Slug) is not (Install, 12
gold, one turn). The run also exercises the army cap: a looping Produce fills
the garrison at (40,25) to 8, then the next unit starts a new army on the
next footprint hex.

## 4. Head-less verification

```powershell
# one page (0 = Produce Merchandise ... 5 = Raze)
.\bin\AoW.exe --root .. --start-map "Barondir Bay.HSM" --scen-human 0 `
  --city-screen --structure-name Venice --structure-race-index 0 `
  --structure-status Cheerful --structure-gold 38 --city-level 2 `
  --city-wall "Stone Wall" --city-change --city-activity 0 `
  --render bin/city_prod_1.png --no-music --no-cursor

# all six pages, then compare the 432 px dialog with bin/cityscreen/01..06.png
python tools/city_prod_bench.py --all bin bin/cityscreen
#   01 638x432 mean|d| = 0.000/255 ... 06 634x429 mean|d| = 0.000/255
#   worst mean 0.000/255

# the user-facing QueueRef.png benchmark (page 01 dialog, row 0..431)
python tools/city_queue_bench.py
#   bin/city_prod_1.png vs bin/QueueRef.png: 638x432 dialog
#   mean|d| = 0.0000/255  changed 0/275616  max 0

# the P1CB..P12CB card row, drawn live (Bloody Marauders' human capital)
python tools/unit_or_bench.py
#   card 0..7  mean|d| = 0.000 0.000 0.000 0.109 0.115 0.248 0.426 0.389
#   worst card 0.426/255 (gate 1.0)  |  whole band 3.264/255
#   -> bin/UnitsORREF_sbs.png (reference, port, difference x4)

# the unit detail page: Bloody Marauders' Venice (32,45), Human Swordsman
# (card 1) checked. UnitsSelectionRef.png is a 636x429 crop at (3,-1).
.\bin\AoW.exe --root .. --start-map "Bloody Marauders.HSM" --scen-human 0 `
  --city-screen --structure-tile 32 45 --city-change --city-activity 0 `
  --game-click 90 190 --render bin/unit_selection.png --no-music --no-cursor
#   city_screen=32,45 city=1 name=[Venice ] ...
#   city_production_sel=1 name=[Human Swordsman]
python tools/unit_sel_bench.py bin/unit_selection.png
#   page      594x130 mean|d| = 0.886/255
#   portrait  90x110  mean|d| = 0.154/255   stats 0.209   abilities 0.107
#   cost      90x110  mean|d| = 0.407/255
#   (every icon cell, the six stat values, the footer lines, the ability rows
#    and the Cost/Turns labels are 0.000; the AoW15 ink edges carry the rest)

# live queue path on a real non-reference city: card -> Queue -> row -> Delete
.\bin\AoW.exe --root .. --start-map "Barondir Bay.HSM" --scen-human 0 `
  --city-screen --city-change --city-activity 0 `
  --game-click 38 190 --game-click 550 390 --game-click 60 365 `
  --render bin/city_mode2.png --no-music --no-cursor
#   city_production_sel=0 name=[Goblin Darter]
#   city_queue_add=[Produce Goblin Darter] rows=1
#   city_queue_sel=0 action=8 name=[Produce Goblin Darter] mode=2

# the mechanic end-to-end from the GUI: Change -> card -> Produce -> Done,
# then two whole days. Barondir Bay's capital (40,25) is player 1's city
# (race 2 = Lizardmen), card 0 is installed and card 3 is not.
.\bin\AoW.exe --root .. --start-map "Barondir Bay.HSM" --scen-human 2 `
  --city-screen --structure-tile 40 25 --city-change `
  --game-click 38 190 --game-click 551 368 --game-click 551 405 `
  --game-turns 2 --game-after-click 204 200 --game-after-click 236 216 `
  --render bin/city_produced.png --no-music --no-cursor
#   city_production_sel=0 name=[Lizardman Archer]
#   city_queue_add=[Produce Lizardman Archer] rows=1
#   city_production_start=[Produce Lizardman Archer] cost=15 time=1 gold=235
#   city_unit_produced=unit=54 at=40,25 army=7
#   city_production_advance=[Produce Lizardman Archer] rows=0 state=0
#   game_army=41,25 mp=22 ...      (the army with the produced unit moved)
# (card 3 instead: city_unit_installed=index=3 unit=60)

# the live ActiveActionTurns: the shipped autosave's Gobton (32,21, owner 1)
# is mid-production (TProductionSettings +0x10 = 2, +0x1c = 2, +8 = 12 gold).
# Opening it shows the remaining turns, and every new day of its owner ticks
# the same byte the label reads (`--scen-human 5` = Goblin, race 5).
.\bin\AoW.exe --root .. --start-map "..\..\Save\autosave.asg" --scen-human 5 `
  --city-screen --structure-tile 32 21 --game-turns 2 `
  --render bin/city_turns.png --no-music --no-cursor
#   city_production_load=state=2 turns_left=1 total=2 gold_paid=12 mana_paid=0
#   city_unit_produced=unit=183 at=32,21 army=2
#   city_production_start=[Produce Goblin Spearman] cost=12 time=2 ...
#   city_production_tick=state=2 turns_left=2 total=2 label=[2 turns left]
#   city_production_tick=state=2 turns_left=1 total=2 label=[1 turn left]
# (the load already shows 1 because the owner's turn started before the screen
#  opened; the two --game-turns days then complete the old item and tick the
#  next one from 2 to 1)
```

`--city-change` implies `--city-screen` and `--city-activity N` selects the
page. `build.ps1 -Test` renders all six pages, runs both benches, drives the
`Change -> B3CB (Fortify) -> Done` click path, runs the
`card -> Queue -> row` interaction smoke test, and runs the two
`Produce`/`Install` end-to-end cases above. The first click used to hit
nothing because `P1CB` was tested at y=203..255 instead of 164..216; the Queue
button used to log a string without touching the list, and MainBtn1 used to
only rewrite the overview labels instead of queueing and executing the
production.

The bench crops the port's 640x480 render at each capture's registered origin
and stops at the dialog's bottom edge (y=432), so the comparison is exact even
though the six PNGs are cropped differently (631..638 x 471..477) and the live
world-map bar is drawn below the captured page. The dialog area matches with
0 differing pixels on all six pages.

## 5. Known limits / `TODO(RE)`

* The six pages are captured artwork (like the overview backdrop): the port
  does not yet re-run `TRndTacHsm.RandomMap` for a live city view or re-render
  `CityWin.ILB` page chrome per city.
* The production list/queue **is** parsed from `City.TCity` rw tag 0x12, and
  the full loop (queue -> ExecuteProduction -> NewTurn -> ProductionDone) is
  live: the queue, installed flags, progress and loop flag are written back
  into the loaded `TCity` object, so they survive closing/re-opening the city
  and drive the turn loop. They are still not serialised back to a `.HSM` save
  file (`TCity.ReadWrite` tag 0x12) because the port has no save writer.
* The unit-detail page is drawn live (section 3.3). The ability captions are
  the concrete constructors' RT_STRING texts (`city_ability_name`) and the
  unit's tag-25 TAbilityData levels are decoded (`UnitEntry::ability_levels`);
  a human priest now carries Walking, Magic Strike, Strike, Turn Undead I,
  Healing, Dispel Magic I and Magic Bolts; the five-row list shows the first
  five and its scrollbar/wheel reveals the rest, as the original does. The
  remaining page gap is only the
  AoW15WhiteGrey ink's edge shading, a few levels darker than the capture's
  16-bit run (page mean |d| 0.89/255). Unknown ability ids are skipped, matching
  `ListAbilities`' empty-GetName rule.
* `Migrate`'s race/relation/turns lists are part of page 04; selecting a race
  and sorting (`SortMigrationRaces/Turns/RelationClick`, `MigrationChange`) are
  not wired into the port's model.
* A finished Upgrade raises `TCity+0x4d` but does not re-run
  `TCity.SetUpgradeLevel@557ac75c`'s race-unit sweep, which adds the newly
  unlocked units to `TUnitProductionList` in the original. The cards a shipped
  city already carries (the file's list) are fully usable; the extra cards an
  upgrade would append are not generated yet.
* `ProductionScreen` (the separate form) is not shown by the city path and is
  not implemented as a second window; its recovered method/address table is
  kept in section 2 for the port's production work.
* `ActivityDoubleClick` / `ProductionDoubleClick` (double-click selects and
  immediately runs the action) are not distinguished from a single click by
  the port's Win32 handler yet.

## 6. Code map

* `src/engine/map.cpp` - `parse_production()` / `read_production_item()`
  (City.TCity rw tag 0x12, TProductionControl tags 0x14/0x15/0x16/0x17/0x18;
  tag 0x16 is decoded as TProductionSettings: 0x0a/0x0b/0x0c/0x0d ->
  paid gold/paid mana/turns left/total)
* `src/include/aow/map_types.hpp` - `CityProductionUnit`, `CityQueueItem`,
  `MapHs::production_*` / `loop_queue` / `production_turns_left/total` /
  `production_gold_paid/mana_paid`
* `src/game/menu.cpp`
  * `Scene::city_change`, `city_queue_edit`, `city_activity`, `city_prod_sel`,
    `city_prod_frame[6]`, `city_prod_units`, `city_queue`, `city_card_art`,
    `city_map_hs`, `city_prod_state`, `city_prod_turns_*`
  * `Scene::load_city_production`, `city_store_to_map`,
    `city_sync_settings_mirror`, `city_turns_left_text`,
    `city_turns_left_label`, `city_unit_name`,
    `city_production_item_name`, `city_production_item_progressive`,
    `city_queue_action`, `city_action_to_production_id`
  * `Scene::city_unit_level/gold`, `city_size_hexes`,
    `city_production_time/gold(_for)`, `city_production_loopable`,
    `city_queue_locked`
  * `Scene::city_production_try_start`, `city_queue_add`,
    `city_queue_select/delete/execute`, `city_try_start_for`,
    `city_production_complete`, `city_production_done`,
    `city_refresh_open_from_map`,
    `city_place_unit_at`, `city_produce_unit_at_city`,
    `mv_city_production_new_turn`
  * `Scene::city_change_production`, `city_activity_select`,
    `city_activity_apply`, `city_activity_title`
  * `Scene::city_hit_test`, `city_action`
  * the unit detail page: `Scene::CityUnitPage`, `city_unit_page`,
    `city_unit_page_refresh`, `city_load_unit_face`, `city_ability_name`,
    `city_ability_level_suffix`, `city_unit_abil_top` (UAbilitiesLB scroll
    position), `city_move_icon`, `city_alignment_name`, `city_current_hs`,
    `draw_city_unit_page`, `blit_tex_rect`, `text_w_track`, and the
    `city_page_tex` / `city_panel_tex3` / `city_unit_frame[8]` /
    `city_unit_gold_frame[8]` / `city_unit_icon[5]` / `city_unit_face` (and
    `city_unit_face_id`, the unit id that decoded face belongs to)
  * `Scene::render_city_change`, `draw_city_production_cards` (the card row:
    `kCityCardX/Y/Pitch/Raise`, `kCityCardFaceProduce/Install`,
    `kCityCardUnitDx/Dy`, `kCityCardInstallBlend`),
    `draw_city_queue_live`, `draw_city_change_buttons`, `render_city_screen`
  * `Scene::game_turn_changed` calls `mv_city_production_new_turn`; the
    `--game-turns N` head-less hook drives whole days for the smoke test
  * `Scene::load` - `assets/cityprod1..6.bmp`, `assets/cityqueue.bmp`,
    CityWin 4/5/62/40..46, RelFr 42/43, Menus 8; for the unit page: GenericT
    2/17/18/19, GenericF 105..120, `Int\Icons.ILB` 0..4, `IconMove.ILB` and
    the Unitgfx FACES library
* `tools/city_prod_bg.py` - register the captures, write the BMPs and the
  clean QueueLB box
* `tools/city_prod_bench.py` - the six-page pixel bench
* `tools/city_queue_bench.py` - the QueueRef.png benchmark
* `tools/unit_sel_bench.py` - the UnitsSelectionRef.png unit-page bench
  (frame/page/portrait/stats/abilities/cost regions + `bin/UnitSel_sbs.png`)
* `tools/unit_or_bench.py` - the UnitsORREF.png card-row bench: renders the row
  through the live card path and grades the eight cells + `bin/UnitsORREF_sbs.png`
* `src/include/aow/mapviz.hpp` + `src/engine/mapviz.cpp` -
  `aow::unit_pose_sprite_rgba`, the pose placed with the frame's own
  `base+shift` (no sequence `xoff/yoff`), which is the card row's anchor
* `build.ps1` - six-page renders + benches, the QueueRef check, the unit-page
  bench (including a card-switch render that proves the portrait follows the
  checked card) and the card/Queue/row interaction smoke test
* `bin/cityscreen/01.png` .. `06.png` + `bin/QueueRef.png` - the shipped
  references
* `assets/cityprod1.bmp` .. `cityprod6.bmp`, `assets/cityqueue.bmp` - the
  port's backdrops
