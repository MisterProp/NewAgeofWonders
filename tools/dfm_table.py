"""dfm_table.py - condense a dfm_dump.py text dump into a compact one-line-per
object table of just the layout/asset properties, so the recovered form geometry
can be read at a glance.

    python tools/dfm_table.py <dump.txt> [--all]

Without --all only the interesting properties are printed (geometry, fonts,
colours and image-library indices).
"""
import sys

KEYS = (
    'WinLeft', 'WinTop', 'WinWidth', 'WinHeight', 'AOWFont', 'TextColor',
    'TextSelColor', 'Text', 'ILButton', 'ILIndexUp', 'ILIndexDown',
    'ILIndexLit', 'ILFrame', 'ILTexture', 'ILIndexFrame', 'ILIndexTexture',
    'ILIndexTNormal', 'ILIndexTSelected', 'ILEdit', 'ILIndex', 'ILIInc',
    'ILIDec', 'ILTBack', 'ILLTab', 'ILIndexIncUp', 'ILIndexIncDown',
    'ILIndexDecUp', 'ILIndexDecDown', 'ILIndexBack', 'ILIndexTab', 'ImageLib',
    'CurrentImage', 'ILTextureNormal', 'ILTextureSelected', 'Tag', 'Visible',
    'ItemHeight', 'TextAlign', 'OnDrawBack', 'OnDrawItem', 'Alignment.AlignWidth',
    'Alignment.AlignHeight',
)


def main():
    path = sys.argv[1]
    show_all = '--all' in sys.argv[2:]
    out = []
    obj = None
    depth = 0
    for raw in open(path, encoding='utf-8', errors='replace'):
        line = raw.rstrip('\n')
        s = line.strip()
        if s.startswith('object '):
            # flush previous
            if obj:
                out.append(obj)
            indent = len(line) - len(line.lstrip())
            obj = {'depth': indent, 'props': [], 'line': s}
        elif obj is not None and '=' in s:
            k, v = s.split('=', 1)
            k = k.strip()
            if show_all or k in KEYS:
                obj['props'].append((k, v.strip()))
    if obj:
        out.append(obj)
    for o in out:
        if not o['props'] and not show_all:
            print(' ' * o['depth'] + o['line'])
            continue
        print(' ' * o['depth'] + o['line'])
        for k, v in o['props']:
            print(' ' * (o['depth'] + 2) + '%-22s %s' % (k, v))


main()
