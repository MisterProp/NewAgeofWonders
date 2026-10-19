"""iat_lookup.py - resolve an import-address-table slot to DLL!function.

Delphi calls imports through `call dword ptr [<IAT slot>]`, so knowing what a
slot holds is often the fastest way to identify an unknown routine. This walks
the PE import descriptors by hand (stdlib only).

    python tools/iat_lookup.py AoW.exe 0x45df70 0x45d698 0x45d798
"""
import struct
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'


def load(path):
    if '\\' not in path and not path.lower().startswith('c:'):
        path = GAME + path
    data = open(path, 'rb').read()
    e = struct.unpack_from('<I', data, 0x3C)[0]
    nsec = struct.unpack_from('<H', data, e + 6)[0]
    opt_size = struct.unpack_from('<H', data, e + 20)[0]
    opt = e + 24
    image_base = struct.unpack_from('<I', data, opt + 28)[0]
    sec = opt + opt_size
    secs = []
    for i in range(nsec):
        o = sec + i * 40
        name = data[o:o + 8].rstrip(b'\0').decode('ascii', 'replace')
        vsize, vaddr, rsize, raw = struct.unpack_from('<IIII', data, o + 8)
        secs.append((name, vaddr, vsize, raw, rsize))
    # data directory 1 = imports
    dd = opt + 96
    imp_rva, imp_size = struct.unpack_from('<II', data, dd + 8)
    return data, image_base, secs, imp_rva, imp_size


def rva_to_off(secs, rva):
    for _n, vaddr, vsize, raw, rsize in secs:
        if vaddr <= rva < vaddr + max(vsize, rsize):
            return raw + (rva - vaddr)
    return None


def cstr(data, off):
    end = data.find(b'\0', off)
    return data[off:end].decode('ascii', 'replace')


def main():
    data, base, secs, imp_rva, _imp_size = load(sys.argv[1])
    descs = []
    p = rva_to_off(secs, imp_rva)
    while True:
        oft, ts, fc, name_rva, first_thunk = struct.unpack_from(
            '<IIIII', data, p)
        if not (oft or name_rva or first_thunk):
            break
        dll_off = rva_to_off(secs, name_rva)
        descs.append((oft, first_thunk, cstr(data, dll_off)))
        p += 20

    for arg in sys.argv[2:]:
        slot = int(arg, 0)
        rva = slot - base
        hit = None
        for oft, first_thunk, dll in descs:
            # Walk the thunk array to find the index of this slot.
            i = 0
            while True:
                cur = first_thunk + i * 4
                if cur == rva:
                    hit = (oft, i, dll)
                    break
                # stop at the terminator of this descriptor
                off = rva_to_off(secs, cur)
                if off is None or struct.unpack_from('<I', data, off)[0] == 0:
                    break
                i += 1
                if i > 4096:
                    break
            if hit:
                break
        if not hit:
            print('0x%08x  (not an import slot)' % slot)
            continue
        oft, i, dll = hit
        off = rva_to_off(secs, oft + i * 4)
        if off is None:
            print('0x%08x  %s!<unnamed slot %d>' % (slot, dll, i))
            continue
        entry = struct.unpack_from('<I', data, off)[0]
        if entry & 0x80000000:
            print('0x%08x  %s!Ordinal_%d' % (slot, dll, entry & 0xFFFF))
        else:
            nb = rva_to_off(secs, entry)
            hint = struct.unpack_from('<H', data, nb)[0]
            print('0x%08x  %s!%s  (hint %d)'
                  % (slot, dll, cstr(data, nb + 2), hint))


if __name__ == '__main__':
    main()
