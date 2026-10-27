"""unit_sel_bench.py - compare the port's unit detail page with the capture.

`bin/UnitsSelectionRef.png` is a 636x429 crop of the 640x480 game frame taken
in Bloody Marauders' human capital (Venice) with the Human Swordsman card
checked.  Its crop origin is (3,-1): ref(x,y) is game pixel (x+3, y-1).  This
bench registers that crop onto the port's render, then reports

  * the whole frame,
  * the page area (the UnitPnl band, ref rows 233..349),
  * the two stat columns / portrait / ability list / cost boxes separately,

and writes bin/UnitSel_sbs.png (reference | render | amplified difference).

    python tools/unit_sel_bench.py bin/unit_sel_venice.png
    python tools/unit_sel_bench.py bin/unit_sel_venice.png bin/UnitsSelectionRef.png
"""
import os
import sys

from PIL import Image, ImageChops

# UnitsSelectionRef.png's crop origin inside the 640x480 game frame.
ORIGIN_X = 3
ORIGIN_Y = -1

# The regions the page draws (game coords; the ref box is shifted by the crop).
REGIONS = {
    "page": (23, 227, 617, 357),
    "portrait": (33, 237, 123, 347),
    "stats": (143, 256, 309, 347),
    "abilities": (331, 256, 497, 347),
    "cost": (517, 237, 607, 347),
}


def mean_diff(a, b):
    """Mean of |dR|+|dG|+|dB| over the pixels of two same-size crops."""
    d = ImageChops.difference(a.convert("RGB"), b.convert("RGB"))
    px = d.load()
    total = 0
    changed = 0
    maxd = 0
    for y in range(d.height):
        for x in range(d.width):
            p = px[x, y]
            e = p[0] + p[1] + p[2]
            total += e
            if e:
                changed += 1
            if e > maxd:
                maxd = e
    n = d.width * d.height
    return total / (n * 3.0), changed, n, maxd


def main():
    cur_path = sys.argv[1] if len(sys.argv) > 1 else "bin/unit_sel_venice.png"
    ref_path = (sys.argv[2] if len(sys.argv) > 2
                else "bin/UnitsSelectionRef.png")
    cur = Image.open(cur_path).convert("RGB")
    ref = Image.open(ref_path).convert("RGB")
    if cur.size != (640, 480):
        raise SystemExit("expected a 640x480 render, got %s" % (cur.size,))

    # Register the reference crop on the render.
    x0, y0 = ORIGIN_X, ORIGIN_Y
    ref_w = min(ref.width, cur.width - x0)
    ref_h = min(ref.height, cur.height - y0)
    ref = ref.crop((0, 0, ref_w, ref_h))
    reg = cur.crop((x0, y0, x0 + ref_w, y0 + ref_h))

    print("# %s vs %s (%dx%d, origin %+d%+d)"
          % (cur_path, ref_path, ref_w, ref_h, x0, y0))
    m, changed, n, mx = mean_diff(ref, reg)
    print("frame      %dx%d mean|d| = %.3f/255  changed %d/%d  max %d"
          % (ref_w, ref_h, m, changed, n, mx))

    worst = 0.0
    for name, (gx0, gy0, gx1, gy1) in REGIONS.items():
        # Game rect -> reference crop coordinates.
        rx0 = max(0, gx0 - x0)
        ry0 = max(0, gy0 - y0)
        rx1 = min(ref_w, gx1 - x0)
        ry1 = min(ref_h, gy1 - y0)
        if rx1 <= rx0 or ry1 <= ry0:
            continue
        a = ref.crop((rx0, ry0, rx1, ry1))
        b = reg.crop((rx0, ry0, rx1, ry1))
        m, changed, n, mx = mean_diff(a, b)
        worst = max(worst, m)
        print("%-9s %dx%d mean|d| = %.3f/255  changed %d/%d  max %d"
              % (name, rx1 - rx0, ry1 - ry0, m, changed, n, mx))

    diff = ImageChops.difference(ref, reg).point(lambda v: min(255, v * 4))
    sheet = Image.new("RGB", (ref_w * 3 + 16, ref_h), (20, 20, 24))
    sheet.paste(ref, (0, 0))
    sheet.paste(reg, (ref_w + 8, 0))
    sheet.paste(diff, (ref_w * 2 + 16, 0))
    out = "bin/UnitSel_sbs.png"
    sheet.save(out)
    print("wrote %s; worst page region mean %.3f/255" % (out, worst))


if __name__ == "__main__":
    main()
