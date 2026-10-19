import sys, zlib, struct
sys.path.insert(0, '.')
import _peek

def save(path, w, h, rgba):
    raw = bytearray()
    for y in range(h):
        raw.append(0)
        raw += rgba[y*w*4:(y+1)*w*4]
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
    w, h, out = _peek.load(src)
    ow, oh = cw*sc, ch*sc
    o = bytearray(ow*oh*4)
    for y in range(oh):
        for x in range(ow):
            sx, sy = x0 + x//sc, y0 + y//sc
            si = (sy*w + sx)*4
            di = (y*ow + x)*4
            o[di:di+4] = out[si:si+4]
    save(dst, ow, oh, bytes(o))
    print('wrote', dst, ow, oh)

main()
