"""tmp_zoom.py - decode any PNG (all filters), crop, nearest-upscale, rewrite.

    python tmp_zoom.py <in.png> <out.png> <x> <y> <w> <h> <scale>
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
    for pos in range(8, len(data)):
        pass
    pos = 8
    while pos + 8 <= len(data):
        (length,) = struct.unpack_from('>I', data, pos)
        ctype = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + length]
        if ctype == b'IHDR':
            w, h, depth, color, _c, _f, _i = struct.unpack('>IIBBBBB', body)
            if depth != 8:
                raise SystemExit('unsupported depth')
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
        elif f != 0:
            raise SystemExit('bad filter %d' % f)
        rows.append(bytes(line))
        prev = line
    return w, h, bpp, rows


def write_png(path, w, h, bpp, rows):
    color = 6 if bpp == 4 else 2
    raw = b''.join(b'\x00' + r for r in rows)
    def chunk(t, body):
        return (struct.pack('>I', len(body)) + t + body +
                struct.pack('>I', zlib.crc32(t + body) & 0xFFFFFFFF))
    out = (b'\x89PNG\r\n\x1a\n' +
           chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, color, 0, 0, 0)) +
           chunk(b'IDAT', zlib.compress(raw, 9)) +
           chunk(b'IEND', b''))
    open(path, 'wb').write(out)


def main():
    src, dst = sys.argv[1], sys.argv[2]
    x, y, w, h, sc = (int(v) for v in sys.argv[3:8])
    sw, sh, bpp, rows = decode(src)
    x1, y1 = min(sw, x + w), min(sh, y + h)
    out = []
    for yy in range(y, y1):
        line = bytearray()
        for xx in range(x, x1):
            line += rows[yy][xx * bpp:(xx + 1) * bpp] * sc
        for _ in range(sc):
            out.append(bytes(line))
    write_png(dst, (x1 - x) * sc, (y1 - y) * sc, bpp, out)
    print('%s %dx%d -> %s %dx%d (crop +%d+%d x%d)' %
          (src, sw, sh, dst, (x1 - x) * sc, (y1 - y) * sc, x, y, sc))


if __name__ == '__main__':
    main()
