"""memdis.py - disassemble a GhidraMCP /read_memory JSON dump (32-bit x86).

Some RTTI-reachable AoW.exe routines were never turned into Ghidra functions, so
/decompile_function refuses them.  /read_memory still returns their bytes; this
tool feeds those bytes to Capstone.

    python tools/ghidra/mcp.py bin/m.json /read_memory \
        address=0x0043e0d4 length=512 program=/AoW.exe
    python tools/ghidra/memdis.py bin/m.json
"""
import json
import sys

import capstone


def main():
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    doc = json.load(open(sys.argv[1], encoding='utf-8'))
    code = bytes(doc['data'])
    base = int(doc['address'], 16)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for insn in md.disasm(code, base):
        print('%08x  %-24s %s %s'
              % (insn.address, insn.bytes.hex(' '), insn.mnemonic,
                 insn.op_str))


if __name__ == '__main__':
    raise SystemExit(main())
