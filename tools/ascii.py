"""ascii.py - print a crop of a PNG as ASCII brightness art (layout cross-check
without needing to view the image).

    python tools/ascii.py <png> [x0 y0 x1 y1] [cols]
"""
import sys

from PIL import Image

RAMP = ' .:-=+*#%@'


def main():
    p = sys.argv[1]
    im = Image.open(p).convert('RGB')
    a = sys.argv[2:]
    if len(a) >= 4:
        x0, y0, x1, y1 = (int(v) for v in a[:4])
        im = im.crop((x0, y0, x1, y1))
    cols = int(a[4]) if len(a) >= 5 else 80
    cols = max(8, cols)
    rows = max(1, int(im.height * cols / im.width / 2.1))
    small = im.resize((cols, rows), Image.LANCZOS)
    for y in range(rows):
        line = []
        for x in range(cols):
            r, g, b = small.getpixel((x, y))
            v = (r * 30 + g * 59 + b * 11) // 100
            line.append(RAMP[min(9, v * 10 // 256)])
        print(''.join(line))


main()
