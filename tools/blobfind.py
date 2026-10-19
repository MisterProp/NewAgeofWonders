"""blobfind.py - locate byte patterns (ASCII/UTF-16 strings) inside the game
binary/data files without a disassembler, for cross-referencing with GhidraMCP.

    python tools/blobfind.py "TitleSceneIL" "OrbFrame" "TITLE.IT"
    python tools/blobfind.py --dir <folder> --max 3 --exts .exe,.dpl "New Game"

Output is grouped per needle so a common string cannot drown out the others.
"""
import os
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

DEFAULT_DIR = r'C:\GOG Games\Age of Wonders'


def iter_files(where, exts):
    if os.path.isfile(where):
        yield where
        return
    for root, dirs, names in os.walk(where):
        dirs[:] = [d for d in dirs if d.lower() not in ('port', 'save', 'emailin',
                                                         'emailout', 'backup')]
        for n in names:
            if n.lower().endswith(exts):
                yield os.path.join(root, n)


def main():
    args = sys.argv[1:]
    where = DEFAULT_DIR
    exts = ('.exe', '.dpl', '.dll')
    cap = 6
    needles = []
    i = 0
    while i < len(args):
        a = args[i]
        if a == '--dir':
            where = args[i + 1]
            i += 2
            continue
        if a == '--exts':
            exts = tuple(e if e.startswith('.') else '.' + e
                         for e in args[i + 1].split(','))
            i += 2
            continue
        if a == '--max':
            cap = int(args[i + 1])
            i += 2
            continue
        if a == '--ctx':
            ctx = int(args[i + 1])
            i += 2
            continue
        needles.append(a)
        i += 1

    files = list(iter_files(where, exts))
    for needle in needles:
        pats = [('ascii', needle.encode('ascii')),
                ('utf16', needle.encode('utf-16-le'))]
        print('== %s' % needle)
        total = 0
        for f in files:
            try:
                data = open(f, 'rb').read()
            except OSError:
                continue
            for label, pat in pats:
                if not pat:
                    continue
                start = 0
                shown = 0
                while shown < cap:
                    j = data.find(pat, start)
                    if j < 0:
                        break
                    print('   0x%08x  %-5s  %s' % (j, label, os.path.basename(f)))
                    if ctx:
                        lo = max(0, j - ctx)
                        hi = min(len(data), j + ctx)
                        chunk = data[lo:hi]
                        txt = ''.join(chr(c) if 32 <= c < 127 else '.'
                                      for c in chunk)
                        print('        ...%s...' % txt)
                    start = j + 1
                    shown += 1
                    total += 1
        if total == 0:
            print('   (none)')


if __name__ == '__main__':
    main()

