"""_smpsrc.py - decode TITLE sample 0 straight from the file, per galaxy spec."""
import struct
import sys

d = open('C:/GOG Games/Age of Wonders/Songs/TITLE.IT', 'rb').read()
on, inn, sn, pn = struct.unpack_from('<4H', d, 0x20)
base = 0xC0 + on + inn * 4
offs = [struct.unpack_from('<I', d, base + i * 4)[0] for i in range(sn)]
want = int(sys.argv[1]) if len(sys.argv) > 1 else 0
so = offs[want]
flags = d[so + 0x12]
convert = d[so + 0x2E]
length = struct.unpack_from('<I', d, so + 0x30)[0]
loopb = struct.unpack_from('<I', d, so + 0x34)[0]
loope = struct.unpack_from('<I', d, so + 0x38)[0]
c5 = struct.unpack_from('<I', d, so + 0x3C)[0]
ptr = struct.unpack_from('<I', d, so + 0x48)[0]
print('sample %d: ITflags=%02x convert=%02x len=%d loop=%d..%d c5=%d ptr=%d'
      % (want, flags, convert, length, loopb, loope, c5, ptr))
is16 = (flags & 2) != 0
isSigned = (convert & 1) != 0
delta = (convert & 4) != 0
vals = []
acc = 0
for i in range(12):
    if is16:
        v = struct.unpack_from('<h' if isSigned else '<H', d, ptr + i * 2)[0]
        if not isSigned:
            v -= 0x8000
    else:
        b = d[ptr + i]
        v = struct.unpack('<b', bytes([b]))[0] if isSigned else b - 0x80
    if delta:
        acc += v
        v = acc
    vals.append(v)
print('raw first 12 decoded values:', vals)
