#!/usr/bin/env python3
from PIL import Image
import sys

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[1]).convert('RGB')
rp, cp = ref.load(), cur.load()
w, h = ref.size
print('ref', ref.size, 'cur', cur.size)

# gold-ish dialog frame bbox
xs, ys = [], []
for y in range(cur.size[1]):
    for x in range(cur.size[0]):
        r, g, b = cp[x, y]
        if r > 150 and g > 125 and 40 < b < 120:
            xs.append(x)
            ys.append(y)
print('gold bbox:', min(xs), min(ys), max(xs), max(ys))

def score(ox, oy, label):
    crop = cur.crop((ox, oy, ox + w, oy + h))
    b = crop.load()
    d = 0
    n = 0
    hits = {10: 0, 20: 0, 30: 0, 45: 0}
    for y in range(h):
        for x in range(w):
            ar, ag, ab = rp[x, y]
            br, bg, bb = b[x, y]
            m = max(abs(ar - br), abs(ag - bg), abs(ab - bb))
            d += m
            n += 1
            for t in hits:
                if m <= t:
                    hits[t] += 1
    print('%s +%d+%d mean=%.2f w10=%.1f w20=%.1f w30=%.1f w45=%.1f' %
          (label, ox, oy, d / n, 100.0 * hits[10] / n, 100.0 * hits[20] / n,
           100.0 * hits[30] / n, 100.0 * hits[45] / n))
    return crop

for oy in (30, 35, 40, 50, 55, 58, 60):
    score(130, oy, 'crop')
for ox in (120, 125, 130, 135, 140):
    score(ox, 35, 'crop')

a = score(130, 35, 'best35')
b = score(130, 58, 'best58')
canvas = Image.new('RGB', (w * 3 + 20, h), (255, 0, 0))
canvas.paste(ref, (0, 0))
canvas.paste(a, (w + 10, 0))
canvas.paste(b, (w * 2 + 20, 0))
canvas.save('re/mw/cmp_scen2.png')
canvas.resize((canvas.size[0] * 2, canvas.size[1] * 2), Image.NEAREST).save(
    're/mw/cmp_scen2_big.png')
