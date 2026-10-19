<#
  run_pipeline.ps1 - run every static-analysis / extraction stage.

  Stages (fast, stdlib-only Python):
    1. pe_inspect   - header/module sanity check on the key binaries
    2. dpl_index    - import graph + export surface -> re/exports/module_index.json
    3. dfm_extract  - Delphi forms -> assets/dfm/**
    4. ilb_tool     - ILB inventory      -> re/ilb_inventory.json
    5. pfs_probe    - PFS profile        -> re/pfs_probe.json
    6. strings      - localization dump  -> re/mld_strings.txt
    7. report       - HTML summary       -> re/report.html

  Ghidra decompilation is NOT run here (it is slow and needs GHIDRA_HOME);
  use tools/ghidra/run_ghidra.ps1 for that.
#>
param([switch]$SkipHeavy)

$ErrorActionPreference = 'Stop'
$Here = Split-Path -Parent $MyInvocation.MyCommand.Path
$Tools = Join-Path $Here 'tools'
$Re = Join-Path $Here 're'
New-Item -ItemType Directory -Force -Path $Re | Out-Null
$py = 'python'

function Step($name, [scriptblock]$body) {
    Write-Host "`n=== $name ===" -ForegroundColor Cyan
    & $body
    if ($LASTEXITCODE -ne 0) { throw "$name failed ($LASTEXITCODE)" }
}

Push-Location $Tools
try {
    Step '1. PE sanity (key modules)' {
        & $py 'pe_inspect.py' '..\..\AoW.exe' '..\..\AoWEPACK.dpl' '..\..\Enginep.dpl'
    }
    Step '2. Module index (imports/exports/forms)' { & $py 'dpl_index.py' }
    Step '3. Extract Delphi forms' { & $py 'dfm_extract.py' 'AoW.exe' 'AoWEd.exe' }
    Step '4. ILB inventory' { & $py 'ilb_tool.py' }
    Step '5. PFS profile' { & $py 'pfs_probe.py' }
    Step '6. Localization strings' {
        & $py 'strings_tool.py' '--scan-mld' '--min' '8' |
            Out-File (Join-Path $Re 'mld_strings.txt') -Encoding utf8
        Write-Host "wrote re/mld_strings.txt"
    }
    Step '7. HTML report' { & $py 'report.py' }
} finally {
    Pop-Location
}
Write-Host "`nPipeline complete. See $Re" -ForegroundColor Green
