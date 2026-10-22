# 23 - Tactical battle maps (`TCMaps\*.HSM`) and AoWTBViewer

`TCMaps\` holds 59 pre-built tactical-combat battlefields (65x59 hexes with a
fixed ocean border). They are ordinary `.HSM` containers (`CFS\0` + zlib + the
Engine EObject graph), but their level/field classes live in **`AoWTCPCK.dpl`**
(the tactical-combat package), not in `AoWEPACK.dpl`:

| class | id | role |
|-------|----|------|
| `AoWTC.TAoWCombatMap` | `0x220100` | top object |
| `AoWTC.TAoWCombatMapLevel` | `0x220102` | the single level |
| `AoWTC.TAoWCombatMapField` | `0x220101` | one hex field |
| `CombatHex.TCombatHexagon` | `0x220500` | flat base terrain hex |
| `CombatHex.TWaterCombatHexagon` | `0x220597` | water base hex |
| `SingleCombatHex.TSingleCombatHexagon` | `0x22051e` | dungeon/exploration floor hex |
| `IsoCombatHex.TIsometricCombatHexagon` | `0x22050a` | elevated/stacked hex |
| `CombatRoad.TCombatRoad` | `0x220502` | road overlay |
| `CombatILTer.TCombatILTerrain` | `0x220136` | trees/rocks/crops |
| `CombatTerrain.TCombatTerrain` | `0x220166` | combat terrain object |
| `CombatObstacle.TCombatObstacle` | `0x22052a` | obstacle |
| `House.THouse` / `CityWall.TCityWall` | `0x220512` / `0x220504` | houses/walls |
| `MainStructure.TMainStructure` | `0x22051c` | city main structure |
| `FleeLoc.TFleeLocation` | `0x220528` | flee marker |
| `AoWTC.TUnitPositionHS` | `0x220103` | deployment marker (not drawn) |
| `BorderHx.TBorderHexagon` | `0x20557` | ocean frame around the field |

`TAoWCombatMapLevel` extends `TAoWMapLevel`, and `TAoWCombatMapField` extends
`TAoWMapField`, so the field serialisation is the same as a scenario map:
**tag 50 = terrain hexagon, tags 51+ = map objects, tag 4 = the 16-bit field
word**. The registry just registers different class ids.

## Resources - `Release\comb_res.pfs`

Every object's rw **tag 1** is a resource id into `comb_res.pfs` (581 records,
the `load_combat_resources()` table). The records use the `TMapObjectResource`
layout:

| tag | meaning |
|-----|---------|
| 1 | resource id (the id stored in a map field) |
| 2 | TEResource type |
| 3 | TEResource index |
| 7 | `TTerrainList`: `u32 count`, then `u16 terrain | overlay << 8` |
| 8 | inline 32x32 cell image (`ILB`) |
| 9/10 | `TImageSequenceList` - the `Images\TCombat\...` library, frame and xoff/yoff |

The first record is the default grass hex (id 0); `H_GR`, `H_SN`, `H_WL`,
`H_LA`, `H_UGIC`, `H_UGWA` ... are the other base-hex resources.

## Drawing (recovered from AoWTCPCK)

* `CombatHex.TCombatHexagon.Show` (0x403cac): blit the base sequence's frame at
  `HXtoHP - 8`, then up to three edge transitions at `HXtoHP - 8, -16` from
  `Images\TCombat\Base_hex\CTransHx.ILB`. `Activate` picks the base frame with
  `RandInt(12)`; `UpdateTransitions` fills the three direction slots.
* `TCombatHexagon.UpdateTransition` (0x403958) maps the NW/N/NE neighbours
  (`DAT_0046701f[1..3] = 6,1,2`) onto a transition type, then
  `frame = type * 6 + RandInt(2) * 3 + (dir - 1)`. CTransHx holds six frames per
  terrain family (grass 0..5, desert 6..11, snow 12..17, steppe 18..23,
  wasteland 24..29, water 30..).
* `SingleCombatHex.TSingleCombatHexagon.Show` (0x4350f8): base sequence frame
  `seq+0x20` at `HXtoHP - 8` (no transitions).
* `TCombatRoad.Show` (0x4111dc): base frame `10 + RandInt(12)` at `HXtoHP - 8`,
  plus six road pieces at `HXtoHP - 8, -16`; `UpdateTransition` maps the six HN
  directions to frames `1,2,6,7,8,0` when the neighbour's overlay is 4.
* `TMapObject.Show` / `TMapObjectResource.ShowComplexImage`: all other combat
  objects anchor their sequence at `HXtoHP - 8 + seq.xoff/yoff`, adding the
  sprite's own `base+shift`.
* `CityWall.TCityWall.Show` (0x4062fc), `House.THouse.Show` and
  `ILTer.TILTerrainMO.Show` are the exception: they anchor at **`HXtoHP`**
  (no `-8`) and walk the resource's **sequence chain** — for a city wall the
  first sequence is the `CGR_WPAD` pad, chained to a `C_SWALL*`/`C_WWALL*` wall
  side (the base frame selects the side/race) and the race flag effect. The
  wall object's `+0x24` is its damage state (0 intact .. 3 destroyed). The
  viewer draws every chain layer with `image_sequence_unlooped_frame(seq, 0)`,
  exactly the frame a static wall uses. (Drawing only the body at `HXtoHP-8`
  shifts the wall and drops the actual wall side — the bug this fix removes.)

`aow::render_tb_map_rgba` / `render_tb_map_png` implement those rules. The
engine's `RandInt` choices are replaced by deterministic positional hashes so a
static render is stable; the geometry, anchors, libraries and transition types
are the decompiled ones. The H_Border.BMP border is not shipped, so the frame
draws `WATERHEX.ILB`'s `H_Wa2` ocean base (the same art the border abuts).

## AoWTBViewer

`port/build.ps1` builds `bin\AoWTBViewer.exe` (installed to the game root by
`build.ps1 -Viewer`). It scans `TCMaps\*.HSM`, lists them on the left, renders
the selected battlefield to an RGBA DIB and lets the camera pan/zoom.

```powershell
# GUI
.\AoWTBViewer.exe

# list every tactical map
.\AoWTBViewer.exe --list --root "C:\GOG Games\Age of Wonders"

# head-less 1:1 render
.\AoWTBViewer.exe "..\TCMaps\BrIcA000.hsm" --root ".." --render out.png
```

Controls: arrows/WASD or drag to pan, wheel or +/- to zoom, `F` fit, `0` 100%.
The tool CLI `aow tbmapviz <map> <out.png>` is the same renderer.
