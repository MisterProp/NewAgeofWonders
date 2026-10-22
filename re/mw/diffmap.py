#!/usr/bin/env python3
"""Amplified abs-diff heatmap + per-band stats between ref and a render crop."""
from PIL import Image, ImageChops
import sys

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[1]).convert('RGB').crop((135, 35, 135 + 369, 35 + 359))
d = ImageChops.difference(ref, cur)
d2 = d.point(lambda v: min(255, v * 4))
d2.save('re/mw/diffmap.png')
# stats by 20px bands
rp, cp = ref.load(), cur.load()
print('bands (x0,y0, w,h): mean / within30')
for by in range(0, 359, 20):
    line = []
    for bx in range(0, 369, 40):
        tot = n = hits = 0
        for y in range(by, min(by + 20, 359)):
            for x in range(bx, min(bx + 40, 369)):
                ar, ag, ab = rp[x, y]
                br, bg, bb = cp[x, y]
                m = max(abs(ar - br), abs(ag - bg), abs(ab - bb))
                tot += m
                n += 1
                if m <= 30:
                    hits += 1
        line.append('%4.1f/%3.0f%%' % (tot / n, 100.0 * hits / n))
    print('y%3d ' % by + ' '.join(line))
