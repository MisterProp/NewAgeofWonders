from PIL import Image, ImageDraw

im = Image.open('bin/BMRef.png').convert('RGB')
S = 8
big = im.resize((im.width * S, im.height * S), Image.NEAREST)
d = ImageDraw.Draw(big)
for x in range(0, im.width, 10):
    c = (255, 0, 0) if x % 50 == 0 else (90, 90, 90)
    d.line([(x * S, 0), (x * S, big.height)], fill=c, width=1)
    d.text((x * S + 2, 2), str(x), fill=(255, 255, 0))
for y in range(0, im.height, 10):
    c = (255, 0, 0) if y % 50 == 0 else (90, 90, 90)
    d.line([(0, y * S), (big.width, y * S)], fill=c, width=1)
    d.text((2, y * S + 2), str(y), fill=(255, 255, 0))
big.save('tmp_bmref_grid.png')
print(big.size)