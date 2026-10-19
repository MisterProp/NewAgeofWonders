"""tmp_cmp_region.py - region-wise mean abs diff between the reference crop and a
port render (the reference starts at the render's (443,220))."""
import sys

sys.path.insert(0, '.')
import tmp_facecmp as F

# ref-space rects (the reference is the port render cropped at +443+220)
REGIONS = [
    ('whole', 0, 0, 193, 77),
    ('face cell', 4, 16, 52, 61),      # FaceFr (7,20 52x64) in ref coords
    ('face px', 7, 19, 46, 58),        # FaceFr inset by 3
    ('shield cell', 136, 16, 52, 61),  # ShieldFr (142,20) in ref coords
    ('shield px', 145, 22, 40, 52),
    ('name text', 52, 2, 84, 46),      # InfoFr
    ('day text', 52, 41, 84, 20),      # TurnFr
]


def main():
    ref, big = sys.argv[1], sys.argv[2]
    ox = int(sys.argv[3]) if len(sys.argv) > 3 else 443
    oy = int(sys.argv[4]) if len(sys.argv) > 4 else 220
    rw, rh, rb, rr = F.decode(ref)
    bw, bh, bb, br = F.decode(big)
    for name, x, y, w, h in REGIONS:
        tot = 0
        n = 0
        worst = (0, 0, 0)
        for j in range(h):
            for i in range(w):
                if y + j >= rh or x + i >= rw:
                    continue
                a, b, c = F.px(rr, rb, x + i, y + j)
                d, e, f = F.px(br, bb, ox + x + i, oy + y + j)
                dv = abs(a - d) + abs(b - e) + abs(c - f)
                tot += dv
                n += 3
                if dv > worst[0]:
                    worst = (dv, i, j)
        print('%-12s %5.2f  (worst %d at +%d+%d)' %
              (name, tot / n if n else 0, worst[0], worst[1], worst[2]))


if __name__ == '__main__':
    main()
