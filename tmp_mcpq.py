"""tmp_mcpq.py - query the GhidraMCP bridge (127.0.0.1:8089).

Usage:
  python tmp_mcpq.py search <program> <term>
  python tmp_mcpq.py decomp <program> <name-or-address> [outfile]
  python tmp_mcpq.py getcurrent
"""
import sys
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'


def get(endpoint, **params):
    url = BASE + endpoint
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=600) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    cmd = sys.argv[1]
    if cmd == 'getcurrent':
        print(get('/get_current_function'))
        return
    prog = sys.argv[2]
    body = get('/open_program', path=prog)
    if cmd == 'search':
        term = sys.argv[3]
        out = get('/search_functions', searchTerm=term)
    elif cmd == 'decomp':
        name = sys.argv[3]
        out = get('/decompile_function', address=name)
        if len(sys.argv) > 4:
            open(sys.argv[4], 'w', encoding='utf-8').write(out)
            print('wrote %s (%d bytes)' % (sys.argv[4], len(out)))
            return
    else:
        raise SystemExit('unknown command')
    print(out)


if __name__ == '__main__':
    main()
