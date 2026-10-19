# 04 - Driving the agent (Cline) through the workflow

This harness is built so you and the agent can run it **one stage at a time**.
Paste the prompts below (one per session/stage) and let the agent work inside
`port/`. Adjust the stage number to match `docs/03_ROADMAP.md`.

## Ground rules to give the agent

* Work only under `port/`; treat the game folder as **read-only input**.
* Prefer `tools/` scripts over ad-hoc code; add a script if one is missing.
* After any change, **run** the affected tool/test and paste the output.
* Record every new format fact in `docs/01_FORMATS.md` and every binary fact in
  `docs/00_FINDINGS.md`.
* No placeholder code in `src/` - if something is unresolved, mark it `TODO(RE)`
  and open an entry in `docs/03_ROADMAP.md`.

## Stage prompts

**Stage 0 - refresh the harness**
> Read port/README.md and port/docs/00-01. Re-run port/run_pipeline.ps1, then
> build the C++ skeleton. Report any tool that fails and fix it.

**Stage 1 - crack the .PFS rule tables**
> Target Release/Ability.pfs first (30 KB). Use tools/strings_tool.py to list its
> strings in file order, correlate them with the binary records, and deduce the
> record schema. Then write src/include/aow/pfs.hpp and a loader that parses it.
> Add a test that asserts the parsed ability count and each ability's name.
> Update docs/01_FORMATS.md with the schema you find. Do the same, iteratively,
> for Spells.pfs, ITEMS.pfs, HEROES.PFS, Unitres.pfs.

**Stage 1b - decode ILB pixels**
> Finish the ILB directory record (tools/ilb_tool.py has the header; the per
> entry layout is TODO). Add frame decode -> PNG under tools/ilb_decode.py and
> validate one output against the shipped ILBEditor. Then port it to C++.

**Stage 2 - object model**
> Install/point to Ghidra per docs/02_TOOLING.md, run
> tools/ghidra/run_ghidra.ps1 for Enginep + AoWEPACK, then work through
> re/exports/ to identify the core object types. Maintain re/symbols/renames.csv
> and (once present) tools/ghidra/import_renames.py.

**Stage 2/3 - port systems**
> Implement <system> in src/engine/<system>./ so it consumes the parsed data and
> can be unit-tested deterministically. No platform code in this layer.

## How to keep it honest

* Ask for a **demo command** for each stage (e.g. `aow pfs Ability.pfs`).
* Ask the agent to show the **test output**, not just the code.
* If the agent claims a format fact, require the file+offset evidence in docs.
* Snapshot progress with git (`git init` inside `port/` is fine) so each stage is
  a reviewable commit.

## Good first session

> Set up the harness (run_pipeline.ps1 + build.ps1), then produce a single HTML
> report (port/re/report.html) summarizing: module import graph, export counts,
> extracted form count, ILB/ILB totals, and the PFS string samples. Commit it.
