import zlib, struct, collections, sys

def load(path):
    d = open(path, 'rb').read()
    p = 8
    idat = b''
    w = h = 0
    while p < len(d):
        ln = struct.unpack('>I', d[p:p+4])[0]
        t = d[p+4:p+8]
        c = d[p+8:p+8+ln]
        p += 12 + ln
        if t == b'IHDR':
            w, h = struct.unpack('>II', c[:8])
        if t == b'IDAT':
            idat += c
    raw = zlib.decompress(idat)
    bpp = 4
    stride = w * bpp
    out = bytearray()
    prev = bytearray(stride)
    q = 0
    for y in range(h):
        f = raw[q]; q += 1
        line = bytearray(raw[q:q+stride]); q += stride
        for i in range(stride):
            a = line[i-bpp] if i >= bpp else 0
            b = prev[i]
            cc = prev[i-bpp] if i >= bpp else 0
            if f == 1:
                line[i] = (line[i] + a) & 255
            elif f == 2:
                line[i] = (line[i] + b) & 255
            elif f == 3:
                line[i] = (line[i] + (a + b) // 2) & 255
            elif f == 4:
                pa = abs(b - cc); pb = abs(a - cc); pc = abs(a + b - 2 * cc)
                pr = a if pa <= pb and pa <= pc else (b if pb <= pc else cc)
                line[i] = (line[i] + pr) & 255
        out += line
        prev = line
    return w, h, out

if __name__ == '__main__':
    w, h, out = load(sys.argv[1])
    cnt = collections.Counter(bytes(out[i:i+3]) for i in range(0, len(out), 4))
    print('size', w, h)
    for c, n in cnt.most_common(10):
        print('  rgb(%d,%d,%d)' % (c[0], c[1], c[2]), n)
