"""Scratch: cache /list_functions for a program and print matches."""
import os
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
OUT = os.path.dirname(os.path.abspath(__file__))


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=600) as r:
        return r.read().decode('utf-8', 'replace')


prog = sys.argv[1]
filters = [f.lower() for f in sys.argv[2:] if not f.startswith('--')]
dest = os.path.join(OUT, '_funcs_' + prog.replace('.', '_') + '.txt')
if os.path.exists(dest) and '--fresh' not in sys.argv:
    txt = open(dest, encoding='utf-8').read()
else:
    txt = get('/list_functions', program=prog, offset=0, limit=100000)
    open(dest, 'w', encoding='utf-8').write(txt)
lines = txt.splitlines()
print('cached %d lines in %s' % (len(lines), os.path.basename(dest)))
if filters:
    for l in lines:
        ll = l.lower()
        if all(f in ll for f in filters):
            print(l)
