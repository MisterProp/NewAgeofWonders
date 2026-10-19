"""Scratch: curate the live MCP map-render report down to the engine classes that
actually draw a game map (hex grid, tiles, terrain, map objects, sprites,
surfaces, image library). Drops UI widgets / dialogs / networking noise.
"""
import os

OUT = r'C:\GOG Games\Age of Wonders\port\tmp_gr'

# class prefixes that belong to the map-render pipeline, per dpl
CURATED = {
    'HSEPack.dpl': ['HSEngine.', 'HSLib.'],
    'AoWEPACK.dpl': ['AoWE.TAoWHSMap', 'AoWE.TAbstractAoWHSMap',
                     'AoWE.TAbstractAoWCombatMap', 'AoWE.TAoWMapField',
                     'AoWE.TAoWMapLevel', 'AoWE.TAoWMapObjectEditor',
                     'AoWE.TAoWNewHSMapSettings', 'AoWE.TAoWMapCampaignSettings',
                     'AoWE.TAoWEngine', 'AoWE.TAgeOfWondersEngine',
                     'AoWHex.', 'AoWTer.', 'IsoHex.', 'Hexagon.', 'Terrain.',
                     'ILTer.', 'RndHex.'],
    'Gfxepack.dpl': ['GFXE.'],
    'Enginep.dpl': ['Engine.TEngine', 'Engine.TObject', 'Engine.TNode',
                    'Engine.TEObject'],
    'Ilpack.dpl': ['ImageLib.'],
    'aowInt.dpl': ['AoWReg.'],
}

report = []
for dpl, prefixes in CURATED.items():
    path = os.path.join(OUT, '_mcp_funcs_' + dpl[:-4] + '.txt')
    if not os.path.exists(path):
        continue
    names = []
    for line in open(path, encoding='utf-8'):
        line = line.strip()
        if not line:
            continue
        names.append(line.split('@')[0].split(' at ')[0].strip())
    hits = [n for n in set(names) if any(n.startswith(p) for p in prefixes)]
    groups = {}
    for n in hits:
        cls = n.rsplit('.', 1)[0]
        groups.setdefault(cls, []).append(n)
    report.append('### %s  (%d curated functions, %d classes)'
                  % (dpl, len(hits), len(groups)))
    for cls in sorted(groups):
        report.append('  %s' % cls)
        for n in sorted(groups[cls]):
            report.append('      ' + n)
    report.append('')

open(os.path.join(OUT, '_mcp_map_render_curated.txt'), 'w',
     encoding='utf-8').write('\n'.join(report))
print('wrote _mcp_map_render_curated.txt')
