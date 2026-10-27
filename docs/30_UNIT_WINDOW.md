# 30 - The Unit info window (TUnitWindow), recovered and reproduced

Clicking a unit's portrait in the Party floater opens the game's **Unit** window:
the `TUnitWindow` form of `AoW.exe` (`assets/dfm/AoW/TUNITWINDOW.dfm`, dumped in
`re/dfm_all/TUNITWINDOW.txt`). It is a 420x417, modal, masked floater centred on
the display (`UnitWin`, `Priority = 300`, `Modal = True`, `Masked = True`) that
shows the unit's portrait, its combat stats, level/experience, morale, abilities
and description.

Everything below was recovered with **GhidraMCP** (`tools/ghidra/`) from the
original `AoW.exe.orig` (image base `0x00400000`). The decompiles this document
cites are in `re/unitwin/`.

## 1. Where the window is opened

The Party window's `Unit1FrameMouseDown` is `AoW.exe 0x00406040` (the party
published-method table, see `docs/20_PARTY_WINDOW.md`). It:

1. takes the clicked cell's `Tag` (`+0xc` -> the army unit slot),
2. asks the selection's `TUnitList.GetUnit` (`FUN_004026c4`) for that slot's
   unit and stores it in the main form's `+0xc0` (the world-map selection),
3. when the mouse event is not a right-click, calls `FUN_00407db8`.

`FUN_00407db8` is the whole window's refresh: it reads the unit in the main
form's `+0xc0` and fills every control. Opening the window is therefore the same
click that focuses a party cell - there is no separate "show" entry point.

`UnitWinKeyDown 0x0040abd4` hides it on `VK_ESCAPE` and `CloseClick 0x0040a144`
hides it with the shared UI click (`FUN_00455dfc(0)` -> `Sfx\Int\Btn1.wav`).

## 2. The form's tables

`TUnitWindow`'s published-method table sits in the code segment with the DFM
strings, around `0x00406b00`, and uses the same records as `TControlWin`:

```
<u16 field><u32 addr><u8 len><name>
```

The decoded handlers:

| handler | address | what it does |
|---------|---------|--------------|
| `UnitWindowCreate` | `0x00409c60` | `+0x220 = 6` (the initial page/state) |
| `UnitWinUpdate` | `0x00409c68` | the paper-doll animation / item drag timer |
| `UnitWinShow` | `0x0040a10c` | resets timer state on show |
| `UnitTabChange` | `0x0040a0b4` | `UnitTabChange` -> `Gnd1Change` when Info is selected |
| `UnitWinMouseDown` | `0x0040a0c4` | window mouse-down bookkeeping |
| `UnitWinKeyDown` | `0x0040abd4` | `VK_ESCAPE` hides the window |
| `CloseClick` | `0x0040a144` | click sound + hide |
| `DisbandBtnClick` | `0x0040aaa8` | asks "Disband this unit?" (res id 65509) |
| `FrLShPreDraw` | `0x0040a214` | draws the unit's face into the left frame |
| `FrRShPreDraw` | `0x0040a2a0` | draws the unit's standing sprite into the right frame |
| `PreDrawAbilityItem` | `0x00407ba8` | colours an ability row (available/unavailable) |
| `T1AbilityLBMouseDown` | `0x0040ab2c` | selects the shared ability row |
| `PPrevClick` / `PNextClick` | `0x0040abf0` / `0x0040ac9c` | previous/next party unit |
| `UnitWinResize` | `0x00409c3c` | the draggable window's re-layout |

The component table publishes the real field offsets (published = real + 4):

| real | component | real | component |
|------|-----------|------|-----------|
| `0x44` | `UnitWin` | `0x170` | `UnitName` |
| `0x48` | `UnitFrameLeft` | `0x174` | `UnitFrRight` |
| `0x4c` | `UnitTab` | `0x178`..`0x18c` | `UAttackIcon`..`UHitsIcon` |
| `0x50`..`0x5c` | `Close`, `Title`, `Help`, `TitleIcon` | `0x190`..`0x1a4` | `UAttack`..`UHits` |
| `0x60`..`0x68` | `BCloseBtn`, `CeltLineR`, `T2Memo` | `0x1a8`..`0x1b0` | `UStatPnl`, `UStatP1`, `UStatP2` |
| `0x6c`..`0x70` | `ManaIcon`, `Mana` | `0x1b4` / `0x1b8` | `UnitFr` / `Level` |
| `0x74`..`0x98` | `T1AbilityLB`..`T1SrcLB` | `0x1bc`..`0x1c8` | `UnitLine1`, `UnitLine2`, `FrLSh`, `FrRSh` |
| `0x9c`..`0xa4` | `T1ListPnl`, `T1SBar`, `T2SBar` | `0x1cc` / `0x1d0` | `RightFramePnl` / `MoraleLbl` |
| `0xa8`..`0xac` | `T2Text`, `T2Leader` | `0x1d4` / `0x1d8` | `UpkeepIcon` / `UpKeep` |
| `0xb0`..`0xcc` | `Sel4`..`Sel7` (leader spheres) | `0x1dc` / `0x1e0` | `BCeltL`, `CeltLineL` |
| `0xd0` / `0xd4` | `AAlignment` / `AAlignmentLbl` | `0x1ec` / `0x1f0` | `RelationIcn` / `RelationV` |
| `0xd8`..`0xe4` | `T2Alignnl`, `AlFr`, `TypeIcn`, `TypeV` | `0x1f4`..`0x200` | `T2RelPnl`, `RelFr`, `RelRing`, `RelationLbl` |
| `0xe8`..`0xf0` | `T2TypePnl`, `TypeFr`, `LdrFr` | `0x204`..`0x214` | `AlignLbl`, `TypeLbl`, `XP`, `PNext`, `PPrev` |


## 3. What `FUN_00407db8` fills in

For a `TUnit` (map class `0x20213`, `MapUnit::resource` is the Unitres record
tag) every value comes from the unit resource joined to the runtime ability
mask; for a `THero`/`TLeader` (`0x20230`/`0x20232`) the map object carries a
Herores index instead.

| control | recovered value |
|---------|-----------------|
| `UnitName` | `TUnit.GetName` -> `Unitres` name + qualifier ("Human Musketeer"); a hero uses `THero.GetName` + nickname |
| `Level` | `"Level " + GetLevel` (resourcestring 65507) |
| `XP` | `"Exp (" + GetExperience + "/" + GetNextLevelExperience + ")"` (65508) |
| `TypeV` | `Unit` / `Creature` / `Machine` / `Hero` / `Leader` (65533/65531/65532/65535/65534) |
| `TypeIcn` | `UnitWin 23` for a unit/hero/leader, `21` Creature, `22` Machine |
| `AAlignmentLbl` | `TAlignmentRStr[GetAlignment & 0x7f]` (Pure Good .. None) |
| `AAlignment` | `RelWin 31` .. `35` (`0 -> 31, 1 -> 32, 2/3 -> 33, 4 -> 34, 5 -> 35`; hidden for None) |
| `RelationV` | `TStatusRStr[TRace.GetPlayerRelation(...)]` (Hostile/Wary/Polite/Friendly) |
| `RelationIcn` | `RelWin 11` + relation (the four status faces 11..14) |
| `MoraleLbl` | `"Morale: %s"` with `TMoraleRStr[GetMorale]` = Terrible/Poor/Okay/Good/High; hidden for Machines |
| `UAttack`..`UHits` | `GetAttack`, `GetDefense`, `GetDamage`, `GetResistance`, `GetMoves/GetMovePoints`, `GetHits/GetHitPoints` |
| `UMovesIcon` | `GetMoveTypes` through the engine's movement-icon map |
| `UpKeep` / `UpkeepIcon` | `GetUpkeep` (`(level+1)*2`, 0 with ability `0x9b`) with `TBWin 51`; `GetManaUpkeep` > 0 uses `TBWin 52` |
| `T1AbilityLB/T1TypeLB/T1SrcLB` | `TAbstractUnit.ListAbilityInfo` (`0x5577fa50`): name, `TAbilityTypeRStr[GetAbilityType]`, `GetSourceName`; sorted by name (`Engine.SortStringsEx`) |
| `T2Memo` | `TAbstractUnit.GetDescription` as a `TStrings` (Age10s, five visible 15 px rows) |
| left frame | `TAbstractUnit.ShowFace` -> the unit-gfx record's face library/index |
| right frame | `TAbstractUnit.ShowUnit` -> the unit's standing sprite |

The ability types are `TAbilityTypeRStr` (AoWEPACK RT_STRING 65317 Movement,
65318 Combat, 65319 Enchantment, 65320 Secret); source names are `Inherent`
(65209), `Spell` (65107), `Item`, or a leader's name for enchantments.


## 4. The layout

The form is a fixed 420x417 `UnitWin` (`awCenter`/`ahCenter`, `Width/HeightPercent
= 0`), so only its origin follows the display; every child keeps its DFM box.
The port's `Scene::unit_window_layout()` reproduces the design rectangles:

* `UnitWin` 420x417 centred; `GenericF` DialogFr `32..39` over the `GenericT 2`
  body, title icon `WinTIcon 3`, title `"Unit "` at `(42,7)`, Close `GenericI
  0/2/1` at `(385,4)`;
* `UnitMainPnl` `(21,22) 378x143`: `UnitFrameLeft` (`UnitWin 44`) + `FrLSh`
  (`UnitWin 46`) at `(0,0)`, `UnitFrRight` (`UnitWin 45`) + `FrRSh` (`UnitWin
  46`) at `(260,0)`, `UnitLine1/2` (`CeltDeco 1`) at `(118,0)/(118,116)`,
  `UnitName` centred on `+189` at `y +8`, `MoraleLbl` centred at `y +136`,
  `UStatPnl` `(117,28) 144x86` (`GenericF 113` + `GenericT 17`) with the six
  15x17/32x17 `Icons`/`IconMove` icons and the `AoW15WhiteGrey` values, plus
  `TBWin 51/52` upkeep, `Level`/`XP` at `(319,18)/(319,30)`;
* `UnitTab` `(21,163) 379x22`: `CeltDeco 67/68/69` (down) and `70/71/72` (up),
  two pages `Abilities` (65528) / `Info` (65529), Age10;
* `T1Panel`/`T2Panel` `(22,178) 376x201` (`GenericT 2`), `T1ListPnl`
  `(5,15) 366x156` (`GenericF 105` + `GenericT 2`), three `GenericF 113` +
  `GenericT 21` listboxes (name 164 / type 85 / source 118 wide, 17 px rows)
  with `GenericB 85/91/88` headers, and `Menus 8` for the selected row;
* `T2Panel`: `T2Memo` `(5,15) 366x87` (`GenericF 105` + `GenericT 17`), then
  `T2TypePnl` `(5,109) 116x87`, `T2Alignnl` `(126,109) 125x87`, `T2RelPnl`
  `(256,109) 115x87`, each `GenericF 105` + `GenericT 17` with the `UnitWin
  21..23` / `RelFr 31..35` / `RelFr 11..18` art;
* footer `y 380`: `BCeltL` (`CeltDeco 55` tiled) + the two `CeltDeco 3` knots,
  `Disband` and `Close` (`GenericB 19/25/22`, 119 px, Age10).

## 5. The port

`src/game/menu.cpp`:

* `Scene::unit_window_open(slot)` is called by the party cell's click handler
  (`party_action`, the `Unit1FrameMouseDown` case); it resolves the slot and
  calls `unit_window_refresh()`;
* `unit_window_refresh()` is the port's `FUN_00407db8`: name/level/exp/type/
  alignment/relation/morale/stats/upkeep/abilities/description, including the
  runtime ability mask (falling back to the Unitres list when the map omitted
  it), the `ListAbilityInfo` name sort and the "Transport (N units)" caption;
* `unit_face_refresh()` / `unit_sprite_refresh()` cache the face and the 64x64
  standing pose; a hero's face comes from `FACERES.PFS` through the same
  `game_load_face` path as the Events banner, and its stats from the new
  `UnitCatalog::hero_stats` (Herores.pfs tags 15..20);
* `unit_window_layout()` and `unit_window_hit_test()` share the same resolved
  boxes; `unit_window_action()` is the DFM handler table (tabs, Close, Disband,
  ability row);
* `draw_game_unit_window()`/`draw_game_unit_tabs()`/`draw_game_unit_info()`/
  `draw_game_unit_footer()` draw the window after the party and the city screen
  (Priority 300), before the pointer; the unit window is modal, so the party,
  map and scanner take no click or hover while it is up;
* `Escape` closes it (`game_key`), a right-click is swallowed, and `mv_reset`
  hides it with the party selection.

The art is loaded from the game's own libraries in `Scene::load`:
`Int\UnitWin.ILB` (44/45/46, 21..23), `Int\GenericF.ILB` (32..39, 105..112,
113..120), `Int\GenericT.ilb` (2/7/17/21), `Int\CeltDeco.ILB` (1, 3, 55,
67..72), `Int\GenericB.ILB` (19..27, 85..93), `Int\GenericI.ILB` (0..2),
`Int\WinTIcon.ilb` (3), `Int\Icons.ILB` (0..4), `Int\IconMove.ILB` (0),
`Int\TBWin.ILB` (51/52), `Int\Menus.ILB` (8) and `Int\Scenes\RelFr.ILB`
(11..18, 31..35).

### Recovered vs. approximated

* ability **type/source** are a static mapping per ability id (the concrete
  `T*Ability` overrides were not all instantiated); every ability the party
  reference shows (Walking -> Movement, Fire Musket -> Combat) is exact, and
  the source is `Inherent` for unit-resource abilities;
* a party unit's race relation is `Friendly` (the port has no diplomatic
  relation graph yet; the selected party belongs to the local player);
* the Disband click removes the unit and closes the window, but the original's
  "Disband this unit?" confirmation dialog is not drawn yet;
* the Info memo shows five rows (the DFM's visible height) and does not scroll
  yet;
* a hero's `T2Leader` sphere row is not shown (it is hidden in the reference
  capture's non-leader case too).


## 6. The test

`build.ps1 -Test` adds the **game unit-window smoke test**: it selects the
Tutorial party, clicks the fourth portrait (`--game-click 610 245`), clicks the
Info tab (`450 205`) and the title-bar Close (`500 42`), and pins

```
game_unit=open slot=4 name=Halfling Pony Rider
game_click=unit 450,205 id=2
game_click=unit 500,42 id=0
game_unit=close
```

The screenshots used while aligning it are `bin/game_unit_window.png`,
`bin/UnitWindow7.png` (Abilities) and `bin/UnitWindowInfo2.png` (Info).

## 7. The 1:1 pass (GhidraMCP + reference benchmark)

The second pass measured the window against `bin/InfoMenuRef.png` (Abilities)
and `bin/InfoMenuRef2.png` (Info), both 640x480 captures of the Human Musketeer
on `Scenario\Custom\Bloody Marauders.hsm` (slot 2). The harness is
`tools/info_bench.py`; the two renders are made head-less with the new
`--game-prekey L` option (press `L` *before* the clicks so the party exists
when the portrait is clicked):

```powershell
AoW.exe --root <game> --rgb555 --screen game `
        --map Scenario\Custom\Bloody Marauders.hsm --no-music --no-cursor `
        --game-prekey L --game-click 515 245 --render bin\bm_abil.png
# add --game-click 450 205 for the Info tab
python tools/info_bench.py bin/InfoMenuRef.png bin/bm_abil.png
python tools/info_bench.py bin/InfoMenuRef2.png bin/bm_info.png
```

`--rgb555` is essential: the original renders into a 16-bit RGB555 surface,
and without the quantisation the text (`160,169,143`) is compared with the
reference's expanded value (`165,173,140`).

The "before" column is the previous replica's capture (`bin/HM2.png` for
Abilities, `bin/HM3.png` for Info) scored with the same harness:

| capture | before | after (mean \|d\|/255) | exact pixels | ≤8 | ≤16 |
|---------|-------:|------:|------:|------:|------:|
| `InfoMenuRef.png` (Abilities) | 38.57 | **4.62** | **86.50 %** | 91.19 % | 93.06 % |
| `InfoMenuRef2.png` (Info) | 35.80 | **4.09** | **87.59 %** | 92.71 % | 94.85 % |

Per control, Abilities: title 4.45, left frame **1.70**, stat panel 5.98,
right frame **2.63**, tabs 3.44, list 2.68, memo 5.95, cards **0.05**, footer
3.88. Info: title **0.00** (pixel-perfect), left 1.70, stat 6.05, right 2.63,
tabs 3.46, memo 5.67, cards 5.80, footer 3.88.

What the decompiles changed (all citations are `AoW.exe`, `Ilpack.dpl`,
`AoWInterface.dpl`, `aowInt.dpl`, `Gfxepack.dpl`; decompiles under
`re/unitwin`, `re/iface2`, `re/ilbdec`, `re/gfxe`):

* **9-slice geometry uses the descriptor slot, not the cropped visual.**
  `TInterfaceIL.DrawILF` lays the eight frame cells out by the image's slot
  width/height and paints the cropped visual at `cell + base + shift`; the
  DialogFr corner is a 22x22 slot holding an 18x22 bitmap at +4, and the left
  edge is a 22x184 slot holding a 9x184 strip at +13. Using the visual sizes
  moved every frame edge and shifted the title-bar tile by 4 px.
* **Image 46 is a `ShadowBlendTable` vignette, not an opaque white mask.**
  `UnitWin.ILB` class `0x16` is `TSprite16`; its descriptor's `+0x38` 8-byte
  ShowInfo is `0x00000402` = mode 2/subtype 4 for image 46 and
  `0x00000100` (opaque) for 44/45. `GFXE.GenerateShadowBlendTable` is
  `ShadowBlendTable[s][dst] = round(dst * s / 31)`: every non-keyed mask
  pixel (a light-grey dither, 222..255) darkens the destination by 0..13 %.
  Drawing it before frames 44/45 is what creates the reference's portrait
  vignette; skipping it leaves the ring too bright, keying it white was the
  first replica's bug.
* **The right frame's pose.** `FUN_0040a2a0` (`FrRShPreDraw`) calls the unit's
  `ShowUnit` (vtable `+0x1a4` = `TAbstractUnit.Show` -> `ShowEx`) at
  `(FrRSh.Left+0x31, FrRSh.Top+0x4e)`; the pose is the plain
  `unit_pose_sprite_rgba` (frame base+shift only, no map sequence offsets).
  `PPrev`/`PNext` are the 12x9 `CampaignTrans` 23/24 and 25/26 arrows
  (`Int\Scenes\CTrans.ILB`), both six pixels above the first replica's y.
* **Panel textures are tiled from their real origins.** `RightFramePnl`'s
  `ScrollT` (GenericT 7) starts at the panel top-left (281,23), not at the
  FrGoldD corner; `blit_tex_rect` now includes the texture's own base margin.
  `UStatPnl` has no `ILTexture`, so the `UnitMainPnl` body shows through and
  the shipped window draws the 2 px bevel sampled in the capture.
* **`T2Memo`, not `T2Text`, paints the Info page.** `T2Text` is
  `GenericT 17` (TextOld1) with `FrThinD`, but the page is **`T2Memo`**:
  `GenericT 18` (TextOld2) inside `FrGoldD` at `T2Text+1`, with
  `BorderWidth 5`, `BorderHeight 3`, `ItemHeight 15`. Its text starts at
  memo + (8,6) and wraps at **323 px**, the exact interval between the three
  reference line widths (321/316/293) and the next word (327). T2SBar's
  normal bottom arrow is GenericS 96 (the black down arrow), not 97 (lit).
* **Age8's CharacterDistance is 0; the glyph slot is the pen step.** The Age8
  captions (the Level/XP labels, the ability list and all three card captions)
  are drawn with the DFM's `CharacterDistance = 0`
  (`re/dfm_all/TFONTMODULE.txt`), so `TImageLibraryFont` steps by the glyph's
  slot width - e.g. the narrow `I` still advances its full 5 px slot in
  `Inherent`, which is what `bin/InfoMenuRef.png` shows. This screen pins the
  0 at its own `text_at(..., tracking = 0)` call sites; the shared class-0x02
  decode keeps `advance = e.width`. Do **not** implement it by advancing by
  the RLE ink width in `decode_glyph`: that is shared with every 0x02 face and
  it squeezed the Scenario screen's Age8s labels ("CPU SQUIRE",
  "Simultaneous") by a pixel per glyph (see docs/08_SCENARIO_SCREEN.md sec. 12).
* **AoW15WhiteGrey is drawn with its native white/grey antialias.** The stat
  values are blitted with the glyph's palette (`text_at(..., keyed=true)`),
  not recoloured pure white.
* **Tabs / headers / rules / buttons are tiled, not stretched.**
  `TAOWTabPanel` requires a six-image library (67/68/69 down, 70/71/72 up),
  pitch `379/2 = 189` (tab 1 at x=210). `BCeltL` is `DrawILLh` over CeltDeco
  55/56/57 (four 100 px braid tiles). `UnitLine1/2` tiles its 100 px rule
  across the 142 px control. The list headers' 97 px NGHeadLight skin tiles
  across each header, and the foot buttons blit their 170 px middle 1:1 and
  clip it inside the 119 px button.
* **Icons are clipped/placed by slot.** `UAttackIcon`/`UDefenseIcon` sit two
  px right of the DFM box (defence another two down); `UpkeepIcon` is the left
  19 px of the 69x18 `TBWin.BarSet2`; the card icons keep their 37x36 visual
  inside 39/51 px slots. `UMovesIcon` is the remaining oddity (see below).

Known remaining deltas: the Moves boot (the capture shows its motion/shadow
around the 10x11 `IconMove` art, so it is ~4 px wider/taller), the relation
face's exact palette (9.6/255 over its 51x36 cell), the game-state morale word
(the port reads `Okay` for this army while the capture says `High`; the
original's `UpdateMoraleValue` adds the leader/army bonuses at runtime), and
the 1 px crop phase between the two reference captures (the harness searches
it).

