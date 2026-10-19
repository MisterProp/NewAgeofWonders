# 06 - The title screen (main menu), recovered and reproduced

Stage 3 deliverable: **`AoW.exe`** reproduces the Age of Wonders 1 title
screen - the panning backdrop, the fading logo, the five menu orbs with their
hover captions, and the `Songs\TITLE.IT` music - from the game's own assets and
the geometry recovered out of `AoW.exe`.

All addresses below are `AoW.exe` (image base `0x00400000`), found with
GhidraMCP (`tools/ghidra/`) plus the local helpers (`tools/pe_va.py`,
`tools/disasm.py`, `tools/find_ref.py`, `tools/blobfind.py`).

## 1. Where the screen lives

`TTitleScene` is a Delphi form whose *published field table* and *published
method table* are embedded in the code section next to its class:

```
VA 0x00419318   field table   TitleSceneIL@0x48 Window@0x4c LegalNotice@0x50
                              StrLbl@0x54 NameLbl@0x58 Copyright@0x5c Version@0x60
VA 0x00419396   method table  TitleSceneCreate 0x41add0
                              TitleSceneDestroy 0x41a030
                              WindowDrawSurfaceEx 0x41ae08
                              WindowUpdate 0x41ae3c
                              WindowMouseDown 0x41ae60
                              WindowMouseMove 0x41ae94
```

The DFM (`assets/dfm/AoW/TTITLESCENE.dfm`) declares
`TImageLibrary TitleSceneIL = Int\Scenes\Title.ILB` and the labels
(LegalNotice/StrLbl/NameLbl/Copyright/Version) - i.e. **the five orbs are not
form components, they are drawn by code**.

`TitleSceneShow` (`0x0041a150`, called from `0x00418b88`) starts the music and
sets the video mode:

```c
if (current song name != "SONGS\\TITLE.IT") { stop; load("SONGS\\TITLE.IT"); Play(); }
display.SetSurface(0x280, 0x1E0);            // 640 x 480
display.SetMode(2); ...                      // 30 fps (DFM: FrameRate = 30)
scene->0x94 = 1; scene->0x7c = -1; scene->0x80 = -1;
```

## 2. The assets (`Int\Scenes\title.ilb`, 25 entries)

| idx | name | class | size | role |
|-----|------|-------|------|------|
| 0 | `ScrollBackg5.BMP` | 0x10 raw 16bpp | 1514x432 | the backdrop that pans |
| 1 | `LogoTitleFinal.BMP` | 0x16 | 331x175 | logo, blended during the fade |
| 3 | `LogoTitleFinal.BMP` | 0x11 RLE16 | 331x175 | logo, solid |
| 2,4..9 | `CompanyLogos.BMP` | 0x11/0x16 | 99x103 / 151x84 | publisher logos |
| 10..24 | `OrbFrame.BMP` | 0x11 RLE16 | 82x125 | **the five menu orbs**: `10 + 3*i + state`, `state` 0 = up, 1 = lit, 2 = down |
| 25 | `OrbFrame.BMP` | 0x16 | 82x125 | overlay drawn when the engine's `+0x6d == 2` |

**Address these by entry id, never by position in the entry vector.** The
directory reuses ids (`id 256`/`14830` appear twice for the company logos), so
id 0..9 occupy vector slots 0..8 but id 10 sits at slot **9**. `ImageLib.Get(id)`
in AoW.exe indexes by id; a port that uses the vector position silently draws
the *next* orb's frame - which is exactly the "hovering an orb turns it into
its neighbour" bug the replica shipped once and now guards against (the smoke
test asserts the resolved ids are `10,11,12,...,24`).

The hover captions use the game's bitmap font **`FontModule.AoW15WhiteGrey` =
`Int\Fonts\Age15WhGr.ILB`**. The evidence is the `TFontModule` published-field
table at `AoW.exe` `0x0040383c`, whose records are `[u32 offset][u16 flag][len]
[name]` in declaration order - the offset comes *before* its name:

```
0x44 Age8   0x48 Age10   0x4c Age16s   0x50 Age10s
0x54 Age8s  0x58 Sessis14G  0x5c AoW15WhiteGrey   <-- scene+0xac
0x60 AoW15BlackBrown  0x64 Age10LB  0x68 Age10b  0x6c Age10bs  0x70 TBarNumber
```

`TitleSceneShow` (`0x0041a289`..) does `scene->0xac = FontModule[+0x5c]` and
`scene->0xb0 = FontModule[+0x5c]` (the credits font), and `0x44 + 6*4 = 0x5c`
puts `AoW15WhiteGrey` at that slot. The same reading is confirmed by
`TTitleScene`: its table stores `TitleSceneIL` at `0x44`, which is exactly what
the object code passes to `LoadDiscardableImageData` (`0x0041a150`) - the naive
"offset follows the name" reading would say `0x48`.

That library is a **two-layer face**: every glyph is stored twice, as the same
letterform in two storage layers - the even entry (`font2.BMP`) is black ink on
a white background, the odd entry (`font.bmp`) is white ink on a black
background. The engine draws **one** of them and recolours its ink through
`TImageLibraryFont.SetColor`, so the replica draws a single layer (default: the
white-ink one, which is what shows up on the title's black background); pass
`--font-variant 0` for the other. Drawing both - which the replica did at first,
treating them as a face+shadow pair - doubles every stroke and is visibly wrong.
The layers are not always in the same order in the file (they swap for one
glyph), so the layer is selected by name.

Each glyph is wrapped in a **`TImageNode`** (class `0x100`,
`TImageNode.ClassID` at `Ilpack.dpl` `0x5520d298`):

```
u32 index, u32 class_id = 0x100, { u32 class_id, <image> }, u32 reference
```

The wrapped image records carry no index of their own, and the node's `index` is
the glyph's **character code** - which is exactly how
`TImageLibraryFont.SetTextEx` addresses a glyph (`Get(c)`, `Ilpack.dpl`
`0x55214698`). The glyph order is therefore *not* contiguous ASCII: it is
`33..95, 97..124, 161, 191..214, 216..246, 248..255` - 155 glyphs, with `96`
(`` ` ``), `215` and `247` absent. `aow::IlbFile` recovers that code into
`IlbEntry::index` (`src/engine/ilb.cpp`); indexing the entries by position
instead shifts every character from `a` up one glyph too early, which is what
made the captions read `Sdfobsjp` instead of `Scenario`.

Its DFM record (`.rsrc` `0x0de8de`) gives `CharacterDistance = 0`; `Sessis14G`
next to it is `-2`.

The advance rule comes from `TImageLibraryFont.GetTextWidth` (`Ilpack.dpl`
`0x552142a0`): `advance = image.width(+0x20) + CharacterDistance(+0x50)`, and a
character with no glyph advances `CharacterDistance + 5`. "Scenario" therefore
measures 68 px with a 13 px cap height - the smoke test pins both.

## 2b. The pointer

`TIntGfxMod.Cursors = Images\Cursors.ilb` (100 entries, `Pointers.BMP` /
`Pointers2.BMP` / `ItemPointers.BMP`). The engine hands the window a system
cursor for the DFMs' `Cursor = crDefault`, and the game shows the classic white
arrow - which is `entry id 0` (`Pointers2.BMP`, 11x19). That sprite is class
`0x16`, i.e. it has **no mask**: its background is the palette's entry 0
(`rgb(65,48,98)`). The replica therefore colour-keys the sprite's background,
taking the **most common exact colour of its border ring** (not pixel `(0,0)`,
which on this sprite is a brown outline pixel) - keying only `(0,0)` leaves the
purple box visible. `entry id 0` is the default; the other pointer/scroll
sprites are selectable with `--cursor-id N` (`--no-cursor` keeps only the system
arrow).



## 2c. The five captions

The hover captions are *not* literals in `AoW.exe`. The scene unit owns five
AnsiString slots (`0x0045a010`..`0x0045a020`), nil-initialised by
`@InitializeArray` and filled by `@InitResStringImports` from the table at
`0x0041b130` (count 5, then five `{dest, src}` pairs pointing at IAT slots
`0x45e180/0x45e184/0x45e188/0x45e18c/0x45e1d0`, i.e. **AoWEPACK.dpl** import
slots 355/354/353/352/372 - note the reverse order for orbs 0..3).

At draw time each one is passed through `AoWE.TranslateRStr` (the import thunk
at `0x004021e4` -> `AoWEPACK.dpl`), so the shown text is the *localized* form of
an English resourcestring. Those slots are patched by the loader, so the five
texts cannot be read straight out of the files; what the shipped data does give
is the candidate vocabulary in `AoWEPACK.dpl`'s own string table:

```
64768 Load Game      64797 Scenario    64798 Campaign   64799 Tutorial
64842 Exit           64886 Options
```

**Verified from the game's own screens:** orb 0 (crossed swords) = `Scenario`,
orb 1 (globe) = `New Game`. Orbs 2..4 ship as `Load Game`, `Options`, `Exit`,
which are inferred from the five actions in `0x0041ae94` - pass
`--captions a,b,c,d,e` to set all five without a rebuild.



```
scene.left   = 0
scene.right  = display.width                       ; parameter record {w,h}
scene.top    = (display.height >> 1) - 0x0F0       ; 240
scene.bottom = scene.top + 0x1E0                   ; 480
```

1. **Backdrop** - drawn twice so it wraps:
   `x = -(((max(0, tick64 - 25)) + 0x1EA) % 1514)`, then `x + 1514`, both at
   `y = scene.top`. `tick64` increases one per frame, so the panorama pans left
   at 1 px per frame.
2. **Logo** - solid `[3]` at `x = (display.width - 0 - 331) >> 1`,
   `y = scene.top + 0x0A`, with fade fraction `a` (`ShowBlend`):
   * `tick64 <= 25` : hidden
   * `25 < tick64 < 75` : `a = (tick64 - 25) * 2` (fade in)
   * `tick64 >= 75` and `tick68 <= 730` : `a = 100`
   * `tick68 > 730` : `a = (750 - tick68) * 5` (fade out)
   * `tick68 == 750` : the phase ends (the real screen then rolls the credits)
3. **The five orbs** at `y = scene.top + 0x163` (355), pitch `0x78` (120 px):
   `x(i) = ((orb_slot_width + scene.left + scene.right) >> 1) - 320 + i*120`
   With `orb_slot_width = 82` and a 640 px display that is **x = 41, 161, 281,
   401, 521** - a 562 px row centred on the screen.
   State frame: `2` when `i == +0x7c` (the index the update stored for the
   cursor) *and* `i` is under the cursor; else `1` when `+0x7c == -1` and
   `i == +0x80` (the index stored by the last mouse-down); else `0`.
4. **The hover caption** - only for the orb under the cursor, centred on the
   orb (`x = orb_x(i) + width/2`), `y = scene.top + 0x1BD` (445). It is drawn
   with `FontModule.AoW15WhiteGrey` (the white-ink variant of each glyph, its
   black background keyed out), advancing the pen by
   `glyph_width + CharacterDistance` per character.

The hit test (`0x00419488`) uses the same arithmetic and constants:

```c
if (!(scene.bottom - 125 < y && y < scene.bottom)) return -1;
for (i = 4; i >= 0; --i) {
    left = ((orb_w + scene.left + scene.right) >> 1) - 320 + i*120;
    if (left < x && x < left + orb_w) return i;
}
return -1;
```

`WindowDrawSurfaceEx`'s tick (`0x0041ae08`) increments `+0x64` and `+0x68`
every frame and, at `+0x68 == 750`, hands over to the credits roll
(`0x00419528`, driven by the TStringList built in `0x0041a310`).

Mouse messages: `0x0041ae3c` stores the cursor's hit index in `+0x7c`,
`0x0041ae60` stores mouse-down's hit index in `+0x80`, and `0x0041ae94`
(which reads a button flag in `cl`) dispatches the five actions and clears
`+0x7c` to `-1`.

## 4. Running the replica

```powershell
./build.ps1                  # builds port/bin/AoW.exe
./build.ps1 -Viewer          # also copies it next to the game

cd port
./bin/AoW.exe                        # interactive, from the game folder
./bin/AoW.exe --root ".."            # explicit game root
./bin/AoW.exe --root ".." --render out.png --frame 90 --hover 3 --no-music
```

Flags: `--width/--height` (the layout is resolution independent - the scene
stays 480 px tall and centred, and the orb row re-centres), `--frames N` (quit
after N frames, used by the build's smoke test), `--overlay` (the
`+0x6d == 2` orb overlay), `--captions a,b,c,d,e`, `--no-music`,
`--cursor-id N` / `--no-cursor`, and for head-less checks `--hover N`,
`--mouse X Y`, `--crop X Y W H` (a small crop keeps the preview/golden image
light enough to eyeball).

`./build.ps1 -Test` includes a title-screen smoke test that renders frame 90
and asserts the recovered geometry (`scene_top=0`, `orb0_x=41`).

## 5. Known deviations (all deliberate)

* **Hover captions.** The engine reads them from a runtime resource-string
  array (`AoW.exe` data `0x0045a010`, five AnsiString slots, walked by the loop
  counter at `0x00419f03`..`0x00419f84`). The array is zero-initialised in the
  file and filled by `@InitResStringImports`
  (`0x41b101`..`0x41b111`) from `AoW.exe`'s string table, whose ids are not yet
  resolved. Two of the five are confirmed from the game's own screens - the
  crossed-swords orb (index 0) reads **"Scenario"** and the globe orb (index 1)
  reads **"New Game"**; indexes 2..4 ship as `Load Game / Options / Exit`
  (inferred from the five actions in `0x0041ae94`: scenario setup, show the file
  window, close the form) and `--captions` overrides all five. TODO(RE): decode
  the string-table ids to settle indexes 2..4.
* **Credits roll.** Instead of rolling credits after frame 750 the replica
  restarts the title timeline - which is what the real screen does once the
  credits finish (it sets `+0x68 = 0`, `+0x95 = 0`). The credits text itself
  (`0x0041a310`) is recoverable (the group headers are `*RStr` resource
  strings, the names are literals) but is not part of the menu screen.
* **Font rendering.** The captions use the game's own `AoW15WhiteGrey` at its
  storage size, with the `CharacterDistance = 0` advance, so the face, size and
  spacing match. Two things are simplified: the replica picks the white-ink
  storage layer instead of driving `SetColor`/`FontColorPaletteIndex` (the
  title screen only ever shows that colour), and glyphs are blitted directly
  rather than through `TImageLibraryFont`'s palette machinery. Each glyph is
  placed at the engine's sprite anchor - `base` **plus** the class shift
  (`TSprite16` +0x60, `TRLESprite08` +0x54) - which `decode_glyph` folds in;
  ignoring that shift (as the first replica did) parks every glyph on its cell
  top and biases the text toward the top. This applies to every face: the small
  `Age8`/`Age8s`/`Age10`/`Age10s`/`Age10tb` are class 0x02 sprite glyphs whose
  `yshift` is 0 for the caps and 2–3 for the x-height glyphs, so those faces
  floated their lowercase letters just like `AoW15WhiteGrey` did. The
  `AoW15WhiteGrey` tall glyphs carry shift 1, so `draw_text`'s callers, which
  anchor to the *cap line*, lift the pen by `kWhgCapShift = 1`; the 0x02 faces
  have cap shift 0 and need no such lift.
* **Audio.** `Songs\TITLE.IT` is played by the ported Galaxy module player
  (`src/engine/galaxy_*`) through WinMM, not by `galaxy.dll`.
* **Pointer.** The engine lets Windows draw the arrow (`crDefault`); the replica
  paints the matching `Cursors.ilb` sprite (`entry id 0`) inside the frame so
  the pointer is also present in head-less renders, and hides the system cursor
  over the client area.
* **Form chrome.** The engine runs the screen inside a VCL form with a black
  mask colour; the replica is a plain Win32 window painting the same 640x480
  frame (the black surround is identical).

