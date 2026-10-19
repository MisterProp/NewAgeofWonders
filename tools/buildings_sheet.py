"""buildings_sheet.py - one contact sheet of every world-map building sprite.

    python tools/buildings_sheet.py <game-root> <out.png> [--cell N] [--cols N]

For every map-object resource in `<game-root>/Release/Release.hss` that carries an
image-sequence list (rw tags 9/10 - the structures: cities, mines, towers, nodes,
farms, shipyards, ...), this walks the sequence *chain* the renderer draws
(`aow::body_sequence` + `aow::image_sequence_chain`) and lays each sequence's first
frame out in a grid over a checkerboard, so a sprite that still carries its
background is obvious at a glance. Each cell's library/frame is printed in
row-major order.

It reuses the port's own decoder through `aow_tools.exe png` (so the sheet shows
exactly what the renderer draws), `tools/structseq_probe.py` to read the
sequence lists, and a stdlib PNG writer. See docs/17_BUILDING_ANIM.md.
"""
import os
import struct
import subprocess
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import structseq_probe as ssp  # noqa: E402  (stdlib-only sibling tool)

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
AOW = os.path.join(PORT, 'bin', 'aow_tools.exe')
EFFECTS = ('smoke', 'twinkgr')


def is_effect(lib):
    base = lib.replace('/', '\\').split('\\')[-1].lower()
    base = base.rsplit('.', 1)[0]
    return base in EFFECTS


def first_frame(seq):
    """The frame the renderer bakes at clock 0 (image_sequence_frame, mode 0/4)."""
    mode = seq['mode'] & 0x7F
    if mode in (4, 5) and seq['ft']:
        return seq['ft'][0]
    if mode == 0:
        return seq['frame']
    return 0


def structure_sprites(hss):
    """[(library, frame)] for every body + chained sequence the renderer draws."""
    data = open(hss, 'rb').read()
    R = data[ssp.u32(data, 0):]
    outer = ssp.read_table(R, 0)
    topo = outer['base'] + ssp.find(outer, 0)
    top = ssp.read_table(R, topo)
    listt = ssp.read_table(R, top['base'] + ssp.find(top, 5))
    ch = ssp.read_table(R, listt['base'] + ssp.find(listt, 1))
    out = []
    seen = set()
    for i in range(len(ch['entries'])):
        a = ch['base'] + ch['entries'][i][1]
        sub = ssp.read_table(R, a + 4)
        o9 = ssp.find(sub, 9)
        if o9 is None:
            o9 = ssp.find(sub, 10)
        if o9 is None:
            continue
        r = ssp.Reader(R, sub['base'] + o9)
        count = r.u32()
        seqs = []
        for _ in range(count):
            seqs.append(None if r.u8() != 0 else ssp.read_seq(r))
        body = None
        for s in seqs:  # aow::body_sequence: the first non-effect sequence
            q = s
            while q is not None:
                if q['lib'] and not is_effect(q['lib']):
                    body = q
                    break
                q = q['chain']
            if body is not None:
                break
        if body is None:
            continue
        q = body  # aow::image_sequence_chain: the body + every chained overlay
        while q is not None:
            if q['lib'] and not is_effect(q['lib']):
                key = (q['lib'].lower(), first_frame(q))
                if key not in seen:
                    seen.add(key)
                    out.append((q['lib'], first_frame(q)))
            q = q['chain']
    return out


def find_ilb(root, name):
    want = name.replace('/', '\\').split('\\')[-1].lower()
    for dirpath, _dirs, files in os.walk(os.path.join(root, 'Images')):
        for f in files:
            if f.lower() == want:
                return os.path.join(dirpath, f)
    return None


def read_png(path):
    data = open(path, 'rb').read()
    w = h = bpp = None
    idat = b''
    pos = 8
    while pos + 8 <= len(data):
        (ln,) = struct.unpack_from('>I', data, pos)
        ctype = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + ln]
        if ctype == b'IHDR':
            w, h, depth, color, _a, _b, _c = struct.unpack('>IIBBBBB', body)
            bpp = 4 if color == 6 else 3
        elif ctype == b'IDAT':
            idat += body
        pos += 12 + ln
    raw = zlib.decompress(idat)
    stride = w * bpp
    rows = [raw[y * (stride + 1) + 1:y * (stride + 1) + 1 + stride]
            for y in range(h)]
    return w, h, bpp, rows


def write_png(path, w, h, rows):
    raw = b''.join(b'\x00' + r for r in rows)

    def chunk(t, body):
        return (struct.pack('>I', len(body)) + t + body +
                struct.pack('>I', zlib.crc32(t + body) & 0xFFFFFFFF))

    open(path, 'wb').write(
        b'\x89PNG\r\n\x1a\n' +
        chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0)) +
        chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b''))


def main():
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    root = sys.argv[1]
    out = sys.argv[2]
    cell, cols = 72, 12
    for i, a in enumerate(sys.argv[3:]):
        if a == '--cell' and 4 + i < len(sys.argv):
            cell = int(sys.argv[4 + i])
        if a == '--cols' and 4 + i < len(sys.argv):
            cols = int(sys.argv[4 + i])

    sprites = structure_sprites(os.path.join(root, 'Release', 'Release.hss'))
    cells = []
    tmp = os.path.join(PORT, 'tmp_sheet_cell.png')
    for lib, frame in sprites:
        path = find_ilb(root, lib)
        if path is None:
            continue
        r = subprocess.run([AOW, 'png', path, str(frame), tmp, '--id'],
                           capture_output=True)
        if r.returncode != 0 or not os.path.exists(tmp):
            continue
        w, h, bpp, rows = read_png(tmp)
        cells.append((lib + ' #' + str(frame), w, h, bpp, rows))
    if os.path.exists(tmp):
        os.remove(tmp)

    rows_n = (len(cells) + cols - 1) // cols
    W, H = cols * cell, max(1, rows_n) * cell
    # Checkerboard so a transparent (removed) background is obvious.
    canvas = []
    for y in range(H):
        row = bytearray()
        for x in range(W):
            c = 200 if ((x // 8) + (y // 8)) % 2 else 140
            row += bytes((c, c, c, 255))
        canvas.append(row)
    for idx, (_label, w, h, bpp, srows) in enumerate(cells):
        cx = (idx % cols) * cell
        cy = (idx // cols) * cell
        ox = cx + (cell - w) // 2
        oy = cy + (cell - h) // 2
        for y in range(h):
            dy = oy + y
            if dy < 0 or dy >= H:
                continue
            srow = srows[y]
            d = canvas[dy]
            for x in range(w):
                dx = ox + x
                if dx < 0 or dx >= W:
                    continue
                p = srow[x * bpp:(x + 1) * bpp]
                if len(p) < 4 or p[3] == 0:
                    continue
                a = p[3]
                di = dx * 4
                if a == 255:
                    d[di], d[di + 1], d[di + 2] = p[0], p[1], p[2]
                else:
                    inv = 255 - a
                    d[di] = (p[0] * a + d[di] * inv) // 255
                    d[di + 1] = (p[1] * a + d[di + 1] * inv) // 255
                    d[di + 2] = (p[2] * a + d[di + 2] * inv) // 255
    write_png(out, W, H, [bytes(r) for r in canvas])
    print('wrote %s  %dx%d  %d sprites (%d cols)'
          % (out, W, H, len(cells), cols))
    for i, (label, _w, _h, _b, _r) in enumerate(cells):
        print('  [%3d] %s' % (i, label))


if __name__ == '__main__':
    main()

