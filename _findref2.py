from PIL import Image

full = Image.open('tmp_fullmap.png').convert('RGB')
ref = Image.open('bin/BMRef.png').convert('RGB')
K = 4
best_sad, bx, by = 2128, 864, 1144
# refine at full res around the coarse hit
fpx = full.load()
refpx = ref.load()
tx0, ty0, tx1, ty1 = 62, 40, 100, 69
tw, th = tx1 - tx0, ty1 - ty0
# coarse hit maps tmpl corner (tx0,ty0) to (bx,by); BMRef origin = (bx-tx0, by-ty0)
ox0, oy0 = bx - tx0, by - ty0
best = None
for oy in range(oy0 - K, oy0 + K + 1):
    for ox in range(ox0 - K, ox0 + K + 1):
        s = 0
        for y in range(ty0, ty1):
            for x in range(tx0, tx1):
                a = fpx[ox + x, oy + y]
                b = refpx[x, y]
                s += abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])
        if best is None or s < best[0]:
            best = (s, ox, oy)
s, ox, oy = best
print('refined BMRef origin in the full map: (%d,%d) sad=%d mean=%.1f' % (
    ox, oy, s, s / (tw * th * 3)))
crop = full.crop((ox, oy, ox + ref.width, oy + ref.height))
crop.save('tmp_port_at_ref.png')
# side-by-side zoom
a = ref.resize((ref.width * 3, ref.height * 3), Image.NEAREST)
b = crop.resize((crop.width * 3, crop.height * 3), Image.NEAREST)
canvas = Image.new('RGB', (a.width + b.width + 12, max(a.height, b.height)), (20, 20, 20))
canvas.paste(a, (0, 0))
canvas.paste(b, (a.width + 12, 0))
canvas.save('tmp_cmp_at_ref.png')
print('saved tmp_cmp_at_ref.png', canvas.size)