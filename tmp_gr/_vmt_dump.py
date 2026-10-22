"""Scratch: decode a Delphi class VMT + published method table in a PE.

    python tmp_gr/_vmt_dump.py <pe> <vmt_va> <out.txt>

Layout used (Delphi 4/7 style): at vmt-0x2C a pointer to the class-name short
string; at vmt-0x28 InstanceSize; vmt-0x24 Parent; vmt-0x04 Destroy; user
methods from vmt+0. The published-method table is at *(vmt-0x34) and holds
entries <u16 index><u32 addr><u8 len><name>.
"""
import struct
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import pe_va  # noqa: E402


def main(argv):
    path, vmt = argv[1], int(argv[2], 0)
    out = argv[3]
    data, base, _secs = pe_va.sections(path)
    lines = []
    off, _ = pe_va.va_to_foff(path, vmt)
    lines.append('vmt 0x%08x file 0x%06x' % (vmt, off))

    def dword(va):
        o, _n = pe_va.va_to_foff(path, va)
        return struct.unpack_from('<I', data, o)[0]

    def short_str(va):
        o, _n = pe_va.va_to_foff(path, va)
        n = data[o]
        return data[o + 1:o + 1 + n].decode('latin1')

    for delta in (-0x34, -0x30, -0x2c, -0x28, -0x24, -0x20, -0x10, -0x04):
        va = vmt + delta
        try:
            v = dword(va)
        except Exception as exc:  # noqa: BLE001
            lines.append('  %-6s 0x%08x  <error %s>' % (delta, va, exc))
            continue
        extra = ''
        if delta == -0x2c:
            try:
                extra = ' class="%s"' % short_str(v)
            except Exception:  # noqa: BLE001
                pass
        lines.append('  %-6s 0x%08x -> 0x%08x%s' % (delta, va, v, extra))

    # class name (vmt-0x2C)
    name_ptr = dword(vmt - 0x2C)
    lines.append('class name: %s' % short_str(name_ptr))
    lines.append('instance size: 0x%x' % dword(vmt - 0x28))
    parent = dword(vmt - 0x24)
    lines.append('parent vmt: 0x%08x' % parent)

    # user methods
    lines.append('user methods:')
    for i in range(0, 16):
        va = vmt + i * 4
        try:
            v = dword(va)
        except Exception:  # noqa: BLE001
            break
        if v < base or v > base + 0x400000:
            break
        lines.append('  +0x%02x 0x%08x' % (i * 4, v))

    # published method table
    mt = dword(vmt - 0x34)
    lines.append('method table: 0x%08x' % mt)
    if base <= mt < base + 0x400000:
        o, _n = pe_va.va_to_foff(path, mt)
        p = o
        for _ in range(64):
            try:
                idx, addr, ln = struct.unpack_from('<HIB', data, p)
            except Exception:  # noqa: BLE001
                break
            name = data[p + 7:p + 7 + ln].decode('latin1')
            if not name.isprintable() or ln == 0:
                break
            lines.append('  idx=0x%04x addr=0x%08x %s' % (idx, addr, name))
            p += 7 + ln
    open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('wrote', out)


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
