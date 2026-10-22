from PIL import Image

r = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
p = r.load()
for tag, x0, x1 in (('globe', 25, 140), ('castle', 140, 245), ('face', 245, 350)):
    xs, ys = [], []
    for y in range(280, 315):
        for x in range(x0, x1):
            q = p[x, y]
            if tag == 'globe':
                hit = q[2] > 90 and q[2] > q[0] + 25
            elif tag == 'castle':
                hit = q[0] > 150 and q[1] > 120 and q[2] < 160
            else:
                hit = q[0] > 80 and q[2] < 90 and q[1] < 120
            if hit:
                xs.append(x)
                ys.append(y)
    print(tag, (min(xs), min(ys), max(xs), max(ys)) if xs else None)
