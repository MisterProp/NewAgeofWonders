import struct, sys

def u16(b,o): return b[o] | (b[o+1]<<8)
def u32(b,o): return b[o] | (b[o+1]<<8) | (b[o+2]<<16) | (b[o+3]<<24)

def read_table(R, n, pos):
    if pos >= n: return None
    b = R[pos]
    p = pos+1
    small=large=0
    if b & 0x80:
        small = b & 0x7f
        large = u32(R,p); p+=4
    else:
        small = b
    if small+large > 100000: return None
    if p + small*2 + large*8 > n: return None
    entries=[]
    for _ in range(small):
        w=u16(R,p); p+=2
        entries.append((w & 0xff, w>>8))
    for _ in range(large):
        entries.append((u32(R,p), u32(R,p+4))); p+=8
    return entries, p

def find(t, tag):
    for (tg,off) in t[0]:
        if tg==tag: return off
    return None

def collect_strings(R, a, end):
    out=[]; i=a
    while i+4<=end:
        ln=u32(R,i)
        if 3<=ln<=64 and i+4+ln<=end:
            ok=True
            for k in range(ln):
                c=R[i+4+k]
                if c<32 or c>=127: ok=False;break
            if ok:
                out.append(R[i+4:i+4+ln].decode('latin1')); i+=4+ln; continue
        i+=1
    return out

def main(path, want_class):
    data=open(path,'rb').read()
    region_off=u32(data,0)
    R=data[region_off:]; n=len(R)
    outer=read_table(R,n,0)
    t0=find(outer,0)
    top=read_table(R,n,outer[1]+t0)
    t5=find(top,5)
    lst=read_table(R,n,top[1]+t5)
    c1=find(lst,1)
    ch=read_table(R,n,lst[1]+c1)
    ents=ch[0]; base=ch[1]
    found=0
    for i,(tag,off) in enumerate(ents):
        a=base+off
        if i+1 < len(ents):
            nxt = base+ents[i+1][1]
        else:
            nxt = len(R)
        cid=u32(R,a)
        if cid!=want_class: continue
        found+=1
        sub=read_table(R,n,a+4)
        raw={}
        for (tg,o) in sub[0]:
            off2=sub[1]+o
            raw[tg]=(off2, R[off2], u32(R,off2))
        idv=raw.get(1,(0,0,0))[2]
        typ=raw.get(2,(0,0,0))[2]
        idx=raw.get(3,(0,0,0))[2]
        ilbs=collect_strings(R,a,nxt)
        print("id=0x%08x type=%d idx=%d ilb=%s" % (idv,typ,idx,ilbs))
        print("   tags:", {hex(k):(v[1]) for k,v in sorted(raw.items())})
    print("count:", found)

main(sys.argv[1], int(sys.argv[2],0))

