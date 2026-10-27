"""tmp_city_list.py - print (x, y, race, name) for every city on every level.

Mirrors src/engine/map.cpp's sequential field walk: a level's tag-6 blob holds
one u32-length-prefixed TAoWMapField record per tile in row-major order, so the
record index IS the tile.

    python tmp_city_list.py <map.HSM>
"""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'tools'))
from eobj_dump import (load_region, read_table, child_objects, u32, short_string)


def main(path, width, height):
    R, _magic, _cid = load_region(path)
    outer, _ = read_table(R, 0)
    top, _ = read_table(R, outer.base + outer.off(0))
    print('%s %dx%d' % (path, width, height))
    cont, _ = read_table(R, top.base + top.off(1))
    for ctag, cid, ctab, coff in child_objects(R, cont):
        if cid != 0x10103:
            continue
        lvl, _ = read_table(R, ctab.base + ctab.off(1))
        for ltag, lcid, ltab, loff in child_objects(R, lvl):
            if lcid not in (0x10102, 0x20013):
                continue
            p = ltab.base + ltab.off(6)
            end = ltab.base + ltab.off(1)
            tile = 0
            while p + 4 <= end:
                ln = u32(R, p)
                if ln == 0 or p + 4 + ln > end:
                    break
                body = p + 4
                ft, _ = read_table(R, body)
                x, y = tile % width, tile // width
                for tag, off in ft.entries:
                    if tag < 50:
                        continue
                    a = ft.base + off
                    ocid = u32(R, a)
                    if ocid != 0x20108:
                        continue
                    ot, _ = read_table(R, a + 4)
                    race = R[ot.base + ot.off(0xd)] if ot.off(0xd) is not None else -1
                    name = short_string(R, ot.base + ot.off(0xc)) if ot.off(0xc) is not None else ''
                    owner = R[ot.base + ot.off(5)] if ot.off(5) is not None else 0
                    print('  L%d (%2d,%2d) race=%d owner=%d name=%s'
                          % (ctag, x, y, race, owner, name))
                p = body + ln
                tile += 1


if __name__ == '__main__':
    main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
