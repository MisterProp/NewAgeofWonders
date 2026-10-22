"""Scratch: which binaries mention a Delphi class/unit name (UTF-8 report)."""
import glob
import os
import sys

root = r'C:\GOG Games\Age of Wonders'
out = sys.argv[1]
needles = sys.argv[2:]
files = []
for pat in ('*.dpl', '*.exe', '*.dll', '*.orig'):
    files += glob.glob(os.path.join(root, pat))
lines = []
for needle in needles:
    nb = needle.encode('ascii')
    lines.append('=== %s' % needle)
    for path in sorted(files):
        data = open(path, 'rb').read()
        hits = data.count(nb)
        if hits:
            lines.append('  %-20s %3d hits  size=%d'
                         % (os.path.basename(path), hits, len(data)))
open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('wrote', out)
