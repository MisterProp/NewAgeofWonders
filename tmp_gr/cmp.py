import math
import struct
import wave


def load(path):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    return sr, s[0::2], s[1::2]


def rms(L, R, a, b):
    if b <= a:
        return 0.0
    e = 0.0
    for j in range(a, b):
        e += L[j] * L[j] + R[j] * R[j]
    return math.sqrt(e / (b - a) / 2)


sr1, L1, R1 = load('C:/GOG Games/Age of Wonders/Title og.wav')
sr2, L2, R2 = load('C:/GOG Games/Age of Wonders/port/re/title_mine.wav')
print('ref rate=%d frames=%d (%.1fs)' % (sr1, len(L1), len(L1) / sr1))
print('mine rate=%d frames=%d (%.1fs)' % (sr2, len(L2), len(L2) / sr2))
print('%6s %10s %10s %8s %8s' % ('t', 'ref', 'mine', 'ratio', 'gaindB'))
for i in range(12):
    a = i * 5 * sr1
    b = min((i + 1) * 5 * sr1, len(L1))
    a2 = i * 5 * sr2
    b2 = min((i + 1) * 5 * sr2, len(L2))
    r1 = rms(L1, R1, a, b)
    r2 = rms(L2, R2, a2, b2)
    ratio = r2 / r1 if r1 else 0.0
    gdb = 20 * math.log10(ratio) if ratio > 0 else -99
    print('%3d-%3ds %10.1f %10.1f %8.2f %8.2f' % (i * 5, (i + 1) * 5, r1, r2, ratio, gdb))

# stereo balance
for name, (L, R) in (('ref', (L1, R1)), ('mine', (L2, R2))):
    tot = sum(L) * 0 + 1
    e = math.sqrt(sum(x * x for x in L))
    f = math.sqrt(sum(x * x for x in R))
    print('%s L/R energy ratio = %.4f' % (name, e / f if f else 0))
