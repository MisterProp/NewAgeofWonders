"""Decompile every AoW.exe function through the GhidraMCP bridge.

Writes one .c file per function under re/struct/all/ and an index.
"""
import concurrent.futures
import os
import re
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
OUT = r'c:\GOG Games\Age of Wonders\port\re\struct\all'
os.makedirs(OUT, exist_ok=True)

lines = open(r'c:\GOG Games\Age of Wonders\port\re\struct\aow_funcs.txt',
             encoding='utf-8', errors='replace').read().splitlines()
funcs = []
for line in lines:
    m = re.search(r' at ([0-9a-fA-F]{8})', line)
    if not m:
        continue
    addr = int(m.group(1), 16)
    if addr < 0x402000:       # skip the Borland RTL
        continue
    funcs.append(addr)


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=180) as r:
        return r.read().decode('utf-8', 'replace')


def dump(addr):
    dest = os.path.join(OUT, '%08x.c' % addr)
    if os.path.exists(dest) and os.path.getsize(dest) > 32:
        return addr, 'cached'
    try:
        body = get('/decompile_function', address='0x%08x' % addr,
                   program='/AoW.exe')
    except Exception as exc:  # noqa: BLE001
        return addr, 'ERR %s' % exc
    with open(dest, 'w', encoding='utf-8') as f:
        f.write(body)
    return addr, 'ok'


if __name__ == '__main__':
    print('functions:', len(funcs))
    done = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as pool:
        for addr, state in pool.map(dump, funcs):
            done += 1
            if done % 50 == 0:
                print('%d/%d' % (done, len(funcs)), flush=True)
    print('done')
