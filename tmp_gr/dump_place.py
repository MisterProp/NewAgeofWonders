"""dump_place.py - decompile army placement (the path that lands on a structure)."""
import concurrent.futures
import os
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
PROG = 'AoWEPACK.dpl'
OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   're', 'exports', 'capture')

TARGETS = {
    'TArmyHS.PlaceOnMap': 0x557909f4,
    'TArmyHS.PlaceHX': 0x557911f8,
    'TArmyHS.PlacePrivate': 0x55791278,
    'TArmyHS.PlaceEx': 0x55791f58,
    'TArmy.Place': 0x5578ddc4,
    'TArmy.CanPlace': 0x5578ddc4,
    'TArmy.GetMoveAction': 0x5578f98c,
    'TArmy.ResetDestinationPos': 0x5578f0a0,
    'TStructure.PlaceOnMap': 0x5575e7c0,
    'TCity.PlaceOnMap': 0x557acc60,
    'TPlayer.PlaceArmy': 0x55751c00,
    'TPlayer.CheckCaptureLocations': 0x55753778,
    'TPlayer.CaptureLocationsLeft': 0x557537a8,
    'TAoWMapField.GetArmyHS': 0x55771eb0,
    'TAoWMapField.SetExplored': 0x55771c04,
    'TAoWMapField.GetExplored': 0x55771bf4,
    'TAoWMapLevel.ExploreArea': 0x5577212c,
}


def get(path, **params):
    url = BASE + path
    if params:
        url += '?' + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=300) as r:
        return r.read().decode('utf-8', 'replace')


def one(item):
    name, addr = item
    try:
        src = get('/decompile_function', address=hex(addr), program=PROG)
    except Exception as exc:
        src = '/* decompile failed: %s */\n' % exc
    try:
        callers = get('/get_function_callers', address=hex(addr),
                      program=PROG, limit=100)
    except Exception as exc:
        callers = 'callers failed: %s' % exc
    return name, addr, src, callers


def main():
    os.makedirs(OUT, exist_ok=True)
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        results = list(pool.map(one, TARGETS.items()))
    for name, addr, src, callers in results:
        safe = name.replace('.', '_')
        path = os.path.join(OUT, '%06x_%s.c' % (addr, safe))
        with open(path, 'w', encoding='utf-8') as fh:
            fh.write('/* %s @ %06x  (AoWEPACK.dpl)\n\nCALLERS:\n%s*/\n\n'
                     % (name, addr, callers))
            fh.write(src)
    print('wrote %d decompiles' % len(results))


if __name__ == '__main__':
    main()
