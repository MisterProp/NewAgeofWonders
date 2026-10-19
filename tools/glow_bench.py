"""glow_bench.py - measure the map renderer's glow against the original game.

    python tools/glow_bench.py [game-root] [--map <file>] [--outdir <dir>]

`docs/refs/glow_ref.png` is a crop of the ORIGINAL Age of Wonders running
`Scenario\\Custom\\Bloody Marauders.hsm` (level 0) over a group of glowing
structures (a mine sparkle, a dungeon mouth, a river ford). This script renders
the same map with `bin/aow_view.exe`, aligns the reference against the render,
and reports the mean |delta| over

  * every pixel of the aligned crop (the terrain dominates), and
  * only the pixels where a glow overlay is drawn (the render differs from a
    `--no-anim` render) - this is the number the glow blend is judged on.

It also writes `<outdir>/glow_bench.png` (our render | reference | diff x4) so the
result can be eyeballed. Exits non-zero when the glow residual regresses past
`--max` (default 16.0), so build.ps1 can gate it.

The reference is a screenshot, so its own colour conversion leaves a floor of
~5 mean |delta| even on pixels that are pure terrain; the glow blend (the GFXE
brighten table) was fitted against it - an opaque blit scored 33.1 on the glow
pixels, the brighten blend 12.5.
"""
import os
import subprocess
import sys

try:
    from PIL import Image, ImageChops
except ImportError:  # Pillow is a dev-only tool dependency
    print('glow_bench: Pillow not installed - skipped')
    sys.exit(0)

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
VIEW = os.path.join(PORT, 'bin', 'aow_view.exe')
REF = os.path.join(PORT, 'docs', 'refs', 'glow_ref.png')
DEFAULT_MAP = '../Scenario/Custom/Bloody Marauders.hsm'
# Where the reference crop sits inside the full-map render (found by aligning the
# reference against the render); the map is fixed, so this is stable.
OX, OY = 1572, 1323


def render(map_path, root, out, extra=None):
    cmd = [VIEW, map_path, '--root', root, '--level', '0', '--render', out]
    if extra:
        cmd += extra
    r = subprocess.run(cmd, capture_output=True)
    return r.returncode == 0 and os.path.exists(out)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    root = args[0] if args else '..'
    map_path = DEFAULT_MAP
    if '--map' in sys.argv:
        map_path = sys.argv[sys.argv.index('--map') + 1]
    outdir = os.path.join(PORT, 'bin')
    if '--outdir' in sys.argv:
        outdir = sys.argv[sys.argv.index('--outdir') + 1]
    limit = 16.0
    if '--max' in sys.argv:
        limit = float(sys.argv[sys.argv.index('--max') + 1])

    full = os.path.join(outdir, 'glow_full.png')
    stat = os.path.join(outdir, 'glow_static.png')
    if not render(map_path, root, full):
        print('glow_bench: render failed')
        return 1
    if not render(map_path, root, stat, ['--no-anim']):
        print('glow_bench: static render failed')
        return 1
    ref = Image.open(REF).convert('RGB')
    w, h = ref.size
    box = (OX, OY, OX + w, OY + h)
    ours = Image.open(full).convert('RGB').crop(box)
    terr = Image.open(stat).convert('RGB').crop(box)
    pr, po, pt = ref.load(), ours.load(), terr.load()

    tot = glo = 0
    ng = 0
    for y in range(h):
        for x in range(w):
            T, S, R = pt[x, y], po[x, y], pr[x, y]
            for i in range(3):
                tot += abs(S[i] - R[i])
            if T != S:
                ng += 1
                for i in range(3):
                    glo += abs(S[i] - R[i])
    whole = tot / (3.0 * w * h)
    glow = glo / (3.0 * ng) if ng else 0.0

    sheet = Image.new('RGB', (w, h * 3 + 8), (255, 0, 255))
    sheet.paste(ours, (0, 0))
    sheet.paste(ref, (0, h + 4))
    sheet.paste(ImageChops.difference(ours, ref).point(
        lambda v: min(255, v * 4)), (0, 2 * h + 8))
    out_png = os.path.join(outdir, 'glow_bench.png')
    sheet.save(out_png)
    os.remove(full)
    os.remove(stat)
    print('glow_bench: %dx%d  whole=%.2f  glow-pixels=%.2f (%d px)  limit=%.1f'
          % (w, h, whole, glow, ng, limit))
    print('glow_bench: wrote %s (ours / reference / diff x4)' % out_png)
    return 1 if glow > limit else 0


if __name__ == '__main__':
    sys.exit(main())
