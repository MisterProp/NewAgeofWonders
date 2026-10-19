"""_it_zeros.py - zero counts + annotated decode for one pattern."""
import struct
import sys


def main(path, pi):
    d = open(path, 'rb').read()
    on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
    po = 0xC0 + on + inn * 4 + sn * 4
    off = struct.unpack_from('<I', d, po + pi * 4)[0]
    ln, rows = struct.unpack_from('<HH', d, off)
    data = d[off + 8:off + 8 + ln]
    print('%s pat%d len=%d rows=%d zeros=%d' % (path, pi, ln, rows, data.count(0)))
    print('raw:', ' '.join('%02x' % b for b in data[:120]))
    p = 0
    r = 0
    while r < rows and p < len(data):
        row = []
        while p < len(data):
            v = data[p]; p += 1
            if v == 0:
                break
            note = ins = vol = cmd = info = None
            if v & 1: note = data[p]; p += 1
            if v & 2: ins = data[p]; p += 1
            if v & 4: vol = data[p]; p += 1
            if v & 8: cmd = data[p]; p += 1; info = data[p]; p += 1
            if r < 6:
                row.append('m%02x n%s i%s v%s c%s/%s' % (v, note, ins, vol, cmd, info))
        if row and r < 6:
            print('  row%d: %s' % (r, ' | '.join(row)))
        r += 1
    print('parsed rows=%d' % r)


if __name__ == '__main__':
    main(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 0)
