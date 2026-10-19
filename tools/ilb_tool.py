#!/usr/bin/env python3
"""ilb_tool.py - enumerate Age of Wonders .ILB image libraries.

.ILB is AoW's sprite/image container. Observed header (little endian):

  0x00  char[4]  magic  = 04 'I' 'L' 'B'
  0x04  u32      flags
  0x08  float32  format version (observed 4.0)
  0x0C  u32      header size (observed 24)
  0x10  u32      image data offset (observed == size of the directory area)
  0x14  u32      total file size
  0x18  u32      reserved 0
  0x1C  u32      reserved 0
  0x20  u32      image count
  0x24+ directory entries. Each entry appears to begin with a u32 tag, a u8
        record type then a length prefixed name (e.g. 'UnitFr.BMP'); the exact
        per-entry layout is still being reversed (see docs/01_FORMATS.md).

This tool reliably dumps the header and enumerates every entry name, which is
enough to build the asset inventory for the C++ port. Full pixel decoding is a
TODO tracked in the roadmap.

Usage:
    python ilb_tool.py                  # scan every .ILB in the game
    python ilb_tool.py Int/Icons.ILB    # one file
    python ilb_tool.py --list           # names only
"""
import json
import struct
import sys

import common

MAGIC = b'\x04ILB'


def parse_header(d):
    magic = d[:4]
    if magic != MAGIC:
        raise ValueError('bad ILB magic %r' % magic)
    flags = struct.unpack_from('<I', d, 4)[0]
    version = struct.unpack_from('<f', d, 8)[0]
    hdr_size = struct.unpack_from('<I', d, 0x0C)[0]
    data_off = struct.unpack_from('<I', d, 0x10)[0]
    total = struct.unpack_from('<I', d, 0x14)[0]
    r0 = struct.unpack_from('<I', d, 0x18)[0]
    r1 = struct.unpack_from('<I', d, 0x1C)[0]
    count = struct.unpack_from('<I', d, 0x20)[0]
    return dict(flags=flags, version=version, header_size=hdr_size,
                data_offset=data_off, total_size=total, reserved=(r0, r1),
                count=count)


def entry_names(d, hdr):
    """Heuristic: scan the directory region for length-prefixed names."""
    names = []
    p = 0x20
    end = min(hdr['data_offset'] or len(d), len(d))
    while p < end:
        ln = struct.unpack_from('<I', d, p)[0]
        if 1 <= ln <= 260 and p + 4 + ln <= len(d):
            raw = d[p + 4:p + 4 + ln]
            if all(32 <= c < 127 for c in raw):
                names.append(raw.decode('latin1'))
                p += 4 + ln
                continue
        p += 1
    return names


def main(argv):
    args = argv[1:]
    listed = '--list' in args
    args = [a for a in args if a != '--list']
    if args:
        files = args
    else:
        import os
        files = []
        for dp, _dn, fn in os.walk(common.GAME_ROOT):
            if os.sep + 'port' + os.sep in dp + os.sep:
                continue
            for f in fn:
                if f.lower().endswith('.ilb'):
                    files.append(os.path.join(dp, f))
    inv = {}
    for path in sorted(files):
        try:
            d = open(path, 'rb').read()
            hdr = parse_header(d)
            names = entry_names(d, hdr)
        except Exception as ex:
            print(f'{path}: ERROR {ex}')
            continue
        rel = path.replace(common.GAME_ROOT + '\\', '').replace('\\', '/')
        inv[rel] = dict(header=hdr, names=names)
        if listed:
            print(f'{rel}  count={hdr["count"]} names={len(names)}')
            for n in names:
                print('   ', n)
        else:
            print(f'{rel}  count={hdr["count"]} names={len(names)} '
                  f'version={hdr["version"]:.2f} data_off={hdr["data_offset"]} '
                  f'total={hdr["total_size"]}')
    out = common.ensure_dir(common.RE_ROOT)
    dest = out + '/ilb_inventory.json'
    with open(dest, 'w', encoding='utf-8') as f:
        json.dump(inv, f, indent=2)
    print(f'wrote {dest} ({len(inv)} ILB files)')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
