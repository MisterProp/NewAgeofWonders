from PIL import Image

r = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
c = Image.open('re/mw/fresh12.png').convert('RGB')
rp, cp = r.load(), c.load()
OX, OY = 134, 37
out = open('re/mw/textcols_out.txt', 'w', encoding='utf-8')


def cols(p, x0, x1, y0, y1, pred, ox=0, oy=0, need=2):
    cols = []
    for x in range(x0, x1):
        n = 0
        for y in range(y0, y1):
            if pred(p[x + ox, y + oy]):
                n += 1
        if n >= need:
            cols.append(x)
    return (min(cols), max(cols), len(cols)) if cols else None


dark = lambda q: sum(q) < 190
light = lambda q: q[0] > 110 and q[1] > 120 and q[2] > 85
for name, box, pred in (
        ('title', (30, 260, 3, 19), dark),
        ('select', (45, 165, 336, 352), dark),
        ('cancel', (210, 325, 336, 352), dark),
        ('list-row1', (40, 250, 109, 127), light),
        ('path', (70, 330, 79, 98), dark)):
    out.write('%-10s ref %s cur %s\n' %
              (name, cols(rp, *box, pred),
               cols(cp, *box, pred, OX, OY)))
out.close()
