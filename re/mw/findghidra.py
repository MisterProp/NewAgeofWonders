import os, socket
for p in (8080, 8081, 8089):
    s = socket.socket()
    s.settimeout(0.4)
    try:
        s.connect(('127.0.0.1', p))
        print('port', p, 'OPEN')
    except OSError:
        print('port', p, 'closed')
    finally:
        s.close()
roots = ['C:\\', 'C:\\Program Files', 'C:\\Program Files (x86)',
         os.path.expanduser('~'), 'C:\\tools', 'C:\\dev']
for r in roots:
    try:
        for n in os.listdir(r):
            if 'ghidra' in n.lower():
                print('ghidra dir:', os.path.join(r, n))
    except OSError:
        pass
for env in ('GHIDRA_HOME', 'AOW_OUT'):
    print(env, '=', os.environ.get(env))
