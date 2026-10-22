#!/usr/bin/env python3
"""Find an RGBA sprite inside a rendered screen by pixel voting."""
from PIL import Image
import sys

screen = Image.open(sys.argv[1]).convert('RGB')
sprite = Image.open(sys.argv[2]).convert('RGBA')
sp = sprite.load()
cp = screen.load()
sw, sh = sprite.size
# sample a grid of solid, opaque pixels from the sprite
samples = []
for y in range(0, sh, max(1, sh // 12)):
    for x in range(0, sw, max(1, sw // 12)):
        r, g, b, a = sp[x, y]
        if a > 200:
            samples.append((x, y, r, g, b))
print('sprite', sprite.size, 'samples', len(samples))

best = []
for oy in range(0, screen.size[1] - sh):
    for ox in range(0, screen.size[0] - sw):
        hit = 0
        for x, y, r, g, b in samples:
            cr, cg, cb = cp[ox + x, oy + y]
            if abs(cr - r) <= 6 and abs(cg - g) <= 6 and abs(cb - b) <= 6:
                hit += 1
        if hit > len(samples) * 0.8:
            best.append((hit, ox, oy))
best.sort(reverse=True)
print('matches:', len(best))
for b in best[:15]:
    print('  hit=%d/%d at +%d+%d' % (b[0], len(samples), b[1], b[2]))
