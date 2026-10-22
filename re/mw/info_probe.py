from PIL import Image

r = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
c = Image.open('re/mw/fresh5.png').convert('RGB')
rp, cp = r.load(), c.load()
OX, OY = 134, 37
out = open('re/mw/info_probe_out.txt', 'w', encoding='utf-8')

for x in (200, 60, 160, 290):
    out.write('=== ref col %d / cur col %d ===\n' % (x, x + OX))
    for y in range(238, 330):
        a = rp[x, y]
        b = cp[x + OX, y + OY]
        if y % 1 == 0:
            out.write('%3d ref=%3d,%3d,%3d cur=%3d,%3d,%3d\n' %
                      (y, a[0], a[1], a[2], b[0], b[1], b[2]))

# bright / colored icon bboxes
def bb(p, x0, x1, y0, y1, pred, ox=0, oy=0):
    xs, ys = [], []
    for y in range(y0, y1):
        for x in range(x0, x1):
            q = p[x + ox, y + oy]
            if pred(q):
                xs.append(x)
                ys.append(y)
    return (min(xs), min(ys), max(xs), max(ys)) if xs else None

tests = [
    ('blue', lambda q: q[2] > 80 and q[2] > q[0] + 20),
    ('gold', lambda q: q[0] > 150 and q[1] > 130 and q[2] < 150),
    ('bright', lambda q: sum(q) > 380),
]
for name, pred in tests:
    out.write('%s ref %s cur %s\n' %
              (name, bb(rp, 25, 350, 250, 325, pred),
               bb(cp, 25, 350, 250, 325, pred, OX, OY)))
out.close()
