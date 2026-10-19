"""imgdiff.py - compare two renders (or a render against the original screenshot)
without needing to view either image.

    python tools/imgdiff.py <a.png> <b.png> [cols] [--crop x0 y0 x1 y1]

Prints, per tile of a cols-wide grid:
  * the mean RGB of A, of B and the per-channel delta, and
  * a signed heat character for the delta magnitude
so a mismatch region can be located and measured numerically.

Also writes `<a>_vs_<b>.png` next to A: A, B and the amplified difference,
stacked, for a human to eyeball.
"""
import sys

from PIL import Image, ImageChops, ImageDraw

RAMP = ' .:-=+*#%@'


def mean(im, box):
    return im.crop(box).resize((1, 1), Image.BOX).getpixel((0, 0))


def main():
    a_path, b_path = sys.argv[1], sys.argv[2]
    rest = sys.argv[3:]
    cols = 60
    crop = None
    if '--crop' in rest:
        i = rest.index('--crop')
        crop = tuple(int(v) for v in rest[i + 1:i + 5])
        rest = rest[:i] + rest[i + 5:]
    if rest:
        cols = int(rest[0])

    A = Image.open(a_path).convert('RGB')
    B = Image.open(b_path).convert('RGB')
    if crop:
        A, B = A.crop(crop), B.crop(crop)
    if A.size != B.size:
        B = B.resize(A.size, Image.LANCZOS)
    w, h = A.size
    cols = max(8, cols)
    rows = max(1, int(h * cols / w / 2.1))
    tw, th = max(1, w // cols), max(1, h // rows)

    print('# %s vs %s   %dx%d   tile %dx%d' % (a_path, b_path, w, h, tw, th))
    print('# legend: heat = delta magnitude, blank < 8, @ >= 200')
    worst = []
    for r in range(rows):
        line = []
        for c in range(cols):
            box = (c * tw, r * th, min(w, (c + 1) * tw), min(h, (r + 1) * th))
            ma = mean(A, box)
            mb = mean(B, box)
            d = max(abs(ma[i] - mb[i]) for i in range(3))
            line.append(RAMP[min(9, d * 10 // 256)])
            worst.append((d, c * tw, r * th, ma, mb))
        print(''.join(line))
    worst.sort(reverse=True)
    print('# worst 12 tiles: delta, x, y, A_mean, B_mean')
    for d, x, y, ma, mb in worst[:12]:
        print('#  %3d  (%3d,%3d)  A=%s  B=%s' % (d, x, y, ma, mb))

    # side-by-side + difference
    diff = ImageChops.difference(A, B).point(lambda v: min(255, v * 4))
    sheet = Image.new('RGB', (w * 3 + 16, h), (20, 20, 24))
    for i, im in enumerate((A, B, diff)):
        sheet.paste(im, (i * (w + 8), 0))
    d = ImageDraw.Draw(sheet)
    for i, t in enumerate(('A', 'B', 'A-B x4')):
        d.text((i * (w + 8) + 4, 4), t, fill=(255, 220, 120))
    out = a_path.rsplit('.', 1)[0] + '_vs_' + b_path.replace('\\', '/').rsplit('/', 1)[-1].rsplit('.', 1)[0] + '.png'
    sheet.save(out)
    print('# wrote %s' % out)


main()
