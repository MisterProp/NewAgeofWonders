"""filter_imports.py - print the GhidraMCP /list_imports entries whose IAT slot
matches one of the given addresses.

    python tools/ghidra/mcp.py re/_imports.json /list_imports
    python tools/filter_imports.py re/_imports.json 0x45df70
"""
import json
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')


def load(path):
    raw = open(path, 'rb').read()
    for enc in ('utf-8-sig', 'utf-16', 'latin-1'):
        try:
            return json.loads(raw.decode(enc))
        except (UnicodeDecodeError, json.JSONDecodeError):
            continue
    raise SystemExit('cannot decode ' + path)


def main():
    data = load(sys.argv[1])
    want = {int(a, 0) for a in sys.argv[2:]}
    for e in data:
        try:
            addr = int(e['address'], 0)
        except (KeyError, ValueError, TypeError):
            addr = None
        if addr is None or addr not in want:
            continue
        print('0x%08x  %s  (orig %s)'
              % (addr, e.get('name', '?'), e.get('original_imported_name', '')))


if __name__ == '__main__':
    main()
