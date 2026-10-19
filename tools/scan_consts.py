"""scan_consts.py - search AoW.exe's code section for instruction patterns that
reveal the title-menu layout arithmetic.

We look for `imul reg,reg,0x78` (120 = the button pitch found in FUN_00419488)
and for the literal 0x140 (320 = half of 640).

    python tools/scan_consts.py AoW.exe 6b c0 78 -- 69 c0 78 00 00 00
"""
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'
TOOLS = r'C:\GOG Games\Age of Wonders\port\tools'
sys.path.insert(0, TOOLS)
from pe_va import sections  # noqa: E402


def main():
    path = sys.argv[1]
    if '\\' not in path and not path.lower().startswith('c:'):
        path = GAME + path
    data, base, secs = sections(path)
    _n, cva, _vs, craw, crsize = next(
        s for s in secs if s[0] in ('CODE', '.text'))
    code = data[craw:craw + crsize]

    pats = {
        'imul r32,r32,0x78': bytes.fromhex('6bc078'),
        'imul eax,0x78': bytes.fromhex('69c078000000'),
        'imul edx,0x78': bytes.fromhex('69d278000000'),
        'imul ebx,0x78': bytes.fromhex('69db78000000'),
        'imul esi,0x78': bytes.fromhex('69f678000000'),
        'imul edi,0x78': bytes.fromhex('69ff78000000'),
    }
    for label, pat in pats.items():
        offs = []
        i = code.find(pat)
        while i >= 0 and len(offs) < 40:
            offs.append(i)
            i = code.find(pat, i + 1)
        print('== %s : %d' % (label, len(offs)))
        for o in offs:
            print('   file 0x%06x  VA 0x%08x' % (craw + o, base + cva + o))


if __name__ == '__main__':
    main()
