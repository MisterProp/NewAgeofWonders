from PIL import Image

out = open('re/mw/texsample_out.txt', 'w', encoding='utf-8')
for n in ('17', '18', '21'):
    im = Image.open('bin/_sheet2/eGenericT.ILB_%s.png' % n).convert('RGB')
    p = im.load()
    w, h = im.size
    out.write('gt%s %dx%d\n' % (n, w, h))
    for oy in (0, 2, 3, 10, 20, 40):
        s = [0, 0, 0]
        nn = 0
        for y in range(oy, min(oy + 12, h)):
            for x in range(0, min(200, w)):
                q = p[x, y]
                s[0] += q[0]
                s[1] += q[1]
                s[2] += q[2]
                nn += 1
        out.write('  y=%d mean=%s\n' %
                  (oy, tuple(round(v / nn, 1) for v in s)))
out.close()
