"""find_handlers.py - resolve a Delphi form's event handlers to code addresses.

A DFM only stores handler *names* (`OnChange = PlayModeCBChange`). Delphi keeps
each published method in the class' MethodAddress table, whose entries are

    MethodEntry = packed record
      Size: Word;         // 6 + 1 + Length(Name)
      CodeAddress: Pointer;
      Name: ShortString;
    end;

so for every handler name the DFM mentions we scan the executable for the
ShortString and read the code pointer six bytes in front of it. That is how the
TScenSetup settings handlers were found (docs/08_SCENARIO_SCREEN.md sec. 11.2):

    PlayModeCBChange        0x004129e8
    TurnTimeSlChange        0x0041285c
    TurnsSlChange           0x004129a8
    LeadersOnMapCBChange    0x00412b04
    ...

    python tools/ghidra/find_handlers.py
    python tools/ghidra/find_handlers.py --dfm re/dfm_all/TSCENSETUP.txt \
        --exe ..\\AoW.exe.orig

The addresses can then be handed to /decompile_function through mcp_call.py.
Standard library only, like the rest of tools/.
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from pe_va import sections  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HERE))
GAME = os.path.dirname(PORT)

DEFAULT_DFMS = [os.path.join(PORT, 're', 'dfm_all', 'TSCENSETUP.txt'),
                os.path.join(PORT, 're', 'dfm_all', 'TGAMEPLAYDLG.txt')]
DEFAULT_EXE = os.path.join(GAME, 'AoW.exe.orig')


def handler_names(dfms):
    """Every `On<Event> = <Handler>` name in the given DFM dumps."""
    names = {}
    for dfm in dfms:
        if not os.path.exists(dfm):
            print('  (missing %s)' % dfm, file=sys.stderr)
            continue
        for line in open(dfm, encoding='latin-1'):
            m = re.match(r'\s*On\w+\s*=\s*(\w+)\s*$', line)
            if m:
                names.setdefault(m.group(1), set()).add(os.path.basename(dfm))
    return names


def find_addresses(exe, names):
    """handler -> [code addresses] via the published-method table."""
    data, base, secs = sections(exe)
    code = next(s for s in secs if s[0] in ('CODE', '.text'))
    text_lo = base + code[1]
    text_hi = text_lo + code[2]
    out = {}
    for name in sorted(names):
        nb = name.encode('ascii')
        # `find` matches the ShortString's length byte too, so the entry (u16
        # Size, u32 CodeAddress) starts six bytes before the match.
        pat = bytes([len(nb)]) + nb
        hits = []
        start = 0
        while True:
            j = data.find(pat, start)
            if j < 0:
                break
            start = j + 1
            if j < 6:
                continue
            entry = j - 6
            size, = struct.unpack_from('<H', data, entry)
            codeptr, = struct.unpack_from('<I', data, entry + 2)
            if size == 7 + len(nb) and text_lo <= codeptr < text_hi:
                hits.append(codeptr)
        out[name] = hits
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--dfm', action='append', default=None,
                    help='DFM dump to scan (repeatable; default: TSCENSETUP, '
                         'TGAMEPLAYDLG)')
    ap.add_argument('--exe', default=DEFAULT_EXE,
                    help='the game executable (default: AoW.exe.orig)')
    args = ap.parse_args(argv)

    dfms = args.dfm or DEFAULT_DFMS
    names = handler_names(dfms)
    if not names:
        print('no handlers found in %s' % ', '.join(dfms))
        return 1
    addrs = find_addresses(args.exe, names)
    print('%-34s %-22s %s' % ('handler', 'code address', 'DFM'))
    for name, hits in addrs.items():
        text = ', '.join('0x%08x' % a for a in hits) if hits else '(none)'
        print('%-34s %-22s %s' % (name, text, ','.join(sorted(names[name]))))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
