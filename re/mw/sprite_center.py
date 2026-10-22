from PIL import Image

ref = Image.open('bin/ScenarioMenuRef.png').convert('RGB')
rp = ref.load()
sprites = [('globe_art', 'bin/_sheet2/eIcons.ILB_9.png', (57, 296)),
           ('castle_art', 'bin/_sheet2/eIcons.ILB_11.png', (160, 298)),
           ('face_art', 'bin/_sheet2/eTitleM.ILB_8.png', (272, 296))]
for name, path, pt in sprites:
    im = Image.open(path).convert('RGB')
    p = im.load()
    w, h = im.size
    print(name, 'size', im.size, 'center px',
          p[w // 2, h // 2], 'corner', p[0, 0])
    print('  ref', rp[pt[0], pt[1]],
          'neighbors', rp[pt[0] + 2, pt[1]], rp[pt[0], pt[1] + 2])
