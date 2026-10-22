from PIL import Image
from collections import Counter

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open('re/mw/fresh13.png').convert('RGB')
rp, cp = ref.load(), cur.load()
OX, OY = 134, 37
for tag, p, ox, oy in (('ref', rp, 0, 0), ('cur', cp, OX, OY)):
    c = Counter()
    for y in range(280, 314):
        for x in range(250, 310):
            c[p[x + ox, y + oy]] += 1
    print(tag, 'top colors', c.most_common(12))
    # brightest tan-ish pixel
    best = max(((q, (x, y)) for y in range(280, 314) for x in range(250, 310)
                for q in [p[x + ox, y + oy]] if q[0] > q[2] and q[0] > q[1]),
               key=lambda z: z[0][0])
    print('  brightest tan', best)

# sprite pos8 colors
im = Image.open('bin/_sheet2/eTitleM.ILB_8.png').convert('RGB')
sp = im.load()
c = Counter()
for y in range(im.height):
    for x in range(im.width):
        c[sp[x, y]] += 1
print('sprite pos8 top', c.most_common(12))
