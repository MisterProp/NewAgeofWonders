p = 'src/engine/mapviz.cpp'
s = open(p, encoding='utf-8').read()
lines = s.split('\n')
for i, l in enumerate(lines):
    if l.startswith('// Resolves a resource') and l.endswith('MOUNTAIN\\'):
        lines[i] = '// Resolves a resource image-library name (e.g. H_gr.BMP,'
        lines[i + 1] = '// MOUNTAIN/MOUNTAIN.ILB, zgr_Trees5.BMP) to an *.ILB file:'
        break
open(p, 'w', encoding='utf-8', newline='').write('\n'.join(lines))
print('ok')
