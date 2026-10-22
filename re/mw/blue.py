from PIL import Image

out = open('re/mw/blue.txt', 'w')
for fn in ('fresh3', 'fresh5'):
    c = Image.open('re/mw/%s.png' % fn).convert('RGB')
    p = c.load()
    for tag, ox, oy in (('crop', 134, 37),):
        xs, ys = [], []
        for y in range(280, 315):
            for x in range(25, 140):
                q = p[x + ox, y + oy]
                if q[2] > 90 and q[2] > q[0] + 25:
                    xs.append(x)
                    ys.append(y)
        out.write('%s %s blue bbox %s\n' %
                  (fn, tag, (min(xs), min(ys), max(xs), max(ys)) if xs else None))
out.close()
