from PIL import Image

for name in ['BMRef', 'OurBM']:
    im = Image.open('bin/%s.png' % name).convert('RGBA')
    im.resize((im.width * 4, im.height * 4), Image.NEAREST).save('tmp_%s_4x.png' % name)
    print(name, im.size)