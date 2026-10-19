# 10 - The Unit class (AoWE.TUnit)

This document records the reverse-engineering of the runtime unit object in
`AoWEPACK.dpl` and the C++ reconstruction in `src/include/aow/unit.hpp` with
its tests in `src/tests/unit_test.cpp`.

## How it was recovered

* **GhidraMCP** (HTTP bridge on `127.0.0.1:8089`) with `AoWEPACK.dpl` opened:
  `tools/ghidra/decompile.py` batch-decompiles the whole class family into
  `re/exports/aowepack_unit/AoWEPACK_dpl/` (one `.c` per address);
  `/read_memory` dumps the constant tables.
* **Capstone** (`tools/disasm.py`) disassembles the getters to recover the exact
  address of each constant array referenced by name in the decompiles.
* `AoWEPACK.dpl` image base is **0x55700000**; the constant tables live in its
  DATA segment around **0x558e83c4**.

The `.PFS` side of the same classes (the on-disk records) is already covered by
`docs/01_FORMATS.md` and `src/include/aow/pfs_types.hpp`; this document is about
the **in-memory** objects.

## Class hierarchy

| Delphi class | Class id | Key method |
|--------------|----------|------------|
| `AoWE.TCustomAbilityList` | - | ability bit array |
| `AoWE.TAbilityOwner` | - | owns `TAbility` instances |
| `AoWE.TAbstractUnit` | - | `Create` @ `0x5577eb28` |
| `AoWE.TUnit` | `0x20213` | `ClassID` @ `0x557826ac` |
| `AoWE.TUnitResource` | `0x20212` | `ClassID` @ `0x55784da0` |
| `AoWE.TUnitGFXResource` | `0x20210` | `ClassID` @ `0x55783974` |

Delphi objects are `[vtable][fields...]`; all offsets below are from the object
pointer (the vtable). Compiled as 32-bit, the C++ structs are byte-identical
(verified by `static_assert` in the header).

## TCustomAbilityList / TAbilityOwner (base, 0x18 bytes)

| off | type | field | evidence |
|-----|------|-------|----------|
| 0x00 | vtable | | |
| 0x04 | void* | context (owning `TUnitList`/`TArmy`) | `SetOwner` @ `0x5577f524`, `GetLocation` @ `0x5577ed94` |
| 0x08 | u8* | ability bitmap, `ceil(count/8)` bytes | `SetAbCount` @ `0x5574e0f8`, `GetAbilitySet` @ `0x5577f618` |
| 0x0c | u32 | ability count | `GetAbCount` @ `0x5574e1b0` |
| 0x10 | void* | head of `TAbility` list | `TAbilityOwner.ReadWrite` @ `0x5574f318` |
| 0x14 | u32 | refcount (= 1) | `TAbstractUnit.Create` @ `0x5577eb28` |

`SetAbCount` reallocates `+0x08` to `ceil(n/8)` bytes and zeroes the new tail;
`GetAbilitySet(id)` returns bit `id` when `id < count`, else 0.

## TAbstractUnit (0x3c bytes)

| off | type | field | evidence |
|-----|------|-------|----------|
| 0x18 | i32 | `id` (rwIntegerEx default -1) | `TAbstractUnit.ReadWrite` @ `0x557820e4` tag 5 |
| 0x1c | i32 | `unknown_0x1c` (rwInteger) | same, tag 4 |
| 0x20 | void* | `unknown_0x20` (owner callback) | `SetPlayer` @ `0x5577f4a8` (`param_1[8]`) |
| 0x24 | u8 | `player_id` | `SetPlayer` @ `0x5577f4a8`; `UpdateMoraleValue` @ `0x5577f058` |
| 0x25 | u8 | `flags` (bit1 = strengths valid) | `Destroy` @ `0x5577eb6c`, `GetStrength` @ `0x5577f8f4` |
| 0x26 | i8 | `morale_value` (clamped -25..125) | `GetMorale` @ `0x5577f178`, `UpdateMoraleValue` |
| 0x27 | u8 | `unknown_0x27` | - |
| 0x28 | u8[10] | per-damage-type strength | `GetStrength` @ `0x5577f8f4` |
| 0x32 | u8 | all-types strength | `GetStrengthMax` @ `0x5577f924` |
| 0x33 | u8 | `unknown_0x33` | - |
| 0x34 | i32 | strength multiplier | `GetStrength` @ `0x5577f8f4` (`param_1[0xd]`) |
| 0x38 | u8 | `unknown_0x38` | `TAbstractUnit.ReadWrite` mode branch |
| 0x39 | u8[3] | pad to 0x3c | (keeps TUnit's fields at their true offsets) |

`UpdateDefensiveStrength` @ `0x5577f714` sets `+0x34 = (defence+10)*(hits+10)`.
`UpdateOffensiveStrength` @ `0x5577f75c` recomputes `+0x32` and the 10-byte
`+0x28` table from the owned abilities' damage values.

## TUnit (0x48 bytes) - the extra fields over TAbstractUnit

| off | type | field | evidence |
|-----|------|-------|----------|
| 0x3c | i8 | `experience` | `GetExperience` @ `0x557828ac` (tag 7) |
| 0x3d | u8 | `move_points` | `GetMovePoints` @ `0x5578276c` (tag 8) |
| 0x3e | u8 | `hit_points` | `GetHitPoints` @ `0x557827c0` (tag 9) |
| 0x3f | u8 | `unknown_0x3f` | - |
| 0x40 | `TUnitResource*` | unit resource | `GetMoves` @ `0x55782b84` (`param_1[0x10]`); tag 10 |
| 0x44 | i8 | attack bonus | `Changed` @ `0x55782b34` |
| 0x45 | i8 | defense bonus | `Changed` |
| 0x46 | i8 | resistance bonus | `Changed` |
| 0x47 | i8 | damage bonus | `Changed` |

`Changed` @ `0x55782b34` fills the four bonus bytes from
`TAbilityOwner.GetAb{Attack,Defense,Resistance,Damage}All`.

## TUnitResource (0x54 bytes)

| off | type | field | tag |
|-----|------|-------|-----|
| 0x18 | i32 | gfx index | 8 (`GetUnitGFXResourceIndex` @ `0x55782c54`) |
| 0x1c | AnsiString | name | 10 |
| 0x20 | u8 | race | 13 (`GetRace` @ `0x55782c90`) |
| 0x21 | u8 | `unknown_0x21` | 22 |
| 0x24 | AnsiString | qualifier | 11 |
| 0x28 | u8 | alignment | 12 (`GetAlignment` @ `0x55782770`) |
| 0x29 | u8 | attack | 14 |
| 0x2a | u8 | defense | 15 |
| 0x2b | u8 | damage | 16 |
| 0x2c | u8 | hit_points | 17 |
| 0x2d | u8 | moves | 18 |
| 0x2e | u8 | resistance | 19 |
| 0x2f | u8 | level | 20 |
| 0x30 | u8 | unit_type | 21 (`GetUnitType` @ `0x55782808`) |
| 0x31 | u8 | gender | 23 (`GetGender` @ `0x55782810`) |
| 0x38 | `TAbilityOwner*[3]` | ability owners | 25,30,31 |
| 0x44 | u8 | transport capacity | 24 |
| 0x48 | `TStringList*` | description | 26 (`GetDescription` @ `0x55782918`) |
| 0x4c | u32 | obtain_value | 27 (`GetObtainValue` @ `0x55782764`) |
| 0x50 | u8 | unit_size | 28 (`GetUnitSize` @ `0x557827f8`) |
| 0x51 | u8 | blood_type | 29 (`GetBloodType` @ `0x55782800`) |

Defaults (`TUnitResource.Create` @ `0x55784c44`): moves 16, hp 5,
attack/defense/damage/resistance 3, unit_size 1, blood_type 1; a `TStringList`
at `+0x48` and three `TAbilityOwner` at `+0x38`.

## TUnitGFXResource (0x48 bytes)

| off | type | field | evidence |
|-----|------|-------|----------|
| 0x14 | i32 | index | `GetIndex` @ `0x55783bf0` |
| 0x18 | i32 | unit resource index | tag 7 |
| 0x1c | i32 | face image index | `GetFaceImage` @ `0x55783b00` (`param[7]`) |
| 0x20 | void* | face image ILB | `GetFaceImage` (`param[8]`) |
| 0x24 | i32 | `unknown_0x24` | tag 6 |
| 0x28 | `TImageSequenceList*` | cell/portrait sequences | `GetCellImage` @ `0x55783b2c` (`param_1[10]`) |
| 0x2c | i32 | 0 | `Create` @ `0x55783878` |
| 0x30 | u8 | `unknown_0x30` | tag 10 |
| 0x34 | void*[5] | sound `TSFXLibrary` nodes | tags 15..19 |

## Engine constants (AoWEPACK .DATA)

Rank progressions (indexed by `Rank`, only 0..2 reachable):

| table | address | values |
|-------|---------|--------|
| Defense    | `0x558e83c4` | 0, 1, 1 |
| Attack     | `0x558e83c8` | 0, 1, 1 |
| Damage     | `0x558e83cc` | 0, 0, 1 |
| Resistance | `0x558e83d0` | 0, 0, 1 |
| Hits       | `0x558e83d4` | 0, 1, 2 |

Morale (indexed by `Morale`: 0=Broken .. 4=Fanatic):

| table | address | values |
|-------|---------|--------|
| MoraleResistanceModifier | `0x558e83d8` | -2, -1, 0, 0, 0 |
| MoraleDefenseModifier    | `0x558e83e0` | -2, -1, 0, 0, 0 |

Morale value <-> level (`MoraleValueToMorale` @ `0x5575ad64`,
`MoraleToMoraleValue` @ `0x5575ace0`):

| Morale | value range | representative |
|--------|-------------|----------------|
| Broken | < 21 | 10 |
| Low | 21..40 | 30 |
| Normal | 41..60 | 50 |
| High | 61..80 | 70 |
| Fanatic | >= 81 | 90 |

`UpdateMoraleValue` @ `0x5577f058` clamps the stored value to -25..125
(`-25` = `0xE7`, `125` = `0x7D`).

## Stat formulas (transcribed from the getters)

```
level = (i8)resource.level
exp   = (u8)experience
rank  = exp < level*2 ? Recruit : exp < level*6 ? Seasoned : Veteran

inherent_attack  = AttackProg[rank]     + (i8)resource.attack
attack           = clamp(1,10, inherent_attack  + attack_bonus)
inherent_defense = DefenseProg[rank]    + (i8)resource.defense
defense          = clamp(1,10, inherent_defense + defense_bonus + MoraleDefMod[morale])
inherent_damage  = DamageProg[rank]     + (i8)resource.damage
damage           = clamp(1,10, inherent_damage  + damage_bonus)
inherent_resist  = ResistProg[rank]     + (i8)resource.resistance
resistance       = clamp(1,10, inherent_resist  + resistance_bonus + MoraleResMod[morale])
hits             = HitsProg[rank]       + (i8)resource.hit_points
moves            = (i8)resource.moves
upkeep           = (level + 1) * 2                 // ability 0x9b suppresses it
transport_cap    = max(ability0x32.level, resource.transport_capacity)
```

`GetNextLevelExperience` @ `0x55782920`: `exp < level*2` -> `level*2`;
else `exp < level*6` -> `level*6`; else 0.

## Serialization tags

`TUnit.ReadWrite` @ `0x55782cec` chains `TAbstractUnit.ReadWrite` @
`0x557820e4` (@ tags 4,5,6,0x2f) and adds tags 7..10; `TUnitResource.ReadWrite`
@ `0x55784de4` adds tags 10..31 over `TAbstractUnitResource.ReadWrite` @
`0x55784a48` (tag 8). Tag encodings (`rwInteger`, `rwByte`, `rwWord`,
`rwString`, `rwStrings`, `rwEObject`, ...) come from `Engine.TEReadStorageStream`
(see `docs/01_FORMATS.md`). The full list is the `UnitRwTag` enum in `unit.hpp`.

## Known unknowns

These fields are not yet named; each carries its access site so a follow-up can
finish them without re-deriving the layout:

* `TAbstractUnit+0x1c` (tag 4), `+0x20`, `+0x27`, `+0x33`, `+0x38`.
* `TUnit+0x3f` (the byte after hit points; not touched by any recovered getter).
* The 4th byte of every rank-progression slot is `0x90` in memory but is never
  indexed (`rank & 0x7f` with rank 0..2), so it is treated as out of range.
* The `0x00 0x8D 0x40 0x00` bytes between `MoraleResistanceModifier` and
  `MoraleDefenseModifier` are not reached by the indexed loads.
* Damage type / immunity / protection bit names (only "all" = bit 8 is known),
  and the alignment/gender/blood/size value names (only Neutral=3, and the
  Create defaults 1/1, are known).

## Tests

`src/tests/unit_test.cpp` (58 checks) locks the rank thresholds, every stat
formula, the morale mapping and modifiers, ability-bit access, the strength
formula and hit/move clamping. It also asserts the 32-bit object sizes/offsets
(the `static_assert`s in `unit.hpp` fire on any 32-bit target). Run with
`./build.ps1 -Test` or `ctest -R unit`.

## On the world map

The *placed* units of a scenario (an `AoWE.TArmyHS` field object and the
`TUnit`/`THero`/`TLeader` objects it carries) are a separate concern from this
runtime class: how they are serialised and drawn is `docs/13_UNITS_ON_MAP.md`
(`MapUnit` in `aow/map_types.hpp`, `parse_army` in `src/engine/map.cpp`).
