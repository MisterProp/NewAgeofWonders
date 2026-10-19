"""Scratch: pull the exact function names for the core map-render classes."""
import os
import sys

OUT = r'C:\GOG Games\Age of Wonders\port\tmp_gr'
PROGRAMS = ['AoWEPACK', 'Enginep', 'HSEPack', 'Gfxepack', 'Ilpack', 'Localize']

# class (unit-qualified) prefixes we consider part of the map-render pipeline.
WANT = sys.argv[1:]


def load(prog):
    path = os.path.join(OUT, '_funcs_%s.txt' % prog)
    names = []
    for line in open(path, encoding='utf-8'):
        line = line.rstrip('\n')
        if line:
            names.append(line.split('@')[0])
    return names


lines = []
for prog in PROGRAMS:
    names = load(prog)
    hits = [n for n in names if any(n.startswith(w) for w in WANT)]
    if not hits:
        continue
    lines.append('===== %s.dpl  (%d) =====' % (prog, len(hits)))
    lines.extend(sorted(hits))
    lines.append('')
open(os.path.join(OUT, '_map_render_curated.txt'), 'w',
     encoding='utf-8').write('\n'.join(lines))
print('wrote %d lines' % len(lines))
