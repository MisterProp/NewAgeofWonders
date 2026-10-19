import math
import struct
import wave

REF = 'C:/GOG Games/Age of Wonders/Title og.wav'
MINE = 'C:/GOG Games/Age of Wonders/port/re/title_mine.wav'
FREQS = [73, 98, 131, 175, 233, 311, 415, 554, 739, 985, 1313, 1751,
         2335, 3114, 4152, 5536, 7382]


def load(path):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    mono = [(s[i * 2] + s[i * 2 + 1]) * 0.5 for i in range(n)]
    return sr, mono


def goertzel(x, f, sr):
    c = 2.0 * math.cos(2.0 * math.pi * f / sr)
    s1 = s2 = 0.0
    for v in x:
        s0 = v + c * s1 - s2
        s2 = s1
        s1 = s0
    return math.sqrt(max(0.0, s1 * s1 + s2 * s2 - c * s1 * s2))


def profile(x, sr):
    p = [goertzel(x, f, sr) for f in FREQS]
    m = max(p) if max(p) > 0 else 1.0
    return [v / m for v in p]


sr1, a = load(REF)
sr2, b = load(MINE)
print('freq   ' + ' '.join('%5d' % f for f in FREQS))
for t in (1, 20):
    w = sr1 // 4
    pa = profile(a[t * sr1:t * sr1 + w], sr1)
    pb = profile(b[t * sr1:t * sr1 + w], sr2)
    print('ref  t=%-2d' % t, ' '.join('%5.2f' % v for v in pa))
    print('mine t=%-2d' % t, ' '.join('%5.2f' % v for v in pb))
