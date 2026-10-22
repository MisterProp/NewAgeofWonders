from PIL import Image

for fn, ox, oy in (('bin/ScenarioMenuRef.png', 0, 0),
                   ('re/mw/fresh9.png', 134, 37),
                   ('re/mw/fresh11.png', 134, 37)):
    im = Image.open(fn).convert('RGB')
    p = im.load()
    print('\n', fn)
    for y in range(282, 312, 3):
        row = []
        for x in range(40, 80, 4):
            q = p[x + ox, y + oy]
            row.append('%3d,%3d,%3d' % q)
        print(y, ' | '.join(row))
