from PIL import Image

r = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
c = Image.open('re/mw/fresh5.png').convert('RGB')
rp, cp = r.load(), c.load()
OX, OY = 134, 37
out = open('re/mw/icon_bbox_out.txt', 'w', encoding='utf-8')
wells = [(30, 140), (140, 245), (245, 350)]
for wi, (x0, x1) in enumerate(wells):
    for tag, p, ox, oy in (('ref', rp, 0, 0), ('cur', cp, OX, OY)):
        # bright icon pixels only, inside the well body (avoid the frame)
        xs, ys = [], []
        for y in range(282, 313):
            for x in range(x0, x1):
                q = p[x + ox, y + oy]
                if sum(q) > 300 and not (q[0] > 140 and q[1] > 100 and q[2] < 100):
                    xs.append(x)
                    ys.append(y)
        out.write('well%d %s bright bbox %s\n' %
                  (wi, tag, (min(xs), min(ys), max(xs), max(ys)) if xs else None))
        # average brightness
        tot = n = 0
        for y in range(282, 313):
            for x in range(x0, x1):
                q = p[x + ox, y + oy]
                tot += sum(q)
                n += 3
        out.write('well%d %s mean brightness %.1f\n' % (wi, tag, tot / n))
out.close()
