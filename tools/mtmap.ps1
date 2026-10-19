# mtmap.ps1 - cross-reference each map's mountain res_types against its terrain.
# Usage: powershell -File mtmap.ps1 <game-root> <map1> <map2> ...
param([string]$Root = '..', [int]$Max = 14)
$port = Split-Path -Parent $MyInvocation.MyCommand.Path | Split-Path -Parent
Set-Location $port
$Maps = @(
  "$Root\Scenario\Custom\Tutorial.hsm",
  "$Root\Scenario\Campaign\Dwarf1.CSM",
  "$Root\Scenario\Custom\AoWx - Desolate Maugogom.hsm",
  "$Root\Scenario\Custom\AoWx - Frosty Lagoon.hsm",
  "$Root\Scenario\Custom\AoWx - Green Valley.hsm",
  "$Root\Scenario\Custom\AoWx - Germany.hsm",
  "$Root\Scenario\Custom\AoWx - Blood Isles.hsm",
  "$Root\Scenario\Custom\AoWx - King of the Hill.hsm",
  "$Root\Scenario\Custom\AoWx - Lord of the Rings.hsm",
  "$Root\Scenario\Custom\AoWx - Kolusurt Caldera.hsm",
  "$Root\Scenario\Custom\AoWx - Icedale.hsm",
  "$Root\Scenario\Custom\AoWx - Frozen Waters of Seshedut.hsm",
  "$Root\Scenario\Custom\AoWx - Honeyvale Falls.hsm",
  "$Root\Scenario\Custom\AoWx - Blackwood Forest.hsm"
)

# id -> (type, index) for Mountain.TMountainMOR, from `aow mapres`.
$types = @{}
& .\bin\aow.exe mapres "$Root\Release\Release.hss" 0x20169 | ForEach-Object {
  if ($_ -match 'id=0x([0-9a-f]{8})\s+type=(\d+)\s+idx=(\d+)') {
    $types[[uint32]("0x" + $matches[1])] = @{ t = [int]$matches[2]; i = [int]$matches[3] }
  }
}

# shape signature = the (dx,dy) shift triple for idx 0,1,2, from the dump above.
$shape = @{}

foreach ($m in $Maps) {
  $csv = Join-Path $env:TEMP ("mt_" + [IO.Path]::GetFileNameWithoutExtension($m) + ".csv")
  & .\bin\aow.exe mapviz $m "$env:TEMP\mt.png" --res "$Root\Release\Release.hss" `
      --root $Root --level 0 --debug-csv $csv *> $null
  if (-not (Test-Path $csv)) { Write-Host "  (skip $m)"; continue }
  $rows = Import-Csv $csv
  $terr = @{}; $mt = @{}
  foreach ($r in $rows) {
    if ($r.layer -ne 'terrain' -and $r.layer -ne 'object') { continue }
    if ($r.ilb -match 'MOUNTAIN\\MOUNTAIN') {
      $id = [uint32]$r.resource
      $k = if ($types.ContainsKey($id)) { "type$($types[$id].t)" } else { "id=$id" }
      if ($mt.ContainsKey($k)) { $mt[$k]++ } else { $mt[$k] = 1 }
    } elseif ($r.layer -eq 'terrain') {
      if ($terr.ContainsKey($r.ilb)) { $terr[$r.ilb]++ } else { $terr[$r.ilb] = 1 }
    }
  }
  Remove-Item $csv -Force -ErrorAction SilentlyContinue
  Write-Host ("== {0}" -f (Split-Path $m -Leaf))
  Write-Host ("   terrain: " + (($terr.GetEnumerator() | Sort-Object Value -Descending |
      Select-Object -First 8 | ForEach-Object { "$($_.Name)=$($_.Value)" }) -join ' '))
  Write-Host ("   mountain types: " + (($mt.GetEnumerator() | Sort-Object Value -Descending |
      ForEach-Object { "$($_.Name)=$($_.Value)" }) -join ' '))
}
