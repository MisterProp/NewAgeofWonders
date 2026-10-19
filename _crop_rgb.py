import sys, zlib, struct


def load_rgb(path):
    d = open(path, 'rb').read()
    p = 8
    idat = b''
    w = h = 0
    bpp = 3
    while p < len(d):
        ln = struct.unpack('>I', d[p:p + 4])[0]
        t = d[p + 4:p + 8]
        c = d[p + 8:p + 8 + ln]
        p += 12 + ln
        if t == b'IHDR':
            w, h, bd, ct = struct.unpack('>IIBB', c[:10])
            bpp = {0: 1, 2: 3, 4: 2, 6: 4}[ct]
        if t == b'IDAT':
            idat += c
    raw = zlib.decompress(idat)
    stride = w * bpp
    out = bytearray()
    prev = bytearray(stride)
    q = 0
    for y in range(h):
        f = raw[q]
        q += 1
        line = bytearray(raw[q:q + stride])
        q += stride
        for i in range(stride):
            a = line[i - bpp] if i >= bpp else 0
            b = prev[i]
            cc = prev[i - bpp] if i >= bpp else 0
            if f == 1:
                line[i] = (line[i] + a) & 255
            elif f == 2:
                line[i] = (line[i] + b) & 255
            elif f == 3:
                line[i] = (line[i] + (a + b) // 2) & 255
            elif f == 4:
                pa = abs(b - cc)
                pb = abs(a - cc)
                pc = abs(a + b - 2 * cc)
                pr = a if pa <= pb and pa <= pc else (b if pb <= pc else cc)
                line[i] = (line[i] + pr) & 255
        out += line
        prev = line
    return w, h, bpp, out


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
    src, dst, x0, y0, cw, ch, sc = sys.argv[1:8]
    x0, y0, cw, ch, sc = int(x0), int(y0), int(cw), int(ch), int(sc)
    w, h, bpp, src_px = load_rgb(src)
    out = bytearray(cw * sc * ch * sc * 4)
    ow = cw * sc
    for y in range(ch * sc):
        for x in range(ow):
            sx, sy = x0 + x // sc, y0 + y // sc
            if sx >= w:
                sx = w - 1
            if sy >= h:
                sy = h - 1
            si = (sy * w + sx) * bpp
            di = (y * ow + x) * 4
            out[di] = src_px[si]
            out[di + 1] = src_px[si + 1]
            out[di + 2] = src_px[si + 2]
            out[di + 3] = 255
    save(dst, ow, ch * sc, bytes(out))
    print('wrote', dst, ow, ch * sc)


if __name__ == '__main__':
    main()
