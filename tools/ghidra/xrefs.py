"""xrefs.py - print get_xrefs_to for many addresses in one program.

    python tools/ghidra/xrefs.py /AoW.exe 00401e5c 00401e64 ...
"""
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'


def get(endpoint, **params):
    url = BASE + endpoint
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=600) as r:
        return r.read().decode('utf-8', 'replace')


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    get('/open_program', path=sys.argv[1])
    for addr in sys.argv[2:]:
        print('=== xrefs to %s' % addr)
        print(get('/get_xrefs_to', address=addr).strip())
