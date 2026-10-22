"""Scratch: list the changed-pixel clusters between HEAD's golden and the work."""
import subprocess
from collections import Counter
from io import BytesIO

from PIL import Image, ImageChops

port = r'C:\GOG Games\Age of Wonders\port'
rel = 're/golden/Tutorial_L0.png'
head = subprocess.run(['git', 'show', 'HEAD:' + rel], cwd=port,
                      capture_output=True, check=True).stdout
a = Image.open(BytesIO(head)).convert('RGB')
b = Image.open(port + '/' + rel).convert('RGB')
d = ImageChops.difference(a, b)
px = d.load()
w, h = d.size
cells = Counter()
for y in range(h):
    for x in range(w):
        if px[x, y] != (0, 0, 0):
            cells[(x // 32, y // 32)] += 1
for (cx, cy), n in cells.most_common(12):
    print('cell (%d,%d) at px (%d,%d): %d pixels' % (cx, cy, cx * 32, cy * 32, n))
