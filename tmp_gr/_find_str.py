"""Scratch: print ASCII strings containing a token with file offset + VA."""
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import pe_va  # noqa: E402

path, token, out = sys.argv[1], sys.argv[2], sys.argv[3]
data, _b, _s = pe_va.sections(path)
needle = token.encode('ascii')
lines = []
pos = 0
while True:
    i = data.find(needle, pos)
    if i < 0:
        break
    pos = i + 1
    s = i
    while s > 0 and 32 <= data[s - 1] < 127:
        s -= 1
    e = i + len(needle)
    while e < len(data) and 32 <= data[e] < 127:
        e += 1
    va, sec = pe_va.foff_to_va(path, s)
    lines.append('file 0x%06x va 0x%08x (%s)  %s'
                 % (s, va or 0, sec, data[s:e].decode('latin1')))
open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('wrote', out, len(lines))
