# 07 - The Game Type screen (TMultiWindow), recovered and reproduced

Orb 0 of the title screen ("Scenario") opens the game-type menu. This stage
reproduces that screen - the `SetupBg03` backdrop, the two sliding panels with
their "Game Type" label and Single Computer / TCP Internet / Play by Email
buttons and the Back to Title strip - from the game's own assets and the
geometry recovered out of `AoW.exe`. It is driven by the same `AoW.exe` as
`docs/06_TITLE_SCREEN.md`: click the first orb and the screen slides in.

All addresses below are `AoW.exe` (image base `0x00400000`), found with
GhidraMCP (`tools/ghidra/`) plus the local helpers (`tools/pe_va.py`,
`tools/disasm.py`).

## 1. Where the screen lives

`TMultiWindow` is a Delphi form in `AoW.exe`. Its RTTI sits in the code section
in the standard Borland layout (field table, then method table, then the class
record), which also gives the exact component offsets:

```
VA 0x0042a6fe   field table   0x44 JoinPnl   0x48 GamesLB  0x4c GLBvsc
                              0x50 JoinBtn   0x54 MultiPnl 0x58 BackBtn
                              0x5c GamePanel 0x60 TCPBtn   0x64 IPXBtn
                              0x68 SingleBtn 0x6c EmailBtn 0x70 TCPPnl
                              ...            0xa4 BottomPnl 0xa8 BackButn
                              ...            0xe8 Translator
VA 0x0042a985   method table  25 entries, e.g. CloseBtnClick 0x0042adf8,
                              MultiWinUpdate 0x0042ae54, SingleBtnClick 0x0042b588,
                              TCPBtnClick 0x0042b0c4, IPXBtnClick 0x0042b0ec,
                              EmailBtnClick 0x0042b610
VA 0x0042ab92   class record  TMultiWindow
```

The field records are `[u32 offset][u16 flag][len][name]` (the offset comes
*before* its name); the method records are `[u16 size][u32 address][len][name]`.

The form resource is `assets/dfm/AoW/TMULTIWINDOW.dfm`; `tools/dfm_dump.py`
turns that binary DFM into text (`object TAOWPanel GamePanel ...`), which is how
the layout in section 5 was read.

## 2. Orb 0 -> the Game Type screen

`TTitleScene.WindowMouseMove` (`0x0041ae94`) dispatches the five orbs. Case 0
(the crossed-swords orb) is:

```c
hide(scene->TitleSceneIL);          // the title window's backdrop
FUN_00455dfc(g_songplayer, 0);      // stop the current song
FUN_0042abf0(g_multiwin, 0);        // -> show TMultiWindow
```

`FUN_0042abf0` is the show routine:

```c
play "SONGS\\TITLE.IT";  display.SetSurface(0x280, 0x1E0);   // 640x480
form->0xec = arg;
MultiPnl.Visible = 1;   GamePanel.Visible = 1;                 // 0x54 / 0x5c
TCPPnl.Visible = 0;     LanPnl.Visible   = 0;                  // 0x70 / 0x98
JoinPnl.Visible = 0;    PBEMPnl.Visible  = 0;                  // 0x44 / 0xcc
TCPBtn.Visible = 0;     IPXBtn.Visible   = 0;                  // 0x60 / 0x64
for each connection c:                                         // 0x1bc list
    if c.type == 2 then TCPBtn.Visible = 1;                    // TCP/IP present
    if c.type == 3 then IPXBtn.Visible = 1;                    // IPX present
```

So the visible set depends on the host's networking: on a PC with a TCP stack
but no IPX the screen shows **Single Computer / TCP Internet / Play by Email**
(IPX LAN stays hidden) - exactly the reviewed screen.

## 3. The animation (MultiWinUpdate)

`MultiPnl.OnUpdate = MultiWinUpdate` (`0x0042ae54`) runs every frame. While
`MultiPnl+0x75` is set (the panel is up) it slides the two panels:

```c
target = GamePanel.height;                 // +0x80 = 286
if (target > self->f0) {                   // self->0xf0 = the slide counter
    self->f0 += 10;                        // 10 px per frame
    if (target < self->f0) self->f0 = target;
    GamePanel.SetY(self->f0 - GamePanel.height);        // -286 -> 0
    BottomPnl.SetY(480 - self->f0 + GamePanel.height - BottomPnl.height);
                                                        //  700 -> 414
}
```

i.e. the top panel drops in from above the screen and the bottom strip rises
from below, arriving together after `286/10 = 29` frames (~1 s at the form's
30 fps). The replica keeps the same counter (`Scene::gt_frame`) and the same
formulas (`gt_top_y()` / `gt_bottom_y()`).

## 4. The assets (`Int\Scenes\TitleM.ILB` = `IntGfxMod.TitleScreen`)

The DFM refers to `IntGfxMod.TitleScreen`; its entry ids map straight onto
`Int\Scenes\TitleM.ILB` (and `Int\TitleM.ILB`), which is why the panels match:

| id | name | size | role |
|----|------|------|------|
| 1 | `SetupBg03.BMP` | 640x480 | the MultiPnl backdrop |
| 2 | `GameMenuTop.BMP` | 158x286 | GamePanel (the tall top panel) |
| 3 | `GameMenuBottom.BMP` | 158x66 | BottomPnl (the Back to Title strip) |
| 20 | `TitleIcon.BMP` | 158x24 | `ILIndexUp` - a button at rest |
| 21 | `TitleIcon.BMP` | 158x24 | `ILIndexDown` - pressed |
| 22 | `TitleIcon.BMP` | 158x24 | `ILIndexLit` - hovered/selected |

Text is the game's `FontModule.AoW15WhiteGrey` (`Int\Fonts\Age15WhGr.ILB`), the
same bitmap font as the title captions; the `GameLbl` "Game Type" caption uses
the small `FontModule.Age8s` (`Int\Fonts\Age8s.ILB`). Buttons are `aowInt.dpl`'s
`AoWButton.TAOWBaseButton`; `SetSize` (`0x59810ffc`) sizes a button to its icon
frame (`Get(ILIndexUp).width x height` = 158x24) and `Draw` (`0x59811194`) blits
that frame and centres the caption, choosing the up/lit/down frame from the
hover/down flags.

The two faces store their glyphs differently, and that matters for the caption:

* `AoW15WhiteGrey` is class `0x16` (`TSprite16`) - a mask-less 16-bit bitmap
  whose background is palette entry 0, so its glyphs need the colour key.
* `Age8s` is class `0x02` (`TRLESprite08`) - an RLE rectangle whose escape runs
  are already transparent; every *opaque* pixel is real content. Each glyph
  therefore carries the ink in one flat palette colour (measured as `(0,0,160)`
  in `Age8s.ILB`) **plus** palette entry 0 (black) as the drawn background, and
  `TImageLibraryFont` recolours only the ink - `GameLbl.TextColor` is `8472387`
  (`$00814743`), and the shipped 8-bit UI shows it as the palettised
  `(66,66,132)` read off `bin/GameTypeRef.png`. Applying the `0x16` colour key
  to a `0x02` glyph keys out that ink whenever the glyph's `(0,0)` pixel happens
  to be opaque, which silently erased `m`, `T`, `y` and `p` from "Game Type";
  `Canvas::glyph_ink` now draws the ink in the label colour and keeps the
  background black. `bin/gt_label_fixed.png` is the before/after against
  `bin/GameTypeRef.png`.

### Colour compositing

The panels are translucent in the running game: the `GamePanel` texture is
composited over the whole column (its own buttons included) at roughly 68%. That
is why the game's resting buttons read as a muted navy rather than the vivid
`TitleIcon` blue:

```
panel art (16,20,32) over backdrop (106,85,90)   -> game's (45,39,50)
TitleIcon (65,85,180) under panel 0.68           -> game's (24,33,65)
```

The port reproduces this by drawing the backdrop, then the button frames, then
the panel textures at `kGtPanelAlpha = 68` (`Canvas::blit`'s fade fraction), and
finally the captions at full strength.

## 5. The layout (TMultiWindow DFM, 640x480 coordinates)

The panels' `WinLeft`/`WinTop` are positions inside the 640x480 scene (the form
is a child of the title window; its own Left/Top are not used).

```
MultiPnl      x=0   y=0     640x480   SetupBg03 backdrop
GamePanel     x=241 y=0     158x286   GameMenuTop
  GameLbl     x=+55 y=+20                       "Game Type"   (centred)
  SingleBtn   x=+0  y=+46   158x24              "Single Computer"
  TCPBtn      x=+0  y=+86   158x24              "TCP Internet"
  IPXBtn      x=+0  y=+126  158x24   hidden unless IPX is present
  EmailBtn    x=+0  y=+166  158x24              "Play by Email"
BottomPnl     x=241 y=380   158x66    GameMenuBottom (animated rest: y=414)
  BackButn    x=+0  y=+22   158x24              "Back to Title"  (CloseBtnClick)
```

`241 = (640-158)/2`, so the whole column is centred.

## 6. Running the replica

```powershell
./build.ps1                  # builds port/bin/AoW.exe
./build.ps1 -Viewer          # also copies it next to the game

cd port
./bin/AoW.exe                        # click the first orb for the screen
./bin/AoW.exe --root ".." --screen gtype            # start on it
./bin/AoW.exe --root ".." --screen gtype --gtype-frame 150 --render x.png
```

Flags: `--screen title|gtype` starts on the title screen or the Game Type
screen; `--gtype-frame N` sets the slide counter (0 = top panel fully above the
screen, 286 = resting, as in `MultiWinUpdate`). `--hover N` (0..2 game types,
3 = Back to Title) picks the lit button frame; `--mouse X Y` and `--no-cursor`
behave as on the title screen.

`./build.ps1 -Test` adds a Game Type smoke test that renders the resting frame
and asserts `screen=gtype gt_top=0 gt_bottom=414`, the panel sizes
(`bg=640x480 panel=158x286 bottom=158x66`) and the button frames (`btn=158x24`).

## 7. Known deviations (all deliberate)

* **IPX button.** The DFM declares `IPXBtn` at y=126, but `FUN_0042abf0` hides
  it unless the host has an IPX connection - so the replica hides it too, which
  is what the reviewed screen shows. Re-showing it is a one-line change if a
  build ever wants the four-button layout.
* **TCP availability.** The engine shows `TCPBtn` only when a type-2 connection
  exists; the replica always shows it (modern hosts always have a TCP stack).
* **Slide speed.** `MultiWinUpdate` adds `+10` per frame (286/10 = 29 frames,
  ~1 s at 30 fps); the replica uses `kGtAnimStep = 20` so the slide reads a
  touch snappier while keeping the same start/end positions and formulas.
* **Slide counter start.** The engine resets `+0xF0` when `MultiPnl` is shown;
  the replica resets `gt_frame` when orb 0 is clicked and clamps it at 286.
* **Button text colour.** `TAOWBaseButton.Draw` tints the caption with the
  button's `TextColor`/`TextSelColor` (`5046289` / `9505286` in the DFM); the
  replica draws the white-ink `AoW15WhiteGrey` layer directly, matching the
  title-screen captions. The caption's vertical position is pinned against
  `bin/TextRef.png` (the shipped `Single Computer` button): the glyphs are
  baseline-aligned (the `TSprite16` class shift, see `docs/06_TITLE_SCREEN.md`)
  so the caption is centred in the 24 px button rather than biased to its top.
* **Form chrome / audio.** As on the title screen: a plain Win32 window instead
  of a VCL form, and `Songs\TITLE.IT` through the ported player.

