"""Precise tab-button edge detection in ref vs cur."""
from PIL import Image

ref = Image.open('bin/LowerHalfRef.png').convert('RGB')
cur = Image.open('bin/cur_game.png').convert('RGB')
rp = ref.load()
cp = cur.load()

def is_tan(p):
    r, g, b = p
    return r > 140 and g > 120 and b < 130 and r - b > 30

def edges(px, y, x0, x1):
    es = []
    prev = None
    for x in range(x0, x1):
        t = is_tan(px[x, y])
        if prev is not None and t != prev:
            es.append(x)
        prev = t
    return es

for y in [10, 12, 14]:
    print('REF y=%d edges:' % y, edges(rp, y, 0, 631))
for y in [440, 442, 444]:
    print('CUR y=%d edges:' % y, edges(cp, y, 0, 640))
