"""Scratch: 2x contact sheet of the first N TBTurnBt frames with labels."""
import os
import sys

from PIL import Image, ImageDraw

d = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'turnbt')
n = int(sys.argv[1]) if len(sys.argv) > 1 else 30
cols = 8
cell_w, cell_h = 92, 92
rows = (n + cols - 1) // cols
sheet = Image.new('RGB', (cols * cell_w, rows * cell_h), (30, 30, 30))
draw = ImageDraw.Draw(sheet)
for i in range(n):
    f = os.path.join(d, 'f%03d.png' % i)
    if not os.path.exists(f):
        continue
    im = Image.open(f).convert('RGB').resize((82, 76), Image.NEAREST)
    x = (i % cols) * cell_w + 5
    y = (i // cols) * cell_h + 14
    sheet.paste(im, (x, y))
    draw.text((x, (i // cols) * cell_h + 1), str(i), fill=(255, 255, 0))
sheet.save(os.path.join(d, 'sheet_first%d.png' % n))
print('wrote sheet_first%d.png' % n)
