"""Scratch: pairwise view + pixel diff of the NextButton up/down art."""
import os

from PIL import Image, ImageChops, ImageDraw

d = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'turnbt')
a = Image.open(os.path.join(d, 'next_up.png')).convert('RGB')
b = Image.open(os.path.join(d, 'next_down.png')).convert('RGB')
print('up', a.size, 'down', b.size)
diff = ImageChops.difference(a, b)
bbox = diff.getbbox()
print('diff bbox', bbox)
sheet = Image.new('RGB', (3 * 90, 130), (30, 30, 30))
for i, im in enumerate((a, b)):
    sheet.paste(im.resize((78, 114), Image.NEAREST), (i * 90 + 6, 8))
draw = ImageDraw.Draw(sheet)
draw.text((6, 0), 'up 3', fill=(255, 255, 0))
draw.text((96, 0), 'down 4', fill=(255, 255, 0))
sheet.save(os.path.join(d, 'next_pair.png'))
print('wrote next_pair.png')
