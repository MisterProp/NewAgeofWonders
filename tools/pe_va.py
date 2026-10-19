"""pe_va.py - map between file offsets and virtual addresses for the AoW PEs.

Delphi/Borland linkers put sections at their natural RVA (kind="rva"), but we
verify from the section table rather than assume.

    python tools/pe_va.py AoW.exe 0x13aebb 0x18729
    python tools/pe_va.py AoW.exe --va 0x413aeb
"""
import struct
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'


def sections(path):
    d = open(path, 'rb').read()
    e_lfanew = struct.unpack_from('<I', d, 0x3C)[0]
    assert d[e_lfanew:e_lfanew + 4] == b'PE\0\0', 'not a PE'
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


def foff_to_va(path, off):
    _d, base, secs = sections(path)
    for name, vaddr, vsize, raw, rsize in secs:
        if raw <= off < raw + rsize:
            return base + vaddr + (off - raw), name
    return None, None


def va_to_foff(path, va):
    _d, base, secs = sections(path)
    rva = va - base
    for name, vaddr, vsize, raw, rsize in secs:
        if vaddr <= rva < vaddr + max(vsize, rsize):
            return raw + (rva - vaddr), name
    return None, None


def main():
    path = sys.argv[1]
    if not path.lower().startswith(('c:', '\\')) and '\\' not in path:
        path = GAME + path
    d, base, secs = sections(path)
    print('%s  image_base=0x%08x' % (path, base))
    for name, vaddr, vsize, raw, rsize in secs:
        print('   %-8s va=0x%08x vsize=0x%06x raw=0x%06x rsize=0x%06x'
              % (name, base + vaddr, vsize, raw, rsize))
    args = sys.argv[2:]
    if args and args[0] == '--va':
        for a in args[1:]:
            va = int(a, 0)
            off, nm = va_to_foff(path, va)
            print('   VA 0x%08x -> file 0x%06x (%s)' % (va, off or 0, nm))
    else:
        for a in args:
            off = int(a, 0)
            va, nm = foff_to_va(path, off)
            print('   file 0x%06x -> VA 0x%08x (%s)' % (off, va or 0, nm))


if __name__ == '__main__':
    main()
