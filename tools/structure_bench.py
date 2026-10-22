"""structure_bench.py - compare the port's TIWindow panel with the reference.

    python tools/structure_bench.py bin/StructurePReference.png bin/struct_final.png

Both images must be the same size (the reference is the 199x217 crop of the
200x216 InfoWin). Writes bin/structure_bench.png: reference, port and an
amplified absolute difference, and prints the mean per-channel |delta| plus a
per-region breakdown. Exit code XC=1 when the mean exceeds 40.
"""
import sys
from PIL import Image, ImageChops, ImageEnhance

LIMIT = 40.0


def mean_abs(a, b):
    d = ImageChops.difference(a, b)
    data = d.convert('L').tobytes()
    return sum(data) / float(len(data))


def main():
    ref_path, port_path = sys.argv[1], sys.argv[2]
    ref = Image.open(ref_path).convert('RGB')
    port = Image.open(port_path).convert('RGB')
    if ref.size != port.size:
        raise SystemExit('size mismatch: %s %s vs %s %s' %
                         (ref_path, ref.size, port_path, port.size))
    whole = mean_abs(ref, port)
    regions = {
        'title': (0, 0, 199, 20),
        'name': (0, 17, 140, 47),
        'left_body': (0, 45, 120, 185),
        'right_city': (120, 18, 199, 212),
        'footer': (0, 180, 199, 217),
    }
    print('structure_bench: %dx%d whole=%.2f/255 limit=%.1f' %
          (ref.width, ref.height, whole, LIMIT))
    for name, box in regions.items():
        print('  %-12s %.2f' % (name, mean_abs(ref.crop(box), port.crop(box))))
    diff = ImageChops.difference(ref, port)
    diff = ImageEnhance.Brightness(diff).enhance(3.0)
    out = Image.new('RGB', (ref.width * 3 + 20, ref.height), (0, 0, 0))
    out.paste(ref, (0, 0))
    out.paste(port, (ref.width + 10, 0))
    out.paste(diff, (ref.width * 2 + 20, 0))
    out.resize((out.width * 2, out.height * 2), Image.NEAREST).save(
        'bin/structure_bench.png')
    return 0 if whole <= LIMIT else 1


if __name__ == '__main__':
    raise SystemExit(main())
