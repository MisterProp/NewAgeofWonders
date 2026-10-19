#!/usr/bin/env python3
"""vmt_scan.py - walk the Delphi class hierarchy out of an AoW1 PE.

Age of Wonders 1 is Delphi, so every class keeps a VMT (virtual method table)
whose header sits just before the first virtual method slot:

    vmt - 76  vmtSelfPtr  ->  `vmt - 12` (the dword at VA `v` holds `v + 64`)
    vmt - 44  vmtClassName -> a ShortString (`u8 length` + chars)
    vmt - 40  vmtInstanceSize
    vmt - 36  vmtParent     ->  `parentVmt - 76`  (NOT the parent VMT itself)

`vmtSelfPtr`'s `value == own address + 64` is a self-identifying pattern, so a
single pass over the image finds every class; from there the parent chain links
them into the real inheritance tree. That is how the port established which map
objects are *ownable* (`AoWE.TPlayerStructure` and its subclasses, whose `Show`
draws the owner flag - see docs/19_STRUCTURE_FLAGS.md).

    python tools/vmt_scan.py AoWEPACK.dpl                    # every class
    python tools/vmt_scan.py AoWEPACK.dpl --children TPlayerStructure
    python tools/vmt_scan.py AoWEPACK.dpl TMine TTower TCity # chains only

Pass a bare file name to look under the game root. Standard library only.
"""
import struct
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

import common  # noqa: E402  (GAME_ROOT)


def sections(path):
    d = open(path, 'rb').read()
    e_lfanew = struct.unpack_from('<I', d, 0x3C)[0]
    if d[e_lfanew:e_lfanew + 4] != b'PE\0\0':
        raise SystemExit('not a PE: %s' % path)
    coff = e_lfanew + 4
    nsec, = struct.unpack_from('<H', d, coff + 2)
    opt_size, = struct.unpack_from('<H', d, coff + 16)
    opt = coff + 20
    image_base, = struct.unpack_from('<I', d, opt + 28)
    sec = opt + opt_size
    out = []
    for i in range(nsec):
        o = sec + i * 40
        name = d[o:o + 8].rstrip(b'\0').decode('ascii', 'replace')
        vsize, vaddr, rsize, raw = struct.unpack_from('<IIII', d, o + 8)
        out.append((name, vaddr, vsize, raw, rsize))
    return d, image_base, out


class Pe:
    def __init__(self, path):
        self.d, self.base, self.secs = sections(path)

    def va_to_foff(self, va):
        rva = va - self.base
        for _n, vaddr, vsize, raw, rsize in self.secs:
            if vaddr <= rva < vaddr + max(vsize, rsize):
                return raw + (rva - vaddr)
        return None

    def foff_to_va(self, off):
        for _n, vaddr, vsize, raw, rsize in self.secs:
            if raw <= off < raw + rsize:
                return self.base + vaddr + (off - raw)
        return None

    def u32(self, va):
        off = self.va_to_foff(va)
        if off is None or off + 4 > len(self.d):
            return None
        return struct.unpack_from('<I', self.d, off)[0]

    def sstr(self, va):
        off = self.va_to_foff(va)
        if off is None:
            return None
        n = self.d[off]
        if n < 1 or n > 60:
            return None
        try:
            return self.d[off + 1:off + 1 + n].decode('ascii')
        except UnicodeDecodeError:
            return None


def find_all(pe):
    """{className: (vmt, parentVmt)} for every class in the image."""
    out = {}
    for off in range(0, len(pe.d) - 8, 4):
        v = struct.unpack_from('<I', pe.d, off)[0]
        va = pe.foff_to_va(off)
        if va is None or v != va + 64:
            continue
        vmt = va + 76
        nv = pe.u32(vmt - 44)
        cn = pe.sstr(nv) if nv else None
        if not cn:
            continue
        pvv = pe.u32(vmt - 36)
        out.setdefault(cn, (vmt, (pvv + 76) if pvv else 0))
    return out


def ancestry(classes, vmt2name, name):
    """[class, parent, grandparent, ...]."""
    chain = [name]
    seen = set()
    cur = classes.get(name, (0, 0))[1]
    while cur and cur not in seen:
        seen.add(cur)
        cn = vmt2name.get(cur)
        if not cn:
            break
        chain.append(cn)
        cur = classes.get(cn, (0, 0))[1]
    return chain


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    path = sys.argv[1]
    import os
    if not path.lower().startswith(('c:', '\\')) and os.sep not in path:
        path = os.path.join(common.GAME_ROOT, path)
    pe = Pe(path)
    classes = find_all(pe)
    vmt2name = {v: n for n, (v, _p) in classes.items()}
    args = sys.argv[2:]
    if args and args[0] == '--children':
        want = args[1]
        print('== subclasses of %s (%d classes in the image)'
              % (want, len(classes)))
        for n in sorted(classes):
            ch = ancestry(classes, vmt2name, n)
            if want in ch:
                print('  %-32s vmt=0x%08x  %s' % (n, classes[n][0], ' < '.join(ch)))
        return
    if not args:
        for n in sorted(classes):
            print('%-36s vmt=0x%08x  %s'
                  % (n, classes[n][0], ' < '.join(ancestry(classes, vmt2name, n))))
        print('\n%d classes' % len(classes))
        return
    for n in args:
        if n not in classes:
            print('%-36s (not found)' % n)
            continue
        print('%-36s vmt=0x%08x  %s'
              % (n, classes[n][0], ' < '.join(ancestry(classes, vmt2name, n))))


if __name__ == '__main__':
    main()
