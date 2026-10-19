from PIL import Image

full = Image.open('tmp_marker.png').convert('RGB')
ref = Image.open('bin/BMRef.png').convert('RGB')
print('full', full.size, 'ref', ref.size)

K = 4
fsmall = full.resize((full.width // K, full.height // K), Image.BOX)
rsmall = ref.resize((ref.width // K, ref.height // K), Image.BOX)
fw, fh = fsmall.size
rw, rh = rsmall.size
fpx = fsmall.load()
rpx = rsmall.load()
fullpx = full.load()
refpx = ref.load()

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
print('coarse best at full(%d,%d)' % (bx, by))

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
print('refined origin', bx, by, 'sad', best2[0])

crop = full.crop((bx, by, bx + ref.width, by + ref.height))
crop.save('tmp_port_at_ref.png')
Image.blend(crop.convert('RGB'), ref, 0.5).save('tmp_blend_ref.png')
diff = Image.new('RGB', ref.size)
cp = crop.load()
dp = diff.load()
tot = 0
n = 0
for y in range(ref.height):
    for x in range(ref.width):
        d = tuple(abs(cp[x, y][c] - refpx[x, y][c]) for c in range(3))
        tot += sum(d)
        n += 1
        dp[x, y] = (min(255, d[0] * 2), min(255, d[1] * 2), min(255, d[2] * 2))
print('mean abs diff', tot / (n * 3))
diff.save('tmp_refdiff.png')
for nm in ('tmp_port_at_ref', 'tmp_blend_ref', 'tmp_refdiff'):
    im = Image.open(nm + '.png')
    im.resize((im.width * 4, im.height * 4), Image.NEAREST).save(nm + '_4x.png')