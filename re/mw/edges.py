#!/usr/bin/env python3
from PIL import Image
import sys

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[1] if len(sys.argv) > 1 else 're/mw/fresh.png').convert('RGB')
rp, cp = ref.load(), cur.load()


def line(p, fixed, lo, hi, axis='x', tag=''):
    out = []
    for v in range(lo, hi):
        x, y = (v, fixed) if axis == 'x' else (fixed, v)
        r, g, b = p[x, y]
        out.append('%d:%d,%d,%d' % (v, r, g, b))
    print(tag, ' '.join(out))


print('=== ref (crop coords) ===')
line(rp, 200, 0, 45, 'x', 'ref row200')
line(rp, 200, 300, 369, 'x', 'ref row200R')
line(rp, 100, 0, 45, 'x', 'ref row100')
line(rp, 100, 320, 369, 'x', 'ref row100R')
line(rp, 200, 0, 30, 'y', 'ref col200')
line(rp, 200, 220, 359, 'y', 'ref col200B')
print('=== cur ===')
line(cp, 200, 120, 165, 'x', 'cur row200')
line(cp, 200, 460, 520, 'x', 'cur row200R')
line(cp, 100, 120, 165, 'x', 'cur row100')
line(cp, 100, 460, 520, 'x', 'cur row100R')
line(cp, 200, 25, 60, 'y', 'cur col200T')
line(cp, 200, 340, 420, 'y', 'cur col200B')
