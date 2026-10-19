import re, collections, sys

def groups(path):
    g = collections.defaultdict(list)
    for enc in ('utf-16', 'utf-8'):
        try:
            fh = open(path, encoding=enc)
            fh.read(1)
            fh.close()
            break
        except (UnicodeError, UnicodeDecodeError):
            continue
    for l in open(path, encoding=enc, errors='replace'):
        m = re.search(r'type=(\d+)\s+idx=(\d+)\s+\[(.*)\]', l)
        if m:
            g[(m.group(1), m.group(3))].append(int(m.group(2)))
    return g

if __name__ == '__main__':
    flt = sys.argv[2] if len(sys.argv) > 2 else ''
    for k in sorted(groups(sys.argv[1])):
        v = sorted(set(groups(sys.argv[1])[k]))
        if flt and flt.lower() not in k[1].lower():
            continue
        print('type=%-3s n=%-3d %-24s idx=%d..%d contig=%s' % (
            k[0], len(v), k[1][:24], v[0], v[-1],
            v == list(range(v[0], v[-1] + 1))))
