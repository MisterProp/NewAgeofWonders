"""eobj_dump.py - inspect the EObject graph of an AoW1 scenario map (.HSM/.CSM).

A diagnostic tool that mirrors the walk in src/engine/map.cpp (and aow/eobject.hpp)
so the on-disk layout of a map object can be checked against the decompiled
`ReadWrite` methods without rebuilding the C++ port.

    python tools/eobj_dump.py <map.HSM> [--class 0x20217] [--tile x,y]

It prints the top object, the container/level/tile tree, and - for the objects on
the selected tile (or every object whose class id matches --class) - the object's
own tag table, so the embedded army/unit payload is visible. stdlib only.
"""
import argparse
import struct
import zlib


def u16(b, o):
    return struct.unpack_from('<H', b, o)[0]


def u32(b, o):
    return struct.unpack_from('<I', b, o)[0]


class Table:
    def __init__(self, base, entries):
        self.base = base          # position just past the table
        self.entries = entries    # list of (tag, offset)

    def off(self, tag):
        for t, o in self.entries:
            if t == tag:
                return o
        return None


def read_table(R, pos):
    """Parse a property table at `pos`; returns (Table, next_pos) or (None, pos)."""
    if pos >= len(R):
        return None, pos
    b = R[pos]
    p = pos + 1
    if b & 0x80:
        small = b & 0x7F
        large = u32(R, p)
        p += 4
    else:
        small = b
        large = 0
    entries = []
    for _ in range(small):
        w = u16(R, p)
        p += 2
        entries.append((w & 0xFF, w >> 8))
    for _ in range(large):
        entries.append((u32(R, p), u32(R, p + 4)))
        p += 8
    return Table(p, entries), p


def short_string(R, off):
    if off >= len(R):
        return ''
    p = off
    ln = R[p]
    p += 1
    if ln == 0xFF:
        ln = u32(R, p)
        p += 4
    return bytes(R[p:p + ln]).decode('latin-1')


def load_region(path):
    raw = open(path, 'rb').read()
    if raw[:4] == b'CFS\x00':
        raw = zlib.decompress(raw[5:])
    region_off = u32(raw, 0)
    return raw[region_off:], raw[4:8], u32(raw, 12)


def child_objects(R, table):
    """Yield (tag, class_id, obj_table, obj_off) for a children table (tag 1)."""
    for tag, off in table.entries:
        a = table.base + off
        cid = u32(R, a)
        t, _ = read_table(R, a + 4)
        yield tag, cid, t, a


def dump_army(R, hs):
    """TArmyHS tag 10 = embedded TArmy; its children (tag 1) are TUnits."""
    t10 = hs.off(10)
    if t10 is None:
        print('      (no tag-10 army object)')
        return
    a = hs.base + t10
    for skip in (0, 4):
        at, _ = read_table(R, a + skip)
        if at and at.entries:
            cid = u32(R, a) if skip == 4 else None
            print('      TArmy @+%d skip=%d class=%s tags=%s' %
                  (t10, skip, ('0x%x' % cid) if cid else '?',
                   sorted(t for t, _ in at.entries)))
            ch = at.off(1)
            if ch is not None:
                cht, _ = read_table(R, at.base + ch)
                for utag, ucid, utab, uoff in child_objects(R, cht):
                    vals = {}
                    for t, o in utab.entries:
                        if t in (5, 6, 7, 8, 9, 10, 22):
                            vals[t] = u32(R, utab.base + o) if t == 5 else \
                                u16(R, utab.base + o) if t == 10 else \
                                R[utab.base + o]
                    print('        unit tag=%d class=0x%x tags=%s vals=%s' %
                          (utag, ucid, sorted(t for t, _ in utab.entries),
                           {k: vals[k] for k in sorted(vals)}))
            break


def dump_field(R, body, want_cls, x, y):
    ft, _ = read_table(R, body)
    for tag, off in ft.entries:
        a = ft.base + off
        if tag < 50:
            continue
        cid = u32(R, a)
        if want_cls is None:
            if tag == 50:
                continue
        elif cid != want_cls:
            continue
        ot, _ = read_table(R, a + 4)
        t9 = ot.off(9)
        t9v = R[ot.base + t9] if t9 is not None else None
        print('  hex(%d,%d) tag=%d class=0x%x tags=%s gfx(tag9)=%s' %
              (x, y, tag, cid, sorted(t for t, _ in ot.entries), t9v))
        if cid == 0x20217:  # AoWE.TArmyHS
            dump_army(R, ot)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('map')
    ap.add_argument('--class', dest='cls', default=None,
                    help='only dump objects of this class id (e.g. 0x20217)')
    ap.add_argument('--tile', default=None, help='dump one tile: x,y')
    args = ap.parse_args()

    R, magic, class_id = load_region(args.map)
    print('%s  magic=%r  top_class=0x%x  region=%d bytes' %
          (args.map, magic, class_id, len(R)))

    outer, _ = read_table(R, 0)
    top, _ = read_table(R, outer.base + outer.off(0))
    print('top object tags: %s' % sorted(t for t, _ in top.entries))

    want_cls = int(args.cls, 0) if args.cls else None
    want_tile = None
    if args.tile:
        want_tile = tuple(int(x) for x in args.tile.split(','))

    cont, _ = read_table(R, top.base + top.off(1))
    for ctag, cid, ctab, coff in child_objects(R, cont):
        if cid != 0x10103:
            continue
        w = ctab.off(3)
        h = ctab.off(4)
        print('container: class=0x%x w=%d h=%d levels=%d' %
              (cid, R[ctab.base + w], R[ctab.base + h],
               R[ctab.base + ctab.off(5)] if ctab.off(5) is not None else 0))
        lvl, _ = read_table(R, ctab.base + ctab.off(1))
        for ltag, lcid, ltab, loff in child_objects(R, lvl):
            if lcid not in (0x10102, 0x20013):
                continue
            lw = R[ltab.base + ltab.off(3)]
            lh = R[ltab.base + ltab.off(4)]
            start = ltab.base + ltab.off(6)
            end = ltab.base + ltab.off(1)
            print('\nlevel tag=%d class=0x%x %dx%d  fields=[%d,%d)' %
                  (ltag, lcid, lw, lh, start, end))
            p = start
            n = 0
            while p + 4 <= end:
                ln = u32(R, p)
                if ln == 0 or p + 4 + ln > end:
                    print('  field desync at %d (len=%d)' % (p, ln))
                    break
                body = p + 4
                x, y = n % lw, n // lw
                if want_tile is None or (x, y) == want_tile:
                    dump_field(R, body, want_cls, x, y)
                n += 1
                p = body + ln
            print('  fields walked: %d' % n)


if __name__ == '__main__':
    main()
