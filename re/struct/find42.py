import sys

path = r'c:\GOG Games\Age of Wonders\port\src\game\menu.cpp'
needle = sys.argv[1]
for i, line in enumerate(open(path, encoding='utf-8', errors='replace'), 1):
    if needle in line:
        print('%d: %s' % (i, line.rstrip()))
