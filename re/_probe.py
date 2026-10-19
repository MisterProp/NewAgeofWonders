import json, sys

path = r'c:\GOG Games\Age of Wonders\port\re\ilb_inventory.json'
d = json.load(open(path, encoding='utf-8'))
out = []
out.append('type=%s' % type(d))
if isinstance(d, dict):
    ks = list(d.keys())
    out.append('keys[0..5]=%s' % ks[:5])
    out.append('sample %s -> %s' % (ks[0], json.dumps(d[ks[0]])[:800]))
elif isinstance(d, list):
    out.append('len=%d' % len(d))
    out.append('sample[0]=%s' % json.dumps(d[0])[:800])
open(r'c:\GOG Games\Age of Wonders\port\re\_probe_out.txt', 'w', encoding='ascii', errors='replace').write('\n'.join(out))
print('\n'.join(out))
