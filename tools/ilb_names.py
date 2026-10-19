"""ilb_names.py - print the entry-name list of one (or all) ILB(s) from the
inventory produced by tools/ilb_tool.py.

    python tools/ilb_names.py "Int/WinTIcon.ilb"
    python tools/ilb_names.py --grep Title
"""
import json
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

PATH = r'c:\GOG Games\Age of Wonders\port\re\ilb_inventory.json'


def main():
    inv = json.load(open(PATH, encoding='utf-8'))
    args = sys.argv[1:]
    if args and args[0] == '--grep':
        rx = args[1].lower()
        for k in sorted(inv):
            if rx in k.lower():
                print('==', k)
    elif args:
        for key in args:
            rec = inv.get(key)
            if rec is None:
                cand = [k for k in inv if key.lower() in k.lower()]
                print('no exact match; candidates:', cand[:20])
                continue
            names = rec.get('names', [])
            print('== %s  (header.count=%s, listed=%d)'
                  % (key, rec['header'].get('count'), len(names)))
            for i, n in enumerate(names):
                print('%4d  %s' % (i, n))
    else:
        print('\n'.join(sorted(inv)))


if __name__ == '__main__':
    main()
