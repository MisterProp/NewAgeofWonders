#!/usr/bin/env python3
"""pixmatch.py - one-number visual match between a render and a reference.

    python tools/pixmatch.py render.png reference.png [--rect x y w h]

Prints the share of pixels that agree within a per-channel tolerance (a
"match %%"), plus the mean absolute difference.  PIL only, so it runs with the
same stdlib-plus-Pillow setup as the rest of port/tools.
"""
import sys

from PIL import Image, ImageChops


def main():
    a_path, b_path = sys.argv[1], sys.argv[2]
    rect = None
    if "--rect" in sys.argv:
        i = sys.argv.index("--rect")
        rect = tuple(int(v) for v in sys.argv[i + 1:i + 5])
    A = Image.open(a_path).convert("RGB")
    B = Image.open(b_path).convert("RGB")
    if A.size != B.size:
        B = B.resize(A.size, Image.LANCZOS)
    if rect:
        A = A.crop(rect)
        B = B.crop(rect)
    d = ImageChops.difference(A, B)
    px = list(d.getdata())
    n = len(px)
    tot = 0
    hits = {10: 0, 20: 0, 30: 0, 45: 0, 60: 0}
    for r, g, bl in px:
        m = max(r, g, bl)
        tot += r + g + bl
        for t in hits:
            if m <= t:
                hits[t] += 1
    print("mean_abs_diff_per_channel = %.2f" % (tot / (3.0 * n)))
    for t in sorted(hits):
        print("match_within_%2d/channel = %5.1f%%" % (t, 100.0 * hits[t] / n))
    print("pixels = %d" % n)


main()
