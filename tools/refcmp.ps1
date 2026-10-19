<#
  refcmp.ps1 - compare a reference capture with the same region of a port render.

  The reference screenshots shipped in port/bin/*Ref.png are *crops* of the
  original game's frame, so their origin is not guaranteed: this finds the
  (dx,dy) around a nominal port-image origin with the smallest mean absolute
  channel difference, reports it, then prints a per-row profile so the differing
  bands are obvious.

  Usage:
    powershell -File tools/refcmp.ps1 -Ref bin\partyRef.png -Our bin\PartyWindow.png `
        -Ox 440 -Oy 216 -Range 6

  `-Ox`/`-Oy` are the pixel origin of the same window inside the port's render
  (e.g. the Party floater sits at (disp_w - 200, 216)).
#>
param(
    [string]$RefPath = 'bin\partyRef.png',
    [string]$OurPath = 'bin\party_our.png',
    [int]$Ox = 440,
    [int]$Oy = 216,
    [int]$Range = 6,
    [int]$ProfileStep = 4
)
Add-Type -AssemblyName System.Drawing
$refBmp = [System.Drawing.Bitmap]::FromFile((Resolve-Path $RefPath).Path)
$ourBmp = [System.Drawing.Bitmap]::FromFile((Resolve-Path $OurPath).Path)
$rw = $refBmp.Width; $rh = $refBmp.Height
Write-Host ("ref {0}x{1}   our {2}x{3}" -f $rw, $rh, $ourBmp.Width, $ourBmp.Height)
$best = $null
for ($dy = -$Range; $dy -le $Range; $dy++) {
    for ($dx = -$Range; $dx -le $Range; $dx++) {
        $sum = 0.0; $n = 0
        for ($y = 0; $y -lt $rh; $y++) {
            for ($x = 0; $x -lt $rw; $x++) {
                $px = $Ox + $dx + $x; $py = $Oy + $dy + $y
                if ($px -lt 0 -or $py -lt 0 -or $px -ge $ourBmp.Width -or $py -ge $ourBmp.Height) { continue }
                $a = $refBmp.GetPixel($x, $y); $b = $ourBmp.GetPixel($px, $py)
                $sum += [math]::Abs($a.R - $b.R) + [math]::Abs($a.G - $b.G) + [math]::Abs($a.B - $b.B)
                $n += 3
            }
        }
        $d = $sum / [math]::Max($n, 1)
        if ($null -eq $best -or $d -lt $best.d) { $best = [pscustomobject]@{ d = $d; dx = $dx; dy = $dy } }
    }
}
Write-Host ("best alignment dx={0} dy={1}  mean|d| = {2:N2}/255" -f $best.dx, $best.dy, $best.d)
for ($y = 0; $y -lt $rh; $y += $ProfileStep) {
    $sum = 0.0; $n = 0
    for ($x = 0; $x -lt $rw; $x++) {
        $px = $Ox + $best.dx + $x; $py = $Oy + $best.dy + $y
        if ($px -lt 0 -or $py -lt 0 -or $px -ge $ourBmp.Width -or $py -ge $ourBmp.Height) { continue }
        $a = $refBmp.GetPixel($x, $y); $b = $ourBmp.GetPixel($px, $py)
        $sum += [math]::Abs($a.R - $b.R) + [math]::Abs($a.G - $b.G) + [math]::Abs($a.B - $b.B)
        $n += 3
    }
    $mean = $sum / [math]::Max($n, 1)
    Write-Host ("y={0,3}  {1,6:N1}  {2}" -f $y, $mean, ('#' * [int]($mean / 2)))
}
$refBmp.Dispose(); $ourBmp.Dispose()
