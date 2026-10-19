from PIL import Image

im = Image.open('bin/BMRef.png').convert('RGB')
w, h = im.size
px = im.load()
ndark = 0
for y in range(h):
    row = ''
    for x in range(w):
        r, g, b = px[x, y]
        s = r + g + b
        if b > r + 30 and b > 120:
            row += 'B'          # bright blue rim
            ndark += 1
        elif s < 240:
            row += '#'          # dark (marker fill?)
            ndark += 1
        elif b > r + 15 and b > g:
            row += 'b'          # dim blue
        elif g > r + 10 and g > b + 10:
            row += '.'          # green terrain
        elif r > g > b and r > 120:
            row += '+'          # tan terrain
        else:
            row += ' '
    print('r%02d' % y, row)
print('dark/blue pixels', ndark)