"""_it_brute.py - brute-force the mask-bit payload byte counts.

Each of the 8 mask bits consumes 0/1/2 payload bytes.  Search for the mapping
that makes every pattern parse to exactly (rows, length).
"""
import glob
import itertools
import struct
import sys

PROBE = []  # (file bytes, off, rows, length)


def load_probes(pat, limit=24):
    out = []
    for f in sorted(glob.glob(pat)):
        d = open(f, 'rb').read()
        if d[:4] != b'IMPM':
            continue
        on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
        po = 0xC0 + on + inn * 4 + sn * 4
        for i in range(pn):
            off = struct.unpack_from('<I', d, po + i * 4)[0]
            if off == 0:
                continue
            ln, rows = struct.unpack_from('<HH', d, off)
            out.append((f, d, off, rows, ln))
            if len(out) >= limit:
                return out
    return out


def try_decode(d, off, rows, length, cost):
    p = off + 8
    end = off + 8 + length
    r = 0
    while r < rows:
        if p >= end:
            return False
        ok_row = False
        while p < end:
            v = d[p]; p += 1
            if v == 0:
                ok_row = True
                break
            for b in range(8):
                if v & (1 << b):
                    p += cost[b]
        if not ok_row:
            return False
        r += 1
    return p == end


def main():
    probes = load_probes(sys.argv[1] if len(sys.argv) > 1 else 'Songs/*.it', 60)
    print('probes=%d' % len(probes))
    scored = []
    for cost in itertools.product(range(3), repeat=8):
        if cost[0] != 1:
            continue
        good = sum(try_decode(f[1], f[2], f[3], f[4], cost) for f in probes)
        scored.append((good, cost))
    scored.sort(reverse=True)
    print('top 10:')
    for good, cost in scored[:10]:
        print('  %2d/%d  cost=%s' % (good, len(probes), cost))


if __name__ == '__main__':
    main()
