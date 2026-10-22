#!/usr/bin/env python3
"""Difference of scenario with/without the picker -> dialog bbox + shadow."""
from PIL import Image, ImageChops

a = Image.open('re/mw/fresh_nomaps.png').convert('RGB')
b = Image.open('re/mw/fresh.png').convert('RGB')
d = ImageChops.difference(a, b)
p = d.load()
w, h = d.size
xs, ys = [], []
for y in range(h):
    for x in range(w):
        if max(p[x, y]) > 3:
            xs.append(x)
            ys.append(y)
print('changed bbox:', min(xs), min(ys), max(xs), max(ys))
# per-row / per-col extents
rows = {}
for (x, y) in zip(xs, ys):
    rows[y] = max(rows.get(y, 0), x)
print('last changed row:', max(ys))
for y in (0, 30, 35, 36, 40, 100, 200, 300, 390, 397, 398, 400, 420, 460, 479):
    print('row', y, 'changed', sum(1 for yy in ys if yy == y),
          'xmax', rows.get(y))
# count differing pixels per x for the dialog region
cols = {}
for (x, y) in zip(xs, ys):
    cols[x] = cols.get(x, 0) + 1
print('col extent first/last:', min(xs), max(xs))
