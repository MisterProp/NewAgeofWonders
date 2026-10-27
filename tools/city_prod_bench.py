"""city_prod_bench.py - compare the port's production pages with the captures.

The six references in `bin/cityscreen/` are 1:1 captures of TCityScreen mode 1
(the B1CB..B6CB activity pages), each cropped by a few pixels and registered on
the 640x480 frame with the offsets `tools/city_prod_bg.py` uses.  This bench
crops the port's 640x480 render at the same offsets, so a correct page is a
pixel-for-pixel match.  Only the 432 px dialog is compared: below it the port
draws the live world-map bar, not the capture's frozen one.

    python tools/city_prod_bench.py bin/city_prod_1.png
    python tools/city_prod_bench.py bin/city_prod_1.png bin/cityscreen
"""
import os
import sys

from PIL import Image, ImageChops

# (capture, origin x, origin y) - the same table as tools/city_prod_bg.py.
CAPTURES = [
    ("01", 0, 0),
    ("02", 3, 1),
    ("03", 1, 0),
    ("04", 2, -1),
    ("05", 3, 1),
    ("06", 2, 3),
]

# TCityScreen.Dlg is 432 px tall; rows below it are the live world-map bar,
# not the captured dialog, so they are excluded from the comparison.
DLG_H = 432


def compare(name, ox, oy, cur, ref_dir, sheet_rows):
    """Compare one 640x480 render with its capture; return mean |d|."""
    ref = Image.open(os.path.join(ref_dir, name + '.png')).convert('RGB')
    w, h = ref.size
    # A negative origin means the capture starts outside the 640x480 frame;
    # skip those leading rows/columns of the reference and compare the part
    # that is on screen.
    x0 = max(0, ox)
    y0 = max(0, oy)
    ref = ref.crop((max(0, -ox), max(0, -oy), w, h))
    # The dialog is 432 px tall; the bar band below it is drawn live by the
    # port, so exclude it from the captured-art comparison.
    ref = ref.crop((0, 0, min(ref.width, 640 - x0), min(ref.height,
                                                       DLG_H - y0)))
    crop = cur.crop((x0, y0, x0 + ref.width, y0 + ref.height))
    if crop.size != ref.size:
        raise SystemExit('%s size mismatch: ref %s crop %s' %
                         (name, ref.size, crop.size))
    diff = ImageChops.difference(ref, crop)
    px = diff.load()
    total = 0
    changed = 0
    maxd = 0
    for y in range(ref.height):
        for x in range(ref.width):
            p = px[x, y]
            e = p[0] + p[1] + p[2]
            total += e
            if e:
                changed += 1
            if e > maxd:
                maxd = e
    mean = total / (ref.width * ref.height * 3.0)
    print('%s %dx%d mean|d| = %.3f/255  changed %d/%d  max %d'
          % (name, ref.width, ref.height, mean, changed,
             ref.width * ref.height, maxd))
    sheet_rows.append((name, ref, crop, diff))
    return mean


def main():
    all_mode = len(sys.argv) > 1 and sys.argv[1] == '--all'
    if all_mode:
        bin_dir = sys.argv[2] if len(sys.argv) > 2 else 'bin'
        ref_dir = sys.argv[3] if len(sys.argv) > 3 else 'bin/cityscreen'
        paths = [os.path.join(bin_dir, 'city_prod_%d.png' % (i + 1))
                 for i in range(6)]
    else:
        paths = [sys.argv[1] if len(sys.argv) > 1 else 'bin/city_prod_1.png']
        ref_dir = sys.argv[2] if len(sys.argv) > 2 else 'bin/cityscreen'
    sheet_rows = []
    worst = 0.0
    for i, path in enumerate(paths):
        cur = Image.open(path).convert('RGB')
        if cur.size != (640, 480):
            raise SystemExit('expected a 640x480 render in %s, got %s'
                             % (path, cur.size))
        if all_mode:
            name, ox, oy = CAPTURES[i]
            worst = max(worst, compare(name, ox, oy, cur, ref_dir,
                                       sheet_rows))
        else:
            for name, ox, oy in CAPTURES:
                worst = max(worst, compare(name, ox, oy, cur, ref_dir,
                                           sheet_rows))
    # A single left/right/diff sheet for eyeballing.
    sw = max(r.width for _, r, _, _ in sheet_rows)
    sh = sum(r.height + 6 for _, r, _, _ in sheet_rows)
    sheet = Image.new('RGB', (sw * 3 + 12, sh), (0, 0, 0))
    y = 0
    for _, ref, crop, diff in sheet_rows:
        sheet.paste(ref, (0, y))
        sheet.paste(crop, (sw + 6, y))
        sheet.paste(diff, (sw * 2 + 12, y))
        y += ref.height + 6
    sheet.save('bin/CityProd_sbs.png')
    print('wrote bin/CityProd_sbs.png; worst mean %.3f/255' % worst)


if __name__ == '__main__':
    main()
