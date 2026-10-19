from PIL import Image

base = Image.open('tmp_fullmap.png').convert('RGBA')
h6 = Image.open('tmp_hex6.png').convert('RGBA')
h7 = Image.open('tmp_hex7.png').convert('RGBA')
KEY = (0, 182, 74)
HX, HY = 520, 1088


def blit(dst, src, ox, oy, mode):
    sp = src.load()
    dp = dst.load()
    for sy in range(src.height):
        dy = oy + sy
        if dy < 0 or dy >= dst.height:
            continue
        for sx in range(src.width):
            dx = ox + sx
            if dx < 0 or dx >= dst.width:
                continue
            s = sp[sx, sy]
            if s[:3] == KEY:
                continue
            d = dp[dx, dy]
            r, g, b = s[:3]
            if mode == 'max':            # lighten
                o = (max(r, d[0]), max(g, d[1]), max(b, d[2]))
            elif mode == 'screen':
                o = tuple(255 - (255 - s[i]) * (255 - d[i]) // 255 for i in range(3))
            elif mode == 'luma':
                a = max(r, g, b)
                o = tuple((s[i] * a + d[i] * (255 - a)) // 255 for i in range(3))
            elif mode == 'luma_bluesrc':  # alpha = max, but src kept (premult issue)
                a = max(r, g, b)
                o = tuple(min(255, d[i] + s[i] * a // 255) for i in range(3))
            dp[dx, dy] = (o[0], o[1], o[2], 255)


for mode in ('max', 'screen', 'luma', 'luma_bluesrc'):
    img = base.copy()
    blit(img, h7, HX - 7, HY - 30, mode)
    blit(img, h6, HX - 8, HY - 15, mode)
    crop = img.crop((HX - 16, HY - 32, HX + 48, HY + 36))
    crop.resize((crop.width * 4, crop.height * 4), Image.NEAREST).save(
        'tmp_m2_%s.png' % mode)
    print(mode, 'ok')