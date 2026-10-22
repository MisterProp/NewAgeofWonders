"""Scratch: find a Delphi published-method-table entry for a name in a PE.

Entries are <u16><u32 addr><len><name> (address precedes its name), so scan the
raw file for the name, print the preceding bytes and decode the candidate
address.  Read-only.

    python tmp_gr/_find_method.py <exe> <name> [<name> ...]
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
        start = 0
        print('=== %s' % name)
        while True:
            i = data.find(needle, start)
            if i < 0:
                break
            start = i + 1
            va, sec = pe_va.foff_to_va(path, i)
            pre = data[max(0, i - 12):i]
            addr = struct.unpack_from('<I', data, i - 4)[0] if i >= 4 else 0
            print('  file 0x%06x va 0x%08x (%s) addr_before=0x%08x  pre=%s'
                  % (i, va or 0, sec, addr, pre.hex()))
            # show the 48 bytes after the name (next entries)
            print('     after: %s' % data[i + len(needle):i + len(needle) + 48].hex())
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
