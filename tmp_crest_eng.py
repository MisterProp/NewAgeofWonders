"""tmp_crest_eng.py - score the engine-style crest draw: 1:1, a=127 blend."""
import sys

sys.path.insert(0, '.')
import tmp_facecmp as F

OX, OY = 443, 220
BX, BY = 145, 22


def main():
    ref, ncrest, cid = sys.argv[1], sys.argv[2], int(sys.argv[3])
    rw, rh, rb, rr = F.decode(ref)
    bw, bh, bb2, brr = F.decode(ncrest)
    sw, sh, sb, sr = F.decode('bin/tmp_sh_%d.png' % cid)
    for a in (255, 160, 127, 100, 64):
        best = None
        for dy in range(-4, 5):
            for dx in range(-4, 5):
                tot = 0
                n = 0
                for j in range(sh):
                    for i in range(sw):
                        x = BX + dx + i
                        y = BY + dy + j
                        if x >= rw or y >= rh or x < 0 or y < 0:
                            continue
                        sr2, sg2, sb2, sa = F.px4(sr, sb, i, j)
                        if sa == 0:
                            continue
                        br2, bg2, bb3 = F.px(brr, bb2, OX + x, OY + y)[:3]
                        # the engine's blend: d = (p*a + d*(255-a))/255
                        cr = (sr2 * a + br2 * (255 - a)) // 255
                        cg = (sg2 * a + bg2 * (255 - a)) // 255
                        cb = (sb2 * a + bb3 * (255 - a)) // 255
                        r2, g2, b2 = F.px(rr, rb, x, y)
                        tot += abs(r2 - cr) + abs(g2 - cg) + abs(b2 - cb)
                        n += 3
                if n:
                    v = tot / n
                    if best is None or v < best[0]:
                        best = (v, dx, dy, n)
        print('id=%2d alpha=%3d: %.2f at %+d,%+d (%d px)' %
              (cid, a, *best))


if __name__ == '__main__':
    main()
