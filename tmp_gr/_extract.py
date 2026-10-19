"""_extract.py - print selected functions from a DecompileAll .decompiled.c."""
import re
import sys

HDR = re.compile(r'^/\* ==== ([0-9A-Fa-f]+)\s+(.*?) ==== \*/$')


def parse(path):
    funcs = {}
    cur = None
    buf = []
    for line in open(path, encoding='utf-8', errors='replace'):
        m = HDR.match(line.rstrip('\n'))
        if m:
            if cur:
                funcs[cur] = ''.join(buf)
            cur = m.group(2)
            buf = []
        elif cur:
            buf.append(line)
    if cur:
        funcs[cur] = ''.join(buf)
    return funcs


if __name__ == '__main__':
    path = sys.argv[1]
    pats = sys.argv[2:]
    funcs = parse(path)
    for name, body in funcs.items():
        if not pats or any(p.lower() in name.lower() for p in pats):
            print('==== %s ====' % name)
            print(body.rstrip())
            print()
