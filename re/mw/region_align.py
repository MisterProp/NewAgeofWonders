#!/usr/bin/env python3
"""Per-region best alignment of the fresh render against the reference crop."""
from PIL import Image
import sys

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[1] if len(sys.argv) > 1 else 're/mw/fresh.png').convert('RGB')
rp, cp = ref.load(), cur.load()
W, H = ref.size
regions = {
    'title':      (8, 0, 360, 20),
    'band1':      (8, 20, 360, 46),
    'drive/row':  (15, 50, 355, 88),
    'path/row':   (15, 88, 355, 108),
    'list-rows':  (20, 108, 355, 245),
    'list-frame': (20, 100, 360, 250),
    'info-strip': (15, 245, 355, 280),
    'wells':      (15, 280, 355, 322),
    'buttons':    (15, 322, 355, 356),
}
for name, (x0, y0, x1, y1) in regions.items():
    best = None
    for oy in range(-8, 9):
        for ox in range(-8, 9):
            tot = 0
            n = 0
            for y in range(y0, y1, 2):
                for x in range(x0, x1, 2):
                    ar, ag, ab = rp[x, y]
                    bx, by = 135 + x + ox, 35 + y + oy
                    if not (0 <= bx < cur.size[0] and 0 <= by < cur.size[1]):
                        continue
                    br, bg, bb = cp[bx, by]
                    tot += abs(ar - br) + abs(ag - bg) + abs(ab - bb)
                    n += 3
            v = tot / n
            if best is None or v < best[0]:
                best = (v, ox, oy)
    print('%-11s best offset %+d,%+d  mean %.2f' % (name, best[1], best[2],
                                                     best[0]))
