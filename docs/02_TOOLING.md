# 02 - Tooling

## Already present on this machine

* Python 3.10 (`C:\Users\Utente\AppData\Local\Programs\Python\Python310`) - the
  `tools/*.py` scripts use **stdlib only** (no pip install required).
* g++ 15.2 via MSYS2 (`C:\msys64\ucrt64\bin\g++.exe`) - builds the C++ skeleton.
* git.

## Install these to unlock the deeper stages

### 1. Java JDK 21 (required by Ghidra)
https://adoptium.net/  - install "Temurin 21 (LTS)", tick "Set JAVA_HOME".

### 2. Ghidra 11.x (primary decompiler)
1. Download: https://github.com/NationalSecurityAgency/ghidra/releases
2. Unzip to e.g. `C:\ghidra_11.1_PUBLIC`.
3. Set the env var so the wrapper finds it:

```powershell
setx GHIDRA_HOME "C:\ghidra_11.1_PUBLIC"
```

Ghidra's generic decompiler handles the **code** well. For Delphi-specific
structure also consider:

### 3. Interactive Delphi Reconstructor (IDR) - **strongly recommended**
Search "IDR Interactive Delphi Reconstructor" (by Crypto). IDR understands
Borland VMTs, published-method tables and DFM units, and produces *named*
Delphi-ish pseudo-source. Run it on `AoWEPACK.dpl`, `Enginep.dpl`, `HSEPack.dpl`
and export the `.idc`/text output into `re/exports/`. This is the single biggest
lever for understanding the engine's classes.

### 4. (optional) FLIRT / signature kits for Ghidra
To collapse Borland runtime/library functions into named ones, apply a Borland
Delphi signature set (e.g. from the Ghidra `Delphi`/`Borland` FID packs or a
community `.fidb`). Cuts noise dramatically.

### 5. (optional) Asset tooling already shipped with the game
* `IlbMaker_Release_1.0.2.7/` - community ILB packer (uses `nconvert`).
* `ILBEditor/` - wxWidgets ILB viewer.
Both are useful to **validate** your own ILB reader against known-good output.

## Python dependencies

None beyond the standard library. If you later want PNG output while decoding
ILBs, add `pillow` (`pip install pillow`); it is deliberately not required.

## Environment variables used by the harness

| Var | Used by | Purpose |
|-----|---------|---------|
| `GHIDRA_HOME` | `tools/ghidra/run_ghidra.ps1` | locate `analyzeHeadless.bat` |
| `AOW_OUT` | `DecompileAll.java` / wrapper | export folder (default `port/re/exports`) |

## Verify your setup

```powershell
python --version
g++ --version
& "$env:GHIDRA_HOME\support\analyzeHeadless.bat" 2>&1 | Select-Object -First 3
```

## Verify GhidraMCP (optional)

[GhidraMCP](https://github.com/LaurieWired/GhidraMCP) runs an HTTP server inside
Ghidra (default `http://127.0.0.1:8080`) and an optional MCP/SSE bridge (default
`http://127.0.0.1:8081/sse`). Check whether either is answering with the
stdlib-only smoke test (no pip installs):

```powershell
python tools/ghidra/check_mcp.py                  # probe API + SSE bridge
python tools/ghidra/check_mcp.py --decompile      # also POST a test decompile
python tools/ghidra/check_mcp.py --json           # machine-readable report
```

It exits `0` only when the required read-only endpoints answer (so it can gate a
workflow); otherwise it prints a checklist of what to fix. It never calls a
mutating endpoint such as `renameFunction`.

## GhidraMCP helper scripts

`tools/ghidra/` wraps the bridge so the agent (or a human) can drive it from a
shell. `check_mcp.py` targets port 8080 by default; the others talk to the port
the plugin actually listens on (this install uses **8089**).

| Script | Purpose |
|--------|---------|
| `mcp.py <out> <endpoint> [k=v ...]` | call one endpoint, write the body to a file (never trust the console encoding) |
| `mcp_call.py <endpoint> [query]` | same, but takes a raw query string - needed for parameters that clash with the script's own argv (e.g. `path=`) |
| `probe_endpoints.py` | which routes this plugin build exposes (and which switch the current program) |
| `probe_params.py <endpoint> <value>` | brute-force the query-parameter name an endpoint wants |
| `dump_all.py [outdir]` | `/list_functions` for **every** program open in the project -> `re/exports/<name>.funcs.txt` |
| `decompile.py <program> <outdir> <fn...>` | batch-decompile (UTF-8 output, so it is readable afterwards) |
| `grep_list.py <file> <regex> [--units]` | grep a UTF-16/UTF-8 symbol dump |
| `dump_methods.py` | page `/list_methods` (as found: prefer `/list_functions`) |

## Local PE / disassembly helpers (no Ghidra needed)

The Ghidra project does not turn every RTTI-reachable routine into a function
(the title-screen handlers are reached only through the published-method table),
so a few stdlib-only helpers read the PE directly:

| Script | Purpose |
|--------|---------|
| `vmt_scan.py <module> [--children C]` | walk the **Delphi class hierarchy** out of a PE: a single pass finds every VMT (`vmtSelfPtr` holds `ownVa + 64`), then the parent chain (`vmt-36` -> `parentVmt-76`) and the class names (`vmt-44` -> a ShortString) give the real inheritance tree. This is how the port established which structures are *ownable* (docs/19_STRUCTURE_FLAGS.md) |
| `pe_va.py <pe> <off>...` | file offset <-> virtual address, from the section table |
| `hexdump_va.py <pe> --file/--va <addr> --len N` | byte dump around an offset/VA. Deliberately short bodies are often the whole proof: `--va 0x557a2dc8` is `8b 40 08 / 8b 40 28 / e8 .. / 84 c0 / 7c 02 / 33 c0 / c3`, i.e. `Mountain.TMountainMO.GetOverlay` returns **0 (mountains)** for every hex it covers (docs/15_MOVEMENT.md §1) |
| `blobfind.py <needle>...` | find ASCII/UTF-16 strings in every game binary |
| `find_ref.py <pe> --va <addr>` / `--hex <bytes>` | find the code that references a VA, or raw instruction patterns |
| `disasm.py <pe> --func <va>` | disassemble a routine with Capstone (dev-only: `pip install capstone`) |
| `ilb_names.py <ilb>` | entry names of one ILB from `re/ilb_inventory.json` |
| `scan_exports.py` / `scan_consts.py` | grep persisted export lists / scan for layout constants |
| `pe_rsrc_strings.py <pe>` | dump RT_STRING tables (Delphi `resourcestring`) |
| `png_colors.py <png>` | read a PNG the port wrote (stored-DEFLATE) and list its colours/alpha |
| `filter_imports.py` | pick IAT slots out of a saved `/list_imports` dump |

`disasm.py` is the only tool needing a pip package; nothing under `port/src`
depends on it. `pip install capstone` once and it works offline afterwards.

It exits `0` only when the required read-only endpoints answer (so it can gate a
workflow); otherwise it prints a checklist of what to fix. It never calls a
mutating endpoint such as `renameFunction`, but the helper scripts above do call
`/create_function`-style routes where the plugin exposes them.


