"""find_ref.py - locate code sites by absolute data reference or raw bytes.

Ghidra's auto-analysis did not create references for these Delphi string
constants, but `push offset str` in 32-bit x86 embeds the absolute VA as a
little-endian dword. We can also scan for arbitrary instruction byte patterns.

    python tools/find_ref.py AoW.exe --va 0x00419329
    python tools/find_ref.py AoW.exe --hex 6b c0 78
    python tools/find_ref.py AoW.exe --hex 69 ?? 78 00 00 00
"""
import struct
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'
sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
from pe_va import sections  # noqa: E402


def parse_pattern(tokens):
    out = []
    mask = []
    for t in tokens:
        if t in ('??', '**', '.'):
            out.append(0)
            mask.append(False)
        else:
            out.append(int(t, 16))
            mask.append(True)
    return bytes(out), mask


def match(data, start, pat, mask):
    for i, (b, m) in enumerate(zip(pat, mask)):
        if m and data[start + i] != b:
            return False
    return True


def main():
    path = sys.argv[1]
    if '\\' not in path and not path.lower().startswith('c:'):
        path = GAME + path
    data, base, secs = sections(path)
    name, cva, cvsize, craw, crsize = next(
        s for s in secs if s[0] in ('CODE', '.text'))
    cr_end = craw + crsize
    args = sys.argv[2:]
    mode = '--va'
    val = None
    if args[0] in ('--va', '--hex'):
        mode = args[0]
        val = args[1]
    if mode == '--va':
        pat = struct.pack('<I', int(val, 0))
        mask = [True] * 4
        print('== dword 0x%08x' % int(val, 0))
    else:
        pat, mask = parse_pattern(val.replace(',', ' ').split())
        print('== bytes %s' % val)
    plen = len(pat)
    hits = 0
    i = craw
    while i <= cr_end - plen:
        if match(data, i, pat, mask):
            print('   file 0x%06x  VA 0x%08x' % (i, base + cva + (i - craw)))
            hits += 1
            if hits >= 60:
                print('   ... (capped)')
                break
        i += 1
    if not hits:
        print('   (no site)')


if __name__ == '__main__':
    main()

