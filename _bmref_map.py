from PIL import Image

im = Image.open('bin/BMRef.png').convert('RGB')
w, h = im.size
px = im.load()
print('BMRef', w, h)
# Mark: '#' = blue-ish tint (b notably above r), '.' = green/warm terrain,
#       ' ' = neutral
for y in range(h):
    row = ''
    for x in range(w):
        r, g, b = px[x, y]
        if b > r + 25 and b > g:
            row += '#'
        elif g > r + 8 and g >= b:
            row += '.'
        elif r > g > b and r > 100:
            row += '+'
        else:
            row += ' '
    print('r%02d' % y, row)