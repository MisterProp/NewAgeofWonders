from PIL import Image

im = Image.open('bin/BMRef.png').convert('RGB')
px = im.load()


def stats(x0, y0, x1, y1, label):
    n = 0
    sr = sg = sb = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            r, g, b = px[x, y]
            sr += r
            sg += g
            sb += b
            n += 1
    print('%-22s mean=(%5.1f,%5.1f,%5.1f) lum=%5.1f n=%d' % (
        label, sr / n, sg / n, sb / n, (sr + sg + sb) / (3 * n), n))


# Marker interior grass (between the two vertical edges, above the floor).
stats(12, 24, 48, 38, 'inside (marker)')
stats(55, 4, 75, 24, 'outside (grass)')
stats(20, 6, 38, 20, 'inside upper (wall)')
stats(80, 6, 95, 20, 'outside right (grass)')
# the floor band of the marker
stats(18, 42, 45, 52, 'marker floor')
stats(55, 42, 62, 52, 'outside near floor')