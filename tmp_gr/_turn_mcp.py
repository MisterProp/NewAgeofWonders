"""Scratch: pull the turn-button / next-unit functions out of the running
GhidraMCP server.

    python tmp_gr/_turn_mcp.py list <program> <filter>
    python tmp_gr/_turn_mcp.py decomp <program> <address|name> <outfile>
    python tmp_gr/_turn_mcp.py call <endpoint> <query>
"""
import os
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
OUT = os.path.dirname(os.path.abspath(__file__))


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=300) as r:
        return r.read().decode('utf-8', 'replace')


def main(argv):
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    cmd = argv[1]
    if cmd == 'list':
        prog, filt = argv[2], (argv[3] if len(argv) > 3 else '')
        txt = get('/list_functions', program=prog, offset=0, limit=1000000)
        hits = [l for l in txt.splitlines() if filt.lower() in l.lower()]
        print('\n'.join(hits))
        print('--- %d hits of %d lines' % (len(hits), len(txt.splitlines())))
    elif cmd == 'decomp':
        prog, addr, dest = argv[2], argv[3], argv[4]
        txt = get('/decompile_function', program=prog, address=addr)
        open(dest, 'w', encoding='utf-8').write(txt)
        print('wrote %s (%d bytes)' % (dest, len(txt)))
    elif cmd == 'call':
        print(get(argv[2], **dict(kv.split('=', 1) for kv in argv[3:])))
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
