import sys
pat = sys.argv[1]
for p in sys.argv[2:]:
    print('\n==', p)
    lines = open(p, encoding='utf-8', errors='replace').read().splitlines()
    for i, ln in enumerate(lines):
        if pat in ln:
            lo = max(0, i - 2)
            hi = min(len(lines), i + 3)
            for k in range(lo, hi):
                print('%5d %s' % (k + 1, lines[k]))
            print('  ---')
