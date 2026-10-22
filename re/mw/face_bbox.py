from PIL import Image

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
cur = Image.open('re/mw/fresh13.png').convert('RGB')
rp, cp = ref.load(), cur.load()
OX, OY = 134, 37
for tag, p, ox, oy in (('ref', rp, 0, 0), ('cur', cp, OX, OY)):
    for name, pred in (
            ('tan', lambda q: q[0] > 120 and q[1] > 90 and q[2] < 130),
            ('purple', lambda q: q[2] > 90 and q[2] > q[0] + 15),):
        xs, ys = [], []
        for y in range(278, 316):
            for x in range(248, 310):
                q = p[x + ox, y + oy]
                if pred(q):
                    xs.append(x)
                    ys.append(y)
        print(tag, name,
              (min(xs), min(ys), max(xs), max(ys)) if xs else None,
              len(xs))
