#!/usr/bin/env python3
"""List the TMapObjectResource classes whose records name an image library.

Usage: python cls_ilb.py <Release.hss> [substring]

Prints `class 0xNNNNN n=<count> ilbs=[...]` for every class whose records embed a
library name containing `substring` (default: MOUNTAIN).
"""
import struct, sys

def u32(b, o): return struct.unpack_from('<I', b, o)[0]
def u16(b, o): return struct.unpack_from('<H', b, o)[0]

def rt(R, pos):
    b = R[pos]; p = pos + 1; large = 0
    if b & 0x80:
        small = b & 0x7F; large = u32(R, p); p += 4
    else:
        small = b
    e = []
    for _ in range(small):
        w = u16(R, p); p += 2; e.append((w & 0xFF, w >> 8))
    for _ in range(large):
        e.append((u32(R, p), u32(R, p + 4))); p += 8
    return {'e': e, 'base': p}

def f(t, tag):
    for a, b in t['e']:
        if a == tag: return b
    return None

def strs(buf):
    o = []; i = 0
    while i + 4 <= len(buf):
        ln = u32(buf, i)
        if 3 <= ln <= 64 and i + 4 + ln <= len(buf) and all(32 <= c < 127 for c in buf[i+4:i+4+ln]):
            o.append(buf[i+4:i+4+ln].decode()); i += 4 + ln; continue
        i += 1
    return o

def main():
    path = sys.argv[1]
    want = (sys.argv[2] if len(sys.argv) > 2 else 'MOUNTAIN').upper()
    d = open(path, 'rb').read()
    R = d[u32(d, 0):]
    outer = rt(R, 0)
    top = rt(R, outer['base'] + f(outer, 0))
    lst = rt(R, top['base'] + f(top, 5))
    ch = rt(R, lst['base'] + f(lst, 1))
    n = len(ch['e']); byc = {}
    for i in range(n):
        a = ch['base'] + ch['e'][i][1]
        nx = ch['base'] + ch['e'][i+1][1] if i+1 < n else len(R)
        byc.setdefault(u32(R, a), []).append((a, R[a:nx]))
    for cid in sorted(byc):
        recs = byc[cid]
        ilbs = set()
        for _, buf in recs:
            for s in strs(buf):
                if s.upper().endswith('.ILB') or s.upper().endswith('.BMP'):
                    ilbs.add(s)
        if any(want in x.upper() for x in ilbs):
            ids = sorted({u32(R, rt(R, a + 4)['base'] + f(rt(R, a + 4), 1))
                          for a, _ in recs if f(rt(R, a + 4), 1) is not None})
            print("class 0x%05x n=%d ilbs=%s" % (cid, len(recs), sorted(ilbs)))
            print("   ids=%s" % [hex(i) for i in ids])

main()
