"""_it_validate.py - validate the IT sample-header field offsets by checking
that samplePointer[k] + length[k]*bytes == samplePointer[k+1] for every song.
"""
import glob
import struct
import sys


def check(path):
    d = open(path, 'rb').read()
    if d[:4] != b'IMPM':
        return
    ordnum, insnum, smpnum, patnum = struct.unpack_from('<4H', d, 0x20)
    base = 0xC0 + ordnum + insnum * 4
    offs = [struct.unpack_from('<I', d, base + i * 4)[0] for i in range(smpnum)]
    ptrs = []
    for k, so in enumerate(offs):
        if so == 0:
            continue
        flags = d[so + 0x12]
        if not (flags & 1):
            continue
        length = struct.unpack_from('<I', d, so + 0x30)[0]
        ptr = struct.unpack_from('<I', d, so + 0x48)[0]
        bytes_per = 2 if (flags & 2) else 1
        ptrs.append((k, ptr, length * bytes_per))
    ok = 0
    bad = 0
    for (k1, p1, n1), (k2, p2, n2) in zip(ptrs, ptrs[1:]):
        if p1 + n1 == p2:
            ok += 1
        else:
            bad += 1
    print('%-14s samples=%-4d contiguous=%d gap=%d  last_end=%d size=%d' % (
        path.split('\\')[-1], len(ptrs), ok, bad,
        (ptrs[-1][1] + ptrs[-1][2]) if ptrs else 0, len(d)))


if __name__ == '__main__':
    for pat in (sys.argv[1:] or ['Songs\\*.it', 'Songs\\*\\*.it']):
        for f in sorted(glob.glob(pat)):
            check(f)
