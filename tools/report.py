#!/usr/bin/env python3
"""report.py - build re/report.html from the JSON produced by the pipeline."""
import os
import json
import html

import common


def load(name):
    p = os.path.join(common.RE_ROOT, name)
    if not os.path.exists(p):
        return None
    with open(p, 'r', encoding='utf-8') as f:
        return json.load(f)


def table(rows, headers):
    out = ['<table><tr>' + ''.join(f'<th>{h}</th>' for h in headers) + '</tr>']
    for r in rows:
        out.append('<tr>' + ''.join(f'<td>{html.escape(str(c))}</td>'
                                    for c in r) + '</tr>')
    out.append('</table>')
    return '\n'.join(out)


def main():
    mods = load('exports\\module_index.json') or load('exports/module_index.json')
    ilbs = load('ilb_inventory.json') or {}
    pfs = load('pfs_probe.json') or {}

    parts = ['<!doctype html><meta charset="utf-8">',
             '<title>Age of Wonders - RE report</title>',
             '<style>body{font-family:Segoe UI,Arial;margin:2rem;background:#181a1b;'
             'color:#ddd}table{border-collapse:collapse;margin:1rem 0}'
             'td,th{border:1px solid #333;padding:4px 8px;font-size:13px}'
             'th{background:#222}h2{border-bottom:1px solid #333;padding-bottom:4px}'
             'code{color:#7fd}</style>',
             '<h1>Age of Wonders 1 - reverse-engineering report</h1>']

    if mods:
        parts.append(f'<h2>Modules ({len(mods)})</h2>')
        rows = [(m['module'], m['size'], m['dfm_forms'], m['export_count'],
                 ', '.join(m['imports'])) for m in
                sorted(mods, key=lambda m: -m['export_count'])]
        parts.append(table(rows, ['module', 'size', 'forms', 'exports', 'imports']))

    if ilbs:
        names = sum(len(v.get('names', [])) for v in ilbs.values())
        parts.append(f'<h2>ILB libraries ({len(ilbs)} files, {names} entries)</h2>')
        parts.append('<details><summary>list</summary>')
        rows = [(k, v['header']['version'], v['header'].get('count', '-'),
                 len(v.get('names', []))) for k, v in sorted(ilbs.items())]
        parts.append(table(rows, ['file', 'version', 'tag', 'names']))
        parts.append('</details>')

    if pfs:
        parts.append(f'<h2>PFS rule tables ({len(pfs)})</h2>')
        rows = []
        for k, v in sorted(pfs.items()):
            sample = '; '.join(s['text'][:48] for s in v['strings'][:3])
            rows.append((k, v['size'], sample))
        parts.append(table(rows, ['file', 'size', 'sample strings']))

    parts.append('<h2>Next</h2><p>See <code>docs/03_ROADMAP.md</code>. '
                 'Highest-ROI task: crack <code>Ability.pfs</code>.</p>')

    dest = os.path.join(common.ensure_dir(common.RE_ROOT), 'report.html')
    with open(dest, 'w', encoding='utf-8') as f:
        f.write('\n'.join(parts))
    print('wrote', dest)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
