#!/usr/bin/env python3
"""Match the reference's three info icons against ILB sprites (additive blit)."""
from PIL import Image
import os

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
rp = ref.load()
tmp = 'bin/_sheet2'
regions = {'size': (20, 275, 125, 325), 'towns': (125, 275, 230, 325),
           'players': (230, 275, 350, 325)}
# background estimate: the ref well is dark green; take the median of the
# non-bright pixels in the region
for name, (x0, y0, x1, y1) in regions.items():
    bg = []
    for y in range(y0, y1):
        for x in range(x0, x1):
            r, g, b = rp[x, y]
            if r + g + b < 240:
                bg.append((r, g, b))
    bg.sort(key=lambda c: c[0] + c[1] + c[2])
    base = bg[len(bg) // 2] if bg else (40, 48, 24)
    print('\n%s base=%s' % (name, base))

cands = []
for fn in sorted(os.listdir(tmp)):
    if fn.startswith('eIcons.ILB_') or fn.startswith('eTitleM.ILB_'):
        p = os.path.join(tmp, fn)
        im = Image.open(p).convert('RGB')
        w, h = im.size
        if w > 80 or h > 80 or w < 4 or h < 4:
            continue
        cands.append((fn, im))

results = {}
for name, (x0, y0, x1, y1) in regions.items():
    best = []
    for fn, im in cands:
        sp = im.load()
        w, h = im.size
        tot = n = 0
        # Additive composite sprite over the median well background, compared
        # with the reference. Opaque-black sprite pixels add nothing, so all
        # pixels are comparable.
        for oy in range(y0, y1 - h + 1):
            for ox in range(x0, x1 - w + 1):
                t = 0
                cnt = 0
                for y in range(0, h, 2):
                    for x in range(0, w, 2):
                        sr, sg, sb = sp[x, y]
                        ar, ag, ab = rp[ox + x, oy + y]
                        # predicted additive pixel
                        pr = min(255, base[0] + sr)
                        pg = min(255, base[1] + sg)
                        pb = min(255, base[2] + sb)
                        t += abs(ar - pr) + abs(ag - pg) + abs(ab - pb)
                        cnt += 3
                v = t / cnt
                if not best or v < best[0][0]:
                    best.append((v, fn, ox, oy))
                elif v < 200:
                    best.append((v, fn, ox, oy))
    best.sort(key=lambda z: z[0])
    print(name, 'top candidates:')
    seen = set()
    shown = 0
    for v, fn, ox, oy in best:
        if fn in seen:
            continue
        seen.add(fn)
        print('   %-22s mean=%6.2f at +%d+%d' % (fn, v, ox, oy))
        shown += 1
        if shown >= 6:
            break
