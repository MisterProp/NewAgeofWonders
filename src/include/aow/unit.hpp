// unit.hpp - the runtime Unit class hierarchy of Age of Wonders 1.
//
// Reconstructed 1:1 from the Delphi game module AoWEPACK.dpl (image base
// 0x55700000). Every field offset, data type, enum and bitmask below cites the
// decompiled method it was recovered from; anything that stayed ambiguous is
// named `unknown_0xNN` and carries the access site.
//
// Class hierarchy (Delphi -> here):
//
//   Engine.TEObject
//     Engine.TEResource
//       AoWE.TAbstractUnitResource
//         AoWE.TUnitResource            class 0x20212
//       AoWE.TUnitGFXResource           class 0x20210
//     AoWE.TCustomAbilityList         (ability bit array)
//       AoWE.TAbilityOwner            (list of TAbility instances)
//         AoWE.TAbstractUnit          (morale, strengths, owner)
//           AoWE.TUnit                  class 0x20213
//           AoWE.THero                  class 0x20230
//
// Offsets are byte offsets from the object's vtable pointer (Delphi objects are
// laid out as [vtable][fields...]). The raw layouts are declared with
// #pragma pack(push,4) so that, compiled as 32-bit (i386), they are byte-for-
// byte identical to the in-memory game objects; static_asserts verify this when
// sizeof(void*) == 4.
//
// Evidence lives in port/re/exports/aowepack_unit/ (Ghidra decompiles) and in
// docs/10_UNIT.md. The constant arrays were read straight out of memory at
// 0x558e83c4.. via GhidraMCP /read_memory and confirmed against the getters'
// disassembly (tools/disasm.py).
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace aow {
namespace unit {

// ===========================================================================
// Engine constants (read from AoWEPACK .DATA, cross-checked with the getters)
// ===========================================================================

// Rank ("medal") progression tables, indexed by Rank (0..2).
//   GetAttack  @ 0x557829ec -> AttackRankProgression     @ 0x558e83c8
//   GetDefense @ 0x55782a40 -> DefenseRankProgression    @ 0x558e83c4
//   GetDamage  @ 0x55782aa8 -> DamageRankProgression     @ 0x558e83cc
//   GetResist. @ 0x55782ae8 -> ResistanceRankProgression @ 0x558e83d0
//   GetHits    @ 0x55782b68 -> HitsRankProgression       @ 0x558e83d4
// Each entry is a signed char; only indices 0..2 are reachable (GetRank
// returns 0..2). The 4th byte of each slot (0x90) is never indexed.
constexpr std::int8_t kAttackRankProgression[3]     = {0, 1, 1};
constexpr std::int8_t kDefenseRankProgression[3]    = {0, 1, 1};
constexpr std::int8_t kDamageRankProgression[3]     = {0, 0, 1};
constexpr std::int8_t kResistanceRankProgression[3] = {0, 0, 1};
constexpr std::int8_t kHitsRankProgression[3]       = {0, 1, 2};

// Morale defence/resistance modifiers, indexed by Morale (0..4).
//   GetDefense    @ 0x55782a40 -> MoraleDefenseModifier    @ 0x558e83e0
//   GetResistance @ 0x55782ae8 -> MoraleResistanceModifier @ 0x558e83d8
constexpr std::int8_t kMoraleDefenseModifier[5]    = {-2, -1, 0, 0, 0};
constexpr std::int8_t kMoraleResistanceModifier[5] = {-2, -1, 0, 0, 0};

// ===========================================================================
// Enumerations
// ===========================================================================

// Unit rank / medal. GetRank @ 0x5578294c, SetRank @ 0x55782978.
enum class Rank : std::uint8_t {
    Recruit  = 0,  // exp <  level*2
    Seasoned = 1,  // level*2 <= exp < level*6
    Veteran  = 2,  // exp >= level*6
};

// Morale (0..4). GetMorale @ 0x5577f178 = MoraleValueToMorale(+0x26).
enum class Morale : std::uint8_t {
    Broken = 0,  // value <  21
    Low    = 1,  // value 21..40
    Normal = 2,  // value 41..60
    High   = 3,  // value 61..80
    Fanatic= 4,  // value >= 81
};

// MoraleValueToMorale @ 0x5575ad64.
inline Morale morale_from_value(int value) {
    if (value < 0x15) return Morale::Broken;
    if (value < 0x51) {
        if (value - 0x15 < 0x14) return Morale::Low;
        if (value - 0x29 < 0x14) return Morale::Normal;
        if (value - 0x3d < 0x14) return Morale::High;
        return Morale::Normal;  // out of range -> 2 (original also raises)
    }
    return Morale::Fanatic;
}

// MoraleToMoraleValue @ 0x5575ace0.
inline int morale_value(Morale m) {
    switch (m) {
        case Morale::Broken:  return 10;
        case Morale::Low:     return 30;
        case Morale::Normal:  return 50;
        case Morale::High:    return 70;
        case Morale::Fanatic: return 90;
    }
    return 50;
}

// Alignment (0..5), stored at TUnitResource+0x28.
//   TUnit.GetAlignment @ 0x55782770: 5 when ability 0x6b is present, else the
//   resource byte. TAbstractUnit.GetAlignment @ 0x5577fc60 (base) returns 3.
// Only "Neutral" (3) is confirmed; the other names are placeholders (the UI
// indexes AoWE.TAlignmentRStr with this byte).
enum class Alignment : std::uint8_t {
    A0 = 0, A1 = 1, A2 = 2, Neutral = 3, A4 = 4, A5 = 5,
};

// Blood type, TUnitResource+0x51 (default 1). GetBloodType @ 0x55782800.
enum class BloodType : std::uint8_t { B0 = 0, B1 = 1 };

// Unit size class, TUnitResource+0x50 (default 1). GetUnitSize @ 0x557827f8.
enum class UnitSize : std::uint8_t { Small = 0, Large = 1 };

// Gender, TUnitResource+0x31. GetGender @ 0x55782810.
enum class Gender : std::uint8_t { G0 = 0, G1 = 1 };

// Race / faction id, TUnitResource+0x20. GetRace @ 0x55782c90. (docs/01_FORMATS)
enum class Race : std::uint8_t {
    Human = 0, Orc = 9, Goblin = 10, Monster = 255,
};

// ===========================================================================
// Bitmask traits
// ===========================================================================

// Movement capabilities (Ability.pfs move_types: 0x0021 walk, 0x3FF fly).
using MoveTypeMask = std::uint32_t;
constexpr MoveTypeMask kMoveTypeAll = 0x3FFu;

// Damage types: 10 of them, indexed 0..9 (GetStrength @ 0x5577f8f4 indexes a
// 10-byte per-type table; UpdateOffensiveStrength @ 0x5577f75c iterates 0..9).
using DamageTypeMask = std::uint16_t;
constexpr DamageTypeMask kDamageTypeAll = 0x0100u;  // dtAll (bit 8)

// Immunity / protection sets. GetImmunityTypes @ 0x5577fd54 ORs in 0x100.
using ImmunityMask   = std::uint32_t;
using ProtectionMask = std::uint32_t;
constexpr ImmunityMask kImmunityAll = 0x0100u;

// State flags, byte at TAbstractUnit+0x25. Destroy @ 0x5577eb6c and
// GetStrength @ 0x5577f8f4 both test (flags & 2).
constexpr std::uint8_t kUnitFlag_strengthValid = 0x02u;

// ===========================================================================
// Ability ids
// ===========================================================================
//
// Abilities are a flat bit array (TCustomAbilityList: buffer @ +0x08, count @
// +0x0c), one bit per ability (296 = indices 0..295 in the shipped data; a
// unit's array holds the subset of abilities it owns). Only the ids the
// decompilation refers to by name are listed.
enum class AbilityId : std::uint16_t {
    WalkTerrain = 0,
    Flying      = 1,
    Water       = 2,
    Forest      = 3,
    Cavern      = 4,
    Transport   = 0x32,  // 50  - transport capacity
    Regeneration= 0x35,  // 53  - morale -40 (GetUnitMoraleValue)
    UndeadAlign = 0x6b,  // 107 - forces Alignment 5 (GetAlignment)
    UndeadMorale= 0x6c,  // 108 - morale -80 (GetUnitMoraleValue)
};

// ===========================================================================
// Raw object layouts (32-bit Delphi objects, [vtable][fields...]).
// ===========================================================================
#pragma pack(push, 4)

// Ability bit array: AoWE.TCustomAbilityList (base of everything below).
//   bits  @ +0x08  (SetAbCount @ 0x5574e0f8 reallocs to ceil(count/8))
//   count @ +0x0c  (GetAbCount @ 0x5574e1b0)
//   list  @ +0x10  first TAbility (TAbilityOwner.ReadWrite @ 0x5574f318)
//   ref   @ +0x14  (TAbstractUnit.Create @ 0x5577eb28 sets it to 1)
struct AbilityList {
    void* vtable;              // +0x00
    void* context;             // +0x04  owning terminator (TUnitList/TArmy)
    std::uint8_t* bits;        // +0x08  ability bitmap, ceil(count/8) bytes
    std::uint32_t count;       // +0x0c  number of ability bits
    void* ability_list;        // +0x10  head of the TAbility instance list
    std::uint32_t refcount;    // +0x14

    // GetAbilitySet @ 0x5577f618
    bool get(std::uint32_t id) const {
        if (id >= count) return false;
        return ((bits[id >> 3] >> (id & 7)) & 1u) != 0;
    }
    // Inverse of GetAbilitySet (bit writer used by SetItemString/Add).
    void set(std::uint32_t id, bool on) {
        if (id >= count) return;
        const std::uint8_t m = static_cast<std::uint8_t>(1u << (id & 7));
        if (on) bits[id >> 3] = static_cast<std::uint8_t>(bits[id >> 3] | m);
        else    bits[id >> 3] = static_cast<std::uint8_t>(bits[id >> 3] & ~m);
    }
};

// TAbstractUnit: AbilityList + identity / morale / strength fields.
struct AbstractUnit : AbilityList {
    std::int32_t id;               // +0x18  tag 5 (rwIntegerEx default -1)
    std::int32_t unknown_0x1c;     // +0x1c  tag 4 (rwInteger, default 0)
    void* owner2;                  // +0x20  notified by SetPlayer @ 0x5577f4a8
    std::uint8_t player_id;        // +0x24  tag 0x2f; SetPlayer @ 0x5577f4a8
    std::uint8_t flags;            // +0x25  bit1 = strengths valid
    std::int8_t morale_value;      // +0x26  tag 6; clamped -25..125
    std::uint8_t unknown_0x27;     // +0x27
    std::uint8_t strength[10];     // +0x28  per damage type 0..9
    std::uint8_t strength_all;     // +0x32  max over owned damage types
    std::uint8_t unknown_0x33;     // +0x33
    std::int32_t strength_mul;     // +0x34  = (defence+10)*(hits+10)
    std::uint8_t unknown_0x38;     // +0x38  byte set by TAbstractUnit.ReadWrite
    // Explicit tail pad so the TAbstractUnit subobject is exactly 0x3c bytes;
    // without it the compiler would let TUnit reuse the base's tail padding and
    // place `experience` at +0x39 instead of +0x3c.
    std::uint8_t pad_0x39[3];      // +0x39

    // GetAbilityCount @ 0x5577f56c
    std::uint32_t ability_count() const { return count; }
    // GetMorale @ 0x5577f178
    Morale morale() const { return morale_from_value(morale_value); }
    // GetStrength @ 0x5577f8f4
    int strength_of(std::uint32_t damage_type) const;
    // GetStrengthMax @ 0x5577f924 (dtAll => strength_all).
    int strength_max(DamageTypeMask types) const;
};

// AoWE.TUnitResource (class 0x20212): the per-unit-type rule record.
// Serialized by TUnitResource.ReadWrite @ 0x55784de4 (tags 10..31) over
// TAbstractUnitResource.ReadWrite @ 0x55784a48 (tag 8) over
// Engine.TEResource.ReadWrite (tags 1..3). Defaults from Create @ 0x55784c44.
struct UnitResource {
    void* vtable;                       // +0x00
    void* list;                         // +0x04  owning TUnitResourceList
    std::uint32_t unknown_0x08;         // +0x08  TEResource id/hash
    std::uint32_t unknown_0x0c;         // +0x0c  TEResource const type (3)
    std::uint32_t unknown_0x10;         // +0x10  TEResource index
    std::uint32_t unknown_0x14;         // +0x14
    std::uint32_t gfx_index;            // +0x18  tag 8  (GetUnitGFXResourceIndex)
    const char* name;                   // +0x1c  tag 10 e.g. "Human"
    std::uint8_t race;                  // +0x20  tag 13 (GetRace)
    std::uint8_t unknown_0x21;          // +0x21  tag 22
    std::uint16_t pad_0x22;             // +0x22
    const char* qualifier;              // +0x24  tag 11 e.g. "Archer"
    std::uint8_t alignment;             // +0x28  tag 12 (GetAlignment)
    std::uint8_t attack;                // +0x29  tag 14
    std::uint8_t defense;               // +0x2a  tag 15
    std::uint8_t damage;                // +0x2b  tag 16
    std::uint8_t hit_points;            // +0x2c  tag 17
    std::uint8_t moves;                 // +0x2d  tag 18
    std::uint8_t resistance;            // +0x2e  tag 19
    std::uint8_t level;                 // +0x2f  tag 20
    std::uint8_t unit_type;             // +0x30  tag 21 (GetUnitType)
    std::uint8_t gender;                // +0x31  tag 23 (GetGender)
    std::uint8_t pad_0x32[6];           // +0x32
    void* ability_owner[3];             // +0x38  tags 25,30,31 (TAbilityOwner)
    std::uint8_t transport_capacity;    // +0x44  tag 24
    std::uint8_t pad_0x45[3];           // +0x45
    void* description;                  // +0x48  tag 26 (TStringList*)
    std::uint32_t obtain_value;         // +0x4c  tag 27 (GetObtainValue)
    std::uint8_t unit_size;             // +0x50  tag 28 (GetUnitSize)
    std::uint8_t blood_type;            // +0x51  tag 29 (GetBloodType)
    std::uint8_t pad_0x52[2];           // +0x52

    // GetUpkeep @ 0x55784d50 (== level + 1; doubled by TAbstractUnit.GetUpkeep)
    int upkeep() const { return static_cast<int>(level) + 1; }
    // GetTransportCapacity @ 0x55784d24 (ability 0x32's level, >= +0x44)
    int transport_capacity_eff() const;
};

// AoWE.TUnitGFXResource (class 0x20210): sprite/ILB reference for a unit.
// Serialized by TUnitGFXResource.ReadWrite @ 0x55783bf4; Create @ 0x55783878.
struct UnitGFXResource {
    void* vtable;                       // +0x00
    std::uint32_t unknown_0x04;         // +0x04
    std::uint32_t unknown_0x08;         // +0x08
    std::uint32_t unknown_0x0c;         // +0x0c
    std::uint32_t unknown_0x10;         // +0x10
    std::int32_t index;                 // +0x14  GetIndex @ 0x55783bf0
    std::int32_t unit_resource_index;   // +0x18  tag 7
    std::int32_t face_image_index;      // +0x1c  tag 8  (GetFaceImage param[7])
    void* face_image_library;           // +0x20  ILB (GetFaceImage param[8])
    std::int32_t unknown_0x24;          // +0x24  tag 6
    void* image_sequences;              // +0x28  TImageSequenceList (GetCellImage)
    std::int32_t unknown_0x2c;          // +0x2c  (0)
    std::uint8_t unknown_0x30;          // +0x30  tag 10
    std::uint8_t pad_0x31[3];           // +0x31
    void* sfx[5];                       // +0x34  tags 0xf..0x13 (TSFXLibrary)
};

// AoWE.TUnit (class 0x20213): TAbstractUnit + per-instance state and the
// combat-stat getters. TUnit.ReadWrite @ 0x55782cec adds tags 7..10.
struct Unit : AbstractUnit {
    std::int8_t experience;         // +0x3c  tag 7  (GetExperience @ 0x557828ac)
    std::uint8_t move_points;       // +0x3d  tag 8  (GetMovePoints @ 0x5578276c)
    std::uint8_t hit_points;        // +0x3e  tag 9  (GetHitPoints @ 0x557827c0)
    std::uint8_t unknown_0x3f;      // +0x3f
    UnitResource* resource;         // +0x40  tag 10 (index -> TUnitResource*)
    std::int8_t attack_bonus;       // +0x44  Changed @ 0x55782b34
    std::int8_t defense_bonus;      // +0x45
    std::int8_t resistance_bonus;   // +0x46
    std::int8_t damage_bonus;       // +0x47

    // Rank / experience. GetRank @ 0x5578294c, SetRank @ 0x55782978,
    // GetNextLevelExperience @ 0x55782920.
    Rank rank() const;
    void set_rank(Rank r);
    void set_experience(int exp);           // 0x557828b4
    int  next_level_experience() const;     // 0x55782920

    // Stat getters. "Inherent" = rank progression + resource only; the plain
    // getter adds the instance bonus byte and is clamped to 1..10.
    int inherent_attack() const;     // 0x557829d0
    int attack() const;              // 0x557829ec
    int inherent_defense() const;    // 0x55782a24
    int defense() const;             // 0x55782a40 (+ morale defence modifier)
    int inherent_damage() const;     // 0x55782a8c
    int damage() const;              // 0x55782aa8
    int inherent_resistance() const; // 0x55782ae0
    int resistance() const;          // 0x55782ae8 (+ morale resist modifier)
    int hits() const;                // 0x55782b68
    int moves() const;               // 0x55782b84
    int unit_level() const;          // 0x55782b8c
    int upkeep() const;              // 0x5577fc64 = (unit_level+1)*2

    void set_hit_points(int hp);     // 0x557827c4 (clamp 0..hits())
    void set_move_points(int mp);    // 0x55782794 (clamp 0..moves())

    // Resource-backed attributes.
    std::uint8_t race() const { return resource->race; }              // 0x55782c90
    std::uint8_t unit_type() const { return resource->unit_type; }    // 0x55782808
    std::uint8_t gender() const { return resource->gender; }          // 0x55782810
    std::uint8_t blood_type() const { return resource->blood_type; }  // 0x55782800
    std::uint8_t unit_size() const { return resource->unit_size; }    // 0x557827f8
    std::uint32_t gfx_index() const { return resource->gfx_index; }   // 0x55782c54
    std::uint32_t obtain_value() const { return resource->obtain_value; }  // 0x55782764
    // GetAlignment @ 0x55782770: 5 when ability 0x6b is owned, else the record.
    std::uint8_t alignment() const {
        return get(static_cast<std::uint32_t>(AbilityId::UndeadAlign))
                   ? 5u : resource->alignment;
    }
    // GetTransportCapacity @ 0x55782ba0: max(ability 0x32 level, resource cap).
    int transport_capacity(int transport_ability_level = 0) const;
    // GetTransporter @ 0x55782bd0.
    bool is_transporter(int transport_ability_level = 0) const {
        return transport_capacity(transport_ability_level) != 0;
    }
    // Changed @ 0x55782b34: recompute the per-instance bonus bytes from the
    // owned abilities (TAbilityOwner.GetAb{Attack,Defense,Resistance,Damage}All).
    void recompute_bonuses(int ab_attack, int ab_defense, int ab_resistance,
                           int ab_damage);
};

#pragma pack(pop)

// ---------------------------------------------------------------------------
// Method definitions (all logic transcribed from the decompiled getters)
// ---------------------------------------------------------------------------

inline int clamp_stat(int v) { return v < 1 ? 1 : (v > 10 ? 10 : v); }

inline Rank Unit::rank() const {
    const int level = static_cast<std::int8_t>(resource->level);
    const int exp = static_cast<std::uint8_t>(experience);
    if (exp < level * 2) return Rank::Recruit;
    if (exp < level * 6) return Rank::Seasoned;
    return Rank::Veteran;
}

inline void Unit::set_rank(Rank r) {
    if (r == rank()) return;
    switch (r) {
        case Rank::Veteran:  set_experience(resource->level * 6); break;
        case Rank::Seasoned: set_experience(resource->level * 2); break;
        case Rank::Recruit:  experience = 0; break;
    }
}

inline void Unit::set_experience(int exp) {
    if (exp > 0xff) exp = 0xff;
    experience = static_cast<std::int8_t>(static_cast<std::uint8_t>(exp));
}

inline int Unit::next_level_experience() const {
    const int level = static_cast<std::int8_t>(resource->level);
    const int exp = static_cast<std::uint8_t>(experience);
    if (exp < level * 2) return level * 2;
    if (exp < level * 6) return level * 6;
    return 0;
}

inline int Unit::inherent_attack() const {
    return kAttackRankProgression[static_cast<unsigned>(rank())] +
           static_cast<std::int8_t>(resource->attack);
}
inline int Unit::attack() const {
    return clamp_stat(inherent_attack() + attack_bonus);
}
inline int Unit::inherent_defense() const {
    return kDefenseRankProgression[static_cast<unsigned>(rank())] +
           static_cast<std::int8_t>(resource->defense);
}
inline int Unit::defense() const {
    return clamp_stat(inherent_defense() + defense_bonus +
                      kMoraleDefenseModifier[static_cast<unsigned>(morale())]);
}
inline int Unit::inherent_damage() const {
    return kDamageRankProgression[static_cast<unsigned>(rank())] +
           static_cast<std::int8_t>(resource->damage);
}
inline int Unit::damage() const {
    return clamp_stat(inherent_damage() + damage_bonus);
}
inline int Unit::inherent_resistance() const {
    return kResistanceRankProgression[static_cast<unsigned>(rank())] +
           static_cast<std::int8_t>(resource->resistance);
}
inline int Unit::resistance() const {
    return clamp_stat(inherent_resistance() + resistance_bonus +
                      kMoraleResistanceModifier[static_cast<unsigned>(morale())]);
}
inline int Unit::hits() const {
    return kHitsRankProgression[static_cast<unsigned>(rank())] +
           static_cast<std::int8_t>(resource->hit_points);
}
inline int Unit::moves() const { return static_cast<std::int8_t>(resource->moves); }
inline int Unit::unit_level() const { return static_cast<std::int8_t>(resource->level); }
inline int Unit::upkeep() const { return (unit_level() + 1) * 2; }

inline void Unit::set_hit_points(int hp) {
    if (hp > hits()) hp = hits();
    if (hp < 0) hp = 0;
    hit_points = static_cast<std::uint8_t>(hp);
}
inline void Unit::set_move_points(int mp) {
    if (mp > moves()) mp = moves();
    if (mp < 0) mp = 0;
    move_points = static_cast<std::uint8_t>(mp);
}

inline int Unit::transport_capacity(int transport_ability_level) const {
    const int cap = static_cast<std::uint8_t>(resource->transport_capacity);
    return transport_ability_level > cap ? transport_ability_level : cap;
}

inline void Unit::recompute_bonuses(int ab_attack, int ab_defense,
                                    int ab_resistance, int ab_damage) {
    attack_bonus = static_cast<std::int8_t>(ab_attack);
    defense_bonus = static_cast<std::int8_t>(ab_defense);
    resistance_bonus = static_cast<std::int8_t>(ab_resistance);
    damage_bonus = static_cast<std::int8_t>(ab_damage);
}

inline int AbstractUnit::strength_of(std::uint32_t damage_type) const {
    if ((flags & kUnitFlag_strengthValid) == 0) return 0;  // caller recomputes
    const std::uint8_t v = strength[damage_type & 0xff];
    return v == 0 ? 0 : (v + 10) * strength_mul;
}

inline int AbstractUnit::strength_max(DamageTypeMask types) const {
    if (types == kDamageTypeAll) {
        const std::uint8_t v = strength_all;
        return v == 0 ? 0 : (v + 10) * strength_mul;
    }
    std::uint8_t best = 0;
    for (int t = 0; t < 10; ++t) {
        if ((types >> t) & 1u) {
            if (strength[t] > best) best = strength[t];
        }
    }
    return best == 0 ? 0 : (best + 10) * strength_mul;
}

inline int UnitResource::transport_capacity_eff() const {
    return static_cast<std::uint8_t>(transport_capacity);
}

// ---------------------------------------------------------------------------
// Serialization tags (property-table tags used by the ReadWrite chain).
// Recovered from: TAbstractUnit.ReadWrite @ 0x557820e4 (tags 4,5,6,0x2f),
// TUnit.ReadWrite @ 0x55782cec (tags 7..10), TUnitResource.ReadWrite @
// 0x55784de4 (tags 10..31), TUnitGFXResource.ReadWrite @ 0x55783bf4.
// ---------------------------------------------------------------------------
enum class UnitRwTag : std::uint32_t {
    AbstractAbilityCount = 2,   // rwInteger (TCustomAbilityList)
    AbstractAbilityMask  = 3,   // rwData ceil(count/8)  (TCustomAbilityList)
    AbstractValue4       = 4,   // rwInteger  -> +0x1c
    AbstractId           = 5,   // rwIntegerEx default -1 -> +0x18
    AbstractMorale       = 6,   // rwByte     -> +0x26
    PlayerId             = 0x2f,// rwByte     -> +0x24
    Experience           = 7,   // rwByte     -> +0x3c
    MovePoints           = 8,   // rwByte     -> +0x3d
    HitPoints            = 9,   // rwByte     -> +0x3e
    UnitResourceIndex    = 10,  // rwWord     -> +0x40 (via resource list)

    ResGfxIndex          = 8,   // rwInteger  -> +0x18
    ResName              = 10,  // rwString   -> +0x1c
    ResQualifier         = 11,  // rwString   -> +0x24
    ResAlignment         = 12,  // rwData(1)  -> +0x28
    ResRace              = 13,  // rwByte     -> +0x20
    ResAttack            = 14,  // rwByte     -> +0x29
    ResDefense           = 15,  // rwByte     -> +0x2a
    ResDamage            = 16,  // rwByte     -> +0x2b
    ResHitPoints         = 17,  // rwByte     -> +0x2c
    ResMoves             = 18,  // rwByte     -> +0x2d
    ResResistance        = 19,  // rwByte     -> +0x2e
    ResLevel             = 20,  // rwByte     -> +0x2f
    ResUnitType          = 21,  // rwData(1)  -> +0x30
    ResExtra22           = 22,  // rwData(1)  -> +0x21
    ResGender            = 23,  // rwData(1)  -> +0x31
    ResTransportCapacity = 24,  // rwByte     -> +0x44
    ResAbilityOwner0     = 25,  // rwEObject  -> +0x38
    ResDescription       = 26,  // rwStrings  -> +0x48
    ResObtainValue       = 27,  // rwInteger  -> +0x4c
    ResUnitSize          = 28,  // rwData(1)  -> +0x50
    ResBloodType         = 29,  // rwData(1)  -> +0x51
    ResAbilityOwner1     = 30,  // rwEObject  -> +0x3c
    ResAbilityOwner2     = 31,  // rwEObject  -> +0x40
};

// Class ids (Engine.TEObject ids) returned by each ClassID method.
constexpr std::uint32_t kUnitGFXResourceClassId = 0x20210;  // 0x55783974
constexpr std::uint32_t kUnitResourceClassId    = 0x20212;  // 0x55784da0
constexpr std::uint32_t kUnitClassId            = 0x20213;  // 0x557826ac

// ---------------------------------------------------------------------------
// Compile-time layout verification (meaningful for the 32-bit target).
// ---------------------------------------------------------------------------
#if defined(__i386__) || defined(_M_IX86) || \
    (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4)
// The objects are non-standard-layout (vtable first, like Delphi), so offsetof
// is conditionally-supported; silence the GCC/Clang warning here.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif
static_assert(sizeof(AbilityList) == 0x18, "AbilityList must be 0x18 bytes");
static_assert(sizeof(AbstractUnit) == 0x3c, "AbstractUnit must be 0x3c bytes");
static_assert(sizeof(Unit) == 0x48, "Unit must be 0x48 bytes");
static_assert(sizeof(UnitResource) == 0x54, "UnitResource must be 0x54 bytes");
static_assert(sizeof(UnitGFXResource) == 0x48, "UnitGFXResource must be 0x48");
static_assert(offsetof(AbilityList, bits) == 0x08, "bits @ +0x08");
static_assert(offsetof(AbilityList, count) == 0x0c, "count @ +0x0c");
static_assert(offsetof(AbilityList, ability_list) == 0x10, "list @ +0x10");
static_assert(offsetof(AbstractUnit, id) == 0x18, "id @ +0x18");
static_assert(offsetof(AbstractUnit, player_id) == 0x24, "player_id @ +0x24");
static_assert(offsetof(AbstractUnit, morale_value) == 0x26, "morale @ +0x26");
static_assert(offsetof(AbstractUnit, strength) == 0x28, "strength @ +0x28");
static_assert(offsetof(AbstractUnit, strength_all) == 0x32, "all @ +0x32");
static_assert(offsetof(AbstractUnit, strength_mul) == 0x34, "mul @ +0x34");
static_assert(offsetof(Unit, experience) == 0x3c, "experience @ +0x3c");
static_assert(offsetof(Unit, resource) == 0x40, "resource @ +0x40");
static_assert(offsetof(Unit, attack_bonus) == 0x44, "attack_bonus @ +0x44");
static_assert(offsetof(UnitResource, gfx_index) == 0x18, "gfx_index @ +0x18");
static_assert(offsetof(UnitResource, attack) == 0x29, "attack @ +0x29");
static_assert(offsetof(UnitResource, ability_owner) == 0x38, "owners @ +0x38");
static_assert(offsetof(UnitResource, obtain_value) == 0x4c, "obtain @ +0x4c");
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#endif

}  // namespace unit
}  // namespace aow
