"""dfm_tree.py - summarise a dfm_dump.py object tree into a compact layout list.

dfm_dump.py decodes a binary Delphi form (TPF0) into an indented object/property
tree. This reads that text and prints one line per object with the geometry and
asset-index properties the port needs, so a form's 640x480 layout can be read at
a glance.

    python tools/dfm_dump.py assets/dfm/AoW/TSCENSETUP.dfm | python tools/dfm_tree.py

Properties kept (only when present):
    Left Top Width Height            - the VCL design position (not used for scene)
    WinLeft WinTop WinWidth WinHeight- the position inside the 640x480 scene
    Text Caption                     - labels/buttons
    Visible
    ILFrame ILTexture ILIndexFrame ILIndexTexture ILIndex ILButton ILEdit
    ILIInc ILIDec ILTBack ILLTab ILIndexIncUp ... (scrollbars)
    PopupMenu
"""
import sys

KEYS_ORDER = [
    "Text", "WinLeft", "WinTop", "WinWidth", "WinHeight",
    "Left", "Top", "Width", "Height",
    "Visible", "ILFrame", "ILTexture", "ILIndexFrame", "ILIndexTexture",
    "ILIndex", "ILButton", "ILEdit", "ILVScroll", "PopupMenu",
    "ImageLib", "CurrentImage", "AnimStart", "AnimEnd",
    "Color", "TextColor", "TextSelColor", "AOWFont",
]


def main():
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    stack = []  # (indent, name)
    props = {}
    cur = None
    cur_indent = 0

    def flush():
        if cur is None:
            return
        bits = []
        for k in KEYS_ORDER:
            if k in props:
                bits.append("%s=%s" % (k, props[k]))
        print("%s%s%s" % ("  " * cur_indent, cur,
                          ("  " + " ".join(bits)) if bits else ""))

    for raw in sys.stdin:
        line = raw.rstrip("\n")
        stripped = line.lstrip(" ")
        indent = (len(line) - len(stripped)) // 2
        if stripped.startswith("object "):
            flush()
            cur = stripped[len("object "):]
            cur_indent = indent
            props = {}
        elif "=" in stripped and cur is not None:
            key, val = stripped.split("=", 1)
            key = key.strip()
            if key in KEYS_ORDER:
                props[key] = val.strip()
    flush()


if __name__ == "__main__":
    main()
