<#
  run_ghidra.ps1 - drive Ghidra headless over every AoW1 binary.

  Requirements (see docs/02_TOOLING.md):
    * Ghidra 11.x with a JDK 17+ (set $env:GHIDRA_HOME or add to PATH)
    * set $env:AOW_OUT to the export folder (default: port/re/exports)

  Examples:
    ./run_ghidra.ps1                 # all exe + .dpl targets
    ./run_ghidra.ps1 AoWEPACK        # only modules matching a name
    ./run_ghidra.ps1 -Big            # include the 2 MB+ modules

  The first pass over AoWEPACK.dpl (10k functions) can take 20-60 min.
#>
param(
    [string[]]$Name = @(),
    [switch]$Big,
    [int]$TimeoutSec = 3600
)

$ErrorActionPreference = 'Stop'
$Here      = Split-Path -Parent $MyInvocation.MyCommand.Path
$PortRoot  = Resolve-Path (Join-Path $Here '..\..')
$GameRoot  = Resolve-Path (Join-Path $PortRoot '..')
$ScriptDir = $Here
$OutRoot   = if ($env:AOW_OUT) { $env:AOW_OUT } else { Join-Path $PortRoot 're\exports' }
$ProjDir   = Join-Path $PortRoot 're\ghidra_proj'
New-Item -ItemType Directory -Force -Path $OutRoot, $ProjDir | Out-Null
$env:AOW_OUT = $OutRoot

function Find-Ghidra {
    if ($env:GHIDRA_HOME -and (Test-Path (Join-Path $env:GHIDRA_HOME 'support\analyzeHeadless.bat'))) {
        return (Join-Path $env:GHIDRA_HOME 'support\analyzeHeadless.bat')
    }
    $cmd = Get-Command analyzeHeadless* -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    throw "analyzeHeadless not found. Set `$env:GHIDRA_HOME to your Ghidra install."
}

$headless = Find-Ghidra
Write-Host "Ghidra headless: $headless"
Write-Host "Export root    : $OutRoot"

# Collect targets (Delphi engine modules first - they hold the game logic).
$targets = Get-ChildItem -Path $GameRoot -Recurse -File |
    Where-Object { $_.Extension -in '.exe', '.dpl', '.dll' } |
    Where-Object { $_.FullName -notlike "*$PortRoot*" }
if ($Name.Count -gt 0) {
    $targets = $targets | Where-Object { $n = $_.BaseName; $Name | Where-Object { $n -like "*$_*" } }
}
if (-not $Big) {
    $targets = $targets | Where-Object { $_.Length -lt 2MB -or $_.BaseName -eq 'AoWEPACK' }
}

# Priority order: core engine -> game logic -> shell/editor -> network.
$order = 'AoWEPACK','Enginep','HSEPack','ILPACK','Gfxepack','SoundP','Localize','aowInt','AoWInterface','AoWTCPCK','AOWTools','DCPACK','Network','aowFXpck','VideoP','ims','AoW','AoWEd'
$targets = $targets | Sort-Object @{ Expression = { $i = [array]::IndexOf($order, $_.BaseName); if ($i -lt 0) { 999 } else { $i } } }

foreach ($t in $targets) {
    Write-Host ("`n==== Importing {0} ({1:N0} bytes) ====" -f $t.Name, $t.Length)
    & $headless $ProjDir aow -import $t.FullName `
        -scriptPath $ScriptDir `
        -postScript DecompileAll.java `
        -analysisTimeoutPerFile $TimeoutSec `
        -deleteProject
    if ($LASTEXITCODE -ne 0) { Write-Warning "Ghidra returned $LASTEXITCODE for $($t.Name)" }
}
Write-Host "`nAll done. Results under $OutRoot"
