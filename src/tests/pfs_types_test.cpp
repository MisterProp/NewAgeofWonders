// pfs_types_test.cpp - Stage 1 rule-fidelity tests for the typed .PFS layer.
//
// Loads the shipped Release/*.PFS and asserts record counts plus a handful of
// values visible in-game (record text, names, unit stats).
//
// NOTE: the counts/values below are baselined against the ORIGINAL Age of
// Wonders 1 data (this GOG install). This repo originally targeted the "Age of
// Wonders X" mod, whose tables are larger/different; those old numbers were
// removed here. Usage:
//
//   aow_tests [dir-with-PFS]        (default: ..\..\Release)
//
// Exits 0 when every check passes, 1 otherwise. stdlib only.
#include <cstdio>
#include <string>
#include <vector>

#include "aow/pfs.hpp"
#include "aow/pfs_types.hpp"

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

bool contains(const std::string& s, const char* sub) {
    return s.find(sub) != std::string::npos;
}

std::string join(const char* dir, const char* file) {
    std::string d = dir;
    if (!d.empty() && d.back() != '/' && d.back() != '\\') d += '\\';
    return d + file;
}

bool load(const char* dir, const char* file, aow::PfsFile& pfs) {
    std::string err;
    const std::string p = join(dir, file);
    if (!pfs.load(p, err)) {
        std::fprintf(stderr, "FAIL: cannot load %s (%s)\n", p.c_str(),
                     err.c_str());
        ++g_fail;
        return false;
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    const char* dir = (argc > 1) ? argv[1] : "..\\..\\Release";
    aow::PfsFile pfs;

    // --- Ability.pfs (147) --------------------------------------------------
    if (load(dir, "Ability.pfs", pfs)) {
        const std::vector<aow::Ability> a = aow::load_abilities(pfs);
        check(a.size() == 147, "Ability.pfs records == 147");
        if (a.size() >= 2) {
            check(contains(a[0].description, "walk upon normal terrain"),
                  "ability[0] is the walk-terrain description");
            check(contains(a[1].description, "fly over and across"),
                  "ability[1] is the Flying description");
            check(a[1].cost == 30, "ability[1] (Flying) cost == 30");
            check(a[1].move_types == 0x21,
                  "ability[1] (Flying) move_types == 0x21");
        }
    }

    // --- Spells.pfs (108) ---------------------------------------------------
    if (load(dir, "Spells.pfs", pfs)) {
        const std::vector<aow::Spell> s = aow::load_spells(pfs);
        check(s.size() == 108, "Spells.pfs records == 108");
        if (!s.empty()) {
            check(contains(s[0].description, "wide area"),
                  "spell[0] is a wide-area description");
            check(s[0].research_cost == 70, "spell[0] research_cost == 70");
            check(s[0].mana_cost == 5, "spell[0] mana_cost == 5");
        }
    }

    // --- Unitres.pfs (142) --------------------------------------------------
    if (load(dir, "Unitres.pfs", pfs)) {
        const std::vector<aow::UnitStat> u = aow::load_units(pfs);
        check(u.size() == 142, "Unitres.pfs records == 142");
        if (!u.empty()) {
            const aow::UnitStat& r = u[0];  // "Human Archer"
            check(r.name == "Human", "unit[0] name == Human");
            check(r.qualifier == "Archer", "unit[0] qualifier == Archer");
            check(r.attack == 3, "unit[0] attack == 3");
            check(r.defense == 2, "unit[0] defense == 2");
            check(r.damage == 1, "unit[0] damage == 1");
            check(r.hit_points == 5, "unit[0] hit_points == 5");
            check(r.moves == 26, "unit[0] moves == 26");
            check(r.resistance == 3, "unit[0] resistance == 3");
            check(r.level == 1, "unit[0] level == 1");
            check(r.race_id == 0, "unit[0] race_id == 0 (Human)");
            check(r.obtain_value == 15, "unit[0] obtain_value == 15");
            check(contains(r.description, "human archers"),
                  "unit[0] description mentions human archers");
        }
    }

    // --- ITEMS.PFS (83) -----------------------------------------------------
    if (load(dir, "ITEMS.PFS", pfs)) {
        const std::vector<aow::Item> it = aow::load_items(pfs);
        check(it.size() == 83, "ITEMS.PFS records == 83");
        if (!it.empty()) {
            check(it[0].name == "Ankh of Turning", "item[0] name == Ankh of Turning");
            check(it[0].gfx_index == 314, "item[0] gfx_index == 314");
            check(it[0].obtain_value == 140, "item[0] obtain_value == 140");
        }
    }

    // --- HEROES.PFS (50) ----------------------------------------------------
    if (load(dir, "HEROES.PFS", pfs)) {
        const std::vector<aow::Hero> h = aow::load_heroes(pfs);
        check(h.size() == 50, "HEROES.PFS records == 50");
        if (!h.empty()) {
            check(h[0].name == "Conan", "hero[0] name == Conan");
            check(h[0].nickname == "McHarr", "hero[0] nickname == McHarr");
            check(h[0].face_index == 10, "hero[0] face_index == 10");
        }
    }

    // --- HERORES.PFS (36) ---------------------------------------------------
    if (load(dir, "HERORES.PFS", pfs)) {
        const std::vector<aow::HeroResource> h = aow::load_hero_resources(pfs);
        check(h.size() == 36, "HERORES.PFS records == 36");
        if (!h.empty()) {
            check(h[0].name == "Human", "herores[0] name == Human");
        }
    }

    // --- Unitgfx.pfs (182) --------------------------------------------------
    if (load(dir, "Unitgfx.pfs", pfs)) {
        const std::vector<aow::UnitGfx> g = aow::load_unit_gfx(pfs);
        check(g.size() == 182, "Unitgfx.pfs records == 182");
        if (!g.empty()) {
            check(g[0].name == "Human Archer", "unitgfx[0] name == Human Archer");
        }
    }

    // --- FACERES.PFS (89) ---------------------------------------------------
    if (load(dir, "FACERES.PFS", pfs)) {
        const std::vector<aow::FaceResource> f = aow::load_faces(pfs);
        check(f.size() == 89, "FACERES.PFS records == 89");
        if (!f.empty()) {
            check(f[0].image_lib == "FACES\\L_FACES.ILB",
                  "face[0] image_lib == FACES\\L_FACES.ILB");
        }
    }

    // --- ITEMGFX.PFS (123) --------------------------------------------------
    if (load(dir, "ITEMGFX.PFS", pfs)) {
        const std::vector<aow::ItemGfx> g = aow::load_item_gfx(pfs);
        check(g.size() == 123, "ITEMGFX.PFS records == 123");
        if (!g.empty()) {
            check(g[0].image_lib == "ITEMS\\I_ATTACK.ILB",
                  "itemgfx[0] image_lib == ITEMS\\I_ATTACK.ILB");
        }
    }

    // --- TEXT.pfs (91) ------------------------------------------------------
    if (load(dir, "TEXT.pfs", pfs)) {
        const std::vector<aow::TextItem> t = aow::load_texts(pfs);
        check(t.size() == 91, "TEXT.pfs records == 91");
        if (!t.empty()) {
            check(t[0].id == 10, "text[0] id == 10");
            check(contains(t[0].name, "Campaign Join"),
                  "text[0] is a campaign-join text");
        }
    }

    // --- comb_res.pfs (581) -------------------------------------------------
    if (load(dir, "comb_res.pfs", pfs)) {
        const std::vector<aow::TerrainRes> t = aow::load_terrain(pfs);
        check(t.size() == 581, "comb_res.pfs records == 581");
    }

    std::printf("pfs_types_test: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
