#!/usr/bin/env python3
"""dfm_extract.py - extract Delphi form resources (TPF0) from AoW binaries.

Every Delphi form compiled into an EXE/package is stored as a resource whose
payload starts with the magic 'TPF0'. Recovering these gives you the exact UI
object tree (TForm, TButton, positions, fonts, images...) which is a huge
head start for re-implementing the interface in C++.

Output:
    assets/dfm/<module>/<form>.dfm   (raw binary DFM stream)

Optionally a best-effort textual tree:
    python dfm_extract.py --parse AoW.exe

Usage:
    python dfm_extract.py [--parse] <bin> [<bin> ...]
"""
import os
import sys

import common
from pe_inspect import PE


def res_name(path):
    # path == [type, name, language]; the form name is the 'name' level.
    val = path[1] if len(path) >= 2 else path[-1]
    return f'res{val}' if isinstance(val, int) else str(val)


def resolve(p):
    """Allow relative names to be given from the game root or cwd."""
    if os.path.isabs(p) and os.path.exists(p):
        return p
    for base in (os.getcwd(), common.GAME_ROOT):
        cand = os.path.join(base, p)
        if os.path.exists(cand):
            return cand
    return p


def extract(path, parse=False):
    path = resolve(path)
    d = open(path, 'rb').read()
    pe = PE(d)
    module = os.path.splitext(os.path.basename(path))[0]
    outdir = common.ensure_dir(os.path.join(common.ASSET_ROOT, 'dfm', module))
    n = 0
    for rpath, size, off in pe.resources():
        if not size or off is None:
            continue
        if d[off:off + 4] != b'TPF0':
            continue
        name = res_name(rpath)
        data = d[off:off + size]
        dest = os.path.join(outdir, f'{name}.dfm')
        with open(dest, 'wb') as f:
            f.write(data)
        n += 1
        if parse:
            print(f'--- {module}/{name} ---')
            try:
                for line in walk_dfm(data):
                    print('   ', line)
            except Exception as ex:
                print('    parse error:', ex)
    print(f'{module}: extracted {n} form(s) -> {outdir}')
    return n


# ----------------------------------------------------- binary DFM parser
# Format (Delphi TStream): a sequence of objects until byte 0.
# Object:  flags(u8) then (if flags&0x02) class short-str, then name short-str.
#   flags: 0x01 inherited, 0x02 has class+name, 0x04 has child, 0x08 part of
#          large collection (skip)
# Property: name short-str, then value-tag (u8), then value.
def _sstr(d, p):
    ln = d[p]
    p += 1
    # short strings can be multi-byte length for very long names
    if ln & 0x80:
        n = (ln & 0x7F) << 8 | d[p]
        p += 1
        ln = n
    s = d[p:p + ln].decode('latin1')
    return s, p + ln


def walk_dfm(d, depth=0, p=0):
    out = []
    while p < len(d):
        flags = d[p]
        p += 1
        if flags == 0:
            break
        cls = name = None
        if flags & 0x02:
            cls, p = _sstr(d, p)
            name, p = _sstr(d, p)
        out.append('  ' * depth + f'object {cls} {name}'
                   if cls else '  ' * depth + '<child>')
        # properties: keep reading until next object start is ambiguous, so we
        # only decode a few common tags and continue conservatively.
        while p < len(d):
            nxt = d[p]
            if nxt == 0:
                p += 1
                break
            if nxt in (0x01, 0x02, 0x04, 0x08, 0x10):  # likely next object flag
                break
            pname, p = _sstr(d, p)
            tag = d[p]
            p += 1
            out.append('  ' * (depth + 1) + f'{pname} <tag 0x{tag:02x}>')
            # skip a value heuristically
            if tag in (0x0C, 0x0D, 0x12):  # string / ident / utf8
                _, p = _sstr(d, p)
            elif tag in (0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                         0x08, 0x16):
                p += {0x00: 1, 0x01: 1, 0x02: 2, 0x03: 3, 0x04: 4, 0x05: 4,
                      0x06: 6, 0x07: 8, 0x08: 10, 0x16: 4}[tag]
            else:
                break
        if flags & 0x04:
            out += walk_dfm(d, depth + 1, p)
            # child block terminated by 0 already consumed by inner loop
    return out


def main(argv):
    args = argv[1:]
    parse = '--parse' in args
    args = [a for a in args if a != '--parse']
    if not args:
        args = [os.path.join(common.GAME_ROOT, 'AoW.exe'),
                os.path.join(common.GAME_ROOT, 'AoWEd.exe')]
    total = 0
    for a in args:
        try:
            total += extract(a, parse)
        except Exception as ex:
            print(f'{a}: ERROR {ex}')
    print(f'TOTAL forms: {total}')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
