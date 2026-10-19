"""Scratch: LIVE query of the running GhidraMCP server for map-rendering function
names across every currently-open .dpl.

Unlike _map_render_search.py (which used a fixed program list), this enumerates
the open programs from /mcp/instance_info, pulls /list_functions for each via the
MCP bridge, caches the raw lists, then filters for map-render-ish names.

Writes:
    _mcp_funcs_<prog>.txt    raw MCP /list_functions output per program
    _mcp_map_render.txt      filtered/grouped report
"""
import json
import os
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
OUT = r'C:\GOG Games\Age of Wonders\port\tmp_gr'

# Tokens that flag a function as part of map rendering / map display.
TOKENS = ['map', 'hex', 'terr', 'tile', 'landscape', 'render', 'draw', 'blit',
          'scroll', 'sprite', 'overlay', 'minimap', 'surface', 'palette',
          'flip', 'viewport', 'zoom', 'brush', 'iso', 'hse', 'sky', 'water',
          'object', 'field', 'node']


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=180) as r:
        return r.read().decode('utf-8', 'replace')


info = json.loads(get('/mcp/instance_info'))
programs = [p['name'] for p in info.get('programs', [])
            if p.get('open') and p['name'].lower().endswith('.dpl')]
programs.sort()

report = []
summary = []
for prog in programs:
    txt = get('/list_functions', program=prog)
    dest = os.path.join(OUT, '_mcp_funcs_' + prog[:-4] + '.txt')
    with open(dest, 'w', encoding='utf-8') as f:
        f.write(txt)

    names = []
    for line in txt.splitlines():
        line = line.strip()
        if not line:
            continue
        names.append(line.split('@')[0].split(' at ')[0].strip())

    hits = [n for n in names if any(t in n.lower() for t in TOKENS)]
    groups = {}
    for n in hits:
        cls = n.rsplit('.', 1)[0] if '.' in n else '(unit)'
        groups.setdefault(cls, []).append(n)

    summary.append((prog, len(names), len(hits), len(groups)))
    report.append('### %s  (%d functions, %d map/render hits, %d classes)'
                  % (prog, len(names), len(hits), len(groups)))
    for cls in sorted(groups):
        report.append('  %s' % cls)
        for n in sorted(set(groups[cls])):
            report.append('      ' + n)
    report.append('')

with open(os.path.join(OUT, '_mcp_map_render.txt'), 'w', encoding='utf-8') as f:
    f.write('\n'.join(report))

lines = ['%-16s %8s %8s %8s' % ('dpl', 'funcs', 'hits', 'classes')]
for prog, nf, nh, nc in summary:
    lines.append('%-16s %8d %8d %8d' % (prog, nf, nh, nc))
lines.append('TOTAL map/render hits: %d' % sum(s[2] for s in summary))
with open(os.path.join(OUT, '_mcp_map_render_summary.txt'), 'w',
          encoding='utf-8') as f:
    f.write('\n'.join(lines))
print('wrote _mcp_map_render.txt and _mcp_map_render_summary.txt')
