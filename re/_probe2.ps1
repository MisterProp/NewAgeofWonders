Add-Type -AssemblyName System.Drawing

function Load-ImgBytes([string]$p) {
  $b = New-Object System.Drawing.Bitmap $p
  $w = $b.Width; $h = $b.Height
  $rect = New-Object System.Drawing.Rectangle(0,0,$w,$h)
  $r = $b.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
  $stride = $r.Stride
  $bytes = New-Object byte[] ($stride * $h)
  [System.Runtime.InteropServices.Marshal]::Copy($r.Scan0, $bytes, 0, $bytes.Length)
  $b.UnlockBits($r)
  $b.Dispose()
  return @{ w = $w; h = $h; stride = $stride; data = $bytes }
}

function Ascii-Map([string]$p, [int]$cols) {
  $img = Load-ImgBytes $p
  $w = $img.w; $h = $img.h; $st = $img.stride; $d = $img.data
  $rows = [int][math]::Max(1, [math]::Round($cols * $h / $w / 2.0))
  $sr = New-Object 'int[]' ($cols * $rows)
  $sg = New-Object 'int[]' ($cols * $rows)
  $sb = New-Object 'int[]' ($cols * $rows)
  $sc = New-Object 'int[]' ($cols * $rows)
  $step = [int][math]::Max(1, [math]::Floor($w / 900))
  for ($y = 0; $y -lt $h; $y += $step) {
    $row = $y * $st
    $cy = [int]($y * $rows / $h); if ($cy -ge $rows) { $cy = $rows - 1 }
    for ($x = 0; $x -lt $w; $x += $step) {
      $i = $row + $x * 4
      $cx = [int]($x * $cols / $w); if ($cx -ge $cols) { $cx = $cols - 1 }
      $k = $cy * $cols + $cx
      $sb[$k] += $d[$i]; $sg[$k] += $d[$i+1]; $sr[$k] += $d[$i+2]; $sc[$k]++
    }
  }
  $out = New-Object System.Text.StringBuilder
  [void]$out.AppendLine("### $p  ${w}x${h}  (ascii ${cols}x${rows}, sample step=$step)")
  for ($cy = 0; $cy -lt $rows; $cy++) {
    $line = ''
    for ($cx = 0; $cx -lt $cols; $cx++) {
      $k = $cy * $cols + $cx
      if ($sc[$k] -eq 0) { $line += ' '; continue }
      $r = [int]($sr[$k] / $sc[$k]); $g = [int]($sg[$k] / $sc[$k]); $b = [int]($sb[$k] / $sc[$k])
      $mx = [math]::Max($r, [math]::Max($g, $b)); $mn = [math]::Min($r, [math]::Min($g, $b))
      if ($mx -lt 24) { $line += '.' }
      elseif (($mx - $mn) -lt 26 -and $mx -gt 140) { $line += 'o' }
      elseif ($b -gt $r + 24 -and $b -gt 50) { $line += '~' }
      elseif ($g -gt $r + 10 -and $g -gt $b + 10) { $line += 'g' }
      elseif ($r -gt $b + 20 -and $r -ge $g) { $line += '#' }
      elseif ($g -gt $b + 10) { $line += 'y' }
      else { $line += '?' }
    }
    [void]$out.AppendLine($line)
  }
  return $out.ToString()
}

$files = @(
  'C:\GOG Games\Age of Wonders\HowItIs.png',
  'C:\GOG Games\Age of Wonders\HowItShouldBe.png',
  'C:\GOG Games\Age of Wonders\port\bin\cur_tut.png',
  'C:\GOG Games\Age of Wonders\port\re\golden\Tutorial_L0.png',
  'C:\GOG Games\Age of Wonders\port\bin\_screen.png'
)
foreach ($f in $files) {
  if (Test-Path $f) { Write-Output (Ascii-Map $f 64) }
  else { Write-Output "MISSING $f" }
}
