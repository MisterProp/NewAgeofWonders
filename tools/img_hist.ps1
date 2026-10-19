# img_hist.ps1 - histogram of the img_ascii colour classes for one or more PNGs.
# Usage: powershell -File img_hist.ps1 <img.png> [<img2.png> ...]
# Prints the percentage of sampled pixels per class so two renders of the same
# map can be compared numerically (the agent cannot view PNGs).
Add-Type -AssemblyName System.Drawing

function Classify([double]$r,[double]$g,[double]$b,[double]$a) {
    if ($a -lt 32) { return '.' }
    $br = ($r + $g + $b) / 3.0
    $mx = [math]::Max($r,[math]::Max($g,$b))
    $mn = [math]::Min($r,[math]::Min($g,$b))
    $sat = $mx - $mn
    if ($br -lt 40) { return '#' }
    if ($b - $r -gt 18 -and $b - $g -gt 10) { return '~' }
    if ($g - $r -gt 12 -and $g - $b -gt 6) { return 'G' }
    if ($sat -lt 30) {
        if ($br -gt 185) { return 'L' }
        if ($br -gt 140) { return 'M' }
        if ($b -ge $r) { return 'p' }
        return 'k'
    }
    if ($r -gt $g -and $g -gt $b) { return 'd' }
    return '?'
}

foreach ($p in $args) {
    $bmp = New-Object System.Drawing.Bitmap ((Resolve-Path $p).Path)
    $W=$bmp.Width; $H=$bmp.Height
    $hist=@{}
    $n=0
    for ($y=0; $y -lt $H; $y+=3) {
        for ($x=0; $x -lt $W; $x+=3) {
            $c=$bmp.GetPixel($x,$y); $n++
            $k = Classify $c.R $c.G $c.B $c.A
            if ($hist.ContainsKey($k)) { $hist[$k]++ } else { $hist[$k]=1 }
        }
    }
    $bmp.Dispose()
    $names = @{'~'='water';'G'='green';'#'='black';'d'='tan';'M'='grey';'L'='light';'k'='brown';'p'='slate';'?'='other';'.'='transp'}
    $parts = $hist.GetEnumerator() | Sort-Object Value -Descending | ForEach-Object {
        "{0}={1:N1}%" -f $names[$_.Key], (100.0*$_.Value/$n)
    }
    Write-Host ("{0,-24} n={1,-7} {2}" -f (Split-Path $p -Leaf), $n, ($parts -join ' '))
}
