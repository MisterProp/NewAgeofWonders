import sys
sys.path.insert(0, '.')
import _peek
import _crop_rgb

REF, OUR = sys.argv[1], sys.argv[2]
RX, RY, W, H, STEP = (int(a) for a in sys.argv[3:8])
DX, DY = 232, 285

rw, rh, bpp, rpx = _crop_rgb.load_rgb(REF)
ow, oh, opx = _peek.load(OUR)


def ch(px, w, x, y, n):
    i = (y * w + x) * n
    r, g, b = px[i], px[i + 1], px[i + 2]
    if b > r + 8 and b > g + 4:
        return '~'
    if r >= 130 and g >= 105 and b <= 135 and (r - b) >= 25:
        return 's'
    return '#'


for name, ox, oy, px, w, n in (
        ('ref', RX, RY, rpx, rw, bpp),
        ('our', RX + DX, RY + DY, opx, ow, 4)):
    print('---', name, 'window origin', ox, oy)
    cnt = {'~': 0, '#': 0, 's': 0}
    for y in range(0, H, STEP):
        line = ''
        for x in range(0, W, STEP):
            c = ch(px, w, ox + x, oy + y, n)
            cnt[c] += 1
            line += c
        print(line)
    tot = sum(cnt.values())
    print('--- %s water=%.1f%% sand=%.1f%% land=%.1f%%'
          % (name, 100.0 * cnt['~'] / tot, 100.0 * cnt['s'] / tot,
             100.0 * cnt['#'] / tot))
