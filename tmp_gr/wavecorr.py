import struct
import wave


def load(path, dec):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    return sr // dec, [(s[i * 2] + s[i * 2 + 1]) * 0.5 for i in range(0, n, dec)]


sr, a = load('C:/GOG Games/Age of Wonders/Title og.wav', 6)
sr2, b = load('C:/GOG Games/Age of Wonders/port/re/title_mine.wav', 6)
print('eff rate %d/%d' % (sr, sr2))
W = sr // 4              # 250 ms window
for t in (1, 5, 16, 26):
    x = a[t * sr:t * sr + W]
    y = b[t * sr:t * sr + W]
    mx = sum(x) / len(x)
    my = sum(y) / len(y)
    x = [v - mx for v in x]
    y = [v - my for v in y]
    nx = (sum(v * v for v in x)) ** 0.5
    best, bl = -9, 0
    for lag in range(-300, 301):
        n = len(x) - abs(lag)
        c = 0.0
        for i in range(0, n, 2):
            c += x[i] * y[i + lag]
        c /= n
        if c > best:
            best, bl = c, lag
    ny = (sum(v * v for v in y)) ** 0.5
    print('t=%2ds  best lag=%+4d (%.1f ms)  normalised corr=%.3f'
          % (t, bl, 1000.0 * bl / sr, best / (nx * ny / len(x)) if nx and ny else 0))
