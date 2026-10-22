// unit_catalog.cpp - assemble the game's whole unit roster (see the header).
#include "aow/unit_catalog.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>

#include "aow/endian.hpp"
#include "aow/map_resources.hpp"
#include "aow/pfs.hpp"
#include "aow/pfs_types.hpp"

namespace aow {
namespace {

std::string join(const std::string& dir, const std::string& leaf) {
    if (dir.empty()) return leaf;
    const char last = dir.back();
    if (last == '/' || last == '\\') return dir + leaf;
    return dir + "\\" + leaf;
}

std::vector<std::uint8_t> read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return {};
    return std::vector<std::uint8_t>((std::istreambuf_iterator<char>(in)),
                                     std::istreambuf_iterator<char>());
}

// The on-map unit sprite library is embedded in the Unitgfx tag-5
// TImageSequenceList as a plain string, e.g. "UNITS\HMN\HMNAR.ILB". The engine
// serialises the unit's own library first, then the shared shadow
// (UNITS\USHADOW.ILB) and corpse (UNITS\DEADUNIT.ILB) libraries, so the first
// "UNITS\...ILB" string is the one a unit is drawn with.
std::string first_units_ilb(const std::vector<std::uint8_t>& b) {
    static const char kNeedle[] = "UNITS\\";
    for (std::size_t i = 0; i + 6 <= b.size(); ++i) {
        if (std::memcmp(b.data() + i, kNeedle, 6) != 0) continue;
        std::size_t j = i;
        while (j < b.size() && b[j] >= 32 && b[j] < 127) ++j;
        const std::string s(reinterpret_cast<const char*>(b.data() + i), j - i);
        if (s.size() > 4 && s.compare(s.size() - 4, 4, ".ILB") == 0) return s;
    }
    return {};
}

// A `.ahl` is a tiny engine record: `u32 0x02000001 | u16 tag14 | u8 1 |
// u8 len+1 | u8 len | name (len bytes) | NUL`. Verified on High Level.ahl /
// Special.ahl (docs/01_FORMATS.md, AHL section). Only the name is interpreted.
LeaderList parse_ahl(const std::string& file, std::vector<std::uint8_t> raw) {
    LeaderList l;
    l.file = file;
    l.raw = std::move(raw);
    if (l.raw.size() >= 9) {
        l.tag14 = read_u16(l.raw.data() + 4);
        const std::size_t len = l.raw[8];
        if (9 + len <= l.raw.size()) {
            l.name.assign(reinterpret_cast<const char*>(l.raw.data() + 9), len);
        }
    }
    return l;
}

}  // namespace

// The order the shipped data uses (Unitres.pfs `race_id` == the position in the
// game pack's TRaceResourceList, whose tag-8 libraries are R{HUMAN,AZRAC,
// LIZARDS,FROSTL,ELF,HALFLNG,DWARF,HIGHMAN,DARKELF,ORC,GOBLIN,UNDEAD}.ILB).
// Verified by sweeping the roster (aow roster) against each race's body library.
std::string race_label(std::uint8_t race_id) {
    switch (race_id) {
        case 0: return "Human";
        case 1: return "Azrac";
        case 2: return "Lizardman";
        case 3: return "Frostling";
        case 4: return "Elf";
        case 5: return "Halfling";
        case 6: return "Dwarf";
        case 7: return "Highman";
        case 8: return "Dark Elf";
        case 9: return "Orc";
        case 10: return "Goblin";
        case 11: return "Undead";
        case 255: return "Monster";
        default: return "race" + std::to_string(race_id);
    }
}

// The shipped plural names (AoWEPACK.dpl resourcestrings - see the header). The
// index is the same race id `race_label` uses (a Unitres.pfs `race_id` == the
// position in the game pack's TRaceResourceList), which is also the index
// `AoWE.RaceNamesPluralRStr` is read with. Race 11's plural is literally
// "Undead(plural)" in the shipped string table (id 65246), so the banner shows
// that placeholder exactly as the original does.
std::string race_label_plural(std::uint8_t race_id) {
    switch (race_id) {
        case 0: return "Humans";
        case 1: return "Azracs";
        case 2: return "Lizards";
        case 3: return "Frostlings";
        case 4: return "Elves";
        case 5: return "Halflings";
        case 6: return "Dwarves";
        case 7: return "Highmen";
        case 8: return "Dark Elves";
        case 9: return "Orcs";
        case 10: return "Goblins";
        case 11: return "Undead(plural)";
        case 255: return "Monsters";
        default: return "race" + std::to_string(race_id);
    }
}

// The shipped singular race names - `AoWE.RaceNamesRStr[race]`, the string the
// Structure floater's CityRaceLbl formats with "Race: %s" (see the header). The
// resourcestring ids are AoWEPACK.dpl's RT_STRING table (tools/pe_rsrc_strings.py
// and the surrounding blocks show the singular/plural pairs):
//
//   65255 Human / 65256 Humans      65257 Azrac / 65258 Azracs
//   65259 Lizard / 65260 Lizards    65261 Frostling / 65262 Frostlings
//   65263 Elf / 65232 Elves         65233 Halfling / 65234 Halflings
//   65235 Dwarf / 65236 Dwarves     65237 Highman / 65238 Highmen
//   65239 DarkElf / 65240 Dark Elves 65241 Orc / 65242 Orcs
//   65243 Goblin / 65244 Goblins    65245 Undead / 65246 Undead(plural)
std::string race_label_singular(std::uint8_t race_id) {
    switch (race_id) {
        case 0: return "Human";
        case 1: return "Azrac";
        case 2: return "Lizard";
        case 3: return "Frostling";
        case 4: return "Elf";
        case 5: return "Halfling";
        case 6: return "Dwarf";
        case 7: return "Highman";
        case 8: return "DarkElf";
        case 9: return "Orc";
        case 10: return "Goblin";
        case 11: return "Undead";
        default: return "race" + std::to_string(race_id);
    }
}

const UnitGraphics* UnitCatalog::find_gfx(std::uint32_t gfx_index) const {
    for (const UnitGraphics& g : graphics) {
        if (g.res_index == gfx_index) return &g;
    }
    return nullptr;
}

const UnitGraphics* UnitCatalog::find_hero_gfx(std::uint32_t hero_index) const {
    const auto it = hero_gfx.find(hero_index);
    if (it == hero_gfx.end()) return nullptr;
    return find_gfx(it->second);
}

std::vector<const UnitEntry*> UnitCatalog::units_of_race(
    std::uint8_t race_id) const {
    std::vector<const UnitEntry*> out;
    for (const UnitEntry& u : units) {
        if (u.race_id == race_id) out.push_back(&u);
    }
    return out;
}

bool load_unit_catalog(const std::string& root, UnitCatalog& out,
                       std::string& error) {
    out = UnitCatalog{};
    out.root = root;
    const std::string release = join(root, "Release");

    // --- Unitres.pfs -> all unit stats --------------------------------------
    PfsFile unitres;
    if (!unitres.load(join(release, "Unitres.pfs"), error)) return false;
    const std::vector<UnitStat> stats = load_units(unitres);
    if (stats.empty()) {
        error = "Unitres.pfs contains no units: " + join(release, "Unitres.pfs");
        return false;
    }

    // --- Unitgfx.pfs -> all sprites -----------------------------------------
    PfsFile unitgfx;
    if (!unitgfx.load(join(release, "Unitgfx.pfs"), error)) return false;
    const std::vector<UnitGfx> gfx = load_unit_gfx(unitgfx);

    out.graphics.reserve(gfx.size());
    for (const UnitGfx& g : gfx) {
        UnitGraphics ug;
        ug.index = g.index;
        ug.res_index = g.res_index;
        ug.name = g.name;
        ug.image_lib = g.image_lib;
        ug.face_index = g.gfx_index;
        ug.frames_size = g.sequence.size();
        ug.sprite_lib = first_units_ilb(g.sequence);
        ug.move_class = g.value10;
        // The tag-5 blob is the unit's `TImageSequenceList` - the animation
        // poses TUnitHS.Show picks from. A blob that does not parse leaves the
        // list empty; every placed unit still draws its idle frame.
        std::string serr;
        (void)parse_image_sequence_list(g.sequence.data(), g.sequence.size(),
                                        ug.sequences, serr);
        out.graphics.push_back(std::move(ug));
    }

    // --- Herores.pfs -> hero/leader sprite indices --------------------------
    // A world-map hero/leader unit (map classes 0x20230 THero / 0x20232 TLeader)
    // resolves its sprite through the *hero* resource, not a unit resource, so
    // keep a hero-resource tag -> Unitgfx gfx_index map. A missing Herores.pfs
    // is non-fatal (hero armies then simply draw no sprite).
    {
        std::string herr;
        PfsFile herores;
        if (herores.load(join(release, "Herores.pfs"), herr)) {
            for (const HeroResource& h : load_hero_resources(herores)) {
                out.hero_gfx.emplace(h.tag, h.gfx_index);
                // THeroResource tag 19 (+0x28, SetUnitMoves +0x6e) is the hero
                // type's per-turn movement allowance; a map hero refills its
                // TUnit+0x79 move points from it at turn start
                // (TAbstractUnit.NewTurn@0x55780d4c). See docs/15_MOVEMENT.md.
                out.hero_moves.emplace(h.tag, h.moves_skill);
            }
        }
    }

    // --- Release.hss -> race body libraries ---------------------------------
    std::vector<std::string> race_libs;
    std::vector<MapResource> mapres;
    std::string ignored;
    if (load_map_resources(join(release, "Release.hss"), mapres, ignored)) {
        race_libs = race_image_libraries(mapres);
    }

    // --- *.ahl leader/special unit lists ------------------------------------
    std::error_code ec;
    std::vector<std::string> ahl_files;
    for (const std::filesystem::directory_entry& de :
         std::filesystem::directory_iterator(release, ec)) {
        if (!de.is_regular_file(ec)) continue;
        const std::string ext = de.path().extension().string();
        if (ext == ".ahl" || ext == ".AHL") {
            ahl_files.push_back(de.path().filename().string());
        }
    }
    std::sort(ahl_files.begin(), ahl_files.end());
    for (const std::string& f : ahl_files) {
        out.leader_lists.push_back(parse_ahl(f, read_file(join(release, f))));
    }

    // --- join ---------------------------------------------------------------
    out.units.reserve(stats.size());
    for (const UnitStat& s : stats) {
        UnitEntry u;
        u.index = s.index;
        u.id = s.tag;
        u.name = s.name;
        u.qualifier = s.qualifier;
        u.display_name = s.qualifier.empty() ? s.name : s.name + " " + s.qualifier;
        u.res_id = s.res_id;
        u.res_index = s.res_index;
        u.race_id = s.race_id;
        u.attack = s.attack;
        u.defense = s.defense;
        u.damage = s.damage;
        u.hit_points = s.hit_points;
        u.moves = s.moves;
        u.resistance = s.resistance;
        u.level = s.level;
        u.unit_type = s.extra21;   // tag 21 == unit_type (GetUnitType +0x30)
        u.gender = s.extra23;      // tag 23 == gender     (GetGender   +0x31)
        u.alignment = s.type;      // tag 12 == alignment  (GetAlignment +0x28)
        u.unit_size = s.extra28;   // tag 28 (GetUnitSize +0x50)
        u.blood_type = s.extra29;  // tag 29 (GetBloodType +0x51)
        u.transport_capacity = s.transport_capacity;
        u.obtain_value = s.obtain_value;
        u.description = s.description;
        u.gfx_index = s.gfx_index;
        u.gfx = out.find_gfx(s.gfx_index);
        if (u.gfx == nullptr) ++out.gfx_missing;
        u.race_name = race_label(s.race_id);
        if (s.race_id < race_libs.size()) u.race_lib = race_libs[s.race_id];
        out.units.push_back(std::move(u));
    }
    return true;
}

}  // namespace aow
