"""png_colors.py - inspect a PNG written by the port (aow/png.hpp).

That writer uses *stored* DEFLATE blocks and filter 0 on every scanline, so a
PNG it produced can be decoded with nothing but the standard library. Handy for
checking sprite palettes/alpha without pulling in Pillow.

    python tools/png_colors.py re/png/cur/id0.png
    python tools/png_colors.py re/png/cur/id0.png --top 8
"""
import collections
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
        filt = raw[off]
        if filt != 0:
            raise SystemExit('unexpected filter %d' % filt)
        rows.append(raw[off + 1:off + 1 + stride])
    return w, h, bpp, rows


def main():
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    path = sys.argv[1]
    top = 10
    if '--top' in sys.argv:
        top = int(sys.argv[sys.argv.index('--top') + 1])
    w, h, bpp, rows = decode(path)
    print('%s: %dx%d, %d bytes/px' % (path, w, h, bpp))
    hist = collections.Counter()
    for row in rows:
        for x in range(0, len(row), bpp):
            px = row[x:x + bpp]
            if bpp == 4:
                hist[(px[0], px[1], px[2], px[3])] += 1
            else:
                hist[(px[0], px[1], px[2])] += 1
    for colour, n in hist.most_common(top):
        if len(colour) == 4:
            print('   rgba(%3d,%3d,%3d,%3d)  %6d px' % (colour + (n,)))
        else:
            print('   rgb(%3d,%3d,%3d)        %6d px' % (colour + (n,)))
    print('   distinct colours: %d' % len(hist))

    if '--pixel' in sys.argv:
        i = sys.argv.index('--pixel')
        px = int(sys.argv[i + 1])
        py = int(sys.argv[i + 2])
        row = rows[py]
        p = row[px * bpp:(px + 1) * bpp]
        print('   pixel(%d,%d) = %s' % (px, py, tuple(p)))
    if '--gray' in sys.argv:
        # Fine luminance ramp, to reveal gradients inside a glyph.
        ramp = ' .:-=+*#@'
        for y in range(h):
            row = rows[y]
            line = []
            for x in range(0, len(row), bpp):
                px = row[x:x + bpp]
                if bpp == 4 and px[3] == 0:
                    line.append('.')
                    continue
                lum = (px[0] + px[1] + px[2]) // 3
                line.append(ramp[min(8, lum * 9 // 256)])
            print('   |' + ''.join(line) + '|')
        # One character per pixel so a glyph can be read without an image
        # viewer: '.' transparent, '#' white, '+' mid, 'o' dark, ' ' black.
        for y in range(h):
            row = rows[y]
            line = []
            for x in range(0, len(row), bpp):
                px = row[x:x + bpp]
                if bpp == 4 and px[3] == 0:
                    line.append('.')
                    continue
                lum = (px[0] + px[1] + px[2]) // 3
                if lum >= 200:
                    line.append('#')
                elif lum >= 128:
                    line.append('+')
                elif lum >= 48:
                    line.append('o')
                elif lum >= 8:
                    line.append(' ')
                else:
                    line.append(' ')
            print('   |' + ''.join(line) + '|')
    if '--border' in sys.argv:
        border = collections.Counter()
        for y in (0, h - 1):
            row = rows[y]
            for x in range(0, len(row), bpp):
                border[tuple(row[x:x + bpp])] += 1
        for y in range(h):
            row = rows[y]
            for x in (0, (w - 1) * bpp):
                border[tuple(row[x:x + bpp])] += 1
        print('   border:')
        for colour, n in border.most_common(4):
            print('      %s  %d px' % (colour, n))


if __name__ == '__main__':
    main()
