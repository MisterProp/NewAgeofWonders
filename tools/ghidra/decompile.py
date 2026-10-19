"""decompile.py - batch-decompile functions through GhidraMCP into UTF-8 files.

    python tools/ghidra/decompile.py /AoW.exe outdir FUN_00419264 FUN_00419488
    python tools/ghidra/decompile.py /AoW.exe outdir --list path/to/functions.txt

Each function is written to <outdir>/<program-stem>/<name>.c so we can read
them without fighting the console encoding.
"""
import os
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'

sys.stdout.reconfigure(encoding='utf-8', errors='replace')


def get(url_path, **params):
    url = BASE + url_path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=600) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    prog = sys.argv[1]
    outdir = sys.argv[2]
    names = sys.argv[3:]
    if names and names[0] == '--list':
        names = [l.strip() for l in open(names[1], encoding='utf-8')
                 if l.strip()]
    get('/open_program', path=prog)
    stem = os.path.basename(prog).replace('.', '_')
    dest = os.path.join(outdir, stem)
    os.makedirs(dest, exist_ok=True)
    for n in names:
        body = get('/decompile_function', address=n)
        if body.lstrip().startswith('{"error"'):
            print('%-22s %s' % (n, body.strip()[:80]))
            continue
        with open(os.path.join(dest, n + '.c'), 'w', encoding='utf-8') as f:
            f.write('// %s : %s\n' % (prog, n))
            f.write(body)
        print('%-22s %d bytes' % (n, len(body)))


if __name__ == '__main__':
    main()
