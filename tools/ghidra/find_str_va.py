"""find_str_va.py - find an ASCII/AnsiString in a PE and print its VA + xrefs.

The DFM event names (e.g. "MapWindowMouseDown") and the Delphi published-method
tables are stored as length-prefixed AnsiStrings in the binary, so we can locate
them by a raw byte scan, convert the file offset to a VA, and then ask GhidraMCP
who references that VA.

    python tools/ghidra/find_str_va.py <pe> <text>
"""
import struct
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


def sections(data):
    e_lfanew = struct.unpack_from('<I', data, 0x3c)[0]
    nsec = struct.unpack_from('<H', data, e_lfanew + 6)[0]
    opt = e_lfanew + 24
    magic = struct.unpack_from('<H', data, opt)[0]
    opt_size = struct.unpack_from('<H', data, e_lfanew + 20)[0]
    base = struct.unpack_from('<I', data, opt + 28)[0]
    out = []
    sec = opt + opt_size
    for i in range(nsec):
        off = sec + i * 40
        name = data[off:off + 8].rstrip(b'\0').decode('latin-1')
        va = struct.unpack_from('<I', data, off + 12)[0]
        vsize = struct.unpack_from('<I', data, off + 8)[0]
        raw = struct.unpack_from('<I', data, off + 20)[0]
        rawsize = struct.unpack_from('<I', data, off + 16)[0]
        out.append((name, va, vsize, raw, rawsize))
    return base, out, magic


def off_to_va(base, secs, off):
    for name, va, vsize, raw, rawsize in secs:
        if raw <= off < raw + max(rawsize, vsize):
            return base + va + (off - raw)
    return None


def main():
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    path = sys.argv[1]
    needle = sys.argv[2].encode('latin-1')
    prog = '/' + path.replace('\\', '/').rsplit('/', 1)[-1]
    data = open(path, 'rb').read()
    base, secs, _ = sections(data)
    offs = []
    start = 0
    while True:
        i = data.find(needle, start)
        if i < 0:
            break
        offs.append(i)
        start = i + 1
    print('%d occurrence(s) of %r' % (len(offs), needle))
    get('/open_program', path=prog)
    for off in offs:
        va = off_to_va(base, secs, off)
        print('--- file 0x%06x -> VA %s' % (off, ('%08x' % va) if va else '?'))
        if va:
            print(get('/get_xrefs_to', address='%08x' % va).strip())


if __name__ == '__main__':
    main()
