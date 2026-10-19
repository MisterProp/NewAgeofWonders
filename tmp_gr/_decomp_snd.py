"""_decomp_snd.py - pull sound-related functions out of the running GhidraMCP.

Searches each loaded program for sound patterns, then batch-decompiles the
matches. Writes re/decomp/sound_api.txt (names+addresses+decompiled C).
"""
import json
import os
import re
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
LINE = re.compile(r'^(?P<name>.*?)\s+@\s+(?P<addr>[0-9A-Fa-f]+)\s*$')

TARGETS = {
    'AoWEPACK.dpl': [
        'PlaySFX', 'PlayLoggedSFX', 'PlayMoveSFX', 'PlaySelectSample',
        'TStartupSongList', 'TPlaySampleMoveOnEvent', 'TChangeSongMoveOnEvent',
        'TCombatSFXType', 'TSFXManager', 'TSFXLibrary', 'TSonglist',
        'Sound.PlaySample', 'Sound.PlaySong', 'Sound.GetSongName',
        'Sound.PlayingSongFileName', 'Sound.RegisterSoundEngine',
    ],
    'HSEPack.dpl': ['TSFXManager', 'Sound.Play', 'Sound.GetSong'],
    'AoW.exe': ['Sound.', 'AoWReg.TAoWRegistry.GetMusic',
                'AoWReg.TAoWRegistry.SetMusic'],
}


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=600) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    os.makedirs('re/decomp', exist_ok=True)
    report = []
    for prog, pats in TARGETS.items():
        addrs = {}
        for pat in pats:
            txt = get('/search_functions', name_pattern=pat, limit=2000,
                      program=prog)
            for line in txt.splitlines():
                m = LINE.match(line.strip())
                if m:
                    addrs[m.group('name')] = m.group('addr')
        report.append('==== %s (%d matches) ====' % (prog, len(addrs)))
        for n, a in sorted(addrs.items()):
            report.append('  %s @ %s' % (n, a))
        report.append('')
        if addrs:
            csv = ','.join('0x' + a for a in sorted(set(addrs.values())))
            dec = get('/batch_decompile', functions=csv, program=prog)
            fname = 're/decomp/%s.sound.c' % prog.replace('.', '_')
            with open(fname, 'w', encoding='utf-8') as fh:
                fh.write('/* %s sound-related functions (Ghidra) */\n' % prog)
                fh.write(dec)
            report.append('---- decompiled %d funcs -> %s ----' % (
                len(set(addrs.values())), fname))
            report.append('')
        print('%-18s %d' % (prog, len(addrs)))
    with open('re/decomp/sound_api.txt', 'w', encoding='utf-8') as fh:
        fh.write('\n'.join(report))
    print('wrote re/decomp/sound_api.txt')


if __name__ == '__main__':
    main()
