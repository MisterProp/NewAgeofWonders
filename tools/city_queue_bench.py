"""city_queue_bench.py - pixel-compare the city queue/production page with
bin/QueueRef.png.

QueueRef.png is a 638x433 1:1 capture of TCityScreen mode 1 page 01 (the
B1CB "Produce Merchandise" page).  Its first 432 rows are the dialog; row 432
is the top of the world-map bar, which the port draws live and therefore does
not compare.  A correct reference render is a 0-pixel match over the dialog.

    python tools/city_queue_bench.py
    python tools/city_queue_bench.py bin/QueueRef.png bin/city_prod_1.png
"""
import sys

from PIL import Image, ImageChops

REF = sys.argv[1] if len(sys.argv) > 1 else 'bin/QueueRef.png'
OURS = sys.argv[2] if len(sys.argv) > 2 else 'bin/city_prod_1.png'
DLG_H = 432


def main():
    ref = Image.open(REF).convert('RGB')
    cur = Image.open(OURS).convert('RGB')
    if cur.size != (640, 480):
        raise SystemExit('expected a 640x480 render, got %s' % (cur.size,))
    w = min(ref.width, 640)
    h = min(ref.height, DLG_H)
    left = ref.crop((0, 0, w, h))
    right = cur.crop((0, 0, w, h))
    diff = ImageChops.difference(left, right)
    px = list(diff.getdata())
    changed = sum(1 for p in px if p != (0, 0, 0))
    total = sum(sum(p) for p in px)
    mean = total / (len(px) * 3.0)
    worst = max(max(p) for p in px)
    print('%s vs %s: %dx%d dialog  mean|d| = %.4f/255  changed %d/%d  max %d'
          % (OURS, REF, w, h, mean, changed, len(px), worst))
    # The reference's last row is the live bar band; report it separately so a
    # real dialog regression cannot hide behind the intentional bar clip.
    if ref.height > DLG_H:
        bar = ref.crop((0, DLG_H, w, ref.height))
        barb = cur.crop((0, DLG_H, w, ref.height))
        bd = ImageChops.difference(bar, barb)
        bpx = list(bd.getdata())
        bch = sum(1 for p in bpx if p != (0, 0, 0))
        print('row %d (live world-map bar): changed %d/%d (not graded)'
              % (DLG_H, bch, len(bpx)))
    diff.save('bin/QueueRef_diff.png')
    if mean > 0.05 or changed != 0:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
