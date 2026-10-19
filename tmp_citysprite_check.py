"""tmp_citysprite_check.py - prove which city body sprite a render drew: count the
pixels around a city hex that match the opaque colours of two candidate ILB
sprites (the fix's before/after bodies), so the check needs no eyeballing.

    python tmp_citysprite_check.py <render.png> <canvas_x> <canvas_y> \
                                   <spriteA.png> <spriteB.png>
"""
import sys

sys.path.insert(0, '.')
import tmp_zoom as Z


def colours(path):
    w, h, bpp, rows = Z.decode(path)
    out = set()
    for y in range(h):
        for x in range(w):
            o = x * bpp
            px = tuple(rows[y][o:o + 3])
            if bpp == 3 or rows[y][o + 3] != 0:
                out.add(px)
    return out


def main():
    render, sx, sy, pa, pb = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), \
        sys.argv[4], sys.argv[5]
    ca, cb = colours(pa), colours(pb)
    print('%s: %d opaque colours;  %s: %d opaque colours' %
          (pa, len(ca), pb, len(cb)))
    # A sprite is drawn keyed - its opaque pixels land on the canvas unchanged.
    # The pixel sets are (nearly) disjoint, so exclusive matches identify the art.
    only_a = ca - cb
    only_b = cb - ca
    w, h, bpp, rows = Z.decode(render)
    BW, BH = 120, 110  # the region a City1 sprite box can occupy
    hits_a = hits_b = pixels = 0
    for y in range(max(0, sy), min(h, sy + BH)):
        for x in range(max(0, sx), min(w, sx + BW)):
            o = x * bpp
            px = tuple(rows[y][o:o + 3])
            if rows[y][o + 3] == 0 and bpp == 4:
                continue
            pixels += 1
            if px in only_a:
                hits_a += 1
            elif px in only_b:
                hits_b += 1
    print('%s: region (%d,%d)+%dx%d -> %d sampled px; exclusive matches:' %
          (render, sx, sy, BW, BH, pixels))
    print('   %s: %d     %s: %d' % (pa, hits_a, pb, hits_b))


if __name__ == '__main__':
    main()
