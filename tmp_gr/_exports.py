"""Scratch: list a module's exported symbols with their RVAs/VAs."""
import struct
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import pe_inspect  # noqa: E402

path = sys.argv[1]
pat = sys.argv[2].lower() if len(sys.argv) > 2 else ''
out = sys.argv[3] if len(sys.argv) > 3 else None
data = open(path, 'rb').read()
pe = pe_inspect.PE(data)
base = struct.unpack_from('<I', data, pe.opt + 28)[0]
rva = pe.dd[0][0]
lines = []
if rva:
    off = pe.rva2off(rva)
    nfun, nnames = struct.unpack_from('<II', data, off + 0x14)
    aof, aon, aoo = struct.unpack_from('<III', data, off + 0x1C)
    functions = [struct.unpack_from('<I', data, pe.rva2off(aof) + 4 * i)[0]
                 for i in range(nfun)]
    for i in range(nnames):
        nr = struct.unpack_from('<I', data, pe.rva2off(aon) + 4 * i)[0]
        idx = struct.unpack_from('<H', data, pe.rva2off(aoo) + 2 * i)[0]
        name = pe.cstr(pe.rva2off(nr))
        if pat in name.lower():
            lines.append('0x%08x  %s' % (base + functions[idx], name))
txt = '\n'.join(lines)
if out:
    open(out, 'w', encoding='utf-8').write(txt + '\n')
print('exports=%d matching=%d' % (nfun, len(lines)))

