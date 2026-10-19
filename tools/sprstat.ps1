Add-Type -AssemblyName System.Drawing
foreach ($f in $args) {
  $b = New-Object System.Drawing.Bitmap ((Resolve-Path $f).Path)
  $W=$b.Width; $H=$b.Height
  $sr=0.0;$sg=0.0;$sb=0.0;$n=0;$op=0
  for ($y=0; $y -lt $H; $y++) {
    for ($x=0; $x -lt $W; $x++) {
      $c=$b.GetPixel($x,$y); $n++
      if ($c.A -lt 32) { continue }
      $op++; $sr+=$c.R; $sg+=$c.G; $sb+=$c.B
    }
  }
  $b.Dispose()
  if ($op -eq 0) { Write-Host ("{0,-14} (all transparent)" -f (Split-Path $f -Leaf)); continue }
  Write-Host ("{0,-14} {1,4}x{2,-4} opaque={3,6:N1}%  mean=({4,3:N0},{5,3:N0},{6,3:N0})" -f `
    (Split-Path $f -Leaf), $W, $H, (100.0*$op/$n), ($sr/$op), ($sg/$op), ($sb/$op))
}
