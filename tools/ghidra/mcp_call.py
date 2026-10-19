"""mcp_call.py - convenience wrapper around the GhidraMCP HTTP bridge.

Unlike mcp.py (which needs `key=value` argv pairs), this lets you pass a raw
URL query string, so endpoints that want an awkward parameter name still work:

    python tools/ghidra/mcp_call.py /search_functions "searchTerm=Menu"
    python tools/ghidra/mcp_call.py /functions "offset=0&limit=50"

It prints the decoded body to stdout (utf-8, errors replaced).
"""
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'


def call(path, query=''):
    url = BASE + path
    if query:
        url += '?' + query
    req = urllib.request.Request(url, headers={'Accept': 'text/plain, */*'})
    with urllib.request.urlopen(req, timeout=120) as r:
        return r.read().decode('utf-8', 'replace')


if __name__ == '__main__':
    path = sys.argv[1]
    query = sys.argv[2] if len(sys.argv) > 2 else ''
    body = call(path, query)
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    print(body)
