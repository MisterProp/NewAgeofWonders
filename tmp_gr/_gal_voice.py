"""_gal_voice.py - which functions reference the voice record array."""
import re
import sys

HDR = re.compile(r'^/\* ==== ([0-9A-Fa-f]+)\s+(.*?) ==== \*/$')
TOKENS = ['1006a86', '1006a87', '1006a88', '1006a89',
          '1006a8a', '1006a8b', '1006a8c', '1006a8d', '1006a8e']


def main(path):
    cur = None
    buf = []
    funcs = []
    for line in open(path, encoding='utf-8', errors='replace'):
        m = HDR.match(line.rstrip('\n'))
        if m:
            if cur:
                funcs.append((cur, ''.join(buf)))
            cur = m.group(2)
            buf = []
        elif cur:
            buf.append(line)
    if cur:
        funcs.append((cur, ''.join(buf)))
    for n, b in funcs:
        c = sum(b.count(t) for t in TOKENS)
        if c > 6:
            print('%-22s refs=%-4d len=%d' % (n, c, len(b)))


if __name__ == '__main__':
    main(sys.argv[1])
