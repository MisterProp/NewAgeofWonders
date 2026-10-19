"""scan_exports.py - which AoW module owns the menu/title symbols?

The PE export tables of the Delphi packages carry mangled names, but Delphi
also emits plain class/method strings into the modules. dpl_index.py stored
every candidate string per module in re/exports/module_index.json; this script
greps those lists so we know which binary to reverse for the title screen.
"""
import json
import re
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

PATH = r'c:\GOG Games\Age of Wonders\port\re\exports\module_index.json'


def main():
    pat = sys.argv[1] if len(sys.argv) > 1 else (
        r'MainMenu|MenuItemIntf|StartMenu|TitleScene|TitleWindow|OrbFrame|'
        r'TitleFull|ScrollBackg|CompanyLogos|LogoTitle')
    rx = re.compile(pat, re.I)
    data = json.load(open(PATH, encoding='utf-8'))
    for m in data:
        hits = [e for e in m.get('exports', [])
                if isinstance(e, str) and rx.search(e)]
        if not hits:
            continue
        print('%-18s size=%-9d hits=%d' % (m['module'], m['size'], len(hits)))
        for h in hits[:25]:
            print('     ', h[:160])


if __name__ == '__main__':
    main()
