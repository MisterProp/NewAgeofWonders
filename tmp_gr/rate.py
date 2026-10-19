import math
import struct
import wave


def load(path, dec=16):
    w = wave.open(path, 'rb')
    n = w.getnframes()
    sr = w.getframerate()
    s = struct.unpack('<%dh' % (n * 2), w.readframes(n))
    return sr // dec, [(s[i * 2] + s[i * 2 + 1]) * 0.5 for i in range(0, n, dec)]


def env_end(path):
    sr, x = load(path)
    W = sr // 10                     # 100 ms
    e = [math.sqrt(sum(v * v for v in x[i:i + W]) / W)
         for i in range(0, len(x) - W, W)]
    mx = max(e)
    thr = 0.25 * mx
    last = 0
    for i, v in enumerate(e):
        if v > thr:
            last = i
    return sr, e, mx, thr, last * 0.1


for name in ('C:/GOG Games/Age of Wonders/Title og.wav',
             'C:/GOG Games/Age of Wonders/port/re/title_mine.wav'):
    sr, e, mx, thr, t = env_end(name)
    print('%-22s max=%.0f thr=%.0f  last-loud=%.2f s' % (name.split('/')[-1], mx, thr, t))
