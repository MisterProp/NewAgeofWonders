"""find_cam.py - search every relevant module for camera / viewport symbols.

Read-only helper built on the GhidraMCP HTTP bridge (127.0.0.1:8089).  It opens
each module in turn and calls /search_functions for a list of search terms,
writing one consolidated, greppable report so we can mine the routines that
drive the world-map camera (scroll / zoom / click-to-move).

    python tools/ghidra/find_cam.py out.txt "term1" "term2" ...
"""
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'

PROGRAMS = ['/AoW.exe', '/AoWEPACK.dpl', '/HSEPack.dpl', '/aowInt.dpl',
            '/AoWInterface.dpl', '/Enginep.dpl', '/ims.dpl']

DEFAULT_TERMS = ['SetScene', 'ViewPort', 'Scroll', 'Zoom', 'Camera',
                 'MouseDown', 'MouseMove', 'MWindow', 'WorldMap', 'TScene',
                 'Viewer', 'Center']


def get(endpoint, **params):
    url = BASE + endpoint
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=600) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    out = sys.argv[1]
    terms = sys.argv[2:] or DEFAULT_TERMS
    lines = []
    for prog in PROGRAMS:
        get('/open_program', path=prog)
        for term in terms:
            body = get('/search_functions', name_pattern=term)
            for ln in body.splitlines():
                ln = ln.strip()
                if ln and 'No functions found' not in ln:
                    lines.append('%-18s %-12s %s' % (prog, term, ln))
    with open(out, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')
    print('wrote %s (%d lines)' % (out, len(lines)))


if __name__ == '__main__':
    main()
