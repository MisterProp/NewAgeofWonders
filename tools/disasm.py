"""disasm.py - local x86 disassembly of a game binary region (Capstone).

Ghidra's project did not turn every RTTI-reachable routine into a function, so
some title-screen handlers cannot be reached through the MCP decompiler. This
tool disassembles them straight from the PE.

    python tools/disasm.py AoW.exe 0x41ae08 400
    python tools/disasm.py AoW.exe --func 0x41ae08

Capstone is a dev-only aid (pip install capstone); nothing in port/src depends
on it.
"""
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

GAME = r'C:\GOG Games\Age of Wonders\\'
sys.path.insert(0, r'C:\GOG Games\Age of Wonders\port\tools')
from pe_va import sections, va_to_foff  # noqa: E402

try:
    import capstone
except ImportError:  # pragma: no cover
    capstone = None


def main():
    if capstone is None:
        print('capstone not installed: python -m pip install capstone')
        return 1
    path = sys.argv[1]
    if '\\' not in path and not path.lower().startswith('c:'):
        path = GAME + path
    args = sys.argv[2:]
    func_mode = args and args[0] == '--func'
    if func_mode:
        args = args[1:]
    va = int(args[0], 0)
    length = int(args[1], 0) if len(args) > 1 else 512

    data, base, _secs = sections(path)
    foff, nm = va_to_foff(path, va)
    code = data[foff:foff + length]

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    print('%s  VA 0x%08x (%s)  %d bytes' % (path, va, nm, len(code)))
    stop = 0
    for insn in md.disasm(code, va):
        print('%08x  %-24s %s %s'
              % (insn.address, insn.bytes.hex(' '), insn.mnemonic,
                 insn.op_str))
        stop += 1
        if func_mode and insn.mnemonic in ('ret', 'retn'):
            break
        if stop > 4000:
            break
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
