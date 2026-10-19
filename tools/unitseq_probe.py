"""unitseq_probe.py - parse a Unitgfx.pfs tag-5 TImageSequenceList blob.

    python tools/unitseq_probe.py <Unitgfx.pfs> <file_off_hex> <size>

Reconstructs ImageLib.TImageSequenceList.LoadFromStream +
ImageLib.TImageSequence.LoadFromStream (ILPACK.dpl 0x552190d8 / 0x55218a60):
  list:  u32 count ; per entry u8 flag (0 => a TImageSequence follows)
  seq :  rwString lib(+0x14) ; u8 +0x08 ; u8 period(+0x18) ; u8 mode(+0x19)
         u32 frame(+0x20) ; u32 xoff(+0x24) ; u32 yoff(+0x28)
         if (mode-4)<2: a TFrameTableList ; u8 has_chain ; if chain: recurse
"""
import struct
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')


class R:
    def __init__(self, b, o=0):
        self.b = b
        self.o = o

    def u8(self):
        v = self.b[self.o]
        self.o += 1
        return v

    def u32(self):
        v = struct.unpack_from('<I', self.b, self.o)[0]
        self.o += 4
        return v

    def i32(self):
        v = struct.unpack_from('<i', self.b, self.o)[0]
        self.o += 4
        return v

    def rws(self):
        n = self.u32()
        s = self.b[self.o:self.o + n].decode('latin-1')
        self.o += n
        return s

    def frame_table(self):
        seed = self.u32()
        n = self.u32()
        vals = [self.i32() for _ in range(n)]
        return seed, vals


def read_seq(r):
    lib = r.rws()
    x08 = r.u8()
    period = r.u8()
    mode = r.u8()
    frame = r.u32()
    xoff = r.i32()
    yoff = r.i32()
    ft = None
    if (mode - 4) & 0xff < 2:
        ft = r.frame_table()
        frame = -1  # randomised at load
    chain_present = r.u8()
    chain = None
    if chain_present:
        chain = read_seq(r)
    return dict(lib=lib, x08=x08, period=period, mode=mode, frame=frame,
                xoff=xoff, yoff=yoff, ft=ft, chain=chain)


def main():
    path, off, size = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3], 0)
    blob = open(path, 'rb').read()[off:off + size]
    r = R(blob)
    count = r.u32()
    print('count=%d' % count)
    for i in range(count):
        flag = r.u8()
        if flag != 0:
            print('  [%2d] <null>' % i)
            continue
        s = read_seq(r)
        ft = s['ft']
        ftd = '-' if ft is None else ('seed=%d n=%d %s' % (ft[0], len(ft[1]), ft[1]))
        print('  [%2d] mode=%d per=%d base=%d xoff=%d yoff=%d x08=%d ft[%s] '
              'chain=%s  %s'
              % (i, s['mode'], s['period'], s['frame'], s['xoff'], s['yoff'],
                 s['x08'], ftd, 'yes' if s['chain'] else 'no', s['lib']))
    print('bytes consumed=%d of %d' % (r.o, len(blob)))


if __name__ == '__main__':
    main()