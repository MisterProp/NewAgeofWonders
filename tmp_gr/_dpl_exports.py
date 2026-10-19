"""_dpl_exports.py - list PE export names from every .dpl (stdlib only).

Used to cross-check the Ghidra search for modules that are NOT loaded in the
Ghidra project (notably SoundP.dpl, the audio backend).
"""
import glob
import re
import struct
import sys

SOUND_RE = re.compile(
    r'(?i)(sound|music|song|sfx|sample|wave|audio|voice|directsound|'
    r'playsample|playsong|playmusic|playsound|fade)')


def exports(path):
    data = open(path, 'rb').read()
    e_lfanew = struct.unpack_from('<I', data, 0x3c)[0]
    if data[e_lfanew:e_lfanew + 4] != b'PE\0\0':
        return []
    coff = e_lfanew + 4
    nsec = struct.unpack_from('<H', data, coff + 2)[0]
    opt_size = struct.unpack_from('<H', data, coff + 16)[0]
    opt = coff + 20
    magic = struct.unpack_from('<H', data, opt)[0]
    dd_off = opt + (96 if magic == 0x10b else 112)
    exp_rva, exp_size = struct.unpack_from('<II', data, dd_off)
    if exp_rva == 0:
        return []
    sec_off = opt + opt_size

    def rva_to_off(rva):
        for i in range(nsec):
            s = sec_off + i * 40
            va = struct.unpack_from('<I', data, s + 12)[0]
            vs = struct.unpack_from('<I', data, s + 8)[0]
            raw = struct.unpack_from('<I', data, s + 20)[0]
            rawsize = struct.unpack_from('<I', data, s + 16)[0]
            if va <= rva < va + max(vs, rawsize):
                return raw + (rva - va)
        return None

    eo = rva_to_off(exp_rva)
    if eo is None:
        return []
    nnames = struct.unpack_from('<I', data, eo + 24)[0]
    names_rva = struct.unpack_from('<I', data, eo + 32)[0]
    no = rva_to_off(names_rva)
    out = []
    for i in range(nnames):
        nrva = struct.unpack_from('<I', data, no + i * 4)[0]
        off = rva_to_off(nrva)
        if off is None:
            continue
        end = data.index(b'\0', off)
        out.append(data[off:end].decode('latin-1'))
    return out


def main():
    pat = sys.argv[1] if len(sys.argv) > 1 else '*.dpl'
    show_all = '--all' in sys.argv
    for path in sorted(glob.glob(pat)):
        try:
            names = exports(path)
        except Exception as exc:
            print('%-20s ERROR %s' % (path, exc))
            continue
        if show_all:
            sel = names
        else:
            sel = [n for n in names if SOUND_RE.search(n)]
        print('==== %s (%d exports, %d match) ====' % (
            path, len(names), len(sel)))
        for n in sel:
            print('  ' + n)
        print()


if __name__ == '__main__':
    main()
