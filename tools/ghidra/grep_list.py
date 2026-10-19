"""grep_list.py - grep a GhidraMCP symbol dump (UTF-16 or UTF-8).

PowerShell `>` writes UTF-16LE, so plain Select-String output looks like it is
full of NULs. This reads either encoding, filters, and prints unique prefixes.

    python tools/ghidra/grep_list.py <file> <regex> [--units | --head N]
    python tools/ghidra/grep_list.py re/exports/aowinterface_list_functions.txt "Menu|Orb"
"""
import re
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')


def read(path):
    raw = open(path, 'rb').read()
    if raw[:2] in (b'\xff\xfe', b'\xfe\xff') or raw[1:2] == b'\x00':
        return raw.decode('utf-16', 'replace')
    return raw.decode('utf-8', 'replace')


def main():
    path = sys.argv[1]
    rx = re.compile(sys.argv[2], re.I)
    opts = sys.argv[3:]
    lines = [l.rstrip('\r\n') for l in read(path).splitlines() if l.strip()]
    hits = [l for l in lines if rx.search(l)]
    if '--units' in opts:
        units = sorted({l.split('.')[0].split(' ')[0] for l in hits})
        for u in units:
            print(u)
        return
    if '--all' in opts:
        hits = lines
    for h in hits:
        print(h)
    print('--- %d/%d matched' % (len(hits), len(lines)))


if __name__ == '__main__':
    main()
