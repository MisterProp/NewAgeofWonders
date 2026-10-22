"""Scratch: compare a git-HEAD blob of a PNG with the working file, pixel-wise."""
import subprocess
import sys
from io import BytesIO

from PIL import Image, ImageChops

path = sys.argv[1]
head = subprocess.run(['git', 'show', 'HEAD:' + path], cwd=r'C:\GOG Games\Age of Wonders\port',
                      capture_output=True, check=True).stdout
a = Image.open(BytesIO(head)).convert('RGB')
b = Image.open(path).convert('RGB')
print(path, a.size, b.size, 'bbox', ImageChops.difference(a, b).getbbox())
