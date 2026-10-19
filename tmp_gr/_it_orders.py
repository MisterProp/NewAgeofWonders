"""_it_orders.py - show order list + per-order pattern validity."""
import struct
import sys


def show(path):
    d = open(path, 'rb').read()
    on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
    flags = struct.unpack_from('<H', d, 0x2c)[0]
    orders = list(d[0xC0:0xC0 + on])
    po = 0xC0 + on + inn * 4 + sn * 4
    patoff = [struct.unpack_from('<I', d, po + i * 4)[0] for i in range(pn)]
    print('%-14s ord=%-3d pat=%-3d flags=%04x orders[:16]=%s'
          % (path.split('\\')[-1], on, pn, flags, orders[:16]))
    for o in orders[:16]:
        if o == 0xFF:
            print('   order %d: end' % o)
            continue
        if o >= pn:
            print('   order %d: -> pattern %d (out of range)' % (o, o))
            continue
        off = patoff[o]
        if off == 0:
            print('   order %d: -> pattern %d EMPTY/offset0' % (o, o))
        else:
            ln, rows = struct.unpack_from('<HH', d, off)
            print('   order %d: -> pattern %d off=%d len=%d rows=%d'
                  % (o, o, off, ln, rows))


if __name__ == '__main__':
    for p in sys.argv[1:]:
        show(p)
