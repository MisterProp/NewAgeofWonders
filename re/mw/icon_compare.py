from PIL import Image, ImageDraw
import os

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
regions = [(28, 278, 118, 322), (132, 278, 222, 322),
           (238, 278, 330, 322)]
names = ['size', 'towns', 'players']
scale = 5
tmp = 'bin/_sheet2'
cands = ['eIcons.ILB_8.png', 'eIcons.ILB_9.png', 'eIcons.ILB_10.png',
         'eIcons.ILB_11.png', 'eTitleM.ILB_7.png', 'eTitleM.ILB_8.png',
         'eTitleM.ILB_9.png', 'eTitleM.ILB_10.png']
bg = (41, 49, 24)
tiles = []
for n, box in zip(names, regions):
    crop = ref.crop(box).resize(((box[2] - box[0]) * scale,
                                 (box[3] - box[1]) * scale), Image.NEAREST)
    tiles.append(('ref ' + n, crop))
for c in cands:
    p = os.path.join(tmp, c)
    im = Image.open(p).convert('RGBA')
    w, h = im.width * scale, im.height * scale
    canvas = Image.new('RGBA', (w, h), bg + (255,))
    im = im.resize((w, h), Image.NEAREST)
    canvas.alpha_composite(im)
    tiles.append((c, canvas.convert('RGB')))
pad, lab = 10, 14
W = sum(t.width + pad for _, t in tiles) + pad
H = max(t.height for _, t in tiles) + lab + pad * 2
sheet = Image.new('RGB', (W, H), (70, 70, 78))
d = ImageDraw.Draw(sheet)
x = pad
for name, t in tiles:
    sheet.paste(t, (x, pad + lab))
    d.text((x, pad), name, fill=(255, 230, 120))
    x += t.width + pad
sheet.save('re/mw/icons_compare.png')
print(sheet.size)
