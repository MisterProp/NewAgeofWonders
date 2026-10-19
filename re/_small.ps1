Add-Type -AssemblyName System.Drawing
$root = 'C:\GOG Games\Age of Wonders'
$outd = "$root\port\re\sm"
New-Item -ItemType Directory -Force -Path $outd | Out-Null

function Save-Small([string]$src, [string]$name, [int]$maxW) {
  if (-not (Test-Path $src)) { Write-Host "MISSING $src"; return }
  $b = New-Object System.Drawing.Bitmap $src
  $sc = $maxW / [double]$b.Width
  $nw = [int]($b.Width * $sc); $nh = [int]($b.Height * $sc)
  $nb = New-Object System.Drawing.Bitmap $nw, $nh
  $gr = [System.Drawing.Graphics]::FromImage($nb)
  $gr.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
  $gr.DrawImage($b, 0, 0, $nw, $nh)
  $gr.Dispose()
  $dst = Join-Path $outd "$name.png"
  $nb.Save($dst, [System.Drawing.Imaging.ImageFormat]::Png)
  $nb.Dispose(); $b.Dispose()
  Write-Host ("{0} <- {1}  src={2}x{3} -> {4}x{5}" -f $dst, $src, $b.Width, $b.Height, $nw, $nh)
}

Save-Small "$root\HowItIs.png"                    'howit'   460
Save-Small "$root\HowItShouldBe.png"              'howsb'   460
Save-Small "$root\port\re\golden\Tutorial_L0.png" 'golden'  460
Save-Small "$root\port\bin\cur_tut.png"           'cur'     460
Save-Small "$root\port\bin\_screen.png"           'screen'  460
Write-Host 'done'
