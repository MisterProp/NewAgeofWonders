from PIL import Image

im = Image.open('tmp_marker3.png').convert('RGB')
c = im.crop((222, 145, 282, 222))
c.resize((c.width * 4, c.height * 4), Image.NEAREST).save('tmp_ourcrop3_big.png')
ref = Image.open('bin/BMRef.png').convert('RGB')
a = ref.resize((ref.width * 3, ref.height * 3), Image.NEAREST)
b = c.resize((c.width * 3, c.height * 3), Image.NEAREST)
canvas = Image.new('RGB', (a.width + b.width + 12, max(a.height, b.height)),
                   (20, 20, 20))
canvas.paste(a, (0, 0))
canvas.paste(b, (a.width + 12, 0))
canvas.save('tmp_cmp3.png')
px = c.load()
dark = sum(1 for y in range(c.height) for x in range(c.width)
           if sum(px[x, y]) < 40)
print('near-black pixels in marker crop:', dark, 'size', c.size)