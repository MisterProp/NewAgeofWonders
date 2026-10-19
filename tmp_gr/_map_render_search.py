"""Scratch: query the running GhidraMCP server for every function name in each
open .dpl and cache the raw results under tmp_gr/.
"""
import json
import os
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
OUT = r'C:\GOG Games\Age of Wonders\port\tmp_gr'

PROGRAMS = ['AoWEPACK.dpl', 'Enginep.dpl', 'HSEPack.dpl',
            'Gfxepack.dpl', 'Ilpack.dpl', 'Localize.dpl']


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=120) as r:
        return r.read().decode('utf-8', 'replace')


for prog in PROGRAMS:
    txt = get('/list_functions', program=prog)
    dest = os.path.join(OUT, '_funcs_' + prog.replace('.dpl', '') + '.txt')
    with open(dest, 'w', encoding='utf-8') as f:
        f.write(txt)
    print('%-16s -> %7d bytes' % (prog, len(txt)))
