import csv, collections, sys

path = sys.argv[1] if len(sys.argv) > 1 else r'tmp_csv0.csv'
rows = list(csv.DictReader(open(path, encoding='utf-8')))
print('rows=%d' % len(rows))

def summarize(layer):
    agg = collections.Counter()
    dims = {}
    for r in rows:
        if r['layer'] != layer:
            continue
        key = (r['ilb'], r['frame'], r['entry'], r['w'], r['h'], r['ok'])
        agg[key] += 1
        dims[(r['ilb'], r['frame'])] = (r['w'], r['h'])
    print('--- %s ---' % layer)
    for key, n in sorted(agg.items()):
        print('  %-28s f=%-4s %-16s %sx%s ok=%s  n=%d' %
              (key[0], key[1], key[2], key[3], key[4], key[5], n))

summarize('terrain')
summarize('object')