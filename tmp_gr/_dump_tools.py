import json, sys
d = json.load(open(r'C:\GOG Games\Age of Wonders\port\tmp_gr\_schema.json', encoding='utf-8'))
tools = d['tools']
print('TYPE', type(tools), 'LEN', len(tools))
print('SAMPLE KEYS', list(tools[0].keys()) if isinstance(tools[0], dict) else tools[0])
out = []
want = set(sys.argv[1:]) if len(sys.argv) > 1 else {'search_functions', 'list_functions'}
for t in tools:
    nm = t.get('path', '') if isinstance(t, dict) else str(t)
    if nm in want:
        out.append(json.dumps(t, indent=1))
open(r'C:\GOG Games\Age of Wonders\port\tmp_gr\_tools_dump.txt', 'w', encoding='utf-8').write('\n-----\n'.join(out))
print('WROTE', len(out))
