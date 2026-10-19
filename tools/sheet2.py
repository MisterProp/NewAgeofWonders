"""sheet2.py - zoomed, labelled contact sheet from several ILBs.

    python tools/sheet2.py out.png 3 "Int/GenericE.ILB:3,4,5,6,7,8,9,10,11,15,16,17" "Int/Icons.ILB:6,8,10"

Each argument after the scale is  "<ilb path relative to game root>:<id,id,...>".
Sprites are drawn at the given integer scale on a mid-grey background so alpha
edges are visible.  Uses the port's own decoder (bin/aow_tools.exe png --id).
"""
import os
import subprocess
import sys

from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
AOW = os.path.join(PORT, 'bin', 'aow_tools.exe')
TMP = os.path.join(PORT, 'bin', '_sheet2')


def main():
    out = sys.argv[1]
    scale = int(sys.argv[2])
    specs = sys.argv[3:]
    os.makedirs(TMP, exist_ok=True)
    cells = []          # (label, image)
    for spec in specs:
        ilb, ids = spec.rsplit(':', 1)
        for i in ids.split(','):
            i = i.strip()
            if not i:
                continue
            p = os.path.join(TMP, 'e%s_%s.png' % (os.path.basename(ilb), i))
            # positional index (matches the ILB directory order, which is what
            # the ILB listings print), not the stored entry id.
            subprocess.run([AOW, 'png', os.path.join(PORT, '..', ilb), i, p],
                           check=False, capture_output=True)
            if os.path.exists(p):
                im = Image.open(p).convert('RGBA')
                im = im.resize((max(1, im.width * scale),
                                max(1, im.height * scale)), Image.NEAREST)
                cells.append(('%s#%s' % (os.path.basename(ilb).split('.')[0], i),
                              im))
    pad = 10
    lab = 14
    # pack into rows of at most 1400 px
    rows = []
    cur = []
    curw = pad
    for label, im in cells:
        if curw + im.width + pad > 1400 and cur:
            rows.append(cur)
            cur, curw = [], pad
        cur.append((label, im))
        curw += im.width + pad
    if cur:
        rows.append(cur)
    W = max((sum(im.width + pad for _, im in r) + pad for r in rows),
            default=1)
    H = sum(max(im.height for _, im in r) + lab + pad for r in rows) + pad
    sheet = Image.new('RGBA', (W, H), (70, 70, 78, 255))
    d = ImageDraw.Draw(sheet)
    y = pad
    for r in rows:
        x = pad
        rh = max(im.height for _, im in r)
        for label, im in r:
            sheet.alpha_composite(im, (x, y + lab))
            d.text((x, y), label, fill=(255, 230, 120, 255))
            x += im.width + pad
        y += rh + lab + pad
    sheet.convert('RGB').save(out)
    print('wrote %s  %dx%d  %d sprites' % (out, sheet.width, sheet.height,
                                           len(cells)))


main()
