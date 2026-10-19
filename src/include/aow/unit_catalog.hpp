// unit_catalog.hpp - load *every* unit of the game from the shipped data.
//
// A unit in AoW1 is assembled from three sources:
//
//   Unitres.pfs    one AoWE.TUnitResource per unit type  (stats, name, race,
//                  gfx_index, abilities, description)  -- class 0x20212
//   Unitgfx.pfs    one AoWE.TUnitGFXResource per sprite  (name, image
//                  library, face index, animation)     -- class 0x20210
//   Release.hss    the game pack (TAoWWHSSet): the race/faction resources
//                  (RACES\R*.ILB) that give each unit's race a body library
//
// and the two small `.ahl` files (High Level / Special) list "leader" unit
// sets. This loader reads all of them and joins them into one roster.
//
// Join key (verified empirically + against the engine):
//   UnitResource.gfx_index  ->  UnitGFXResource.res_index
// `TUnit.GetFace` @ 0x5578283c looks the sprite up in the UnitGFXResourceList
// by `resource+0x18` (== gfx_index); the list is a TEResourceList indexed by
// the record's own TEResource index (res_index), which is sparse (it skips
// ids 10/11/15..17/28/29 ...), so a positional lookup is wrong - the id must be
// matched. Evidence: re/exports/aowepack_unit/AoWEPACK_dpl/0x55783cf8.c
// (GetUnitGFXResource) + a full data sweep (unit 18 "Azrac Archer" gfx_index 18
// -> Unitgfx res_index 18 "Azrac Archer").
//
// Counts are data-version dependent: this GOG install ships 142 unit resources
// and 182 unit-graphics (the docs' older "188 / 239" numbers are stale - see
// docs/11_UNIT_CATALOG.md). The loader never hardcodes them.
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "aow/unit_anim.hpp"

namespace aow {

// One Unitgfx.pfs record, resolved to the fields a unit needs.
struct UnitGraphics {
    std::size_t index = 0;         // position in Unitgfx.pfs
    std::uint32_t res_index = 0;   // tag 3 - the id UnitResource.gfx_index uses
    std::string name;              // tag 7 - sprite/unit display name
    std::string image_lib;         // tag 6 - face/portrait library (FACES\*.ILB)
    std::uint32_t face_index = 0;  // tag 8 - face image index (+0x1c)
    std::size_t frames_size = 0;   // tag 5 - rwStreamProc ILB byte count
    // The on-map unit sprite library, e.g. "UNITS\HMN\HMNAR.ILB". It is embedded
    // (as a string) inside the tag-5 TImageSequenceList, next to the shadow
    // (UNITS\USHADOW.ILB) and corpse (UNITS\DEADUNIT.ILB) libraries. Each frame
    // of this ILB is a 64x64 sprite; the first entry is the idle pose.
    std::string sprite_lib;
    // The parsed tag-5 `TImageSequenceList` - the unit's animation poses. Every
    // shipped unit has 31 entries laid out `[0..5] standing dir 1..6,
    // [6..11] walking dir 1..6, ...`; see aow/unit_anim.hpp + docs/16_UNIT_ANIM.md.
    std::vector<ImageSequence> sequences;
    // The unit-gfx resource's rw tag 10 (`TUnitGFXResource+0x30`) - the per-hex
    // animation-frame count class (`TUnitHS.GetMoveSteps@0x55784988`).
    std::uint8_t move_class = 2;
};

// One fully-resolved unit (a Unitres.pfs record joined to its sprite + race).
struct UnitEntry {
    std::size_t index = 0;         // position in Unitres.pfs
    std::uint32_t id = 0;          // record tag (sparse unit id)
    std::string name;              // tag 10 e.g. "Human"
    std::string qualifier;         // tag 11 e.g. "Archer"
    std::string display_name;      // "Human Archer" (GetUnitName @ 0x55784da8)
    std::uint32_t res_id = 0;      // tag 1 (TEResource id/hash)
    std::uint32_t res_index = 0;   // tag 3 (TEResource index)
    std::uint8_t race_id = 0;      // tag 13
    std::uint8_t attack = 0;       // tag 14
    std::uint8_t defense = 0;      // tag 15
    std::uint8_t damage = 0;       // tag 16
    std::uint8_t hit_points = 0;   // tag 17
    std::uint8_t moves = 0;        // tag 18
    std::uint8_t resistance = 0;   // tag 19
    std::uint8_t level = 0;        // tag 20
    std::uint8_t unit_type = 0;    // tag 21
    std::uint8_t gender = 0;       // tag 23
    std::uint8_t alignment = 0;    // tag 12
    std::uint8_t unit_size = 0;    // tag 28
    std::uint8_t blood_type = 0;   // tag 29
    std::uint8_t transport_capacity = 0;  // tag 24
    std::uint32_t obtain_value = 0;       // tag 27
    std::string description;              // tag 26
    std::uint32_t gfx_index = 0;          // tag 8 (join key)
    const UnitGraphics* gfx = nullptr;    // resolved sprite (nullptr if missing)
    std::string race_name;                // resolved race label
    std::string race_lib;                 // RACES\R*.ILB (empty for monsters)
};

// One `.ahl` leader/special unit list.
struct LeaderList {
    std::string file;             // base file name, e.g. "High Level.ahl"
    std::string name;             // embedded label, e.g. "High Level"
    std::uint16_t tag14 = 0;      // header u16 at +4 (observed 0x14 = 20)
    std::vector<std::uint8_t> raw;  // the whole file
};

// The whole roster of the game.
struct UnitCatalog {
    std::string root;                     // the game root that was loaded
    std::vector<UnitEntry> units;         // every unit, in Unitres.pfs order
    std::vector<UnitGraphics> graphics;   // every Unitgfx.pfs record
    std::vector<LeaderList> leader_lists; // the shipped *.ahl files
    // Herores.pfs: THeroResource record-list tag -> Unitgfx gfx_index. On the
    // world map a leading hero/leader unit (map classes 0x20230/0x20232) carries
    // a *hero-resource* index (its rw tag 8), not a unit-resource tag, so the
    // army's sprite is resolved through this table. Evidence:
    // AoWE.THero.GetUnitGFXResourceIndex@557875bc (resource+0x18) +
    // docs/13_UNITS_ON_MAP.md.
    std::map<std::uint32_t, std::uint32_t> hero_gfx;
    // Herores.pfs: THeroResource record-list tag -> the hero type's per-turn
    // movement allowance (tag 19, +0x28 = SetUnitMoves +0x6e). A placed hero
    // refills its move points from this at turn start; see docs/15_MOVEMENT.md.
    std::map<std::uint32_t, std::uint8_t> hero_moves;
    std::size_t gfx_missing = 0;          // units whose gfx_index found nothing

    // The sprite for a UnitResource.gfx_index (matches UnitGraphics.res_index).
    const UnitGraphics* find_gfx(std::uint32_t gfx_index) const;
    // The sprite for a hero/leader resource index (a Herores.pfs record tag),
    // or nullptr when unknown.
    const UnitGraphics* find_hero_gfx(std::uint32_t hero_index) const;
    // Units whose race_id == id (use 255 for monsters).
    std::vector<const UnitEntry*> units_of_race(std::uint8_t race_id) const;
};

// Loads <root>/Release/{Unitres.pfs, Unitgfx.pfs, Release.hss, *.ahl}.
// Returns false (and sets 'error') only when the two .PFS files can't be read;
// a missing Release.hss / .ahl is non-fatal (races/leader lists stay empty).
bool load_unit_catalog(const std::string& root, UnitCatalog& out,
                       std::string& error);

// Human-readable race label for a race id (docs/01_FORMATS.md table); unknown
// ids render as "race<N>".
std::string race_label(std::uint8_t race_id);

// The race's *plural* label - what the Events banner's `Race` label shows and
// what `TRaceResource`/`TPlayer` messages use. `Engine.TPlayer`-side code reads
// `AoWE.RaceNamesPluralRStr[player.race]` (a resourcestring pointer array;
// e.g. AoWEPACK 0x55751848 / 0x55751c00, and FUN_00423170 = the Events floater's
// update, which sets the label from it). The shipped strings live in
// AoWEPACK.dpl's RT_STRING table: 65256 "Humans", 65258 "Azracs", 65260
// "Lizards", 65262 "Frostlings", 65232 "Elves", 65234 "Halflings", 65236
// "Dwarves", 65238 "Highmen", 65240 "Dark Elves", 65242 "Orcs", 65244 "Goblins",
// 65246 "Undead" - dumped with tools/pe_rsrc_strings.py.
std::string race_label_plural(std::uint8_t race_id);

}  // namespace aow
