from PIL import Image

full = Image.open('tmp_fullmap.png').convert('RGB')
ref = Image.open('bin/BMRef.png').convert('RGB')

# marker-free template: BMRef's bottom-right (water + shore)
tx0, ty0, tx1, ty1 = 62, 40, 100, 69
tmpl = ref.crop((tx0, ty0, tx1, ty1))
K = 4
fs = full.resize((full.width // K, full.height // K), Image.BOX)
ts = tmpl.resize((max(1, tmpl.width // K), max(1, tmpl.height // K)), Image.BOX)
fw, fh = fs.size
tw, th = ts.size
fpx = fs.load()
tpx = ts.load()
print('full ds', fs.size, 'tmpl ds', ts.size)

best = []
for oy in range(0, fh - th):
    for ox in range(0, fw - tw):
        s = 0
        for y in range(th):
            for x in range(tw):
                a = fpx[ox + x, oy + y]
                b = tpx[x, y]
                s += abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])
        best.append((s, ox, oy))
best.sort()
print('top 5 coarse:')
for s, ox, oy in best[:5]:
    print('  sad=%d  full=(%d,%d)' % (s, ox * K, oy * K))