"""common.py - shared helpers for the AoW1 port toolchain."""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
GAME_ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
PORT_ROOT = os.path.abspath(os.path.join(HERE, '..'))
RE_ROOT = os.path.join(PORT_ROOT, 're')
ASSET_ROOT = os.path.join(PORT_ROOT, 'assets')


def ensure_dir(p):
    os.makedirs(p, exist_ok=True)
    return p


def iter_binaries(root=None):
    """Yield (path, kind) for every PE binary in the game root.

    kind is 'exe' or 'dpl' (Delphi package / DLL).
    """
    root = root or GAME_ROOT
    for dirpath, _dirs, files in os.walk(root):
        if os.sep + 'port' + os.sep in dirpath + os.sep:
            continue
        for f in files:
            low = f.lower()
            if low.endswith(('.exe', '.dll', '.dpl')):
                kind = 'dpl' if low.endswith('.dpl') else 'exe'
                yield os.path.join(dirpath, f), kind


def demangle_delphi(name):
    """Best-effort pretty-print of a Delphi export name.

    Borland package exports look like '@Unit@Method$qqrx...'.
    This strips the decoration for readability; it is lossy on purpose.
    """
    if not name.startswith('@'):
        return name
    body = name[1:]
    body = body.split('$q', 1)[0]  # drop parameter signature
    return body.replace('@', '::')


def run(cmd, **kw):
    return subprocess.run(cmd, check=False, **kw)


if __name__ == '__main__':
    print('GAME_ROOT =', GAME_ROOT)
    print('PORT_ROOT =', PORT_ROOT)
    n = sum(1 for _ in iter_binaries())
    print('binaries =', n)
    sys.exit(0)
