"""hexdump_va.py - dump bytes around a file offset or VA of a game binary.

    python tools/hexdump_va.py AoW.exe --file 0x18729 --len 96
    python tools/hexdump_va.py AoW.exe --va 0x419329 --len 96
"""
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'
sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
from pe_va import sections, foff_to_va, va_to_foff  # noqa: E402


def main():
    path = sys.argv[1]
    if '\\' not in path and not path.lower().startswith('c:'):
        path = GAME + path
    data, base, _secs = sections(path)
    args = sys.argv[2:]
    mode = '--file'
    off = None
    length = 96
    i = 0
    while i < len(args):
        if args[i] in ('--file', '--va'):
            mode = args[i]
            off = int(args[i + 1], 0)
            i += 2
        elif args[i] == '--len':
            length = int(args[i + 1], 0)
            i += 2
        else:
            off = int(args[i], 0)
            i += 1
    if mode == '--va':
        foff, nm = va_to_foff(path, off)
        print('VA 0x%08x -> file 0x%06x (%s)' % (off, foff, nm))
    else:
        foff = off
        self_va, nm = foff_to_va(path, off)
        print('file 0x%06x -> VA 0x%08x (%s)' % (off, self_va, nm))
    start = foff - 16
    chunk = data[start:start + length + 16]
    for row in range(0, len(chunk), 16):
        addr = start + row
        cells = ' '.join('%02x' % b for b in chunk[row:row + 16])
        txt = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk[row:row + 16])
        print('  0x%06x  %-47s  %s' % (addr, cells, txt))


if __name__ == '__main__':
    main()
