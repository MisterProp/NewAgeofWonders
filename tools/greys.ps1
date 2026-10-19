Add-Type -AssemblyName System.Drawing
$p = $args[0]
$b = [System.Drawing.Bitmap]::FromFile((Resolve-Path $p).Path)
$W=$b.Width; $H=$b.Height
$hist = @{}
for ($y=0; $y -lt $H; $y+=2) {
  for ($x=0; $x -lt $W; $x+=2) {
    $c=$b.GetPixel($x,$y)
    $mx=[math]::Max($c.R,[math]::Max($c.G,$c.B)); $mn=[math]::Min($c.R,[math]::Min($c.G,$c.B))
    if (($mx-$mn) -gt 25) { continue }          # only low-saturation (greys)
    $k = '{0:x2}{1:x2}{2:x2}' -f $c.R,$c.G,$c.B
    if ($hist.ContainsKey($k)) { $hist[$k]++ } else { $hist[$k]=1 }
  }
}
$b.Dispose()
Write-Host ("{0}: low-saturation greys, top 14" -f (Split-Path $p -Leaf))
$hist.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 14 | ForEach-Object {
  $r=[convert]::ToInt32($_.Key.Substring(0,2),16); $g=[convert]::ToInt32($_.Key.Substring(2,2),16); $bl=[convert]::ToInt32($_.Key.Substring(4,2),16)
  Write-Host ("  #{0} rgb({1,3},{2,3},{3,3}) bright={4,3} n={5}" -f $_.Key,$r,$g,$bl,[int](($r+$g+$bl)/3),$_.Value)
}
