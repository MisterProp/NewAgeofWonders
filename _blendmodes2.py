from PIL import Image

base = Image.open('tmp_fullmap.png').convert('RGBA')
h6 = Image.open('tmp_hex6.png').convert('RGBA')   # front (id 6)
h7 = Image.open('tmp_hex7.png').convert('RGBA')   # back  (id 7)
KEY = (0, 182, 74)
HX, HY = 520, 1088          # hex_origin(16,34)


def blit(dst, src, ox, oy, mode):
    w, h = src.size
    sp = src.load()
    dp = dst.load()
    for sy in range(h):
        dy = oy + sy
        if dy < 0 or dy >= dst.height:
            continue
        for sx in range(w):
            dx = ox + sx
            if dx < 0 or dx >= dst.width:
                continue
            s = sp[sx, sy]
            if s[:3] == KEY:
                continue
            d = dp[dx, dy]
            r, g, b = s[:3]
            if mode == 'luma':       # alpha = brightness (max channel)
                a = max(r, g, b)
                o = tuple((s[i] * a + d[i] * (255 - a)) // 255 for i in range(3))
            elif mode == 'luma_avg':  # alpha = mean channel
                a = (r + g + b) // 3
                o = tuple((s[i] * a + d[i] * (255 - a)) // 255 for i in range(3))
            elif mode == 'add':
                o = tuple(min(255, s[i] + d[i]) for i in range(3))
            elif mode == 'add50':
                o = tuple(min(255, d[i] + s[i] // 2) for i in range(3))
            elif mode == 'over50':
                o = tuple((s[i] * 128 + d[i] * 127) // 255 for i in range(3))
            dp[dx, dy] = (o[0], o[1], o[2], 255)


for mode in ('luma', 'luma_avg', 'add', 'add50', 'over50'):
    img = base.copy()
    blit(img, h7, HX - 7, HY - 30, mode)
    blit(img, h6, HX - 8, HY - 15, mode)
    crop = img.crop((HX - 16, HY - 32, HX + 48, HY + 36))
    crop.resize((crop.width * 4, crop.height * 4), Image.NEAREST).save(
        'tmp_m_%s.png' % mode)
    print(mode, 'ok')