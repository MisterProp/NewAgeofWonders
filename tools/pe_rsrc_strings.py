"""pe_rsrc_strings.py - dump Delphi/Borland RT_STRING resources from a PE.

Delphi stores its `resourcestring` declarations in the standard Win32 string
table (resource type 6). Each block holds 16 length-prefixed UTF-16 strings, so
dumping them lets us recover captions that are only referenced by numeric id in
code (`LoadResString` / @InitResStringImports).

    python tools/pe_rsrc_strings.py AoW.exe
    python tools/pe_rsrc_strings.py AoW.exe --grep Game
"""
import struct
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'


def read_cstr(data, off):
    n = data[off]
    return data[off + 1:off + 1 + n].decode('latin-1')


def rsrc_root(data):
    e = struct.unpack_from('<I', data, 0x3C)[0]
    nsec = struct.unpack_from('<H', data, e + 6)[0]
    opt_size = struct.unpack_from('<H', data, e + 20)[0]
    opt = e + 24
    image_base = struct.unpack_from('<I', data, opt + 28)[0]
    sec = opt + opt_size
    rva = raw = vsize = 0
    for i in range(nsec):
        o = sec + i * 40
        name = data[o:o + 8].rstrip(b'\0').decode('ascii', 'replace')
        if name == '.rsrc':
            vsize, rva, _rsize, raw = struct.unpack_from('<IIII', data, o + 8)
            break
    return data[raw:raw + vsize], rva, raw, image_base, sec, nsec


def rva_to_off(data, sec, nsec, rva):
    """Map an RVA to a file offset using the section table."""
    for i in range(nsec):
        o = sec + i * 40
        vsize, vaddr, rsize, raw = struct.unpack_from('<IIII', data, o + 8)
        if vaddr <= rva < vaddr + max(vsize, rsize):
            return raw + (rva - vaddr)
    return None


def sub_entries(rsrc, sub):
    """Yield (id_or_namedflag, offset) of a resource directory's entries."""
    n_named, n_id = struct.unpack_from('<HH', rsrc, sub + 12)
    for i in range(n_named + n_id):
        o = sub + 16 + i * 8
        yield struct.unpack_from('<II', rsrc, o)


def find_string_tables(data):
    rsrc, _rsrc_rva, _raw, _base, sec, nsec = rsrc_root(data)
    out = []
    for name_id, off in sub_entries(rsrc, 0):
        sub = off & 0x7FFFFFFF
        if (name_id & 0x80000000) or name_id != 6:  # 6 == RT_STRING
            continue
        for blk_id, off2 in sub_entries(rsrc, sub):
            sub2 = off2 & 0x7FFFFFFF
            for _lang, off3 in sub_entries(rsrc, sub2):
                d = off3 & 0x7FFFFFFF
                data_rva, size, _cp, _r = struct.unpack_from('<IIII', rsrc, d)
                # IMAGE_RESOURCE_DATA_ENTRY.OffsetToData is an RVA.
                foff = rva_to_off(data, sec, nsec, data_rva)
                if foff is None:
                    continue
                blob = data[foff:foff + size]
                base = (blk_id - 1) * 16
                p = 0
                for s in range(16):
                    if p + 2 > len(blob):
                        break
                    ln = struct.unpack_from('<H', blob, p)[0]
                    p += 2
                    txt = blob[p:p + ln * 2].decode('utf-16-le', 'replace')
                    p += ln * 2
                    if txt:
                        out.append((base + s, txt))
    return out


def list_types(data):
    rsrc, _rsrc_rva, _raw, _base, _sec, _nsec = rsrc_root(data)
    names = {1: 'CURSOR', 2: 'BITMAP', 3: 'ICON', 4: 'MENU', 5: 'DIALOG',
             6: 'STRING', 7: 'FONTDIR', 8: 'FONT', 9: 'ACCELERATOR',
             10: 'RCDATA', 11: 'MESSAGETABLE', 12: 'GROUP_CURSOR',
             14: 'GROUP_ICON', 16: 'VERSION', 24: 'MANIFEST'}
    rows = []
    for name_id, off in sub_entries(rsrc, 0):
        sub = off & 0x7FFFFFFF
        n_named, n_id = struct.unpack_from('<HH', rsrc, sub + 12)
        label = ('name@0x%x' % (name_id & 0x7FFFFFFF)
                 if name_id & 0x80000000 else names.get(name_id, str(name_id)))
        rows.append((name_id & 0x7FFFFFFF if not name_id & 0x80000000 else -1,
                     label, n_named + n_id))
    return rows


def main():
    path = sys.argv[1]
    if '\\' not in path and not path.lower().startswith('c:'):
        path = GAME + path
    grep = None
    if '--grep' in sys.argv:
        grep = sys.argv[sys.argv.index('--grep') + 1].lower()
    data = open(path, 'rb').read()
    if '--list' in sys.argv:
        for tid, label, n2 in list_types(data):
            print('  type %-4s %-14s entries=%d' % (tid, label, n2))
        return
    if '--blob' in sys.argv:
        i = sys.argv.index('--blob')
        off = int(sys.argv[i + 1], 0)
        size = int(sys.argv[i + 2], 0)
        blob = data[off:off + size]
        p = 0
        for n in range(16):
            if p + 2 > len(blob):
                break
            ln = struct.unpack_from('<H', blob, p)[0]
            p += 2
            txt = blob[p:p + ln * 2].decode('utf-16-le', 'replace')
            p += ln * 2
            print('  [%2d] len=%-3d %r' % (n, ln, txt))
        return
    strs = find_string_tables(data)
    print('%s: %d resource strings' % (path, len(strs)))
    for idx, txt in strs:
        if grep and grep not in txt.lower():
            continue
        print('  %5d  %s' % (idx, txt))


if __name__ == '__main__':
    main()
