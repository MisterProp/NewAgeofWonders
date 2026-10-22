"""Scratch: decode the Delphi published-method-table entry that owns a name.

    python tmp_gr/_method_entry.py <exe> <name> [...]
"""
import struct
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import pe_va  # noqa: E402


def main(argv):
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    path = argv[1]
    data, base, secs = pe_va.sections(path)
    for name in argv[2:]:
        needle = name.encode('ascii')
        pos = data.find(needle)
        if pos < 0:
            print('%-28s NOT FOUND' % name)
            continue
        entry = data[pos - 8:pos]
        idx, addr, ln = struct.unpack_from('<HIB', entry, 1)
        va, sec = pe_va.foff_to_va(path, pos)
        print('%-28s nameVa=0x%08x (%s) idx=0x%04x addr=0x%08x len=%d'
              % (name, va, sec, idx, addr, ln))
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
