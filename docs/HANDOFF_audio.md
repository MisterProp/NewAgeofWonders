# HANDOFF - Audio: faithful port of the AoW1 music engine

Status: **working but not yet 1:1**. All 24 songs play; the port is verified
instruction-by-instruction up to the mixer, and one block of the mixer's
per-voice gain computation is still reconstructed rather than transcribed.

Read this together with `docs/05_AUDIO.md` (architecture + RE notes).

> **Update:** the shipped `aow_audio` / `aow_menu` now decode the `.IT` music
> with **libopenmpt** (`src/include/aow/openmpt.hpp` +
> `src/engine/openmpt_player.cpp`), which plays the modules faithfully.  This
> reconstructed Galaxy port is still compiled in and selectable with
> `aow_audio --backend galaxy`, so the work below remains the reference for
> A/B comparison.  See `docs/05_AUDIO.md` section 4.

---

## 1. What the game's audio actually is

```
AoW.exe / AoWEPACK.dpl  (Delphi)
   -> SoundP.dpl : unit Sound (Sound.PlaySample / PlaySongEx / TSFXLibrary.Play)
                   + unit GalSndS (TGalaxySoundSystem)
   -> galaxy.dll : "Galaxy Sound System 4.00"  (MSVC C, 373,760 bytes,
                   96 glx* exports, WinMM + DirectSound output)
```
* `SoundP.dpl` **dynamically loads `galaxy.dll`** (string "galaxy.dll",
  "Cannot link to DLL (ProcParam:"). The real audio engine is `galaxy.dll`.
* `Sound.GetSongName` compares `ExtractFileExt` against the literals **".IT"**
  and **".S3M"** -> the engine's module formats are Impulse Tracker and
  Scream Tracker 3.
* `glxLoadMusic` dispatches on the 4-byte magic read at offset 0.
  `DAT_10050c5c` == `"IMPM"` -> `FUN_10020290` (the IT loader used by AoW).
  Other magics: MTM, MThd, SCRS, WAVE, FAR\0xFE, "AM  ", SCRM, PTMF,
  MAS_UTrack_V00, "Extended Module: ", "!Scream!", BMOD2STM.
* Shipped audio: `Songs/**/*.IT` (24), `Sfx/**/*.WAV` (454),
  `Sfx/Kain/*.VAG` (31 - these are **RIFF/WAVE**, not PSX VAG).
* Output rate: **48000 Hz** (the game's "sound quality" setting). The engine
  adapts to `glxSamplingrate`, so the port renders at whatever rate you ask.

## 2. File formats as the engine sees them (VERIFIED)

IT header (0xC0): orders/instr/sample/pattern offset tables at
`0x20/0x22/0x24/0x26`, GV `0x30`, initial speed `0x32`, tempo `0x33`,
channel pan `0x40`.

**Sample header = Galaxy's own layout, NOT canonical IT** (this cost hours):
| off | field |
|-----|-------|
| 0x12 | IT flags (bit1 16-bit, bit2 stereo, bit3 loop, bit4 pingpong) |
| 0x13 | volume |
| 0x14 | name (26 bytes) |
| 0x2E | convert (bit0 signed, bit2 delta, bit3 IT215) |
| 0x2F | default pan |
| 0x30 | length (frames) |
| 0x34 / 0x38 | loop begin / end |
| 0x3C | C5 speed |
| 0x40 / 0x44 | sustain begin / end |
| 0x48 | sample data pointer |

Instrument header (0x226, standard IT): fadeout `0x14`, global vol `0x18`,
numSamples `0x1E`, name `0x20`, keyboard map `0x40` (120 bytes),
volume env `0x130`, panning env `0x182`, pitch env `0x1D4`
(each: flags, num, loopBegin, loopEnd, sustainBegin, sustainEnd, then 25 x
`{int8 value, uint16 tick}`).

**Pattern packing is NOT the standard IT bitfield.** It is channel-indexed:
```
cell  := byte cb
   ch   = (cb - 1) & 0x3F
   if (cb & 0x80) { mask = next_byte; remember_mask[ch] = mask; }
   else           { mask = remember_mask[ch]; }          ; reuse!
   mask bits: 1=note 2=instrument 4=volume 8=cmd(+info) 0x10/0x20/0x40/0x80 = reuse remembered field
   payload order: note, instrument, volume, cmd, info
   a 0x00 byte ends the row
```
This reproduces **454/454 patterns byte-exactly** on all 24 songs
(`tmp_gr/_gal_pat.py`).

Loader output (internal cell stream, one `0x00` per row):
`[ channel | 0x80 if effect | 0x40 if note+instr | 0x20 if volume ]`, payloads
in the order **effect(param,cmd), instrument, note, volume** - i.e. the
note+instrument pair is stored **[instrument][note]**.

IT effect -> Galaxy internal effect (from `FUN_10020290`):
`A`->0x0F speed, `B`->0x0B jump, `C`->0x0D break, `D`->0x0A/0x0E vol slide,
`E`->0x02, `F`->0x01 (fine/extra-fine -> 0x0E with 0x20/0x10), `G`->0x03,
`H`->0x04, `J`->0x00, `K`->0x06, `L`->0x05, `O`->0x09, `Q`->0x0E|0x90,
`R`->0x07, `S`->0x0E, `T`->0x0F, `X`->0x08; **`M` is dropped**.

`glxPlayerMode = (((flags & 8) >> 3) ^ 1) | 2`  (bit1 = IT/linear).

## 3. Playback model (VERIFIED at instruction level)

**Voice record = 160 bytes** (`0x1006a860 + ch*0xA0`; the DLL's overlapping
globals are typed views of it: byte arrays at `+ch*0xA0`, ints at `+ch*0x28`,
shorts at `+ch*0x50`).
| off | field |
|-----|-------|
| +0x0C | current **note** |
| +0x0D | current **instrument** (bit7 = key-off) |
| +0x0E | sample slot |
| +0x0F | note-active flag |
| +0x10 | enable (glxStartMusic sets 1) |
| +0x12 | per-voice volume = **20000** after glxResetMusic |
| +0x18 | note volume (0x7FFF on trigger) |
| +0x1A | channel volume (0x7FFF default) |
| +0x1E | panning (0x28 default) |
| +0x24 | volume-envelope output |
| +0x2A | fadeout counter (0x7FFF) |
| +0x66 | pan-envelope output (~0x4000) |
| +0x32/+0x46/+0x4E | loop start / position / 16.16 step (disasm view) |

`glxResetMusic` defaults: speed 0x30, tempo 0x78, minPeriod 0x1C5,
maxPeriod 0x7800, musicVolume 0x7F, `glxInitialPanning[32]`
(`<0x41` -> `*2` clamped 0x7F, else `0xA0` = "no panning" marker).

**Note trigger** (`FUN_1000e8c0`): `slot = instrument.noteMap[note]`;
`period = (0x78 - (note+1)) * 0x100` (linear);
`freq = c5speed * 2^((note-60)/12)` (both modes; the tables
`DAT_100447E0`/`DAT_100449C0` reduce to exactly this);
`step = freq/rate` in **16.16**.
Note byte semantics: `0xFE` = cut (+vol 0), `0xFF` = off (loader maps to 0x80).
An **instrument-only cell must not retrigger**.

**Tick pump** (`0x1000cb94`-`0x1000cc34`) - both sides 16.16 **samples**:
```
acc -= blockSamples << 16;
if (acc < 0x10000) { FUN_1000d120(); acc += DAT_10074d0c; }
DAT_10074d0c = DAT_100748cc = ((rate*5)/(tempo*2)) built exactly in 16.16
```

**Mixer** - this is the important structural finding:
* `glxInit` allocates a 0x2000 buffer at `DAT_10074eb4`.
* `glxStartOutput` calls `FUN_1001aad1`, which **copies a mixing routine into
  that buffer** (`0x603` bytes from `0x1001a030` + `0x1ED` from `0x1001a8e4`).
* The caller does `CALL dword ptr [0x10074eb4]` (`0x1000cef0`) with **14 stack
  args** (push order at `0x1000ceb8`-`0x1000ceef`, `ADD ESP,0x38`).
* Mixer body `FUN_1001a030` (MMX/SSE):
  - samples read as bytes and packed pairwise, `PMADDWD` with the loop-wrap
    vector `MM3/MM4/MM5` (mask `0x10053780` = 0x7FFF per word),
    `PADDW MM3,MM4` / `PAND MM3,MM5` to advance+wrap the loop position,
  - `CVTPI2PS` -> **float32**, `MULPS` by the gain vectors at
    `0x10053750/60/70`, `ADDPS` into the output buses at EDI/EAX/EBX
    (**three stereo output buses, accumulated in float**),
  - position advance is `ADC ESI,EBP` -> **nearest neighbour, NO interpolation**.
  - gains are **6 x int16** in the arg block at `+0x24/+0x28/+0x2c/+0x30/
    +0x34/+0x38`, converted to float and scaled by the constant at
    `0x10053790` = four x `0x30800000` = **2^-30**; flag bit2 `NEG`s three of
    them (reverse/pingpong).
* Gain chain in the caller (pseudo-C, operands verified in the disasm):
```
g  = ((voice[+0x12] * gvol * gvol * 2) >> 15)      ; gvol = glxMusicVolume (0..127)
g  = (g * voice[+0x18]) >> 15                       ; note volume
g  = (g * voice[+0x1A]) >> 15                       ; channel volume
g  = (g * voice[+0x24]) >> 15                       ; volume envelope
g  = (g * voice[+0x2A]) >> 15                       ; fade
g  = (g * voice[+0x66]) >> 15                       ; pan-envelope output (~0x4000)
panL = sin(pi/2 * (0x7FFF-pan)/0x7FFF) ; panR = sin(pi/2 * pan/0x7FFF)
```
  Note the caller passes **`sinTable[0x7FFF-pan]` as the FIRST gain** (so
  "left" uses the complement) - getting this backwards inverts the stereo.

## 4. The port (what exists where)

| file | contents |
|------|----------|
| `src/include/aow/galaxy.hpp` | `Sample`, `Envelope`, `Instrument`, `Pattern`, `Song`, `Voice`, the internal effect enum, `load_it()`, `class Player` |
| `src/engine/galaxy_it.cpp` | loader port of `FUN_10020290` (header, orders, offsets, panning, samples, instruments, patterns) + `decode_sample` port of `FUN_10027410` (stereo de-interleave, unsigned bias, 32-step fade ramp) |
| `src/engine/galaxy_player.cpp` | sequencer + note trigger + effects + `mixBlock` + `render` (the `Player` class) |
| `src/engine/audio.cpp` | RIFF/WAVE decode (8/16/24/32-bit), PSX VAG, WAV writer, WinMM `waveOut` streaming (`AudioOut`) |
| `src/game/audio_showcase.cpp` | `aow_audio` (interactive playlist / `--list` / `--render` / `--sfx`) |
| `src/tests/galaxy_test.cpp` | loader validation over every song |
| `src/tests/audio_test.cpp` | headless render of every song + SFX decode |

The interim hand-written `ItModule` was **retired** (file deleted, declaration
replaced by a pointer to `aow/galaxy.hpp`).

Build / test:
```powershell
cd "c:\GOG Games\Age of Wonders\port"
./build.ps1 -Test          # builds aow.exe, aow_view.exe, aow_audio.exe + all tests
```
Expected output (regression gate):
```
audio tests: 7 passed, 0 failed (songs=24, samples=277)
galaxy loader: songs=24/24 patterns-ok=578/578 samples=283
```

Listen / render:
```powershell
./bin/aow_audio.exe --root ..                                   # interactive (starts on TITLE.IT)
./bin/aow_audio.exe --root .. --render re\preview --seconds 25  # 24 preview WAVs (currently committed)
./bin/aow_audio.exe --root .. --sfx claw                        # single SFX (proves the output path)
```

## 5. Benchmark workflow (IMPORTANT - use this for every change)

The reference is `C:\GOG Games\Age of Wonders\Title og.wav`
(**48000 Hz**, stereo, 60.2 s, recorded from the game, music only).

Build the comparison renderer and render:
```powershell
& "C:\msys64\ucrt64\bin\g++.exe" -std=c++17 -O2 -I src/include `
   src/engine/audio.cpp src/engine/galaxy_it.cpp src/engine/galaxy_player.cpp `
   tmp_gr/ref.cpp -o bin/ref.exe -lwinmm
./bin/ref.exe '..\Songs\TITLE.IT' re\title_mine.wav 48000 60      # optional 6th arg = start order
```
Meters (all pure-stdlib Python, in `tmp_gr/`):
| script | measures |
|--------|----------|
| `cmp.py` | per-5s RMS ref vs mine + the flat level offset (dB) |
| `cmp2.py` | mean zero-crossing rate (brightness) + timbre-contour correlation |
| `spec.py` | Goertzel band energies, 73 Hz .. 7.4 kHz (spectral balance) |
| `scale.py` | best time-scale factor between the two (rate error) |
| `rate.py` | where the loud content ends in each file |
| `wavecorr.py` | normalised waveform correlation vs lag (same signal or not) |
| `order.py` | correlation for start orders 0..7 |
**Current readings** (what "not 1:1" looks like numerically):
```
waveform corr        0.05 - 0.30 at inconsistent lags   (=> not the same signal)
envelope corr        0.335 at lag 0 ; best 0.45-0.54 at scale 1.015-1.035
mean ZCR             ref 0.0168  mine 0.0189  (mine ~13% brighter)
level offset         ~13.5 dB flat (mine louder)
loud content ends    ref 60.1 s (end of capture)  mine 56.9 s
```

## 6. Dead ends - DO NOT re-litigate these

1. **Sample-mode sample selection.** `slot = noteMap[note]` vs the instrument
   column: my loader builds an identity map, so both are identical -
   changing it produced **byte-identical output**.
2. **Pingpong loops.** Implemented; metric change negligible (1.127 -> 1.125).
3. **Wrong start order.** Rendered orders 0..7; **order 0 is best (0.335)**, all
   others worse -> the capture does start at order 0.
4. **Samples-vs-bytes units in the tick accumulator.** Disproved at
   instruction level: `SUB EDI,EBP` and `DAT_10074d0c` are both 16.16 samples.
5. **Sample decode bug.** Verified: file sample 0 = `-5,-3,-1,3,8,15,...`
   and my decode = `0,0,0,0,1,2,...` which is exactly `raw[i]*i >> 5`
   (galaxy's 32-step fade-in) for i=0..5.
6. **Linear interpolation.** Added it: ZCR ratio 1.53 -> 1.13 (closer!) but the
   disassembly shows galaxy is **nearest-neighbour** - so this is a real
   contradiction that is still unexplained (possibly the output stage resamples,
   or the loop-wrap vector smooths, or the capture was resampled by the OS).
7. **The decompiler's pseudo-C for the mixer/pitch is unreliable.** Ghidra warns
   `Could not recover jumptable`, `Treating indirect jump as call`, swaps
   `__allshr` arguments, and mis-types `DAT_10074eb4` (buffer vs function
   pointer) because of overlapping globals. **Use the disassembly.**

## 7. The open problem (start here)

Everything upstream of the mixer is transcribed and verified. The one block
that is still *reconstructed* is the **per-voice gain computation in
`FUN_1000c330`**, and the evidence says that is where the remaining difference
lives (13 % brightness, wrong note emphasis, ~1.5 % rate drift).

Do this, instruction by instruction:
1. Dump `0x1000c400`-`0x1000cb70` (`python tmp_gr/_dis.py <lo> <hi>`) - this is
   the loop that computes `iVar27` (the combined gain) and `iVar20/iVar23`
   (the pan pair) per voice. Find the **actual** multiply/shift sequence; do not
   trust the pseudo-C's `>> 15` rendering.
2. Dump `0x1000ce70`-`0x1000ceb0` - the ramp/portamento step
   (`SHL 0x10`, `CDQ/IDIV`, `INC`, clamp) that feeds the mixer args.
3. Dump the 14 `PUSH`es at `0x1000ceb8`-`0x1000ceef` to fix the exact argument
   order (which gain goes to which of the six mixer slots).
4. Re-express `Player::State::mixBlock` on that: **float32** accumulation,
   `int16 gain x 2^-30`, **nearest neighbour** (`ADC ESI,EBP`), and the
   `MM3/MM4/MM5` loop-wrap vector semantics.
5. Re-run the benchmark after **each** change; the success signal is
   `wavecorr.py` rising well above ~0.5 at **one consistent lag**, plus
   `spec.py` matching band-for-band.

Secondary suspects, in order:
* the **~13.5 dB** flat level offset (the user says it is not his volume
  setting) - re-check `gvol`'s source at play time: the loader seeds
  `glxMusicVolume` from the IT GV (0x30) and the game overrides it via
  `glxSetMusicVolume`; verify which is live.
* the **~1.5 %** rate difference - `scale.py` peaks at 1.015-1.035. The tick
  pump itself is exact, so look for a *second* rate factor (e.g. the output
  stage, the `sVar17` shift in `uVar22 = param_7 << sVar17`, or the device
  rate vs `glxSamplingrate`).
* the **ZCR contradiction** in dead-end #6 (reference is smoother than
  nearest-neighbour) - the `MM3/MM4/MM5` vector may be smoothing more than I
  assumed.

## 8. How to regenerate the RE artifacts

Ghidra 12.1.2 is at `C:\Users\Utente\Desktop\Programmini\ghidra_12.1.2_PUBLIC`
(the GUI is running; a project `AoWDecomp` has 13 Delphi modules open).

Decompiled + disassembled output already in `re/exports/`:
```
re/exports/Galaxy/Galaxy.decompiled.c     417 functions (galaxy.dll)
re/exports/Galaxy/Galaxy.disasm.txt       59141 instructions (+ function markers)
re/exports/Galaxy/Galaxy.functions.csv    address,name,size,...
re/exports/Galaxy/Galaxy.strings.txt
re/exports/SoundP/SoundP.decompiled.c     291 functions (SoundP.dpl)
re/decomp/*.sound.c                        game-side callers (AoWEPACK/HSEPack/AoW.exe)
```
Regenerate galaxy.dll (headless; note the project dir must exist and stdin must
be redirected because the .bat pauses on error):
```powershell
$env:GHIDRA_HOME='C:\Users\Utente\Desktop\Programmini\ghidra_12.1.2_PUBLIC'
$env:AOW_OUT='C:\GOG Games\Age of Wonders\port\re\exports'
New-Item -ItemType Directory -Force -Path re\ghidra_proj_gal | Out-Null
& "$env:GHIDRA_HOME\support\analyzeHeadless.bat" re\ghidra_proj_gal aowg `
  -import ..\Galaxy.dll -scriptPath tools\ghidra -postScript DecompileAll.java `
  -analysisTimeoutPerFile 3600 -deleteProject
# same with -postScript DisasmAll.java for the listing
```
Slice the listing:
```
python tmp_gr/_dis.py <lo_hex> <hi_hex>       # prints only instructions in range
```

GhidraMCP (for the *loaded* Delphi modules) is on `http://127.0.0.1:8089`;
`tools/ghidra/mcp.py <outfile> <endpoint> [k=v ...]`. Caveat: it **cannot pass a
`path=` query param** (name clash with its own argument) - use direct urllib /
`Invoke-WebRequest` for `/open_program`. `SoundP.dpl` is NOT in the GUI project,
hence the headless decompile.

## 9. Environment / build pitfalls

* Compiler: MSYS2 `C:\msys64\ucrt64\bin\g++.exe` (no CMake on this box; use `build.ps1`).
* Link `-lwinmm` for anything that includes `audio.cpp`.
* PowerShell + `.bat`: java's stderr warning trips `$ErrorActionPreference='Stop'`
  -> set `Continue`; redirect stdin (`RedirectStandardInput`) so the pause does
  not hang.
* Writing `bin\aow_audio.exe` fails if an instance is still running - kill it first.
* `*_output.txt` via `>` in PowerShell 5.1 is UTF-16 - use `Out-File -Encoding utf8`
  (this bit me twice with the extractors).
* `python` here has **no numpy** - all meters are pure stdlib (`wave`,`struct`,`math`).

## 10. One-line summary for a fresh agent

The engine port is complete and instruction-verified except the **per-voice gain
block in `FUN_1000c330` (disassemble `0x1000c400`-`0x1000cb70` + the arg pushes
at `0x1000ceb8`)**; transcribe that into `mixBlock`, and use
`Title og.wav` + the `tmp_gr/*.py` meters as the acceptance test after every
change.
