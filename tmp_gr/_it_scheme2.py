"""_it_scheme2.py - try treating bit7 as end-of-row (S3M-style)."""
import glob
import struct
import sys


def decode(d, off, mode):
    length, rows = struct.unpack_from('<HH', d, off)
    p = off + 8
    end = off + 8 + length
    r = 0
    while r < rows:
        if p >= end:
            break
        ended = False
        while p < end:
            v = d[p]; p += 1
            if v == 0:
                ended = True
                break
            if v & 1:
                p += 1
            if v & 2:
                p += 1
            if v & 4:
                p += 1
            if v & 8:
                p += 2
            if mode == 's3m' and (v & 0x80):
                ended = True
                break
        r += 1
        if not ended:
            break
    return rows, r, p - (off + 8), length


def run(path, mode):
    d = open(path, 'rb').read()
    on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
    po = 0xC0 + on + inn * 4 + sn * 4
    exact = over = under = tot = 0
    for i in range(pn):
        off = struct.unpack_from('<I', d, po + i * 4)[0]
        if off == 0:
            continue
        rows, r, used, length = decode(d, off, mode)
        tot += 1
        if r == rows and used == length:
            exact += 1
        elif r < rows:
            over += 1
        else:
            under += 1
    return tot, exact, over, under


if __name__ == '__main__':
    for mode in ('it', 's3m'):
        print('=== mode=%s ===' % mode)
        te = to = 0
        for f in sorted(glob.glob(sys.argv[1] if len(sys.argv) > 1 else 'Songs/*.it')):
            tot, ex, ov, un = run(f, mode)
            te += ex; to += tot
            print('  %-14s pats=%-3d exact=%-3d over=%-3d under=%d' % (f.split('\\')[-1], tot, ex, ov, un))
        print('  TOTAL exact %d/%d' % (te, to))
