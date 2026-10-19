#!/usr/bin/env python3
"""Analyse the 8 move-cost tables: per (terrain, overlay) show the cost for each move bit."""
import re, sys

hdr = open('src/include/aow/move_cost_tables.hpp', encoding='utf-8').read()
# grab the 8 tables
body = hdr.split('kMovePointSourceTables[8][256] = {', 1)[1]
tables = []
cur = None
for line in body.splitlines():
    m = re.search(r'// table (\d)', line)
    if m:
        cur = []
        tables.append(cur)
        continue
    if cur is None:
        continue
    vals = re.findall(r'0x([0-9a-f]{2})', line)
    if len(vals) == 16:
        cur.extend(int(v, 16) for v in vals)
    elif vals and len(vals) < 4:
        pass
names = ['walk', 'water', 'fly', 'forest', 'cave', 'mount', 'bit6', 'bit7']
bits = [0x01, 0x04, 0x02, 0x08, 0x10, 0x20, 0x40, 0x80]
assert all(len(t) == 256 for t in tables), [len(t) for t in tables]

terr = int(sys.argv[1]) if len(sys.argv) > 1 else 1
print(f'--- terrain {terr} (row {terr}), one line per overlay column ---')
print('ovl  ' + '  '.join('%-5s' % n for n in names))
for ov in range(16):
    idx = terr * 16 + ov
    row = []
    for t in range(8):
        v = tables[t][idx]
        row.append('.' if v == 0xff else '%d' % v)
    print('%-4d ' % ov + '  '.join('%-5s' % v for v in row))

print()
print('--- the min over all 8 tables (what a unit knowing everything pays) ---')
for ov in range(16):
    idx = terr * 16 + ov
    vals = [tables[t][idx] for t in range(8)]
    mn = min(vals)
    print('ovl %-3d min=%-4s walk=%-4s forest=%-4s mount=%-4s fly=%-4s' % (
        ov, '.' if mn == 0xff else mn,
        '.' if tables[0][idx] == 0xff else tables[0][idx],
        '.' if tables[3][idx] == 0xff else tables[3][idx],
        '.' if tables[5][idx] == 0xff else tables[5][idx],
        '.' if tables[2][idx] == 0xff else tables[2][idx]))

print()
print('--- which columns each ability cell *improves* over walk (terrain %d) ---' % terr)
for t in range(8):
    diff = []
    for ov in range(16):
        w = tables[0][terr * 16 + ov]
        v = tables[t][terr * 16 + ov]
        if w != v:
            diff.append('ovl%d:%s->%s' % (ov, '.' if w == 0xff else w, '.' if v == 0xff else v))
    print('%-6s %s' % (names[t], ', '.join(diff) if diff else '(same as walk)'))
