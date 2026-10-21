<#
  turnbtn_bench.ps1 - proof sheet for the bottom bar's two turn buttons.

  Renders the in-game screen once per button state and composes the crops into
  bin/TurnButtons.png (5x zoom), labelled:

    NextButton   up (TBWin.ILB 3)          - the idle picture
    NextButton   down (TBWin.ILB 4)        - while the button is held (+0x110)
    TurnButton   idle (TBTurnBt.ILB 46)    - Halflings seat block, first frame
    TurnButton   ended (TBTurnBt.ILB 52)   - the block's transition frame
    TurnButton   swirl (TBTurnBt.ILB 5)    - the "turn changing" glow block

  Every crop is the exact button box the layout resolves (596,439 41x38 for the
  TurnButton, 568,439 26x38 for the NextButton), so a crop is a 1:1 view of the
  rendered bar. See docs/22_TURN_BUTTONS.md.

  Usage:
    powershell -File tools/turnbtn_bench.ps1            # writes bin/TurnButtons.png
#>
param(
    [string]$Out = 'bin\TurnButtons.png',
    [int]$Zoom = 5
)
$ErrorActionPreference = 'Stop'
$Here    = Split-Path -Parent $MyInvocation.MyCommand.Path
$Port    = Resolve-Path (Join-Path $Here '..')
$BinDir  = Join-Path $Port 'bin'
$Exe     = Join-Path $BinDir 'AoW.exe'
$GameRoot = Resolve-Path (Join-Path $Port '..')

function Render([string]$out, [string[]]$extra) {
    # One quoted argument string: Start-Process splits an array on spaces inside
    # an element, which would break the "Age of Wonders" root path.
    $argstr = '--root "{0}" --start-map Tutorial.hsm --no-music --no-cursor --render "{1}"' -f $GameRoot, $out
    if ($extra.Count -gt 0) { $argstr += ' ' + ($extra -join ' ') }
    $p = Start-Process -FilePath $Exe -ArgumentList $argstr -Wait -PassThru `
        -NoNewWindow -RedirectStandardOutput ($out + '.log')
    if ($p.ExitCode -ne 0) { throw "render failed: $out" }
}

Add-Type -AssemblyName System.Drawing

$tmp = Join-Path $BinDir 'turnbtn_bench'
New-Item -ItemType Directory -Force -Path $tmp | Out-Null
$states = @(
    @{ name = 'NextButton up (id 3)';        x = 568; y = 439; w = 26; h = 38; extra = @() },
    @{ name = 'NextButton down (id 4)';      x = 568; y = 439; w = 26; h = 38; extra = @('--next-held') },
    @{ name = 'TurnButton idle (46)';        x = 596; y = 439; w = 41; h = 38; extra = @('--turn-frame', '46') },
    @{ name = 'TurnButton ended (52)';       x = 596; y = 439; w = 41; h = 38; extra = @('--turn-frame', '52') },
    @{ name = 'TurnButton swirl (5)';        x = 596; y = 439; w = 41; h = 38; extra = @('--turn-frame', '5') }
)
$crops = @()
for ($i = 0; $i -lt $states.Count; $i++) {
    $s = $states[$i]
    $png = Join-Path $tmp ("state$i.png")
    Render $png $s.extra
    $bmp = [System.Drawing.Bitmap]::FromFile($png)
    $crop = New-Object System.Drawing.Bitmap ($s.w * $Zoom), ($s.h * $Zoom)
    $g = [System.Drawing.Graphics]::FromImage($crop)
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
    $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
    $g.DrawImage($bmp, (New-Object System.Drawing.Rectangle 0, 0, ($s.w * $Zoom), ($s.h * $Zoom)),
                 (New-Object System.Drawing.Rectangle $s.x, $s.y, $s.w, $s.h),
                 [System.Drawing.GraphicsUnit]::Pixel)
    $g.Dispose()
    $bmp.Dispose()
    $crops += [pscustomobject]@{ name = $s.name; bmp = $crop; w = $s.w; h = $s.h }
}

$pad = 8
$labelH = 16
$cellW = 41 * $Zoom
$cellH = 38 * $Zoom
$sheet = New-Object System.Drawing.Bitmap (($cellW + $pad) * $crops.Count + $pad), ($cellH + $labelH + 2 * $pad)
$sg = [System.Drawing.Graphics]::FromImage($sheet)
$sg.Clear([System.Drawing.Color]::FromArgb(32, 32, 32))
$font = New-Object System.Drawing.Font 'Consolas', 8
$brush = [System.Drawing.Brushes]::Yellow
for ($i = 0; $i -lt $crops.Count; $i++) {
    $c = $crops[$i]
    $x = $pad + $i * ($cellW + $pad)
    $sg.DrawString($c.name, $font, $brush, $x, $pad)
    $sg.DrawImage($c.bmp, $x + (($cellW - $c.w * $Zoom) / 2), $pad + $labelH)
    $c.bmp.Dispose()
}
$sg.Dispose()
$dest = Join-Path $Port $Out
$sheet.Save($dest, [System.Drawing.Imaging.ImageFormat]::Png)
$sheet.Dispose()
Write-Host ("wrote {0} ({1} states)" -f $dest, $crops.Count)
