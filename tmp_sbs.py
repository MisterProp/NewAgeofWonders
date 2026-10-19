"""tmp_sbs.py - stack the reference and the port's banner crop for comparison.

    python tmp_sbs.py <ref.png> <render.png> <out.png> [scale]
"""
import sys

sys.path.insert(0, '.')
import tmp_facecmp as F
import tmp_zoom as Z

OX, OY = 443, 220


def main():
    ref, big, out = sys.argv[1], sys.argv[2], sys.argv[3]
    sc = int(sys.argv[4]) if len(sys.argv) > 4 else 3
    rw, rh, rb, rr = F.decode(ref)
    bw, bh, bb, br = F.decode(big)
    W, H = rw * sc, (rh * 2 + 4) * sc
    rows = []
    for half in range(2):
        src_w = rw if half == 0 else bw
        for y in range(rh):
            line = bytearray()
            for x in range(rw):
                if half == 0:
                    r, g, b = F.px(rr, rb, x, y)
                else:
                    r, g, b = F.px(br, bb, OX + x, OY + y)
                line += bytes([r, g, b, 255]) * sc
            for _ in range(sc):
                rows.append(bytes(line))
        if half == 0:
            for _ in range(4 * sc):
                rows.append(bytes([255, 0, 0, 255] * W))
    Z.write_png(out, W, H, 4, rows)
    print('%s (%s top / %s bottom) -> %s %dx%d' % (out, ref, big, out, W, H))


if __name__ == '__main__':
    main()
