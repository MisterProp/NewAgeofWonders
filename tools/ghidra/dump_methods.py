"""dump_methods.py - page the whole function list out of GhidraMCP.

The plugin's /search_functions endpoint is picky about its query parameter
name, but /methods is not: it takes offset/limit. We pull everything once and
grep locally, which also gives us a full symbol inventory for the port.

    python tools/ghidra/dump_methods.py re/exports/aowepack_methods.txt
"""
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
PAGE = 2000


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=300) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    out = sys.argv[1]
    path = sys.argv[2] if len(sys.argv) > 2 else '/methods'
    names = []
    offset = 0
    while True:
        body = get(path, offset=offset, limit=PAGE)
        lines = [l for l in body.splitlines() if l.strip()]
        if not lines:
            break
        names.extend(lines)
        if len(lines) < PAGE:
            break
        offset += PAGE
    with open(out, 'w', encoding='utf-8') as f:
        f.write('\n'.join(names))
    print('wrote %s (%d entries)' % (out, len(names)))


if __name__ == '__main__':
    main()
