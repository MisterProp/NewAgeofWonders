// unit_catalog_test.cpp - "load every unit of the game" fidelity test.
//
// Loads <game-dir>/Release/{Unitres.pfs, Unitgfx.pfs, Release.hss, *.ahl}
// through aow::load_unit_catalog and checks the roster is complete and the
// unit<->sprite join is correct.
//
//   aow_unit_catalog_tests [game-dir]     (default: ..\..)
//
// Exits 0 when every check passes, 1 otherwise. stdlib only.
#include <cstdio>
#include <string>
#include <vector>

#include "aow/unit_catalog.hpp"

namespace {

int g_fail = 0;
int g_pass = 0;

void check(bool ok, const std::string& what) {
    if (ok) {
        ++g_pass;
    } else {
        ++g_fail;
        std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    }
}

const aow::UnitEntry* find_id(const aow::UnitCatalog& c, std::uint32_t id) {
    for (const aow::UnitEntry& u : c.units) {
        if (u.id == id) return &u;
    }
    return nullptr;
}

const aow::UnitEntry* find_name(const aow::UnitCatalog& c,
                                const std::string& display) {
    for (const aow::UnitEntry& u : c.units) {
        if (u.display_name == display) return &u;
    }
    return nullptr;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..\\..";
    aow::UnitCatalog cat;
    std::string err;
    if (!aow::load_unit_catalog(root, cat, err)) {
        std::fprintf(stderr, "FAIL: cannot load catalog from %s (%s)\n",
                     root.c_str(), err.c_str());
        return 1;
    }

    std::printf("root=%s\n", root.c_str());
    std::printf("  units=%zu (no sprite: %zu)  unitGfx=%zu  leaderLists=%zu\n",
                cat.units.size(), cat.gfx_missing, cat.graphics.size(),
                cat.leader_lists.size());

    // --- the roster is populated --------------------------------------------
    check(cat.units.size() >= 100, "catalog has >= 100 units");
    check(cat.graphics.size() >= 100, "catalog has >= 100 unit sprites");

    // --- first unit is the Human Archer -------------------------------------
    check(!cat.units.empty() && cat.units.front().id == 0,
          "unit[0].id == 0");
    check(!cat.units.empty() && cat.units.front().display_name == "Human Archer",
          "unit[0] == Human Archer");

    // --- the units<->sprites join (gfx_index -> UnitGfx.res_index) ----------
    const aow::UnitEntry* archer = find_name(cat, "Human Archer");
    check(archer != nullptr, "found Human Archer by display name");
    if (archer != nullptr) {
        check(archer->gfx != nullptr, "Human Archer has a sprite");
        if (archer->gfx != nullptr) {
            check(archer->gfx->name == "Human Archer",
                  "Human Archer joins to sprite 'Human Archer'");
        }
    }
    const aow::UnitEntry* azrac = find_id(cat, 18);  // "Azrac Archer"
    check(azrac != nullptr, "found unit id 18");
    if (azrac != nullptr) {
        check(azrac->display_name == "Azrac Archer", "unit 18 == Azrac Archer");
        check(azrac->gfx != nullptr && azrac->gfx->name == "Azrac Archer",
              "unit 18 joins to sprite 'Azrac Archer' (by res_index)");
        // race_id 1 is Azrac (the data's race order, not alphabetical).
        check(azrac->race_name == "Azrac", "race_id 1 label == Azrac");
        check(azrac->race_lib.find("RAZRAC") != std::string::npos,
              "race_id 1 body == RACES\\RAZRAC.ILB");
    }

    // --- every unit has a name, and most have a sprite ----------------------
    std::size_t unnamed = 0;
    for (const aow::UnitEntry& u : cat.units) {
        if (u.display_name.empty()) ++unnamed;
    }
    check(unnamed == 0, "every unit has a display name");
    check(cat.gfx_missing * 4 <= cat.units.size() + 3,
          "at least ~3/4 of units have a resolved sprite");

    // --- races covered -------------------------------------------------------
    std::size_t with_race_lib = 0;
    for (const aow::UnitEntry& u : cat.units) {
        if (!u.race_lib.empty()) ++with_race_lib;
    }
    check(with_race_lib > 0, "some units resolved a RACES\\R*.ILB body");

    // --- the .ahl leader/special lists --------------------------------------
    check(cat.leader_lists.size() >= 2, "found >= 2 .ahl leader lists");
    bool high_level = false, special = false;
    for (const aow::LeaderList& l : cat.leader_lists) {
        if (l.name == "High Level") high_level = true;
        if (l.name == "Special") special = true;
    }
    check(high_level, ".ahl list 'High Level' present");
    check(special, ".ahl list 'Special' present");

    // --- the race labels (docs/09 sec. 4.1) ---------------------------------
    // `race_label` is the data's race order; `race_label_plural` is what the
    // Events banner's `Race` label shows - `AoWE.RaceNamesPluralRStr[race]`,
    // the plural resourcestrings in AoWEPACK.dpl (65246 is literally
    // "Undead(plural)"; the shipped placeholder is reproduced as-is).
    check(aow::race_label_plural(0) == "Humans", "plural race 0 == Humans");
    check(aow::race_label_plural(2) == "Lizards", "plural race 2 == Lizards");
    check(aow::race_label_plural(7) == "Highmen", "plural race 7 == Highmen");
    check(aow::race_label_plural(9) == "Orcs", "plural race 9 == Orcs");
    check(aow::race_label_plural(10) == "Goblins", "plural race 10 == Goblins");
    check(aow::race_label_plural(11) == "Undead(plural)",
          "plural race 11 == Undead(plural) (the shipped string)");

    // `race_label_singular` is `AoWE.RaceNamesRStr[race]`, the string the
    // Structure floater's CityRaceLbl shows ("Race: %s" = 64846). The ids are
    // the singular of each plural above (Elf: 65263, its plural "Elves" being
    // 65232), so every entry must be the shipped spelling - including the
    // missing space in "Highman"/"DarkElf".
    check(aow::race_label_singular(0) == "Human", "singular race 0 == Human");
    check(aow::race_label_singular(1) == "Azrac", "singular race 1 == Azrac");
    check(aow::race_label_singular(2) == "Lizard", "singular race 2 == Lizard");
    check(aow::race_label_singular(3) == "Frostling",
          "singular race 3 == Frostling");
    check(aow::race_label_singular(4) == "Elf",
          "singular race 4 == Elf (Heartwood's elven cities)");
    check(aow::race_label_singular(5) == "Halfling",
          "singular race 5 == Halfling");
    check(aow::race_label_singular(6) == "Dwarf", "singular race 6 == Dwarf");
    check(aow::race_label_singular(7) == "Highman",
          "singular race 7 == Highman (no space, as shipped)");
    check(aow::race_label_singular(8) == "DarkElf",
          "singular race 8 == DarkElf (no space, as shipped)");
    check(aow::race_label_singular(9) == "Orc", "singular race 9 == Orc");
    check(aow::race_label_singular(10) == "Goblin",
          "singular race 10 == Goblin");
    check(aow::race_label_singular(11) == "Undead", "singular race 11 == Undead");
    check(aow::race_label_singular(12) == "race12",
          "unknown singular race id keeps the race<N> fallback");

    std::printf("unit_catalog: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
