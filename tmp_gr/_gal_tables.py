"""_gal_tables.py - dump galaxy.dll's note-period and linear-freq tables as C."""
import struct
import sys

import importlib.util
spec = importlib.util.spec_from_file_location('pe', 'tmp_gr/_pe_dat.py')
pe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pe)

PATH = '../Galaxy.dll'


def dump(va, count, fmt, per_line, name):
    d = open(PATH, 'rb').read()
    off, _ = pe.va_to_off(d, va)
    if off is None:
        print('// %s: not mapped' % name)
        return
    vals = struct.unpack_from('<%d%s' % (count, fmt), d, off)
    print('static const std::uint32_t %s[%d] = {' % (name, count))
    line = []
    for i, v in enumerate(vals):
        line.append('0x%Xu' % v)
        if len(line) == per_line:
            print('    ' + ', '.join(line) + ',')
            line = []
    if line:
        print('    ' + ', '.join(line) + ',')
    print('};')


if __name__ == '__main__':
    dump(0x100447E0, 120, 'I', 12, 'kNoteTable')
    dump(0x100449C0, 768, 'I', 12, 'kLinearTable')
