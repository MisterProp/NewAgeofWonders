"""Scratch: measure how much of the GhidraMCP map-render function set the C++ port
covers. Extracts engine symbol references from the port sources and intersects
them with the curated render-pipeline names found via MCP.
"""
import os
import re

PORT = r'C:\GOG Games\Age of Wonders\port'
SRC = os.path.join(PORT, 'src')
CUR = os.path.join(PORT, 'tmp_gr', '_mcp_map_render_curated.txt')

# engine units that own map-render functions
UNITS = r'(HSEngine|HSLib|AoWE|AoWHex|AoWTer|GFXE|ImageLib|Engine|Terrain|' \
        r'IsoHex|Hexagon|ILTer|RndHex|BorderHx)'

# 1) curated MCP function names (class.function) -> set
curated = set()
cur_classes = set()
for line in open(CUR, encoding='utf-8'):
    s = line.strip()
    if s.startswith('###') or not s:
        continue
    if line.startswith('  ') and not line.startswith('      '):
        cur_classes.add(s)
    elif line.startswith('      '):
        curated.add(s)
# also treat bare classes as names
curated |= cur_classes

# 2) every engine symbol referenced in the C++ sources
ref = re.compile(UNITS + r'(\.[A-Za-z_][\w]*)?')
cited = {}   # symbol -> set(files)
for dirpath, _, files in os.walk(SRC):
    for fn in files:
        if not fn.lower().endswith(('.cpp', '.hpp', '.h')):
            continue
        p = os.path.join(dirpath, fn)
        rel = os.path.relpath(p, PORT)
        for m in ref.finditer(open(p, encoding='utf-8', errors='replace').read()):
            sym = m.group(0)
            cited.setdefault(sym, set()).add(rel)

hits = sorted(s for s in cited if s in curated)
classes_hit = sorted(c for c in cur_classes if c in cited)

lines = []
lines.append('curated MCP render functions : %d (across %d classes)'
             % (len(curated), len(cur_classes)))
lines.append('engine symbols cited in C++ : %d distinct' % len(cited))
lines.append('cited AND in curated set    : %d' % len(hits))
lines.append('')
lines.append('--- matched function names (C++ re-uses/re-implements) ---')
for h in hits:
    lines.append('  %-55s %s' % (h, ', '.join(sorted(cited[h]))))
lines.append('')
lines.append('--- curated classes touched by the C++ ---')
for c in classes_hit:
    lines.append('  ' + c)
lines.append('')
lines.append('--- engine symbols cited in C++ but NOT in curated render set ---')
for s in sorted(cited):
    if s not in curated:
        lines.append('  %-55s %s' % (s, ', '.join(sorted(cited[s]))))

open(os.path.join(PORT, 'tmp_gr', '_cpp_coverage.txt'), 'w',
     encoding='utf-8').write('\n'.join(lines))
print('matched %d / %d curated functions' % (len(hits), len(curated)))
