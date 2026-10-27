# 01 - On-disk formats

Offsets are file offsets, integers/floats little-endian. "RE" = reverse
engineered from this install; "TBD" = needs more work (tracked in ROADMAP).

## ILB - image library (`.ILB`, 538 files)

Container of named images. Verified header:

```
0x00 char[4] magic  = 04 'I' 'L' 'B'
0x04 u32     flags
0x08 f32     format version     (retail assets: 4.0; some effects: 3.0)
0x0C u32     header size        (24 for v4, 16 for v3)
0x10 u32     data offset        (start of pixel data; 0 on some v3 files)
0x14 u32     total file size
```

`[header_size, data_offset)` is the **directory** (for the v3 files whose
`data_offset` is 0 it runs to end-of-file):

```
u32 palette_count
palette_count * (u32 + 1024)            palettes (GFXE.TGFXEPalette)
repeat until index == 0xFFFFFFFF:
    u32 index                           image slot id (may skip numbers)
    u32 class_id                         TLibraryImage subclass tag
    <descriptor>
    u32 reference                       0xFFFFFFFF = none
```

The descriptor begins with a `u8 fmt` revision and a `u32-length` name, then
the `TLibraryImage.ReadImageData` fields:

```
u8  fmt
u32 name_len ; char[name_len] name      e.g. "UnitFr.BMP"
u32 width       (+0x18)
u32 height      (+0x1c)
u32             (+0x28)
u32             (+0x2c)
u32             (+0x34)
u8              (+0x16)
u32 data_size   (+0x10)                 serialized pixel/RLE byte count
if fmt > 1: u32 (+0x0C), u32 (+0x20), u32 (+0x24)
if fmt > 2: u64 (+0x38)
<class-specific tail>                   (variable length)
```

class ids seen in the retail data: `0x02`, `0x10`, `0x11`, `0x12`, `0x16`.
Names repeat for sub-images (a library lists the same name once per frame,
e.g. `AntiM2.BMP` x4). Dimensions are real values; some classes store raw
rows of `width` bytes, others pack an RLE blob (e.g. `GRSparks*.BMP` are
20x20 with only 18..180 byte payloads), which is why `data_size` is carried
per entry.

`0x100` is `TImageNode` (`Ilpack.dpl` `0x5520d298`). A node is a wrapper rather
than an image: its record is `u32 index, u32 class_id = 0x100`, then the wrapped
`<image>` records (each `u32 class_id` + a descriptor; the loop ends at the
`u32 reference`), and the wrapped records carry **no index of their own** - the
node's `index` is the entry's. Several libraries (e.g. the bitmap fonts,
`Int\Fonts\*.ILB`) store *every* entry this way, with the wrapped images being
the layers of one entry, so `index` there is a character code rather than a
sprite id. `src/engine/ilb.cpp` detects such a library from its first record and
stamps the node's index onto the wrapped entries; ordinary libraries are
unaffected.

Several **mixed** libraries (the UI art, e.g. `Int\Icons.ILB`,
`Int\Scenes\TitleM.ILB`) place a node header in front of every multi-layer
sprite instead of wrapping the whole library:

```
... u32 reference = 0xFFFFFFFF, u32 node_index, u32 class_id = 0x100,
    u32 layer0_class_id = 0x16, <descriptor>,
    u32 0, u32 layer1_class_id = 0x16, <descriptor>, ...
```

The signature walk lands on the node header (`sig.index` reads 0x100), and the
following layer starts with a zero index. The loader now takes the node's index
from the u32 **immediately before** the 0x100 (`p - 4`) and stamps it on the
node header and on the following zero-indexed layer. That is what makes
`UnitIcons` ids 7/8 and `TitleScreen` id 10 address the two-layer
globe/castle/leader-face images (verified against the shipped picker). This is
`ImageLib.TImageNode.Show` (`Ilpack.dpl` 0x5520d654), which blits every layer in
order; `TSprite16.ShowTransparent` (0x55211ea0) uses the engine's alpha-50
lookup table.

Because the class-specific tail length is variable, records are located by
their signature (`index, class_id, fmt, name_len, printable name`).

**Sentinel slot ids.** A multi-layer sprite's *secondary* layer can be stored
with the id `0xFFFFFFFF` instead of a real slot: `Images\Str_node.ILB`'s last
record (`NEarth06.BMP`, the overlay layer of the node before it) carries it, as
do the extra layers in several other `Images\*.ILB`. `0xFFFFFFFF` is the
directory's own end marker (see the loop above) and never a real slot, so any
consumer that sizes an array by `IlbEntry::index` must skip it - `mapviz`'s
library index used to take the maximum and asked for 2^32 sprites, a
`std::bad_alloc` that aborted the process as soon as a map with state nodes
(e.g. `Scenario\Custom\Blood Isles.hsm`) was started.

Implemented by `src/engine/ilb.cpp` (`aow ilb <file>`), validated against all
538 `.ILB` files of this install; decompiled reference in
`re/exports/ilpack/`. The older `tools/ilb_tool.py` is heuristic (name scan
only) and superseded.

**Reloading.** `IlbFile::load()` / `load_memory()` reset the parsed directory
(`entries_`, `data_`) before reading a new file. An `IlbEntry::blob_offset` is
relative to the file it was read from, while `decode_rgb()` indexes the current
`data_`, so keeping a previous library's entries would decode the new file at
the old offsets. The unit detail page reuses one `IlbFile` for the selected
unit's face library and hit exactly this when switching from a
`FACES\UNITS.ILB` unit (machines/monsters) to a `FACES\R_FACES.ILB` race face:
the portrait came out sheared and showed slivers of the neighbouring pictures.

### Image classes and decoding

Each image record carries a `class_id` naming its `TLibraryImage` subclass; the
`ClassID` getters (decompiled) give the exact mapping:

| class | Delphi class          | storage                              |
|-------|-----------------------|--------------------------------------|
| 0x02  | TRLESprite08          | 8-bit paletted, RLE                  |
| 0x10  | TPicture16            | raw 16-bit (RGB565), `w*h*2` bytes   |
| 0x11  | TRLESprite16          | 16-bit RLE                           |
| 0x12  | TTransparentRLESprite16 | 16-bit RLE (+transparency)         |
| 0x16  | TSprite16             | 16-bit sprite                        |

Raw decoding is implemented for **class 0x10** (the pixel region is simply
`w*h` little-endian RGB565 words), which covers ~1,870 images including the
640x480 demo/quit screens in `Images/Demo.ILB`. Emit a PNG with:

```powershell
aow png ..\..\Images\Demo.ILB AoWDemoScreen1.bmp out.png
```

PNG output is written by `src/engine/png.cpp` - dependency-free (stored DEFLATE
+ CRC-32/Adler-32), matching the project's stdlib-only policy.

### Sprite RLE (classes 0x02 / 0x11)

Both sprite RLE formats are now decoded (`FUN_55216ae8` = 8-bit,
`FUN_55210b54` = 16-bit; decompile them with the **`__register`** calling
convention - the Borland `register` pass, EAX/EDX/ECX - otherwise the
decompiler puts every argument on the stack and the code is unreadable).

The RLE bitmap is a sub-rectangle of the `+0x18`/`+0x1c` slot; its size lives in
class fields (`+0x60`/`+0x64` for 0x02, `+0x58`/`+0x5c` for 0x11).

The descriptor also carries the sprite's **placement box**: the base margins at
`+0x28`/`+0x2c` and the content shift at `+0x50`/`+0x54` (class 0x11) or
`+0x5c`/`+0x60` (class 0x16). The engine blits content at `param + base + shift`
(`re/exports/ilpack/TSprite16.ShowOpaque.c`); a base terrain hexagon is `48x32`
at `(0,0)`. All four are parsed into `IlbEntry` (`base_x`/`base_y`,
`xshift`/`yshift`) and applied as the object anchor (`anchor + base + shift`; see
"Object draw anchor" in the render section below).

* **Class 0x02 (TRLESprite08)** - 8-bit palette indices, fixed 4-byte row
  header. For each of `rle_h` rows: skip 4 bytes, then fill `rle_w` pixels; a
  token equal to the escape byte (`+0x4C`) is followed by a run length in
  *pixels*, any other byte is one opaque palette index. Verified bit-exact on
  20,577 / 20,660 records. Palette is either inline in the record or an index
  into the palette manager (see **Palette entries and byte order** below).
* **Class 0x11 (TRLESprite16)** - 16-bit RGB565. Each row is a `u32` length
  (including its 4-byte header); the next row starts at `align4(p + len)`.
  Within a row a `u16` token equal to the escape word (`+0x48`) is followed by a
  run length in *bytes*, any other `u16` is one opaque pixel. Verified on
  5,690 / 5,690 records.

* **Class 0x12 (TTransparentRLESprite16)** - derives from `TRLESprite16` and
  inherits its reader, so it is byte-identical to 0x11 (verified 95/95).
* **Class 0x16 (TSprite16)** - raw 16-bit RGB565. The bitmap size is
  `+0x64` x `+0x68` (NOT the base slot), i.e. `data_size == w*h*2`
  (verified 3,977/3,977; e.g. `GRSparks00` is really 5x5 even though the slot is
  20x20). `TSprite16.ShowOpaque@0x5521193c` (the vtable+0x9c entry, used for the
  cached static scene) calls `GFXE.BltColorKey16@0x55202050`, and the key is the
  u16 in the descriptor right after the shift fields - the same slot class
  0x11/0x12 use for their RLE escape word, i.e. `c + 16` in the parser's terms
  (`TSprite16.ReadImageData@0x5521172c` reads width, height, xshift, yshift and
  then that field). `decode_rgb` turns it into alpha. Verified against the sprite
  backgrounds: `Cursors.ilb` id 0 -> `0x418C`, `General.ILB` `3dHex` -> `0x05A9`,
  `Str_mine.ILB` frame 10 -> `0x0000`, `Str_wtwr.ILB` `Wtwr00` -> `0x0000`. A few
  shipped frames (MISC_OBJ's gold glitter, EPL_DNG2's dungeon mouth) carry a
  *stale* key that does not occur in the bitmap - the blended glows - so when the
  stored key removes nothing the sprite's dominant border-ring colour is keyed
  instead (the rule the menu screens already used); a full-bleed picture has no
  dominant border and is left untouched.
  **The sequence path blends instead of keying**: the vtable+0x84 slot that
  `ShowComplexImage@5560a134` uses is `TSprite16.ShowTransparent@0x55211ea0` ->
  `ShowBlended@0x55211b64` with the GFXE per-channel **brighten** table
  (`dst = max(dst, src)`), so a glow lights the map up instead of painting its
  dark surround over it - see docs/17_BUILDING_ANIM.md section 5b.
  `tools/buildings_sheet.py <game-root> <out.png>` lays every building sprite out
  on a checkerboard, and `tools/glow_bench.py` measures the glow against the
  original game's screenshot (`docs/refs/glow_ref.png`).

### Palette entries and byte order

Each palette in the manager (and each inline palette) is a `u32 format_tag`
followed by 1024 bytes = 256 entries of 4 bytes. `GFXE.TGFXEPalette.GetEntry`
(Gfxepack.dpl `0x5500ba2c`) is `palette + 8 + i*4`, so entry `i` is the 4 bytes
at `+8 + i*4`. The **tag decides the entry byte order**, which
`GFXE.TGFXEPalette.Remap` (`0x5500ba34`) reads to build an RGB565 palette:

| tag | entry bytes | 555 target `0x55509310` | 565 target `0x56509310` |
|-----|-------------|-------------------------|-------------------------|
| `0x88801B18` | `(R,G,B)` | `(b0&0xF8)<<7 \| (b1&0xF8)<<2 \| (b2>>3)` | `(b0&0xF8)<<8 \| (b1&0xFC)<<3 \| (b2>>3)` |
| `0x88809318` | `(B,G,R)` | `(b2&0xF8)<<7 \| (b1&0xF8)<<2 \| (b0>>3)` | `(b2&0xF8)<<8 \| (b1&0xFC)<<3 \| (b0>>3)` |

Every palette in this install carries `0x88801B18` (verified across all 538
`.ILB`s), so an 8-bit entry decodes as `(R,G,B)`. `src/engine/ilb.cpp` reads the
tag (per palette and for inline palettes) and only swaps to `(B,G,R)` for
`0x88809318`. Decoding them the other way round exchanges R and B and tints every
8-bit unit sprite blue.

`decode_rgb` now handles classes **0x02, 0x10, 0x11, 0x12 and 0x16** - the whole
retail surface (bar the older v3 files). Emit any image with:

```powershell
aow png ..\..\Images\ADD_STRUCTURES1.ilb 0 out.png
```



## PFS - rule data tables (`.PFS`, 12 files in `Release/`)

Fully decoded. These files are a generic engine **object graph** dump (not a
bespoke record format): `Engine.TEngine.WriteToFileCRC` serialises a `TAoWHSSet`
sub-table through `Engine.TEStorageStream` / `TPropertyTable` (Enginep.dpl),
and `ReadFromFileCRC` reads it back. All integers are little-endian. Reference
decompilations: `re/exports/enginep/`, `re/exports/aowepack/`.

### Container

```
0x00 u32 crc2                       CRC-32 of bytes [4, size)
0x04 ...                            object region ("EObject" stream)
     u32 crc1                       CRC-32 of bytes [4, size-4)
```

`crc1`/`crc2` are the standard IEEE CRC-32 (`Engine.GetCRC32`, identical to
`zlib.crc32`). `HEROES.PFS` and `ITEMS.PFS` are an **older variant with no
CRCs** (the whole file is the region); `General.pfs` is an empty 5-byte stub.

### Object region - property tables

The region is a tree of "EObject" nodes. A node is a **property table**
followed by its data area:

```
u8  n        if bit 0x80 is set: n = (n & 0x7f) compact entries and a u32
             wide-entry count follows; otherwise n compact entries
n * u16      compact entry: tag = low byte, offset = high byte
large * 8    wide entry: u32 tag, u32 offset
<data>       base = position just past the table; an entry's value lives at
             base + offset
```

This is exactly `TPropertyTable.LoadFromStream` / `SaveToStream` / `FindOffset`
(compact entries are emitted before wide entries). A value is either raw bytes
or another node (`[table][data]`, recursively).

The **record list** is the table with the most entries whose offsets are
strictly increasing and start at 0, reached by walking the tree from the
region root. Each of its entries is one record.

### Records

A record is `[table][data]`, optionally preceded by a `u32` class id (when the
list instantiates elements by class via `TEReadStorageStream.rwCreateEObject`),
so the field table starts at offset `0` or `4`. Fields are `(tag -> byte
range)`; a string field is stored as a `u32` length followed by that many
bytes (no NUL terminator). The raw reader exposes the `(tag, offset, size)`
triples and every `u32`-length-prefixed string; the per-class tag *semantics*
are mapped in [Typed field tags](#typed-field-tags-stage-15) below.

### Verified record counts (this install)

| file | records | contents |
|------|---------|----------|
| `Ability.pfs`   | 147 | ability descriptions, e.g. "Allows the unit to walk upon normal terrain." |
| `Spells.pfs`    | 108 | spell descriptions + `*.ILB` effect refs |
| `ITEMS.PFS`     |  83 | items |
| `HEROES.PFS`    |  50 | hero templates (names, e.g. "Esmeralda") |
| `HERORES.PFS`   |  36 | hero race resources |
| `Unitres.pfs`   | 142 | unit stats / descriptions |
| `Unitgfx.pfs`   | 182 | unit sprite refs (`UNITS\HMN\*.ILB`) |
| `comb_res.pfs`  | 581 | combat terrain refs (`TCOMBAT\*.ILB`) |
| `FACERES.PFS`   |  89 | face refs (`FACES\*.ILB`) |
| `ITEMGFX.PFS`   | 123 | item sprite refs |
| `TEXT.pfs`      |  91 | campaign / message prose |
| `General.pfs`   |   0 | empty stub |

> These counts are for the **original Age of Wonders 1** (this GOG install).
> This document was originally written while the project still targeted the
> **Age of Wonders X** mod, whose tables are larger (older, now-removed numbers
> were e.g. Unitres 188, Unitgfx 239, FACERES 260, ITEMGFX 372, ITEMS 176,
> HEROES 193, HERORES 50). The rule-fidelity test
> (`src/tests/pfs_types_test.cpp`) baselines the original numbers.

Implemented by `src/engine/pfs.cpp` (`aow pfs <file> [index]`), validated
against all 12 files: CRCs verify, records tile the region with no desync, and
the decoded names/descriptions match the game. The older `tools/pfs_probe.py`
only sampled header words and strings and is superseded.


### Typed field tags (Stage 1.5)

Each record class was recovered from its `ReadWrite` method (Enginep.dpl /
AoWEPACK.dpl), and every tag was cross-checked against the shipped bytes.
`TEReadStorageStream` (Enginep) has its vtable at `0x5550de38` (slot 0 =
`StorageMode`); the field encodings are:

| method (slot) | on-disk encoding |
|---------------|------------------|
| `rwInteger` +0x2c / `rwIntegerEx` +0x40 | 4 bytes |
| `rwWord` +0x34 / `rwWordEx` +0x48 | 2 bytes |
| `rwByte` +0x30 / `rwByteEx` +0x38 / `rwShortint` +0x3c / `rwShortintEx` +0x44 | 1 byte |
| `rwData` +0x1c | N raw bytes (N is a literal at the call site) |
| `rwString` +0x20 | u8 length, `0xFF` => a u32 length follows, then bytes |
| `rwStrings` +0x24 | u32 length, then bytes (`Engine.StringsFromStream` @ `0x5550f908`) |
| `rwBool` +0x38 | 0 bytes - the value is the tag's *presence* |
| `rwEObject` +0x0c | an embedded EObject |

#### Ability.pfs - `AoWE.TAbility` (`ReadWrite` @ `0x5574f07c`)

| tag | field | method | evidence |
|-----|-------|--------|----------|
| 5 | description | rwStrings | prose, e.g. "Allows the unit to walk upon normal terrain." |
| 6 | cost | rwInteger | 100 for "Flying" [TODO(RE)] |
| 7 | has_sound | rwEObject | a `TENode` / `TSFXLibrary` sound node |
| 8 | has_sequence | rwStreamProc | a `TImageSequenceList` |
| 9 | move_types | rwData (2 B) | 0x3FF for "Flying", 0x61 for walking |

#### Spells.pfs - `AoWE.TSpell` (`ReadWrite` @ `0x55779234`)

| tag | field | method | evidence |
|-----|-------|--------|----------|
| 10 | description | rwStrings | spell prose |
| 11 | effect_ref | rwEObject | embedded node |
| 12 | animation | rwStreamProc | `EFFECTS\*.ILB` image sequence |
| 13 | mana_cost | rwInteger | +0x14; `THero.CastingMana` @ `0x557894ec` (doubled when the sphere's mana is doubled) |
| 14 | upkeep | rwInteger | +0x1c; copied into a global enchantment (`TGlobalEnchantmentSpell.SetupEnchantment`) |
| 15 | research_cost | rwInteger | +0x18; `TPlayerMagicControl.ExecuteResearchSpell` @ `0x5577d538`, `TSpell.SetupCastSpellAction` @ `0x557796d4` |
| 16 | sphere | rwData (1 B) | +0x20 magic realm; `TPlayerMagicControl.ValidResearchSpell` @ `0x5577cff0` |
| 17 | level | rwByte | +0x21 spell level (1..4); `ValidResearchSpell` / `TSpellControl.GetRandomSpell` |

#### Unitres.pfs - `AoWE.TUnitResource` (class `0x20212`)

Chain: `Engine.TEResource.ReadWrite` @ `0x5551b064` (tags 1..3) ->
`AoWE.TAbstractUnitResource.ReadWrite` @ `0x55784a48` (tag 8) ->
`AoWE.TUnitResource.ReadWrite` @ `0x55784de4` (tags 10..31).

| tag | field | method | evidence |
|-----|-------|--------|----------|
| 1 | res_id | rwInteger | TEResource id/hash |
| 2 | res_type | rwInteger | TEResource - constant 3 for units |
| 3 | res_index | rwInteger | TEResource index |
| 8 | gfx_index | rwInteger | unitGFXResourceIndex |
| 10 | name | rwString | byte off 0x1c, e.g. "Human" |
| 11 | qualifier | rwString | byte off 0x24, e.g. "Archer"; `GetUnitName` @ `0x55784da8` joins both |
| 12 | type | rwData (1 B) | |
| 13 | race_id | rwByte | race/faction; full data order in docs/11_UNIT_CATALOG.md (0=Human, 1=Azrac, ... 9=Orc, 10=Goblin, 11=Undead, 255=monster) |
| 14 | attack | rwByte | `TUnit.GetAttack` @ `0x557829ec` reads +0x29 |
| 15 | defense | rwByte | `TUnit.GetDefense` @ `0x55782a40` reads +0x2a |
| 16 | damage | rwByte | `TUnit.GetDamage` @ `0x55782aa8` reads +0x2b |
| 17 | hit_points | rwByte | shown by `TUnitInfoDlg.ShowUnitResource` @ `0x557ff5a4` (+0x2c) |
| 18 | moves | rwByte | `TUnit.GetMoves` @ `0x55782b84` reads +0x2d |
| 19 | resistance | rwByte | `TUnit.GetResistance` @ `0x55782ae8` reads +0x2e |
| 20 | level | rwByte | `TUnit.GetUnitLevel` @ `0x55782b8c` reads +0x2f; upkeep = level + 1 |
| 21,22,23 | extra21..23 | rwData (1 B) | |
| 24 | transport_capacity | rwByte | `GetTransportCapacity` @ `0x55784d24` |
| 25 | ability_ref | rwEObject | an ability owner |
| 26 | description | rwStrings | unit prose |
| 27 | obtain_value | rwInteger | `GetObtainValue` @ `0x55784d58` |
| 28,29 | extra28,extra29 | rwData (1 B) | |
| 30,31 | obj30,obj31 | rwEObject | ability owners |

#### ITEMS.PFS - `AoWE.TItem` (class `0x20268`)

Chain: `AoWE.TAbilityOwner.ReadWrite` (tags 2,3 from `TCustomAbilityList`) ->
`AoWE.TItem.ReadWrite` @ `0x557945c8` (tags 7..22).

| tag | field | method | evidence |
|-----|-------|--------|----------|
| 2 | ability_count | rwInteger | number of ability bits |
| 3 | ability_mask | rwData | ceil(count/8) bytes |
| 7 | gfx_index | rwInteger | `SetItemGFXIndex` +0x40 |
| 8 | name | rwString | byte off 0x18, e.g. "Ancient Helm" |
| 9 | description | rwString | byte off 0x30 |
| 10..14 | flag10..flag14 | rwData / rwByte | |
| 15 | item_type | rwData (1 B) | `SetItemType` +0x34 |
| 16 | bonus | rwBool | presence only |
| 17 | value17 | rwInteger | [TODO(RE)] (consumer is in an un-imported UI module) |
| 18 | text18 | rwStrings | |
| 19 | obtain_value | rwInteger | `GetObtainValue` +0x2c |
| 20 | spell | rwInteger | granted spell id; `TItem.CanUse` @ `0x557940ac` reads +0x38 when item_type == 5 |
| 21 | library_item | rwIntegerEx | `GetLibraryItem` +0x24 |
| 22 | library_name | rwString | byte off 0x28 (`GetLibraryName`) |

#### HEROES.PFS - `AoWE.THero` (class `0x20230`)

Chain: `AoWE.TAbilityOwner` (tags 2,3), `AoWE.TAbstractUnit.ReadWrite` @
`0x557820e4` (tags 4,5,6,0x2f), `AoWE.THero.ReadWrite` @ `0x55788880`
(tags 7..0x27).

| tag | field | method | evidence |
|-----|-------|--------|----------|
| 2 | ability_count | rwInteger | |
| 3 | ability_mask | rwData | ceil(count/8) bytes |
| 4,5 | value4,value5 | rwInteger / Ex | `TAbstractUnit` [TODO(RE)] |
| 6 | morale | rwByte | `TAbstractUnit.GetMorale` +0x26 |
| 7 | face_index | rwInteger | `GetFace`/`SetFaceIndex` +0x3c |
| 8 | hero_resource_index | rwInteger | `SetHeroResourceIndex` +0x44 |
| 9 | value9 | rwInteger | [TODO(RE)] transient |
| 10 | name | rwString | byte off 0x60, e.g. "Conan" |
| 11 | nickname | rwString | byte off 0x64, e.g. "McHarr" |
| 12 | value12 | rwByte | [TODO(RE)] |
| 13 | casting_points | rwInteger | `GetCastingPoints` +0x80 |
| 14 | value14 | rwInteger | [TODO(RE)] |
| 15 | gender | rwData (1 B) | `GetGender` +0x69 |
| 16 | attack_skill | rwByte | `SetUnitAttack` +0x6a |
| 17 | defense_skill | rwByte | `SetUnitDefense` +0x6b |
| 18 | damage_skill | rwByte | `SetUnitDamage` +0x6c |
| 19 | hits_skill | rwByte | `SetUnitHits` +0x6d |
| 20 | moves_skill | rwByte | `SetUnitMoves` +0x6e |
| 21 | resistance_skill | rwByte | `SetUnitResistance` +0x6f |
| 22 | unit_level | rwByte | `GetUnitLevel` +0x4c |
| 23 | value23 | rwBool | presence |
| 24 | experience | rwInteger | `GetExperience`/`GetLevel` +0x48 |
| 25 | value25 | rwData (1 B) | [TODO(RE)] (always 0) |
| 26 | value26 | rwInteger | [TODO(RE)] |
| 27 | value27 | rwByte | [TODO(RE)] |
| 29 | move_points | rwByte | `GetMovePoints` +0x79 |
| 30 | hit_points | rwByte | `GetHitPoints` +0x7a |
| 31 | value31 | rwInteger | [TODO(RE)] |
| 33 | value33 | rwBool | presence |
| 34 | value34 | rwInteger | [TODO(RE)] |
| 35 | library_hero | rwIntegerEx | `GetLibraryHero` +0x90 |
| 36,37 | ability_set_b,ability_set | rwEObject | `GetAbilitySet` +0x74/+0x70 |
| 38 | library_name | rwString | byte off 0x94 (`GetLibraryName`) |
| 39 | used_skill_points | rwInteger | `GetSkillPoints` +0x50 |

Loaders: `aow::load_abilities/_spells/_units/_items/_heroes` in
`src/engine/pfs_types.cpp` (declared in `src/include/aow/pfs_types.hpp`).
CLI: `aow abilities|spells|units|items|heroes <file.PFS>`, or
`aow pfs <file.PFS> --typed` (auto-detects the table). The `TODO(RE)` fields
are stored but their semantics are unconfirmed. Rule-fidelity tests live in
`src/tests/pfs_types_test.cpp` (run via `build.ps1 -Test` or `ctest`): they
load the shipped `Release/*.PFS` and assert the record counts plus known
names/stats.

### Additional typed tables (Stage 1.5b)

The remaining six `.PFS` tables are typed the same way:

| file | class | class id | ReadWrite |
|------|-------|----------|-----------|
| `HERORES.PFS` | `AoWE.THeroResource` | `0x20231` | `0x55789fd4` |
| `Unitgfx.pfs`  | `AoWE.TUnitGFXResource` | `0x20210` | `0x55783bf4` |
| `FACERES.PFS`  | `AoWE.TFaceResource` | `0x20270` | `0x5574d540` |
| `ITEMGFX.PFS`  | `AoWE.TItemGFX` | `0x20269` | `0x55795b64` |
| `TEXT.pfs`     | `AoWE.TTextControlItem` | `0x2029a` | `0x55764910` |
| `comb_res.pfs` | `AoWTer.TAoWTerrainResource` | - | `0x5579be68` |

Every *resource* table starts with the three `Engine.TEResource.ReadWrite`
tags (`0x5551b064`): tag 1 = a 32-bit id/hash, tag 2 = a constant type code
(units 3, hero-resources 24, unit-gfx 4, faces 25, item-gfx 26, terrain
34..42), tag 3 = a small index.

| file | typed fields |
|------|--------------|
| HERORES | tag8 gfx_index, tag10 name, tag13 hero_class, tag15..20 skills (attack,defense,damage,hits,moves,resistance), tag29 EObject, tag30 int |
| Unitgfx | tag5 image sequence, tag6 image_lib, tag7 name, tag8 face index, tag10 byte, tags 15..19 EObjects |
| FACERES | tag5 image_lib, tag6 face_index, tag7 bool |
| ITEMGFX | tag6 image_lib, tag7 gfx_index, tag8 byte, tag9 bool |
| TEXT    | tag9 id, tag10 text (`rwStrings`), tag11 name |
| comb_res | tags 1..3 plus tag 0x14 (`Terrain.TTerrainMOR.ReadWrite` @ `0x557066bc` is an unrecovered jump table, so the body is left raw) |

Loaders: `aow::load_hero_resources` / `_unit_gfx` / `_faces` / `_item_gfx` /
`_texts` / `_terrain`. CLI:
`aow herores|unitgfx|faces|itemgfx|texts|terrain <file.PFS>`.

## MLD - localized strings (`.MLD`, `Dict/`)

```
0x00 char[3] magic = 'M' 'L' 'D'
0x03 u8      version (3)
...          string table (ANSI + UTF-16 mix, multi-language)
```
`tools/strings_tool.py` extracts the text. Files: `AoW.mld`, `AoWEd.mld`,
`Campaign.mld`, `CustMaps.mld`, `Release.mld`, `ResStr.mld`, `CampMaps.mld`,
`AoWSetup.mld`.

## HSM / CSM / HSS - scenario / campaign maps (Stage 2.1)

The map files reuse the same generic **engine object graph** as `.PFS`
(`TEngine.WriteEObject`/`ReadEObject` -> `TEStorageStream.rw*`), so the reader
in `src/engine/map.cpp` walks the same `[table][data]` property tables.

### Container

`.HSM` / `.CSM` (all 144 in `Scenario/Campaign` + `Scenario/Custom`):

```
0x00 char[4] magic  = 'C' 'F' 'S' 00
0x04 u8      version (2)
0x05 ...     zlib stream (RFC 1950) -> the region below
```

`.HSS` (`Release.hss`, the *game pack*: a `TAoWWHSSet`, not a map) is
**uncompressed**. `TCompressStream` (`Engine.TCompressStream`) does the inflate;
the port implements it in `src/engine/inflate.cpp` (stdlib-only DEFLATE).

### Region header + top object

After inflate, the region begins with a **16-byte** header written by
`THSEngine.SaveHSMtoStream` (HSEPack `0x55610524`) and read by `LoadHSMfromStream`
(`0x55610288`):

```
0x00 u32  region offset  = 0x10 (the object graph starts here)
0x04 char[4] magic       = 'H' 'S' 'M' 00        (HSM/CSM)
0x08 u32  0
0x0C u32  top object class id                    (0x20011 = AoWE.TAoWHSMap)
```

`Release.hss` uses the `SaveHSS` (`0x5560f970`) header instead: a 52-byte block
`u32 0x34 | 0x53534804 ('HSS') | u8 0 | u8 name_len | name`; its object class is
not stored (the caller knows it is a `TAoWWHSSet`).

The region is the usual `[table][data]` graph. Its first table has a single
entry `(tag 0 -> offset 0)`; `outer.base + 0` is the top object's own table.

### Class tag tables (decompiled `ReadWrite`)

| class | class id | ReadWrite | tags |
|-------|----------|-----------|------|
| `AoWE.TAoWHSMap` | `0x20011` | AoWEPACK `0x55776d60` | 7 rwString, 0xb/0xc rwStrings, 0x10 rwByte, 0x15 rwEObject (TPlayerList), 0x16 rwEObject (TRaceList), 0x17/0x1a/0x27/0x2d/0x35/0x3b/0x3d/0x3e rwBool, 0x1b rwByte, 0x1c/0x24/0x29/0x2e/0x2f/0x33/0x34/0x37/0x3a rwInteger, 0x2a..0x2c/0x36/0x38 rwEObject, **0x1d rwEObject (`AoWE.TStartupSongList` = the song list)**, **0x3c rwEObject (`Sound.TSonglist`)**, 0x1f rwData(1), 0x22/0x23/0x25 rwData(1), 0x2d.. 1 rwByte; **tag 1 = children** |
| `HSEngine.THSMap` | (base) | HSEPack `0x5560c4c4` | 2 rwString (HSS file name), 3 rwByte, 4/5/6 rwInteger |
| `HSEngine.TMapContainer` | `0x10103` | HSEPack `0x55608c44` | 3 rwByte = width, 4 rwByte = height, 5 rwByte = level count; tag 1 = children |
| `HSEngine.TMapLevel` / `AoWE.TAoWMapLevel` | `0x10102` / `0x20013` | HSEPack `0x556084d0` | 3 rwByte = width, 4 rwByte = height, 5/6 rwByte; **tag 6 = the field blob** (see below); tag 1 = children |
| `HSEngine.TMapField` / `AoWE.TAoWMapField` | `0x10101` / `0x20012` | HSEPack `0x55607128` / AoWEPACK `0x55771c28` | **tags >= 50 = hexagon sprites** (rwCreateEObject: u32 class id + nested table); tag 4 rwWord (AoWE only) |

The scenario **music playlist** is two of those objects: **tag 0x1d**
(`AoWE.TStartupSongList`, the scenario's "original" songs) and **tag 0x3c**
(`Sound.TSonglist`). `Sound.TSonglist.ReadWrite` (`SoundP.dpl` 0x5540572c)
serialises its song list at object **tag 10** as a `u32` byte length followed by
the `TStringList.Text` - the file names joined by CRLF with a trailing CRLF
(`"SONGS\CLOUDS.IT\r\nSONGS\REMARCH.IT\r\n"`, verified on all shipped maps).
AoW.exe plays tag 0x3c when it has entries, otherwise tag 0x1d; the two lists
land in `Map::songs_startup` / `Map::songs_list` and are written up in
`09_GAME_SCREEN.md` section 8.

`Engine.TECustomNode.ReadWriteChildren` (`0x55519608`) is
`rwChildren(stream, this, 1)`, so **tag 1 of every node is its child table**
(each entry = u32 class id + `[table][data]`). The container is the single child
of the map; the levels are the children of the container.

### The terrain tile layer

`TMapLevel` tag 6 holds the field list as a sequence of `u32 length` +
one field EObject per hex (`rwEObjectStream`, `TEReadStorageStream` slot +0x10,
`0x555117a8`), **row-major, exactly `width*height` records**. Each field
(`TMapField`) lists its hexagon sprites under table tags >= 50:

* **tag 50** = the terrain hexagon,
* **tags 51, 52, ...** = the map objects on that hex (units, cities, items, ...).

An army on the field is `AoWE.TArmyHS` (engine class `0x20217`, tag 10 = the
embedded `AoWE.TArmy` whose children are the `TUnit`/`THero`/`TLeader` objects).
See **docs/13_UNITS_ON_MAP.md** for the full chain and its rendering.

Every sprite starts with a `u32` Engine class id followed by its own
`[table][data]`; its **tag 1** is a `rwInteger` (a resource/variant id, `[TODO(RE)]`).

Terrain hexagon classes seen in the retail maps (cross-validated):

| class id | Delphi class | meaning |
|----------|--------------|---------|
| `0x20152` | `Hexagon.THexagon` | plain land hexagon |
| `0x20170` | `IsoHex.TUpperIsometricHexagon` | elevated (isometric upper) terrain |
| `0x20557` | `BorderHx.TBorderHexagon` | map border |
| `0x2073d` | `AoWHex.TAoWWaterHexagon` | water |

Object classes resolve to the typed rule resources, e.g. `0x20166`
`Terrain.TTerrainMO`, `0x20168` `Mountain.TMountainMO`, `0x2036a` `Crops.TCrop`,
`0x20164` `Road.TRoad`, `0x20108` `City.TCity`, `0x20217` `AoWE.TArmyHS`,
`0x2037a` `Mine.TMine`, `0x2037e` `Cave.TCave`, `0x2043e` `Tower.TTower`, ... The
full table is `kClassNames[]` in `src/engine/map.cpp`.

### The field's terrain / overlay axes

A `TMapField` carries the two bytes the world-map movement rules index their
cost table by (`docs/15_MOVEMENT.md`):

* `+0x14` = **terrain** type, `+0x15` = **overlay** code.

Both come from the *resource*'s `TTerrainList` (rw **tag 7**): a `u32 count`
then `count` `u16` entries, entry `i` = `terrain | overlay << 8`
(`HSEngine.TTerrainList.GetTerrain@55605054` / `GetOverlay@556050c8`).
`HSEngine.TMultiHexMO.OverrideTerrain@5560b2d4` writes them onto every hex a
multi-hex object covers, and only when the byte is `>= 0` as a **signed char**
(`0x80..0xFF` = "leave alone"), which is why a plain hexagon ships `0xFE`. The
port stores that "no opinion" as `-1` (`MapResource::overlay`).

Terrain codes: `0 water, 1 grass, 2 desert, 3 snow, 4 steppe, 5 wasteland,
6 ice, 7 earth, 8 rock, 9 lava, 10 dirt, 11 upper-wall, 12 upper-coast,
13 upper-ice, 14 coast, 15 border`.

Overlay codes (the cost table's column is **code + 1** - see
`docs/15_MOVEMENT.md` §2): `-1 none/plains, 0 mountains, 1 forest (dense
vegetation), 2 hill, 3 scrub, 4 road/city, 5 bridge, 6 rubble, 8 bones,
9 vegetation`.

> On every shipped map the **tag-50 base hexagon** resource reports overlay
> `0xFE` (no overlay): a forest/hill/mountain/road is a *separate object* on the
> hex whose own resource carries the overlay (e.g. `gr_tree.BMP` -> 1 = forest,
> `gr_willow.BMP` -> 3, a road `Rd_gr.BMP` -> 4 = 3 movement points,
> `MOUNTAIN\MOUNTAIN.ILB` -> 0 = impassable). `aow::fill_tile_terrain` takes each
> sprite's answer top-down (terrain and overlay independently, like
> `TMapField.UpdateTerrainType@55607df4` / `UpdateOverlayType@55607e48`) and
> `aow::move_point_cost` applies the +1 (`src/tests/movement_test.cpp`).

### Validation

All 144 `.HSM`/`.CSM` parse with **no desync**: every level reports exactly
`width*height` fields, `width`/`height` match the container, and every tile has a
terrain hexagon (`src/tests/map_test.cpp`: 2.76M tiles, 556k objects, 0 failures).
Dwarf1.CSM = 96x96, 3 levels; Tutorial.hsm = 48x48, 2 levels (its `--preview`
renders a coherent island).

### Resource resolution + rendering (Stage 2.2)

The sprite **tag 1** (`MapHs::resource`) is a `TMapObjectResource` id.
`HSEngine.TMapObject.ReadWrite` (HSEPack `0x55609824`) serialises it as
`TMapObject.GetResourceID` and resolves it against a `TMapObjectResourceList`
(raising `EMapObjectResourceMissing` when absent).

That list is **`Release.hss` tag 5** (`THSSet+0x24`, `HSEngine.THSSet.ReadWrite`
`0x55609280`): its child table holds the `TMapObjectResource` subclasses
(data-version dependent: **~439** on this GOG install - the older "1142" was a
different data build, like the 188->142 unit counts),
each `[u32 class id][table][data]` with

```
tag 1 u32   resource id         (matched by MapHs::resource)
tag 2 u32   TEResource type code
tag 3 u32   TEResource index
```

and an **embedded image-library name** (a `u32`-length string, e.g.
`EARTHHEX.ILB`, `WATERHEX.ILB`, `H_Border.BMP`). The terrain hexagon resources:

| resource class | n | Delphi class | example libraries |
|----------------|---|--------------|-------------------|
| `0x20153` | 122 | `Hexagon.THexagonResource` | `H_gr.BMP`, `H_sn.BMP`, `H_st.BMP`, `H_Wl.BMP` |
| `0x20171` | 2 | `IsoHex.TUpperIsometricHexagonResource` | `EARTHHEX.ILB`, `ROCKHEX.ILB` |
| `0x20558` | 12 | `BorderHx.TBorderHexagonResource` | `H_Border.BMP` |
| `0x2073e` | 2 | `AoWHex.TAoWWaterHexagonResource` | `WATERHEX.ILB`, `ICEHEX.ILB` |

The library name resolves to a file by stem (`H_gr.BMP` ->
`Images/TCombat/Base_hex/H_Gr.ILB`); the tiles are **48x32** hexagons
(45-degree flanks, 16px flat top; ILB class `0x11`, 16-bit RLE - decoded by
`src/engine/ilb.cpp`). They sit on the engine's axis-aligned, column-staggered
grid: `HSEngine.HXtoHP@8E9B8701` places a hex top-left at `px = x*32 + 8`,
`py = y*32 + (x & 1)*16` (odd columns shifted down 16px) - *not* a 45-degree
isometric diamond grid.

**Variant frames.** A scenario stores a *distinct resource id per terrain
variant*: within one `(res_type, library-stem)` group the resource ids carry a
contiguous **tag 3** (`res_index`) counter, and each library occupies one
contiguous run, so `frame = res_index - run_start` (`aow::map_resource_frames`,
`src/engine/mapviz.cpp`). A new run starts when `res_index` is not the previous
id `+ 1`. Confirmed blocks (terrain `class 0x20153`, `res_type 11`):

| library (stem) | res_index run | frames | library entries |
|----------------|---------------|--------|-----------------|
| `H_gr.BMP` | 9..20 | 0..11 | 12 (`H_Gr.ILB`) |
| `H_sn.BMP` | 22..29 | 0..7 | - |
| `H_st.BMP` | 30..39 | 0..9 | - |
| `H_Wl.BMP` | 49..63 | 0..14 | - |
| `H_Border.BMP` | 78..89 | 0..11 | (engine-loaded) |
| `H_Wa2.BMP` | 40..48 **and** 102..107 | 0..8 / 0..5 | (2 runs) |

Two rendering gotchas found while verifying this against `Tutorial.hsm` level 0:

* **`WATERHEX.ILB` (`Images/Waterhex.ILB`) is a *mixed* library** - it holds the
  `t_wa-*` water **transition** overlays (66x62/86x84) *and* the base `H_Wa2.BMP`
  water hexes. The water resource (`0x007b134f`) therefore names the file, and
  frame 0 is `t_wa-gr.BMP` (a transition), so 692 water tiles render as that
  overlay's khaki lattice. **Fixed (Phase 2):** `mapviz` selects the *base*
  entry by name (`terrain_frame` in `src/engine/mapviz.cpp`): base hexes are the
  entries whose name contains `h_` (`zH_Gr4`, `zH_Sn2`, `H_Wa2`); the overlays
  are `t_wa-*` / `T_Ic-*` / `ZT_*`. All 692 `Tutorial.hsm` water tiles now
  resolve to `h_wa2.bmp` (verified with the `--debug-csv` dump).
* **`H_Border.BMP` has no on-disk library.** `Images/BORDER.ILB` holds only
  `B_*.bmp` / `*border*.png` UI chrome, and a full scan of the game root finds
  **no `H_Border.ILB`** - the only libraries carrying `h_*` base-hex entries are
  `Images/IceHex.ILB` (`h_ic2.bmp`), `Images/Waterhex.ILB` (`h_wa2.bmp`) and the
  `Images/TCombat/Base_hex/` set (`H_De/H_Dr/H_Gr/H_Ic/H_La/H_Sn/H_St/H_Wa/H_Wl`,
  12 x `zH_*` 48x32 each). The border resource block is `res_type 11`, `res_index`
  78..89 - **12 variants, exactly like every other terrain hexagon library**.
  On a 48x48 map the frame is 6 hexes thick (`48^2 - 36^2 = 1008` tiles).
  **Fixed:** `H_Border.BMP` is a logical name with no file, so
  `object_library_alias()` (`src/engine/mapviz.cpp`) now maps it to the world-map
  water library (`waterhex`), whose `H_Wa2.BMP` base hexagon is the ocean tile the
  frame abuts. Probing `HowItShouldBe.png` (the original's screenshot) shows the
  frame and the open ocean are the *same* colour (~`(41,57,90)`), and after the
  change `Tutorial.hsm` L0 renders the frame at `(41,56,90)` instead of the old
  flat `rgb(30,30,40)` band. `mapres_test` asserts every `H_Border.BMP` hex
  resolves to a real sprite.
* **Multi-hex resources carry their own draw shift (`TMultiHexMOR`).** The
  terrain-object / mountain resource classes are `TMultiHexMOR` subclasses
  (`HSEngine.TMultiHexMOR.ReadWrite@5560b4ec`), and that `ReadWrite` reads **four
  extra integers at +0x34/+0x38/+0x3c/+0x40 with tags 12/13/14/15**.
  `TMultiHexMOR.SetdXmainP` / `SetdYmainP` write +0x34/+0x38, and
  `HSEngine.TMultiHexMO.Show@5560aaec` draws the sprite at
  `HXtoHP(hex) + resource[+0x34] / + resource[+0x38]` - so **tags 12/13 are the
  sprite's `dXmainP`/`dYmainP` pixel shifts** (negative: the visual top-left sits
  up/left of the object's hex). Parsed into `MapResource::dx_main`/`dy_main`
  (`has_shift`); tags 14/15 are still unidentified. Present on
  `0x20167 Terrain.TTerrainMOR` (e.g. `sn_tree.BMP` id 87 -> `(-14,-27)`) and
  `0x20169 Mountain.TMountainMOR` (grass `0x0a75015a` -> `(-48,-47)`,
  `0x0809390e` -> `(-77,-51)`), absent on plain `0x20153` hexagons. The mountain
  shifts grow with the resource's size index (idx 0 -> ~`(-48,-47)`, idx 2 ->
  ~`(-77,-55)`) - this is the per-sprite anchor. `mapviz` applies it as part of
  `object_sprite_origin()` (Phase 3, done; see "Object draw anchor" below).
  `Mountain.TMountainMO.CanPlace@556a2e54` confirms mountains are genuinely
  *multi-hex* (`TMultiHexMO`): it walks a hex neighbourhood and rejects overlaps
  with other mountains unless `ValidMountainOverlap` agrees. Each covered hex
  carries its own `TTerrainList` entry (`TMultiHexMO.OverrideTerrain@5560b2d4`);
  the port stamps them all (`aow::hn_walk_hex` + the second pass of
  `aow::fill_tile_terrain` - docs/15_MOVEMENT.md §1), so a mountain's whole
  footprint is impassable, not just the hex it is stored on. `Mountain.TMountainMO`
  goes one step further: it **overrides** `GetOverlay`
  (`Mountain.TMountainMO.GetOverlay@557a2dc8`) to answer *0 = mountains* for every
  hex it covers, whatever that hex's entry says, so a mountain range is a wall -
  its "slope" entries ship forest/hill and are mountains too
  (docs/15_MOVEMENT.md §1). `Terrain.TTerrainMO` (trees, rocks, the hill mounds),
  `Cave.TCave` and `Sign.TSign` are the other classes that touch the accessor -
  the first does not override it, the latter two answer "no opinion".
* **Backdrop coverage + grid geometry (Phase 1).** `render_map_rgba` lays a solid
  base hexagon under *every* hex before blitting its sprite, so a
  transparent/overlaid sprite no longer exposes the `(12,12,20)` backdrop. The
  tile size is confirmed against the decoded art (`detect_tile_size`) and the
  grid is the engine's `HXtoHP` transform above (axis-aligned, 32x32 pitch,
  column-staggered). `mapres_test` samples every hex centre with that formula
  and asserts none shows the backdrop.

* **Object libraries use *logical* names (Phase 2.5).** The terrain-object
  resources (`0x20167 Terrain.TTerrainMOR`, `0x20169 Mountain.TMountainMOR`, ...)
  name their library *logically* (`gr_tree.BMP`, `Rd_gr.BMP`, `gr_rocks.BMP`,
  `Crops.bmp`, `sn_tree.BMP`, ...). None of those names is a file or an ILB entry
  on disk - the sprites live in abbreviated files (`gr_tree.BMP` ->
  `Images/TCombat/Gr/zgr_Tre.ILB`, entries `zgr_Trees5.BMP`). `mapviz` maps them
  via `object_library_alias()` (`src/engine/mapviz.cpp`); Tutorial level 0 now
  resolves 319 of 362 objects (was ~120). `MOUNTAIN.ILB` packs five 5-frame
  terrain blocks (desert/grass/snow/stone/wall); the block is picked by the
  resource `res_type` (`mountain_block()`), the size by `res_index % 5`. Additive
  effects (`TWINKGR`, `SMOKE`) are skipped rather than blitted opaque.

  **Mountain block mapping - corrected.** The original `res_type` -> block table
  was guessed and had snow/wall/desert permuted, so a snow map drew *desert*
  mountains (`Mnt_de`) and a wall map drew *snow* mountains - i.e. a whole
  terrain type rendered with the wrong art. The mapping is now pinned by
  cross-referencing each map's mountain `res_type` mix against that map's terrain
  tiles (`tools/mtmap.ps1`, run over 14 maps):

  | `res_type` | terrain | block (first frame) |
  |------------|---------|---------------------|
  | 13 | wall | 20 (`wl_mnt`) |
  | 14 | snow | 10 (`Mnt_sn`) |
  | 15 | desert | 0 (`Mnt_de`) |
  | 16 | stone | 15 (`Mnt_st`) |
  | 17 | grass | 5 (`gr_mnt2`) |

  Evidence: the snow-only maps (`AoWx - Frosty Lagoon`, `Frozen Waters of
  Seshedut`, `Icedale`) use `res_type` 14 almost exclusively; the grass-only
  `Green Valley` uses 17; `Blackwood Forest` has **no desert tiles** yet its most
  common mountain type is 13 (and the wall volcanoes `MOUNTAIN\WL_VOLCS/L` carry
  res_type 13); 16 tracks the stone tile count and 15 the desert count
  (`Desolate Maugogom`, `Blood Isles`, `Lord of the Rings`). Verified after the
  fix: Frosty Lagoon draws 921 `mnt_sn` (was `mnt_de`), Blackwood draws
  `wl_mnt`/`gr_mnt2`/`mnt_st`, Tutorial is unchanged (`gr_mnt2`/`mnt_st`), and
  `mapres_test` asserts a no-desert map draws no `Mnt_de`. The two remaining
  mountain types (10, 22) are the underground twins and share the five surface
  shapes (identified by the resource's `dXmainP`/`dYmainP` triple); their
  `res_type` does not select a shape, so they stay unmapped pending a decompile
  of `TMapObjectResource.ShowComplexImage`.

* `aow mapres <Release.hss>` dumps the table (id -> class -> ILB).
* `aow map <map> --res <Release.hss>` prints the resolved terrain/object ILBs.
* `aow mapviz <map> <out.png> --res <Release.hss> --root <game-dir>`
  `[--level N] [--no-objects] [--max N] [--debug-csv <out.csv>]` renders a
  level (terrain hexagons + object sprites) to a PNG - `src/engine/mapviz.cpp`.
* Validation: `src/tests/mapres_test.cpp` (37 checks) - 1142 resources, exact id
  -> ILB mappings, the variant-frame runs (`H_gr.BMP` 9..20 -> 0..11,
  `H_Border.BMP` 78..89 -> 0..11), the `TMultiHexMOR` shift fields, every
  `H_Border.BMP` frame hex resolving to a sprite, the mountain terrain block on a
  snow map (Dwarf1), and a rendered PNG. Dwarf1 level 0 resolves to 5207 grass /
  2160 border / 1103 snow / 655 water tiles and renders a coherent campaign map.
* Inspection helpers: `tools/img_ascii.ps1` renders a PNG as an ASCII colour-class
  map, `tools/probe.ps1` prints per-row colour runs, `tools/greys.ps1` histograms
  low-saturation greys, `tools/sprstat.ps1` reports a sprite's mean colour,
  `tools/dump_res.py` / `tools/cls_ilb.py` dump `Release.hss` records,
  `tools/structseq_probe.py <Release.hss> [class_hex]` dumps every resource's
  image-sequence list (library / mode / period / frame table / chain - how the
  building animations were found, docs/17_BUILDING_ANIM.md),
  `tools/buildings_sheet.py <game-root> <out.png>` lays every building sprite out
  on a checkerboard (the per-building transparency check) and
  `tools/png_crop.py <in.png> <x> <y> <w> <h> <out.png>` crops a render, and
  `tools/mtmap.ps1` cross-references a map's mountain types against its terrain -
  these are how the checks above were verified without an image viewer.
* Golden harness: `src/tests/mapviz_golden.cpp` (`aow_mapviz_golden`) renders
  `Tutorial.hsm`, `Underground Titans.hsm` and `Dwarf1.CSM` level 0 to
  `re/golden/*.png` for eyeballing; `aow_view --debug-csv <out.csv>` dumps every
  hex's resolved entry (`layer,x,y,resource,ilb,frame,entry,w,h,ok`).

#### `aow_view.exe` - the clickable viewer

`src/game/viewer.cpp` builds a Win32/GDI front-end (`bin/aow_view.exe`) so the
map can be viewed without the command line:

* Double-clicking it shows the native *Open* dialog (filter
  `*.hsm;*.csm`) and renders the picked map in a scrollable, zoomable window.
* The game root is auto-detected by walking up from the map file and then from
  the executable, so putting the exe next to `Release/` just works; if
  `Release/Release.hss` is missing it offers a folder picker.
* File > Save PNG (via `src/engine/png.cpp`), View > Level 0/1/2 (surface /
  underground), View > Show objects, mouse-wheel zoom at the cursor, left-drag
  and arrow-key pan, `F`/`0` fit/100%, PageUp/PageDown level, drag & drop a
  `.hsm`/`.csm` onto the window.
* It paints straight from `aow::render_map_rgba` (new in `mapviz.hpp`), so the
  pixels never round-trip through a file.
* Head-less mode for scripts/CI: `aow_view <map> --root <dir>
  [--level N] [--no-objects] [--max N] --render <out.png>`. Used by
  `./build.ps1 -Test` and by the ctest case `viewer_render`.

### TODO(RE)

* The **stored variant frame** of a terrain library is now selected
  (`map_resource_frames`, see above), so hexes of one terrain type show their
  per-tile variant.
* **Objects pick their sprite through an image SEQUENCE, not a slot guess.** A
  multi-hex map-object resource carries **rw tags 9/10**: count + a chain of
  sequences (`TImageSequenceList.LoadFromStream` `ILPACK` `0x552190d8`,
  `TImageSequence.LoadFromStream` `0x55218a60`) - each with its library name,
  an animation mode/period, a **frame index** and an x/y offset.
  `HSEngine.TMultiHexMO.Show` -> `TMapObjectResource.ShowComplexImage` draws that
  frame from that library at `anchor + xoff/yoff`. `mapviz` uses it first. This is
  why `MOUNTAIN.ILB` (30 entries = five terrain blocks of five sizes at idx 0..24
  **plus the per-terrain hills at idx 25..29**, e.g. `gr_mnt2` 60x50) must not be
  indexed by an assumed block: the grass mountain resources are frames 26 / 8 / 9.
* **ILB libraries are indexed by image-slot id, not array position.** Each record
  carries an `index` (the image slot); e.g. `EARTHHEX.ILB` has slots 0..32 then
  40..43. `TCustomImageLibrary.Get(i)` uses the slot id, so `mapviz`'s library
  cache is indexed that way too (identical for contiguous libraries). This matters
  for the upper-isometric hexes below and for `HEXTRANS.ILB`'s 16-spaced blocks.
* **Raised terrain / the underground (Phase 5, composed).** `IsoHex.TUpperIsometricHexagon`
  (class `0x20170`) hexes cover the whole underground level; their resource is
  `IsoHex.TUpperIsometricHexagonResource` (`0x20171`, ids 1/2 = `EARTHHEX.ILB` /
  `ROCKHEX.ILB`). `IsoHex.TUpperIsometricHexagon.Show` (`HSEPack` `0x55619e24`)
  draws the base at `(sprite+0x15 >> 6) + 0x28` (a `RandInt(4)` variant -> slots
  40..43 = `hu_er.BMP` tops) and the cliff edges at slots `0x1f`/`0x20`/`0x1e`
  (`tu_er-rk.BMP`) for mask bits 0/1/5. `mapviz` draws the base variant; the edges
  are still open.
* **Terrain type list (`TTerrainList`, rw tag 7).** Each hexagon resource carries
  a `TTerrainList` at `resource+0x28`; `GetTerrain(list, 0)` returns its entry 0's
  **low** byte = the terrain type the transition/coast logic compares:
  `0` water, `1` grass, `2` desert, `3` snow, `4` steppe, `5` wasteland, `6` ice,
  `7` earth, `8` rock, `9` lava, `10` dirt, `11` upper-wall, `12` upper-coast,
  `13` upper-ice, `14` coast, `15` border. `TTerrainList.LoadFromStream` serialises
  `u32 count` + `count` u16 (low = terrain, high = overlay). `load_map_resources`
  parses it into `MapResource::terrain`/`has_terrain`, so the renderer no longer
  has to guess the terrain from the library stem. Evidence:
  `re/exports/hsepack/HSEngine.TTerrainList.{LoadFromStream,GetTerrain}.c`;
  locked by `mapres_test`.
* **Land terrain transitions (Phase 4, composed).** `mapviz` draws the land↔land
  overlays: `AoWHex.TAoWHexagon.UpdateTransition` (`AoWEPACK` `0x5579a9e0`) picks
  `HexTrans.ILB` frame `own*16 + 6 + edge + variant*3` for each upper edge
  (NW/N/NE = HN {6,1,2}) whose neighbour has a different land terrain, and
  `Hexagon.THexagonResource.Show` blits it at `(-6,-9)`. The neighbour terrain is
  now the real `TTerrainList` value. The water/coast/ice branches (`own`/neighbour
  `0`/`6`) and the "long" variants remain unimplemented: a land hex deliberately
  draws nothing toward water (`n != 0 && n != 6` guard), so the shoreline is the
  water hexagon's job (next bullet). `re/exports/aowepack/AoWHex.TAoWHexagon.UpdateTransition.c`
  has the full rule.
* **Coastline / shoreline shelf (Phase 4, composed).** The wave/shelf a player sees
  between land and sea is drawn by the **water hexagon**, not the land hexagon:
  `AoWHex.TAoWWaterHexagon.ShowDynamic` (`AoWEPACK` `0x5579afb4`) calls
  `IsoHex.TLowerIsometricHexagon.ShowDynamic` (`HSEPack` `0x5561a400`), which reads
  the sprite's `+0x14` mask (one bit per direction, set by
  `AoWHex.TAoWWaterHexagon.UpdateTransitions` `0x5579ad98` for every neighbour that
  carries **no** `TLowerIsometricHexagon`/`TBorderHexagon` - i.e. the flat land) and
  composites up to a handful of the water library's `t_wa-gr` slots `0..0x1e`
  (`WATERHEX.ILB`) at `x = HXtoHP - 0x13`, `y = HXtoHP - 0x20` (most) or `- 0x1f`
  (the corner frames `0x16/0x13/0x1e/0x12/0x1c`), adding a per-hex phase
  `(sprite >> 8) & 3` (3 -> 0). `mapviz` reproduces this rule
  (`aow::water_shore_pieces` + `aow::water_shore_variant`; locked by
  `mapres_test`); the per-direction snow/wasteland *slot offsets*
  (`t_wa-sn`/`t_wa-wl`) are still drawn from the `t_wa-gr` slots.
* **The water hexagon's animation (Phase 4c, done - `docs/18_WATER_ANIM.md`).**
  Two halves:
  * `AoWHex.TAoWWaterHexagon.Show` (`0x5579aeac`) draws the base tile as
    `((field>>3) + x + y*2 + (sprite+0x15>>6)) & 0xf + 0x8c` - the sixteen
    `H_Wa2` wave frames `WATERHEX.ILB` carries at `0x8c..0x9b` (48x32, all
    distinct) - so each water tile shows its own wave phase - plus the
    `T_Ic-Wa2` foam (slots `0x24/0x25/0x23` for `sprite+0x15` bits 0/1/5) at
    `(-6,-9)` where a neighbour of the hexagon's own water/ice family
    (`IsoHex.TLowerIsometricHexagon`) has a *different* terrain - the water/ice
    boundary, drawn out of `WATERHEX.ILB` *and* `ICEHEX.ILB`.
  * `ShowDynamic` (`0x5579afb4`) then drives the resource's own image-sequence
    list once per rendered map frame from a **private per-hex counter**
    (`sprite+0x18`, seeded `-1 - RandInt(0x14)` by `Create@5579ad40`, parked at
    `-(0x14|10) - RandInt(0x19)` when the sequence runs out) and an entry index
    (`sprite+0x19`) rolled at `0x14 + RandInt(10)` (the ten `WATERANM.ILB`
    ripples), `0x1e + RandInt(10)` (the `Fish1-*`/`FishSplas1-*` chains, 1 in
    700) or `0x32 + RandInt(9)` (the `TCOMBAT\OBJECTS\SEFFECT.ILB` sparkles).
    The frame comes from `ImageLib.TImageSequence.GetUnloopedFrameIndex`
    (`ILPACK` `0x552188a8`) - the one-shot sibling of the resolver the buildings
    and units use - and every sprite is ILB class `0x16`, so it is composited
    with the GFXE **brighten** blend.
  * `aow::WaterHexAnim` / `aow::image_sequence_unlooped_frame` /
    `aow::draw_water_overlays` implement it; the live screens re-composite their
    viewport once per map frame, a one-shot render bakes it at
    `MapVizOptions::clock`.
* **Transition overlay mechanics (Phase 4 RE).** The overlay library for a base
  terrain hexagon is a constant: `Hexagon.THexagonResource.MsgProc`
  (`HSEPack` `0x55611e50`) links `TImageLibraryManager.LinkToIL(..., 1, "HexTrans.ILB")`
  into `resource+0x34`, and `Hexagon.THexagonResource.Show` (`0x55611fa4`) draws
  up to three overlays from it, chosen by the sprite's `+0x13` flag bits
  (frame = the neighbour terrain index stored at `sprite+0x10/+0x11/+0x12`; short
  offsets `(-6,-9)`, long-variant bits 3/4/5 give `(-6,-41)/(-38,-25)/(26,-25)`).
  Those sprite bytes are **not stored** in the `.HSM` (a tag-50 terrain record
  carries only tag 1); they are computed at runtime by `HSEngine.TMapField`
  (terrain type at `+0x14`, overlay at `+0x15`; `TMultiHexMO.OverrideTerrain`
  `0x5560b2d4` writes them, `TMapField.TerrainChanged` `0x55607cb4` /
  `UpdateNeighbourTerrain` `0x55607f9c` propagate to the 6 neighbours).
  Evidence: `re/exports/{hsepack,aowepack}/Hexagon.THexagonResource.{Show,MsgProc}.c`,
  `HSEngine.TMapField.*`, `AoWHex.TAoWWaterHexagon.*`,
  `IsoHex.TLowerIsometricHexagon.ShowDynamic.c`.
* **Base-tile selection in a mixed library.** **Closed (Phase 2)** - `mapviz`
  now picks the base entry by name (`terrain_frame`), so `WATERHEX.ILB` and any
  other mixed terrain library draw their `h_*` base hexes instead of a
  transition overlay.
* **`H_Border.BMP` library** is not on disk (`Images/BORDER.ILB` is UI chrome),
  so border hexes fall back to a flat colour. Where the engine loads the border
  sprite from (assumed `HEXTRANS.ILB` / `CTransHx.ILB`) is unresolved.
* **Object-library *logical* names - RESOLVED (inline libraries).** Some
  terrain-object resources name their library *logically* (`gr_tree.BMP`,
  `gr_rocks.BMP`, `st_bones.BMP`, `Crops.bmp`, ...). Those names match no file or
  ILB entry anywhere, and `TILManagerList.LinkToIL` (`ILPACK` `0x55217088`)
  resolves names by an **exact, uppercased relative path** from the directory scan
  in `FUN_55217460` - so the engine cannot be loading them from disk. The answer:
  `TMapObjectResource.ReadWrite` reads **rw tag 8** into `resource+0x20` - an
  image library **serialised inline**, and its data is a complete ILB image
  (magic `04 'I' 'L' 'B'`, version 4.0, `header_size` 24, `data_offset`,
  `total_size`). `mapviz` now decodes those bytes with `IlbFile::load_memory` and
  uses the library's slot 0 (the engine's `Get(resource+0x20, 0)` in
  `HSEngine.TMultiHexMO.Show`), falling back to the named-file/alias path only
  when the inline library is absent or does not parse. Evidence:
  `re/exports/hsepack/HSEngine.TMapObjectResource.MsgProc.c`,
  `re/exports/ilpack/ImageLib.{TILManagerList.LinkToIL,TImageLibraryManager.ExecutePathSearch}.c`.
  **Roads - RESOLVED (Phase 4b).** A road is a *placed object* (engine class
  0x20164 `Road.TRoad`) whose resource is class 0x20165 `Road.TRoadResource`
  (`Road.TRoadResource.ClassID` `0x55617134`). Release.hss holds five of them
  (ids 21/47/48/49/38, `res_type` 6 -> `Rd_de`/`Rd_sn`/`Rd_st`/`Rd_Wl`/`Rd_gr`).
  `Rd_gr.BMP` names no file or ILB entry, but the resource DOES carry its overlay
  library inline (rw tag 8): a complete 6688-byte ILB whose 8 entries - all named
  `Rd_gr.BMP` - sit at image slots **1..8** (60x50 placement cells; each frame's
  own RLE content is one road segment). The engine recomputes a per-road
  transition byte from the six neighbouring fields
  (`Road.TRoad.UpdateTransitions@55616f8c`): bit `dir-1` is set when the neighbour
  carries a road, **or** - in the loaded game
  (`AoWRoad.TAoWRoad.UpdateTransitions@5579b84c`) - when the neighbour answers the
  road query message `0x1131`, which `AoWE.TStructure.MapFieldMsgProc@5575f730`
  answers for a structure whose resource byte `+0x45` is set. That is what makes a
  road lead into a city/building (`AoWE.TStructure.UpdateRoads@5575e70c` re-runs
  the neighbouring roads when a structure is placed). The override additionally
  invalidates the cached static scene. Then the matching slots are drawn
  (`Road.TRoadResource.Show@5561713c`) at the road anchor
  `HXtoHP(hex) + (-14,-9)` (`Road.TRoad.Show@55616ed4`). `mapviz` reproduces all
  of it (`aow::road_transition_byte`, `aow::road_frames`, the road pass in
  `src/engine/mapviz.cpp`); see `re/exports/hsepack/Road.*.c`. The `Rd_<terrain>`
  -> `H_<Terrain>Dr.ILB` pairing is also registered in `object_library_alias()`
  as a fallback. NB: the earlier "carries no inline library" note was wrong - the
  parse condition now finds the tag-8 image.

* **Bridges (Phase 4d, done).** The shipped scenarios place `HxBridge.THexagonBridge`
  objects (engine class `0x2087a`, `HxBridge.THexagonBridgeResource` class
  `0x2087b`). There are five records (`res_index` 24..28): two surface
  `Str_brdg.bmp` and three underground `U_brdg.bmp`, each a **single 78x65 sprite
  with a three-frame library carried INLINE** (rw tag 8, all entries named after
  the resource). The map field stores only the bridge's resource id (`tag 1`), and
  `HxBridge.THexagonBridge.Create@5579973c` initialises its orientation byte
  `+0x11` to `0xff`, so the engine recomputes it on load:
  `UpdateTransitions@55799fd8` tests `DirectionValid@55799c04` in the order
  **1, 2, 0** (keeping a still-valid stored value) and writes `0..2`, or `0xff`
  when none applies. `DirectionValid(d)` is true when the neighbours at HN
  `(d+1)` and HN `(d+4)` are both land; on the column-staggered grid
  (`HSEngine.CenterHNtoHX@5560e17c` = `HNtoHX` + the parity fold) those pairs are
  **N/S**, **NE/SW** and **SE/NW** - which is exactly the three frames of the art.
  `Show@55799a70` then draws `TCustomImageLibrary.Get(resource+0x20, +0x11)` at
  `HXtoHP(hex) + (-0x17,-0x14)`. `mapviz` reproduces it (`aow::bridge_land` =
  `AoWE.Land@5575aea4` minus {water 0, ice 6, lava 9, dirt 10, upper-ice 13};
  `aow::hex_bridge_direction`; the Phase 4d pass). NB: the older base-engine
  `Bridge.TBridge`/`TBridgeResource` (HSEPack classes `0x20174`/`0x20175`,
  `Wa_bdge.BMP`/`Ula_bdge.BMP`, a 19-frame multi-piece overlay selected from a
  6-direction byte by `Bridge.TBridgeResource.Show@55617e8c` at `HXtoHP + (-14,-9)`)
  is not placed by the shipped scenarios. Evidence:
  `re/exports/aowepack/{HxBridge.THexagonBridge.UpdateTransitions,
  HxBridge.THexagonBridge.DirectionValid, HxBridge.THexagonBridge.Show,
  HxBridge.THexagonBridge.ClassID, HxBridge.THexagonBridgeResource.ClassID,
  AoWE.Land}.c`, `re/exports/hsepack/HSEngine.CenterHNtoHX.c`.

* Object sprites: the descriptor's placement fields (`base` +0x28/+0x2c, and
  `shift` +0x50/+0x54) are **parsed** into `IlbEntry` (`base_x`/`base_y`,
  `xshift`/`yshift`; asserted by `mapres_test`) and the engine blits content at
  `param + base + shift`. Every sprite class carries the pair at the same
  descriptor position: `TRLESprite08` (0x02) at +0x50/+0x54 (`c+8`/`c+12`),
  `TRLESprite16` (0x11/0x12) at +0x50/+0x54, `TSprite16` (0x16) at +0x5c/+0x60
  (`re/exports/ilpack/TSprite16.ShowOpaque.c`). Terrain is calibrated (`H_Gr`
  base hex = 48x32 at shift 0). The bitmap fonts are 0x02 sprites whose `yshift`
  is their baseline offset (0 for the caps, 2–4 for the x-height and descenders)
  — `aow_menu`'s `decode_glyph` folds it in so every glyph sits on the baseline.
* **Object draw anchor (Phase 3, done).** The map-object draw path is decompiled
  (`re/exports/hsepack/`): `HSEngine.TMapObject.Show@55609a00` draws a *single-hex*
  object at `HXtoHP(hex) - 8`, and `HSEngine.TMultiHexMO.Show@5560aaec` draws a
  *multi-hex* object at `HXtoHP(hex) + resource dXmainP/dYmainP` (tags 12/13). In
  both cases the image's `Show` then adds `base + shift`, so the sprite's visual
  box lands at `anchor + base + shift`. `mapviz` implements this as
  `aow::object_sprite_origin()` (the renderer's 8px grid margin cancels the
  single-hex `-8` and is added for multi-hex objects). The earlier "regression"
  when applying base+shift was the missing `dXmainP/dYmainP`. Objects whose
  resource carries an image-sequence list (`resource +0x24 != 0`) are drawn by the
  engine as a *chain* of images (`HSEngine.TMapObjectResource.ShowComplexImage@5560a134`,
  each frame at `anchor + sequence+0x24/+0x28`); `mapviz` still draws the single
  `Get(...,0)` image, so the mountain size/block rule stays an empirical fit
  (roadmap Phase 3 known gap).
* **Structure placement via the image sequence (Phase 4c, done).** A structure
  (City/Mine/Tower/WizardsTower/...) whose resource carries an image-sequence list
  (`resource+0x24`, rw tags 9/10) is drawn by `AoWE.TStructureResource.Show@557605dc`
  via `ShowComplexImage` at `HXtoHP + sequence xoff/yoff + base + shift`; the
  multi-hex `dXmainP/dYmainP` (+0x34/+0x38) is only the *fallback* for the
  single-image branch (and `map+0x40 & 6 == 6`). The tag-9 blob is
  `u32 head` + 1-2 housekeeping bytes + the first sequence's `rwString` library +
  the offset fields (**xoff at name_end+7, yoff at name_end+11**). `load_map_resources`
  now scans a few positions for a plausible length-prefixed library name (City3 has
  one housekeeping byte, Mine/Tower/WizardsTower two), so the sequences parse for
  all of them - WizardsTower's `xoff/yoff = (-10,-90)` is what grounds its 120px
  tower sprite; the Mine's `(-16,-22)` ≈ its `dXmainP` so it is unchanged. Frame
  index is the object's own cell image `MO+0x14` `[TODO(RE)]` - the port still uses
  resource frame 0. Evidence: `re/exports/aowepack/{AoWE.TStructure.Show,
  AoWE.TStructureResource.Show}.c`.
* **A structure's BODY is the effect's *chain*, not the list's first name
  (Phase 4f, done - this is why ports were missing).** A map-object resource's
  image-sequence list (rw tags 9/10) can **open with an additive-effect
  sequence**: `Shipyard.TShipyardResource` (class `0x204bf`, ids `0x3c`/`0x6b`)
  entry 1 is `SMOKE.ILB` (mode 4, a 12-frame table) and its `.next` chain (`+0xc`)
  carries the dock itself - `PRO_SYRD.ILB`, mode 0, base frame 0, `xoff/yoff =
  (-19,-25)`. `HSEngine.TMapObjectResource.ShowComplexImage@5560a134` walks that
  chain and draws *every* sequence whose `TImageSequence.GetFrameIndex` is valid;
  the old loader instead scanned the blob for the first library name, recorded
  `SMOKE.ILB`, and the renderer then dropped the whole object through
  `is_transient_effect()` - **so no Shipyard/port was ever drawn**.
  `load_map_resources` now parses the *whole* `TImageSequenceList` (with the same
  `aow::parse_image_sequence_list` the unit animation uses, so both NULL
  placeholders and chains are reconstructed) and `aow::body_sequence()` selects the
  body: the first sequence - entries in list order, each entry followed by its
  chain - whose library is not an additive effect (`aow::is_effect_library`).
  `has_seq` / `seq_lib` / `seq_frame` / `seq_dx` / `seq_dy` describe that body, and
  `mapviz` only skips an object as an effect when the sequence it resolved really
  is one (`!Picked::by_seq`). Rendering `Underground Titans` L0, the 4
  `Shipyard.TShipyard` hexes now draw `PRO_SYRD.ILB` `pro_syrd2.bmp`, while every
  other object's library/frame is unchanged (`--debug-csv` before/after diff).
  `[TODO(RE)]` The object's own cell image (`MO+0x14`, the sequence *start* index
  `ShowComplexImage` is handed) is still not parsed, so the port uses "first
  non-effect sequence" as the body proxy - it matches `STR_MINE` / `STR_TOWR` /
  `STR_WTWR` / `PRO_SYRD` for every shipped structure.
  Evidence: `re/exports/hsepack/HSEngine.TMapObjectResource.ShowComplexImage.c`,
  `re/exports/anim/ILPACK_dpl/TImageSequenceList.LoadFromStream.c`; asserted by
  `mapres_test` ("body_sequence skips the effect and returns the PRO_SYRD dock").
* **A structure's chain is ANIMATED (Stage 2.6, done).** `ShowComplexImage` draws
  every sequence of the chain, and the chain carries the moving part: the mine's
  body is `STR_MINE.ILB` mode 0 chained to a mode-5 6-frame sparkle, the wizard
  tower's is mode 0 chained to a mode-4 12-frame orb (`frames 10..21`), a
  teleporter has four mode-4 shimmer chains, and a node/farm/elven-cave/grove body
  is itself mode 4. Each sequence's frame is
  `ImageLib.TImageSequence.GetFrameIndex@0x552186ec(seq, AoWHSMap+0x40, phase)`
  with the per-object phase `hex_x + hex_y*0x10` (the expression
  `AoWE.TStructureResource.Show@557605dc` / `HSEngine.TMapObject.Show@55609a00`
  compute; `TMultiHexMO.Show@5560aaec` uses the `HXtoHP` pixel sum). The renderer
  resolves the chain (`MapVizOptions::clock`; `draw_structure_overlays`) and the
  live screens re-composite it each map frame. See `docs/17_BUILDING_ANIM.md`.
* **Cities: race body, walls and the pad hill (Phase 4e, done).** `City.TCity`
  (class `0x20108`) draws via `AoWE.TCity.Show` -> `TCityResource.ShowCity` as
  three layers at `HXtoHP + resource dXmainP/dYmainP + sprite base+shift`: back
  wall (`+0x68`), body (`+0x70`), front wall (`+0x6c`). `TCity.UpdateImages`
  picks the body from the owner race's `TRaceResource` image library
  (`GetCityImage(size, terrain, level)`: slot `terrain*10 + size + 0x45`, or
  `size + 0x3b` for terrain 5, else `size + (level == 0 ? 0x32 : 0x3c) - 1`) and
  the walls from the city resource's own `CityN.ILB` block (`T + wallType*2`).
  GetCityImage's last argument is the **map field's level byte**
  (`TMapField+0x12`; `TMapField.GetXYL` packs `x + y*0x100 + L*0x10000` from
  `+0x10/+0x11/+0x12` and `UpdateTerrainType` reads the terrain from `+0x14`),
  *not* the city's wall type (`TCity+0x4c`). So level 0 (surface) falls back to
  the race library's overworld block (slots 50..53) and a non-zero level
  (underground) to its underground block (slots 60..63); the
  `Scenario\Custom\Barondir Bay.HSM` Human town at (12,20) is on the surface and
  its Human library ships only the underground block, so it correctly ends up
  drawing `CITY4.ILB` slot 0 `C_human.bmp`. `TCity.ReadWrite`
  serialises the **name** at rw **tag 0xc** (`+0x48`, a `rwString`: one length
  byte, `0xff` => u32 length, then the bytes - `TCity.GetName@557ac918` is just
  an `LStrAsg` of it, so the scenario stores "Venice ", "Ricordia", ...),
  the race at **rw tag 0xd** (`+0x45`), the size/level byte at **rw tag 0x10**
  (`+0x4d`) and the wall type at **rw tag
  0xf** (`+0x4c`). A city whose race tag is **0xff** (the editor's "Random" city
  race, e.g. every city of `Scenario\Custom\test.hsm`) has no race of its own:
  `GetCityImage` yields nothing and `UpdateImages` draws the city resource's own
  `C_random` "?" block - so the port only draws that placeholder for a city with
  **no usable owner**, and otherwise uses the owner player's race
  (`TPlayer.GetRace`, `+0xa5`; `TCity.UpdateFlagID` packs that same race into the
  city's flag byte), mirroring the flag the city already flies
  (`aow::city_body_race`). The 12 race libraries (`RACES\R{HUMAN,AZRAC,...}.ILB`, faction
  order = Human 0 / Orc 9 / Goblin 10) are the game pack's `TRaceResourceList`
  (`TAoWWHSSet` rw **tag 10** -> `+0x54`; each `TRaceResource` rw **tag 8** is the
  library name, a `rwString`). The city's **hill** is a separate `Pad.TPad`
  (`0x20106` / `0x20107`) map object already placed on the tile; `PADn.ILB` slots
  1..5 are the grass/desert/snow/stone/wall mounds. Evidence:
  `re/exports/aowepack/{AoWE.TCity.Show, AoWE.TCityResource.ShowCity,
  AoWE.TCity.UpdateImages, AoWE.TCity.{SetRace,SetWallType,ReadWrite},
  AoWE.TRaceResource.{GetCityImage,ReadWrite,LinkToIL}, AoWE.TAoWHSSet.ReadWrite,
  Pad.TPad.ClassID, Hill.THillMOR.ClassID}.c`; port helpers in
  `src/engine/mapviz.cpp` / `src/engine/map_resources.cpp`.
* `THSMap` tags 3/4/5/6 (bytes/ints at `THSMap+0x34/0x80/0x84/0x88`) and the
  `TAoWHSMap` fields beyond the player/race lists are decoded structurally only.
* The player/setup lists (`tag 0x15` TPlayerList, `0x16` TRaceList) are located
  but their per-record tags are not yet mapped.

## AHL / AIL - hero & item list rules

Small text-record files. Layout (verified on the shipped `High Level.ahl` /
`Special.ahl`): `u32 0x02000001 | u16 tag14 (= 20) | u8 1 | u8 len+1 | u8 len |
name[len] | NUL`, e.g. `High Level.ahl` = `01 00 00 02 14 00 01 0b 0a
'High Level' 00`, `Special.ahl` = `01 00 00 02 14 00 01 08 07 'Special' 00`.
Only the embedded name is interpreted; the remaining field semantics are still
[TODO(RE)]. The units loader reads them (`aow roster`, see `docs/11_UNIT_CATALOG.md`).

## DPL - Delphi packages (+ EXE/DLL)

Standard PE32 DLLs. Exports are Borland-mangled (`@Unit@Method$qqr...`),
DFM forms live in `.rsrc` as `TPF0` resources. Tools: `pe_inspect.py`,
`dpl_index.py`, `dfm_extract.py`.

## Binary DFM (form resource)

Resource payload starts with `TPF0`; then a binary object tree (object flags,
short-string class/name, property list with typed values). `dfm_extract.py`
saves raw copies and has an experimental `--parse` tree dump.
TODO: a lossless binary-DFM -> text-DFM converter for clean diffing/porting.
