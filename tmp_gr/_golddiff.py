"""Scratch: write the pixel diff of a git-HEAD PNG against the working copy."""
import subprocess
from io import BytesIO

from PIL import Image, ImageChops

port = r'C:\GOG Games\Age of Wonders\port'
for name in ('Tutorial_L0', 'Dwarf1_L0'):
    rel = 're/golden/%s.png' % name
    head = subprocess.run(['git', 'show', 'HEAD:' + rel], cwd=port,
                          capture_output=True, check=True).stdout
    a = Image.open(BytesIO(head)).convert('RGB')
    b = Image.open(port + '/' + rel).convert('RGB')
    d = ImageChops.difference(a, b)
    box = d.getbbox()
    print(name, box)
    if box:
        x0, y0, x1, y1 = box
        # side-by-side of the changed region (head | work | diff x8)
        ch = a.crop(box)
        cw = b.crop(box)
        dd = d.crop(box)
        w = x1 - x0
        h = y1 - y0
        sheet = Image.new('RGB', (w * 3 + 20, h), (20, 20, 20))
        sheet.paste(ch, (0, 0))
        sheet.paste(cw, (w + 10, 0))
        sheet.paste(dd, (2 * w + 20, 0))
        sheet.save(port + '/tmp_gr/_gold_%s_diff.png' % name)
        print('  wrote tmp_gr/_gold_%s_diff.png %dx%d' % (name, w, h))
