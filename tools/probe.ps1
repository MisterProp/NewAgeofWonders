Add-Type -AssemblyName System.Drawing
$p = $args[0]
$b = [System.Drawing.Bitmap]::FromFile((Resolve-Path $p).Path)
$W=$b.Width; $H=$b.Height
Write-Host ("{0} {1}x{2}" -f (Split-Path $p -Leaf), $W, $H)
foreach ($frac in 0.05,0.2,0.5,0.8,0.95) {
  $y = [int]($H * $frac)
  $runs = New-Object System.Collections.Generic.List[string]
  $prev = ''
  $start = 0
  for ($x=0; $x -lt $W; $x++) {
    $c = $b.GetPixel($x,$y)
    $k = '{0:x2}{1:x2}{2:x2}' -f $c.R,$c.G,$c.B
    if ($k -ne $prev) {
      if ($prev -ne '') { $runs.Add(("{0}-{1}:{2}" -f $start,($x-1),$prev)) }
      $prev = $k; $start = $x
    }
  }
  $runs.Add(("{0}-{1}:{2}" -f $start,($W-1),$prev))
  if ($runs.Count -gt 24) { $runs = $runs[0..23] }
  Write-Host ("y={0}: {1}" -f $y, ($runs -join '  '))
}
$b.Dispose()
