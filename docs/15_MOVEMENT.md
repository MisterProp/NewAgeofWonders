# 15 - World-map unit movement

This document records the reverse-engineering of **how Age of Wonders 1 moves an
army on the world map** - the terrain move-cost tables, the per-unit and
per-army movement rules, the pathfinder and the path overlay sprites - and its
C++ reconstruction in `src/include/aow/movement.hpp` +
`src/engine/movement.cpp` (+ `src/include/aow/move_cost_tables.hpp`), the
`aow::fill_tile_terrain` / `aow::draw_move_path_overlay` map hooks, the viewer
interaction in `src/game/viewer.cpp` and the tests in
`src/tests/movement_test.cpp`.

Everything was recovered with **GhidraMCP** (`tools/ghidra/`) from
`AoWEPACK.dpl` (image base `0x55700000`) and `HSEPack.dpl` (`0x55600000`); the
decompiles live under `re/exports/move/`. No separate data file is needed: the
cost tables are the engine's own startup tables, dumped from the binary and
turned into a header by `tools/gen_move_cost_tables.py`.

## 1. The two map axes: terrain and overlay

`HSEngine.TMapField` carries the two bytes the move-cost table is indexed by:

* `+0x14` = **terrain** type
* `+0x15` = **overlay** code

They come from the resource's `TTerrainList` (rw **tag 7**): a `u32 count`
followed by `count` `u16` entries, entry `i` being `terrain | overlay << 8`
(`HSEngine.TTerrainList.GetTerrain@55605054` = the low byte,
`GetOverlay@556050c8` = the high byte; a missing entry reads `0xFE`).

`HSEngine.TMultiHexMO.OverrideTerrain@5560b2d4` is what writes them onto a field:

```pascal
for i := 0 to count-1 do begin
  t := GetTerrain(i);            // TTerrainList entry i, low byte
  o := GetOverlay(i);            // entry i, high byte
  if t >= 0 then field.terrain := t;      // `>= 0` as a SIGNED char
  if o >= 0 then field.overlay := o;
end;
```

so `0x80..0xFF` means "leave it alone" - a plain grass hexagon resource ships
`0xFE` for the overlay. The terrain codes are (docs/01_FORMATS.md, and the
`terr=` column of `aow mapres Release/Release.hss`):

```
0 water   1 grass   2 desert  3 snow    4 steppe   5 wasteland  6 ice
7 earth   8 rock    9 lava    10 dirt   11 upper-wall  12 upper-coast
13 upper-ice  14 coast   15 border
```

**Finding (evidence in §7):** on every shipped map the *tag-50 base hexagon*
resource reports `0xFE` for the overlay - a forest, a hill, a mountain or a road
is a **separate overlay object** on the hex (`Terrain.TTerrainMO`, a
`Road.TRoad`, ...), and *its* resource's tag-7 entry 0 carries the overlay.
`HSEngine.TMapField.UpdateTerrainType@55607df4` / `UpdateOverlayType@55607e48`
ask the field's sprites from the **top of the stack down** and stop at the first
non-negative answer, so `aow::fill_tile_terrain` walks each tile's sprites in
reverse - tag 50 is the terrain hexagon, tags >= 51 the objects stacked on it.

**Finding (the decoration-terrain bug):** the *terrain* answer is gated by the
sprite's own flag: `TMapObject.GetTerrain@55609780` returns "no opinion" (-1)
whenever the object's flag byte (`+0xc`) has bit 4 set, and *every* decoration
object has it set. That gate is essential because a decoration's tag-7 terrain
can be garbage: `Crops.bmp`, `PAD1.ILB` and `STR_FARM.ILB` all ship
`terrain 0` (= **water**) in tag 7 (`TT=ff00`) while sitting on grassland
(`H_gr.BMP`, `TT=fe01`). Without the gate the crops would override the hexagon
and turn whole swathes of an island into impassable water - the "movement is
completely different" bug. `aow::fill_tile_terrain` reproduces the result: the
field's **terrain** comes from the **tag-50 base hexagon** only (a tile with no
base hexagon falls back to the topmost object that carries one), while every
object sprite contributes only its **overlay** (forest/hill/mountain/road/...).
A tile no sprite answers for keeps `terrain = -1` / `overlay = -1`.

**Finding (a multi-hex object covers several fields).** The terrain/object
resources are `HSEngine.TMultiHexMO` subclasses whose `TTerrainList` holds **one
entry per covered hex**, not only the object's own: `TMultiHexMO.OverrideTerrain
@5560b2d4` walks the object's hex neighbourhood (`HSEngine.InitHNWalk@5560e5c8` +
`WalkToNextHN@5560e610`) and, for covered hex `i`, writes `GetTerrain(i)` /
`GetOverlay(i)` onto that field when the byte is `>= 0` (signed). The walk starts
at the object's own hex with radius 0 and spirals clockwise - `(0,0), (0,-1),
(1,-1), (1,0), (0,1), (-1,0), (-1,-1), (0,-2), ...` on an even column - driven by
the direction table `DAT_5562e250` (HSEPack file **0x02d050**, indexed
`[edge 0..5][x-parity]`), and it agrees with `Mountain.TMountainMO.CanPlace
@556a2e54`, which builds the same entry<->hex pairing through
`HSEngine.CenterHNtoHX@5560e17c` (the per-field index is stored and read back with
`TMapField.SetHNofHS@55608364` / `GetHNofHS@556083a0`). Both halves are
implemented: `aow::hn_walk_hex` and the second pass of
`aow::fill_tile_terrain`.

**Finding (a mountain is a wall over its whole footprint - the reported "you can
still pass through mountains" bug).** The per-hex entries are *not* the whole
story: `Mountain.TMountainMO` **overrides** the overlay accessor. The body of
`Mountain.TMountainMO.GetOverlay@557a2dc8` (AoWEPACK.dpl file `0xa21c8`, dumped
with `python tools/hexdump_va.py AoWEPACK.dpl --va 0x557a2dc8 --len 20`; the
ClassID getter right before it is `b8 68 01 02 00 c3` = `mov eax, 0x20168`, which
is what validates the mapping) is

```asm
mov  eax, [eax+8]        ; the object's resource
mov  eax, [eax+0x28]     ; its TTerrainList
call TTerrainList.GetOverlay   ; entry 0 - the object's OWN hex opinion
test al, al
jl   keep                ; "no opinion" (0x80..0xFF) -> return it as is
xor  eax, eax            ; else -> 0 = MOUNTAINS
ret
```

so a mountain answers **0 (mountains) for every hex it is asked about**, whatever
the entry of that hex says - the covered-hex index is ignored. The big grass
mountain `0x0809390e` ships `[0|1, 0|1, 0|1, 1|1, 2|1, 1|1, 0|1, ...]`: entry 0's
opinion is `0`, so *all* of its covered hexes are mountains, including the three
"slope" entries that declare forest/hill themselves. (Before the override was
modelled the port stamped only the entries that declared `0` themselves, so a
range could be threaded hex by hex through its own slopes and its own hex when
entry 0 declared a hill.) The class also explains two related behaviours:

* the hexes an object covers are the ones `TMultiHexMO.UpdateMapFields@5560af6c`
  registers it on: the walk stops at the entry count and registers - then re-runs
  the field's `UpdateOverlayType@55607e48` - only when the entry has an opinion
  (terrain *or* overlay), so an entry that ships `0xFEFE`/`0xFFFF` covers nothing;
* only three map classes override the accessor at all: `Mountain.TMountainMO`
  (answers 0), `Cave.TCave@557b3878` and `Sign.TSign@557a44e8` (both answer
  `-1` = "no opinion"). `Terrain.TTerrainMO` (class `0x20166` - trees, rocks and
  the 60x50 **hill mounds**) does *not* override it, so a hill mound keeps
  answering *hill* and stays walkable even underneath a mountain - which is how
  the tutorial's road pass works.

`aow::fill_tile_terrain` therefore forces the claim to `0` for an object whose
class is `aow::kMountainMoClassId` (0x20168) as long as its entry 0 has an
overlay opinion, in both passes (its own hex and its covered hexes). On
`Tutorial.hsm` L0 that takes the overlay histogram from `0=152` to `0=313` and
the walker-impassable tiles from 1852 to 2013 of 2304, while the hill mounds the
tutorial's army walks over (`(16,32)`, `(17,32)`, `(18,32)` - all
`Terrain.TTerrainMO`) and every road stay exactly as they were.

The overlay codes seen in `Release.hss` (the whole distribution is in §7):

```
-1 none/plains   0 mountains   1 forest / dense vegetation   2 hill
 3 scrub/decor   4 road, city  5 bridge   6 rubble   8 bones   9 vegetation
```

`terrain` / `overlay` are exposed on `aow::MapResource` and `aow::MapTile`.

### The overlay code is one BELOW the cost table's column

The tables index their column 0 with *no overlay* (plains), so the raw code above
is shifted by one when the cost is looked up: **`column = overlay + 1`**
(`-1 -> 0`, `0 = mountains -> 1`, `1 = forest -> 2`, `4 = road -> 5`,
`5 = bridge -> 6`). `aow::move_point_cost` applies that shift; using the code
directly is what made **forests impassable and mountains cost only 4** - the
cost table's column 1 (raw code 1, "forest") is `0xFF` for a walker while its
column 0 (raw code 0, "mountains") is 4.

## 2. The move-cost tables

At startup `FUN_55744b2c` builds `MovePointTables` (256 tables x 256 bytes, BSS
at `0x558ea040`) from **eight 256-byte source tables** stored back-to-back at
`0x558e84fc`:

```
fill MovePointTables with 0xFF
for move_type := 0 to 255:
  for terrain := 0 to 12:
    for overlay := 0 to 15:
      v := 0x7F
      for each bit b of move_type, in the order 0x01,0x04,0x02,0x08,0x10,0x20,0x40,0x80:
        s := src[b][terrain*16 + overlay]
        if s <> 0xFF and s < v then v := s
      MovePointTables[move_type][terrain*16+overlay] := (v = 0x7F) ? 0xFF : v
```

i.e. the **cheapest of the selected movement modes**, and `0xFF` (impassable)
when none applies. `FUN_55744b2c` copies cell `terrain*16 + overlay` verbatim
into the per-unit table, but the two *lookups* add one - the table's column 0 is
"no overlay" (`overlay = -1`), column 1 is the code 0 (mountains), column 2 the
code 1 (forest), and so on:

* `TAbstractUnit.MovePointCost@0x55780848` returns `acStack_106[i]`, and the
  256-byte table `CreateMovePointTable` filled sits at `local_107` - the frame
  byte *before* `acStack_106` - so it reads `table[terrain*16 + overlay + 1]`;
* `TMoveControl.DefaultMoveOnMovePointProc@0x557454d8` reads
  `*(char*)(mc + field->terrain*0x10 + 0x31 + field->overlay)`; the table starts
  at `mc + 0x30`, so a field overlay of -1 reads column 0 and a mountain (code 0)
  column 1.

The eight source tables, in file order and with the bit each is selected by:

| # | bit | name | what it is |
|---|-----|------|-----------|
| 0 | 0x01 | walk | normal ground movement |
| 1 | 0x04 | water | swimming |
| 2 | 0x02 | fly | flying |
| 3 | 0x08 | forest | forest-walker (ability 3 / 0x9d) |
| 4 | 0x10 | cave | cave-walker |
| 5 | 0x20 | mountain | mountaineer |
| 6 | 0x40 | bit6 | ability 7 / 0xa3 |
| 7 | 0x80 | bit7 | ability 0x2a |

They are the ground truth and are **not hand-authored**:
`tools/gen_move_cost_tables.py` turns a Ghidra memory dump into
`src/include/aow/move_cost_tables.hpp`:

```powershell
python tools/ghidra/mcpq.py /AoWEPACK.dpl /read_memory address=0x558e84fc length=2048 > tmp_tables.json
python tools/gen_move_cost_tables.py tmp_tables.json src/include/aow/move_cost_tables.hpp
```

Spot values (`aow::move_point_cost`, walk table) next to the wiki's terrain table
(ageofwonders.fandom.com/wiki/Terrain, /wiki/Movement) - the code is the field's
raw overlay. The Movement page: *"Moving a grounded unit one hex across standard
terrain will use up 4 movement points... Forests and hills cost 6 movement points
... while roads (including bridges) and structures cost only 3... Flying and
Floating units only require 4 ... except mountains, which requires 8"*, and its
hex table's 28-movement-point row reads **7 standard / 9 road / 4
forest-or-hill** - the values below (locked by `movement_test.cpp`):

| field | code | walk cost | wiki |
|-------|------|-----------|------|
| grass, no overlay | -1 | 4 | Grassland 4 |
| grass + road / city | 4 | 3 | Road 3, City 3 |
| grass + forest (dense vegetation) | 1 | 6 | Forest 6 (4 with Forestry/Flying) |
| grass + hill | 2 | 6 | Hill 6 (4 with Mountaineering/Flying) |
| grass + mountains | 0 | 0xFF | Mountains Impassable |
| grass + mountains, mountaineer | 0 | 8 | ...or 8 with Mountaineering |
| grass + rubble | 6 | 6 | (4 with Cave Crawling) |
| water, no overlay | -1 | 0xFF | Water Impassable |
| water + bridge | 5 | 3 | (the only way a walker crosses water) |

Dirt (underground terrain 12) is 4, or 3 with Cave Crawling; lava (terrain 9) is
impassable. Deeper per-column detail (`aow move` prints it, and
`src/tests/movement_test.cpp` locks it):

* walkers: grass 4, road 3, forest 6, hill 6, mountains/water/lava impassable;
* Forest-walker (bit 0x08): forest 4;
* Mountaineer (bit 0x20): mountains 8, hill 4;
* Flying (bit 0x02): 4 nearly everywhere, mountains 8, and water 4;
* swimmers (bit 0x04) move only on water (4).

`aow::build_move_point_tables()` reproduces the 256 tables at run time from the
header (the full 64 KB array is not duplicated in the source tree).

## 3. Move types: the ability bitmap

`TAbilityOwner.GetAbMoveTypesAll@0x5574f764` folds a unit's abilities into the
move-type byte:

```
bit0 = ability 0                         bit4 = ability 4 | 0x9d
bit1 = ability 1 | 0xa5 | 0x3b           bit5 = ability 5 | 0x9d
bit2 = ability 2 | 0x17 | 0xa4 | 0xa6    bit6 = ability 7 | 0xa3
bit3 = ability 3 | 0x9d                  bit7 = ability 0x2a
```

`Ability.pfs` record order **is** the ability id space: record 0 = "walk",
1 = "fly", 2 = "travel across water", 3 = "less restricted movement through
forests", 4 = "... caverns", 5 = "travel over mountains" (verified with
`aow abilities Release/Ability.pfs`).

The bitmap itself is a `TCustomAbilityList`, serialised by
`TCustomAbilityList.ReadWrite@0x5574e2bc` as

* rw **tag 2** = a `u16` **bit count**, then
* rw **tag 3** = `ceil(count/8)` bitmap bytes, **LSB-first**
  (`GetAbSet@0x5574e0e0`: `mask[i>>3] >> (i&7) & 1`; `SetAbSet@0x5574e0bc` grows
  the count when a higher bit is set, which is why the stored count is always
  `highest set bit + 1`).

Two places carry it:

* **a placed map unit** - `TAbstractUnit.ReadWrite@0x557820e4` calls
  `TAbilityOwner.ReadWrite@0x5574f318` first, so every `AoWE.TUnit` /
  `THero` / `TLeader` object on the map has rw tags 2 and 3. `parse_army` reads
  them into `aow::MapUnit::ability_count` / `ability_mask`.
* **a unit *type*** - the `Unitres.pfs` record's rw **tag 25** is the same
  `TCustomAbilityList` as a nested `[table][data]` EObject
  (`UnitEntry::ability_ref`). `aow::decode_ability_list` parses it; the Human
  Archer's 12-byte payload `02 02 00 03 04 17 00 00 00 01 00 40` decodes to
  count 23 and mask `{0x01,0x00,0x40}` = abilities 0 (walk) and 22.

Verified values: the Human Archer is `walk`; the Halfling Eagle Rider's payload
has bit 1 set = `fly`; Tutorial's hero (Herores resource 41) has 53 abilities
and is `walk`.

## 4. Per-unit and per-army movement

* `TAbstractUnit.CreateMovePointTable@0x5577fdc4` copies
  `MovePointTables[MoveTypesToInt(move_types)]` (`MoveTypesToInt@0x5575aec0` is
  the identity) and then applies the per-player ability tweaks, in order:
  `(TPlayer+0x54) & 2` forces the road column to `2`; abilities `0x17|1|0xa5`
  with `TPlayer` flags `+0x54 & 4` / `+0x55 & 4` either double every cost
  (`!a && b`) or halve every cost >2 (`a && !b`); ability `0x98` drops every cost
  >2 by one (floor 2); ability `0x84` adds 2 to every positive cost. The whole
  block is gated on the unit's player index being `> 0`. The port implements all
  of it (`aow::unit_move_point_table(tables, move_types, has, traits)` +
  `aow::PlayerMoveTraits`); only `AoWHSMap+0x120` (which clears move-type bit
  `0x80`) is left out - no exported routine writes it (§9).
* `AoWE.MaximizeMovePointTable@0x55744d34` folds two tables:
  `if (dst <> 0xFF) and ((dst < src) or (src = 0xFF)) then dst := src` - i.e.
  `dst = max(dst, src)` with `0xFF` propagating, so the **slowest unit wins and
  any blocked unit blocks the whole army**.
* `TArmy.CreateMovePointTable@0x5578dc40` seeds every cell with `1`, folds each
  selected unit, and leaves an empty selection all-`0xFF`.
* `TArmy.MoveTypes@0x5578d9e8` is the **union** of the selected units' move
  types.
* `TArmy.MovePoints@0x5578d8d0` is `min(0x32, min unit move points)`, and `0`
  for an empty selection.
* A unit's **move points** are `TUnit.GetMovePoints@0x5578276c` = `+0x3d`, which
  `TAbstractUnit.NewTurn@0x55780d4c` refills each turn from `GetMoves` (`+0x2d`)
  - the `Unitres.pfs` record's `moves` field (tag 18) for a plain unit, or the
  `Herores.pfs` record's `moves` (tag 19) for a hero. The port models a fresh
  turn, so `aow::unit_move_points` returns that allowance.

## 5. The pathfinder

`TMoveControl.CalculatePath@0x557463f8` activates the move control and calls
`CalculatePathEx@0x557460b0`:

1. it floods the accumulated move-point grid out from the source. `SetupMove`
   (`0x55745ae8`) resets the grid to `0xFFFFFF`, writes `0` on the source
   (`TMoveControlSettings.SetMovePoints@0x55745318`) and builds the direction
   offset tables; then `SetupSubMove@0x55745d64` creates a `TSubMove` on the
   source hex and fans out six straight `CalculateMovePointLine@0x557455b4` rays
   (the two 3-entry direction tables at `AoWEPACK 0x558e8d54` / `0x558e8d68`,
   selected by the source->destination facing). `Process@0x55745fbc` ->
   `ProcessSubMove@0x55745ebc` -> `ProcessHexagon@0x557456f8` then recurse over
   the rays' endpoints, so the whole reachable grid is filled by straight-line
   relaxations. A step's cost is the **destination hex's** `MovePointCost`
   (bounded by `TMoveControl+0x2c`, the estimated cost to the target);
2. it then walks the **gradient** back from the destination to the source,
   picking the **first** neighbour (directions 1..6 in order) whose accumulated
   cost is `current - step_cost` (where `step_cost` is the *current* hex's own
   `MovePointCost`), and stores the per-step cost in the path entry;
3. if the destination is unreachable the path is **CLEARED** (no gradient step
   found at all also clears it).

**The path list is stored destination-first** - the descent adds the destination
first and the source last.

A bounded cheapest-path search computes exactly the same accumulated costs, so
the port uses a bounded Dijkstra (`aow::find_move_path`) to fill the grid and
then reconstructs the path with the **same** direction-ordered gradient descent
(directions 1..6, first match) - so equal-cost ties resolve exactly as the
engine's. It returns the path **source-first** with each step's cumulative cost,
`total_cost` and `reached`. `bound` is the engine's `TMoveControl+0x2c`; the
viewer passes `0xFF` so a path beyond this turn is still computed (that is what
the dim arrows show).

The six hex neighbours are `HSEngine.HNtoHX@5560e08c` +
`HSEngine.CenterHNtoHX@5560e17c` (an odd column folds one row for an odd `dx`),
so on the `HXtoHP` grid (`px = x*32+8`, `py = y*32 + (x&1)*16`) they are

```
even x: (0,-1) (1,-1) (1,0) (0,1) (-1,0) (-1,-1)
odd  x: (0,-1) (1,0)  (1,1) (0,1) (-1,1) (-1,0)
```

`aow::hex_direction` is `HSEngine.dHXtoHNfast@5560e338` verbatim (1..6, 0 for
the same hex) and `aow::hex_distance` is `HSEngine.dHXtoRad@5560dcf4`.

## 6. The overlay sprites - the game's own arrows and X

`AoWE.TMoveSelector.ShowMovePaths@0x55763f80` draws the trail. `lib` is
`*(AoWHSSet + 0x4c)`, which `AoWE.TAoWWHSSet.Create@0x5574b9ec` sets to
`TImageLibraryManager.LinkToIL(manager, 1, "CURSORS.ILB")` - so the arrows come
from **`Images\Cursors.ilb`**, indexed by the ILB *image-slot id*
(`TCustomImageLibrary.Get@0x5521a8b8`).

For every step from `list[i]` to `list[i+1]` it blits, at
`HXtoHP(list[i]) - camera + (-8,-8)` (plus the sprite's own `base+shift`):

```
dir   = dHXtoHNfast(list[i], list[i+1])
frame = dir + 0x22   while `budget >= mp(list[i])`   (bright, reachable this turn)
frame = dir + 0x29   otherwise                        (dim, beyond this turn)
```

Because the engine's list is **destination-first** and it draws the arrow at
`list[i]` (the hex *closer to the destination*), the trail lands on **every hex
except the source** - the destination hex included (the X is then drawn on top of
it). The port's `MovePath` is source-first, so the hex to draw for the step
`steps[i-1] -> steps[i]` is `steps[i]`: the loop starts at index 1 (the hero's own
hex gets no arrow) and ends at the destination.

where `mp(list[i])` is the accumulated move points **from the source** to
`list[i]`. `ShowMovePaths` computes it as a running sum: `local_34` starts at the
path total and, each iteration, subtracts the entry's stored cost - and because
an entry stores `mp(list[i]) - mp(list[i+1])`, the running value is exactly
`mp(list[i])`. So the **near half of the trail (from the army) is bright and the
far half (toward the destination) is dim** - the opposite of the first port
implementation, which used `total - mp(list[i])` and drew it inverted.

and the destination hex gets an **X**: frame `0x29` (bright) when the whole path
fits this turn, else frame `0x30` (dim) plus a "N turns" label
(`((total - budget) - 1) / per_turn + 1`).

When the path does **not** fit this turn, `ShowMovePaths` (its trailing
`TMovepath.MovePointsToPos@0x557638d8` block) draws a **second, bright X** on the
hex where this turn's move points run out - `pos = MovePointsToPos(path, budget)`
while `0 < pos < count-1`. The port reproduces it
(`aow::path_budget_stop_index` + `draw_move_path_overlay`): the bright X marks the
end of this turn, the dim X the eventual destination.

Because the engine's list is **destination-first**, its `dir` is the *reverse*
step; the port stores the path source-first, so
`aow::path_arrow_dir(path, i)` = `hex_direction(steps[i+1], steps[i])`.

**Verified against the real game.** `bin/MovementRef.png` is an in-game capture
whose path steps are spaced exactly `(32,-16)` px - i.e. NE, `dir 2` - and the
chevron sprite there matches `Cursors.ilb` slot **0x27 (39)** pixel-for-pixel
(IoU 0.976 against the captured sprite; `dir + 0x22` would be 0x24). The port
therefore draws `0x27` for those steps, which is exactly what the formula above
produces.

Frames 34..48 of `Images\Cursors.ilb` are the 49x49 `Pointers.BMP` set; 35..40
are the six bright arrows, 42..47 the six dim ones, **41** the bright X and
**48** the dim X (34 is an unused duplicate of 38).

`aow::draw_move_path_overlay` blits them straight into an RGBA or BGRA canvas;
`bin/MovementProof_reachable.png` and `bin/MovementProof_beyondturn.png` are
head-less proofs over `Tutorial.hsm` (§7).

## 7. Evidence

* **Move tables.** `python tools/ghidra/mcpq.py /AoWEPACK.dpl /read_memory
  address=0x558e84fc length=2048` and `FUN_55744b2c`'s decompile agree; the
  extracted values match the walk/road/forest/hill/mountain spot values above
  and the `move_types` bit order 0x01,0x04,0x02,0x08,0x10,0x20,0x40,0x80.
  (Re-running `tools/gen_move_cost_tables.py` over a fresh Ghidra dump
  reproduces `move_cost_tables.hpp` byte for byte.)
* **`column = terrain*16 + overlay + 1`.** `TAbstractUnit.MovePointCost@0x55780848`
  reads `acStack_106[param_3 + param_2*0x10]` but its table lives at `local_107`
  (one byte earlier in the frame), and `DefaultMoveOnMovePointProc@0x557454d8`
  reads `mc + terrain*0x10 + 0x31 + overlay` with the table at `mc + 0x30`. The
  codes and the columns they select (all from `Release.hss`):

  | raw code | objects | column | walk cost | resolves to |
  |----------|---------|--------|-----------|-------------|
  | -1 (`0xFE`/`0xFF` byte) | base hexagons, rocks, effects | 0 | 4 | plains |
  | 0 | `MOUNTAIN\MOUNTAIN.ILB`, `WL_VOLCL.ILB`, `STR_ELVC.ILB` | 1 | 0xFF (8 mountaineer) | mountains |
  | 1 | `gr_tree.BMP`, `sn_tree.BMP`, `GND_HOLY.ILB`, ... | 2 | 6 (4 forestry) | forest |
  | 2 | `MOUNTAIN\MOUNTAIN.ILB` (raised), `WL_VOLCS.ILB` | 3 | 6 (4 mountaineer) | hill |
  | 3 | `gr_willow.BMP`, `gr_toad.BMP`, `*_dtree.BMP` | 4 | 4 | scrub |
  | 4 | `Rd_gr/de/sn/st/Wl.BMP` (roads), `CITY1..4.ILB` | 5 | 3 | road / city |
  | 5 | `Wa_bdge.BMP`, `Uic_bdge.BMP`, `Str_brdg.bmp`, ... | 6 | 3 | bridge (the only water crossing) |
  | 6 | `Ug_Rubble.BMP` | 7 | 6 (4 cave crawling) | rubble |
  | 8 | `wl_bones.BMP` | 9 | 0xFF (4 fly) | bones |
  | 9 | `gr_flwer.BMP`, `*_lveg.BMP` | 10 | 4 | vegetation |

  The same shift explains the engine's own ability texts: ability 3 ("less
  restricted movement through forests") is exactly the 6 -> 4 the forestry table
  gives column 2, and ability 5 ("travel over mountains") the 0xFF -> 8 of column
  1 - with the codes used raw, *forests* would be impassable and *mountains* cost
  4.
  The per-column costs behind that table are printed by
  `python tmp_gr/_analyze_tables.py 1` (which ability improves which column).
* **Overlay source.** `aow move Scenario/Custom/Tutorial.hsm --root . --level 0`
  prints the histograms: `terrain: 0=692 1=547 3=1 4=47 5=9 15=1008`,
  `overlay: none=1759 0=313 1=105 2=65 3=18 4=43 9=1` and
  `base-hex overlay: none=2304` - i.e. **every** tag-50 hexagon declines the
  overlay (its byte is `0xFE` = -1), and the codes come from the objects:
  `gr_tree.BMP` -> 1 (forest, 6 - walkable), `MOUNTAIN\MOUNTAIN.ILB` -> 0
  (mountains, impassable; 313 tiles once the whole `Mountain.TMountainMO`
  footprint is stamped - see §1), `Rd_gr.BMP` (a road) -> 4 (3),
  `H_st.BMP` + `MOUNTAIN\MOUNTAIN.ILB` -> 0 on stone. The same count for the
  *footprint-only* rule was 152, and for the anchor-only rule 39: each fix in
  §1 raised it. A standalone parse of
  `Tutorial.hsm`'s fields (`tmp_gr/_mapfield_dump.py`) reproduces the histogram
  tile for tile, and the hero army's line reads
  `grass=4 forest=6 hill=6 mountain=255 road=3 water=255`.
* **Ability bitmap.** The Human Archer's `Unitres.pfs` tag-25 payload decodes to
  count 23 / bits {0, 22} and to `walk`; the Eagle Rider's has bit 1 (`fly`).
  `GetAbSet@0x5574e0e0` pins the LSB-first bit order.
* **Army budget.** Tutorial's hero army at (15,34): hero (Herores 41, 32 mp,
  walk) + 3 units (28/36/32 mp) -> move types `walk|forest`, budget 28.
* **Overlay sprites.** `bin/MovementRef.png` chevron IoU 0.976 against
  `Cursors.ilb` slot 39; the port's head-less render of the same NE-going path
  reproduces it (`bin/MovementProof_*.png`).
* **The fix end to end.** `aow_view --move 19,12` walks the Tutorial army from
  (15,34) into the grass forest at (19,12) - 28 hexes, cost 98, the last steps 6
  each (`bin/ForestMoveProof.png`) - while a mountain tile (`--move 25,10`)
  prints `unreachable`. Before the fix the forest was unreachable and the
  mountain cost 4.

## 8. The C++ reconstruction

`src/include/aow/movement.hpp` + `src/engine/movement.cpp`:

* `build_move_point_tables()`, `move_types_from_abilities`,
  `unit_move_point_table`, `maximize_move_point_table`, `army_move_point_table`,
  `army_move_points`, `army_move`, `move_point_cost`, `clamp_move_points`;
* `decode_ability_bitmap`, `decode_ability_list`, `unit_move_types`,
  `unit_move_points`;
* `hex_direction`, `hex_distance`, `hex_neighbour`, `hn_walk_hex`;
* `fill_tile_terrain` (the per-tile resolution + the multi-hex footprint stamp);
* `find_move_path` (a bounded Dijkstra) + `MovePath` / `MoveStep`;
* `path_arrow_dir`, `path_arrow_frame`, `path_target_frame`, `path_turns`,
  `path_budget_stop_index`.

`src/engine/mapviz.cpp` adds `hex_origin`, `draw_move_path_overlay`,
`blit_sprite_bgra` and `army_sprite_rgba`, and `MapVizOptions::hide_army_*` (the
walk animation hides the army on the hex it is moving onto).

`src/game/viewer.cpp`:

* opening a map loads the roster (`aow::load_game_state`), the unit catalogue,
  the per-level terrain and the move tables, **auto-selects the human player's
  first army and centres the view on it**;
* **hover** picks the hex under the cursor and draws the trail with the real
  `Cursors.ilb` arrows / X (composited over a pristine copy of the render, with
  a dirty-rect restore so a big map stays responsive);
* **left-click** on one of your armies selects it (the title bar shows its hex,
  its move points and the hovered path's cost / turn count); a left-click on a
  reachable hex **relocates the `AoWE.TArmyHS` between the field tables**, sets
  its `army_direction` (TUnitHS rw tag 9) and walks the army there with a
  per-hex sprite animation;
* **N** cycles your armies, **Escape** deselects, and dragging still pans.

`src/game/menu.cpp` (the port's `AoW.exe`, the in-game screen):

* `Scene::load_game_map` keeps the parsed `.HSM` (`game_state_map`) and the
  `Release.hss` resources instead of dropping them after the render, then runs
  `Scene::mv_load` - the startup `MovePointTables`, `aow::load_game_state` (the
  roster), `aow::load_unit_catalog` (the units' move points) and
  `aow::fill_tile_terrain` for the shown level - and records the
  `PlayerType::Human` player (`mv_human`);
* a **left-click** on the world map (`Scene::game_map_click`, the engine's
  `TAoWHSMap.Select`) selects the human army under the cursor: `Scene::mv_select`
  folds the army's units with `aow::army_move` (on `Tutorial.hsm` the hero army
  at (15,34) yields walk|forest and a 28 move-point budget);
* **moving the pointer** retargets the trail at once
  (`Scene::game_resolve_hover` + `game_update_path`) and `mv_refresh_draw`
  composites the game's own `Cursors.ilb` arrows / X over a copy of the level
  render (`game_draw`) that the map view blits; the source-first `MovePath` is
  the same `aow::find_move_path` the viewer uses;
* a **click on another hex** walks the selected army there when this turn's
  budget covers the path (`Scene::game_move_selected` relocates the
  `AoWE.TArmyHS` between the level's field tables, writes its `army_direction`
  and re-renders the level from the in-memory map); the army stays selected on
  its new hex, and a beyond-this-turn path shows the dim arrows / dim X instead;
  an unreachable hex reports `destination unreachable` and keeps the selection,
  and clicking the selected army again clears it; a **right-click** on the map
  also clears the selection (`Scene::game_right_click`, the window procedure's
  `WM_RBUTTONDOWN`), matching the engine removing the party floater on a
  right-click of the main map window (which deselects the party);
* the scanner's **Surface / Caverns** tabs now re-render the other level *from the
  in-memory map* (`Scene::game_set_level`), so an army moved this session stays
  moved (the old path reloaded the `.HSM` from disk);
* the map's camera keys (`Scene::game_key`, the engine's `TAoWHSMapKeyboard`):
  `C` centres on the selection, and `L` - unwired until now - centres on the human
  player's army whose lead unit is the hero and selects it
  (`Scene::game_center_leader`). A game that has *just started* also opens centred
  on that leader - `Scene::start_game` arms `game_center_on_start`, which the first
  `game_update` applies once the map window's viewport size is known - but the
  leader is left **unselected** (no default selection when a game starts).

### Interactive cost (and the caches that keep it low)

The first version of this was unusable: **~343 ms per hover step** and **~286 ms
per move**. Both were per-call rebuilds, not blitting:

* `draw_move_path_overlay` re-built the game-root ILB index (a recursive walk of
  the install, tens of thousands of files) and re-decoded `Cursors.ilb` on every
  call;
* `render_map_rgba` did the same for the whole level - and the in-game screen
  re-renders after *every* move.

So `src/engine/mapviz.cpp` now keeps the ILB index / resolver (`shared_resolver`)
and the decoded libraries (`shared_libraries`) in process-wide caches, exactly as
the engine's `TILManagerList` keeps its libraries loaded, and the in-game screen
composites the trail with a **dirty rectangle** (`Scene::mv_mark_hex` /
`mv_restore_dirty`) instead of recopying the megabyte-scale level render.

`AoW.exe --game-bench N` measures the loop (head-less; see docs/09_GAME_SCREEN.md):

| step | before | after |
|------|--------|-------|
| find the hover path | - | **0.01 ms** |
| composite the trail (restore + `Cursors.ilb` arrows/X) | ~343 ms | **0.25 ms** |
| the *first* hover of a session (decoding `Cursors.ilb`) | ~50 ms | **0.03 ms** |
| a move: re-render the level | ~286 ms | **27 ms** |
| one screen frame (30 fps, the map + bar + floaters) | - | **1.2 ms** |

`aow::prepare_move_overlay` warms `Cursors.ilb` when a level loads (the engine
links it at start-up too), which is what removes the first-hover cost. The
remaining 27 ms per move is the level's own hex/object blitting into the
1560x1552 surface; the engine instead repaints its cached surface. The build's
`game performance smoke test` pins the two regressions that were found here.

### Overlay size (documented deviation)

The game's world map is always drawn 1:1 (a hex is 48x32 px, an arrow 19x14),
but this viewer *scales the whole rendered canvas* to fit the window. When it
does, the 19x14 arrow sprites were being shrunk to 4-5 px - unreadable specks
scattered over the map - so:

* `aow::draw_move_path_overlay` takes a `sprite_scale` that grows every overlay
  sprite by whole pixels around its hex, and the viewer passes
  `round(1 / zoom)` (clamped to 1..8) so the arrows keep their **1:1 on-screen
  size** no matter how far the map is zoomed out;
* `zoom_initial` now floors the opening zoom at **50%** instead of fitting a
  large map down to ~23%, so the map (and the arrows on it) is readable on
  open. `F` still fits the whole map on demand, and the wheel still zooms.

This is the one place the viewer deliberately departs from a pure 1:1 blit; the
sprite *art*, the *frames*, the *anchor* (`HXtoHP - 8 + base+shift`) and the
bright/dim rule are all the engine's.

Head-less proof (`--move X,Y [--army X,Y]`):

```powershell
aow_view.exe "Scenario\Custom\Tutorial.hsm" --root . --render C:\tmp\out.png --move 17,32
```

The in-game screen (`AoW.exe`) proves the same interaction head-lessly:
`--game-click X Y` may be repeated, and each click is applied in order (the
scanner floater first when it covers the point), so the build's smoke test drives
a whole selection and walk - `--game-click 513 141` (the scanner, which centres
the view on the human hero army at (15,34)) then `--game-click 220 216` (select)
then `--game-click 284 152` (walk to (17,32)) - and asserts the printed
`game_move=15,34->17,32` / `game_army=17,32 mp=28`. `--mouse X Y` parks the
pointer like a hover first, so the trail alone can be rendered.

Demo CLI:

```
aow move Scenario/Custom/Tutorial.hsm --root . --level 0 --to 20,28
```

## 9. Out of scope / `TODO(RE)`

* The per-player ability tweaks in `TAbstractUnit.CreateMovePointTable` are now
  **implemented** (`aow::unit_move_point_table(tables, move_types, has, traits)` +
  `aow::PlayerMoveTraits`), but the port does not model `TPlayer`, so the viewer
  always passes default traits (which equals the common, unmodified case). The one
  remaining piece is `AoWHSMap+0x120`, which clears move-type bit `0x80`: **no
  exported routine writes that field** (only `5577fdc4`/`55763f80` read it), so its
  meaning is not recovered and the port leaves it 0.
* A multi-hex object's **per-hex** `TTerrainList` entries are now **stamped**
  (`aow::hn_walk_hex` + the second pass of `fill_tile_terrain`), so a mountain
  marks every hex of its footprint and not only the one the object is stored on,
  and a `Mountain.TMountainMO` (0x20168) marks **all** of them as mountains - its
  `GetOverlay` override ignores the covered-hex index (§1). Two approximations
  remain. (a) The engine resolves a covered field's overlay from its sprites
  top-down by *show priority* (`UpdateOverlayType@55607e48`), which the port
  cannot compare, so where two objects claim one hex -
  `Mountain.TMountainMO.ValidMountainOverlap@556a2de0` allows an overlap only when
  their opinions differ - the port lets the impassable claim win (a hex a mountain
  covers stays a mountain) and applies a `Terrain.TTerrainMO` feature only to a
  hex nothing impassable has answered for yet; a road/bridge is never replaced
  (the road is drawn over the mountain - the pass the maps rely on). (b) Only the
  *overlay* is stamped, never the terrain, because several of these lists carry a
  garbage terrain (a city resource's entry is `0 | 4<<8` = water | city) and the
  port's rule is that the terrain comes from the tag-50 hexagon only. A road's
  per-neighbour transition overlays are still the separate `mapviz` pass; the road
  *discount* itself is resolved: a road's raw code is 4, which is cost-table
  column 5 = 3 (§2).
* **Move-point spending between turns**, transporters / sub-moves
  (`TArmy.Transporter`, `TSubMove`) and the turn loop's refill are out of scope:
  the port always models a fresh turn's allowance. `TArmy.MovePoints`/
  `MoveTypes`/`CreateMovePointTable` all take a **transporter** branch when the
  army is aboard one (`TArmy.Transporter@0x5578e00c`); the port has no such model.
  Likewise `TMoveControl.FindSubMoveOrigin`/`SetupSubMove` enrich the flood with
  multi-hex sub-moves, which the port's Dijkstra reproduces as plain hex steps.
* `TUnitHS` rw tag 9 (`army_direction`) is written with the step's hex direction
  (1..6). The facing's image-sequence semantics **are** recovered in
  `docs/16_UNIT_ANIM.md`: the direction selects the unit's standing pose
  (`seq[dir-1]`) and its walking cycle (`seq[dir+5]`), and the walk plays the
  unit's own sprites while the sprite interpolates between the two hex origins.
* The "N turns" text next to a beyond-turn X is not drawn (the engine layer has
  no font); the turn count is shown in the viewer's title bar and by
  `aow move`.
