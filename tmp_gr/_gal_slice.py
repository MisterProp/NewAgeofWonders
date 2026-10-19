"""_gal_slice.py - print a line range of one decompiled function."""
import re
import sys

HDR = re.compile(r'^/\* ==== ([0-9A-Fa-f]+)\s+(.*?) ==== \*/$')


def main(path, name, a, b):
    cur = None
    buf = []
    for line in open(path, encoding='utf-8', errors='replace'):
        m = HDR.match(line.rstrip('\n'))
        if m:
            if cur == name:
                break
            cur = m.group(2)
            buf = []
        elif cur == name:
            buf.append(line)
    a = int(a)
    b = int(b) if b else len(buf)
    for i in range(a, min(b, len(buf))):
        print('%4d %s' % (i, buf[i].rstrip('\n')))
    print('total lines: %d' % len(buf))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4] if len(sys.argv) > 4 else None)
