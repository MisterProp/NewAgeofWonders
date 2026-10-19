import sys, zlib, struct
sys.path.insert(0, '.')
import _peek


def save(path, w, h, rgba):
    raw = bytearray()
    for y in range(h):
        raw.append(0)
        raw += rgba[y * w * 4:(y + 1) * w * 4]

    def chunk(t, d):
        c = t + d
        return struct.pack('>I', len(d)) + c + struct.pack('>I', zlib.crc32(c) & 0xffffffff)

    png = b'\x89PNG\r\n\x1a\n'
    png += chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0))
    png += chunk(b'IDAT', zlib.compress(bytes(raw), 9))
    png += chunk(b'IEND', b'')
    open(path, 'wb').write(png)


def main():
    src, dst, tgt = sys.argv[1:4]
    tgt = int(tgt)
    w, h, px = _peek.load(src)
    f = max(1, (w + tgt - 1) // tgt)
    nw, nh = w // f, h // f
    out = bytearray(nw * nh * 4)
    for y in range(nh):
        for x in range(nw):
            si = ((y * f) * w + x * f) * 4
            di = (y * nw + x) * 4
            out[di:di + 4] = px[si:si + 4]
    save(dst, nw, nh, bytes(out))
    print('wrote', dst, nw, nh)


main()
