"""dump_capture.py - decompile the building-acquisition functions from AoWEPACK.dpl.

Saves one .c file per address under re/exports/capture/ and also prints the
callers/callees so the ownership path can be reconstructed.
"""
import concurrent.futures
import os
import urllib.parse
import urllib.request

BASE = 'http://127.0.0.1:8089'
PROG = 'AoWEPACK.dpl'
OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   're', 'exports', 'capture')

TARGETS = {
    'TPlayerStructure.StructureCaptured': 0x55760ac4,
    'TPlayerStructure.SetPlayer': 0x55760d08,
    'TPlayerStructure.UpdatePlayer': 0x55760980,
    'TPlayerStructure.UpdatePlayerSeen': 0x557610bc,
    'TPlayerStructure.UpdateFlagID': 0x55760ed8,
    'TPlayerStructure.Activate': 0x55761394,
    'TPlayerStructure.Deactivate': 0x557613e4,
    'TPlayerStructure.MsgProc': 0x557614f4,
    'TPlayerStructure.MapFieldMsgProc': 0x55761658,
    'TPlayerStructure.CanMoveOn': 0x55761428,
    'TPlayerStructure.CanMoveOver': 0x55761478,
    'TPlayerStructure.GetMoveAction': 0x55761620,
    'TPlayerStructure.NewDay': 0x55760a90,
    'TPlayerStructure.NewTurn': 0x55760a70,
    'TPlayerStructure.BuildingDone': 0x557608f4,
    'TPlayerStructure.CanRebuild': 0x55760920,
    'TPlayerStructure.ExecuteRebuild': 0x55760928,
    'TPlayerStructure.ExecuteBuild': 0x55760954,
    'TPlayerStructure.Destroy': 0x55760778,
    'TPlayerStructure.RegisterVictoryLocations': 0x5576082c,
    'TPlayerStructure.SeatedPlayerChanged': 0x5576137c,
    'TPlayerStructure.ArmyChanged': 0x55760f30,
    'TPlayerStructure.ArmyPlaced': 0x55760f24,
    'TPlayerStructure.FinalizeCombat': 0x55760cd4,
    'TPlayerStructure.GetCombatEventText': 0x55760c1c,
    'TCity.StructureCaptured': 0x557aa0a4,
    # The class-reference correction: SetPlayer is at +0x1f0 = 0x557aa2c8,
    # UpdateFlagID at +0x1fc = 0x557ac8b0 and UpdatePlayer at +0x220 = 0x557aa9fc.
    'TCity.SetPlayer': 0x557aa2c8,
    'TCity.UpdateFlagID': 0x557ac8b0,
    'TCity.UpdatePlayer': 0x557aa9fc,
    'TCity.FinalizeCombat': 0x557acefc,
    'TCity.GetCombatEventText': 0x557acdb8,
    'TProductionPlace.StructureCaptured': 0x557bed44,
    'TProductionPlace.SetPlayer': 0x557befcc,
    'TMine.SetPlayer': 0x557b4564,
    'TPowerNode.SetPlayer': 0x557d0584,
    'TAltar.SetPlayer': 0x557cf234,
    'TReflectingPool.SetPlayer': 0x557d4f1c,
    'TProductionControl.PlayerChanged': 0x5575cef0,
    'TStructure.GetRebuildInfo': 0x5575f18c,
    'TCity.GetRebuildInfo': 0x557ab50c,
    'TCity.CheckRebellion': 0x557ab534,
    'TCity.IncommingNegotiateOffer': 0x557ad300,
    'TPlayer.CheckCaptureLocations': 0x55753778,
    'TPlayer.CaptureLocationsLeft': 0x557537a8,
    'TStructure.CanRaze': 0x5575fb80,
    'TStructure.RazeEx': 0x5576023c,
    'TStructure.Raze': 0x557602f8,
    'TStructure.ExecuteRaze': 0x5575ffc8,
    'TStructure.GenerateRazeDefenders': 0x5575fd1c,
    'TStructure.PlaceRazeDefenders': 0x5575fe84,
    'TStructure.SetupRazeDefenderAG': 0x5575fe0c,
    'TStructure.ShowRazeAnimation': 0x5575ff90,
    'TStructure.GetRazeable': 0x5575eddc,
    'TStructure.SetRazed': 0x5575ede8,
    'TCity.ExecuteRaze': 0x557ab414,
    'TCity.SetupRazeDefenderAG': 0x557aa378,
    'TCity.SetRazed': 0x557ab47c,
    'TMine.SetRazed': 0x557b4648,
    'TTower.SetRazed': 0x557c3684,
    'TPowerNode.SetRazed': 0x557d03f8,
    'TProductionPlace.SetRazed': 0x557be560,
    'TCrop.SetCropOwner': 0x557a5cbc,
    'TCrop.LinkToCropOwner': 0x557a5dac,
    'TPlayerCropStructure.SetPlayer': 0x557a6278,
    'TPlayerCropStructure.Activate': 0x557a62e4,
    'TPlayerCropStructure.Deactivate': 0x557a6338,
    'TAoWHSMap.StructureChanged': 0x557763b0,
    'TAoWHSMap.StructureDestroyed': 0x557763c8,
    'TAoWHSMap.BeginStructureUpdate': 0x55776390,
    'TAoWHSMap.EndStructureUpdate': 0x55776398,
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
    except Exception as exc:  # pragma: no cover
        src = '/* decompile failed: %s */\n' % exc
    try:
        callers = get('/get_function_callers', address=hex(addr),
                      program=PROG, limit=100)
    except Exception as exc:  # pragma: no cover
        callers = 'callers failed: %s' % exc
    try:
        callees = get('/get_function_callees', address=hex(addr),
                      program=PROG, limit=100)
    except Exception as exc:  # pragma: no cover
        callees = 'callees failed: %s' % exc
    return name, addr, src, callers, callees


def main():
    os.makedirs(OUT, exist_ok=True)
    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as pool:
        results = list(pool.map(one, TARGETS.items()))
    idx = []
    for name, addr, src, callers, callees in results:
        safe = name.replace('.', '_')
        path = os.path.join(OUT, '%06x_%s.c' % (addr, safe))
        with open(path, 'w', encoding='utf-8') as fh:
            fh.write('/* %s @ %06x  (AoWEPACK.dpl)\n\nCALLERS:\n%s\n\n'
                     'CALLEES:\n%s*/\n\n' % (name, addr, callers, callees))
            fh.write(src)
        idx.append('%s @ %06x -> %s' % (name, addr, os.path.basename(path)))
    with open(os.path.join(OUT, 'index.txt'), 'w', encoding='utf-8') as fh:
        fh.write('\n'.join(idx) + '\n')
    print('wrote %d decompiles to %s' % (len(idx), OUT))


if __name__ == '__main__':
    main()
