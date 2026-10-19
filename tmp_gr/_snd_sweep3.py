"""_snd_sweep3.py - curated SUBSTRING sweep for music/SFX playback functions.

Plain GhidraMCP /search_functions (substring match) across all open programs.
"""
import json
import re
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'

TOKENS = [
    'Sound.', 'Music.', 'Audio.',
    'PlaySample', 'PlaySong', 'PlayMusic', 'PlayWave', 'PlaySFX', 'PlaySfx',
    'PlaySound', 'PlayVoice', 'PlayStream', 'PlayAmbient', 'PlayTrack',
    'StopSong', 'StopMusic', 'StopSound', 'StopStream',
    'SongPlayer', 'SongList', 'Songlist', 'SFXLibrary', 'SFXManager',
    'SoundManager', 'SoundEngine', 'SoundLib', 'SoundList',
    'InitializeSound', 'InitSound', 'ReleaseSound', 'ProcessSound',
    'LoadSample', 'LoadSong', 'LoadMusic', 'LoadSFX', 'LoadWave',
    'RegisterSound', 'MusicVolume', 'SFXVolume', 'SoundVolume', 'FadeMusic',
    'Play', 'Song', 'SFX', 'Sample', 'Voice',
]

LINE = re.compile(r'^(?P<name>.*?)\s+@\s+(?P<addr>[0-9A-Fa-f]+)\s*$')


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=120) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    progs = [p['name'] for p in
             json.loads(get('/list_open_programs'))['programs']]
    hits = {}
    for prog in progs:
        names = {}
        for tok in TOKENS:
            txt = get('/search_functions', name_pattern=tok, limit=5000,
                      program=prog)
            for line in txt.splitlines():
                line = line.strip()
                if not line or line.lower().startswith('no functions'):
                    continue
                m = LINE.match(line)
                if m:
                    names[m.group('name')] = m.group('addr')
        hits[prog] = names
        print('%-18s %d' % (prog, len(names)))

    with open('tmp_gr/_snd_hits3.json', 'w', encoding='utf-8') as fh:
        json.dump(hits, fh, indent=1, sort_keys=True)
    with open('tmp_gr/_snd_report3.txt', 'w', encoding='utf-8') as fh:
        for prog in progs:
            fh.write('==== %s (%d) ====\n' % (prog, len(hits[prog])))
            for name in sorted(hits[prog]):
                fh.write('%s\n' % name)
            fh.write('\n')
    print('wrote tmp_gr/_snd_report3.txt')


if __name__ == '__main__':
    main()
