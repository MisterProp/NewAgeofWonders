import sys
for p in sys.argv[1:]:
    raw = open(p, 'rb').read()
    try:
        txt = raw.decode('utf-16')
    except UnicodeDecodeError:
        txt = raw.decode('utf-8', 'replace')
    txt = '\n'.join(ln for ln in txt.splitlines() if ln.strip())
    open(p, 'w', encoding='utf-8', newline='\n').write(txt + '\n')
    print(p, len(txt))
