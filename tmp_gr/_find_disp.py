"""Scratch: find code that references a 32-bit displacement (e.g. 0x118)."""
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import pe_va  # noqa: E402

path, disp, out = sys.argv[1], int(sys.argv[2], 0), sys.argv[3]
data, base, secs = pe_va.sections(path)
code = [s for s in secs if s[0] == 'CODE'][0]
_name, vaddr, vsize, raw, rsize = code
lo, hi = raw, raw + rsize
pat = disp.to_bytes(4, 'little')
lines = []
i = lo
while True:
    j = data.find(pat, i, hi)
    if j < 0:
        break
    i = j + 1
    va, _n = pe_va.foff_to_va(path, j)
    ctx = data[j - 6:j + 8]
    lines.append('va 0x%08x  before=%s  after=%s'
                 % (va, ctx[:6].hex(' '), ctx[6:].hex(' ')))
open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('wrote', out, len(lines))
