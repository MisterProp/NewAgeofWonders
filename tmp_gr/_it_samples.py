"""_it_samples.py - report IT sample flags (compression/16-bit/loops)."""
import glob
import struct
import sys

HDR = 0xC0  # IT header size


def probe(path):
    d = open(path, 'rb').read()
    if d[:4] != b'IMPM':
        return
    ordnum, insnum, smpnum, patnum = struct.unpack_from('<4H', d, 0x20)
    flags = struct.unpack_from('<H', d, 0x2c)[0]
    linear = (flags >> 3) & 1
    instr = (flags >> 2) & 1
    off = HDR + ordnum + insnum * 4  # sample offset table
    cmpset = {}
    n16 = 0
    ncomp = 0
    nloop = 0
    for i in range(smpnum):
        so = off + i * 4
        samp_off = struct.unpack_from('<I', d, so)[0]
        if samp_off == 0 or samp_off + 80 > len(d):
            continue
        flg = d[samp_off + 0x12]
        conv = d[samp_off + 0x2E]
        length = struct.unpack_from('<I', d, samp_off + 0x30)[0]
        cmpset[conv] = cmpset.get(conv, 0) + 1
        if flg & 2:
            n16 += 1
        if flg & 8:
            ncomp += 1
        if flg & 0x10:
            nloop += 1
        _ = length
    print('%-14s linear=%d instr=%d smp=%d  convert=%s  16bit=%d comp=%d loop=%d'
          % (path.split('\\')[-1], linear, instr, smpnum,
             dict(sorted(cmpset.items())), n16, ncomp, nloop))


if __name__ == '__main__':
    for pat in (sys.argv[1:] or ['Songs\\*.it', 'Songs\\*\\*.it']):
        for f in sorted(glob.glob(pat)):
            probe(f)
