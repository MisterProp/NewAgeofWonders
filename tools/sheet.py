"""sheet.py - decode a list of ILB entries to a single labelled contact sheet.

    python tools/sheet.py out.png "Int/GenericF.ILB" 32 33 72 73 113 114 ...

Runs bin/aow_tools.exe png for each id (so it uses the port's own ILB decoder) and
composes the sprites on a dark background with their id printed above.
"""
import os
import subprocess
import sys

from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
AOW = os.path.join(PORT, 'bin', 'aow_tools.exe')
TMP = os.path.join(PORT, 'bin', '_sheet')


def main():
    out = sys.argv[1]
    ilb = sys.argv[2]
    ids = sys.argv[3:]
    os.makedirs(TMP, exist_ok=True)
    imgs = []
    for i in ids:
        p = os.path.join(TMP, 'e%s.png' % i)
        subprocess.run([AOW, 'png', os.path.join(PORT, '..', ilb), i, p, '--id'],
                       check=False, capture_output=True)
        if os.path.exists(p):
            im = Image.open(p).convert('RGBA')
            imgs.append((i, im))
    pad = 8
    lab = 12
    W = sum(im.width + pad for _, im in imgs) + pad
    H = max((im.height for _, im in imgs), default=1) + lab + pad * 2
    sheet = Image.new('RGBA', (max(W, 1), max(H, 1)), (30, 30, 40, 255))
    d = ImageDraw.Draw(sheet)
    x = pad
    for i, im in imgs:
        sheet.alpha_composite(im, (x, lab + pad))
        d.text((x, 2), i, fill=(255, 220, 120, 255))
        x += im.width + pad
    sheet.save(out)
    print('wrote %s  %dx%d  %d sprites' % (out, sheet.width, sheet.height,
                                           len(imgs)))


main()
