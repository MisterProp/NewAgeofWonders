#!/usr/bin/env python3
"""Resolve Tutorial level 0's field terrain/overlay exactly like the engine
(TMapField.UpdateTerrainType/UpdateOverlayType: topmost sprite with an opinion wins,
8-bit signed, 0x80..0xFF = no opinion) and report the resulting histogram plus sample
forest (raw overlay 1) / mountain (raw overlay 0) hexes."""
import struct, sys, collections

def u32(b, o): return struct.unpack_from('<I', b, o)[0]
def u16(b, o): return struct.unpack_from('<H', b, o)[0]

def read_table(R, pos):
    if pos >= len(R): return None
    b = R[pos]; p = pos + 1
    large = 0
    if b & 0x80:
        small = b & 0x7F; large = u32(R, p); p += 4
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

def load_resources(path):
    data = open(path, 'rb').read()
    R = data[u32(data, 0):]
    outer = read_table(R, 0)
    top = read_table(R, outer['base'] + find(outer, 0))
    listt = read_table(R, top['base'] + find(top, 5))
    ch = read_table(R, listt['base'] + find(listt, 1))
    res = {}
    n = len(ch['entries'])
    for i in range(n):
        a = ch['base'] + ch['entries'][i][1]
        nxt = ch['base'] + ch['entries'][i+1][1] if i+1 < n else len(R)
        cid = u32(R, a)
        sub = read_table(R, a + 4)
        if sub is None: continue
        o1 = find(sub, 1)
        if o1 is None: continue
        rid = u32(R, sub['base'] + o1)
        o7 = find(sub, 7)
        terr = ov = None
        if o7 is not None:
            o = sub['base'] + o7
            cnt = u32(R, o)
            if 1 <= cnt <= 4096 and o + 4 + cnt*2 <= len(R):
                terr = R[o+4]; ov = R[o+5]
        names = [s for s in u32_strings(R[a:nxt]) if '.' in s]
        res[rid] = (cid, terr, ov, names[0] if names else '')
    return res

def main():
    map_path, hss = sys.argv[1], sys.argv[2]
    res = load_resources(hss)
    raw = open(map_path, 'rb').read()
    R = raw[u32(raw, 0):] if raw[:4] != b'CFS\x00' else None
    if R is None:
        import zlib
        R = zlib.decompress(raw[5:])
        R = R[u32(R, 0):]
    outer = read_table(R, 0)
    top = read_table(R, outer['base'] + find(outer, 0))
    cont = read_table(R, top['base'] + find(top, 1))
    lvls = []
    for tag, off in cont['entries']:
        a = cont['base'] + off
        cid = u32(R, a)
        if cid != 0x10103: continue
        ct = read_table(R, a + 4)
        lvl = read_table(R, ct['base'] + find(ct, 1))
        lvls.append((lvl, ct))
    lvl, ct = lvls[0]
    for tag, off in lvl['entries']:
        a = lvl['base'] + off
        if u32(R, a) not in (0x10102, 0x20013): continue
        lt = read_table(R, a + 4)
        w = R[lt['base'] + find(lt, 3)]
        h = R[lt['base'] + find(lt, 4)]
        start = lt['base'] + find(lt, 6)
        end = lt['base'] + find(lt, 1)
        print('level class=0x%x %dx%d' % (u32(R, a), w, h))
        hist = collections.Counter()
        samples = collections.defaultdict(list)
        p = start
        n = 0
        while p + 4 <= end:
            ln = u32(R, p)
            if ln == 0 or p + 4 + ln > end: break
            body = p + 4
            ft = read_table(R, body)
            terr = ov = -1
            objs = []
            for tg, o in ft['entries']:
                if tg < 50: continue
                oa = ft['base'] + o
                ot = read_table(R, oa + 4)
                if ot is None: continue
                r1 = find(ot, 1)
                if r1 is None: continue
                objs.append((tg, u32(R, ot['base'] + r1)))
            for tg, rid in reversed(objs):      # topmost first
                r = res.get(rid)
                if r is None: continue
                cid, rt, ro, nm = r
                if rt is None: continue
                rt = rt - 256 if rt >= 128 else rt
                ro = ro - 256 if ro >= 128 else ro
                if terr < 0 and rt >= 0: terr = rt
                if ov < 0 and ro >= 0: ov = ro
                objs_names = nm
            x, y = n % w, n // w
            hist[(terr, ov)] += 1
            if ov in (0, 1, 2, 4, 5) and len(samples[(terr, ov)]) < 3:
                names = []
                for tg, rid in objs:
                    if rid in res: names.append('%d:%s' % (tg, res[rid][3]))
                samples[(terr, ov)].append((x, y, names))
            n += 1
            p = body + ln
        print('tiles=%d' % n)
        for k in sorted(hist, key=lambda k: (-hist[k], k)):
            print('  terrain=%-3d overlay=%-3d  n=%-5d  %s' %
                  (k[0], k[1], hist[k],
                   '; '.join('(%d,%d)[%s]' % (s[0], s[1], ','.join(s[2])) for s in samples.get(k, [])[:2])))
        return

main()
