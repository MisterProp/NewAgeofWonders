"""city_bench.py - compare the port's TCityScreen render with UnitCreationRef.png.

The reference is a 1:1 capture of the original 640x480 screen cropped to
638x472 (2 px off the right edge, 8 px off the bottom).  The port renders the
full 640x480 frame, so the comparison crop is (0,0,638,472): the capture's
rows are the screen's own rows.

    python tools/city_bench.py bin/UnitCreationRef.png bin/city_final.png
"""
import sys

from PIL import Image


def main():
    ref_path = sys.argv[1] if len(sys.argv) > 1 else 'bin/UnitCreationRef.png'
    cur_path = sys.argv[2] if len(sys.argv) > 2 else 'bin/city_final.png'
    ref = Image.open(ref_path).convert('RGB')
    cur = Image.open(cur_path).convert('RGB')
    if cur.size == (640, 480):
        cur = cur.crop((0, 0, 638, 472))
    if cur.size != ref.size:
        raise SystemExit('size mismatch: ref %s cur %s' % (ref.size, cur.size))
    w, h = ref.size
    a = ref.load()
    b = cur.load()
    total = 0
    diff = Image.new('RGB', (w, h))
    d = diff.load()
    bands = {}
    for y in range(h):
        for x in range(w):
            p, q = a[x, y], b[x, y]
            e = abs(p[0] - q[0]) + abs(p[1] - q[1]) + abs(p[2] - q[2])
            total += e
            v = min(255, e // 3 * 4)
            d[x, y] = (v, v, v)
            band = ('title' if y < 164 else
                    'panels' if y < 354 else
                    'queue' if y < 432 else 'bar')
            s, n = bands.get(band, (0, 0))
            bands[band] = (s + e, n + 3)
    mean = total / (w * h * 3)
    print('frame %dx%d mean|d| = %.3f/255 (%.2f%% similarity)'
          % (w, h, mean, 100.0 * (1.0 - mean / 255.0)))
    for name, (s, n) in bands.items():
        print('  %-7s %.3f' % (name, s / max(n, 1)))
    diff.save('bin/city_diff.png')
    sbs = Image.new('RGB', (w * 3 + 8, h), (0, 0, 0))
    sbs.paste(ref, (0, 0))
    sbs.paste(cur, (w + 4, 0))
    sbs.paste(diff, (w * 2 + 8, 0))
    sbs.save('bin/CityScreen_sbs.png')
    print('wrote bin/city_diff.png and bin/CityScreen_sbs.png')


if __name__ == '__main__':
    main()
