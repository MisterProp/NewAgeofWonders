# Crops (`Crops.TCrop`) - the farm fields on the world map

This is the 1:1 recovery of AoW1's crop tiles: the dense yellow/coloured field
around a farm (the grower) and the reason a crop changes its picture with the
terrain under it. The implementation is `aow::crop_image_index` +
`render_map_rgba`'s `kCropClassId` branch in `src/engine/mapviz.cpp`; the proof
is `bin/CropsRef.png` (a crop of the original game), reproduced by
`tools/crops_bench.py`.

## 1. The classes

| Engine class | Class id | Role |
|---|---:|---|
| `Crops.TCrop` | `0x2036a` | the placed crop tile (a `TILTerrainMO`, stored in a field as tag 51+) |
| `Crops.TCropResource` | `0x2036b` | the crop tile art (`Crops.bmp`, one record: id 41 / `0x29`) |
| `Crops.TPlayerCropStructure` | `0x2034e` | base class of a structure that plants/owns crops |
| `Crops.TPlayerCropStructureResource` | `0x2034f` | its resource (income source + surrounding-hex table) |
| `Farm.TFarm` | `0x2036e` | the only shipped subclass; the farm is what places the crop tiles |
| `Farm.TFarmResource` | `0x2036f` | the farm building (`STR_FARM.ILB`, id 42 / `0x2a`) |

Crop objects are **serialized in the scenario** (tag 51, class `0x2036a`,
resource id 41). `TPlayerCropStructure` creates more while the game runs
(`SetCropCount`), but a `.HSM` already carries the fields this document renders.

## 2. What picture a crop shows - `TCrop.GetTerrainTypeImage`

`Crops.TCrop.GetTerrainTypeImage@0x557a5f14` (AoWEPACK.dpl):

```c
if (crop->owner != NULL) {
    if (owner->vtable[0x238/4]() == 0x0b)   // TPlayerCropStructure.GetRace
        terrain += 0x1e;
}
return terrain;
```

* `owner` is the `TPlayerCropStructure` (`Farm.TFarm`) that owns the crop.
  The vtable slot at `+0x238` is `Crops.TPlayerCropStructure.GetRace@0x557a6450`
  (the DATA xref at `0x557a586c` is that slot; `+0x240` is `CropTrampled`, which
  `TCrop.EndTurn` calls through the same table).
* `GetRace` returns `player+0xa5` (the roster race) and `0xff` when the
  structure has no owner (player `0`).
* `0x0b` is **Undead** in the race order (Human 0, Halfling 5, Undead 11 - see
  `docs/11_UNIT_CATALOG.md`). Undead crops use a second art block.

The terrain is the field's terrain byte (`TCrop+0x18`, set by
`TILTerrainMO.Activate` / `TerrainChanged`). `TCrop.UpdateSettings@0x557a5cc8`
stores the resulting image index in `TCrop+0x14` and frees the crop when
`ImageExist(resource.ImageLibrary, index)` is false. `Crops.TCrop.CanPlaceCrop`
restricts planted crops to terrains 1..4 and 12, plus terrain 5 only for an
Undead owner - exactly the ids that exist in the library.

### The `Crops.bmp` image library

`Crops.TCropResource` has no image-sequence list. It carries a complete ILB
inline in rw **tag 8** (the same mechanism as the logical terrain-object
libraries, docs/01_FORMATS.md). Enumerating that ILB:

| image id | name | used for |
|---:|---|---|
| 1 | `Crops.bmp` | grass - yellow flowers |
| 2 | `Crops.bmp` | desert |
| 3 | `Crops.BMP` | snow |
| 4 | `Crops.bmp` | steppe |
| 12 | `U_crops.bmp` | the upper/underground terrain family |
| 31..35 | `Crops.bmp` | the Undead variants of terrains 1..5 |
| 41 | `U_crops.bmp` | the Undead variant of terrain 12 |

Every entry is a class-0x11 RLE sprite, nominal 60x50 (43x36 non-transparent
content for frame 1). **There is no slot 0**: that is why the generic object
path, which always asked for slot 0, never drew a crop. `Library::frames` in
`mapviz.cpp` is indexed by the ILB's image id, so `frames[terrain]` resolves the
right art directly.

## 3. How a crop is drawn - `ILTer.TILTerrainMO.Show`

`Crops.TCrop` does not override `Show`. It is a `TILTerrainMO`, so
`ILTer.TILTerrainMO.Show@0x55618940` runs:

```c
HXtoHP(crop->x, crop->y, &px, &py);
lib = crop->resource->image_library;          // resource+0x20
image = lib->Get(crop->image_index);          // TCrop+0x14
image->Show(px - view_x + resource->dXmainP,  // resource+0x34
            py - view_y + resource->dYmainP,  // resource+0x38
            canvas);
```

`TCropResource` is a `TMultiHexMOR`; its rw tags 12/13 are **(-15,-9)**. The
image's own base+shift (`TSprite16.ShowOpaque` = `param + base + shift`) is
added inside `Show`. That is exactly the port's
`aow::object_sprite_origin(hex_sx, hex_sy, resource, sprite.vx, sprite.vy,
ox, oy)` for a resource with `has_shift`, so the crop needs no special anchor.
There is no animation (no sequence list, static image).

## 4. Owner resolution in the port

The map file does **not** serialize the `TCrop+0x1c` owner pointer. The engine
reconstructs it:

* `TCrop.LinkToCropOwner@0x557a5dac` walks the six neighbouring fields
  (`HSEngine.InitHNWalk(radius=1)` / `WalkToNextHN`) and latches a
  `TPlayerCropStructure` found there.
* `TPlayerCropStructure.Activate` / `LinkToCrops@0x557a686c` then assigns every
  crop in the structure's `CreateSurroundingHNTable` (resource `+0x74`).

`mapviz` already has all placed objects, so it builds the same association
geometrically: for each crop it takes the **nearest `Farm.TFarm` (0x2036e) on
the same level within 3 hexes** and uses that farm's `structure_owner` ->
`map_player_races[owner]`. A crop with no such farm (a decorative crop beside a
city) has no owner, so its race is -1 and it uses the normal terrain art. This
only affects the rare Undead block; the terrain frame is always exact.

## 5. Validation against the original

`bin/CropsRef.png` is a 164x131 crop of the original game's screen around the
crop fields of `Scenario\Custom\Bloody Marauders.hsm` at (16,36). The port's
full level render puts the same rectangle at `(477,1085)` - located by matching
the city/army block, then the whole frame. `tools/crops_bench.py` reproduces it:

```
python tools/crops_bench.py .. .
whole crop field  164x131  mean|d| R=10.71 G=12.77 B=8.61  avg=10.70/255
crop tiles only    45x45   mean|d| R=0.44  G=2.21  B=0.28  avg=0.97/255
```

For scale, the same rectangle rendered with `--no-objects` scores 22.72/255
(terrain + roads only), so the crop layer halves the error. A *pure crop field*
patch (`0,85..45,130`, no city or road) is essentially pixel-identical at
**0.97/255**. The yellow-flower mask covers 7107 pixels in the port vs 6534 in
the reference; their XOR is 1467 pixels, concentrated on 1-pixel sprite edges
and the city, not on a missing field. Both crop field and city align at offset
`(0,0)`; the whole-frame residual comes from the pre-existing city/army anchor
difference (`docs/16_UNIT_ANIM.md` section 10) and a road edge, not from the
crop tiles.

Visual proofs: `bin/CropsBench.png` (port | original | x4 diff) and
`bin/CropsDiff.png`.

## 6. Tests and code map

* `src/engine/mapviz.cpp`
  * `aow::crop_image_index(terrain, owner_race)` - the
    `GetTerrainTypeImage` formula.
  * `render_map_rgba`: the `kCropClassId` branch overrides the generic object's
    slot-0 frame with the terrain (or Undead) id, and logs a `crop,` debug-CSV
    row. A missing frame is skipped exactly like `TCrop.UpdateSettings` freeing
    the object.
  * The farm/owner scan (`CropFarm`, `crop_owner_race`) runs once per render.
* `src/include/aow/map_types.hpp` - `kCropClassId`, `kCropResourceClassId`,
  `kFarmClassId`.
* `src/include/aow/mapviz.hpp` - the `crop_image_index` contract + evidence.
* `src/tests/mapres_test.cpp`:
  * release resource 41 = `Crops.TCropResource`, inline ILB, shift (-15,-9);
  * the sparse ids 1..4/12 and 31..35/41, and the `crop_image_index` cases
    (including the missing Undead id 42);
  * a Tutorial render logs all 7 crop tiles with frame 1 (grass).

## 7. Known limits

* The owner race is derived from the nearest farm. The original serializes no
  pointer; a scenario that deliberately leaves a crop 3+ hexes from every farm
  would be drawn with normal art instead of the Undead art. Nothing shipped
  exercises that case.
* A crop whose current terrain has no image (e.g. water, or Undead terrain 12)
  is skipped. The engine would free the object; the static renderer simply does
  not draw it.
* No runtime crop creation/growth (`SetCropCount`, `RegrowCrops`, trampling) is
  simulated - the map's serialized crop tiles are rendered. The same is true of
  the other world objects.
