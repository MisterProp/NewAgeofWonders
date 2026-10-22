#!/usr/bin/env python3
"""Precise geometry probe for ScenarioMenuRef.png vs a port render."""
from PIL import Image
import sys

ref = Image.open(sys.argv[1] if len(sys.argv) > 1 else 'bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[2] if len(sys.argv) > 2 else 're/mw/cur_scen.png').convert('RGB')
print('ref', ref.size, 'cur', cur.size)
rp, cp = ref.load(), cur.load()

def prof_col(im, p, x, y0, y1):
    return ['%d:%d' % (y, sum(p[x, y]) // 3) for y in range(y0, y1)]

def prof_row(im, p, y, x0, x1):
    return ['%d:%d' % (x, sum(p[x, y]) // 3) for x in range(x0, x1)]

print('ref col 200 rows 0..60:', prof_col(ref, rp, 200, 0, 61))
print('ref col 200 rows 240..359:', prof_col(ref, rp, 200, 240, 359))
print('ref row 200 cols 0..40:', prof_row(ref, rp, 200, 0, 41))
print('ref row 200 cols 330..369:', prof_row(ref, rp, 200, 330, 369))
print('ref row 10 cols 0..369:', prof_row(ref, rp, 10, 0, 369))
print('ref row 350 cols 0..369:', prof_row(ref, rp, 350, 0, 369))

print('cur col 200 rows 0..70:', prof_col(cur, cp, 200, 0, 71))
print('cur col 200 rows 330..480:', prof_col(cur, cp, 200, 330, 480))
print('cur row 200 cols 120..165:', prof_row(cur, cp, 200, 120, 166))
print('cur row 200 cols 460..520:', prof_row(cur, cp, 200, 460, 521))
print('cur row 40 cols 120..520:', prof_row(cur, cp, 40, 120, 521))

# candidate crops of the current render, compared to the reference 1:1
for ox, oy in [(130, 35), (130, 58), (130, 59), (129, 58), (131, 58),
               (130, 50), (125, 55), (126, 54), (125, 58), (134, 58)]:
    w, h = ref.size
    crop = cur.crop((ox, oy, ox + w, oy + h))
    d = 0
    n = 0
    hits = {10: 0, 20: 0, 30: 0, 45: 0}
    a = ref.load()
    b = crop.load()
    for y in range(h):
        for x in range(w):
            ar, ag, ab = a[x, y]
            br, bg, bb = b[x, y]
            m = max(abs(ar - br), abs(ag - bg), abs(ab - bb))
            d += m
            n += 1
            for t in hits:
                if m <= t:
                    hits[t] += 1
    print('crop +%d+%d mean=%.2f w10=%.1f w20=%.1f w30=%.1f w45=%.1f' %
          (ox, oy, d / n, 100.0 * hits[10] / n, 100.0 * hits[20] / n,
           100.0 * hits[30] / n, 100.0 * hits[45] / n))
