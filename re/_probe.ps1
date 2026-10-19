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

# gradient energy projected on the x axis (vertical edges) / y axis (horizontal edges)
function ColEdges($im) {
  $w = $im.w; $h = $im.h; $st = $im.st; $d = $im.d
  $sig = New-Object 'double[]' $w
  for ($x = 0; $x -lt $w - 1; $x++) {
    $s = 0.0
    for ($y = 0; $y -lt $h; $y++) {
      $i = $y * $st + $x * 4
      $s += [math]::Abs([int]$d[$i]     - [int]$d[$i+4]) + `
            [math]::Abs([int]$d[$i+1]   - [int]$d[$i+5]) + `
            [math]::Abs([int]$d[$i+2]   - [int]$d[$i+6])
    }
    $sig[$x] = $s
  }
  return $sig
}
function RowEdges($im) {
  $w = $im.w; $h = $im.h; $st = $im.st; $d = $im.d
  $sig = New-Object 'double[]' $h
  for ($y = 0; $y -lt $h - 1; $y++) {
    $s = 0.0
    $r0 = $y * $st; $r1 = ($y + 1) * $st
    for ($x = 0; $x -lt $w; $x++) {
      $i0 = $r0 + $x * 4; $i1 = $r1 + $x * 4
      $s += [math]::Abs([int]$d[$i0]   - [int]$d[$i1]) + `
            [math]::Abs([int]$d[$i0+1] - [int]$d[$i1+1]) + `
            [math]::Abs([int]$d[$i0+2] - [int]$d[$i1+2])
    }
    $sig[$y] = $s
  }
  return $sig
}

# local maxima of the gradient signal above mean + k*std -> edge positions
function EdgePeaks($sig) {
  $n = $sig.Length
  $sum = 0.0
  for ($i = 0; $i -lt $n; $i++) { $sum += $sig[$i] }
  $mean = $sum / $n
  $v = 0.0
  for ($i = 0; $i -lt $n; $i++) { $d = $sig[$i] - $mean; $v += $d * $d }
  $std = [math]::Sqrt($v / $n)
  $thr = $mean + 1.5 * $std
  $pos = New-Object System.Collections.Generic.List[int]
  for ($i = 1; $i -lt $n - 1; $i++) {
    if ($sig[$i] -gt $thr -and $sig[$i] -ge $sig[$i-1] -and $sig[$i] -ge $sig[$i+1]) { $pos.Add($i) }
  }
  return @{ pos = $pos; thr = $thr; mean = $mean; std = $std }
}
function GapHist($pos, [int]$lo, [int]$hi) {
  $h = @{}
  for ($i = 1; $i -lt $pos.Count; $i++) {
    $g = $pos[$i] - $pos[$i-1]
    if ($g -ge $lo -and $g -le $hi) { if ($h.ContainsKey($g)) { $h[$g]++ } else { $h[$g] = 1 } }
  }
  return $h
}
function PhaseOf($pos, [int]$p) {
  $h = @{}
  foreach ($v in $pos) { $k = $v % $p; if ($h.ContainsKey($k)) { $h[$k]++ } else { $h[$k] = 1 } }
  $b = $h.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 1
  return ("{0}px (x{1})" -f $b.Key, $b.Value)
}

$targets = @(
  @{ n = 'cur_tut'; p = "$root\port\bin\cur_tut.png" },
  @{ n = 'golden';  p = "$outd\golden\Tutorial_L0.png" },
  @{ n = 'howsb';   p = "$root\HowItShouldBe.png" },
  @{ n = 'howit';   p = "$root\HowItIs.png" }
)
foreach ($t in $targets) {
  if (-not (Test-Path $t.p)) { Say ("== {0} MISSING" -f $t.n); continue }
  $im = Load-Img $t.p
  Say ("== {0}  {1}x{2}" -f $t.n, $im.w, $im.h)
  $cx = ColEdges $im
  $cy = RowEdges $im
  $px = EdgePeaks $cx
  $py = EdgePeaks $cy
  $hx = GapHist $px.pos 8 80
  $hy = GapHist $py.pos 8 80
  $topx = @($hx.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 6)
  $topy = @($hy.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 6)
  Say ("   col edges: {0} peaks (thr={1:N0}, mean={2:N0}, sd={3:N0})" -f $px.pos.Count, $px.thr, $px.mean, $px.std)
  Say ("   col gap hist: " + (($topx | ForEach-Object { "{0}px x{1}" -f $_.Key, $_.Value }) -join '  '))
  Say ("   row edges: {0} peaks (thr={1:N0}, mean={2:N0}, sd={3:N0})" -f $py.pos.Count, $py.thr, $py.mean, $py.std)
  Say ("   row gap hist: " + (($topy | ForEach-Object { "{0}px x{1}" -f $_.Key, $_.Value }) -join '  '))
  $pbest = [int]$topx[0].Key
  $qbest = [int]$topy[0].Key
  Say ("   col phase(mod {0}): {1} ; row phase(mod {2}): {3}" -f $pbest, (PhaseOf $px.pos $pbest), $qbest, (PhaseOf $py.pos $qbest))
  $im = $null
  Say ''
}

$L -join "`r`n" | Set-Content -Encoding UTF8 "$outd\_probe.out.txt"
Write-Host "wrote $outd\_probe.out.txt"
