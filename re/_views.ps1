Add-Type -AssemblyName System.Drawing
$root = 'C:\GOG Games\Age of Wonders'
$outd = "$root\port\re\view"
New-Item -ItemType Directory -Force -Path $outd | Out-Null

function Save-Scaled([string]$src, [string]$dst, [int]$maxW) {
  $b = New-Object System.Drawing.Bitmap $src
  $sc = $maxW / [double]$b.Width
  $nw = [int]($b.Width * $sc); $nh = [int]($b.Height * $sc)
  $nb = New-Object System.Drawing.Bitmap $nw, $nh
  $gr = [System.Drawing.Graphics]::FromImage($nb)
  $gr.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
  $gr.DrawImage($b, 0, 0, $nw, $nh)
  $gr.Dispose()
  $nb.Save($dst, [System.Drawing.Imaging.ImageFormat]::Png)
  $nb.Dispose(); $b.Dispose()
  Write-Output "saved $dst ${nw}x${nh}"
}

# 1:1 nearest-neighbour crop (scaled by $k) centred on the given point.
function Save-Crop([string]$src, [string]$dst, [int]$cx, [int]$cy, [int]$cw, [int]$ch, [int]$k) {
  $b = New-Object System.Drawing.Bitmap $src
  $x = $cx - [int]($cw / 2); $y = $cy - [int]($ch / 2)
  if ($x -lt 0) { $x = 0 }; if ($y -lt 0) { $y = 0 }
  if ($x + $cw -gt $b.Width) { $x = $b.Width - $cw }
  if ($y + $ch -gt $b.Height) { $y = $b.Height - $ch }
  $nb = New-Object System.Drawing.Bitmap ($cw * $k), ($ch * $k)
  $gr = [System.Drawing.Graphics]::FromImage($nb)
  $gr.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
  $gr.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
  $gr.DrawImage($b, (New-Object System.Drawing.Rectangle(0, 0, ($cw * $k), ($ch * $k))), (New-Object System.Drawing.Rectangle($x, $y, $cw, $ch)), [System.Drawing.GraphicsUnit]::Pixel)
  $gr.Dispose()
  $nb.Save($dst, [System.Drawing.Imaging.ImageFormat]::Png)
  $nb.Dispose(); $b.Dispose()
  Write-Output "saved $dst (crop ${cw}x${ch} at $x,$y x$k)"
}

Save-Scaled "$root\HowItIs.png" "$outd\howit.png" 1000
Save-Scaled "$root\HowItShouldBe.png" "$outd\howsb.png" 1000
Save-Scaled "$root\port\re\golden\Tutorial_L0.png" "$outd\golden.png" 1000
Save-Scaled "$root\port\bin\cur_tut.png" "$outd\cur.png" 1000
Save-Crop "$root\HowItShouldBe.png" "$outd\howsb_center.png" 556 451 500 380 1
Save-Crop "$root\port\re\golden\Tutorial_L0.png" "$outd\golden_center.png" 1152 768 500 380 1
Save-Crop "$root\HowItIs.png" "$outd\howit_center.png" 476 397 500 380 1
Write-Output 'done'
