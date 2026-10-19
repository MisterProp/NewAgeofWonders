"""tmp_citydiff_img.py - pixel A/B of two full-map renders (before/after the
race-less-city fix): counts the differing pixels and their bounding box, so the
change can be shown to be exactly the city bodies.

    python tmp_citydiff_img.py <before.png> <after.png>
"""
import sys

sys.path.insert(0, '.')
import tmp_zoom as Z


def main():
    b_path, a_path = sys.argv[1], sys.argv[2]
    bw, bh, bb, brows = Z.decode(b_path)
    aw, ah, ab, arows = Z.decode(a_path)
    print('%s %dx%d (bpp %d)  vs  %s %dx%d (bpp %d)' %
          (b_path, bw, bh, bb, a_path, aw, ah, ab))
    if (bw, bh) != (aw, ah) or bb != ab:
        raise SystemExit('size mismatch')
    diff = 0
    x0 = y0 = 10 ** 9
    x1 = y1 = -1
    for y in range(bh):
        rb, ra = brows[y], arows[y]
        for x in range(bw):
            o = x * bb
            if rb[o:o + bb] != ra[o:o + bb]:
                diff += 1
                if x < x0: x0 = x
                if x > x1: x1 = x
                if y < y0: y0 = y
                if y > y1: y1 = y
    total = bw * bh
    print('differing pixels: %d / %d (%.3f%%)' % (diff, total, 100.0 * diff / total))
    if diff:
        print('bounding box: x %d..%d  y %d..%d' % (x0, x1, y0, y1))
        print('  pixel (%d,%d): before %s after %s' %
              (x0, y0, tuple(brows[y0][x0 * bb:x0 * bb + 3]),
               tuple(arows[y0][x0 * bb:x0 * bb + 3])))
    # Attribute every differing pixel to the hex whose sprite box (mapviz's grid:
    # the hex field starts at +8/+8, pitch 32, odd columns staggered down 16) it
    # falls in, allowing the sprite's own overhang.
    per_hex = {}
    for y in range(bh):
        rb, ra = brows[y], arows[y]
        for x in range(bw):
            o = x * bb
            if rb[o:o + bb] == ra[o:o + bb]:
                continue
            hx = (x - 8) // 32
            hy = (y - 8 - ((hx & 1) * 16)) // 32
            best = None
            for cx in range(hx - 2, hx + 3):
                for cy in range(hy - 2, hy + 3):
                    ccx = 8 + cx * 32 + 16
                    ccy = 8 + cy * 32 + (cx & 1) * 16 + 16
                    d = max(abs(x - ccx) / 40.0, abs(y - ccy) / 40.0)
                    if d <= 1.0 and (best is None or d < best[0]):
                        best = (d, (cx, cy))
            key = best[1] if best else 'outside'
            per_hex[key] = per_hex.get(key, 0) + 1
    print('differing pixels per hex:')
    for k in sorted(per_hex, key=lambda k: str(k)):
        print('   %-10s %d' % (str(k), per_hex[k]))


if __name__ == '__main__':
    main()
