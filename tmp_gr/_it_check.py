"""_it_check.py - decode an IT pattern and report anomalies."""
import struct
import sys


def decode(d, off):
    length, rows = struct.unpack_from('<HH', d, off)
    p = off + 8
    end = off + 8 + length
    bad = {'note': 0, 'instr': 0, 'cmd': 0}
    r = 0
    hit_end = False
    while r < rows:
        if p >= end:
            hit_end = True
            break
        c = 0
        while p < end:
            v = d[p]; p += 1
            if v == 0:
                break
            if v & 1:
                note = d[p]; p += 1
                if note > 119 and note not in (254, 255):
                    bad['note'] += 1
            if (v & 2) or (v & 16):
                ins = d[p]; p += 1
                if ins > 120:
                    bad['instr'] += 1
            if (v & 4) or (v & 32):
                p += 1
            if v & 8:
                cmd = d[p]; p += 1
                info = d[p]; p += 1
                if cmd > 31:
                    bad['cmd'] += 1
                _ = info
            c += 1
        r += 1
    return rows, length, p - (off + 8), r, hit_end, bad


if __name__ == '__main__':
    path = sys.argv[1]
    pi = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    d = open(path, 'rb').read()
    on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
    po = 0xC0 + on + inn * 4 + sn * 4
    off = struct.unpack_from('<I', d, po + pi * 4)[0]
    rows, length, used, parsed, hit_end, bad = decode(d, off)
    print('%-14s pat%-3d rows=%-3d parsed=%-3d len=%-5d used=%-5d hit_end=%d bad=%s'
          % (path.split('\\')[-1], pi, rows, parsed, length, used, hit_end, bad))
