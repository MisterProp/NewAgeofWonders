Add-Type -AssemblyName System.Drawing
$root = 'C:\GOG Games\Age of Wonders\port'
$paths = @('re\HowItIs.png','re\HowItShouldBe.png','bin\cur_tut.png','bin\cur_tut_small.png',
           're\png\Tutorial_L0.png','re\golden\Tutorial_L0.png','bin\_screen.png','bin\_hgr0.png','bin\_hgr0_big.png')
foreach ($p in $paths) {
  $full = Join-Path $root $p
  if (-not (Test-Path $full)) { Write-Output "MISSING $p"; continue }
  $b = New-Object System.Drawing.Bitmap $full
  $w = $b.Width; $h = $b.Height
  $r = $b.LockBits((New-Object System.Drawing.Rectangle 0,0,$w,$h), 'ReadOnly', [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $stride = $r.Stride
  $bytes = New-Object byte[] ($stride * $h)
  [System.Runtime.InteropServices.Marshal]::Copy($r.Scan0, $bytes, 0, $bytes.Length)
  $b.UnlockBits($r); $b.Dispose()
  $minx = $w; $maxx = -1; $miny = $h; $maxy = -1
  $cntBlack = 0; $cntBlue = 0; $cntTan = 0; $cntGreen = 0; $n = 0; $cntOpaque = 0
  for ($y = 0; $y -lt $h; $y += 3) {
    $row = $y * $stride
    for ($x = 0; $x -lt $w; $x += 3) {
      $i = $row + $x * 4
      $bl = $bytes[$i]; $g = $bytes[$i+1]; $rr = $bytes[$i+2]; $al = $bytes[$i+3]
      $n++
      if ($al -gt 0) { $cntOpaque++ }
      elseif ($al -eq 0) { $cntBlack++; continue }
      if ($rr -lt 16 -and $g -lt 16 -and $bl -lt 16) { $cntBlack++; continue }
      if ($bl -gt $rr + 20 -and $bl -gt 60 -and $bl -gt $g) { $cntBlue++ }
      if ($rr -gt $g + 10 -and $g -gt $bl + 10) { $cntTan++ }
      if ($g -gt $rr + 10 -and $g -gt $bl + 10) { $cntGreen++ }
      if ($x -lt $minx) { $minx = $x }; if ($x -gt $maxx) { $maxx = $x }
      if ($y -lt $miny) { $miny = $y }; if ($y -gt $maxy) { $maxy = $y }
    }
  }
  Write-Output ("{0} {1}x{2} bbox=({3},{4})-({5},{6}) opaque={7} black={8} blue={9} tan={10} green={11} n={12}" -f $p,$w,$h,$minx,$miny,$maxx,$maxy,$cntOpaque,$cntBlack,$cntBlue,$cntTan,$cntGreen,$n)
}
Write-Output '--- corner samples (format32bppArgb -> B,G,R,A) ---'
foreach ($p in @('re\HowItIs.png','re\HowItShouldBe.png','bin\cur_tut.png')) {
  $full = Join-Path $root $p
  if (-not (Test-Path $full)) { continue }
  $b = New-Object System.Drawing.Bitmap $full
  $w = $b.Width; $h = $b.Height
  $pts = @(@(0,0), @([int]($w/2),0), @(0,[int]($h/2)), @([int]($w/2),[int]($h/2)), @($w-1,$h-1))
  $s = "$p"
  foreach ($pt in $pts) {
    $c = $b.GetPixel($pt[0], $pt[1])
    $s += ("  ({0},{1})=#{2:X2}{3:X2}{4:X2}/a{5}" -f $pt[0],$pt[1],$c.R,$c.G,$c.B,$c.A)
  }
  $b.Dispose()
  Write-Output $s
}
