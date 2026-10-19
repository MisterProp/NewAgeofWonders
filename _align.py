import sys
sys.path.insert(0, '.')
import _peek
import _crop_rgb

OUR = sys.argv[1]
REF = sys.argv[2]
STEP = int(sys.argv[3]) if len(sys.argv) > 3 else 8


def mask_rgba(path, step):
    w, h, px = _peek.load(path)
    mw, mh = w // step, h // step
    m = bytearray(mw * mh)
    for y in range(mh):
        for x in range(mw):
            si = ((y * step) * w + x * step) * 4
            r, g, b = px[si], px[si + 1], px[si + 2]
            m[y * mw + x] = 1 if (b > r + 8 and b > g + 4) else 0
    return mw, mh, m


def mask_rgb(path, step):
    w, h, bpp, px = _crop_rgb.load_rgb(path)
    mw, mh = w // step, h // step
    m = bytearray(mw * mh)
    for y in range(mh):
        for x in range(mw):
            si = ((y * step) * w + x * step) * bpp
            r, g, b = px[si], px[si + 1], px[si + 2]
            m[y * mw + x] = 1 if (b > r + 8 and b > g + 4) else 0
    return mw, mh, m


ow, oh, o = mask_rgba(OUR, STEP)
rw, rh, r = mask_rgb(REF, STEP)
print('ours', ow, oh, 'ref', rw, rh)
best = (-1, 0, 0)
for oy in range(0, oh - rh + 1, 2):
    for ox in range(0, ow - rw + 1, 2):
        same = 0
        for y in range(0, rh, 2):
            base_o = (oy + y) * ow + ox
            base_r = y * rw
            for x in range(0, rw, 2):
                if o[base_o + x] == r[base_r + x]:
                    same += 1

        if same > best[0]:
            best = (same, ox, oy)
tot = (rh // 2 + 1) * (rw // 2 + 1)
print('best offset(step=%d) ox=%d oy=%d  agree=%.1f%%'
      % (STEP, best[1], best[2], 100.0 * best[0] / tot))
print('pixel offset =', best[1] * STEP, best[2] * STEP)
