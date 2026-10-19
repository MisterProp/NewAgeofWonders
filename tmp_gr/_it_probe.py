"""_it_probe.py - dump Impulse Tracker (.IT) header facts for AoW songs."""
import glob
import struct
import sys


def probe(path):
    d = open(path, 'rb').read()
    sig = d[:4]
    if sig != b'IMPM':
        print('%-40s NOT IT (sig=%r, %d bytes)' % (path, sig, len(d)))
        return
    (ordnum, insnum, smpnum, patnum) = struct.unpack_from('<4H', d, 0x20)
    (cwtv, cmwt, flags, special) = struct.unpack_from('<4H', d, 0x28)
    gv, mv, is_, it, sep, pwd = struct.unpack_from('<6B', d, 0x30)
    msg_len = struct.unpack_from('<H', d, 0x36)[0]
    msg_off = struct.unpack_from('<I', d, 0x38)[0]
    print('%-16s v%04x/%04x ord=%-3d ins=%-3d smp=%-3d pat=%-3d '
          'flags=%04x spec=%04x GV=%d MV=%d IS=%d IT=%d' % (
              path.split('\\')[-1], cwtv, cmwt, ordnum, insnum, smpnum,
              patnum, flags, special, gv, mv, is_, it))
    print('    flags: stereo=%d vol0mix=%d instr=%d linear=%d oldFX=%d '
          'link=%d midi=%d' % (
              flags & 1, (flags >> 1) & 1, (flags >> 2) & 1,
              (flags >> 3) & 1, (flags >> 4) & 1, (flags >> 5) & 1,
              (flags >> 6) & 1))
    print('    msg_off=%d msg_len=%d  titles: %r' % (
        msg_off, msg_len, d[4:4 + 26].split(b'\0')[0]))


if __name__ == '__main__':
    pats = sys.argv[1:] or ['Songs\\*.it', 'Songs\\*\\*.it']
    for pat in pats:
        for f in sorted(glob.glob(pat)):
            probe(f)
