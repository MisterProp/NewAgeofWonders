"""Scratch: printable strings in a VA range of a PE."""
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
from pe_va import sections, va_to_foff  # noqa: E402


def main(argv):
    path, va, length, out = argv[1], int(argv[2], 0), int(argv[3], 0), argv[4]
    data, _b, _s = sections(path)
    foff, _nm = va_to_foff(path, va)
    blob = data[foff:foff + length]
    lines = []
    cur = []
    for i, b in enumerate(blob):
        if 32 <= b < 127:
            cur.append(chr(b))
        else:
            if len(cur) >= 3:
                lines.append('0x%08x  %s' % (va + i - len(cur), ''.join(cur)))
            cur = []
    if len(cur) >= 3:
        lines.append('0x%08x  %s' % (va + len(blob) - len(cur), ''.join(cur)))
    open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('wrote %s (%d strings)' % (out, len(lines)))


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
