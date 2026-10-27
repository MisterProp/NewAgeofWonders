"""dump_vmt.py - print the virtual-method slots of an AoWEPACK.dpl class.

    python tmp_gr/dump_vmt.py TPlayerStructure 0x000 0x240

Resolves each slot through GhidraMCP's /get_function_by_address so the offsets
used by the SetPlayer / MsgProc code (+0x1f0, +0x1fc, ...) get names.
"""
import json
import os
import sys
import urllib.parse
import urllib.request

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(
    os.path.abspath(__file__))), 'tools'))
import common            # noqa: E402
import vmt_scan          # noqa: E402

BASE = 'http://127.0.0.1:8089'
PROG = 'AoWEPACK.dpl'


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=120) as r:
        return r.read().decode('utf-8', 'replace')


def name_of(addr):
    try:
        txt = get('/get_function_by_address', address=hex(addr), program=PROG)
    except Exception:
        return ''
    try:
        d = json.loads(txt)
    except Exception:
        return txt.strip().splitlines()[0] if txt.strip() else ''
    return d.get('name') or d.get('function') or ''


def main():
    cls = sys.argv[1]
    lo = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0
    hi = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x240
    pe = vmt_scan.Pe(os.path.join(common.GAME_ROOT, 'AoWEPACK.dpl'))
    classes = vmt_scan.find_all(pe)
    # vmt_scan returns the address 12 bytes *after* the class reference this
    # Borland compiler stores in an object's field 0 (verified: classRef+0x1f0
    # is TPlayerStructure.SetPlayer, the slot the decompiles call).
    vmt = classes[cls][0] - 12
    print('%s class=0x%08x (vmt_scan 0x%08x)'
          % (cls, vmt, classes[cls][0]))
    for off in range(lo, hi, 4):
        addr = pe.u32(vmt + off)
        if not addr:
            continue
        print('  +0x%03x  0x%08x  %s' % (off, addr, name_of(addr)))


if __name__ == '__main__':
    main()
