"""probe_endpoints.py - discover which GhidraMCP routes exist.

Read-only probe: GET every candidate path with no/!minimal params and report
status + a short sample, so we know what the loaded plugin build supports
(notably whether we can switch the current program).

    python tools/ghidra/probe_endpoints.py
"""
import sys
import urllib.error
import urllib.request

BASE = 'http://127.0.0.1:8089'

PATHS = [
    '/', '/get_current_program', '/current_program', '/list_programs',
    '/programs', '/open_program', '/set_current_program', '/get_program_info',
    '/list_methods', '/list_functions', '/list_classes', '/list_segments',
    '/list_imports', '/list_exports', '/list_strings', '/list_data',
    '/get_function_by_address', '/get_function_by_name', '/get_current_function',
    '/decompile_function', '/search_functions', '/searchFunctions',
    '/xrefs_to', '/xrefs_from', '/function_xrefs',
]


def probe(path):
    try:
        with urllib.request.urlopen(BASE + path, timeout=30) as r:
            body = r.read().decode('utf-8', 'replace')
            return r.status, body
    except urllib.error.HTTPError as exc:
        return exc.code, exc.read().decode('utf-8', 'replace')[:120]
    except Exception as exc:  # noqa: BLE001
        return 0, str(exc)


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    for p in PATHS:
        st, body = probe(p)
        sample = body[:110].replace('\n', ' | ')
        print('%-28s %-4s len=%-7d %s' % (p, st, len(body), sample))
