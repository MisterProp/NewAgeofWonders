"""Scratch: prove the rendered TurnButton == the library frame it selects.

Renders the in-game screen once per requested frame with --turn-frame, crops the
button (596,439 41x38), and compares it with the ILB frame exported to PNG.
"""
import os
import subprocess
import sys

from PIL import Image, ImageChops

port = r'C:\GOG Games\Age of Wonders\port'
exe = os.path.join(port, 'bin', 'AoW.exe')
lib = os.path.join(port, 'tmp_gr', 'turnbt')
frames = [int(v) for v in sys.argv[1:]] or [0, 5, 10, 46, 52, 81, 95]
bad = 0
for n in frames:
    out = os.path.join(port, 'tmp_gr', 'turnframe_%03d.png' % n)
    subprocess.run([exe, '--root', '..', '--start-map', 'Tutorial.hsm',
                    '--no-music', '--no-cursor', '--render', out,
                    '--turn-frame', str(n)],
                   cwd=port, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, check=True)
    got = Image.open(out).convert('RGB').crop((596, 439, 596 + 41, 439 + 38))
    ref = Image.open(os.path.join(lib, 'f%03d.png' % n)).convert('RGB')
    diff = ImageChops.difference(got, ref)
    bbox = diff.getbbox()
    # mean absolute difference over R,G,B (0..255): sum over the three channels
    total = 0
    for i, band in enumerate(diff.split()):
        hist = band.histogram()
        total += sum(v * c for v, c in enumerate(hist))
    mad = total / (3.0 * 41 * 38)
    print('frame %3d: diff_bbox=%s  mean|d|=%.2f' % (n, bbox, mad))
    if bbox is not None:
        bad += 1
print('mismatching frames:', bad)
