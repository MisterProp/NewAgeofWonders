#!/usr/bin/env python3
"""Dump TMapObjectResource records (all field tags) from a Release.hss pack.

Usage: python dump_res.py <Release.hss> [class_hex] [limit]

The container is the generic Engine EObject graph (see src/include/aow/eobject.hpp):
  u32 region_off; region = file[region_off:]
  table: u8 n (bit 0x80 => u32 large-count follows), then n compact (u16 tag|off),
         then large (u32 tag, u32 off) entries

NOTE: redirect with ASCII/UTF-8. PowerShell's `*>` writes UTF-16LE, which
interleaves NUL bytes and makes the values unreadable:
    python dump_res.py Release.hss 0x20169 5 | Out-File -Encoding ascii out.txt
`aow mapres <Release.hss> <class_hex>` prints the same fields for a class.
"""
import struct, sys

def u32(b, o): return struct.unpack_from('<I', b, o)[0]
def u16(b, o): return struct.unpack_from('<H', b, o)[0]

def read_table(R, pos):
    if pos >= len(R): return None
    b = R[pos]; p = pos + 1
    large = 0
    if b & 0x80:
        small = b & 0x7F
        large = u32(R, p); p += 4
    else:
        small = b
    ents = []
    for _ in range(small):
        w = u16(R, p); p += 2
        ents.append((w & 0xFF, w >> 8))
    for _ in range(large):
        ents.append((u32(R, p), u32(R, p + 4))); p += 8
    return {'entries': ents, 'base': p}

def find(t, tag):
    if t is None: return None
    for tg, off in t['entries']:
        if tg == tag: return off
    return None

def u32_strings(buf):
    out = []; i = 0
    while i + 4 <= len(buf):
        ln = u32(buf, i)
        if 3 <= ln <= 64 and i + 4 + ln <= len(buf):
            s = buf[i+4:i+4+ln]
            if all(32 <= c < 127 for c in s):
                out.append(s.decode('ascii')); i += 4 + ln; continue
        i += 1
    return out

def main():
    path = sys.argv[1]
    want = int(sys.argv[2], 0) if len(sys.argv) > 2 and not sys.argv[2].startswith('--') else None
    data = open(path, 'rb').read()
    roff = u32(data, 0)
    R = data[roff:]
    outer = read_table(R, 0)
    top = read_table(R, find(outer, 0)) if find(outer, 0) is not None else None
    # top object sits at outer.base + off0
    off0 = find(outer, 0)
    topo = outer['base'] + off0
    top = read_table(R, topo)
    t5 = find(top, 5)
    listt = read_table(R, top['base'] + t5)
    ch = read_table(R, listt['base'] + find(listt, 1))
    n = len(ch['entries'])
    print(f"region_off={roff} children={n}")
    byc = {}
    for i in range(n):
        a = ch['base'] + ch['entries'][i][1]
        nxt = ch['base'] + ch['entries'][i+1][1] if i+1 < n else len(R)
        cid = u32(R, a)
        sub = read_table(R, a + 4)
        rec = {'class': cid, 'off': a, 'tags': {}}
        for tg, o in sub['entries']:
            rec['tags'][tg] = sub['base'] + o
        rec['buf'] = R[a:nxt]
        byc.setdefault(cid, []).append(rec)
    for cid in sorted(byc):
        if want is not None and cid != want: continue
        recs = byc[cid]
        tagsets = sorted(set(t for r in recs for t in r['tags']))
        print(f"\nclass 0x{cid:05x}  n={len(recs)}  tags={tagsets}")
        lim = int(sys.argv[3]) if len(sys.argv) > 3 else 200
        for r in recs[:lim]:
            vals = []
            for tg in tagsets:
                o = r['tags'].get(tg)
                if o is None: vals.append(f"{tg}=--"); continue
                if tg in (1, 2, 3, 12, 13, 14, 15) or (tg in (4,5,6,7,8,9,10,11) and r['buf'][o:o+4]):
                    v = u32(R, o)
                    vals.append(f"{tg}={v}(0x{v:08x})")
                else:
                    vals.append(f"{tg}=?")
            strs = [s for s in u32_strings(r['buf']) if '.' in s]
            print(f"  off=0x{r['off']:x} " + " ".join(vals) + ("  " + ",".join(strs[:2]) if strs else ""))

main()
