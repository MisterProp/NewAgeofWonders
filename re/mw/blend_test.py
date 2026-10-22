#!/usr/bin/env python3
"""Try blend modes / offsets for the two-layer info icons against the ref."""
from PIL import Image
import os

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
rp = ref.load()
tmp = 'bin/_sheet2'
cases = {
    'size': (20, 270, 125, 330, ['eIcons.ILB_8.png', 'eIcons.ILB_9.png']),
    'towns': (125, 270, 235, 330, ['eIcons.ILB_10.png', 'eIcons.ILB_11.png']),
    'players': (235, 270, 350, 330, ['eTitleM.ILB_7.png', 'eTitleM.ILB_8.png']),
}


def compose(bg, layers, mode):
    out = [list(c) for row in bg for c in row]
    w = len(bg[0])
    for sp in layers:
        sw, sh = sp.size
        p = sp.load()
        for y in range(sh):
            for x in range(sw):
                r, g, b = p[x, y]
                i = y * w + x
                if mode == 'add':
                    out[i][0] = min(255, out[i][0] + r)
                    out[i][1] = min(255, out[i][1] + g)
                    out[i][2] = min(255, out[i][2] + b)
                elif mode == 'key':
                    if (r, g, b) != (0, 0, 0):
                        out[i] = [r, g, b]
    return out


for name, (x0, y0, x1, y1, files) in cases.items():
    layers = [Image.open(os.path.join(tmp, f)).convert('RGB') for f in files]
    print('\n%s layers %s sizes %s' % (name, files, [i.size for i in layers]))
    best = []
    w, h = x1 - x0, y1 - y0
    bg = Image.new('RGB', (w + 60, h + 60), (41, 49, 24))
    for mode in ('key', 'add'):
        # mode describes layer 0; layer 1 is always keyed
        for oy in range(0, 25):
            for ox in range(0, 35):
                test = bg.copy()
                tp = test.load()
                sp0, sp1 = layers
                for layer, m in ((sp0, mode), (sp1, 'key')):
                    sp = layer.load()
                    sw, sh = layer.size
                    for y in range(sh):
                        for x in range(sw):
                            r, g, b = sp[x, y]
                            if m == 'key' and (r, g, b) == (0, 0, 0):
                                continue
                            tr, tg, tb = tp[20 + ox + x, 20 + oy + y]
                            if m == 'add':
                                tp[20 + ox + x, 20 + oy + y] = (
                                    min(255, tr + r), min(255, tg + g),
                                    min(255, tb + b))
                            else:
                                tp[20 + ox + x, 20 + oy + y] = (r, g, b)
                tp_ref = Image.new('RGB', (w, h))
                tp_ref.paste(test, (-x0 + 20, -y0 + 20))
                a = tp_ref.load()
                tot = n = 0
                for y in range(h):
                    for x in range(w):
                        ar, ag, ab = rp[x0 + x, y0 + y]
                        br, bgc, bb = a[x, y]
                        # only compare pixels the icon touched
                        if (br, bgc, bb) == (41, 49, 24):
                            continue
                        tot += abs(ar - br) + abs(ag - bgc) + abs(ab - bb)
                        n += 3
                if n and (not best or tot / n < best[0][0]):
                    best.append((tot / n, mode, ox, oy, n // 3))
    best.sort(key=lambda z: z[0])
    for b in best[:5]:
        print('  mean=%6.2f mode0=%-3s offset=%d,%d px=%d' % b)
