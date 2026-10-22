"""Scratch: grep a cached /list_functions dump (UTF-8 or -16) for tokens."""
import sys

path = sys.argv[1]
tokens = [t.lower() for t in sys.argv[2:]]
data = open(path, 'rb').read()
for enc in ('utf-8-sig', 'utf-16', 'latin-1'):
    try:
        txt = data.decode(enc)
        if txt.count('\x00') == 0:
            break
    except UnicodeDecodeError:
        continue
lines = [l.rstrip() for l in txt.replace('\x00', '').splitlines() if l.strip()]
print('lines=%d' % len(lines))
seen = 0
for l in lines:
    ll = l.lower()
    if all(t in ll for t in tokens):
        print(l)
        seen += 1
print('hits=%d' % seen)
