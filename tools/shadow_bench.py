"""shadow_bench.py - measure the unit ground shadow against the original game.

    python tools/shadow_bench.py [game-root] [--outdir <dir>] [--max <n>]

`bin/ShadowRef.png` is a 66x58 crop of the ORIGINAL Age of Wonders running
`Scenario\\Custom\\Bloody Marauders.hsm` (level 0): the Human leader army (the
"Human Hero Leader" gfx record, `UNITS\\HMN\\HMNHE.ILB`) standing on hex (31,46)
after a move south - facing 4, so the standing pose is `seq[3]` = HMNHE frame 109
and the shadow `TUnitHS.ShowShadow@0x55784504` draws is `seq[27]` =
`UNITS\\USHADOW.ILB` frame 26 at `HXtoHP + (-1,18)` (docs/16_UNIT_ANIM.md 10).

This script drives the port's own in-game screen into that exact state - click
the leader army on its scenario hex (31,45), click the hex south of it, let the
walk finish - then crops the reference's window out of the render (the camera the
port reports), and reports the mean |delta| over

  * the whole crop (the terrain dominates), and
  * the 17x14 rectangle the shadow occupies - the number the shadow is judged
    on. Its floor is the screenshot's own colour conversion plus the port's
    pre-existing 8 px terrain/object offset (see the note below), so `--max`
    gates the *regression*, not perfection.

It also writes `<outdir>/shadow_bench.png` (ours | reference | diff x4) so the
result can be eyeballed.

Two known, pre-existing differences show up in the crop and are NOT what this
benchmark gates on:
  * the port draws terrain hexagons and single-hex objects 8 px right of the
    engine (`Hexagon.THexagon.Show` / `HSEngine.TMapObject.Show` draw at
    `HXtoHP.x - 8`; `TUnitHS.Show` and the port's units use `HXtoHP`), so the
    terrain under the crop is shifted by 8 px;
  * the army's own size banner (`TArmyHS.Show`: the race shield with the unit
    count) is not drawn by the port yet.
"""
import os
import re
import subprocess
import sys

try:
    from PIL import Image, ImageChops
except ImportError:  # Pillow is a dev-only tool dependency
    print('shadow_bench: Pillow not installed - skipped')
    sys.exit(0)

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
GAME = os.path.join(PORT, 'bin', 'AoW.exe')
REF = os.path.join(PORT, 'bin', 'ShadowRef.png')
# `--start-map` takes the scenario's own file name (it prepends Scenario/Custom).
MAP = 'Bloody Marauders.hsm'

# The reference's origin in map pixels (HXtoHP space): the crop shows hex (31,46)
# - whose HXtoHP is (1000,1488) - with the unit's own content (the standing pose
# `seq[3]` = HMNHE 109, at `HXtoHP + (4,-11)`) landing at crop (21,8).
REF_HX, REF_HY = 983, 1469
# The hex the leader army stands on in the shipped scenario and the hex it walks
# onto - the state the reference screenshot was taken in.
FROM_HEX, TO_HEX = (31, 45), (31, 46)
# The shadow's own footprint in the crop: `seq[27]` = USHADOW 26 at
# `HXtoHP + (-1,18)`, 17x14.
SHADOW_BOX = (1000 - 1 - REF_HX, 1488 + 18 - REF_HY, 17, 14)
# The map window's client box: 416x408 at the window's 12 px inset.
CAM_W, CAM_H, INSET = 416, 408, 12


def hxtohp(x, y):
    return x * 32 + 8, y * 32 + (x & 1) * 16


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    root = args[0] if args else '..'
    outdir = os.path.join(PORT, 'bin')
    if '--outdir' in sys.argv:
        outdir = sys.argv[sys.argv.index('--outdir') + 1]
    limit = 16.0
    if '--max' in sys.argv:
        limit = float(sys.argv[sys.argv.index('--max') + 1])

    ref = Image.open(REF).convert('RGB')
    w, h = ref.size

    # The camera the game starts with: `cam_center` puts the leader's hex centre
    # (`HXtoHP + 16`) at the middle of the map area. The two clicks are client
    # coordinates (the army's hex, then the hex south of it).
    px, py = hxtohp(*FROM_HEX)
    cam_x = max(16, px + 16 - CAM_W // 2)
    cam_y = max(16, py + 16 - CAM_H // 2)

    def client(hx, hy):
        x, y = hxtohp(hx, hy)
        return x - cam_x + INSET, y - cam_y + INSET

    out_png = os.path.join(outdir, 'shadow_ours.png')
    log = os.path.join(outdir, 'shadow_ours.txt')
    for p in (out_png, log):
        if os.path.exists(p):
            os.remove(p)
    fx, fy = client(*FROM_HEX)
    tx, ty = client(*TO_HEX)
    # `--start-map` runs the Scenario -> Start! flow, which centres the view on
    # the player's leader (`TAoWE.TAoWHSMap.CenterOnLeader`, without selecting
    # it) - the same state the reference screenshot was taken from. The two
    # clicks then select the leader and walk it one hex south.
    cmd = [GAME, '--root', root, '--start-map', os.path.basename(MAP),
           '--scen-human', '0', '--no-music', '--no-cursor',
           '--render', out_png, '--game-clock', '0', '--mouse', '560', '440',
           '--game-click', str(fx), str(fy),
           '--game-click', str(tx), str(ty),
           '--game-anim', '12']
    with open(log, 'w') as fh:
        subprocess.run(cmd, stdout=fh, stderr=subprocess.STDOUT, timeout=300)
    if not os.path.exists(out_png):
        print('shadow_bench: the port produced no render (%s)' % out_png)
        return 1
    text = open(log, errors='replace').read()
    m = re.search(r'game_cam=(-?\d+),(-?\d+)\+(\d+)x(\d+)', text)
    if not m:
        print('shadow_bench: the port reported no camera: %s' % text.strip()[:200])
        return 1
    cam_x, cam_y = int(m.group(1)), int(m.group(2))
    # Where the reference's origin lands in the port's screen: the unit's hex is
    # at HXtoHP in both (the port's terrain is 8 px right, see the module note).
    ox = REF_HX - cam_x + INSET
    oy = REF_HY - cam_y + INSET
    ours = Image.open(out_png).convert('RGB').crop((ox, oy, ox + w, oy + h))
    pr, po = ref.load(), ours.load()

    tot = 0
    for y in range(h):
        for x in range(w):
            for i in range(3):
                tot += abs(po[x, y][i] - pr[x, y][i])
    whole = tot / (3.0 * w * h)
    sx, sy, sw, sh = SHADOW_BOX
    stot = 0
    for y in range(sy, sy + sh):
        for x in range(sx, sx + sw):
            for i in range(3):
                stot += abs(po[x, y][i] - pr[x, y][i])
    shadow = stot / (3.0 * sw * sh)

    sheet = Image.new('RGB', (w, h * 3 + 8), (255, 0, 255))
    sheet.paste(ours, (0, 0))
    sheet.paste(ref, (0, h + 4))
    sheet.paste(ImageChops.difference(ours, ref).point(
        lambda v: min(255, v * 4)), (0, 2 * h + 8))
    bench_png = os.path.join(outdir, 'shadow_bench.png')
    sheet.save(bench_png)
    os.remove(out_png)
    os.remove(log)
    print('shadow_bench: %dx%d  cam=%d,%d  whole=%.2f  shadow(%dx%d)=%.2f  '
          'limit=%.1f' % (w, h, cam_x, cam_y, whole, sw, sh, shadow, limit))
    print('shadow_bench: wrote %s (ours / reference / diff x4)' % bench_png)
    return 1 if shadow > limit else 0


if __name__ == '__main__':
    sys.exit(main())
