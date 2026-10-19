Add-Type -AssemblyName System.Drawing
$src = 'C:\GOG Games\Age of Wonders\port\bin\cur_tut.png'
$b = [System.Drawing.Bitmap]::FromFile($src)
$out = New-Object System.Collections.Generic.List[string]
$out.Add("img $($b.Width)x$($b.Height)   file $src")
$cnt = @{}
$first = @{}
$lines = New-Object System.Collections.Generic.List[string]
$hdr = "    "
for ($x = 0; $x -lt 48; $x++) { $hdr += [string]([int]($x / 10)) }
$lines.Add($hdr)
$hdr2 = "    "
for ($x = 0; $x -lt 48; $x++) { $hdr2 += [string]($x % 10) }
$lines.Add($hdr2)
for ($y = 0; $y -lt 48; $y++) {
  $row = ""
  for ($x = 0; $x -lt 48; $x++) {
    $sx = 8 + $x * 32
    $sy = $y * 32 + (($x -band 1) * 16)
    $px = $b.GetPixel($sx + 24, $sy + 16)
    $k = "$($px.R),$($px.G),$($px.B)"
    if (-not $cnt.ContainsKey($k)) { $cnt[$k] = 0; $first[$k] = @() }
    $cnt[$k]++
    if ($first[$k].Count -lt 8) { $first[$k] += "$x,$y" }
    if ($px.B -lt 70 -and $px.R -lt 70) { $c = '.' }
    elseif ($px.B -gt $px.R + 24 -and $px.B -ge $px.G) { $c = '~' }
    else { $c = '#' }
    $row += $c
  }
  $lines.Add(("{0,2}  {1}" -f $y, $row))
}
$out.Add("--- hex-centre classification (.=dark/border  ~=water  #=land) ---")
$out.AddRange($lines)
$tot = 48 * 48
$out.Add("--- distinct centre colours (top 25 of all $tot samples) ---")
foreach ($kv in ($cnt.GetEnumerator() | Sort-Object -Property Value -Descending | Select-Object -First 25)) {
  $out.Add(("{0,13}  n={1,5}  ({2,6:P1})  first: {3}" -f $kv.Key, $kv.Value, ($kv.Value / $tot), ($first[$kv.Key] -join ' ')))
}
$out.Add("distinct colours total = $($cnt.Count)")
$b.Dispose()
Set-Content -Path 'C:\GOG Games\Age of Wonders\port\re\_grid.out.txt' -Value $out -Encoding UTF8
