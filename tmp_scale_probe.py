"""tmp_scale_probe.py - is ShadowRef exactly 1:1 with the port's render?"""
from PIL import Image

ref = Image.open('bin/ShadowRef.png').convert('RGB')
rp = ref.load()
render = Image.open('tmp_bm_l0r.png').convert('RGB')
ox = 8 + 31 * 32
oy = 45 * 32 + (31 & 1) * 16

# sample grid: skip the banner/unit/shadow area (x 12..45, y 0..52)
SAMPLES = [(x, y) for y in range(0, ref.height, 2)
           for x in range(0, ref.width, 2)
           if not (11 <= x <= 46 and y <= 53)]

for scale in (0.94, 0.96, 0.98, 0.99, 1.0, 1.01, 1.02, 1.04, 1.06, 1.08):
    cw = int(round(ref.width / scale))
    ch = int(round(ref.height / scale))
    best = None
    for dy in range(-20, 21):
        for dx in range(-20, 21):
            x0 = ox - 9 + dx
            y0 = oy + 13 + dy
            if x0 < 0 or y0 < 0 or x0 + cw >= render.width or y0 + ch >= render.height:
                continue
            crop = render.crop((x0, y0, x0 + cw, y0 + ch))
            crop = crop.resize((ref.width, ref.height), Image.LANCZOS)
            cp = crop.load()
            tot = 0
            for (x, y) in SAMPLES:
                r1, g1, b1 = cp[x, y]
                r2, g2, b2 = rp[x, y]
                tot += abs(r1 - r2) + abs(g1 - g2) + abs(b1 - b2)
            v = tot / (len(SAMPLES) * 3)
            if best is None or v < best[0]:
                best = (v, dx, dy)
    print('scale %.2f: mean|d|=%.2f at offset (%d,%d)' %
          (scale, best[0], best[1], best[2]))
