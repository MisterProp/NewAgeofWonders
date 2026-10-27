"""city_prod_bg.py - build the city production/change screen's backdrops.

`TCityScreen` has two modes. `CityBtn1Click` (AoW.exe `0x0043d37c`) mode 0
switches the form to its production picker (mode 1): `FUN_0043bb14` rebuilds
the 12 `P1CB..P12CB` production cells, `FUN_0043c414` shows `UpgradePnl`,
`InfoPnl` and `ActivityPnl2`, and the six `B1CB..B6CB` activity cells call
`ActivityChange` -> `FUN_0043c43c` to fill the title / memo / cost / turns /
relation controls.  The six reference captures in `bin/cityscreen/` are the
six activity pages (`B1CB` Produce Merchandise .. `B6CB` Raze).

The original renders that page from `CityWin.ILB` plus a runtime-generated
city map, so - exactly like the overview backdrop in `city_screen_bg.py` - this
tool turns the six captures into the 640x480 24-bit BMPs the port blits.  The
captures are 1:1 but cropped (and a few pixels offset) relative to the 640x480
frame; each one is registered onto the reference 01 frame with the shift found
by the bottom bar and the missing strips are filled from 01.  The renderer
clips the pages at the 432 px dialog edge, so the captured bar band is only
used for registration and the live overworld bar is drawn below the dialog.

    python tools/city_prod_bg.py bin/cityscreen assets
"""
import os
import struct
import sys

from PIL import Image

# (file, screen origin of the capture's (0,0)) - measured by matching the
# static bottom bar against 01; each capture is a crop of the same 640x480
# frame, shifted by a few pixels on both axes.
CAPTURES = [
    ("01", 0, 0),
    ("02", 3, 1),
    ("03", 1, 0),
    ("04", 2, -1),
    ("05", 3, 1),
    ("06", 2, 3),
]


def write_bmp24(path, im):
    im = im.convert('RGB')
    w, h = im.size
    row = (w * 3 + 3) & ~3
    data = bytearray()
    for y in range(h - 1, -1, -1):          # bottom-up rows
        line = bytearray()
        for x in range(w):
            r, g, b = im.getpixel((x, y))
            line += bytes((b, g, r))
        line += b'\0' * (row - len(line))
        data += line
    size = 54 + len(data)
    hdr = struct.pack('<2sIHHI', b'BM', size, 0, 0, 54)
    info = struct.pack('<IiiHHIIiiII', 40, w, h, 1, 24, 0, len(data),
                       2835, 2835, 0, 0)
    with open(path, 'wb') as f:
        f.write(hdr + info + data)


def pad_to_frame(im):
    frame = Image.new('RGB', (640, 480))
    frame.paste(im.crop((0, 0, min(im.width, 640), min(im.height, 480))),
                (0, 0))
    # The captures stop 2..9 px short of the right edge and 3..9 px short of
    # the bottom; repeat the captured edge so the frame is complete.
    last_col = frame.crop((min(im.width, 640) - 1, 0,
                           min(im.width, 640), min(im.height, 480)))
    for x in range(min(im.width, 640), 640):
        frame.paste(last_col, (x, 0))
    last_row = frame.crop((0, min(im.height, 480) - 1, 640,
                           min(im.height, 480)))
    for y in range(min(im.height, 480), 480):
        frame.paste(last_row, (0, y))
    return frame


# QueueLB: the TAOWListBox at (11,358) 157x63 (DFM WinLeft/WinTop/WinWidth/
# WinHeight, BorderWidth 3 / BorderHeight 1). Its five `<empty>` rows are part
# of every capture; the port needs the same box without that text when it has
# real queue rows to draw. The text is the exact Age8 ink colour qr,qg,qb =
# (115,132,99) in the capture, so it can be removed by a small neighbourhood
# fill; the row text drawn afterwards covers the repaired pixels.
QUEUE_BOX = (11, 358, 168, 421)
QUEUE_TEXT = (115, 132, 99)


def clean_queue(im):
    crop = im.crop(QUEUE_BOX).convert('RGB')
    w, h = crop.size
    px = crop.load()
    mask = [[False] * w for _ in range(h)]
    for y in range(h):
        for x in range(w):
            r, g, b = px[x, y]
            if (abs(r - QUEUE_TEXT[0]) < 40 and
                    abs(g - QUEUE_TEXT[1]) < 40 and
                    abs(b - QUEUE_TEXT[2]) < 40):
                mask[y][x] = True
    # Iterative nearest-neighbour fill from the (very dark, mottled) stone.
    for _ in range(12):
        changed = False
        for y in range(h):
            for x in range(w):
                if not mask[y][x]:
                    continue
                best = None
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        if dx == 0 and dy == 0:
                            continue
                        nx, ny = x + dx, y + dy
                        if 0 <= nx < w and 0 <= ny < h and not mask[ny][nx]:
                            d = dx * dx + dy * dy
                            if best is None or d < best[0]:
                                best = (d, px[nx, ny])
                if best is not None:
                    px[x, y] = best[1]
                    mask[y][x] = False
                    changed = True
        if not changed:
            break
    return crop


def main():
    src_dir = sys.argv[1] if len(sys.argv) > 1 else 'bin/cityscreen'
    dst_dir = sys.argv[2] if len(sys.argv) > 2 else 'assets'
    base = pad_to_frame(Image.open(os.path.join(src_dir, '01.png')))
    for idx, (name, ox, oy) in enumerate(CAPTURES, 1):
        ref = Image.open(os.path.join(src_dir, name + '.png')).convert('RGB')
        frame = base.copy()
        # Paste the capture at its registered origin; clipping handles the
        # negative `oy` of capture 04 (it starts one row above the frame).
        src_box = (max(0, -ox), max(0, -oy),
                   min(ref.width, 640 - ox), min(ref.height, 480 - oy))
        dst = (max(0, ox), max(0, oy))
        if src_box[2] > src_box[0] and src_box[3] > src_box[1]:
            frame.paste(ref.crop(src_box), dst)
        out = os.path.join(dst_dir, 'cityprod%d.bmp' % idx)
        write_bmp24(out, frame)
        print('wrote %s  (%s at %+d,%+d, %dx%d)' %
              (out, name, ox, oy, ref.width, ref.height))
    # The clean QueueLB box (for live queue rows).
    base_clean = clean_queue(pad_to_frame(
        Image.open(os.path.join(src_dir, '01.png'))))
    qout = os.path.join(dst_dir, 'cityqueue.bmp')
    write_bmp24(qout, base_clean)
    print('wrote %s  (QueueLB 157x63 without the <empty> rows)' % qout)


if __name__ == '__main__':
    main()
