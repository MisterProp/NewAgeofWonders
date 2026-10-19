Add-Type -AssemblyName System.Drawing
function DarkFrac([string]$p) {
  $b = New-Object System.Drawing.Bitmap ((Resolve-Path $p).Path)
  $W=$b.Width; $H=$b.Height
  $dark=0; $n=0
  for ($y=0; $y -lt $H; $y+=4) {
    for ($x=0; $x -lt $W; $x+=4) {
      $c=$b.GetPixel($x,$y); $n++
      $mx=[math]::Max($c.R,[math]::Max($c.G,$c.B))
      if ($mx -le 45) { $dark++ }
    }
  }
  $b.Dispose()
  return @{ pct = (100.0*$dark/$n); n=$n }
}
foreach ($f in $args) {
  $r = DarkFrac $f
  Write-Host ("{0,-22} dark(max<=45) = {1,6:N2}% of {2} samples" -f (Split-Path $f -Leaf), $r.pct, $r.n)
}
