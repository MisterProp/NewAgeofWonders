import struct
import wave


def load(path):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    return sr, s[0::2], s[1::2]


def feats(L, R, sr, win_ms=100):
    win = sr * win_ms // 1000
    out = []
    for i in range(0, len(L) - win, win):
        seg = L[i:i + win]
        zc = 0
        for j in range(1, len(seg)):
            if (seg[j - 1] < 0) != (seg[j] < 0):
                zc += 1
        e = sum(x * x for x in seg) + sum(x * x for x in R[i:i + win])
        out.append((zc / win, (e / win / 2) ** 0.5))
    return out


sr1, L1, R1 = load('C:/GOG Games/Age of Wonders/Title og.wav')
sr2, L2, R2 = load('C:/GOG Games/Age of Wonders/port/re/title_mine.wav')
f1 = feats(L1, R1, sr1)
f2 = feats(L2, R2, sr2)
print('windows ref=%d mine=%d' % (len(f1), len(f2)))
# compare first 30 s (300 windows)
n = min(len(f1), len(f2), 300)
zc1 = [x[0] for x in f1[:n]]
zc2 = [x[0] for x in f2[:n]]
m1 = sum(zc1) / n
m2 = sum(zc2) / n
print('mean ZCR ref=%.4f mine=%.4f  ratio=%.3f' % (m1, m2, m2 / m1 if m1 else 0))
# correlation of the ZCR curves (timbre contour)
a = [x - m1 for x in zc1]
b = [x - m2 for x in zc2]
num = sum(x * y for x, y in zip(a, b))
den = (sum(x * x for x in a) * sum(y * y for y in b)) ** 0.5
print('ZCR-contour correlation = %.3f' % (num / den if den else 0))
for i in range(0, 60, 5):
    print(' t=%2d-%2ds  refZCR=%.4f mineZCR=%.4f' % (i, i + 5, sum(zc1[i * 10:i * 10 + 50]) / 50,
                                                     sum(zc2[i * 10:i * 10 + 50]) / 50))
