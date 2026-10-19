"""walk_settle_check.py - verify the world-map walk's hand-off to the standing pose.

    python tools/walk_settle_check.py [--exe <AoW.exe>] [--ticks N] [--out <dir>]

`TUnitHS.Show` frames a unit's walking cycle off the *global* map clock, so which
pose of the cycle is on screen when a move ends has nothing to do with the move:
the walk is cut off wherever the clock happens to be and the standing pose
(`seq[dir-1]`) then replaces that mid-stride pose in a single frame - the visible
"stops a little and then dashes" on arrival.  The port fixes that in two steps
(src/include/aow/unit_anim.hpp, docs/16_UNIT_ANIM.md):

  * `walk_settle_phase` picks the clock phase that lands the walking cycle's last
    pose on the last animation frame the move draws, so the stride *finishes* at
    the destination, and
  * `walk_settle_weight` fades that last pose into the standing pose over the
    walk's final few frames, so the sprite the front-end swaps to when the walk
    ends is already the pose the standing army is drawn in.

This tool drives the port's own head-less in-game walk (the tutorial hero's
3-hex move, the command line docs/16_UNIT_ANIM.md records) for every animation
tick of the move and then checks, from the rendered frames alone:

  * the front-end reports the aligned settle phase (`game_walk=... settle=N`),
  * the sprite's per-tick travel is printed (it must be even, not stuttering),
  * the hand-off is a *pure translation*: taking the step out between the last
    walking frame and the frame the walk ends on leaves (almost) no pixels
    changed - i.e. the unit arrives in the pose it stands in, no pose snap.

Exits 0 when the hand-off is clean, 1 otherwise.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import common  # noqa: E402  (port paths, stdlib only)

from PIL import Image  # noqa: E402

# The click sequence docs/16_UNIT_ANIM.md records: park the scanner on the
# tutorial hero, select it, then click a hex three steps away.
CLICKS = [(513, 141), (220, 216), (284, 152)]


def run_walk(exe, ticks, out_dir):
    """Render ticks 0..ticks of the move; return (frames, game_walk line)."""
    frames = []
    report = None
    for n in range(ticks + 1):
        path = os.path.join(out_dir, 'a%02d.png' % n)
        cmd = [exe, '--screen', 'game']
        for x, y in CLICKS:
            cmd += ['--game-click', str(x), str(y)]
        cmd += ['--game-anim', str(n), '--game-clock', '0', '--no-cursor',
                # Park the pointer off the map *after* the ticks: otherwise the
                # selection hexagon and the hover trail are composited over the
                # army on the frame the walk ends, which would swamp the sprite.
                '--mouse', '500', '400', '--render', path]
        res = subprocess.run(cmd, capture_output=True, text=True)
        if res.returncode != 0 or not os.path.exists(path):
            print('# ERROR: %s exited %d' % (exe, res.returncode))
            print(res.stdout[-2000:])
            return [], None
        for line in res.stdout.splitlines():
            if line.startswith('game_walk='):
                report = line.strip()
        frames.append(path)
    return frames, report


def sprite_pixels(bg, im):
    """The pixels of `im` that differ from the modal background `bg`."""
    px = im.load()
    w, h = im.size
    out = set()
    for y in range(h):
        row = bg[y]
        for x in range(w):
            # The scanner minimap (> x 400) changes only when the move ends.
            if x <= 400 and px[x, y] != row[x]:
                out.add((x, y))
    return out


def centroid(points):
    return (sum(p[0] for p in points) / len(points),
            sum(p[1] for p in points) / len(points))


def main():
    argv = sys.argv[1:]
    exe = os.path.join(common.PORT_ROOT, 'bin', 'AoW.exe')
    ticks = 12          # 3 hexes x 8 sub-steps / 2 frames per tick
    out_dir = os.path.join(common.PORT_ROOT, 'tmp_walksettle')
    if '--exe' in argv:
        exe = argv[argv.index('--exe') + 1]
    if '--ticks' in argv:
        ticks = int(argv[argv.index('--ticks') + 1])
    if '--out' in argv:
        out_dir = argv[argv.index('--out') + 1]
    if not os.path.exists(exe):
        print('ERROR: %s not found - build the port first (build.ps1)' % exe)
        return 1

    shutil.rmtree(out_dir, ignore_errors=True)
    common.ensure_dir(out_dir)
    frames, report = run_walk(exe, ticks, out_dir)
    if not frames:
        return 1
    if report is None:
        print('ERROR: the front-end printed no game_walk= line')
        return 1
    print('# %s' % report)

    ims = [Image.open(p).convert('RGB') for p in frames]
    w, h = ims[0].size
    data = [im.load() for im in ims]
    # The army is hidden on its destination hex while the walk runs, so the
    # per-pixel mode over the *moving* frames is the level without the walker.
    bg = [[None] * w for _ in range(h)]
    for y in range(h):
        for x in range(w):
            counts = {}
            for n in range(1, ticks):
                c = data[n][x, y]
                counts[c] = counts.get(c, 0) + 1
            bg[y][x] = max(counts.items(), key=lambda kv: kv[1])[0]

    sprites = [sprite_pixels(bg, im) for im in ims]
    print('# tick  sprite px  centroid        step from previous tick')
    prev = None
    for n, s in enumerate(sprites):
        if not s:
            print('# %4d  (nothing differs - the walk already ended)' % n)
            continue
        c = centroid(s)
        step = '' if prev is None else '(%+6.1f,%+6.1f)' % (c[0] - prev[0],
                                                            c[1] - prev[1])
        print('# %4d  %9d  (%7.2f,%7.2f)  %s' % (n, len(s), c[0], c[1], step))
        prev = c

    # The hand-off: the last frame that draws the moving sprite vs the first
    # frame drawn after the walk (the level render, i.e. the standing army).
    last = ticks - 1
    a, b = sprites[last], sprites[last + 1]
    if not a or not b:
        print('ERROR: could not isolate the sprites at ticks %d/%d' %
              (last, last + 1))
        return 1
    ax, ay = centroid(a)
    bx, by = centroid(b)
    dx, dy = int(round(bx - ax)), int(round(by - ay))
    shifted = {(x + dx, y + dy) for x, y in a}
    changed = len(shifted ^ b)
    pct = 100.0 * changed / max(1, len(b))
    print('# hand-off: tick %d -> %d, step (%+d,%+d), %d px vs %d px'
          % (last, last + 1, dx, dy, len(a), len(b)))
    print('#   pixels still differing once the step is taken out: %d (%.1f%%)'
          % (changed, pct))
    ok = pct <= 2.0
    print('# %s: the unit %s its standing pose (no pose snap)'
          % ('OK' if ok else 'FAIL',
             'settles into' if ok else 'does NOT arrive in'))
    if not ok:
        print('# (a mid-stride pose swapped for the standing pose is the '
              '"stops a little and then dashes" bug)')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
