"""tmp_tplayer_dump.py - dump a map's TAoWHSMap tag 0x15 TPlayerList records raw."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'tools'))
from eobj_dump import load_region, read_table, child_objects, u32, u16, short_string

path = sys.argv[1]
R, magic, cid = load_region(path)
print('%s magic=%r top_class=0x%x' % (path, magic, cid))
outer, _ = read_table(R, 0)
top, _ = read_table(R, outer.base + outer.off(0))
print('top tags:', sorted(t for t, _ in top.entries))
pl_off = top.off(0x15)
print('tag 0x15 offset:', pl_off)
pl, _ = read_table(R, top.base + pl_off)
print('TPlayerList tags:', sorted(t for t, _ in pl.entries))
ch = pl.off(1)
cht, _ = read_table(R, pl.base + ch)
for tag, pcid, tab, off in child_objects(R, cht):
    print('--- player class=0x%x tags=%s' % (pcid, sorted(t for t, _ in tab.entries)))
    for t, o in sorted(tab.entries):
        a = tab.base + o
        b = R[a]
        try:
            s = short_string(R, a)
        except Exception:
            s = ''
        print('   tag=0x%-3x off=%-4d byte=%-4d u32=%-10d str=%r' %
              (t, o, b, u32(R, a), s[:24]))
