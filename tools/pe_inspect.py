#!/usr/bin/env python3
"""pe_inspect.py - Portable PE32 inspector for the AoW1 port workflow.

No third-party dependencies. Reports headers, sections, imports, exports,
resources (incl. Delphi DFM/TPF0 form resources) and section entropy.

Usage:
    python pe_inspect.py <file.exe> [more.dll ...]
    python pe_inspect.py --strings 6 <file.exe>
"""
import struct
import sys
import math


class PE:
    """Minimal PE32/PE32+ reader (AoW1 targets are all PE32/i386)."""
    def __init__(self, data):
        self.d = data
        if data[:2] != b'MZ':
            raise ValueError('not an MZ image')
        e = struct.unpack_from('<I', data, 0x3C)[0]
        if data[e:e + 4] != b'PE\x00\x00':
            raise ValueError('not a PE image')
        self.pe_off = e
        self.machine = struct.unpack_from('<H', data, e + 4)[0]
        self.nsec = struct.unpack_from('<H', data, e + 6)[0]
        self.timestamp = struct.unpack_from('<I', data, e + 8)[0]
        self.chars = struct.unpack_from('<H', data, e + 0x16)[0]
        ohsize = struct.unpack_from('<H', data, e + 0x14)[0]
        self.opt = e + 0x18
        self.magic = struct.unpack_from('<H', data, self.opt)[0]
        self.subsystem = struct.unpack_from('<H', data, self.opt + 0x44)[0]
        self.entry = struct.unpack_from('<I', data, self.opt + 0x10)[0]
        so = self.opt + ohsize
        self.sections = []
        for i in range(self.nsec):
            b = so + i * 40
            self.sections.append(dict(
                name=data[b:b + 8].rstrip(b'\x00').decode('latin1'),
                vsize=struct.unpack_from('<I', data, b + 8)[0],
                rva=struct.unpack_from('<I', data, b + 12)[0],
                rawsize=struct.unpack_from('<I', data, b + 16)[0],
                rawptr=struct.unpack_from('<I', data, b + 20)[0],
                chars=struct.unpack_from('<I', data, b + 36)[0]))
        self.num_dd = struct.unpack_from('<I', data, self.opt + 0x5C)[0]
        self.dd = []
        for i in range(min(self.num_dd, 16)):
            rva, size = struct.unpack_from('<II', data, self.opt + 0x60 + i * 8)
            self.dd.append((rva, size))

    def rva2off(self, rva):
        for s in self.sections:
            if s['rva'] <= rva < s['rva'] + max(s['vsize'], s['rawsize']):
                return s['rawptr'] + (rva - s['rva'])
        return None

    def cstr(self, off):
        end = self.d.index(b'\x00', off)
        return self.d[off:end].decode('latin1')

    def imports(self):
        rva = self.dd[1][0]
        if not rva:
            return []
        off = self.rva2off(rva)
        out, i = [], 0
        while True:
            b = off + i * 20
            if b + 20 > len(self.d):
                break
            name_rva = struct.unpack_from('<I', self.d, b + 12)[0]
            if name_rva == 0:
                break
            out.append(self.cstr(self.rva2off(name_rva)))
            i += 1
        return out

    def exports(self):
        rva = self.dd[0][0]
        if not rva:
            return []
        off = self.rva2off(rva)
        nfun = struct.unpack_from('<I', self.d, off + 0x14)[0]
        if nfun == 0:
            return []
        out = []
        for i in range(min(nfun, 20000)):
            nr = struct.unpack_from('<I', self.d, off + 0x20 + i * 4)[0]
            if nr:
                out.append(self.cstr(self.rva2off(nr)))
        return out

    def resources(self):
        """Yield (path, size, file_off) for every leaf resource.

        PE resource tree is Type -> Name -> Language -> data-entry.  The
        3rd level entry points at an IMAGE_RESOURCE_DATA_ENTRY.
        """
        rva = self.dd[2][0]
        if not rva:
            return
        base = self.rva2off(rva)

        def label(name):
            if name & 0x80000000:  # offset to a UTF-16 name string
                o = base + (name & 0x7FFFFFFF)
                ln = struct.unpack_from('<H', self.d, o)[0]
                return self.d[o + 2:o + 2 + ln * 2].decode('utf-16-le')
            return name

        def walk(dir_off, path):
            # NumberOfNamedEntries is at +12, NumberOfIdEntries at +14.
            n_named, n_id = struct.unpack_from('<HH', self.d, dir_off + 12)
            for i in range(n_named + n_id):
                e = dir_off + 16 + i * 8
                name, off = struct.unpack_from('<II', self.d, e)
                if off & 0x80000000:  # OffsetToData -> subdirectory
                    yield from walk(base + (off & 0x7FFFFFFF),
                                    path + [label(name)])
                else:                 # OffsetToData -> data entry (leaf)
                    drva, size = struct.unpack_from('<II', self.d, base + off)
                    yield (path + [label(name)], size, self.rva2off(drva))
        yield from walk(base, [])

    def section_entropy(self):
        for s in self.sections:
            raw = self.d[s['rawptr']:s['rawptr'] + s['rawsize']]
            if not raw:
                yield s['name'], 0.0
                continue
            counts = [0] * 256
            for b in raw:
                counts[b] += 1
            ent = 0.0
            for c in counts:
                if c:
                    p = c / len(raw)
                    ent -= p * math.log2(p)
            yield s['name'], ent



# ------------------------------------------------------------------- report
def report(path, string_min=0):
    d = open(path, 'rb').read()
    pe = PE(d)
    print('=' * 70)
    print(path, f'({len(d)} bytes)')
    arch = {0x14c: 'i386', 0x8664: 'amd64'}.get(pe.machine, hex(pe.machine))
    print(f'  arch={arch}  sections={pe.nsec}  subsystem={pe.subsystem}  '
          f'ep=0x{pe.entry:x}')
    delphi = (pe.timestamp == 0x2A425E19) or (pe.chars & 0x818E) == 0x818E
    print(f'  timestamp=0x{pe.timestamp:08x}  chars=0x{pe.chars:04x}  '
          f'delphi_like={delphi}')
    print('  sections (entropy):')
    for (n, e) in pe.section_entropy():
        print(f'    {n:10s} {e:.2f}')
    imps = pe.imports()
    print(f'  imports ({len(imps)}): {", ".join(imps) if imps else "-"}')
    exps = pe.exports()
    if exps:
        tail = ' ...' if len(exps) > 25 else ''
        print(f'  exports ({len(exps)}): {", ".join(exps[:25])}{tail}')
    forms = 0
    for _p, size, off in pe.resources():
        if size and off is not None and d[off:off + 4] == b'TPF0':
            forms += 1
    if forms:
        print(f'  delphi_forms (TPF0 resources): {forms}')
    if string_min:
        import re
        seen = set()
        for m in re.finditer(rb'[ -~]{%d,}' % string_min, d):
            s = m.group().decode('latin1')
            if s not in seen:
                seen.add(s)
                print('   str', hex(m.start()), s)


def main(argv):
    args = list(argv[1:])
    smin = 0
    if args and args[0] == '--strings':
        smin = int(args[1])
        args = args[2:]
    if not args:
        print(__doc__)
        return 1
    for p in args:
        try:
            report(p, smin)
        except Exception as ex:
            print(f'{p}: ERROR {ex}')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
