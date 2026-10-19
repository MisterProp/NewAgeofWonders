#!/usr/bin/env python3
"""structseq_probe.py - dump the image-sequence list (rw tags 9/10) of every
TMapObjectResource in a Release.hss pack.

    python tools/structseq_probe.py <Release.hss> [class_hex]

Reconstructs ImageLib.TImageSequenceList.LoadFromStream@0x552190d8 +
ImageLib.TImageSequence.LoadFromStream@0x55218a60 (see
docs/16_UNIT_ANIM.md / re/exports/anim/ILPACK_dpl/):
  list: u32 count ; per entry u8 flag (0 => a TImageSequence follows, else NULL)
  seq : rwString lib(+0x14) ; u8 +0x08 ; u8 period(+0x18) ; u8 mode(+0x19)
        u32 base frame(+0x20) ; i32 xoff(+0x24) ; i32 yoff(+0x28)
        if (mode-4)<2: TFrameTableList (u32 seed, u32 count, count*i32)
        u8 has_chain ; if set, recurse

This is how the *building* animations were found: a structure's sequence mode is
0 (static) / 1 (loop) / 2 (ping-pong) / 4 (frame table), exactly like a unit pose.
"""
import struct
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')


def u32(b, o):
    return struct.unpack_from('<I', b, o)[0]


def i32(b, o):
    return struct.unpack_from('<i', b, o)[0]


def read_table(R, pos):
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
        w = struct.unpack_from('<H', R, p)[0]
        p += 2
        ents.append((w & 0xFF, w >> 8))
    for _ in range(large):
        ents.append((u32(R, p), u32(R, p + 4)))
        p += 8
    return {'entries': ents, 'base': p}


def find(t, tag):
    for tg, off in t['entries']:
        if tg == tag:
            return off
    return None


class Reader:
    def __init__(self, b, o=0):
        self.b, self.o = b, o

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

    def rws(self):
        n = self.u32()
        s = self.b[self.o:self.o + n].decode('latin-1')
        self.o += n
        return s


def read_seq(r):
    lib = r.rws()
    x08 = r.u8()
    period = r.u8()
    mode = r.u8()
    frame = r.u32()
    xoff = r.i32()
    yoff = r.i32()
    ft = None
    if (mode - 4) & 0xFF < 2:
        seed = r.u32()
        n = r.u32()
        ft = [r.i32() for _ in range(n)]
        frame = -1
    chain_present = r.u8()
    chain = read_seq(r) if chain_present else None
    return dict(lib=lib, x08=x08, period=period, mode=mode, frame=frame,
                xoff=xoff, yoff=yoff, ft=ft, chain=chain)


def fmt(s, ind):
    ft = '-' if s['ft'] is None else 'ft[n=%d %s]' % (len(s['ft']), s['ft'])
    out = ('%s[mode=%d per=%d base=%d xoff=%d yoff=%d %s] %s\n'
           % (ind, s['mode'], s['period'], s['frame'], s['xoff'], s['yoff'],
              ft, s['lib']))
    if s['chain']:
        out += fmt(s['chain'], ind + '    -> ')
    return out


def chain_modes(s):
    out = ['m%d/%s' % (s['mode'], s['lib'])]
    if s['chain']:
        out += chain_modes(s['chain'])
    return out


def main():
    path = sys.argv[1]
    summary = '--summary' in sys.argv
    want = None
    i = 2
    while i < len(sys.argv):
        a = sys.argv[i]
        if a == '--out':
            sys.stdout = open(sys.argv[i + 1], 'w', encoding='utf-8')
            i += 2
            continue
        if not a.startswith('--'):
            want = int(a, 0)
        i += 1
    data = open(path, 'rb').read()
    R = data[u32(data, 0):]
    outer = read_table(R, 0)
    topo = outer['base'] + find(outer, 0)
    top = read_table(R, topo)
    listt = read_table(R, top['base'] + find(top, 5))
    ch = read_table(R, listt['base'] + find(listt, 1))
    n = len(ch['entries'])
    print('children=%d' % n)
    for i in range(n):
        a = ch['base'] + ch['entries'][i][1]
        nxt = ch['base'] + ch['entries'][i + 1][1] if i + 1 < n else len(R)
        cid = u32(R, a)
        if want is not None and cid != want:
            continue
        sub = read_table(R, a + 4)
        o1 = find(sub, 1)
        rid = u32(R, sub['base'] + o1) if o1 is not None else 0
        o9 = find(sub, 9)
        if o9 is None:
            o9 = find(sub, 10)
        if o9 is None:
            continue
        head = sub['base'] + o9
        r = Reader(R, head)
        count = r.u32()
        seqs = []
        for _ in range(count):
            if r.u8() != 0:
                seqs.append(None)
                continue
            seqs.append(read_seq(r))
        if summary:
            modes = []
            for s in seqs:
                if s is not None:
                    modes += chain_modes(s)
            animated = any(not m.startswith('m0/') for m in modes)
            print('class 0x%05x id=0x%08x n=%2d %s %s'
                  % (cid, rid, count, 'ANIM' if animated else 'stat',
                     ' '.join(modes)))
            continue
        print('\nclass 0x%05x id=0x%08x count=%d' % (cid, rid, count))
        for j, s in enumerate(seqs):
            if s is None:
                print('  [%2d] <null>' % j)
            else:
                sys.stdout.write('  [%2d] %s' % (j, fmt(s, '')))


if __name__ == '__main__':
    main()
