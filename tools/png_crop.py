"""png_crop.py - crop a PNG the port wrote (aow/png.hpp) to a smaller PNG.

    python tools/png_crop.py <in.png> <x> <y> <w> <h> <out.png>

The port's writer uses *stored* DEFLATE blocks and filter 0 on every scanline, so
the input decodes with the standard library alone (see tools/png_colors.py); the
output is written the same way (filter 0 + zlib). Handy for zooming into a big
map render - e.g. a single building - without an image viewer.
"""
import struct
import sys
import zlib


def read_chunks(data):
    pos = 8
    while pos + 8 <= len(data):
        (length,) = struct.unpack_from('>I', data, pos)
        ctype = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + length]
        yield ctype, body
        pos += 12 + length


def decode(path):
    data = open(path, 'rb').read()
    w = h = bpp = None
    idat = b''
    for ctype, body in read_chunks(data):
        if ctype == b'IHDR':
            w, h, depth, color, _c, _f, _i = struct.unpack('>IIBBBBB', body)
            bpp = 4 if color == 6 else 3
            if depth != 8 or color not in (2, 6):
                raise SystemExit('unsupported PNG (depth=%d color=%d)'
                                 % (depth, color))
        elif ctype == b'IDAT':
            idat += body
    raw = zlib.decompress(idat)
    stride = w * bpp
    rows = []
    for y in range(h):
        off = y * (stride + 1)
        if raw[off] != 0:
            raise SystemExit('unexpected filter %d' % raw[off])
        rows.append(raw[off + 1:off + 1 + stride])
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
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    src, x, y, w, h, dst = (sys.argv[1], int(sys.argv[2]), int(sys.argv[3]),
                            int(sys.argv[4]), int(sys.argv[5]), sys.argv[6])
    sw, sh, bpp, rows = decode(src)
    x0 = max(0, min(x, sw))
    y0 = max(0, min(y, sh))
    x1 = max(x0, min(x + w, sw))
    y1 = max(y0, min(y + h, sh))
    cw, ch = x1 - x0, y1 - y0
    if cw <= 0 or ch <= 0:
        raise SystemExit('empty crop')
    out = []
    for yy in range(y0, y1):
        row = rows[yy]
        out.append(row[x0 * bpp:x1 * bpp])
    write_png(dst, cw, ch, bpp, out)
    print('%s: %dx%d -> %s: %dx%d (+%d+%d)'
          % (src, sw, sh, dst, cw, ch, x0, y0))


if __name__ == '__main__':
    main()
