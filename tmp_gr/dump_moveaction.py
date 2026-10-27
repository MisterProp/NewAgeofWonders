"""dump_moveaction.py - the field/structure move-action resolution."""
import os
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
PROG = 'AoWEPACK.dpl'
OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   're', 'exports', 'capture')

TARGETS = {
    'TAoWMapField.GetMoveAction': 0x55771ddc,
    'TCity.GetMoveAction': 0x557ad0d4,
    'TAoWMapField.GetPlayer': 0x55771c98,
    'TAoWMapField.GetArmyHS': 0x55771eb0,
    'TAoWMapField.GetDefenseRequirements': 0x55771e48,
    'TAoWMapField.GetDefensePriority': 0x55771e28,
    'TAoWMapField.GetIndependentRelation': 0x55771e04,
    'TAoWMapField.GetProductionControl': 0x55771d64,
    'TAoWMapField.GetWallType': 0x55771e8c,
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
