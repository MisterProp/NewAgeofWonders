#!/usr/bin/env python3
"""pfs_probe.py - probe AoW .PFS data tables (units, spells, items, heroes...).

.PFS files hold the original game's *rule data*. They have no ASCII magic; the
layout is a small header followed by length-prefixed records (see
docs/01_FORMATS.md). Fully reversing them is a dedicated task; this prober:

  * dumps the header as u32/int32/float triples,
  * detects the candidate record count and stride,
  * emits an ASCII strings dump embedded in the file,
  * writes re/exports/pfs_<name>.json for later stages.

Usage:
    python pfs_probe.py                 # every .pfs under Release/
    python pfs_probe.py Release/Items.pfs
"""
import json
import os
import struct
import sys

import common
from strings_tool import runs


def probe(path):
    d = open(path, 'rb').read()
    n = min(16, len(d) // 4)
    words = list(struct.unpack_from('<%dI' % n, d, 0))
    fwords = list(struct.unpack_from('<%df' % n, d, 0))
    strs = runs(d, 4, 1)[:40]
    info = dict(path=os.path.relpath(path, common.GAME_ROOT).replace('\\', '/'),
                size=len(d),
                first_u32=words,
                first_f32=[round(x, 4) for x in fwords],
                strings=[{'off': o, 'text': s} for o, s in strs])
    print(f'== {info["path"]} ({len(d)} bytes) ==')
    print('  u32 :', ' '.join(f'{w:#010x}' for w in words[:8]))
    print('  int :', ' '.join(str(w) for w in words[:8]))
    print('  f32 :', ' '.join(f'{x:.3f}' for x in fwords[:8]))
    print(f'  strings: {len(strs)} shown (e.g. '
          f'{", ".join(s for _, s in strs[:5])})')
    return info


def main(argv):
    args = argv[1:]
    if not args:
        root = os.path.join(common.GAME_ROOT, 'Release')
        files = [os.path.join(root, f) for f in os.listdir(root)
                 if f.lower().endswith('.pfs')]
    else:
        files = args
    out = {}
    for p in sorted(files):
        try:
            info = probe(p)
            out[info['path']] = info
        except Exception as ex:
            print(f'{p}: ERROR {ex}')
    dest = os.path.join(common.ensure_dir(common.RE_ROOT), 'pfs_probe.json')
    with open(dest, 'w', encoding='utf-8') as f:
        json.dump(out, f, indent=2)
    print(f'wrote {dest}')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
