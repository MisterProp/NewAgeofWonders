#!/usr/bin/env python3
"""Measure the ScenarioMenuRef crop and locate it inside a port render."""
from PIL import Image, ImageChops
import sys

ref = Image.open(sys.argv[1] if len(sys.argv) > 1 else 'bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[2] if len(sys.argv) > 2 else 're/mw/cur_scen.png').convert('RGB')
print('ref', ref.size, 'cur', cur.size)


def bbox_nonblack(im, thresh=8):
    p = im.load()
    w, h = im.size
    xs, ys = [], []
    for y in range(h):
        for x in range(w):
            if max(p[x, y]) > thresh:
                xs.append(x)
                ys.append(y)
    return (min(xs), min(ys), max(xs), max(ys)) if xs else None


print('ref bbox nonblack>8:', bbox_nonblack(ref))
print('ref bbox nonblack>25:', bbox_nonblack(ref, 25))

# Print a coarse map of the ref: for each 10x10 block, the mean brightness.
rp = ref.load()
print('\nref 10x10 block mean brightness:')
for by in range(0, ref.size[1], 10):
    row = []
    for bx in range(0, ref.size[0], 10):
        n = 0
        s = 0
        for y in range(by, min(by + 10, ref.size[1])):
            for x in range(bx, min(bx + 10, ref.size[0])):
                r, g, b = rp[x, y]
                s += (r + g + b) // 3
                n += 1
        row.append('%3d' % (s // max(n, 1)))
    print('%3d ' % by + ' '.join(row))

cp = cur.load()
print('\ncur 10x10 block mean brightness:')
for by in range(0, cur.size[1], 10):
    row = []
    for bx in range(0, cur.size[0], 10):
        n = 0
        s = 0
        for y in range(by, min(by + 10, cur.size[1])):
            for x in range(bx, min(bx + 10, cur.size[0])):
                r, g, b = cp[x, y]
                s += (r + g + b) // 3
                n += 1
        row.append('%3d' % (s // max(n, 1)))
    print('%3d ' % by + ' '.join(row))

# Best integer offset of the reference crop inside the current render.
r = ref.resize((ref.size[0] // 2, ref.size[1] // 2), Image.BOX)
rw, rh = r.size
best = []
gp = r.load()
for oy in range(0, cur.size[1] - r.size[1] + 1, 2):
    for ox in range(0, cur.size[0] - r.size[0] + 1, 2):
        tot = 0
        n = 0
        for y in range(0, rh, 2):
            for x in range(0, rw, 2):
                br, bg, bb = gp[x, y]
                cr, cg, cb = cp[ox + x * 2, oy + y * 2]
                tot += abs(br - cr) + abs(bg - cg) + abs(bb - cb)
                n += 3
        best.append((tot / n, ox, oy))
best.sort()
print('\nbest ref-crop offsets (score, x, y):')
for b in best[:12]:
    print('  %.2f  +%d+%d' % b)
