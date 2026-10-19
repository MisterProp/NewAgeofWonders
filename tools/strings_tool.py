#!/usr/bin/env python3
"""strings_tool.py - extract readable strings from binaries and data files.

Works for compressed-free Delphi binaries, .mld localization dictionaries and
.pfs data tables alike (the same printable-run heuristic). Used early in the
workflow to harvest UI text, unit/spell names, error messages, etc. which both
documents the game and seeds the C++ port's translation tables.

Usage:
    python strings_tool.py <file> [--min N] [--ascii|--utf16|--both]
    python strings_tool.py --scan-mld            # every Dict/*.mld
"""
import os
import sys

import common


def runs(data, min_len, width):
    if width == 1:
        pattern = bytes(range(32, 127))
    out = []
    cur = bytearray()
    start = 0
    step = width
    i = 0
    while i + width <= len(data):
        ch = data[i]
        printable = 32 <= ch < 127
        if width == 2:
            printable = printable and data[i + 1] == 0
        if printable:
            if not cur:
                start = i
            cur.append(ch)
            i += step
        else:
            if len(cur) >= min_len:
                out.append((start, cur.decode('latin1')))
            cur = bytearray()
            i += 1
    if len(cur) >= min_len:
        out.append((start, cur.decode('latin1')))
    return out


def main(argv):
    args = argv[1:]
    min_len = 5
    mode = 'ascii'
    if '--min' in args:
        i = args.index('--min')
        min_len = int(args[i + 1])
        del args[i:i + 2]
    for m in ('--ascii', '--utf16', '--both'):
        if m in args:
            mode = m[2:]
            args.remove(m)
    if args and args[0] == '--scan-mld':
        d = os.path.join(common.GAME_ROOT, 'Dict')
        files = [os.path.join(d, f) for f in os.listdir(d)
                 if f.lower().endswith('.mld')]
    else:
        files = args
    total = 0
    for path in files:
        data = open(path, 'rb').read()
        print(f'== {path} ({len(data)} bytes) ==')
        found = []
        if mode in ('ascii', 'both'):
            found += [(o, s, 1) for o, s in runs(data, min_len, 1)]
        if mode in ('utf16', 'both'):
            found += [(o, s, 2) for o, s in runs(data, min_len, 2)]
        found.sort()
        for o, s, w in found:
            print(f'  {o:#08x} w{w}: {s}')
        total += len(found)
    print(f'# {total} strings')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
