"""Scratch: crop + upscale a region of a rendered PNG for inspection."""
import sys

from PIL import Image

src, out = sys.argv[1], sys.argv[2]
x, y, w, h = (int(v) for v in sys.argv[3:7])
scale = int(sys.argv[7]) if len(sys.argv) > 7 else 3
im = Image.open(src).convert('RGB').crop((x, y, x + w, y + h))
im = im.resize((w * scale, h * scale), Image.NEAREST)
im.save(out)
print('wrote', out, im.size)
