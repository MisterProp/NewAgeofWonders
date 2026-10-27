"""dump_move.py - decompile the army-move / structure-entry functions."""
import concurrent.futures
import os
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
PROG = 'AoWEPACK.dpl'
OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   're', 'exports', 'capture')

TARGETS = {
    'TMoveArmyTE.ExecuteMove': 0x557481fc,
    'TMoveArmyTE.FinishMove': 0x55747fbc,
    'TMoveArmyTE.InitializeMove': 0x55747e7c,
    'TMoveArmyTE.DefaultMove': 0x55748498,
    'TMoveArmyTE.SetupNextMove': 0x557483d8,
    'TMoveArmyTE.Validate': 0x55747c20,
    'TMoveArmyTE.Setup': 0x55747ad4,
    'TSelectedArmy.ExecuteMove': 0x55793078,
    'TSelectedArmy.CalculateMovePath': 0x55792900,
    'TStructure.MapFieldMsgProc': 0x5575f730,
    'TStructure.MsgProc': 0x5575f898,
    'TStructure.ArmyPlaced': 0x5575f360,
    'TStructure.ArmyRemoved': 0x5575f364,
    'TStructure.ArmyChanged': 0x5575f368,
    'TStructure.Activate': 0x5575f6c0,
    'TStructure.GetBuilding': 0x5575f008,
    'TStructure.CanPlace': 0x5575ecc8,
    'TStructure.MainValidateMap': 0x5575f4f8,
    'TStructure.ValidateMap': 0x5575f4f4,
    'TStructure.FogChanged': 0x5575ea6c,
    'TCity.ArmyPlaced': 0x557aab78,
    'TCity.ArmyRemoved': 0x557aaba0,
    'TCity.MapFieldMsgProc': 0x557ad614,
    'TCity.MsgProc': 0x557acce4,
    'TProductionPlace.MapFieldMsgProc': 0x557bf3d0,
    'TProductionPlace.MsgProc': 0x557be440,
    'TTower.MapFieldMsgProc': 0x557c36f0,
    'TReflectingPool.MapFieldMsgProc': 0x557d5338,
    'TMoveOnEvent.ArmyPlaced': 0x557d35d4,
    'TAoWHSMap.NotifyPlayerChanged': 0x55777890,
    'TAoWHSMap.ArmyChanged': 0x5577636c,
    'TPlayerStructureIncomeSource.Setup': 0x55761adc,
    'TPlayerStructureIncomeSource.Income': 0x55761af8,
    'TPlayerStructureIncomeSource.Name': 0x55761ae0,
    'TPlayerStructurePowerSource.Setup': 0x55761b38,
    'TPlayerStructurePowerSource.Power': 0x55761b54,
    'TPlayerStructurePowerSource.Select': 0x55761b94,
    'TPlayerStructureResource.ActivateMsg': 0x55761a68,
    'TPlayerStructureResource.GenerateFlagPositions': 0x557619e8,
    'TPlayerStructureResource.PlaceResourceUser': 0x55761a9c,
    'TPlayerStructureResource.MsgProc': 0x55761a78,
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
