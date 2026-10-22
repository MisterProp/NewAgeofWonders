"""Scratch: case-insensitive search for a token across the game binaries."""
import glob
import os
import sys

root = r'C:\GOG Games\Age of Wonders'
out = sys.argv[1]
needles = [n.lower() for n in sys.argv[2:]]
files = []
for pat in ('*.dpl', '*.exe', '*.dll', '*.orig'):
    files += glob.glob(os.path.join(root, pat))
lines = []
for needle in needles:
    nb = needle.encode('ascii')
    lines.append('=== %s' % needle)
    for path in sorted(files):
        data = open(path, 'rb').read().lower()
        hits = data.count(nb)
        if hits:
            lines.append('  %-20s %3d hits' % (os.path.basename(path), hits))
open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('wrote', out)
