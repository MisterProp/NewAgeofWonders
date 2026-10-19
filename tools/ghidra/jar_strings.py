"""jar_strings.py - dump printable strings from every class in a jar.

Used to reverse the GhidraMCP plugin's endpoint/parameter names straight out of
the shipped bytecode (no source available locally).

    python tools/ghidra/jar_strings.py <jar> <out.txt>
"""
import re
import sys
import zipfile

PRINTABLE = re.compile(rb'[\x20-\x7e]{4,}')


def main():
    jar = sys.argv[1]
    out = sys.argv[2]
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    z = zipfile.ZipFile(jar)
    with open(out, 'w', encoding='utf-8') as f:
        for name in sorted(z.namelist()):
            if not name.endswith('.class'):
                continue
            data = z.read(name)
            for m in PRINTABLE.finditer(data):
                f.write('%s\t%s\n' % (name, m.group().decode('ascii')))
    print('wrote %s' % out)


if __name__ == '__main__':
    main()
