# 00 - Findings (verified against this install)

Everything here was measured with `tools/pe_inspect.py` and `tools/dpl_index.py`,
not guessed.

## Toolchain / build fingerprint

| Property | Value | Meaning |
|----------|-------|---------|
| Machine | `0x14c` | 32-bit x86 (i386) |
| Subsystem | `2` | Windows GUI |
| TimeDateStamp | `0x2A425E19` | The canonical **Delphi/Borland** stamp |
| Characteristics | `0x818E` (exe) / `0xA18E` (dpl) | Borland linker flags |
| Sections | `CODE, DATA, BSS, .idata, .reloc, .rsrc` | Delphi naming |

Conclusion: **the game is compiled with Delphi (Object Pascal)**, not C/C++.
Every `.exe` and `.dpl` shares this fingerprint.

## The engine is a set of Delphi runtime packages

`AoW.exe` imports these modules (from its import table):

```
VCL30.dpl     - Borland Visual Component Library (the GUI toolkit)
AoWEPACK.dpl  - main game logic        (10,036 exports -> the biggest surface)
Enginep.dpl   - core engine            (1,035 exports)
HSEPack.dpl   - HSE engine layer       (1,040 exports)
ILPACK.dpl    - image library / ILB    (785 exports)
Gfxepack.dpl  - graphics backend
SoundP.dpl    - audio backend
Localize.dpl  - localized strings      (1,258 exports)
aowInt.dpl    - game state / scoring   (558 exports)
AoWInterface.dpl - UI screens          (449 exports)
AoWTCPCK.dpl  - TCP/IP networking      (739 exports)
AOWTools.dpl  - misc tools             (123 exports)
DCPACK.dpl    - data container         (226 exports)
Network.dpl   - networking             (199 exports)
aowFXpck.dpl  - special effects
VideoP.dpl    - video playback
ims.dpl       - input manager          (691 exports)
aowDPlay.dpl  - DirectPlay support     (133 exports)
```

`AoWEd.exe` (the map/scenario editor) links a similar set plus `ILToolsP.dpl`
and `VCLADDON.dpl`.

## Class/name recovery is possible

* **163 Delphi forms** are embedded as `TPF0` resources and have been extracted
  to `assets/dfm/`. Names are meaningful (`TCITYSCREEN`, `TFASTCOMBATWINDOW`,
  `TARENADLG`, `TCAMPAIGNTRANSFERDLG`, ...), which hands you the entire screen
  inventory and their widget layout.
* Exports are Borland-mangled but decodable (e.g.
  `@Unit@Method$qqr...`), and Delphi's published-method/RTTI tables survive in
  the binary, so a Delphi-aware tool (IDR) can recover *unit.class.method* names.

## Data is king

| Format | Count | Content sampled |
|--------|-------|-----------------|
| `.PFS` | 12 | `Ability.pfs` = ability descriptions, `Spells.pfs` = spell text, `ITEMS.pfs`/`HEROES.PFS` = names, `TEXT.pfs` = campaign prose, `Unitres.pfs` = unit graphics refs |
| `.ILB` | 538 | sprite banks (`UnitFr.BMP`, `LoadIcons.BMP`, terrain, effects, UI) |
| `.MLD` | 8 | localized strings, 5+ languages (EN/DE/FR/IT/ES) |
| `.WAV` / `.IT` | 216 / 24 | sound FX and Impulse-Tracker music |
| `.HSM` / `.CSM` | 90 / 41 | scenario / campaign maps |

(Counts are for this original AoW1 install; earlier drafts, written while the
project targeted the Age of Wonders X mod, listed larger numbers.)

`tools/pfs_probe.py` already pulls readable strings (e.g. "Allows the unit to
fly over and across terrain...") proving the rule data is plain, unencrypted.

## What this means for the port

1. **Don't wait for a "full decompile".** Start the port now from the data
   formats; use the decompiler output as reference while implementing systems.
2. **Reuse VCL semantics sparingly.** The UI is Win32 + VCL widgets; either
   emulate the widget tree from the DFMs, or (recommended) re-implement screens
   natively from the extracted layouts and step into your own widget toolkit.
3. **Target modern but faithful:** SDL2/SFML for platform+render, keep the exact
   turn-based rules fed by the original `.PFS` data.
