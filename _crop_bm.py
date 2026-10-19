from PIL import Image
import sys

# Marker hex is (16,34): hex_origin = (16*32+8, 34*32) = (520,1088).
# camera (296,916), viewport offset (12,12) -> screen = world - 284 / - 904.
# back half box (513,1058)-(559,1105); front (512,1073)-(560,1121).
# Crop with a margin around both.
x0, y0, x1, y1 = 222, 145, 282, 222
im = Image.open('tmp_marker.png').convert('RGBA')
print('full', im.size)
crop = im.crop((x0, y0, x1, y1))
crop.save('tmp_ourcrop.png')
print('crop', crop.size)
crop.resize((crop.width * 4, crop.height * 4), Image.NEAREST).save(
    'tmp_ourcrop_big.png')