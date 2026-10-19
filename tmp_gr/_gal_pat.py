"""_gal_pat.py - test Galaxy's pattern packing: [chan+1|0x80][mask][payloads]."""
import glob
import struct
import sys

HDR = 0xC0


def decode(data, rows):
    """Returns (rows_parsed, bytes_used)."""
    rem = [0] * 64
    p = 0
    n = len(data)
    for _ in range(rows):
        if p >= n:
            return -1, p
        while p < n and data[p] != 0:
            b = data[p]; p += 1
            ch = (b - 1) & 0x3F
            if (b & 0x80) == 0:
                mask = rem[ch]
            else:
                if p >= n:
                    return -1, p
                mask = data[p]; p += 1
                rem[ch] = mask
            if mask & 1: p += 1
            if mask & 2: p += 1
            if mask & 4: p += 1
            if mask & 8: p += 2
        if p < n:
            p += 1  # the 0x00 terminator
        else:
            return -1, p
    return rows, p


def run(path):
    d = open(path, 'rb').read()
    on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
    po = HDR + on + inn * 4 + sn * 4
    exact = over = 0
    tot = 0
    for i in range(pn):
        off = struct.unpack_from('<I', d, po + i * 4)[0]
        if off == 0:
            continue
        ln, rows = struct.unpack_from('<HH', d, off)
        data = d[off + 8:off + 8 + ln]
        r, used = decode(data, rows)
        tot += 1
        if r == rows and used == ln:
            exact += 1
        else:
            over += 1
    return tot, exact, over


if __name__ == '__main__':
    pat = sys.argv[1] if len(sys.argv) > 1 else 'Songs/*.it'
    te = tt = 0
    for f in sorted(glob.glob(pat)):
        tot, ex, ov = run(f)
        te += ex; tt += tot
        print('%-14s pats=%-3d exact=%-3d mismatch=%d' % (f.split('\\')[-1], tot, ex, ov))
    print('TOTAL exact %d/%d' % (te, tt))
