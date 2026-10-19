"""_snd_sweep.py - sweep every open Ghidra program for sound/music functions.

Uses the GhidraMCP HTTP bridge (127.0.0.1:8089). Read-only: only GETs
/search_functions. Writes a grouped report to tmp_gr/_snd_report.txt and a
JSON dump to tmp_gr/_snd_hits.json.
"""
import json
import re
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'

PATTERNS = [
    'Sound', 'Music', 'Song', 'SFX', 'Sfx', 'Sample', 'Wave', 'Wav',
    'Audio', 'Voice', 'Mixer', 'Volume', 'DirectSound', 'MIDI', 'Midi',
    'BGM', 'Chant', 'Stream', 'Track', 'PlayS', 'PlayM', 'PlayW',
    'StopSound', 'StopMusic', 'StopSong', 'TSound', 'TSFX', 'TMusic',
    'TSong', 'TAudio', 'Fade', 'Crossfade', 'Reverb', 'Chorus',
]


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=120) as r:
        return r.read().decode('utf-8', 'replace')


def open_programs():
    data = json.loads(get('/list_open_programs'))
    return [p['name'] for p in data['programs']]


LINE = re.compile(r'^(?P<name>.*?)\s+@\s+(?P<addr>[0-9A-Fa-f]+)\s*$')


def search(program, pattern):
    txt = get('/search_functions', name_pattern=pattern, limit=5000,
              program=program)
    out = []
    for line in txt.splitlines():
        line = line.strip()
        if not line:
            continue
        m = LINE.match(line)
        if m:
            out.append((m.group('name'), m.group('addr')))
        else:
            out.append((line, ''))
    return out


def main():
    progs = open_programs()
    hits = {}          # program -> {name: addr}
    for prog in progs:
        hits[prog] = {}
        for pat in PATTERNS:
            try:
                for name, addr in search(prog, pat):
                    hits[prog][name] = addr
            except Exception as exc:      # keep going on a single failure
                print('WARN %s %s: %s' % (prog, pat, exc))
        print('%-18s %d hits' % (prog, len(hits[prog])))

    with open('tmp_gr/_snd_hits.json', 'w', encoding='utf-8') as fh:
        json.dump(hits, fh, indent=1, sort_keys=True)

    with open('tmp_gr/_snd_report.txt', 'w', encoding='utf-8') as fh:
        for prog in progs:
            fh.write('==== %s (%d) ====\n' % (prog, len(hits[prog])))
            for name in sorted(hits[prog]):
                fh.write('%s @ %s\n' % (name, hits[prog][name]))
            fh.write('\n')
    print('wrote tmp_gr/_snd_report.txt and tmp_gr/_snd_hits.json')


if __name__ == '__main__':
    main()
