Add-Type -AssemblyName System.Drawing
$outd = 'C:\GOG Games\Age of Wonders\port\re'
$script:L = New-Object System.Collections.Generic.List[string]
function Say([string]$s){ $script:L.Add($s) | Out-Null }

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

# Frame-agnostic normaliser: area-average downsample any image to an N x N grid
# (bilinear via GDI+), returning R/G/B samples 0..255. No assumption is made
# about either image's pixel pitch, so it can compare the port render against a
# reference captured at a different zoom/crop.
function Small([string]$p, [int]$n) {
  $b = New-Object System.Drawing.Bitmap $p
  $nb = New-Object System.Drawing.Bitmap $n, $n
  $g = [System.Drawing.Graphics]::FromImage($nb)
  $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
  $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
  $g.DrawImage($b, 0, 0, $n, $n); $g.Dispose(); $b.Dispose()
  $rect = New-Object System.Drawing.Rectangle(0, 0, $n, $n)
  $r = $nb.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $st = $r.Stride
  $d = New-Object byte[] ($st * $n)
  [System.Runtime.InteropServices.Marshal]::Copy($r.Scan0, $d, 0, $d.Length)
  $nb.UnlockBits($r); $nb.Dispose()
  $R = New-Object 'double[]' ($n * $n)
  $G = New-Object 'double[]' ($n * $n)
  $B = New-Object 'double[]' ($n * $n)
  for ($y = 0; $y -lt $n; $y++) {
    for ($x = 0; $x -lt $n; $x++) {
      $i = $y * $st + $x * 4
      $k = $y * $n + $x
      $B[$k] = $d[$i]; $G[$k] = $d[$i+1]; $R[$k] = $d[$i+2]
    }
  }
  return @{ n = $n; r = $R; g = $G; b = $B }
}

function MapIdx([int]$t, [int]$x, [int]$y, [int]$n) {
  switch ($t) {
    0 { return @($x, $y) }
    1 { return @(($n-1-$x), $y) }
    2 { return @($x, ($n-1-$y)) }
    3 { return @(($n-1-$x), ($n-1-$y)) }
    4 { return @($y, $x) }
    5 { return @(($n-1-$y), $x) }
    6 { return @($y, ($n-1-$x)) }
    7 { return @(($n-1-$y), ($n-1-$x)) }
  }
}
$script:names = @('identity','mirror-x','mirror-y','rot180','transpose','rot90cw','rot90ccw','antitranspose')

function Compare-Grid($A, $B, [string]$tag) {
  $n = $A.n
  Say ("-- {0} (A = first image, B = second) --" -f $tag)
  # land := not blue-dominant (same rule as _geo.ps1's water test)
  $la = New-Object 'bool[]' ($n * $n)
  $lb = New-Object 'bool[]' ($n * $n)
  for ($k = 0; $k -lt $n * $n; $k++) {
    $la[$k] = -not ($A.b[$k] -ge $A.r[$k] -and $A.b[$k] -ge $A.g[$k])
    $lb[$k] = -not ($B.b[$k] -ge $B.r[$k] -and $B.b[$k] -ge $B.g[$k])
  }
  $best = ''; $bestErr = [double]::MaxValue
  for ($t = 0; $t -lt 8; $t++) {
    $sum = 0.0; $agree = 0
    for ($y = 0; $y -lt $n; $y++) {
      for ($x = 0; $x -lt $n; $x++) {
        $ij = MapIdx $t $x $y $n
        $ka = $y * $n + $x
        $kb = $ij[1] * $n + $ij[0]
        $sum += [math]::Abs($A.r[$ka] - $B.r[$kb]) + `
                [math]::Abs($A.g[$ka] - $B.g[$kb]) + `
                [math]::Abs($A.b[$ka] - $B.b[$kb])
        if ($la[$ka] -eq $lb[$kb]) { $agree++ }
      }
    }
    $err = $sum / ($n * $n * 3.0)
    $ag = 100.0 * $agree / ($n * $n)
    Say ("   {0,-14} colourErr={1,7:N1}  landAgree={2,5:N1}%" -f $script:names[$t], $err, $ag)
    if ($err -lt $bestErr) { $bestErr = $err; $best = $script:names[$t] }
  }
  Say ("   => best orientation: {0} (colourErr={1:N1})" -f $best, $bestErr)
  Say ''
}

# --- reference framing helpers --------------------------------------------
# bbox of the content that is not the flat dark margin/backdrop (max channel>60)
function ContentBox([string]$p) {
  $img = Load-Img $p
  $w = $img.w; $h = $img.h; $st = $img.st; $d = $img.d
  $minx = $w; $maxx = -1; $miny = $h; $maxy = -1
  for ($y = 0; $y -lt $h; $y += 2) {
    $row = $y * $st
    for ($x = 0; $x -lt $w; $x += 2) {
      $i = $row + $x * 4
      $mx = $d[$i]
      if ($d[$i+1] -gt $mx) { $mx = $d[$i+1] }
      if ($d[$i+2] -gt $mx) { $mx = $d[$i+2] }
      if ($mx -le 60) { continue }
      if ($x -lt $minx) { $minx = $x }; if ($x -gt $maxx) { $maxx = $x }
      if ($y -lt $miny) { $miny = $y }; if ($y -gt $maxy) { $maxy = $y }
    }
  }
  if ($maxx -lt 0) { return @(0, 0, $w, $h) }
  return @([int]$minx, [int]$miny, [int]($maxx - $minx + 1), [int]($maxy - $miny + 1))
}

# like Small, but downsamples the sub-rectangle (cx,cy,cw,ch) to N x N
function Small2([string]$p, [int]$n, [int]$cx, [int]$cy, [int]$cw, [int]$ch) {
  $b = New-Object System.Drawing.Bitmap $p
  $nb = New-Object System.Drawing.Bitmap $n, $n
  $g = [System.Drawing.Graphics]::FromImage($nb)
  $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
  $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
  $dr = New-Object System.Drawing.Rectangle(0, 0, $n, $n)
  $g.DrawImage($b, $dr, $cx, $cy, $cw, $ch, [System.Drawing.GraphicsUnit]::Pixel)
  $g.Dispose(); $b.Dispose()
  $r = $nb.LockBits($dr, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $st = $r.Stride
  $d = New-Object byte[] ($st * $n)
  [System.Runtime.InteropServices.Marshal]::Copy($r.Scan0, $d, 0, $d.Length)
  $nb.UnlockBits($r); $nb.Dispose()
  $R = New-Object 'double[]' ($n * $n)
  $G = New-Object 'double[]' ($n * $n)
  $B = New-Object 'double[]' ($n * $n)
  for ($y = 0; $y -lt $n; $y++) {
    for ($x = 0; $x -lt $n; $x++) {
      $i = $y * $st + $x * 4
      $k = $y * $n + $x
      $B[$k] = $d[$i]; $G[$k] = $d[$i+1]; $R[$k] = $d[$i+2]
    }
  }
  return @{ n = $n; r = $R; g = $G; b = $B }
}

$root = 'C:\GOG Games\Age of Wonders'
$N = 48
$paths = @{
  golden = "$root\port\re\golden\Tutorial_L0.png"
  cur    = "$root\port\bin\cur_tut.png"
  howsb  = "$root\HowItShouldBe.png"
  howit  = "$root\HowItIs.png"
}
$boxes = @{}
foreach ($k in @('golden', 'cur', 'howsb', 'howit')) {
  if (-not (Test-Path $paths[$k])) { Say ("MISSING {0} = {1}" -f $k, $paths[$k]); continue }
  $boxes[$k] = ContentBox $paths[$k]
  Say ("content bbox {0,-7} = {1}" -f $k, ($boxes[$k] -join ','))
}
Say ''

Say ("== PASS A: whole frame normalised to {0}x{0} ==" -f $N)
$gG = Small $paths['golden'] $N
$gC = Small $paths['cur'] $N
$gS = Small $paths['howsb'] $N
Compare-Grid $gG $gC 'golden vs cur'
Compare-Grid $gC $gS 'cur vs howsb'

Say ("== PASS B: content region (dark margin trimmed) normalised to {0}x{0} ==" -f $N)
function CB([string]$k) {
  $bb = $boxes[$k]
  return (Small2 $paths[$k] $N $bb[0] $bb[1] $bb[2] $bb[3])
}
$cG = CB 'golden'; $cC = CB 'cur'; $cS = CB 'howsb'; $cI = CB 'howit'
Compare-Grid $cG $cC 'golden vs cur'
Compare-Grid $cC $cS 'cur vs howsb'
Compare-Grid $cG $cS 'golden vs howsb'
Compare-Grid $cC $cI 'cur vs howit'

$script:L -join "`r`n" | Set-Content -Encoding UTF8 "$outd\_align.out.txt"
Write-Host "wrote $outd\_align.out.txt"
