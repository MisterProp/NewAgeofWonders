from PIL import Image
import collections, sys


def set_key(path):
    im = Image.open(path).convert('RGBA')
    w, h = im.size
    px = im.load()
    ring = []

    def push(x, y):
        p = px[x, y]
        if p[3] == 0:
            return
        ring.append(p[:3])

    for x in range(w):
        push(x, 0)
        push(x, h - 1)
    for y in range(1, h - 1):
        push(0, y)
        push(w - 1, y)
    c = collections.Counter(ring)
    best, bestn = c.most_common(1)[0]
    ok = bestn * 2 >= len(ring)
    print(path, 'size', (w, h), 'ring', len(ring), 'best', best, bestn,
          'dominant_ok', ok)
    print('  top5', c.most_common(5))


for n in sys.argv[1:]:
    set_key(n)