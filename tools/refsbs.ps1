<#
  refsbs.ps1 - stack a reference capture above the same region of a port render,
  magnified, so a band of the UI can be compared pixel by pixel from a terminal.

  Usage:
    powershell -File tools/refsbs.ps1 -RefPath bin\partyRef.png -OurPath bin\PartyWindow.png `
        -X 0 -Y 165 -W 197 -H 54 -Ox 441 -Oy 214 -Scale 3 -Out bin\PartyFoot_sbs.png

  The reference on top, the port's frame below (max 197 px wide at 1:1), a
  magenta separator between them.
#>
param(
    [string]$RefPath = 'bin\partyRef.png',
    [string]$OurPath = 'bin\party_our.png',
    [int]$X = 0, [int]$Y = 0, [int]$W = 197, [int]$H = 40,
    [int]$Ox = 441, [int]$Oy = 214,
    [int]$Scale = 3,
    [string]$Out = 'tmp_sbs.png'
)
Add-Type -AssemblyName System.Drawing
$refBmp = [System.Drawing.Bitmap]::FromFile((Resolve-Path $RefPath).Path)
$ourBmp = [System.Drawing.Bitmap]::FromFile((Resolve-Path $OurPath).Path)
$dst = New-Object System.Drawing.Bitmap ($W * $Scale), ($H * 2 * $Scale + 6)
$g = [System.Drawing.Graphics]::FromImage($dst)
$g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
$g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
$g.Clear([System.Drawing.Color]::Magenta)
$g.DrawImage($refBmp, (New-Object System.Drawing.Rectangle 0,0,($W*$Scale),($H*$Scale)),
             (New-Object System.Drawing.Rectangle $X,$Y,$W,$H), [System.Drawing.GraphicsUnit]::Pixel)
$g.DrawImage($ourBmp, (New-Object System.Drawing.Rectangle 0,(($H+2)*$Scale),($W*$Scale),($H*$Scale)),
             (New-Object System.Drawing.Rectangle ($Ox+$X),($Oy+$Y),$W,$H), [System.Drawing.GraphicsUnit]::Pixel)
$g.Dispose()
$dst.Save((Join-Path (Get-Location) $Out), [System.Drawing.Imaging.ImageFormat]::Png)
$dst.Dispose(); $refBmp.Dispose(); $ourBmp.Dispose()
Write-Host ("wrote {0}  (reference on top, the port's frame below)" -f $Out)
