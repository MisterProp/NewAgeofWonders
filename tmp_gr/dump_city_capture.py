"""dump_city_capture.py - the subclass SetPlayer / capture overrides."""
import os
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
PROG = 'AoWEPACK.dpl'
OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   're', 'exports', 'capture')

TARGETS = {
    'TCity.SetPlayer': 0x557aa2c8,
    'TCity.UpdateFlagID': 0x557ac8b0,
    'TCity.UpdatePlayer': 0x557aa9fc,
    'TCity.FinalizeCombat': 0x557acefc,
    'TCity.GetCombatEventText': 0x557acdb8,
    'TProductionPlace.SetPlayer': 0x557befcc,
    'TMine.SetPlayer': 0x557b4564,
    'TPowerNode.SetPlayer': 0x557d0584,
    'TAltar.SetPlayer': 0x557cf234,
    'TReflectingPool.SetPlayer': 0x557d4f1c,
    'TProductionPlace.BuildingDone': 0x557bf32c,
    'TProductionPlace.GenerateDefenders': 0x557bf1a8,
    'TProductionPlace.Update': 0x557bece4,
    'TProductionPlace.NewDay': 0x557bf2f8,
    'TProductionPlace.NewTurn': 0x557bf3b0,
    'TPlayerCropStructure.SetPlayer': 0x557a6278,
    'TPlayerCropStructure.UpdatePlayer': 0x557a6278,
    'TCrop.SetCropOwner': 0x557a5cbc,
    'TCrop.LinkToCropOwner': 0x557a5dac,
    'TPlayer.CheckCaptureLocations': 0x55753778,
    'TPlayer.CaptureLocationsLeft': 0x557537a8,
    'TPlayerStructure.UpdatePlayerSeen': 0x557610bc,
    'TPlayerStructure.UpdateUnfog': 0x55761228,
    'TPlayerStructure.UpdateExploration': 0x55761148,
    'TPlayerStructure.UpdateFlagID': 0x55760ed8,
}


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=300) as r:
        return r.read().decode('utf-8', 'replace')


def main():
    os.makedirs(OUT, exist_ok=True)
    for name, addr in TARGETS.items():
        try:
            src = get('/decompile_function', address=hex(addr), program=PROG)
        except Exception as exc:
            src = '/* decompile failed: %s */\n' % exc
        path = os.path.join(OUT, '%06x_%s.c' % (addr, name.replace('.', '_')))
        with open(path, 'w', encoding='utf-8') as fh:
            fh.write('/* %s @ %06x  (AoWEPACK.dpl) */\n\n' % (name, addr))
            fh.write(src)
    print('done')


if __name__ == '__main__':
    main()
