from PIL import Image

r = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
c = Image.open('re/mw/fresh5.png').convert('RGB')
rp, cp = r.load(), c.load()
OX, OY = 134, 37
out = open('re/mw/icon_bbox2_out.txt', 'w', encoding='utf-8')

regions = {'globe': (25, 140), 'castle': (140, 245), 'face': (245, 350)}
preds = {
    'globe': lambda q: q[2] > 90 and q[2] > q[0] + 25,
    'castle': lambda q: q[0] > 140 and q[1] > 110 and q[2] < 180,
    'face': lambda q: q[0] > 90 and q[0] > q[2] + 20 and q[1] < q[0],
}
for tag, (x0, x1) in regions.items():
    for nm, p, ox, oy in (('ref', rp, 0, 0), ('cur', cp, OX, OY)):
        for y0, y1 in ((282, 313), (280, 318)):
            xs, ys = [], []
            for y in range(y0, y1):
                for x in range(x0, x1):
                    if preds[tag](p[x + ox, y + oy]):
                        xs.append(x)
                        ys.append(y)
            out.write('%s %s y%d..%d %s\n' %
                      (tag, nm, y0, y1,
                       (min(xs), min(ys), max(xs), max(ys)) if xs else None))
out.close()
