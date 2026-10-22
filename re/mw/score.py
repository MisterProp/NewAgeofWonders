#!/usr/bin/env python3
"""Score a scenario render against ScenarioMenuRef.png and write a report."""
from PIL import Image, ImageChops
import sys

out = open('re/mw/score_out.txt', 'w', encoding='utf-8')
ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[1] if len(sys.argv) > 1 else 're/mw/fresh3.png').convert('RGB')
rp, cp = ref.load(), cur.load()
W, H = ref.size


def score(ox, oy):
    tot = n = 0
    hits = {10: 0, 20: 0, 30: 0, 45: 0}
    for y in range(H):
        for x in range(W):
            ar, ag, ab = rp[x, y]
            br, bg, bb = cp[ox + x, oy + y]
            m = max(abs(ar - br), abs(ag - bg), abs(ab - bb))
            tot += m
            n += 1
            for t in hits:
                if m <= t:
                    hits[t] += 1
    return (tot / n, [100.0 * hits[t] / n for t in (10, 20, 30, 45)])


best = None
for oy in range(30, 42):
    for ox in range(128, 142):
        v = score(ox, oy)
        if best is None or v[0] < best[0][0]:
            best = (v, ox, oy)
out.write('best origin +%d+%d\n' % (best[1], best[2]))
out.write('mean=%.2f w10=%.1f w20=%.1f w30=%.1f w45=%.1f\n' %
          ((best[0][0],) + tuple(best[0][1])))
v = score(134, 37)
out.write('origin 134,37 mean=%.2f w10=%.1f w20=%.1f w30=%.1f w45=%.1f\n' %
          ((v[0],) + tuple(v[1])))
regions = {'title': (8, 0, 360, 20), 'band1': (8, 20, 360, 46),
           'drive/path': (15, 50, 355, 108), 'list': (20, 104, 360, 250),
           'info': (15, 245, 355, 322), 'buttons': (15, 322, 355, 356)}
for name, (x0, y0, x1, y1) in regions.items():
    b = None
    for oy in range(-8, 9):
        for ox in range(-8, 9):
            t = n = 0
            for y in range(y0, y1, 2):
                for x in range(x0, x1, 2):
                    ar, ag, ab = rp[x, y]
                    br, bg, bb = cp[135 + x + ox, 35 + y + oy]
                    t += abs(ar - br) + abs(ag - bg) + abs(ab - bb)
                    n += 3
            vv = t / n
            if b is None or vv < b[0]:
                b = (vv, ox, oy)
    out.write('%-11s %+d,%+d mean %.2f\n' % (name, b[1], b[2], b[0]))

ox, oy = 134, 37
crop = cur.crop((ox, oy, ox + W, oy + H))
d = ImageChops.difference(ref, crop).point(lambda v: min(255, v * 4))
d.save('re/mw/diff4.png')
canvas = Image.new('RGB', (W * 3 + 20, H), (255, 0, 0))
canvas.paste(ref, (0, 0))
canvas.paste(crop, (W + 10, 0))
canvas.paste(Image.blend(ref, crop, 0.5), (W * 2 + 20, 0))
canvas.resize((canvas.width * 2, canvas.height * 2), Image.NEAREST).save(
    're/mw/cmp4.png')
out.close()
