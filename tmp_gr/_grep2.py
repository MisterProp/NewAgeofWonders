"""Scratch: grep a cached /list_functions dump for tokens (writes UTF-8)."""
import sys

path = sys.argv[1]
out = sys.argv[2]
tokens = [t.lower() for t in sys.argv[3:]]
data = open(path, 'rb').read()
txt = data.decode('utf-8-sig', 'replace')
if txt.count('\x00') > 10:
    txt = data.decode('utf-16', 'replace')
lines = [l.rstrip() for l in txt.replace('\x00', '').splitlines() if l.strip()]
hits = [l for l in lines if all(t in l.lower() for t in tokens)]
with open(out, 'w', encoding='utf-8') as f:
    f.write('lines=%d hits=%d\n' % (len(lines), len(hits)))
    f.write('\n'.join(hits))
print('lines=%d hits=%d -> %s' % (len(lines), len(hits), out))
