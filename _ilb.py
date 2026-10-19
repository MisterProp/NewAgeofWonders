import struct
import sys
from PIL import Image

PATH = r'../Images/General.ILB'
data = open(PATH, 'rb').read()
print('file size', len(data), 'magic', data[:4])


def u32(o):
    return struct.unpack_from('<I', data, o)[0]


def i32(o):
    return struct.unpack_from('<i', data, o)[0]


def u16(o):
    return struct.unpack_from('<H', data, o)[0]


hdr = u32(0x0C)
dir_end = u32(0x10)
print('hdr', hdr, 'data_off', dir_end)
dirb = data[hdr:dir_end]
dir_size = len(dirb)


def du32(o):
    return struct.unpack_from('<I', dirb, o)[0]


def di32(o):
    return struct.unpack_from('<i', dirb, o)[0]


pcount = du32(0)
p = 4 + pcount * (4 + 1024)
print('palette_count', pcount)


def printable(b):
    return all(32 <= c < 127 for c in b)


def sig(off):
    if off + 17 > dir_size:
        return None
    cls = du32(off + 4)
    if cls < 1 or cls > 64:
        return None
    fmt = dirb[off + 8]
    if fmt > 3:
        return None
    nl = du32(off + 9)
    if nl < 1 or nl > 300 or off + 13 + nl > dir_size:
        return None
    if not printable(dirb[off + 13:off + 13 + nl]):
        return None
    return (du32(off), cls, fmt, nl)


blob = dir_end
entries = []
while True:
    s = sig(p)
    if s is None:
        break
    index, cls, fmt, nl = s
    if index == 0xFFFFFFFF:
        break
    q = p + 13 + nl
    width = du32(q)
    height = du32(q + 4)
    base_x = di32(q + 8)
    base_y = di32(q + 12)
    data_size = du32(q + 21)
    rle_w = rle_h = xshift = yshift = 0
    if cls == 0x16 and data_size:
        c = q + 20 + 1 + 4
        if fmt > 1:
            c += 12
        if fmt > 2:
            c += 8
        c += 4
        rle_w = du32(c)
        rle_h = du32(c + 4)
        xshift = di32(c + 8)
        yshift = di32(c + 12)
    e = dict(index=index, cls=cls, fmt=fmt, w=width, h=height,
             base=(base_x, base_y), size=data_size, rle=(rle_w, rle_h),
             shift=(xshift, yshift), blob=blob, name=dirb[p + 13:p + 13 + nl].decode())
    entries.append(e)
    blob += data_size
    # find next record
    nxt = None
    for o in range(q, dir_size - 16):
        if sig(o):
            nxt = o
            break
    if nxt is None:
        break
    p = nxt

for e in entries:
    print(e['index'], hex(e['cls']), e['name'], e['w'], 'x', e['h'],
          'rle', e['rle'], 'shift', e['shift'], 'size', e['size'],
          'blob', e['blob'])

# decode entries 6 and 7 both ways
def decode(e, mode):
    w, h = e['rle']
    off = e['blob']
    out = bytearray(w * h * 4)
    for i in range(w * h):
        v = u16(off + i * 2)
        if mode == 565:
            r = ((v >> 11) & 0x1F) * 255 // 31
            g = ((v >> 5) & 0x3F) * 255 // 63
            b = (v & 0x1F) * 255 // 31
        elif mode == 555:
            r = ((v >> 10) & 0x1F) * 255 // 31
            g = ((v >> 5) & 0x1F) * 255 // 31
            b = (v & 0x1F) * 255 // 31
        out[i * 4:i * 4 + 4] = bytes((r, g, b, 255))
    return out


for e in entries:
    if e['index'] in (6, 7):
        for mode, tag in ((565, '565'), (555, '555')):
            rgba = decode(e, mode)
            im = Image.frombytes('RGBA', e['rle'], bytes(rgba))
            im.save('tmp_py_%d_%s.png' % (e['index'], tag))
            print('saved tmp_py_%d_%s.png' % (e['index'], tag))