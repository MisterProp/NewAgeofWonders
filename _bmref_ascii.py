from PIL import Image

im = Image.open('bin/BMRef.png').convert('RGB')
w, h = im.size
px = im.load()
print('size', w, h)


def cls(p):
    r, g, b = p
    # marker wall: bluish dark (b > g > r and not too bright)
    if b > r + 15 and b >= g:
        return 'b' if (r + g + b) < 200 else 'B'
    if r < 40 and g < 40 and b < 40:
        return 'k'
    if g > r and g > b:
        return '.'
    return '?'


# find bbox of blue-ish pixels
xs, ys = [], []
for y in range(h):
    for x in range(w):
        c = cls(px[x, y])
        if c in 'bB':
            xs.append(x)
            ys.append(y)
if xs:
    print('blue bbox', min(xs), min(ys), max(xs), max(ys))
for y in range(0, h, 2):
    print('r%02d' % y, ''.join(cls(px[x, y]) for x in range(w)))