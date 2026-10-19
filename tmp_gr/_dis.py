"""_dis.py - print disassembly lines whose address is in [lo, hi)."""
import re
import sys

PATH = 're/exports/Galaxy/Galaxy.disasm.txt'
LINE = re.compile(r'^([0-9a-fA-F]+)\s\s(.*)$')


def main(lo, hi):
    a = int(lo, 16)
    b = int(hi, 16)
    pending = None
    for line in open(PATH, encoding='utf-8', errors='replace'):
        s = line.rstrip('\n')
        if s.startswith('===='):
            pending = s
            continue
        m = LINE.match(s)
        if m:
            v = int(m.group(1), 16)
            if a <= v < b:
                if pending:
                    print(pending)
                    pending = None
                print(s)


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
