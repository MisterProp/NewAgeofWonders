"""_peak.py - report peak/RMS of rendered WAVs (validates the IT player)."""
import glob
import os
import struct
import sys
import wave


def main(pattern):
    bad = 0
    for f in sorted(glob.glob(pattern)):
        with wave.open(f, 'rb') as w:
            n = w.getnframes()
            raw = w.readframes(n)
        s = struct.unpack('<%dh' % (len(raw) // 2), raw)
        mono = s[0::2] + s[1::2]
        pk = max(abs(x) for x in mono) if mono else 0
        rms = (sum(x * x for x in mono) / len(mono)) ** 0.5 if mono else 0.0
        ok = rms > 200
        if not ok:
            bad += 1
        print('%-14s frames=%-7d peak=%-6d rms=%-8.1f %s'
              % (os.path.basename(f), len(s) // 2, pk, rms, 'ok' if ok else 'CHECK'))
    print('files=%d suspicious=%d' % (len(glob.glob(pattern)), bad))
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1] if len(sys.argv) > 1 else 're/audio_out/*.wav'))
