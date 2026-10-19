"""_pe_dat.py - read raw bytes at virtual addresses in a PE (for Ghidra DAT_*)."""
import struct
import sys


def va_to_off(data, va):
    e = struct.unpack_from('<I', data, 0x3c)[0]
    coff = e + 4
    nsec = struct.unpack_from('<H', data, coff + 2)[0]
    opt_size = struct.unpack_from('<H', data, coff + 16)[0]
    opt = coff + 20
    magic = struct.unpack_from('<H', data, opt)[0]
    image_base = struct.unpack_from('<I', data, opt + 28)[0]
    sec = opt + opt_size
    rva = va - image_base
    for i in range(nsec):
        s = sec + i * 40
        vsz = struct.unpack_from('<I', data, s + 8)[0]
        vaddr = struct.unpack_from('<I', data, s + 12)[0]
        rawsize = struct.unpack_from('<I', data, s + 16)[0]
        raw = struct.unpack_from('<I', data, s + 20)[0]
        if vaddr <= rva < vaddr + max(vsz, rawsize):
            return raw + (rva - vaddr), image_base
    return None, image_base


if __name__ == '__main__':
    path = sys.argv[1]
    data = open(path, 'rb').read()
    for arg in sys.argv[2:]:
        va = int(arg, 16)
        off, ib = va_to_off(data, va)
        if off is None:
            print('0x%08x: (not mapped)' % va)
            continue
        chunk = data[off:off + 48]
        asc = ''.join(chr(c) if 32 <= c < 127 else '.' for c in chunk)
        print('0x%08x -> %r  | %s' % (va, chunk[:32], asc))
