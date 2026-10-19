Add-Type -AssemblyName System.Drawing
$root = 'C:\GOG Games\Age of Wonders'
$outd = "$root\port\re"
$L = New-Object System.Collections.Generic.List[string]
function Say([string]$s){ $L.Add($s) | Out-Null }

function Load-Img([string]$p) {
  $b = New-Object System.Drawing.Bitmap $p
  $w = $b.Width; $h = $b.Height
  $rect = New-Object System.Drawing.Rectangle(0, 0, $w, $h)
  $r = $b.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $st = $r.Stride
  $d = New-Object byte[] ($st * $h)
  [System.Runtime.InteropServices.Marshal]::Copy($r.Scan0, $d, 0, $d.Length)
  $b.UnlockBits($r); $b.Dispose()
  return @{ w = $w; h = $h; st = $st; d = $d }
}

function Save-Crop($img, [string]$dst, [int]$x0, [int]$y0, [int]$w, [int]$h, [int]$z) {
  $nb = New-Object System.Drawing.Bitmap ($w * $z), ($h * $z)
  for ($y = 0; $y -lt $h; $y++) {
    for ($x = 0; $x -lt $w; $x++) {
      $i = ($y0 + $y) * $img.st + ($x0 + $x) * 4
      $c = [System.Drawing.Color]::FromArgb($img.d[$i+3], $img.d[$i+2], $img.d[$i+1], $img.d[$i])
      for ($yy = 0; $yy -lt $z; $yy++) { for ($xx = 0; $xx -lt $z; $xx++) { $nb.SetPixel($x*$z+$xx, $y*$z+$yy, $c) } }
    }
  }
  $nb.Save($dst, [System.Drawing.Imaging.ImageFormat]::Png); $nb.Dispose()
}

function Save-Small([string]$src, [string]$dst, [int]$maxW) {
  $b = New-Object System.Drawing.Bitmap $src
  $sc = $maxW / [double]$b.Width
  $nw = [int]($b.Width * $sc); $nh = [int]($b.Height * $sc)
  $nb = New-Object System.Drawing.Bitmap $nw, $nh
  $g = [System.Drawing.Graphics]::FromImage($nb)
  $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
  $g.DrawImage($b, 0, 0, $nw, $nh); $g.Dispose()
  $nb.Save($dst, [System.Drawing.Imaging.ImageFormat]::Png); $nb.Dispose(); $b.Dispose()
}

# ---- 1. identity + colour census of every candidate render ------------------
$files = @(
  @{ n = 'after_tut'; p = "$outd\after_tut.png" },
  @{ n = 'cur_tut';   p = "$root\port\bin\cur_tut.png" },
  @{ n = 'golden';    p = "$outd\golden\Tutorial_L0.png" },
  @{ n = 'howsb';     p = "$root\HowItShouldBe.png" }
)
foreach ($f in $files) {
  if (-not (Test-Path $f.p)) { Say ("== {0}  MISSING {1}" -f $f.n, $f.p); continue }
  $im = Load-Img $f.p
  Say ("== {0}  {1}x{2}" -f $f.n, $im.w, $im.h)
  $hist = @{}
  for ($y = 0; $y -lt $im.h; $y += 3) {
    $row = $y * $im.st
    for ($x = 0; $x -lt $im.w; $x += 3) {
      $i = $row + $x * 4
      $k = ("{0},{1},{2}" -f ($im.d[$i+2] -shr 4), ($im.d[$i+1] -shr 4), ($im.d[$i] -shr 4))
      if ($hist.ContainsKey($k)) { $hist[$k]++ } else { $hist[$k] = 1 }
    }
  }
  $top = $hist.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 6
  foreach ($t in $top) { Say ("   rgb(hi-nib) {0,-10} x{1}" -f $t.Key, $t.Value) }
  $im = $null
}

# ---- 2. land/water geometry of the port render ------------------------------
# water := blue dominant (b >= r and b >= g); everything else is land.
function Scan($im) {
  $w = $im.w; $h = $im.h; $st = $im.st; $d = $im.d
  $minx = $w; $maxx = -1; $miny = $h; $maxy = -1
  $top = New-Object 'int[]' $w
  for ($x = 0; $x -lt $w; $x++) { $top[$x] = -1 }
  for ($y = 0; $y -lt $h; $y++) {
    $row = $y * $st
    for ($x = 0; $x -lt $w; $x++) {
      $i = $row + $x * 4
      $b = $d[$i]; $g = $d[$i+1]; $r = $d[$i+2]
      if ($b -ge $r -and $b -ge $g) { continue }   # water
      if ($top[$x] -lt 0) { $top[$x] = $y }
      if ($x -lt $minx) { $minx = $x }; if ($x -gt $maxx) { $maxx = $x }
      if ($y -lt $miny) { $miny = $y }; if ($y -gt $maxy) { $maxy = $y }
    }
  }
  return @{ minx = $minx; maxx = $maxx; miny = $miny; maxy = $maxy; top = $top }
}

foreach ($f in @(@{ n = 'cur_tut'; p = "$root\port\bin\cur_tut.png" })) {
  $im = Load-Img $f.p
  $s = Scan $im
  Say ''
  Say ("-- {0}: land bbox = ({1},{2})-({3},{4})" -f $f.n, $s.minx, $s.miny, $s.maxx, $s.maxy)
  # run-length encode top[x] across the land span -> reveals hexagon flat-top pitch
  $runs = New-Object System.Collections.Generic.List[string]
  $cx = $s.minx; $prev = $s.top[$s.minx]; $len = 0
  $first = New-Object System.Collections.Generic.List[int]
  for ($x = $s.minx; $x -le $s.maxx; $x++) {
    $v = $s.top[$x]
    if ($v -eq $prev) { $len++ } else { $runs.Add(("y={0} len={1}" -f $prev, $len)); $prev = $v; $len = 1 }
  }
  $runs.Add(("y={0} len={1}" -f $prev, $len))
  Say ("   coast top RLE (first 40): " + (($runs | Select-Object -First 40) -join '  |  '))
  # ASCII mask of the top-left land corner at 4px sampling
  $x0 = [Math]::Max(0, $s.minx - 8); $y0 = [Math]::Max(0, $s.miny - 8)
  Say ("   ASCII mask @ ({0},{1}) 1 char = 4px  ('#'=land '.'=water)" -f $x0, $y0)
  for ($y = $y0; $y -lt [Math]::Min($im.h, $y0 + 128); $y += 4) {
    $line = ''
    for ($x = $x0; $x -lt [Math]::Min($im.w, $x0 + 192); $x += 4) {
      $t = $s.top[$x]
      if ($t -ge 0 -and $t -le $y) { $line += '#' } else { $line += '.' }
    }
    Say ("     " + $line)
  }
  Save-Crop $im "$outd\_geo_hex.png" $x0 $y0 192 128 3
  $im = $null
}

Save-Small "$root\port\bin\cur_tut.png" "$outd\_geo_small.png" 720
Save-Small "$root\HowItShouldBe.png"    "$outd\_geo_howsb.png" 720

$L -join "`r`n" | Set-Content -Encoding UTF8 "$outd\_geo.out.txt"
Write-Host "wrote $outd\_geo.out.txt"
