#!/usr/bin/env python3
"""Mask-based alignment of individual text runs / bands (font-shaped masks)."""
from PIL import Image
import sys

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open(sys.argv[1] if len(sys.argv) > 1 else 're/mw/fresh3.png').convert('RGB')
rp, cp = ref.load(), cur.load()
OX, OY = 134, 37


def mask(p, x0, y0, x1, y1, pred, ox=0, oy=0):
    pts = []
    for y in range(y0, y1):
        for x in range(x0, x1):
            if pred(p[x + ox, y + oy]):
                pts.append((x, y))
    return pts


def align(name, box, pred):
    a = mask(rp, *box, pred)
    if len(a) < 30:
        print(name, 'ref mask too small', len(a))
        return
    best = None
    for oy in range(-6, 7):
        for ox in range(-12, 13):
            b = set(mask(cp, *box, pred, OX + ox, OY + oy))
            sa = set(a)
            inter = len(sa & b)
            score = inter - (len(sa - b) + len(b - sa)) * 0.5
            if best is None or score > best[0]:
                best = (score, ox, oy, inter, len(sa), len(b))
    print('%-12s screen offset %+d,%+d  inter=%d/%d cur=%d' %
          ((name,) + best[1:]))


light = lambda q: q[0] > 110 and q[1] > 120 and q[2] > 85
dark = lambda q: sum(q) < 210
align('title', (30, 3, 220, 19), dark)
align('list-rows', (40, 108, 350, 243), light)
align('drive-lbl', (25, 56, 70, 75), dark)
align('path-lbl', (25, 78, 68, 98), dark)
align('path-text', (70, 78, 300, 98), dark)
align('select', (40, 335, 175, 352), dark)
align('cancel', (205, 335, 332, 352), dark)
align('band', (20, 22, 350, 45), lambda q: sum(q) > 400)
align('bottom-band', (20, 324, 350, 354), lambda q: sum(q) > 400)
align('globe', (25, 275, 110, 325), lambda q: q[2] > 90 and q[2] > q[0])
align('castle', (130, 275, 225, 325), lambda q: q[0] > 120 and q[1] > 100)
align('face', (235, 275, 330, 325), lambda q: q[0] > 90 and q[2] < 100)
