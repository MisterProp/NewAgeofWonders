"""Scratch: contact sheet of the TBTurnBt frames (+ the NextButton art)."""
import glob
import os

from PIL import Image

d = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'turnbt')
files = sorted(glob.glob(os.path.join(d, 'f*.png')))
cell_w, cell_h = 45, 42
cols = 12
rows = (len(files) + cols - 1) // cols
sheet = Image.new('RGB', (cols * cell_w, rows * cell_h), (40, 40, 40))
for i, f in enumerate(files):
    im = Image.open(f).convert('RGB')
    x = (i % cols) * cell_w + 2
    y = (i // cols) * cell_h + 2
    sheet.paste(im, (x, y))
sheet.save(os.path.join(d, 'sheet.png'))
print('wrote sheet (%d frames)' % len(files))
