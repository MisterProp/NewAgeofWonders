"""scen_bench.py - pixel benchmark for the Scenario screen (TScenSetup).

    python tools/scen_bench.py bin/FontRef.png bin/scen_final.png

`FontRef.png` is a 634x466 capture of the shipped screen (its window client is
a few pixels smaller than the port's 640x480 canvas), so the reference is first
located inside the render. It shows the Humans / Dark Elves / Halflings /
Dwarves cell state; the closest map in the retail install is
`Scenario\\Custom\\Bloody Marauders.hsm` (Humans as the Human, the other three
as CPU Squire), so only the map name in the title line differs.

Render the input with:

    AoW.exe --root <game> --rgb555 --screen scenario --scen-frame 90 \
            --scen-pick "Bloody Marauders.hsm" --no-music --no-cursor \
            --render bin\\scen_final.png

Prints the mean per-channel difference (also as a percentage of full scale),
the share of pixels over a few tolerances, and per-area scores; writes
<ref>_sbs.png (reference above render), <ref>_diff.png (x3 difference) and
<ref>_blend.png next to the reference.
"""
import sys

from PIL import Image, ImageChops, ImageStat

AREAS = {
    'title text': (100, 4, 540, 26),
    'settings panel': (505, 38, 632, 245),
    'race cells': (6, 32, 495, 320),
}


def place(ref, shot):
    """Find the reference's origin inside the render (coarse-to-fine)."""
    rp, sp = ref.load(), shot.load()
    best = None
    for oy in range(0, shot.height - ref.height + 1):
        for ox in range(0, shot.width - ref.width + 1):
            s = 0
            n = 0
            for y in range(0, ref.height, 6):
                for x in range(0, ref.width, 6):
                    a = rp[x, y]
                    b = sp[ox + x, oy + y]
                    s += abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])
                    n += 3
            v = s / n
            if best is None or v < best[0]:
                best = (v, ox, oy)
    return best[1], best[2]


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 1
    ref = Image.open(sys.argv[1]).convert('RGB')
    shot = Image.open(sys.argv[2]).convert('RGB')
    ox, oy = place(ref, shot)
    port = shot.crop((ox, oy, ox + ref.width, oy + ref.height))
    print('placement ref inside render at (%d,%d)' % (ox, oy))

    diff = ImageChops.difference(ref, port)
    mean = sum(ImageStat.Stat(diff).mean) / 3.0
    px = list(diff.getdata())
    print('mean|d| = %.2f/255  (%.2f%%)' % (mean, mean * 100.0 / 255.0))
    for tol in (0, 2, 4, 8, 16, 32):
        n = sum(1 for p in px if max(p) > tol)
        print('pixels differing by >%2d : %6.2f%%' % (tol, 100.0 * n / len(px)))

    for name, (x0, y0, x1, y1) in AREAS.items():
        d = ImageChops.difference(ref.crop((x0, y0, x1, y1)),
                                  port.crop((x0, y0, x1, y1)))
        p = list(d.getdata())
        print('%-15s mean=%6.2f  >8: %6.2f%%' %
              (name, sum(ImageStat.Stat(d).mean) / 3,
               100.0 * sum(1 for q in p if max(q) > 8) / len(p)))

    stem = sys.argv[1].rsplit('.', 1)[0]
    side = Image.new('RGB', (ref.width, ref.height * 2 + 4), (255, 0, 0))
    side.paste(ref, (0, 0))
    side.paste(port, (0, ref.height + 4))
    side.save(stem + '_sbs.png')
    dimg = Image.new('RGB', ref.size)
    dp = dimg.load()
    for y in range(ref.height):
        for x in range(ref.width):
            q = diff.getpixel((x, y))
            dp[x, y] = (min(255, q[0] * 3), min(255, q[1] * 3),
                        min(255, q[2] * 3))
    dimg.save(stem + '_diff.png')
    Image.blend(port, ref, 0.5).save(stem + '_blend.png')
    print('wrote %s_sbs.png / %s_diff.png / %s_blend.png' % (stem, stem, stem))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
