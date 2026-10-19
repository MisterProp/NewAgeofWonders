// pfs_types.hpp - typed views over the AoW1 .PFS rule tables (Stage 1.5).
//
// pfs.hpp exposes the *container* and each record as raw (tag -> byte range)
// fields plus u32-length strings. This header maps those tags onto the Delphi
// classes that own each record, so the port gets real game data with names.
//
// Field encodings come from the decompiled Engine.TEReadStorageStream rw*
// methods (Enginep.dpl); every tag below cites the ReadWrite call that emits
// it (module + address) and was cross-checked against the shipped data files.
//
// rw* method -> on-disk encoding (the stream vtable is at 0x5550de38, slot 0
// is StorageMode):
//   +0x0c rwEObject     : an embedded EObject ([table][data])
//   +0x1c rwData        : N raw bytes (N is a literal at the call site)
//   +0x20 rwString      : u8 length, 0xFF => a u32 length follows, then bytes
//   +0x24 rwStrings     : u32 length, then bytes (assigned to a TStrings)
//   +0x2c rwInteger     : 4 bytes (u32/i32)
//   +0x30 rwByte        : 1 byte
//   +0x34 rwWord        : 2 bytes
//   +0x38 rwBool        : 0 bytes - the value is the tag's *presence*
//   +0x3c rwShortint    : 1 byte (signed)
//   +0x40 rwIntegerEx   : 4 bytes (value = a default when the tag is absent)
//   +0x44 rwByteEx / +0x48 rwWordEx / +0x4c rwShortintEx : 1/2/1 bytes
//
// Reference decompiles live in re/exports/enginep/ and re/exports/aowepack/.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "aow/pfs.hpp"

namespace aow {

// Class ids (Engine TEObject ids) returned by each record class's ClassID
// method; a record whose field table starts at offset 4 carries this id.
constexpr std::uint32_t kUnitResourceClassId = 0x20212;  // AoWE.TUnitResource
constexpr std::uint32_t kHeroResourceClassId = 0x20231;  // AoWE.THeroResource
constexpr std::uint32_t kHeroClassId = 0x20230;          // AoWE.THero
constexpr std::uint32_t kItemClassId = 0x20268;          // AoWE.TItem
constexpr std::uint32_t kUnitGfxClassId = 0x20210;       // AoWE.TUnitGFXResource
constexpr std::uint32_t kItemGfxClassId = 0x20269;       // AoWE.TItemGFX
constexpr std::uint32_t kFaceResourceClassId = 0x20270;  // AoWE.TFaceResource
constexpr std::uint32_t kTextItemClassId = 0x2029a;      // AoWE.TTextControlItem

// Reads one record's fields with the correct width/encoding per tag.
class RecordView {
public:
    RecordView(const PfsRecord& rec, const std::vector<std::uint8_t>& file);

    // The class id preceding the field table, or 0 when the record has none.
    std::uint32_t class_id() const { return class_id_; }

    const PfsField* find(std::uint32_t tag) const;
    bool has(std::uint32_t tag) const { return find(tag) != nullptr; }

    // rwInteger / rwIntegerEx (4 bytes); returns 'def' when the tag is absent.
    std::uint32_t u32(std::uint32_t tag, std::uint32_t def = 0) const;
    // rwWord (2 bytes).
    std::uint16_t u16(std::uint32_t tag, std::uint16_t def = 0) const;
    // rwByte / rwShortint / rwData(1) (1 byte).
    std::uint8_t u8(std::uint32_t tag, std::uint8_t def = 0) const;
    // rwBool: the value is whether the tag is present at all (0 bytes stored).
    bool boolean(std::uint32_t tag) const { return has(tag); }

    // rwString: u8 length (0xFF => u32 length), then bytes.
    std::string short_string(std::uint32_t tag) const;
    // rwStrings: u32 length, then bytes.
    std::string text(std::uint32_t tag) const;
    // rwData: the raw bytes of a field.
    std::vector<std::uint8_t> bytes(std::uint32_t tag) const;

private:
    const std::uint8_t* rec_ = nullptr;  // record start (file[file_offset])
    std::size_t size_ = 0;
    std::uint32_t class_id_ = 0;
    const std::vector<PfsField>* fields_ = nullptr;
};

// ---------------------------------------------------------------------------
// Ability.pfs (147 records) - AoWE.TAbility.ReadWrite @ AoWEPACK 0x5574f07c.
// TAbility.Create @ 0x5574e5f4 shows the fields: +0x10 TStringList (text),
// +0x14 int, +0x18 a TENode sound node, +0x1c a TImageSequenceList.
// ---------------------------------------------------------------------------
struct Ability {
    std::size_t index = 0;  // record index in the list
    std::uint32_t tag = 0;  // record tag

    // tag 5 rwStrings: the ability's text (u32 length + bytes).
    std::string description;
    // tag 6 rwInteger: +0x14 [TODO(RE)] small int, e.g. 30 for "Flying".
    std::uint32_t cost = 0;
    // tag 7 rwEObject: a sound node present flag (TENode / TSFXLibrary).
    bool has_sound = false;
    // tag 8 rwStreamProc: an image-sequence list present flag.
    bool has_sequence = false;
    // tag 9 rwData (2 bytes): a small movement bitmask (version-dependent). In
    // the original data "Flying" and plain walking are both 0x21 and
    // forest/cavern passage is 0x3A1; the AoW X mod used 0x3FF for "Flying".
    std::uint16_t move_types = 0;
};

// ---------------------------------------------------------------------------
// Spells.pfs (108 records) - AoWE.TSpell.ReadWrite @ AoWEPACK 0x55779234.
// ---------------------------------------------------------------------------
struct Spell {
    std::size_t index = 0;
    std::uint32_t tag = 0;

    std::string description;               // tag 10 rwStrings (u32 + bytes)
    std::vector<std::uint8_t> effect_ref;  // tag 11 rwEObject (embedded node)
    std::vector<std::uint8_t> animation;   // tag 12 rwStreamProc (image list)
    std::uint32_t mana_cost = 0;           // tag 13 rwInteger - +0x14; THero.CastingMana
                                           //   @ 0x557894ec (doubled per sphere)
    std::uint32_t upkeep = 0;              // tag 14 rwInteger - +0x1c; copied into a
                                           //   global enchantment (SetupEnchantment)
    std::uint32_t research_cost = 0;       // tag 15 rwInteger - +0x18, read by
                                           //   ExecuteResearchSpell @ 0x5577d538 and
                                           //   TSpell.SetupCastSpellAction @ 0x557796d4
    std::uint8_t sphere = 0;               // tag 16 rwData(1) - +0x20 magic realm;
                                           //   ValidResearchSpell @ 0x5577cff0
    std::uint8_t level = 0;                // tag 17 rwByte - +0x21 spell level (1..4);
                                           //   ValidResearchSpell / GetRandomSpell
};

// ---------------------------------------------------------------------------
// Unitres.pfs (188 records) - class 0x20212 TUnitResource.
// Chain: TEResource.ReadWrite (Enginep 0x5551b064, tags 1..3),
//        TAbstractUnitResource.ReadWrite @ 0x55784a48 (tag 8),
//        TUnitResource.ReadWrite @ 0x55784de4 (tags 10..31).
// ---------------------------------------------------------------------------
struct UnitStat {
    std::size_t index = 0;
    std::uint32_t tag = 0;

    // tags 1..3 come from TEResource.ReadWrite (Enginep 0x5551b064). tag 2 is a
    // constant per table (3 for units); tag 1 is a 32-bit id/hash present in a
    // subset of records; tag 3 is a small index.
    std::uint32_t res_id = 0;     // tag 1 rwInteger (TEResource)
    std::uint32_t res_type = 0;   // tag 2 rwInteger (TEResource) - 3 for units
    std::uint32_t res_index = 0;  // tag 3 rwInteger (TEResource)
    std::uint32_t gfx_index = 0;  // tag 8 rwInteger - unitGFXResourceIndex
    std::string name;             // tag 10 rwString (u8 len) e.g. "Human"
    std::string qualifier;        // tag 11 rwString (u8 len) e.g. "Archer"
                                  //   GetUnitName @ 0x55784da8 joins both.
    std::uint8_t type = 0;        // tag 12 rwData(1)
    std::uint8_t race_id = 0;     // tag 13 rwByte - race/faction id: 0=Human,
                                  //   9=Orc, 10=Goblin, 255=monster (no race)
    std::uint8_t attack = 0;      // tag 14 rwByte (TUnit.GetAttack reads +0x29)
    std::uint8_t defense = 0;     // tag 15 rwByte (TUnit.GetDefense reads +0x2a)
    std::uint8_t damage = 0;      // tag 16 rwByte (TUnit.GetDamage reads +0x2b)
    std::uint8_t hit_points = 0;  // tag 17 rwByte (unit info dialog shows +0x2c)
    std::uint8_t moves = 0;       // tag 18 rwByte (TUnit.GetMoves reads +0x2d)
    std::uint8_t resistance = 0;  // tag 19 rwByte (TUnit.GetResistance reads +0x2e)
    std::uint8_t level = 0;       // tag 20 rwByte (TUnit.GetUnitLevel reads +0x2f);
                                  //   TUnitResource.GetUpkeep @ 0x55784d50 = level + 1
    std::uint8_t extra21 = 0;     // tag 21 rwData(1)
    std::uint8_t extra22 = 0;     // tag 22 rwData(1)
    std::uint8_t extra23 = 0;     // tag 23 rwData(1)
    std::uint8_t transport_capacity = 0;    // tag 24 rwByte
    std::vector<std::uint8_t> ability_ref;  // tag 25 rwEObject
    std::string description;                // tag 26 rwStrings (u32 + bytes)
    std::uint32_t obtain_value = 0;         // tag 27 rwInteger
    std::uint8_t extra28 = 0;               // tag 28 rwData(1)
    std::uint8_t extra29 = 0;               // tag 29 rwData(1)
    std::vector<std::uint8_t> obj30;        // tag 30 rwEObject
    std::vector<std::uint8_t> obj31;        // tag 31 rwEObject
};

// ---------------------------------------------------------------------------
// ITEMS.PFS (176 records) - class 0x20268 TItem.
// Chain: TAbilityOwner.ReadWrite (tags 2,3 via TCustomAbilityList) +
//        TItem.ReadWrite @ AoWEPACK 0x557945c8 (tags 7..22).
// ---------------------------------------------------------------------------
struct Item {
    std::size_t index = 0;
    std::uint32_t tag = 0;

    std::uint32_t ability_count = 0;         // tag 2 rwInteger (ability count)
    std::vector<std::uint8_t> ability_mask;  // tag 3 rwData (ceil(count/8) B)
    std::uint32_t gfx_index = 0;       // tag 7 rwInteger (SetItemGFXIndex +0x40)
    std::string name;                  // tag 8 rwString (u8) "Ancient Helm"
    std::string description;           // tag 9 rwString (u8 len)
    std::uint8_t flag10 = 0;           // tag 10 rwData(1)
    std::uint8_t flag11 = 0;           // tag 11 rwByte
    std::uint8_t flag12 = 0;           // tag 12 rwByte
    std::uint8_t flag13 = 0;           // tag 13 rwByte
    std::uint8_t flag14 = 0;           // tag 14 rwByte
    std::uint8_t item_type = 0;        // tag 15 rwData(1) (SetItemType +0x34)
    bool bonus = false;                // tag 16 rwBool (presence only)
    std::uint32_t value17 = 0;         // tag 17 rwInteger [TODO(RE)]
    std::string text18;                // tag 18 rwStrings (u32 + bytes)
    std::uint32_t obtain_value = 0;    // tag 19 rwInteger (GetObtainValue +0x2c)
    std::uint32_t spell = 0;           // tag 20 rwInteger - granted spell id; TItem.CanUse
                                       //   @ 0x557940ac reads +0x38 when item_type == 5
    std::uint32_t library_item = 0;    // tag 21 rwIntegerEx (GetLibraryItem +0x24)
    std::string library_name;          // tag 22 rwString (u8 len) (GetLibraryName)
};

// ---------------------------------------------------------------------------
// HEROES.PFS (193 records) - class 0x20230 THero.
// Chain: TAbilityOwner (tags 2,3), TAbstractUnit.ReadWrite @ 0x557820e4
//        (tags 4,5,6,0x2f), THero.ReadWrite @ 0x55788880 (tags 7..0x27).
// ---------------------------------------------------------------------------
struct Hero {
    std::size_t index = 0;
    std::uint32_t tag = 0;

    std::uint32_t ability_count = 0;         // tag 2  rwInteger
    std::vector<std::uint8_t> ability_mask;  // tag 3  rwData
    std::uint32_t value4 = 0;         // tag 4  rwInteger (TAbstractUnit) [TODO(RE)]
    std::uint32_t value5 = 0;         // tag 5  rwIntegerEx (TAbstractUnit) [TODO(RE)]
    std::uint8_t morale = 0;          // tag 6  rwByte (TAbstractUnit.GetMorale +0x26)
    std::uint32_t face_index = 0;     // tag 7  rwInteger (GetFace/SetFaceIndex +0x3c)
    std::uint32_t hero_resource_index = 0;  // tag 8 rwInteger (SetHeroResourceIndex +0x44)
    std::uint32_t value9 = 0;         // tag 9  rwInteger [TODO(RE)] (transient)
    std::string name;                 // tag 10 rwString (u8) "Conan"
    std::string nickname;             // tag 11 rwString (u8) "McHarr"
    std::uint8_t value12 = 0;         // tag 12 rwByte [TODO(RE)]
    std::uint32_t casting_points = 0; // tag 13 rwInteger (GetCastingPoints +0x80)
    std::uint32_t value14 = 0;        // tag 14 rwInteger [TODO(RE)]
    std::uint8_t gender = 0;          // tag 15 rwData(1) (GetGender +0x69)
    std::uint8_t attack_skill = 0;    // tag 16 rwByte (SetUnitAttack +0x6a)
    std::uint8_t defense_skill = 0;   // tag 17 rwByte (SetUnitDefense +0x6b)
    std::uint8_t damage_skill = 0;    // tag 18 rwByte (SetUnitDamage +0x6c)
    std::uint8_t hits_skill = 0;      // tag 19 rwByte (SetUnitHits +0x6d)
    std::uint8_t moves_skill = 0;     // tag 20 rwByte (SetUnitMoves +0x6e)
    std::uint8_t resistance_skill = 0;  // tag 21 rwByte (SetUnitResistance +0x6f)
    std::uint8_t unit_level = 0;      // tag 22 rwByte (GetUnitLevel +0x4c)
    bool value23 = false;             // tag 23 rwBool (presence)
    std::uint32_t experience = 0;     // tag 24 rwInteger (GetExperience/GetLevel +0x48)
    std::uint8_t value25 = 0;         // tag 25 rwData(1) [TODO(RE)]
    std::uint32_t value26 = 0;        // tag 26 rwInteger [TODO(RE)]
    std::uint8_t value27 = 0;         // tag 27 rwByte [TODO(RE)]
    std::uint8_t move_points = 0;     // tag 29 rwByte (GetMovePoints +0x79)
    std::uint8_t hit_points = 0;      // tag 30 rwByte (GetHitPoints +0x7a)
    std::uint32_t value31 = 0;        // tag 31 rwInteger [TODO(RE)]
    bool value33 = false;             // tag 33 rwBool (presence)
    std::uint32_t value34 = 0;        // tag 34 rwInteger [TODO(RE)]
    std::uint32_t library_hero = 0;   // tag 35 rwIntegerEx (GetLibraryHero +0x90)
    std::vector<std::uint8_t> ability_set_b;  // tag 36 rwEObject
    std::vector<std::uint8_t> ability_set;    // tag 37 rwEObject
    std::string library_name;         // tag 38 rwString (u8) (GetLibraryName +0x94)
    std::uint32_t used_skill_points = 0;  // tag 39 rwInteger (GetSkillPoints +0x50)
};

// ---------------------------------------------------------------------------
// HERORES.PFS (50 records) - class 0x20231 THeroResource
// (ReadWrite @ AoWEPACK 0x55789fd4). Chain: TEResource (tags 1..3),
// TAbstractUnitResource (tag 8 = gfx index), then tags 10..30.
// ---------------------------------------------------------------------------
struct HeroResource {
    std::size_t index = 0;
    std::uint32_t tag = 0;
    std::uint32_t res_id = 0;      // tag 1 rwInteger (TEResource)
    std::uint32_t res_type = 0;    // tag 2 rwInteger (TEResource) - 24
    std::uint32_t res_index = 0;   // tag 3 rwInteger (TEResource)
    std::uint32_t gfx_index = 0;   // tag 8 rwInteger
    std::string name;              // tag 10 rwString (u8 len)
    std::uint8_t value11 = 0;      // tag 11 rwByte [TODO(RE)]
    std::uint8_t value12 = 0;      // tag 12 rwData(1) [TODO(RE)]
    std::uint8_t hero_class = 0;   // tag 13 rwData(1) (clamped to <= 6)
    bool value14 = false;          // tag 14 rwBool (presence)
    std::uint8_t attack_skill = 0; // tag 15 rwByte (+0x24)
    std::uint8_t defense_skill = 0;    // tag 16 rwByte (+0x25)
    std::uint8_t damage_skill = 0;     // tag 17 rwByte (+0x26)
    std::uint8_t hits_skill = 0;       // tag 18 rwByte (+0x27)
    std::uint8_t moves_skill = 0;      // tag 19 rwByte (+0x28)
    std::uint8_t resistance_skill = 0; // tag 20 rwByte (+0x29)
    std::vector<std::uint8_t> obj29;   // tag 29 rwEObject
    std::uint32_t value30 = 0;         // tag 30 rwInteger [TODO(RE)]
};

// ---------------------------------------------------------------------------
// Unitgfx.pfs (239 records) - class 0x20210 TUnitGFXResource
// (ReadWrite @ AoWEPACK 0x55783bf4).
// ---------------------------------------------------------------------------
struct UnitGfx {
    std::size_t index = 0;
    std::uint32_t tag = 0;
    std::uint32_t res_id = 0;      // tag 1 (TEResource)
    std::uint32_t res_type = 0;    // tag 2 - 4
    std::uint32_t res_index = 0;   // tag 3
    std::vector<std::uint8_t> sequence;  // tag 5 rwStreamProc (ILB frames)
    std::string image_lib;         // tag 6 rwString (u8) - +0x24; SetFaceImageLibraryName
    std::string name;              // tag 7 rwString (u8) - +0x18; unit display name
    std::uint32_t gfx_index = 0;   // tag 8 rwInteger - +0x1c face image index
    std::uint8_t value10 = 0;      // tag 10 rwData(1)
    std::vector<std::uint8_t> obj15;  // tag 15 rwEObject
    std::vector<std::uint8_t> obj16;  // tag 16 rwEObject
    std::vector<std::uint8_t> obj17;  // tag 17 rwEObject
    std::vector<std::uint8_t> obj18;  // tag 18 rwEObject
    std::vector<std::uint8_t> obj19;  // tag 19 rwEObject
};

// ---------------------------------------------------------------------------
// FACERES.PFS (260 records) - class 0x20270 TFaceResource
// (ReadWrite @ AoWEPACK 0x5574d540).
// ---------------------------------------------------------------------------
struct FaceResource {
    std::size_t index = 0;
    std::uint32_t tag = 0;
    std::uint32_t res_id = 0;      // tag 1 (TEResource)
    std::uint32_t res_type = 0;    // tag 2 - 25
    std::uint32_t res_index = 0;   // tag 3
    std::string image_lib;         // tag 5 rwString (u8) image library name
    std::uint32_t face_index = 0;  // tag 6 rwInteger (FaceResourceIndex)
    bool value7 = false;           // tag 7 rwBool (presence)
};

// ---------------------------------------------------------------------------
// ITEMGFX.PFS (372 records) - class 0x20269 TItemGFX (ReadWrite @ 0x55795b64).
// ---------------------------------------------------------------------------
struct ItemGfx {
    std::size_t index = 0;
    std::uint32_t tag = 0;
    std::uint32_t res_id = 0;      // tag 1 (TEResource)
    std::uint32_t res_type = 0;    // tag 2 - 26
    std::uint32_t res_index = 0;   // tag 3
    std::string image_lib;         // tag 6 rwString (u8) image library name
    std::uint32_t gfx_index = 0;   // tag 7 rwInteger (ItemGFXIndex)
    std::uint8_t value8 = 0;       // tag 8 rwData(1)
    bool value9 = false;           // tag 9 rwBool (presence)
};

// ---------------------------------------------------------------------------
// TEXT.pfs (91 records) - class 0x2029a TTextControlItem
// (ReadWrite @ AoWEPACK 0x55764910).
// ---------------------------------------------------------------------------
struct TextItem {
    std::size_t index = 0;
    std::uint32_t tag = 0;
    std::uint32_t id = 0;          // tag 9  rwInteger
    std::string text;              // tag 10 rwStrings (u32 + bytes)
    std::string name;              // tag 11 rwString (u8 len)
};

// ---------------------------------------------------------------------------
// comb_res.pfs (581 records) - AoWTer.TAoWTerrainResource
// (ReadWrite @ 0x5579be68 = Terrain.TTerrainMOR.ReadWrite + tag 0x14).
// TTerrainMOR.ReadWrite is an unrecovered jump table, so only the TEResource
// tags and the trailing tag 0x14 are typed; the rest is kept raw.
// ---------------------------------------------------------------------------
struct TerrainRes {
    std::size_t index = 0;
    std::uint32_t tag = 0;
    std::uint32_t res_id = 0;      // tag 1 (TEResource)
    std::uint32_t res_type = 0;    // tag 2 (34..42 - terrain type)
    std::uint32_t res_index = 0;   // tag 3
    std::uint32_t value20 = 0;     // tag 0x14 rwInteger [TODO(RE)]
};


// Loaders - each reads every record (optionally filtered by class id) and
// returns the typed structs in file order.
std::vector<Ability> load_abilities(const PfsFile& pfs);
std::vector<Spell> load_spells(const PfsFile& pfs);
std::vector<UnitStat> load_units(const PfsFile& pfs);
std::vector<Item> load_items(const PfsFile& pfs);
std::vector<Hero> load_heroes(const PfsFile& pfs);
std::vector<HeroResource> load_hero_resources(const PfsFile& pfs);
std::vector<UnitGfx> load_unit_gfx(const PfsFile& pfs);
std::vector<FaceResource> load_faces(const PfsFile& pfs);
std::vector<ItemGfx> load_item_gfx(const PfsFile& pfs);
std::vector<TextItem> load_texts(const PfsFile& pfs);
std::vector<TerrainRes> load_terrain(const PfsFile& pfs);
}  // namespace aow

