"""dump_all.py - pull /list_functions for every program open in GhidraMCP.

The project has the whole game loaded (AoW.exe + all .dpl packages), and
/open_program switches the "current" one. We iterate, dump each list to
re/exports/<name>.funcs.txt, and reset to AoWEPACK at the end.

    python tools/ghidra/dump_all.py [outdir]
"""
import json
import os
import sys
import time
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
DEFAULT_OUT = r'c:\GOG Games\Age of Wonders\port\re\exports'

sys.stdout.reconfigure(encoding='utf-8', errors='replace')


def get(url_path, **params):
    url = BASE + url_path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=600) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    outdir = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_OUT
    info = json.loads(get('/list_open_programs'))
    progs = info['programs']
    print('%d programs open' % len(progs))
    for p in progs:
        name = p['name']
        dest = os.path.join(outdir, name + '.funcs.txt')
        if os.path.exists(dest) and os.path.getsize(dest) > 1000:
            print('%-20s cached (%d funcs)' % (name, p.get('function_count', 0)))
            continue
        get('/open_program', path=p['path'])
        body = get('/list_functions')
        with open(dest, 'w', encoding='utf-8') as f:
            f.write(body)
        print('%-20s %d bytes' % (name, len(body)))
        time.sleep(0.2)
    # leave the project on AoWEPACK (the game module) as we found it
    get('/open_program', path='/AoWEPACK.dpl')


if __name__ == '__main__':
    main()
