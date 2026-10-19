"""tmp_facecmp.py - compare the reference portrait cell with candidate draws of
the leader face image (L_FACES 45 = L-orc01.BMP, 103x128).

    python tmp_facecmp.py <ref.png> <face.png> <x> <y>
"""
import struct
import sys
import zlib


def paeth(a, b, c):
    p = a + b - c
    pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    if pb <= pc:
        return b
    return c


def decode(path):
    data = open(path, 'rb').read()
    w = h = bpp = None
    idat = b''
    pos = 8
    while pos + 8 <= len(data):
        (length,) = struct.unpack_from('>I', data, pos)
        ctype = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + length]
        if ctype == b'IHDR':
            w, h, depth, color, _c, _f, _i = struct.unpack('>IIBBBBB', body)
            bpp = 4 if color == 6 else 3
        elif ctype == b'IDAT':
            idat += body
        pos += 12 + length
    raw = zlib.decompress(idat)
    stride = w * bpp
    rows = []
    prev = bytearray(stride)
    for y in range(h):
        off = y * (stride + 1)
        f = raw[off]
        line = bytearray(raw[off + 1:off + 1 + stride])
        if f == 1:
            for i in range(bpp, stride):
                line[i] = (line[i] + line[i - bpp]) & 0xFF
        elif f == 2:
            for i in range(stride):
                line[i] = (line[i] + prev[i]) & 0xFF
        elif f == 3:
            for i in range(stride):
                a = line[i - bpp] if i >= bpp else 0
                line[i] = (line[i] + ((a + prev[i]) >> 1)) & 0xFF
        elif f == 4:
            for i in range(stride):
                a = line[i - bpp] if i >= bpp else 0
                c = prev[i - bpp] if i >= bpp else 0
                line[i] = (line[i] + paeth(a, prev[i], c)) & 0xFF
        rows.append(bytes(line))
        prev = line
    return w, h, bpp, rows


def px(rows, bpp, x, y):
    o = x * bpp
    return rows[y][o], rows[y][o + 1], rows[y][o + 2]


def px4(rows, bpp, x, y):
    o = x * bpp
    if bpp == 4:
        return rows[y][o], rows[y][o + 1], rows[y][o + 2], rows[y][o + 3]
    return rows[y][o], rows[y][o + 1], rows[y][o + 2], 255


def avg(rows, bpp, x0, y0, x1, y1):
    r = g = b = n = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            a, c, d = px(rows, bpp, x, y)
            r += a
            g += c
            b += d
            n += 1
    return (r / n, g / n, b / n)


def main():
    ref, face = sys.argv[1], sys.argv[2]
    cx, cy = int(sys.argv[3]), int(sys.argv[4])
    rw, rh, rb, rr = decode(ref)
    fw, fh, fb, fr = decode(face)
    W, H = 46, 58
    # candidate A: box-downscale the whole face into the rect
    A = [[avg(fr, fb, x * fw // W, y * fh // H,
              max(x * fw // W + 1, (x + 1) * fw // W),
              max(y * fh // H + 1, (y + 1) * fh // H))
          for x in range(W)] for y in range(H)]
    # candidate B: nearest-neighbour scale
    B = [[px(fr, fb, x * fw // W, y * fh // H) for x in range(W)]
         for y in range(H)]
    # candidate C: centre crop at 1:1
    C = [[px(fr, fb, (fw - W) // 2 + x, (fh - H) // 2 + y) for x in range(W)]
         for y in range(H)]
    for name, cand in (('box-scale', A), ('nearest-scale', B), ('centre-crop', C)):
        tot = 0.0
        n = 0
        for y in range(H):
            for x in range(W):
                if cy + y >= rh or cx + x >= rw:
                    continue
                rp, gp, bp = px(rr, rb, cx + x, cy + y)
                cp0, cp1, cp2 = cand[y][x]
                tot += abs(rp - cp0) + abs(gp - cp1) + abs(bp - cp2)
                n += 1
        print('%-14s mean abs diff = %.2f over %d px' % (name, tot / (3 * n), n))


if __name__ == '__main__':
    main()
