import struct, sys

for path in sys.argv[1:]:
    d = open(path, 'rb').read()
    print('\n', path)
    for i in range(0, len(d) - 12):
        if struct.unpack_from('<I', d, i)[0] == 0x100:
            idx = struct.unpack_from('<I', d, i - 4)[0]
            if idx < 0x10000:
                # name follows class(4) + fmt(1) + len(4) + name
                ln = struct.unpack_from('<I', d, i + 5)[0]
                if 1 <= ln < 60 and i + 9 + ln <= len(d):
                    nm = d[i + 9:i + 9 + ln]
                    if all(32 <= c < 127 for c in nm):
                        print('node idx=%d at 0x%x name=%s' %
                              (idx, i - 4, nm.decode()))
