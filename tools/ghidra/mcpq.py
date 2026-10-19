"""mcpq.py - open a program then call one GhidraMCP endpoint, printing the body.

    python tools/ghidra/mcpq.py /AoW.exe /get_xrefs_to address=00401e5c
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
    prog = sys.argv[1]
    endpoint = sys.argv[2]
    params = {}
    for kv in sys.argv[3:]:
        k, v = kv.split('=', 1)
        params[k] = v
    if prog and prog != '-':
        get('/open_program', path=prog)
    print(get(endpoint, **params))
