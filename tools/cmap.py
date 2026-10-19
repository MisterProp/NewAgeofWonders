"""cmap.py - print a crop of a PNG as a colour-family character map, so layout
AND colour can be checked without viewing the image.

    python tools/cmap.py <png> [x0 y0 x1 y1] [cols]

Legend: K black  D dark  d dim  g grey  w white  R red  O orange  Y yellow
        G green  T teal  B blue  P purple  N brown  . background
"""
import sys

from PIL import Image


def classify(r, g, b):
    mx = max(r, g, b)
    mn = min(r, g, b)
    v = (r * 30 + g * 59 + b * 11) // 100
    sat = mx - mn
    if sat < 24:
        if v < 26:
            return 'K'
        if v < 60:
            return 'D'
        if v < 110:
            return 'd'
        if v < 170:
            return 'g'
        if v < 225:
            return 'G'  # light grey -> use 'w'?
        return 'w'
    # hue
    if r >= g and r >= b:
        if g > b * 1.25:
            return 'O' if v > 120 else 'N'
        if b > g * 1.25:
            return 'P'
        return 'R' if v > 90 else 'N'
    if g >= r and g >= b:
        if r > b * 1.3:
            return 'Y'
        if b > r * 1.3:
            return 'T'
        return 'G'
    # blue dominant
    if r > g * 1.2:
        return 'P'
    if g > r * 1.2:
        return 'T'
    return 'B' if v > 90 else 'b'


def main():
    p = sys.argv[1]
    im = Image.open(p).convert('RGB')
    a = sys.argv[2:]
    if len(a) >= 4:
        x0, y0, x1, y1 = (int(v) for v in a[:4])
        im = im.crop((x0, y0, x1, y1))
    cols = int(a[4]) if len(a) >= 5 else 100
    cols = max(8, cols)
    rows = max(1, int(im.height * cols / im.width / 2.1))
    small = im.resize((cols, rows), Image.LANCZOS)
    for y in range(rows):
        print(''.join(classify(*small.getpixel((x, y))) for x in range(cols)))


main()
