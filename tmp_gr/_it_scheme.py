"""_it_scheme.py - try mask schemes and report which parses all rows exactly."""
import glob
import struct
import sys

NOTE_ONLY = 0


def decode(d, off, use_hi):
    length, rows = struct.unpack_from('<HH', d, off)
    p = off + 8
    end = off + 8 + length
    r = 0
    hi = 0
    while r < rows:
        if p >= end:
            break
        while p < end:
            v = d[p]; p += 1
            if v == 0:
                break
            if v & 1:
                p += 1
            if use_hi:
                if (v & 2) or (v & 16):
                    p += 1
                if (v & 4) or (v & 32):
                    p += 1
            else:
                if v & 2:
                    p += 1
                if v & 4:
                    p += 1
            if v & 8:
                p += 2
            hi = max(hi, v)
        r += 1
    return rows, r, p - (off + 8), length, hi


def run(path, use_hi):
    d = open(path, 'rb').read()
    on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
    po = 0xC0 + on + inn * 4 + sn * 4
    ok = 0
    tot = 0
    over = 0
    for i in range(pn):
        off = struct.unpack_from('<I', d, po + i * 4)[0]
        if off == 0:
            continue
        rows, r, used, length, hi = decode(d, off, use_hi)
        tot += 1
        if r == rows and used == length:
            ok += 1
        if r < rows:
            over += 1
    return tot, ok, over


if __name__ == '__main__':
    for scheme in (False, True):
        print('=== use_hi=%s ===' % scheme)
        for f in sorted(glob.glob(sys.argv[1] if len(sys.argv) > 1 else 'Songs/*.it')):
            tot, ok, over = run(f, scheme)
            print('  %-14s patterns=%-3d exact=%-3d overconsume=%d' % (f.split('\\')[-1], tot, ok, over))
