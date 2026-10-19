import json

path = r'c:\GOG Games\Age of Wonders\port\re\ilb_inventory.json'
d = json.load(open(path, encoding='utf-8'))
out = []
for k in sorted(d):
    kl = k.lower()
    if any(t in kl for t in ('mountain', 'waterhex', 'h_gr', 'hextrans', 'earthhex', 'icehex')):
        v = d[k]
        out.append('=== %s' % k)
        out.append('   keys=%s' % list(v.keys()))
        for key in v.keys():
            if key == 'names' and len(v[key]) > 40:
                out.append('   names[%d]=%s ...' % (len(v[key]), v[key][:40]))
            else:
                out.append('   %s=%s' % (key, json.dumps(v[key])[:2000]))
open(r'c:\GOG Games\Age of Wonders\port\re\_probe_out.txt', 'w', encoding='ascii', errors='replace').write('\n'.join(out))
print(len(out))
