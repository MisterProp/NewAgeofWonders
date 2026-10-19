"""tmp_seqdump.py - dump the TImageSequenceList (rw tag 10) of map-object
resources so we can see how a structure (Shipyard) picks its library.

    python tmp_seqdump.py <Release.hss> <class_hex> [...]
"""
import struct
import sys


def u32(b, o):
    return struct.unpack_from('<I', b, o)[0]


def i32(b, o):
    return struct.unpack_from('<i', b, o)[0]


def u16(b, o):
    return struct.unpack_from('<H', b, o)[0]


def read_table(R, pos):
    if pos >= len(R):
        return None
    b = R[pos]
    p = pos + 1
    large = 0
    if b & 0x80:
        small = b & 0x7F
        large = u32(R, p)
        p += 4
    else:
        small = b
    ents = []
    for _ in range(small):
        w = u16(R, p)
        p += 2
        ents.append((w & 0xFF, w >> 8))
    for _ in range(large):
        ents.append((u32(R, p), u32(R, p + 4)))
        p += 8
    return {'entries': ents, 'base': p}


def find(t, tag):
    if t is None:
        return None
    for tg, off in t['entries']:
        if tg == tag:
            return off
    return None


class Reader:
    def __init__(self, b, o):
        self.b = b
        self.o = o

    def u8(self):
        v = self.b[self.o]
        self.o += 1
        return v

    def u32(self):
        v = u32(self.b, self.o)
        self.o += 4
        return v

    def i32(self):
        v = i32(self.b, self.o)
        self.o += 4
        return v

    def s(self):
        n = u32(self.b, self.o)
        self.o += 4
        v = self.b[self.o:self.o + n].decode('latin-1')
        self.o += n
        return v


def parse_seq(r, depth=0):
    lib = r.s()
    res = r.u8()
    per = r.u8()
    mode = r.u8()
    base = r.i32()
    xo = r.i32()
    yo = r.i32()
    table = []
    if ((mode - 4) & 0xFF) < 2:
        seed = r.u32()
        n = r.u32()
        table = [r.i32() for _ in range(n)]
    chain = r.u8()
    print('    %sseq lib=%-16s res=%-3d per=%-3d mode=%-3d base=%-5d '
          'xoff=%-5d yoff=%-5d table=%s chain=%d' %
          ('  ' * depth, lib, res, per, mode, base, xo, yo, table, chain))
    if chain:
        parse_seq(r, depth + 1)


def dump_list(R, name, off):
    print('  -- %s @0x%x --' % (name, off))
    r = Reader(R, off)
    start = off
    n = r.u32()
    print('    count=%d' % n)
    if n > 512:
        print('    (implausible count)')
        return
    for i in range(n):
        flag = r.u8()
        if flag == 0:
            print('    [%d] flag=0 @+%d' % (i, r.o - start))
            parse_seq(r)
        else:
            print('    [%d] flag=%d (null) @+%d' % (i, flag, r.o - start))
    print('    consumed %d bytes' % (r.o - start))



def main():
    path = sys.argv[1]
    want = [int(c, 0) for c in sys.argv[2:]]
    data = open(path, 'rb').read()
    roff = u32(data, 0)
    R = data[roff:]
    outer = read_table(R, 0)
    topo = outer['base'] + find(outer, 0)
    top = read_table(R, topo)
    t5 = find(top, 5)
    lt = read_table(R, top['base'] + t5)
    ch = read_table(R, lt['base'] + find(lt, 1))
    n = len(ch['entries'])
    for i in range(n):
        a = ch['base'] + ch['entries'][i][1]
        cid = u32(R, a)
        if cid not in want:
            continue
        sub = read_table(R, a + 4)
        print('class 0x%05x off=0x%x tags=%s' %
              (cid, a, sorted(t for t, _ in sub['entries'])))
        for tag in (9, 10):
            o = find(sub, tag)
            if o is None:
                continue
            dump_list(R, 'tag %d' % tag, sub['base'] + o)
        print()


if __name__ == '__main__':
    main()
