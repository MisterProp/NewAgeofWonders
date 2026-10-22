from PIL import Image
import sys

cur = sys.argv[1] if len(sys.argv) > 1 else 're/mw/fresh6.png'
r = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
c = Image.open(cur).convert('RGB')
rp, cp = r.load(), c.load()
OX, OY = 134, 37
out = open('re/mw/probe_areas_out.txt', 'w', encoding='utf-8')


def mean(p, x0, x1, y0, y1, ox=0, oy=0):
    s = [0, 0, 0]
    n = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            q = p[x + ox, y + oy]
            s[0] += q[0]
            s[1] += q[1]
            s[2] += q[2]
            n += 1
    return tuple(round(v / n, 1) for v in s)


areas = {
    'list-clean': (250, 330, 160, 200),
    'list-clean2': (70, 120, 140, 160),
    'info-outer': (60, 300, 262, 274),
    'well1-body': (40, 130, 286, 310),
    'body-left': (18, 28, 60, 300),
    'button-strip': (40, 330, 326, 332),
}
for name, box in areas.items():
    out.write('%-12s ref=%s cur=%s\n' %
              (name, mean(rp, *box), mean(cp, *box, OX, OY)))


def textbb(p, x0, x1, y0, y1, pred, ox=0, oy=0):
    xs, ys = [], []
    for y in range(y0, y1):
        for x in range(x0, x1):
            if pred(p[x + ox, y + oy]):
                xs.append(x)
                ys.append(y)
    return (min(xs), min(ys), max(xs), max(ys)) if xs else None


dark = lambda q: sum(q) < 230
for name, box in {'title': (25, 230, 2, 19),
                  'path-text': (70, 330, 78, 99),
                  'drive-text': (130, 330, 56, 75)}.items():
    out.write('%s ref=%s cur=%s\n' %
              (name, textbb(rp, *box, dark),
               textbb(cp, *box, dark, OX, OY)))
out.close()
