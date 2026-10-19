"""Scratch: filter cached GhidraMCP function lists for map-rendering names."""
import os
import re

OUT = r'C:\GOG Games\Age of Wonders\port\tmp_gr'
PROGRAMS = ['AoWEPACK', 'Enginep', 'HSEPack', 'Gfxepack', 'Ilpack', 'Localize']

# Tokens that flag a function as part of map rendering / map display.
TOKENS = ['map', 'hex', 'terr', 'tile', 'landscape', 'render', 'draw', 'blit',
          'scroll', 'sprite', 'overlay', 'minimap', 'surface', 'palette',
          'flip', 'ilb', 'isometric', 'viewport', 'zoom', 'brush']


def matches(name):
    low = name.lower()
    return [t for t in TOKENS if t in low]


report = []
summary = []
for prog in PROGRAMS:
    path = os.path.join(OUT, '_funcs_%s.txt' % prog)
    if not os.path.exists(path):
        continue
    names = []
    for line in open(path, encoding='utf-8'):
        line = line.rstrip('\n')
        if not line:
            continue
        names.append(line.split('@')[0])
    hits = [n for n in names if matches(n)]
    # group by unit/class (everything before the last dot)
    groups = {}
    for n in hits:
        cls = n.rsplit('.', 1)[0] if '.' in n else n
        groups.setdefault(cls, []).append(n)
    summary.append((prog, len(names), len(hits), len(groups)))
    report.append('### %s.dpl  (%d functions, %d map/render hits, %d classes)\n'
                  % (prog, len(names), len(hits), len(groups)))
    for cls in sorted(groups):
        report.append('  %s  (%d)' % (cls, len(groups[cls])))
        for n in sorted(groups[cls]):
            report.append('      ' + n)

open(os.path.join(OUT, '_map_render_functions.txt'), 'w',
     encoding='utf-8').write('\n'.join(report))

print('%-12s %8s %8s %8s' % ('module', 'funcs', 'hits', 'classes'))
for prog, nf, nh, nc in summary:
    print('%-12s %8d %8d %8d' % (prog, nf, nh, nc))
print('TOTAL map/render hits:', sum(s[2] for s in summary))
