"""_snd_final.py - robust collector for music/SFX functions via GhidraMCP.

Calls the HTTP bridge directly (urllib) so any query params can be passed.
Collects the union of substring matches across every open program, then
writes tmp_gr/_snd_final.txt (grouped, deduped) and tmp_gr/_snd_final.json.
"""
import json
import re
import time
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'

TOKENS = [
    'Sound', 'Music', 'Song', 'SFX', 'Sfx', 'Sample', 'Voice', 'Wave',
    'Audio', 'Fade', 'DirectSound', 'PlaySFX', 'PlaySfx', 'PlaySound',
    'PlaySong', 'PlayMusic', 'PlaySample', 'PlayWave', 'PlayVoice',
    'StopSound', 'StopSong', 'StopMusic', 'SongPlayer', 'Songlist',
    'SFXLibrary', 'SFXManager', 'SoundEngine', 'SoundManager',
    'InitializeSound', 'ReleaseSound', 'ProcessSound', 'MusicVolume',
    'SFXVolume', 'SoundVolume', 'FadeMusic', 'LoadSample', 'LoadSong',
    'LoadMusic', 'RegisterSound', 'Ambient', 'Chant', 'PlayLoggedSFX',
]

# Names that contain a token but are NOT about audio playback.
EXCLUDE = re.compile(
    r'(?i)(enchant|stream|display|volumeinformation|getvolumeinformation|'
    r'track|reverb|chorus|midi|mixer|bgm)')

LINE = re.compile(r'^(?P<name>.*?)\s+@\s+(?P<addr>[0-9A-Fa-f]+)\s*$')


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=120) as r:
        return r.read().decode('utf-8', 'replace')


def search(program, pattern):
    for attempt in range(3):
        try:
            txt = get('/search_functions', name_pattern=pattern, limit=5000,
                      program=program)
            out = []
            for line in txt.splitlines():
                line = line.strip()
                if not line or line.lower().startswith('no functions'):
                    continue
                m = LINE.match(line)
                if m:
                    out.append(m.group('name'))
            return out
        except Exception as exc:
            print('  retry %s/%s: %s' % (program, pattern, exc))
            time.sleep(1.0)
    return []


def main():
    progs = [p['name'] for p in
             json.loads(get('/list_open_programs'))['programs']]
    print('programs:', progs)
    hits = {}
    for prog in progs:
        names = {}
        for tok in TOKENS:
            for name in search(prog, tok):
                if not EXCLUDE.search(name):
                    names[name] = True
        hits[prog] = sorted(names)
        print('%-18s %d' % (prog, len(names)))
        time.sleep(0.15)

    with open('tmp_gr/_snd_final.json', 'w', encoding='utf-8') as fh:
        json.dump(hits, fh, indent=1, sort_keys=True)
    with open('tmp_gr/_snd_final.txt', 'w', encoding='utf-8') as fh:
        for prog in progs:
            fh.write('==== %s (%d) ====\n' % (prog, len(hits[prog])))
            for name in hits[prog]:
                fh.write('%s\n' % name)
            fh.write('\n')
    print('wrote tmp_gr/_snd_final.txt')


if __name__ == '__main__':
    main()
