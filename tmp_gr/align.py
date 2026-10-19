import math
import struct
import wave


def load(path, dec):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    mono = [(s[i * 2] + s[i * 2 + 1]) * 0.5 for i in range(0, n, dec)]
    return sr // dec, mono


def envel(x, win):
    return [math.sqrt(sum(v * v for v in x[i:i + win]) / win)
            for i in range(0, len(x) - win, win)]


sr, a = load('C:/GOG Games/Age of Wonders/Title og.wav', 16)
sr2, b = load('C:/GOG Games/Age of Wonders/port/re/title_mine.wav', 16)
W = sr // 20                      # 50 ms
ea = envel(a, W)
eb = envel(b, W)
print('windows ref=%d mine=%d' % (len(ea), len(eb)))


def corr(x, y, lag):
    n = min(len(x), len(y) - lag) if lag >= 0 else min(len(x) + lag, len(y))
    if n < 20:
        return -9
    if lag >= 0:
        u, v = x[:n], y[lag:lag + n]
    else:
        u, v = x[-lag:-lag + n], y[:n]
    mu = sum(u) / n
    mv = sum(v) / n
    num = sum((p - mu) * (q - mv) for p, q in zip(u, v))
    den = (sum((p - mu) ** 2 for p in u) * sum((q - mv) ** 2 for q in v)) ** 0.5
    return num / den if den else 0


best = (-9, 0)
for lag in range(-200, 201):      # +-10 s in 50 ms steps
    c = corr(ea, eb, lag)
    if c > best[0]:
        best = (c, lag)
print('best lag = %d windows (%.1f s)  correlation = %.3f' % (best[1], best[1] * 0.05, best[0]))
print('corr at lag 0 = %.3f' % corr(ea, eb, 0))
for lag in (0, 2, 5, 10, 20, -10, -20):
    print('  lag %+3d (%.2fs): %.3f' % (lag, lag * 0.05, corr(ea, eb, lag)))
