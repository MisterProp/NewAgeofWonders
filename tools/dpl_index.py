#!/usr/bin/env python3
"""dpl_index.py - index every AoW1 binary (exe + .dpl Delphi packages).

For each binary it reports the imported modules (the internal engine graph),
the exported API surface (the cross-package contract you must reproduce in
C++) and the number of Delphi DFM form resources.

Output:
  * human readable table to stdout
  * re/exports/module_index.json  (machine readable, used by later stages)

Usage:
    python dpl_index.py                 # scan the whole game root
    python dpl_index.py AoW.exe X.dpl   # explicit files
"""
import json
import os
import sys

import common
from pe_inspect import PE


def index_one(path):
    d = open(path, 'rb').read()
    pe = PE(d)
    exps = pe.exports()
    imps = []
    seen = set()
    for m in pe.imports():
        ml = m.lower()
        if ml not in seen:
            seen.add(ml)
            imps.append(m)
    forms = sum(1 for _p, size, off in pe.resources()
                if size and off is not None and d[off:off + 4] == b'TPF0')
    pretty = [common.demangle_delphi(e) for e in exps]
    return dict(
        path=os.path.relpath(path, common.GAME_ROOT).replace('\\', '/'),
        size=len(d),
        module=os.path.splitext(os.path.basename(path))[0],
        imports=[i for i in imps if i.lower().endswith(('.dpl', '.dll'))],
        raw_imports=imps,
        export_count=len(exps),
        exports=exps,
        exports_pretty=pretty,
        dfm_forms=forms,
    )


def main(argv):
    args = argv[1:]
    if args:
        targets = [(a, 'exe') for a in args]
    else:
        targets = list(common.iter_binaries())
    records = []
    print(f'{"module":22s} {"size":>9s} {"forms":>5s} {"exp":>5s}  imports')
    print('-' * 90)
    for path, _kind in sorted(targets, key=lambda t: t[0].lower()):
        try:
            r = index_one(path)
        except Exception as ex:
            print(f'{path}: ERROR {ex}')
            continue
        records.append(r)
        print(f'{r["module"]:22s} {r["size"]:9d} {r["dfm_forms"]:5d} '
              f'{r["export_count"]:5d}  {", ".join(r["imports"])}')
    out = common.ensure_dir(os.path.join(common.RE_ROOT, 'exports'))
    dest = os.path.join(out, 'module_index.json')
    with open(dest, 'w', encoding='utf-8') as f:
        json.dump(records, f, indent=2)
    print('-' * 90)
    print(f'wrote {dest} ({len(records)} modules)')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
