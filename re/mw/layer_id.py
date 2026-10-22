from PIL import Image

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open('re/mw/fresh13.png').convert('RGB')
rp, cp = ref.load(), cur.load()
OX, OY = 134, 37

for pos in range(7, 15):
    im = Image.open('bin/_sheet2/eTitleM.ILB_%d.png' % pos).convert('RGBA')
    sp = im.load()
    colors = set()
    for y in range(im.height):
        for x in range(im.width):
            r, g, b, a = sp[x, y]
            if a > 200 and (r + g + b) > 90:
                colors.add((r, g, b))
    for tag, p, ox, oy in (('ref', rp, 0, 0), ('cur', cp, OX, OY)):
        n = 0
        xs, ys = [], []
        for y in range(278, 316):
            for x in range(248, 330):
                if p[x + ox, y + oy] in colors:
                    n += 1
                    xs.append(x)
                    ys.append(y)
        if n > 30:
            print('TitleM %d %s match=%d bbox=%s' %
                  (pos, tag, n, (min(xs), min(ys), max(xs), max(ys))))
