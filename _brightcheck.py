from PIL import Image

a = Image.open('tmp_marker3.png').convert('RGB')
b = Image.open('tmp_marker_nosel.png').convert('RGB')
pa, pb = a.load(), b.load()


def in_cursor(x, y):
    # the walk-cursor is painted at the mouse (224,188) in the "shown" render only
    return 216 <= x <= 276 and 168 <= y <= 212


neg = []
allthree = []
for y in range(145, 222):
    for x in range(222, 282):
        if in_cursor(x, y):
            continue
        for c in range(3):
            if pa[x, y][c] - pb[x, y][c] < 0:
                neg.append((x, y, c, pa[x, y][c] - pb[x, y][c]))
        if (pa[x, y][0] < pb[x, y][0] and pa[x, y][1] < pb[x, y][1] and
                pa[x, y][2] < pb[x, y][2]):
            allthree.append((x, y))
print('negative channel deltas outside the cursor:', len(neg))
print('pixels darker in all 3 channels outside the cursor:', len(allthree))
for n in (neg + allthree)[:12]:
    print('  ', n)
mx = max(pa[x, y][2] - pb[x, y][2] for y in range(145, 222)
         for x in range(222, 282) if not in_cursor(x, y))
print('max blue lift inside the marker:', mx)
