import struct
import wave

REF = 'C:/GOG Games/Age of Wonders/Title og.wav'
MINE = 'C:/GOG Games/Age of Wonders/port/re/title_mine.wav'
DEC = 16          # decimation -> effective rate
LO, HI = 40, 1500  # Hz search range


def load(path, dec):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    mono = [(s[i * 2] + s[i * 2 + 1]) // 2 for i in range(0, n, dec)]
    return sr // dec, mono


def pitch(seg, sr):
    m = sum(seg) / len(seg)
    x = [v - m for v in seg]
    n = len(x)
    lo = max(2, sr // HI)
    hi = min(n - 1, sr // LO)
    best, bl = -1e18, 0
    for lag in range(lo, hi):
        c = 0.0
        for i in range(0, n - lag, 4):
            c += x[i] * x[i + lag]
        c /= (n - lag)
        if c > best:
            best, bl = c, lag
    return sr / bl if bl else 0.0


sr1, a = load(REF, 4)
sr2, b = load(MINE, 4)
print('eff rate ref=%d mine=%d' % (sr1, sr2))
for t in (1, 5, 10, 16, 22, 26):
    w1 = sr1
    s1 = a[t * sr1:(t + 1) * sr1]
    s2 = b[t * sr1:(t + 1) * sr1]
    p1 = pitch(s1, sr1)
    p2 = pitch(s2, sr2)
    print('t=%2ds ref=%7.1f Hz mine=%7.1f Hz ratio=%.3f' % (t, p1, p2, (p2 / p1) if p1 else 0))
