"""Scratch: print the DFM object blocks whose header matches a regex."""
import re
import sys

src, pattern = sys.argv[1], sys.argv[2]
out = sys.argv[3]
data = open(src, 'rb').read()
txt = data.decode('utf-8-sig', 'replace')
if txt.count('\x00') > 10:
    txt = data.decode('utf-16', 'replace')
lines = txt.replace('\x00', '').splitlines()
res = []
grab = False
for l in lines:
    s = l.strip()
    if re.match(r'object\s', s):
        grab = bool(re.search(pattern, s))
    if grab:
        res.append(l.rstrip())
    if grab and s == 'end':
        res.append('')
        grab = False
open(out, 'w', encoding='utf-8').write('\n'.join(res) + '\n')
print('wrote', out, len(res), 'lines')
