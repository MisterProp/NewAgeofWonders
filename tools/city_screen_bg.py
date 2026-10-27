"""city_screen_bg.py - build the city screen's static backdrop asset.

`TCityScreen` (AoW.exe TCityScreen, DFM assets/dfm/AoW/TCITYSCREEN.dfm) draws
its 640x432 dialog from `Int\\Scenes\\CityWin.ILB` id 0 (`CityBg.BMP`) plus a
runtime-generated tactical city map in `MapView`.  The reference capture
`bin/UnitCreationRef.png` shows the *original* 640x480 screen cropped to
638x472 (2 px from the right edge and 8 px from the bottom); its city skin is
not the CityBg shipped with this GOG install (the parchment/city-view strip
differs), so the port ships the reference's own 0..432 dialog area as a static
backdrop and draws every live control (text, faces, icons, queue, buttons) on
top of it.

This tool extracts that backdrop into `assets/cityscreen.bmp` - a plain
uncompressed 24-bit BMP, which `Scene::load_city_art` reads at runtime (the
port deliberately has no PNG decoder).  The missing two columns on the right
are filled by repeating the last captured column.  The 48 px bar band of the
capture is kept in the file, but the renderer clips every city blit at the
432 px dialog edge so the live world-map bar below remains visible.

    python tools/city_screen_bg.py bin/UnitCreationRef.png assets/cityscreen.bmp
"""
import struct
import sys

from PIL import Image


def write_bmp24(path, im):
    im = im.convert('RGB')
    w, h = im.size
    row = (w * 3 + 3) & ~3
    data = bytearray()
    for y in range(h - 1, -1, -1):          # bottom-up rows
        line = bytearray()
        for x in range(w):
            r, g, b = im.getpixel((x, y))
            line += bytes((b, g, r))
        line += b'\0' * (row - len(line))
        data += line
    size = 54 + len(data)
    hdr = struct.pack('<2sIHHI', b'BM', size, 0, 0, 54)
    info = struct.pack('<IiiHHIIiiII', 40, w, h, 1, 24, 0, len(data),
                       2835, 2835, 0, 0)
    with open(path, 'wb') as f:
        f.write(hdr + info + data)


# The dynamic controls the port paints live. They are erased from the secondary
# backdrop (`cityscreen_clean.bmp`) by mirror-tiling a blank patch of the same
# panel over each block, so a different city does not show the reference city's
# text underneath its own. The exact backdrop stays the byte-for-byte capture
# and is used for the reference city itself.
#
#   (x0, y0, x1, y1)            region to erase
#   (sx, sy, sw, sh)            blank source patch on the same surface
# Blank body patches from the Enhancements box (the right-hand side of its
# header and memo is free of text). They are large enough that mirror-tiling
# keeps the crack pattern organic instead of reading as a repeating stamp.
BODY_HDR = (525, 230, 50, 55)
BODY_MEMO = (525, 296, 50, 38)
LIVE_REGIONS = [
    ((106, 224, 226, 289), BODY_HDR),              # PopulationHdr
    ((52, 288, 226, 340), BODY_MEMO),              # PopulationMemo
    ((285, 224, 405, 289), BODY_HDR),              # IncomeHdr
    ((231, 288, 405, 340), BODY_MEMO),             # IncomeMemo + gold values
    ((464, 224, 532, 289), BODY_HDR),              # EnhancementsHdr (text only)
    ((410, 288, 532, 340), BODY_MEMO),             # EnhancementsMemo
    ((234, 360, 412, 400), BODY_MEMO),             # ActiveAction / Turns
    ((10, 354, 168, 422), (120, 360, 40, 9)),      # QueueLB rows
]


def clean_title(img):
    """Remove the reference city's StructName from the green title plaque.

    The plaque is a curved, leaf-textured surface, so a rectangular patch
    leaves a very visible block. Mask only the white glyph ink + its black
    shadow and fill each masked pixel from the nearest unmasked plaque pixel;
    the shape and border are untouched.
    """
    import numpy as np
    out = img.copy()
    x0, y0, x1, y1 = 268, 8, 382, 36
    region = out[y0:y1, x0:x1]
    mx = region.max(axis=2)
    mn = region.min(axis=2)
    mask = (mx > 150) | ((mx < 42) & ((mx - mn) < 16))
    # Dilate once for the anti-aliased edge.
    grown = mask.copy()
    grown[1:, :] |= mask[:-1, :]
    grown[:-1, :] |= mask[1:, :]
    grown[:, 1:] |= mask[:, :-1]
    grown[:, :-1] |= mask[:, 1:]
    mask = grown
    for _ in range(12):
        changed = False
        ys, xs = np.nonzero(mask)
        for y, x in zip(ys, xs):
            if not mask[y, x]:
                continue
            best = None
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    if dx == 0 and dy == 0:
                        continue
                    ny, nx = y + dy, x + dx
                    if ny < 0 or nx < 0 or ny >= mask.shape[0] or nx >= mask.shape[1]:
                        continue
                    if mask[ny, nx]:
                        continue
                    d = dx * dx + dy * dy
                    if best is None or d < best[0]:
                        best = (d, region[ny, nx].copy())
            if best is not None:
                region[y, x] = best[1]
                mask[y, x] = False
                changed = True
        if not changed:
            break
    return out


def write_clean(src, dst):
    import cv2
    import numpy as np
    img = cv2.imread(src)
    out = img.copy()
    for (x0, y0, x1, y1), (sx, sy, sw, sh) in LIVE_REGIONS:
        patch = img[sy:sy + sh, sx:sx + sw]
        tile = np.vstack([
            np.hstack([patch, patch[:, ::-1]]),
            np.hstack([patch[::-1, :], patch[::-1, ::-1]]),
        ])
        th, tw = tile.shape[:2]
        for y in range(y0, y1):
            for x in range(x0, x1):
                out[y, x] = tile[(y - y0) % th, (x - x0) % tw]
    out = clean_title(out)
    cv2.imwrite(dst, out)


def main():
    src = sys.argv[1] if len(sys.argv) > 1 else r'bin/UnitCreationRef.png'
    dst = sys.argv[2] if len(sys.argv) > 2 else r'assets/cityscreen.bmp'
    ref = Image.open(src).convert('RGB')
    if ref.size != (638, 472):
        raise SystemExit('unexpected reference size %s' % (ref.size,))
    # The Dlg is 640x432 and the 48 px world-map bar below it stays visible; the
    # capture is 1:1 but cropped 2 px on the right and 8 px on the bottom, so the
    # backdrop is the full 640x480 frame with the two missing columns / eight
    # bottom rows extended from the captured edge.  At runtime the city blits
    # stop at y=432, so the captured bar band is stored but never drawn: the
    # live overworld bar is used instead.
    bg = Image.new('RGB', (640, 480))
    bg.paste(ref.crop((0, 0, 638, 472)), (0, 0))
    last_col = ref.crop((637, 0, 638, 472))
    for x in (638, 639):
        bg.paste(last_col, (x, 0))
    last_row = bg.crop((0, 471, 640, 472))
    for y in range(472, 480):
        bg.paste(last_row, (0, y))
    write_bmp24(dst, bg)
    print('wrote %s  %dx%d' % (dst, bg.size[0], bg.size[1]))
    clean = dst.replace('.bmp', '_clean.bmp')
    write_clean(dst, clean)
    print('wrote %s (live-control areas mirror-tiled)' % clean)


if __name__ == '__main__':
    main()
