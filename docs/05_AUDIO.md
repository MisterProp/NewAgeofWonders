# 05 - Audio: how the game plays music & sound, and the C++ port

> **Resuming this work?** Read **`HANDOFF_audio.md`** first - it records what is
> verified, the dead ends (do not re-litigate them), the benchmark workflow
> (`Title og.wav` + the `tmp_gr/*.py` meters), and the single open item
> (the per-voice gain block in `FUN_1000c330`).


This documents the audio path recovered with Ghidra (MCP + headless) and the
modern C++ re-implementation that ships as **`aow_audio`**.

## 1. The original stack

```
game code (AoWEPACK.dpl / AoW.exe)
      |  Sound.PlaySample / Sound.PlaySongEx / Sound.TSFXLibrary.Play
      v
SoundP.dpl  -- Delphi wrapper (unit Sound + GalSndS + Galaxy + DSound)
      |  dynamically loads galaxy.dll ("Cannot link to DLL (ProcParam:")
      v
galaxy.dll  -- "Galaxy Sound System 4.00" tracker/module + sample mixer
      |  WinMM / DirectSound output ("Windows(r) wave output driver 4.0.0",
      v  "Directsound(tm) output driver v4.00")
speakers
```

* `SoundP.dpl` is **not** part of the Ghidra GUI project, so it was decompiled
  head-less (`port/re/exports/SoundP/SoundP.decompiled.c`, 291 functions).
* The MCP was used for the game-side callers in `AoWEPACK.dpl`,
  `HSEPack.dpl` and `AoW.exe` (`port/re/decomp/*.sound.c`).
* `galaxy.dll` exports exactly the `glx*` API `SoundP` resolves at runtime:
  `glxInit/glxDeinit`, `glxStartOutput/glxStopOutput/glxFlushOutput`,
  `glxLoadMusic/glxStartMusic/glxStopMusic/glxControlMusic/glxResetMusic`,
  `glxLoadSample/glxStartSample/glxStartSample3D/glxStopSample/glxControlSample`,
  `glxLoadInstrument/glxStartInstrument/glxControlInstrument`,
  `glxSetMusic|SampleVolume`, `glxSetMusic|SampleReverb/Chorus`,
  `glxSongName/glxSongLength/glxOrders/glxPatterns/glxPatternRow`,
  `glxSetIOInterface/glxSetMemInterface/glxfOpen/glxfClose`, ...

## 2. The `Sound` unit API (what the game actually calls)

Flat functions (imported by `AoW.exe`, implemented in `SoundP.dpl`):

`Sound.InitializeSoundSystem`, `ReleaseSoundSystem`, `ProcessSound`,
`RegisterSoundSystem`, `RegisterSoundEngineClasses`, `PlaySample`,
`PlaySampleEx`, `PlaySong`, `PlaySongEx`, `StopSong`, `SetMusicEnabled`,
`SetMusicVolume`, `FadeMusicVolume`, `SetSFXEnabled`, `SetSFXVolume`,
`GetSongName`, `PlayingSongFileName`, `GetSoundSettings`, `SetSoundSettings`,
`BackupSongStatus`, `RestoreSongStatus`, `SamplesActive`, `SamplesLoaded`,
`CreateSample`, `CreateSampleData`, `LockSampleData`, `UnlockSampleData`.

Classes: `TSoundSystem`, `TSample`, `TSampleData`, `TSongPlayer`, `TSonglist`,
`TSFX`, `TSFXLibrary`, `TSFXSequence`, `TSFXManager`.  Playing an effect is
`TSFXLibrary.Play(lib, id)` -> `Find(id)` -> `TSFXSequence.Play(seq)`.

Two decompiled facts worth keeping:

* **`Sound.GetSongName`** opens `<Sound.BaseSoundEnginePath><file>` and, when
  `ExtractFileExt` upper-cased equals the literal **`.IT`**, seeks to offset 4
  and reads 28 bytes; for **`.S3M`** it reads 28 bytes from offset 0.  So the
  engine's module formats are **Impulse Tracker and Scream Tracker 3**, and the
  song name is the IT header's 26-byte title.
* **`GalSndS.TGalaxySoundSystem`** is the concrete engine registered by
  `RegisterSoundEngineClasses`; it forwards to `galaxy.dll`.

## 3. On-disk audio in this install

| File(s) | Real format | Contents |
|---------|-------------|----------|
| `Songs/**/*.IT` (24) | Impulse Tracker 2.14 (`IMPM`) | the soundtrack (title, races, combat themes) |
| `Sfx/**/*.WAV` (≈454) | RIFF/WAVE PCM (mostly 16-bit 44.1 kHz) | sound effects |
| `Sfx/Kain/*.VAG` (31) | **RIFF/WAVE**, despite the `.VAG` name | more sound effects |

Notes measured on the songs: `linear` slides are used by most, `instrument`
mode only by `CONTENT.IT`; samples are 8- or 16-bit, signed, **never
delta/compressed**; the sample-header fields sit at the **IT-2.14 offsets**
(`length` @0x30, loop @0x34/0x38, C5 speed @0x3C, data pointer @0x48), i.e.
14 bytes after the "2.15" offsets.  The decoder auto-detects which layout a
file uses by checking that the sample data is contiguous.

## 4. The C++ port

The port has **two module backends behind one interface**:

* **libopenmpt** (`src/include/aow/openmpt.hpp` + `src/engine/openmpt_player.cpp`,
  `aow::openmpt::Player`) - the **default**.  The shipped `.IT`/`.S3M` files are
  ordinary tracker modules, so they are handed to the battle-tested
  [libopenmpt](https://lib.openmpt.org) decoder instead of relying on our
  reconstruction.  `aow_audio` and `aow_menu` use this unless told otherwise.
* **the reconstructed Galaxy player** (`src/include/aow/galaxy.hpp` +
  `src/engine/galaxy_it.cpp`, `galaxy_player.cpp`, `aow::galaxy::Player`) - the
  instruction-level re-implementation of galaxy.dll, kept for A/B comparison
  (`aow_audio --backend galaxy`).

| File | Purpose |
|------|---------|
| `src/include/aow/audio.hpp` | public API (`PcmBuffer`, `load_wav`, `load_vag`, `load_sample`, `write_wav`, `AudioOut`) |
| `src/engine/audio.cpp` | RIFF/WAVE decode (8/16/24/32-bit PCM), PSX VAG decode, WAV writer, WinMM `waveOut` streaming output |
| `src/include/aow/openmpt.hpp` + `src/engine/openmpt_player.cpp` | `aow::openmpt::Player`: libopenmpt-backed module player |
| `src/include/aow/galaxy.hpp` + `src/engine/galaxy_it.cpp`, `src/engine/galaxy_player.cpp` | the reconstructed Galaxy loader / sequencer / mixer |
| `src/game/audio_showcase.cpp` | `aow_audio` showcase application (`--backend openmpt|galaxy`) |
| `src/tests/audio_test.cpp` | head-less decoder tests (no sound device needed) |

`AudioOut` uses WinMM `waveOut` (the same "wave output driver" family Galaxy
listed) with a small queue of prepared buffers, so no third-party audio library
is required for **output** - consistent with the port's stdlib-only policy.  The
module **decoding** is delegated to libopenmpt: on MSYS2 install
`mingw-w64-ucrt-x86_64-libopenmpt`, and `build.ps1` links `-lopenmpt` and copies
its runtime DLLs (`libopenmpt-0.dll` + the mpg123/ogg/vorbis/zlib closure) into
`port/bin/` so the executables stay self-contained.

`aow::openmpt::Player` deliberately mirrors the `aow::galaxy::Player` API
(`load`/`start`/`stop`/`playing`/`set_loop`/`set_start_order`/`set_master_volume`/
`render`/`title`), so the showcase can swap backends with no call-site changes.
It wraps the libopenmpt C API: `openmpt_module_create_from_memory2` to load,
`openmpt_module_set_repeat_count(-1|0)` for looping, `openmpt_module_set_render_param`
with `OPENMPT_MODULE_RENDER_MASTERGAIN_MILLIBEL` for the Galaxy 0..128 volume
scale (mapped as `2000*log10(vol/128)`), `openmpt_module_set_position_order_row`
for the start order, and `openmpt_module_read_interleaved_stereo` for the PCM.

The game's **music player** - the ordered song list a scenario plays - is a port
of `Sound.TSongPlayer` (`SoundP.dpl`): `aow::PlaylistPlayer`
(`src/engine/playlist.cpp`) plus `aow::scenario_playlist`, which reads the
scenario's playlist out of the map (tag 0x1d / 0x3c).  The title screen and the
in-game screen both feed the same player (`src/game/menu.cpp`), which swaps
`Songs\TITLE.IT` for the map's list when a game starts.  The recovered tag
layout and the `FUN_00454d2c` / `TSongPlayer.Update` logic are written up in
**`09_GAME_SCREEN.md` section 8**; `aow_playlist_tests` covers it.

## 5. Using the showcase

```powershell
cd "c:\GOG Games\Age of Wonders\port"
./build.ps1                 # also builds bin/aow_audio.exe

./bin/aow_audio.exe --root .. --list          # inventory + libopenmpt version
./bin/aow_audio.exe --root ..                 # interactive playlist (libopenmpt)
./bin/aow_audio.exe --root .. --render re/audio_out --seconds 20
./bin/aow_audio.exe --root .. --sfx claw      # play one effect (substring)
./bin/aow_audio.exe --root .. --backend galaxy  # A/B: the reconstructed engine
```

Music is decoded by **libopenmpt** by default; `--backend galaxy` switches to the
reconstructed Galaxy player.  `--list` prints the linked libopenmpt version.

Interactive keys: **n** next song, **p** previous, **w/s** volume, **r**
restart, **f** play a random sound effect, **q** quit.

`--render` writes each song to a 16-bit stereo WAV, which is how the decoder is
verified on machines with no audio device.

## 6. Tests

`./build.ps1 -Test` (or `ctest`, target `audio_decode`) runs `aow_audio_tests`,
which

* loads and renders all 24 `Songs/**/*.IT` for 20 s each **through both the
  reconstructed Galaxy player and libopenmpt**, requiring every one to produce
  real audio,
* checks that libopenmpt reports the title, duration, order count and `it` type
  for the modules,
* decodes a spread of the `Sfx` library (`.WAV` + `.VAG`) and requires each
  decoded sample to be non-silent,
* round-trips a `PcmBuffer` through `write_wav`/`load_wav`.

Current result: `13 passed, 0 failed (songs=24, samples=178)` with libopenmpt
`0.8.3`; all 40 sampled effects decode.

> Note: `build.ps1 -Test` currently stops earlier, in the unrelated PFS
> rule-fidelity test (`FAIL: ability[1] (Flying) cost == 100`).  Run the audio
> suite on its own with the command in `src/CMakeLists.txt`'s `audio_decode`
> test, or build/run `bin/aow_audio_tests.exe ..` directly.

## 8. galaxy.dll analysis (the real engine)

`galaxy.dll` was decompiled head-less (417 functions, MSVC C, 96 `glx*` exports;
`re/exports/Galaxy/`). Key findings, all verified against the shipped files:

* `glxLoadMusic` dispatches on the file magic read at offset 0. `DAT_10050c5c`
  is the literal **`"IMPM"`**, which routes to `FUN_10020290` - so **the game's
  `.IT` files are loaded by this function**. (Other magics handled: `MTM`,
  `MThd`, `SCRS`, `WAVE`, `FAR\0xFE`, `AM  `, `SCRM`, `PTMF`,
  `MAS_UTrack_V00`, `Extended Module: `, `!Scream!`, `BMOD2STM`.)
* `FUN_10020290` is an Impulse-Tracker loader: 0xC0 header, 0x226 instrument
  headers, 0x50 sample headers, IT order/instrument/sample/pattern offset
  tables at 0x20/0x22/0x24/0x26, global volume at 0x30, initial speed/tempo at
  0x32/0x33, channel pan at 0x40. It sets
  `glxPlayerMode = ((flags & 8) >> 3) ^ 1 | 2`, i.e. bit 1 means "IT/linear".
* **The pattern packing is NOT the standard IT bitfield.** Each cell begins
  with a byte whose low 6 bits are `(channel + 1)` and whose bit 7 means "a new
  mask byte follows"; otherwise the *previously seen* mask for that channel is
  reused. The mask bits are note/instr/volume/effect, with `0x10/0x20/0x40/0x80`
  meaning "reuse the remembered field". `0x00` ends the row. Notes are stored
  as `note + 1` (so `0xFE` = note cut + volume 0, `0xFF` = note off).
  This scheme reproduces **454/454 patterns byte-exactly** across the 24 songs
  (`tmp_gr/_gal_pat.py`), which is why a plain-IT decoder mis-read some cells.
* The IT effect letter is remapped into a small internal effect set
  (`A` -> speed, `B` -> order jump, `C` -> break, `D` -> vol slide,
  `E`/`F` -> porta with fine/extra-fine variants, `G` -> tone porta,
  `H` -> vibrato, `J` -> arpeggio, `K`/`L` -> combined, `O` -> offset,
  `Q` -> retrig, `R` -> tremolo, `S` -> misc, `T` -> tempo, `X` -> panning).
  `M` (channel volume) is dropped by the loader.
* Per-voice state is a 160-byte record (`0xA0`) reached from
  `0x1006a870 + ch*0xA0`; pitch is kept as a period (`glxMinPeriod=0x1C5`,
  `glxMaxPeriod=0xFF00`) and the mixer step is a 16.16 fixed-point increment
  computed from a per-note frequency table (`DAT_100449c0`) times the sample's
  C5 speed. `glxResetMusic` documents the defaults: tempo 0x78, speed 0x30.

**Status of the port:** the engine is ported and wired in.
`src/engine/galaxy_it.cpp` (loader `FUN_10020290` + sample decode `FUN_10027410`),
`src/engine/galaxy_player.cpp` (sequencer `FUN_1000d120`, note trigger + effects
`FUN_1000e8c0`, volume scaling `FUN_1000f960`, mixer `FUN_1000c330`).  `aow_audio`
selects it with `--backend galaxy` (`aow::galaxy::Player`); the default music
backend is now libopenmpt, and the interim hand-written `ItModule` has been
retired.  Recovered verbatim from the mixer: the gain chain
`((v+0x12 * gvol^2 * 2) >> 15) * noteVol ... * volEnv * fadeVol`, the sine
panning, the per-tick volume-envelope/sustain/fadeout, and the note tables
(`DAT_100447E0`, `DAT_100449C0`) - whose closed form is exactly
`freq = c5speed * 2^((note-60)/12)`.

Two root-cause bugs were found and fixed during bring-up, both in the mixer:

* the play cursor was stored as an **integer**, so adding the 16.16 fixed-point
  step (e.g. 0.25 samples/frame) truncated to 0 and the cursor never advanced -
  every voice emitted a constant DC sample.  `Voice::posf` now keeps the cursor
  in fixed point, as galaxy does (`v+0x19`/`v+0x27`).
* the panning was scaled twice (once when loading, once in the mixer), which
  pinned pan to hard-left and left the right channel silent.

The showcase therefore plays all 24 songs at normal stereo levels.


* The Impulse-Tracker **pattern** decoder implements the documented IT cell
  encoding; a handful of cells in `TITLE.IT`, `DEFEND.IT` and `MACABRE.IT`
  decode ambiguously (the game's own files predate strict IT 2.15).  The player
  therefore rejects out-of-range commands and clamps `Axx` speed to 1..32, which
  keeps all 24 shipped songs playing correctly but is a deliberate guard rather
  than a byte-exact decode of those few cells.
* No resonance-filter (`S0x`) or NNA/past-note support; none of the shipped
  songs use them.
* `AudioOut` targets Windows (WinMM).  The decoders and the module player are
  platform-independent.
