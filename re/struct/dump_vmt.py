import struct
import sys

sys.path.insert(0, r'c:\GOG Games\Age of Wonders\port\tools')
import pe_va  # noqa: E402

path = r'C:\GOG Games\Age of Wonders\AoW.exe'
data, base, secs = pe_va.sections(path)


def read(va, length):
    rva = va - 0x400000
    for _n, vaddr, vsize, raw, rsize in secs:
        if vaddr <= rva < vaddr + max(vsize, rsize):
            off = raw + (rva - vaddr)
            return data[off:off + length]
    raise SystemExit('VA 0x%x not mapped' % va)


va = int(sys.argv[1], 16)
length = int(sys.argv[2]) if len(sys.argv) > 2 else 256
blob = read(va, length)
for i in range(0, len(blob), 4):
    print('%08x %08x' % (va + i, struct.unpack_from('<I', blob, i)[0]))
