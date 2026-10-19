"""probe_params.py - find the query parameter name a GhidraMCP endpoint wants.

The plugin rejects unknown parameter names with a JSON error, so we simply try
a list of candidates and report the first that returns real payload.

    python tools/ghidra/probe_params.py /search_functions Menu
"""
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'

CANDIDATES = ['searchTerm', 'term', 'query', 'name', 'filter', 'pattern',
              'q', 's', 'text', 'search', 'value', 'keyword', 'string']


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    try:
        with urllib.request.urlopen(url, timeout=120) as r:
            return r.read().decode('utf-8', 'replace')
    except Exception as exc:  # noqa: BLE001
        return 'ERR ' + str(exc)


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    path = sys.argv[1]
    needle = sys.argv[2] if len(sys.argv) > 2 else 'A'
    for c in CANDIDATES:
        body = get(path, **{c: needle})
        head = body[:90].replace('\n', ' ')
        print('%-12s -> %d bytes: %s' % (c, len(body), head))
