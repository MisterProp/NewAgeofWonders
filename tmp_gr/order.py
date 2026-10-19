import math
import struct
import sys
import wave


def load(path, dec):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    return sr // dec, [(s[i * 2] + s[i * 2 + 1]) * 0.5 for i in range(0, n, dec)]


def envel(x, win):
    return [math.sqrt(sum(v * v for v in x[i:i + win]) / win)
            for i in range(0, len(x) - win, win)]


def corr(u, v):
    n = min(len(u), len(v))
    u, v = u[:n], v[:n]
    mu, mv = sum(u) / n, sum(v) / n
    num = sum((a - mu) * (b - mv) for a, b in zip(u, v))
    den = (sum((a - mu) ** 2 for a in u) * sum((b - mv) ** 2 for b in v)) ** 0.5
    return num / den if den else 0


sr, a = load('C:/GOG Games/Age of Wonders/Title og.wav', 16)
ea = envel(a, sr // 20)
for order in range(0, 8):
    sr2, b = load('C:/GOG Games/Age of Wonders/port/re/o%d.wav' % order, 16)
    eb = envel(b, sr2 // 20)
    print('start order %d: correlation with capture = %.3f' % (order, corr(ea, eb)))
