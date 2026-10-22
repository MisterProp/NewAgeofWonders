from PIL import Image, ImageDraw
import os

paths = ['id_ic6', 'id_ic7', 'id_ic8', 'id_ic9', 'id_ic10', 'id_tm10',
         'id_gi0', 'id_gi1', 'id_gi2', 'id_gf105', 'id_gf106', 'id_gf107',
         'id_gf108', 'id_gf109', 'id_gf110', 'id_gf111', 'id_gf112']
scale = 4
cells = []
for n in paths:
    p = n + '.png'
    if os.path.exists(p):
        im = Image.open(p).convert('RGBA')
        im = im.resize((im.width * scale, im.height * scale), Image.NEAREST)
        cells.append((n, im))
pad, lab = 8, 14
W = sum(im.width + pad for _, im in cells) + pad
H = max(im.height for _, im in cells) + lab + pad * 2
sheet = Image.new('RGBA', (W, H), (70, 70, 78, 255))
d = ImageDraw.Draw(sheet)
x = pad
for name, im in cells:
    sheet.alpha_composite(im, (x, pad + lab))
    d.text((x, pad), name, fill=(255, 230, 120, 255))
    x += im.width + pad
sheet.convert('RGB').save('icons_sheet.png')
print(W, H)
