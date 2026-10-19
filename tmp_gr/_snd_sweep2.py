"""_snd_sweep2.py - curated sweep for music/SFX playback functions.

Regex (case-insensitive) search via GhidraMCP /search_functions_enhanced.
Writes tmp_gr/_snd_hits2.json and tmp_gr/_snd_report2.txt.
"""
import json
import re
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'

# Strong, playback-oriented patterns (regex alternation, case-insensitive).
PATTERN = (r'(?i)('
           r'playsample|playsong|playmusic|playsfx|playsound|playwave|'
           r'playvoice|playtrack|playambient|playstream|'
           r'stopsong|stopmusic|stopsound|stopall|stopstream|'
           r'songplayer|songlist|sfxlibrary|sfxmanager|soundmanager|'
           r'soundengine|soundlib|soundlist|musictrack|'
           r'initializesound|initsound|releasesound|processsound|'
           r'loadsample|loadsong|loadmusic|registersound|'
           r'musicvolume|sfxvolume|soundvolume|fademusic|'
           r'sound\.|music\.|audio\.|snd\.'
           r')')

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
        txt = get('/search_functions_enhanced', name_pattern=PATTERN,
                  regex='true', limit=5000, program=prog)
        names = {}
        for line in txt.splitlines():
            line = line.strip()
            if not line or line.lower().startswith('no functions'):
                continue
            m = LINE.match(line)
            if m:
                names[m.group('name')] = m.group('addr')
        hits[prog] = names
        print('%-18s %d' % (prog, len(names)))

    with open('tmp_gr/_snd_hits2.json', 'w', encoding='utf-8') as fh:
        json.dump(hits, fh, indent=1, sort_keys=True)
    with open('tmp_gr/_snd_report2.txt', 'w', encoding='utf-8') as fh:
        for prog in progs:
            fh.write('==== %s (%d) ====\n' % (prog, len(hits[prog])))
            for name in sorted(hits[prog]):
                fh.write('%s\n' % name)
            fh.write('\n')
    print('wrote tmp_gr/_snd_report2.txt')


if __name__ == '__main__':
    main()
