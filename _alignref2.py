from PIL import Image

full = Image.open('tmp_fullmap.png').convert('RGB')
ref = Image.open('bin/BMRef.png').convert('RGB')
print('full', full.size, 'ref', ref.size)

K = 8
fs = full.resize((full.width // K, full.height // K), Image.BOX)
rs = ref.resize((ref.width // K, ref.height // K), Image.BOX)
fw, fh = fs.size
rw, rh = rs.size
fpx = fs.load()
rpx = rs.load()
print('ds', fs.size, rs.size)

best = None
for oy in range(0, fh - rh):
    for ox in range(0, fw - rw):
        s = 0
        for y in range(rh):
            for x in range(rw):
                a = fpx[ox + x, oy + y]
                b = rpx[x, y]
                s += abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])
        if best is None or s < best[0]:
            best = (s, ox, oy)
bx, by = best[1] * K, best[2] * K
print('coarse full(%d,%d)' % (bx, by))

fullpx = full.load()
refpx = ref.load()
best2 = None
for oy in range(max(0, by - K), min(full.height - ref.height, by + K) + 1):
    for ox in range(max(0, bx - K), min(full.width - ref.width, bx + K) + 1):
        s = 0
        for y in range(0, ref.height, 2):
            for x in range(0, ref.width, 2):
                a = fullpx[ox + x, oy + y]
                b = refpx[x, y]
                s += abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])
        if best2 is None or s < best2[0]:
            best2 = (s, ox, oy)
bx, by = best2[1], best2[2]
print('refined origin', bx, by, 'sad', best2[0],
      'mean', best2[0] / (ref.height // 2 * (ref.width // 2) * 3))
crop = full.crop((bx, by, bx + ref.width, by + ref.height))
crop.save('tmp_port_at_ref.png')
for nm in ('tmp_port_at_ref',):
    im = Image.open(nm + '.png')
    im.resize((im.width * 4, im.height * 4), Image.NEAREST).save(nm + '_4x.png')