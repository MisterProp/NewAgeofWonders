#!/usr/bin/env python3
from PIL import Image
import sys

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[1] if len(sys.argv) > 1 else 're/mw/fresh2.png').convert('RGB')
r = ref.load()
c = cur.load()
OX, OY = 134, 37


def bbox(im, p, x0, y0, x1, y1, pred, ox=0, oy=0):
    xs, ys = [], []
    for y in range(y0, y1):
        for x in range(x0, x1):
            if pred(p[x + ox, y + oy]):
                xs.append(x)
                ys.append(y)
    return (min(xs), min(ys), max(xs), max(ys)) if xs else None


print('title dark:',
      'ref', bbox(ref, r, 15, 0, 200, 20, lambda q: sum(q) < 200),
      'cur', bbox(cur, c, 15, 0, 200, 20, lambda q: sum(q) < 200, OX, OY))
print('row1 bright:',
      'ref', bbox(ref, r, 30, 108, 220, 128,
                  lambda q: q[0] > 90 and q[1] > 100 and q[2] > 70),
      'cur', bbox(cur, c, 30, 108, 220, 128,
                  lambda q: q[0] > 90 and q[1] > 100 and q[2] > 70, OX, OY))
print('row2 bright:',
      'ref', bbox(ref, r, 30, 125, 260, 145,
                  lambda q: q[0] > 90 and q[1] > 100 and q[2] > 70),
      'cur', bbox(cur, c, 30, 125, 260, 145,
                  lambda q: q[0] > 90 and q[1] > 100 and q[2] > 70, OX, OY))
print('btn text:',
      'ref', bbox(ref, r, 40, 330, 330, 355, lambda q: sum(q) < 170),
      'cur', bbox(cur, c, 40, 330, 330, 355, lambda q: sum(q) < 170, OX, OY))
print('title icon/close bright:',
      'ref', bbox(ref, r, 0, 0, 369, 22,
                  lambda q: q[2] > 160 and q[0] > 150),
      'cur', bbox(cur, c, 0, 0, 369, 22,
                  lambda q: q[2] > 160 and q[0] > 150, OX, OY))
print('band bright:',
      'ref', bbox(ref, r, 20, 22, 350, 46,
                  lambda q: sum(q) > 430),
      'cur', bbox(cur, c, 20, 22, 350, 46,
                  lambda q: sum(q) > 430, OX, OY))
