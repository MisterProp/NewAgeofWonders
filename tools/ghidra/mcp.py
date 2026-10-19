"""mcp.py - tiny CLI for the GhidraMCP HTTP bridge (see run_ghidra.ps1).

Usage:
    python tools/ghidra/mcp.py <out-file> <endpoint> [key=value ...]

Example:
    python tools/ghidra/mcp.py out.c /decompile_function \
        address=0x55776d60 program=AoWEPACK.dpl

Writes the raw JSON/text response to <out-file> (stdout is unreliable through
the VS Code shell integration). Requires GhidraMCP listening on 127.0.0.1:8089.
"""
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=60) as r:
        return r.read().decode('utf-8', 'replace')


if __name__ == '__main__':
    out = sys.argv[1]
    path = sys.argv[2]
    params = {}
    for kv in sys.argv[3:]:
        k, v = kv.split('=', 1)
        params[k] = v
    txt = get(path, **params)
    open(out, 'w', encoding='utf-8').write(txt)
    print('wrote %s (%d bytes)' % (out, len(txt)))
