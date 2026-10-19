"""_it_dump.py - dump IT orders + first rows to diagnose silent songs."""
import glob
import struct
import sys

HDR = 0xC0


def dump(path):
    d = open(path, 'rb').read()
    if d[:4] != b'IMPM':
        return
    on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
    flags = struct.unpack_from('<H', d, 0x2c)[0]
    orders = list(d[HDR:HDR + on])
    po = HDR + on + inn * 4 + sn * 4
    patoff = [struct.unpack_from('<I', d, po + i * 4)[0] for i in range(pn)]
    print('%-14s ord=%-3d ins=%-3d smp=%-3d pat=%-3d flags=%04x linear=%d'
          % (path.split('\\')[-1], on, inn, sn, pn, flags, (flags >> 3) & 1))
    print('   orders[:12] =', orders[:12])
    pi = orders[0] if orders and orders[0] < pn else None
    if pi is None:
        print('   first order is not a pattern')
        return
    off = patoff[pi]
    length, rows = struct.unpack_from('<HH', d, off)
    print('   pattern[%d] rows=%d len=%d' % (pi, rows, length))
    p = off + 8
    end = off + 8 + length
    shown = 0
    r = 0
    while p < end and r < rows and shown < 2:
        cells = []
        for c in range(64):
            v = d[p]; p += 1
            if v == 0:
                break
            note = instr = -1
            cmd = info = -1
            if v & 1:
                note = d[p]; p += 1
            if (v & 2) or (v & 16):
                instr = d[p]; p += 1
            if (v & 4) or (v & 32):
                p += 1
            if v & 8:
                cmd = d[p]; p += 1
                info = d[p]; p += 1
            cells.append('c%d:n%d i%d e%d/%d' % (c, note, instr, cmd, info))
        if cells:
            print('   row %d: %s' % (r, '; '.join(cells[:8])))
            shown += 1
        r += 1


if __name__ == '__main__':
    for pat in (sys.argv[1:] or ['Songs\\*.it']):
        for f in sorted(glob.glob(pat)):
            dump(f)
