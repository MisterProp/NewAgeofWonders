"""info_bench.py - pixel benchmark for the TUnitWindow (the Unit info floater).

    python tools/info_bench.py bin/InfoMenuRef.png bin/bm_abil.png
    python tools/info_bench.py bin/InfoMenuRef2.png bin/bm_info.png

Both captures are the original game's Unit window (Abilities and Info tabs).
The screenshot is cropped to the 420x417 UnitWin box (centred, floor at
640x480) and the reference is then searched over every placement inside that
box, so a capture cropped a few pixels off still compares correctly.

Prints the mean |d| per channel over the whole window and over the DFM
controls, and writes <ref>_sbs.png / <ref>_diff.png next to the reference.

Render the two inputs with the port on Bloody Marauders (slot 2 is the Human
Musketeer the captures show):

    AoW.exe --root <game> --screen game --map Scenario\\Custom\\Bloody Marauders.hsm \
            --no-music --no-cursor --game-prekey L --game-click 515 245 \
            --render bin/bm_abil.png
    ... --game-click 450 205 --render bin/bm_info.png
"""
import sys

from PIL import Image, ImageChops, ImageStat

W, H = 420, 417
REGIONS = {
    'title':      (0, 0, 420, 22),
    'leftframe':  (21, 22, 139, 165),
    'statpanel':  (138, 50, 282, 136),
    'rightframe': (281, 22, 399, 165),
    'tabs':       (21, 163, 399, 185),
    'listpanel':  (22, 178, 398, 379),
    'memo':       (27, 193, 393, 283),
    'cards':      (27, 287, 393, 374),
    'footer':     (21, 380, 399, 407),
}


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 1
    ref_path, shot_path = sys.argv[1], sys.argv[2]
    ref = Image.open(ref_path).convert('RGB')
    shot = Image.open(shot_path).convert('RGB')
    x0 = int(sys.argv[3]) if len(sys.argv) > 3 else (shot.width - W) // 2
    y0 = int(sys.argv[4]) if len(sys.argv) > 4 else (shot.height - H) // 2
    port = shot.crop((x0, y0, x0 + W, y0 + H))
    rw, rh = ref.size
    best = None
    for ry in range(0, H - rh + 1):
        for rx in range(0, W - rw + 1):
            b = port.crop((rx, ry, rx + rw, ry + rh))
            score = sum(ImageStat.Stat(ImageChops.difference(ref, b)).mean) / 3.0
            if best is None or score < best[0]:
                best = (score, rx, ry)
    score, rx, ry = best
    print('best placement ref inside port at (%d,%d) mean|d|=%.2f/255' %
          (rx, ry, score))
    b = port.crop((rx, ry, rx + rw, ry + rh))
    diff = ImageChops.difference(ref, b)
    print('%-11s %8s %8s' % ('region', 'mean|d|', 'diffpx%'))
    for name, (x0r, y0r, x1r, y1r) in REGIONS.items():
        x0r = max(0, x0r - rx)
        y0r = max(0, y0r - ry)
        x1r = min(rw, x1r - rx)
        y1r = min(rh, y1r - ry)
        if x1r <= x0r or y1r <= y0r:
            continue
        d = ImageChops.difference(ref.crop((x0r, y0r, x1r, y1r)),
                                  b.crop((x0r, y0r, x1r, y1r)))
        px = list(d.getdata())
        n = sum(1 for p in px if max(p) > 8)
        print('%-11s %8.2f %7.1f%%' %
              (name, sum(ImageStat.Stat(d).mean) / 3.0,
               100.0 * n / max(1, len(px))))
    total = rw * rh
    px = list(diff.getdata())
    print('%-11s %8s %8s' % ('tolerance', 'pixels', 'percent'))
    for tol in (0, 2, 4, 8, 16, 32, 64):
        n = sum(1 for p in px if max(p) <= tol)
        print('<=%-9d %8d %7.2f%%' % (tol, n, 100.0 * n / max(1, total)))
    sbs = Image.new('RGB', (rw * 2 + 8, rh), (255, 0, 255))
    sbs.paste(ref, (0, 0))
    sbs.paste(b, (rw + 8, 0))
    base = ref_path.rsplit('.', 1)[0]
    sbs.save(base + '_sbs.png')
    diff.save(base + '_diff.png')
    print('wrote', base + '_sbs.png', base + '_diff.png')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
