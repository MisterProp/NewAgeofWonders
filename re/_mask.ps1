Add-Type -AssemblyName System.Drawing
$root = 'C:\GOG Games\Age of Wonders\port'
foreach ($rel in @('bin\_hgr0.png','bin\_hgr0_big.png')) {
  $src = Join-Path $root $rel
  if (-not (Test-Path $src)) { Write-Host "MISSING $src"; continue }
  $b = New-Object System.Drawing.Bitmap $src
  Write-Host ("== {0}  {1}x{2}" -f $rel, $b.Width, $b.Height)
  $sx = [Math]::Max(1, [int]($b.Width / 48))
  $sy = [Math]::Max(1, [int]($b.Height / 32))
  for ($y = 0; $y -lt $b.Height; $y += $sy) {
    $line = ''
    for ($x = 0; $x -lt $b.Width; $x += $sx) {
      $p = $b.GetPixel($x, $y)
      # magenta/white = transparent corner, else terrain
      $transparent = (($p.R -gt 200) -and ($p.G -lt 120) -and ($p.B -gt 200)) -or
                     (($p.R -gt 245) -and ($p.G -gt 245) -and ($p.B -gt 245))
      $line += $(if ($transparent) { '.' } else { '#' })
    }
    Write-Host $line
  }
  $b.Dispose()
}
