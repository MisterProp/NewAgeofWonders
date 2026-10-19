# 11 - Loading every unit of the game

`src/include/aow/unit_catalog.hpp` + `src/engine/unit_catalog.cpp` turn the
shipped game data into one complete roster of units. `aow roster <game-dir>`
prints it; `aow_unit_catalog_tests` checks it.

## Where a unit comes from

| source | provides |
|--------|----------|
| `Release/Unitres.pfs` | one `AoWE.TUnitResource` (class `0x20212`) per unit type: name, qualifier, race, the 7 stats, level, obtain value, description, and `gfx_index` |
| `Release/Unitgfx.pfs` | one `AoWE.TUnitGFXResource` (class `0x20210`) per sprite: name, image library (`FACES\R_FACES.ILB`, `FACES\UNITS.ILB`), face index and the `tag 5` animation |
| `Release/Release.hss` | the game pack (`TAoWWHSSet`): the race/faction resources whose `tag 8` image library is `RACES\R*.ILB` |
| `Release/*.ahl` | the small "leader / special" unit lists (`High Level.ahl`, `Special.ahl`) |

The per-record tag maps are in `docs/01_FORMATS.md` (Unitres / Unitgfx) and the
in-memory class in `docs/10_UNIT.md`.

## The unit <-> sprite join

```
UnitResource.gfx_index (tag 8)  ==  UnitGFXResource.res_index (tag 3)
```

`TUnit.GetFace` (`0x5578283c`) resolves the sprite with
`TUnitGFXResourceList.GetUnitGFXResource(list, resource+0x18)`. That getter
(`0x55783cf8`) returns `children[index]` of a `TEResourceList` **indexed by the
record's own TEResource index** - i.e. `res_index`. `res_index` is *sparse*
(Unitres ids jump `8 -> 18 -> 36 ..`, and so do the Unitgfx `res_index` values:
`0..9, 12..14, 18..27, 30..32, ...`), so a plain positional lookup is wrong.
Sweeping the data confirms the id match: unit id 18 `Azrac Archer`
(`gfx_index` 18) -> Unitgfx `res_index` 18 = `Azrac Archer`, and likewise for
every race. `aow::UnitCatalog::find_gfx` implements this.

The unit's **body library** is `RACES\R<race>.ILB` from `Release.hss`; the
sprite's `image_lib` is the *face/portrait* library.

## Race order (data-driven)

`Unitres.pfs` `race_id` is the position in the game pack's `TRaceResourceList`
(not alphabetical). Verified across the roster:

| id | race | body library | id | race | body library |
|----|------|--------------|----|------|--------------|
| 0 | Human | `RHUMAN.ILB` | 6 | Dwarf | `RDWARF.ILB` |
| 1 | Azrac | `RAZRAC.ILB` | 7 | Highman | `RHIGHMAN.ILB` |
| 2 | Lizardman | `RLIZARDS.ILB` | 8 | Dark Elf | `RDARKELF.ILB` |
| 3 | Frostling | `RFROSTL.ILB` | 9 | Orc | `RORC.ILB` |
| 4 | Elf | `RELF.ILB` | 10 | Goblin | `RGOBLIN.ILB` |
| 5 | Halfling | `RHALFLNG.ILB` | 11 | Undead | `RUNDEAD.ILB` |
|  |  |  | 255 | Monster | (none) |

(Note: `docs/01_FORMATS.md` lists the *city* race indices `0=Human, 9=Orc,
10=Goblin`; those happen to agree only for 0/9/10.)

## `.ahl` files

`High Level.ahl` / `Special.ahl` are tiny engine records:

```
u32 0x02000001 | u16 tag14 (=20) | u8 1 | u8 len+1 | u8 len | name[len] | NUL
```

e.g. `High Level.ahl` = `01 00 00 02 14 00 01 0b 0a "High Level" 00`. Only the
name is interpreted (the rest is kept in `LeaderList::raw`); the field
semantics beyond the name are still [TODO(RE)].

## Counts (this GOG install)

```
Unitres.pfs   142 unit resources   (docs' older "188" is stale)
Unitgfx.pfs   182 unit graphics     (docs' older "239" is stale)
*.ahl           2 lists
```

All 142 units resolve to a sprite (`gfx_missing == 0`). The counts are
data-version dependent, so the loader never hardcodes them; the test asserts
`>= 100` plus specific named entries (`Human Archer`, `Azrac Archer`).
