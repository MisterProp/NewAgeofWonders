"""mcp_post.py - POST helper for the GhidraMCP HTTP bridge.

The bridge exposes mutating/detail endpoints as POST with JSON body params
(e.g. /create_function). mcp.py only does GET.

    python tools/ghidra/mcp_post.py /create_function address=0x004123c0 name=ScenSetupPnlUpdate
    python tools/ghidra/mcp_post.py /create_function address=0x004123c0 program=AoW.exe

Writes the body to stdout (utf-8). Requires GhidraMCP on 127.0.0.1:8089.
"""
import json
import sys
import urllib.request

BASE = 'http://127.0.0.1:8089'


def main():
    path = sys.argv[1]
    body = {}
    for kv in sys.argv[2:]:
        k, v = kv.split('=', 1)
        body[k] = v
    data = json.dumps(body).encode('utf-8')
    req = urllib.request.Request(BASE + path, data=data, method='POST',
                                 headers={'Content-Type': 'application/json'})
    with urllib.request.urlopen(req, timeout=120) as r:
        out = r.read().decode('utf-8', 'replace')
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    print(out)


if __name__ == '__main__':
    main()
