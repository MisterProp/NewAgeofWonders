"""Scratch: capstone disassembly of a VA range, written as UTF-8."""
import sys

sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
import capstone  # noqa: E402
from pe_va import sections, va_to_foff  # noqa: E402


def main(argv):
    path, va, length, out = argv[1], int(argv[2], 0), int(argv[3], 0), argv[4]
    data, _base, _secs = sections(path)
    foff, _nm = va_to_foff(path, va)
    code = data[foff:foff + length]
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    lines = []
    for insn in md.disasm(code, va):
        lines.append('%08x  %-22s %s %s'
                     % (insn.address, insn.bytes.hex(' '), insn.mnemonic,
                        insn.op_str))
    open(out, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('wrote %s (%d insns)' % (out, len(lines)))


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
