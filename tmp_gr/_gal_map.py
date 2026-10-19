"""_gal_map.py - which functions touch the key globals (voice array, music)."""
import re
import sys

HDR = re.compile(r'^/\* ==== ([0-9A-Fa-f]+)\s+(.*?) ==== \*/$')

KEY = {
    'voice_a870': '1006a870',
    'voice_a860': '1006a860',
    'music_state': '10064e7d',
    'song_len': 'glxSongLength',
    'cur_order': 'glxCurrentOrder',
    'cur_speed': 'glxCurrentSpeed',
    'cur_tempo': 'glxCurrentTempo',
    'pattern_row': 'glxPatternRow',
    'sample_vol': 'glxSampleVolume',
    'patt_data': '10060',
}


def main(path):
    funcs = []
    cur = None
    buf = []
    for line in open(path, encoding='utf-8', errors='replace'):
        m = HDR.match(line.rstrip('\n'))
        if m:
            if cur:
                funcs.append((cur, ''.join(buf)))
            cur = m.group(2)
            buf = []
        elif cur:
            buf.append(line)
    if cur:
        funcs.append((cur, ''.join(buf)))

    for name, body in funcs:
        hits = [k for k, v in KEY.items() if v in body]
        if hits:
            print('%-34s %s' % (name, ','.join(hits)))
    print('total functions: %d' % len(funcs))


if __name__ == '__main__':
    main(sys.argv[1])
