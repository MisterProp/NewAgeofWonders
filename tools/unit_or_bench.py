"""unit_or_bench.py - the P1CB..P12CB production-card row vs UnitsORREF.png.

`bin/UnitsORREF.png` is a 412x70 crop of the original game's production page
(`TCityScreen` mode 1) showing the first eight cards of the Bloody Marauders
human capital (Venice, hex 32,45): eight `PreDrawProductions` cells with their
unit art. This bench renders the same page through the port's *live* card path -
the city is not the captured reference city, so `draw_city_production_cards`
draws the row instead of blitting the capture - registers the crop on the
640x480 frame and reports the mean |delta| per card.

    python tools/unit_or_bench.py                 # render + compare
    python tools/unit_or_bench.py --render bin/row.png
    python tools/unit_or_bench.py --compare bin/row.png

What the row must show, recovered from `PreDrawProductions@0x43d950`:
  * RelFr 42/43 (52x64, the TAOWCheckBox art) at (cell.x, cell.y - 2), *not*
    clipped to the 52x52 control - its frame reaches cell.y + 62;
  * CityWin 4 (produce) / 5 (install) 48x60 at (cell.x + 2, cell.y);
  * the unit's standing pose facing 3 (produce) / 5 (install), drawn with the
    frame's own base+shift at (cell.x - 4, cell.y - 6);
  * on an install card, the CityWin 5 face again at 45 % alpha over the art
    (`GetLookupTables(AlphaBlendTable, 0x2d)` + `TSprite16.ShowBlended`).
"""
import os
import subprocess
import sys

from PIL import Image, ImageChops

HERE = os.path.dirname(os.path.abspath(__file__))
PORT_ROOT = os.path.abspath(os.path.join(HERE, '..'))
GAME_ROOT = os.path.abspath(os.path.join(PORT_ROOT, '..'))
REF = os.path.join(PORT_ROOT, 'bin', 'UnitsORREF.png')
DEFAULT_RENDER = os.path.join(PORT_ROOT, 'bin', 'UnitsORREF_live.png')
SHEET = os.path.join(PORT_ROOT, 'bin', 'UnitsORREF_sbs.png')

# The reference is the 640x480 page cropped at (8,161); the card cells sit at
# x = 13 + 51*i, y = 164, 52x52 (ActivityPnl 11,164 + P1CB WinLeft 2/WinTop 0).
REF_OX, REF_OY = 8, 161
CELL_X, CELL_Y, PITCH, CARD = 13, 164, 51, 52
# The case UnitsORREF.png was captured from.
MAP = 'Bloody Marauders.hsm'
CITY_HEX = (32, 45)      # Venice, the human capital
SEAT = 0                 # --scen-human index
CARDS = 8                # the crop only covers the first eight cells
# The live row is the point of the bench, so the run must NOT look like the
# captured page: a non-reference city name forces draw_city_production_cards().
CITY_NAME = 'Venezia'
# Gate on the worst *card*: a correct card is pixel-identical or within the
# engine's 16-bit surface rounding (the captures are RGB555-expanded), so 1.0 is
# already generous - but the fade pass runs through a 5-bit-channel lookup table,
# which leaves a couple of units of rounding on the faded cards.
MAX_CARD = 1.0


def render(out_path):
    """Render the live production row; returns the finished process."""
    exe = os.path.join(PORT_ROOT, 'bin', 'AoW.exe')
    args = [exe, '--root', GAME_ROOT, '--start-map', MAP,
            '--scen-human', str(SEAT), '--city-screen',
            '--structure-tile', str(CITY_HEX[0]), str(CITY_HEX[1]),
            '--structure-name', CITY_NAME, '--city-change', '--city-activity',
            '0', '--render', out_path, '--no-music', '--no-cursor']
    if os.path.exists(out_path):
        os.remove(out_path)
    proc = subprocess.run(args, capture_output=True, text=True)
    if not os.path.exists(out_path):
        sys.stderr.write(proc.stdout[-2000:])
        raise SystemExit('render failed: %s' % ' '.join(args))
    return proc


def mean_abs(a, b):
    px = ImageChops.difference(a, b).load()
    total = 0
    for y in range(a.height):
        for x in range(a.width):
            p = px[x, y]
            total += p[0] + p[1] + p[2]
    return total / (a.width * a.height * 3.0)


def compare(render_path):
    ref = Image.open(REF).convert('RGB')
    cur = Image.open(render_path).convert('RGB')
    if cur.size != (640, 480):
        raise SystemExit('expected a 640x480 render in %s, got %s'
                         % (render_path, cur.size))
    crop = cur.crop((REF_OX, REF_OY, REF_OX + ref.width,
                     REF_OY + ref.height))
    worst = 0.0
    print('%s vs %s  (%dx%d at %d,%d)'
          % (os.path.basename(render_path), os.path.basename(REF),
             ref.width, ref.height, REF_OX, REF_OY))
    for i in range(CARDS):
        x = CELL_X + i * PITCH - REF_OX + 2
        y = CELL_Y - REF_OY + 2
        a = ref.crop((x, y, x + CARD - 4, y + CARD - 4))
        b = crop.crop((x, y, x + CARD - 4, y + CARD - 4))
        d = mean_abs(a, b)
        worst = max(worst, d)
        print('  card %d  mean|d| = %6.3f/255' % (i, d))
    # The whole band also carries the 2 px gutters between the cards; a
    # non-reference city repaints `city_bg_clean` over them (the live page owns
    # no capture), so that figure is informational - the cards are graded.
    print('worst card %.3f/255 (gate %.1f)  |  whole band %.3f/255'
          % (worst, MAX_CARD, mean_abs(ref, crop)))

    # One sheet: reference / port / amplified difference, 3x for the eye.
    sheet = Image.new('RGB', (ref.width, ref.height * 3 + 8), (20, 20, 24))
    sheet.paste(ref, (0, 0))
    sheet.paste(crop, (0, ref.height + 4))
    sheet.paste(ImageChops.difference(ref, crop).point(
        lambda v: min(255, v * 4)), (0, ref.height * 2 + 8))
    sheet = sheet.resize((ref.width * 3, (ref.height * 3 + 8) * 3),
                         Image.NEAREST)
    sheet.save(SHEET)
    print('wrote %s (reference, port, difference x4)' % SHEET)
    return worst


def main():
    args = sys.argv[1:]
    path = DEFAULT_RENDER
    if '--render' in args:
        path = args[args.index('--render') + 1]
    if '--compare' not in args:
        proc = render(path)
        for line in proc.stdout.splitlines():
            if line.startswith('city_production_card='):
                print('  %s' % line)
    worst = compare(path)
    if worst > MAX_CARD:
        raise SystemExit('production cards differ from %s (worst %.3f/255)'
                         % (os.path.basename(REF), worst))


if __name__ == '__main__':
    main()
