"""Scratch: locate a Delphi class-name string and its VMT (class name pointer).

    python tmp_gr/_vmt_find.py <exe> <ClassName> <out.txt>

Delphi stores a class name as a short string; the VMT holds a pointer to it at
vmtClassName (offset -0x2C..-0x30 from vmtSelfPtr). We find every occurrence of
the string, then every dword in the image equal to its VA (and VA-1 for the
short-string length byte), and dump the 48 dwords around each hit so the VMT
(method pointers into CODE) is visible.
"""
import struct
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import pe_va  # noqa: E402


def main(argv):
    path, cls, out = argv[1], argv[2], argv[3]
    data, base, secs = pe_va.sections(path)
    lines = ['%s  class=%s base=0x%08x' % (path, cls, base)]
    nb = cls.encode('ascii')
    pos = 0
    names = []
    while True:
        i = data.find(nb, pos)
        if i < 0:
            break
        va, sec = pe_va.foff_to_va(path, i)
        names.append(va)
        lines.append('name at file 0x%06x va 0x%08x (%s)' % (i, va or 0, sec))
        pos = i + 1
    for name_va in names:
        for target in (name_va, name_va - 1):
            pat = struct.pack('<I', target)
            p = 0
            while True:
                j = data.find(pat, p)
                if j < 0:
                    break
                p = j + 1
                va, sec = pe_va.foff_to_va(path, j)
                lines.append('  ptr to 0x%08x at file 0x%06x va 0x%08x (%s)'
                             % (target, j, va or 0, sec))
                lo = max(0, j - 0x40)
                for k in range(lo, min(len(data), j + 0x40), 4):
                    v = struct.unpack_from('<I', data, k)[0]
                    mark = ' <==' if k == j else ''
                    lines.append('      +%+04x  0x%08x%s' % (k - j, v, mark))
    open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('wrote', out, len(lines), 'lines')


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
