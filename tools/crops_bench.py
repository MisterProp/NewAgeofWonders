"""crops_bench.py - measure the port's world-map crops against the original.

The reference `bin/CropsRef.png` is a 164x131 crop of the original game around
the grass/steppe farm fields of `Scenario\\Custom\\Bloody Marauders.hsm`, at
(16,36). The port's renderer now draws those `Crops.TCrop` tiles with the
terrain-selected `Crops.bmp` frame, so the same rectangle can be compared 1:1.

    python tools/crops_bench.py [game-root] [port-root]

Defaults: game-root = `..`, port-root = `.` (this folder). It renders the map
with `bin/aow_tools.exe mapviz` into `bin/CropsBench_render.png`, crops the
reference rectangle, prints the per-channel / mean absolute pixel difference and
writes:

    bin/CropsBench.png   ours | original | x4 difference
    bin/CropsDiff.png    the x4 amplified difference alone

A lower mean is better: 0 = pixel-identical. The numbers this scene produces:

    with crops      10.70 / 255   (the three crop art frames + terrain/roads)
    no crop objects 22.72 / 255   (`--no-objects`; shows what the fields add)
    crop-only patch  0.97 / 255   (the pure flower field at 0,85..45,130)

The residual of the whole frame is the pre-existing object-anchor offset
(city/army) and a road edge, not the crop tiles: a crop-only patch is almost
pixel-identical and the field aligns at (0,0) with the reference.
"""
import os
import subprocess
import sys

from PIL import Image, ImageChops, ImageStat

# (left, top, right, bottom) inside the 64x64 level render of Bloody Marauders.
REGION = (477, 1085, 477 + 164, 1085 + 131)
# A pure crop patch away from the city and roads, to isolate the crop tiles.
PATCH = (0, 85, 45, 130)


def mean_abs(a, b):
    d = ImageChops.difference(a, b)
    s = ImageStat.Stat(d).mean
    return s, sum(s) / 3.0


def main():
    game_root = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else '..')
    port_root = os.path.abspath(sys.argv[2] if len(sys.argv) > 2 else '.')
    ref_path = os.path.join(port_root, 'bin', 'CropsRef.png')
    if not os.path.exists(ref_path):
        raise SystemExit('missing reference: ' + ref_path)

    render = os.path.join(port_root, 'bin', 'CropsBench_render.png')
    tools = os.path.join(port_root, 'bin', 'aow_tools.exe')
    cmd = [tools, 'mapviz',
           os.path.join(game_root, 'Scenario', 'Custom', 'Bloody Marauders.hsm'),
           render,
           '--res', os.path.join(game_root, 'Release', 'Release.hss'),
           '--root', game_root, '--level', '0']
    print('+', ' '.join(cmd))
    subprocess.run(cmd, check=True)

    ours = Image.open(render).convert('RGB').crop(REGION)
    ref = Image.open(ref_path).convert('RGB')
    if ours.size != ref.size:
        raise SystemExit('region size %s != reference %s' % (ours.size, ref.size))

    s, m = mean_abs(ours, ref)
    print('whole crop field  %dx%d  mean|d| R=%.2f G=%.2f B=%.2f  avg=%.2f/255'
          % (ref.width, ref.height, s[0], s[1], s[2], m))

    patch = (REGION[0] + PATCH[0], REGION[1] + PATCH[1],
             REGION[0] + PATCH[2], REGION[1] + PATCH[3])
    ps, pm = mean_abs(Image.open(render).convert('RGB').crop(patch),
                      ref.crop(PATCH))
    print('crop tiles only  %dx%d  mean|d| R=%.2f G=%.2f B=%.2f  avg=%.2f/255'
          % (PATCH[2] - PATCH[0], PATCH[3] - PATCH[1], ps[0], ps[1], ps[2], pm))

    diff = ImageChops.difference(ours, ref).point(lambda v: min(255, v * 4))
    sheet = Image.new('RGB', (ref.width * 3 + 16, ref.height), (24, 20, 24))
    for i, im in enumerate((ours, ref, diff)):
        sheet.paste(im, (i * (ref.width + 8), 0))
    sheet.save(os.path.join(port_root, 'bin', 'CropsBench.png'))
    diff.save(os.path.join(port_root, 'bin', 'CropsDiff.png'))
    print('wrote bin/CropsBench.png and bin/CropsDiff.png')


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    main()
