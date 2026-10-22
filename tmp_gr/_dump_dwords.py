"""Scratch: dump dwords at a VA range and try to resolve them to function names.

    python tmp_gr/_dump_dwords.py <pe> <va> <count> <funcs.txt> <out.txt>
"""
import struct
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import pe_va  # noqa: E402

path, va, count, funcs, out = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3]), \
    sys.argv[4], sys.argv[5]
data, _b, _s = pe_va.sections(path)
names = {}
data_f = open(funcs, 'rb').read()
txt = data_f.decode('utf-8-sig', 'replace')
if txt.count('\x00') > 10:
    txt = data_f.decode('utf-16', 'replace')
for line in txt.replace('\x00', '').splitlines():
    if ' at ' in line:
        nm, ad = line.rsplit(' at ', 1)
        try:
            names[int(ad.strip(), 16)] = nm.strip()
        except ValueError:
            pass
lines = []
for i in range(count):
    a = va + i * 4
    off, _nm = pe_va.va_to_foff(path, a)
    if off is None:
        break
    v = struct.unpack_from('<I', data, off)[0]
    lines.append('0x%08x  +0x%02x  0x%08x  %s'
                 % (a, i * 4, v, names.get(v, '')))
open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('wrote', out)
