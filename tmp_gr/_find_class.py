"""Scratch: find which module(s) mention a Delphi class/unit name."""
import glob
import os
import sys

root = r'C:\GOG Games\Age of Wonders'
needle = sys.argv[1].encode('ascii')
for path in sorted(glob.glob(os.path.join(root, '*.dpl')) +
                   glob.glob(os.path.join(root, '*.exe')) +
                   glob.glob(os.path.join(root, '*.dll'))):
    data = open(path, 'rb').read()
    hits = data.count(needle)
    if hits:
        off = data.find(needle)
        print('%-24s %d hits (first at 0x%06x) size=%d'
              % (os.path.basename(path), hits, off, len(data)))
