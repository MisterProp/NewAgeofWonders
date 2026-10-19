"""find_ansistr.py - locate Delphi AnsiString *constants* in a binary.

A literal `resourcestring`/AnsiString constant is stored as
`u32 length` + bytes + NUL, so searching for that shape finds the exact
constant pool an address points at - which is how the title screen's five
caption strings can be read out of AoW.exe.

    python tools/find_ansistr.py AoW.exe Scenario "New Game"
    python tools/find_ansistr.py AoW.exe --dump 0x5bd80 5
"""
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'


def main():
    path = sys.argv[1]
    if '\\' not in path and not path.lower().startswith('c:'):
        path = GAME + path
    data = open(path, 'rb').read()

    args = sys.argv[2:]
    if args and args[0] == '--dump':
        off = int(args[1], 0)
        n = int(args[2], 0)
        for i in range(n):
            p = off + i * 4
            import struct
            (v,) = struct.unpack_from('<I', data, p)
            # Try to read a length-prefixed string at v (RVA or file offset).
            for base in (v, v - 0x400000):
                if 0 < base < len(data) - 8:
                    (ln,) = struct.unpack_from('<I', data, base)
                    if 0 < ln < 200:
                        s = data[base + 4:base + 4 + ln]
                        if all(32 <= c < 127 for c in s):
                            print('  0x%08x -> 0x%06x  %r' % (p, v, s.decode()))
                            break
            else:
                print('  0x%08x -> 0x%08x  (not a string)' % (p, v))
        return

    for needle in args:
        pat = needle.encode('ascii')
        print('== %s' % needle)
        start = 0
        found = 0
        while True:
            j = data.find(pat, start)
            if j < 0:
                break
            # Is it a length-prefixed constant?
            prefixed = j >= 4 and data[j - 4:j] == bytes([len(pat), 0, 0, 0])
            term = data[j + len(pat):j + len(pat) + 1] == b'\0'
            if prefixed and term:
                lo = max(0, j - 24)
                chunk = data[lo:j + len(pat) + 24]
                txt = ''.join(chr(c) if 32 <= c < 127 else '.'
                              for c in chunk)
                print('  0x%06x  AnsiString constant  ...%s...' % (j, txt))
                found += 1
            start = j + 1
        if not found:
            print('  (no length-prefixed constant)')


if __name__ == '__main__':
    main()
