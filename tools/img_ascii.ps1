<#
  img_ascii.ps1 - render a PNG as an ASCII colour-class map so it can be
  inspected from a terminal (the agent's read_files tool cannot show PNGs).

  Usage:
    powershell -File img_ascii.ps1 -Path <img.png> [-Cols 100] [-Rows 40] [-Mode cls|hex]

  Classification (per down-sampled cell, average of its pixels):
    '.' transparent   '#' near-black   '~' blue water   'G' green
    'd' tan/desert    'M' grey/stone   'W' white/snow   '?' other
#>
param(
    [Parameter(Mandatory=$true)][string]$Path,
    [int]$Cols = 100,
    [int]$Rows = 40,
    [string]$Mode = 'cls'
)

Add-Type -AssemblyName System.Drawing
$bmp = [System.Drawing.Bitmap]::FromFile((Resolve-Path $Path).Path)
Write-Host ("{0}  {1}x{2}" -f (Split-Path $Path -Leaf), $bmp.Width, $bmp.Height)

# Pre-extract pixels via LockBits for speed.
$rect = New-Object System.Drawing.Rectangle 0,0,$bmp.Width,$bmp.Height
$data = $bmp.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly,
                      [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$stride = $data.Stride
$bytes = New-Object byte[] ($stride * $bmp.Height)
[System.Runtime.InteropServices.Marshal]::Copy($data.Scan0, $bytes, 0, $bytes.Length)
$bmp.UnlockBits($data)

$W = $bmp.Width; $H = $bmp.Height; $bmp.Dispose()

function Classify([double]$r,[double]$g,[double]$b,[double]$a) {
    if ($a -lt 32) { return '.' }
    $br = ($r + $g + $b) / 3.0
    $mx = [math]::Max($r,[math]::Max($g,$b))
    $mn = [math]::Min($r,[math]::Min($g,$b))
    $sat = $mx - $mn
    if ($br -lt 40) { return '#' }
    if ($b - $r -gt 18 -and $b - $g -gt 10) { return '~' }
    if ($g - $r -gt 12 -and $g - $b -gt 6) { return 'G' }
    # Low-saturation: separate light stone (the original's mountain faces) from
    # the mid-grey and the darker "boulder" look.
    if ($sat -lt 30) {
        if ($br -gt 185) { return 'L' }   # bright limestone / snow face
        if ($br -gt 140) { return 'M' }   # mid grey rock
        if ($b -ge $r) { return 'p' }     # dark slate / lavender
        return 'k'                        # dark brown-grey
    }
    if ($r -gt $g -and $g -gt $b) { return 'd' }
    return '?'
}

$out = New-Object System.Text.StringBuilder
for ($cy = 0; $cy -lt $Rows; $cy++) {
    $y0 = [int]($cy * $H / $Rows); $y1 = [int](($cy+1) * $H / $Rows)
    if ($y1 -le $y0) { $y1 = $y0 + 1 }
    for ($cx = 0; $cx -lt $Cols; $cx++) {
        $x0 = [int]($cx * $W / $Cols); $x1 = [int](($cx+1) * $W / $Cols)
        if ($x1 -le $x0) { $x1 = $x0 + 1 }
        $sr=0.0;$sg=0.0;$sb=0.0;$sa=0.0;$n=0
        for ($y=$y0; $y -lt $y1; $y++) {
            $row = $y * $stride
            for ($x=$x0; $x -lt $x1; $x++) {
                $i = $row + $x*4
                $sb += $bytes[$i]; $sg += $bytes[$i+1]; $sr += $bytes[$i+2]; $sa += $bytes[$i+3]
                $n++
            }
        }
        if ($n -eq 0) { [void]$out.Append(' ') ; continue }
        $r=$sr/$n; $g=$sg/$n; $b=$sb/$n; $a=$sa/$n
        if ($Mode -eq 'hex') {
            [void]$out.Append(('{0:x2}{1:x2}{2:x2} ' -f [int]$r,[int]$g,[int]$b))
        } else {
            [void]$out.Append((Classify $r $g $b $a))
        }
    }
    [void]$out.Append("`n")
}
Write-Host $out.ToString()
