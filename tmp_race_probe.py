"""tmp_race_probe.py - per-player race/type tag presence + city/army races of a map."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'tools'))
from eobj_dump import (load_region, read_table, child_objects, u32, u16)

RACES = ["Human", "Azrac", "Lizardman", "Frostling", "Elf", "Halfling",
         "Dwarf", "Highman", "Dark Elf", "Orc", "Goblin", "Undead", "MONSTER"]


def rlabel(v):
    return RACES[v] if v < len(RACES) else "?%d" % v


def players(R):
    outer, _ = read_table(R, 0)
    top, _ = read_table(R, outer.base + outer.off(0))
    o = top.off(0x15)
    if o is None:
        return []
    pl, _ = read_table(R, top.base + o)
    ch = pl.off(1)
    if ch is None:
        return []
    cht, _ = read_table(R, pl.base + ch)
    out = []
    for tag, pcid, tab, off in child_objects(R, cht):
        idx = tab.off(0x12)
        race = tab.off(0x2)
        typ = tab.off(0x3)
        rv = R[tab.base + race] if race is not None else None
        tv = R[tab.base + typ] if typ is not None else None
        iv = R[tab.base + idx] if idx is not None else None
        out.append((iv, rv, tv, race is not None, typ is not None))
    return out


def walk_objects(R, cb):
    outer, _ = read_table(R, 0)
    top, _ = read_table(R, outer.base + outer.off(0))
    conto = top.off(1)
    cont, _ = read_table(R, top.base + conto)
    for ctag, cid, ctab, coff in child_objects(R, cont):
        if cid != 0x10103:
            continue
        lvlo = ctab.off(1)
        lvl, _ = read_table(R, ctab.base + lvlo)
        for ltag, lcid, ltab, loff in child_objects(R, lvl):
            if lcid not in (0x10102, 0x20013):
                continue
            start = ltab.base + ltab.off(6)
            end = ltab.base + ltab.off(1)
            p = start
            while p + 4 <= end:
                ln = u32(R, p)
                if ln == 0 or p + 4 + ln > end:
                    break
                body = p + 4
                ft, _ = read_table(R, body)
                for tag, off in ft.entries:
                    if tag < 50:
                        continue
                    a = ft.base + off
                    ocid = u32(R, a)
                    ot, _ = read_table(R, a + 4)
                    cb(ocid, ot)
                p = body + ln


def city_races(R):
    seen = {}

    def cb(cid, ot):
        if cid == 0x20108:  # City.TCity
            o = ot.off(0xd)
            if o is not None:
                seen[R[ot.base + o]] = seen.get(R[ot.base + o], 0) + 1
    walk_objects(R, cb)
    return seen


for path in sys.argv[1:]:
    R, magic, cid = load_region(path)
    print('== %s' % path)
    for iv, rv, tv, rhas, thas in players(R):
        print('   idx=%-4s race=%-9s type=%-4s (tag2=%s tag3=%s)' %
              (iv, rlabel(rv) if rhas else 'ABSENT', tv, rhas, thas))
    cr = city_races(R)
    print('   city races: %s' %
          {rlabel(k): v for k, v in sorted(cr.items())})
