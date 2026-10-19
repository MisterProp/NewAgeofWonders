#!/usr/bin/env python3
"""dfm_dump.py - decode a binary Delphi DFM (TPF0) resource into text.

`dfm_extract.py` pulls the raw form resources out of the binaries; this turns
one into a readable object/property tree so a screen can be re-authored 1:1.
It is how `docs/07_GAME_TYPE_SCREEN.md` got the TMultiWindow layout out of
`assets/dfm/AoW/TMULTIWINDOW.dfm`.

Usage:
    python tools/dfm_dump.py assets/dfm/AoW/TMULTIWINDOW.dfm
    python tools/dfm_dump.py assets/dfm/AoW/TMULTIWINDOW.dfm out.txt

Binary DFM grammar (this build's Delphi flavour - no component prefix byte):
  Signature = 'TPF0'
  Component = ClassName:ShortStr Name:ShortStr
              (PropName:ShortStr Value)* $00
              (Component)* $00
  Value     = Tag:Byte <payload>   (vaNull/vaInt8/vaLString/vaSet/... )
"""
import os
import sys

import common

VA = {0: 'Null', 1: 'List', 2: 'Int8', 3: 'Int16', 4: 'Int32', 5: 'Extended',
      6: 'String', 7: 'Ident', 8: 'False', 9: 'True', 10: 'Binary', 11: 'Set',
      12: 'LString', 13: 'Nil', 14: 'Collection', 15: 'Single', 16: 'Currency',
      17: 'Date', 18: 'WString', 19: 'Int64', 20: 'UTF8String', 21: 'Double'}


def resolve(p):
    """Allow relative names to be given from the game root or cwd."""
    if os.path.isabs(p) and os.path.exists(p):
        return p
    for base in (os.getcwd(), common.GAME_ROOT):
        cand = os.path.join(base, p)
        if os.path.exists(cand):
            return cand
    return p


class R:
    def __init__(self, d):
        self.d = d
        self.p = 0

    def u8(self):
        v = self.d[self.p]
        self.p += 1
        return v

    def i16(self):
        v = self.d[self.p] | (self.d[self.p + 1] << 8)
        self.p += 2
        return v

    def i32(self):
        v = (self.d[self.p] | (self.d[self.p + 1] << 8) |
             (self.d[self.p + 2] << 16) | (self.d[self.p + 3] << 24))
        self.p += 4
        return v

    def sstr(self):
        ln = self.u8()
        s = self.d[self.p:self.p + ln].decode('latin1')
        self.p += ln
        return s

    def value(self):
        tag = self.u8()
        name = VA.get(tag, '?%d' % tag)
        if tag in (8, 9, 0, 13):
            return name
        if tag == 2:
            return '%d' % self.u8()
        if tag == 3:
            return str(self.i16())
        if tag == 4:
            return str(self.i32())
        if tag == 6:
            return '"%s"' % self.sstr()
        if tag in (12, 20):
            ln = self.i32()
            s = self.d[self.p:self.p + ln].decode('latin1')
            self.p += ln
            return '"%s"' % s
        if tag == 7:
            return self.sstr()
        if tag == 5:
            self.p += 10
            return '<extended>'
        if tag == 15:
            self.p += 4
            return '<single>'
        if tag in (16, 17, 21):
            self.p += 8
            return '<%s>' % name
        if tag == 19:
            self.p += 8
            return '<int64>'
        if tag == 10:
            ln = self.i32()
            self.p += ln
            return '<binary %d>' % ln
        if tag == 18:
            ln = self.i32()
            s = self.d[self.p:self.p + ln * 2].decode('utf-16-le', 'replace')
            self.p += ln * 2
            return '"%s"' % s
        if tag == 1:  # list
            items = []
            while self.d[self.p] != 0:
                items.append(self.value())
            self.p += 1
            return '[' + ', '.join(items) + ']'
        if tag == 11:  # set of short strings, terminated by an empty one
            items = []
            while self.d[self.p] != 0:
                items.append(self.sstr())
            self.p += 1
            return '{' + ', '.join(items) + '}'
        if tag == 14:  # collection
            return '<collection>'
        raise ValueError('unknown tag %d at %d' % (tag, self.p - 1))


def component(r, depth, out):
    cls = r.sstr()
    name = r.sstr()
    ind = '  ' * depth
    out.append('%sobject %s %s' % (ind, cls, name))
    while r.d[r.p] != 0:
        pname = r.sstr()
        out.append('%s  %s = %s' % (ind, pname, r.value()))
    r.u8()  # property-list terminator
    while r.d[r.p] != 0:
        component(r, depth + 1, out)
    r.u8()  # child-list terminator


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    path = resolve(argv[1])
    d = open(path, 'rb').read()
    if d[:4] != b'TPF0':
        print('%s: not a Delphi form (no TPF0)' % path)
        return 1
    r = R(d)
    r.p = 4
    out = []
    component(r, 0, out)
    text = '\n'.join(out) + '\nconsumed %d / %d bytes\n' % (r.p, len(d))
    if len(argv) > 2:
        open(argv[2], 'w', encoding='utf-8').write(text)
        print('wrote %s' % argv[2])
    else:
        print(text)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
