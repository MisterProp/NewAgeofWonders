from PIL import Image

r = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
c = Image.open('re/mw/fresh2.png').convert('RGB')
rp, cp = r.load(), c.load()


def bb(p, x0, x1, y0, y1, ox=0, oy=0, lim=230):
    xs, ys = [], []
    for y in range(y0, y1):
        for x in range(x0, x1):
            q = p[x + ox, y + oy]
            if sum(q) < lim:
                xs.append(x)
                ys.append(y)
    return (min(xs), min(ys), max(xs), max(ys)) if xs else None


print('title ref', bb(rp, 30, 220, 2, 19), 'cur', bb(cp, 30, 220, 2, 19, 134, 37))
print('select ref', bb(rp, 40, 170, 332, 354), 'cur',
      bb(cp, 40, 170, 332, 354, 134, 37))
print('cancel ref', bb(rp, 195, 335, 332, 354), 'cur',
      bb(cp, 195, 335, 332, 354, 134, 37))
print('drive ref', bb(rp, 25, 130, 50, 75), 'cur',
      bb(cp, 25, 130, 50, 75, 134, 37))
print('row1 ref', bb(rp, 30, 250, 108, 128, lim=260), 'cur',
      bb(cp, 30, 250, 108, 128, 134, 37, lim=260))
