// map_test.cpp - Stage 2.1 format tests for the scenario/campaign map loader.
//
// Loads every `.HSM` / `.CSM` under Scenario/Campaign and Scenario/Custom and
// asserts the structural invariants recovered in docs/01_FORMATS.md:
//   * the top object is AoWE.TAoWHSMap (class 0x20011),
//   * the level count matches the container,
//   * every level has exactly width*height tiles (no desync),
//   * every tile carries a terrain hexagon (field-table tag 50).
//
//   aow_map_tests [game-root]        (default: ..)
//
// Exits 0 when every check passes. stdlib only.
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "aow/map_types.hpp"

namespace fs = std::filesystem;

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

}  // namespace

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..";

    std::vector<fs::path> files;
    for (const char* sub : {"Scenario/Campaign", "Scenario/Custom"}) {
        const fs::path d = fs::path(root) / sub;
        if (!fs::exists(d)) continue;
        for (const auto& e : fs::directory_iterator(d)) {
            if (!e.is_regular_file()) continue;
            std::string ext = e.path().extension().string();
            for (char& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (ext == ".hsm" || ext == ".csm") files.push_back(e.path());
        }
    }
    // The shipped GOG install carries 72 maps (41 campaign + 31 custom); the
    // original retail build had 144. Just assert we found the scenario set.
    check(files.size() >= 40, "found the scenario map set (>=40 maps)");

    std::size_t total_maps = 0, total_tiles = 0, total_objects = 0;
    std::size_t tiles_without_terrain = 0;
    std::size_t maps_with_playlist = 0;
    std::map<std::uint32_t, std::size_t> class_hist;

    for (const fs::path& f : files) {
        aow::Map m;
        std::string err;
        if (!aow::load_map(f.string(), m, err)) {
            check(false, "load " + f.string() + " (" + err + ")");
            continue;
        }
        ++total_maps;
        if (m.class_id != aow::kAoWHSMapClassId) {
            check(false, f.filename().string() + " top class is TAoWHSMap");
        }
        if (m.levels.size() != m.level_count) {
            check(false, f.filename().string() + " level count matches container");
        }
        if (m.width == 0 || m.height == 0) {
            check(false, f.filename().string() + " has non-zero dimensions");
        }
        if (!m.songs_startup.empty() || !m.songs_list.empty()) {
            ++maps_with_playlist;
        }
        for (const aow::MapLevel& lv : m.levels) {
            if (lv.tiles.size() !=
                static_cast<std::size_t>(lv.width) * lv.height) {
                check(false, f.filename().string() + " level tiles == width*height");
            }
            if (lv.width != m.width || lv.height != m.height) {
                check(false, f.filename().string() + " level dims match container");
            }
            for (const aow::MapTile& t : lv.tiles) {
                bool has_terrain = false;
                for (const aow::MapHs& o : t.objects) {
                    class_hist[o.class_id]++;
                    if (o.tag == 50) {
                        has_terrain = true;
                    } else {
                        ++total_objects;
                    }
                }
                if (!has_terrain) ++tiles_without_terrain;
                ++total_tiles;
            }
        }
    }

    check(total_tiles > 500000, "decoded >500k tiles across the map set");
    check(tiles_without_terrain == 0, "every tile has a terrain hexagon");
    check(maps_with_playlist == total_maps,
          "every map carries a song playlist (tag 0x1d or 0x3c)");
    check(class_hist.count(0x20152) > 0, "Hexagon.THexagon terrain seen");
    check(class_hist.count(0x20170) > 0, "IsoHex.TUpperIsometricHexagon terrain seen");
    check(class_hist.count(0x20557) > 0, "BorderHx.TBorderHexagon terrain seen");
    check(class_hist.count(0x2073d) > 0, "AoWHex.TAoWWaterHexagon (water) seen");
    check(class_hist.count(0x20108) > 0, "City.TCity object seen");
    check(class_hist.count(0x20217) > 0, "AoWE.TArmyHS (armies) object seen");
    check(class_hist.count(0x20168) > 0, "Mountain.TMountainMO object seen");
    check(aow::map_class_name(0x20108) != nullptr &&
              std::string(aow::map_class_name(0x20108)) == "City.TCity",
          "map_class_name(0x20108) == City.TCity");
    check(aow::map_class_name(0x20011) != nullptr &&
              std::string(aow::map_class_name(0x20011)) == "AoWE.TAoWHSMap",
          "map_class_name(0x20011) == AoWE.TAoWHSMap");

    // A couple of exact maps visible in-game.
    {
        aow::Map m;
        std::string err;
        const std::string p = (fs::path(root) / "Scenario/Campaign/Dwarf1.CSM").string();
        if (aow::load_map(p, m, err)) {
            check(m.width == 96 && m.height == 96, "Dwarf1.CSM is 96x96");
            check(m.levels.size() == 3, "Dwarf1.CSM has 3 levels");
            check(m.levels[0].tiles.size() == 96 * 96, "Dwarf1 level 0 has 9216 tiles");
        } else {
            check(false, "load Dwarf1.CSM (" + err + ")");
        }
    }
    {
        aow::Map m;
        std::string err;
        const std::string p = (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        if (aow::load_map(p, m, err)) {
            check(m.width == 48 && m.height == 48, "Tutorial.hsm is 48x48");
            check(m.levels.size() == 2, "Tutorial.hsm has 2 levels");
            // The scenario playlist: tag 0x1d holds the two songs, tag 0x3c is
            // present but empty (AoW.exe FUN_00454d2c then plays tag 0x1d).
            const std::vector<std::string> want = {"SONGS\\CLOUDS.IT",
                                                   "SONGS\\REMARCH.IT"};
            check(m.songs_startup == want, "Tutorial.hsm tag-0x1d playlist");
            check(m.songs_list.empty(), "Tutorial.hsm tag-0x3c list is empty");
        } else {
            check(false, "load Tutorial.hsm (" + err + ")");
        }
    }
    {
        aow::Map m;
        std::string err;
        const std::string p = (fs::path(root) / "Scenario/Campaign/Darkelf1.csm").string();
        if (aow::load_map(p, m, err)) {
            const std::vector<std::string> want = {
                "SONGS\\RUINS.IT", "SONGS\\THROUGH.IT", "SONGS\\TRUTH.IT",
                "SONGS\\CARVED.IT"};
            check(m.songs_startup == want, "Darkelf1.csm tag-0x1d playlist");
            check(m.songs_list.empty(), "Darkelf1.csm has no tag-0x3c list");
        } else {
            check(false, "load Darkelf1.csm (" + err + ")");
        }
    }

    // Placed armies + units (docs/13_UNITS_ON_MAP.md). Every map that carries an
    // AoWE.TArmyHS must parse its embedded AoWE.TArmy into a non-empty unit list,
    // and each unit must resolve to a sprite tag (TUnit rw tag 10 / hero tag 8).
    {
        std::size_t total_armies = 0, total_units = 0, empty_armies = 0;
        std::size_t unresolved = 0, heroes = 0, named_heroes = 0;
        for (const fs::path& f : files) {
            aow::Map m;
            std::string err;
            if (!aow::load_map(f.string(), m, err)) continue;
            for (const aow::MapLevel& lv : m.levels) {
                for (const aow::MapTile& t : lv.tiles) {
                    for (const aow::MapHs& hs : t.objects) {
                        if (hs.class_id != aow::kMapArmyHsClassId) continue;
                        ++total_armies;
                        if (hs.units.empty()) ++empty_armies;
                        for (const aow::MapUnit& u : hs.units) {
                            ++total_units;
                            if (u.is_hero) {
                                ++heroes;
                                if (!u.name.empty()) ++named_heroes;
                            }
                            // A plain TUnit must resolve a resource tag; a hero
                            // may be -1 (its THeroResource is unset - the engine
                            // assigns the race at load).
                            if (!u.is_hero && u.resource < 0) ++unresolved;
                            const bool known = u.class_id == aow::kMapUnitClassId ||
                                               u.class_id == aow::kMapHeroClassId ||
                                               u.class_id == aow::kMapLeaderClassId;
                            if (!known) ++unresolved;
                        }
                    }
                }
            }
        }
        check(total_armies > 0, "scenario maps carry AoWE.TArmyHS armies");
        check(empty_armies == 0, "every parsed army has a non-empty unit list");
        check(total_units >= total_armies,
              "every army carries at least its lead unit");
        check(unresolved == 0, "every placed unit resolves to a sprite tag");
        check(heroes > 0, "hero-led armies were parsed (THero/TLeader)");
        check(named_heroes == heroes,
              "every placed hero carries a name (THero rw tag 10)");
    }
    {
        aow::Map m;
        std::string err;
        const std::string p = (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        if (aow::load_map(p, m, err)) {
            const aow::MapTile& t = m.levels[0].tiles[16 * m.levels[0].width + 33];
            const aow::MapHs* army = nullptr;
            for (const aow::MapHs& hs : t.objects) {
                if (hs.class_id == aow::kMapArmyHsClassId) army = &hs;
            }
            check(army != nullptr, "Tutorial.hsm hex (33,16) holds an army");
            if (army != nullptr) {
                check(army->units.size() == 4, "Tutorial.hsm (33,16) has 4 units");
                check(!army->units.empty() &&
                          army->units.front().class_id == aow::kMapUnitClassId &&
                          army->units.front().resource == 113,
                      "Tutorial.hsm (33,16) leads with the Giant (TUnitResource 113)");
            }
            // The hero-led army at (32,21) must fully load its THero record.
            const aow::MapTile& ht =
                m.levels[0].tiles[21 * m.levels[0].width + 32];
            const aow::MapHs* harmy = nullptr;
            for (const aow::MapHs& hs : ht.objects) {
                if (hs.class_id == aow::kMapArmyHsClassId) harmy = &hs;
            }
            check(harmy != nullptr && !harmy->units.empty() &&
                      harmy->units.front().is_hero,
                  "Tutorial.hsm (32,21) leads with a hero");
            if (harmy != nullptr && !harmy->units.empty()) {
                const aow::MapUnit& h = harmy->units.front();
                check(h.class_id == aow::kMapHeroClassId && h.name == "Doktok" &&
                          h.nickname == "the Fat" && h.resource == 9,
                      "Tutorial.hsm hero loads as 'Doktok the Fat' (THeroResource 9)");
            }
        } else {
            check(false, "load Tutorial.hsm for the unit check (" + err + ")");
        }
    }

    // City names (docs/25_STRUCTURE_PANEL.md). `City.TCity.ReadWrite@557acbcc`
    // streams +0x48 - the string `TCity.GetName@557ac918` hands to the Structure
    // floater's StructNameLbl - at **rw tag 0xc** through rwString: one length
    // byte (0xff => a u32 length), then the bytes. Reading it as a u32 length
    // (the first four name bytes!) is what made every city show "City".
    {
        std::size_t cities = 0, nameless = 0, nonprintable = 0;
        std::set<std::string> names;
        for (const fs::path& f : files) {
            aow::Map m;
            std::string err;
            if (!aow::load_map(f.string(), m, err)) continue;
            for (const aow::MapLevel& lv : m.levels) {
                for (const aow::MapTile& t : lv.tiles) {
                    for (const aow::MapHs& hs : t.objects) {
                        if (hs.class_id != aow::kCityClassId) continue;
                        ++cities;
                        if (hs.city_name.empty()) {
                            ++nameless;
                            continue;
                        }
                        names.insert(hs.city_name);
                        for (unsigned char c : hs.city_name) {
                            if (c < 0x20) ++nonprintable;
                        }
                    }
                }
            }
        }
        check(cities > 2000, "scenario maps carry >2000 City.TCity objects");
        check(nameless == 0,
              "every placed city carries its rw tag 0xc name");
        check(nonprintable == 0, "city names carry no control bytes (rwString)");
        check(names.count("Venice ") == 1,
              "Bloody Marauders' Human capital is named 'Venice '");
    }
    {
        // The exact record of Bloody Marauders' Human capital.
        aow::Map m;
        std::string err;
        const std::string p =
            (fs::path(root) / "Scenario/Custom/Bloody Marauders.hsm").string();
        if (aow::load_map(p, m, err)) {
            std::vector<std::string> found;
            for (const aow::MapLevel& lv : m.levels) {
                for (const aow::MapTile& t : lv.tiles) {
                    for (const aow::MapHs& hs : t.objects) {
                        if (hs.class_id != aow::kCityClassId) continue;
                        found.push_back(hs.city_name);
                    }
                }
            }
            check(!found.empty() && found.front() == "Ricordia",
                  "Bloody Marauders' first city is 'Ricordia' (rw tag 0xc)");
            // TCity rw tag 0xc is the name verbatim: the capital's string is
            // "Venice " (a trailing space - the map's own bytes, and the label
            // shows whatever TCity.GetName returns).
            check(std::find(found.begin(), found.end(), "Venice ") !=
                      found.end(),
                  "Bloody Marauders carries 'Venice ' (rw tag 0xc)");
            check(std::find(found.begin(), found.end(), "Confinia Minor") !=
                      found.end(),
                  "multi-word city names survive ('Confinia Minor')");
        } else {
            check(false, "load Bloody Marauders.hsm (" + err + ")");
        }
    }

    std::printf("map_test: %zu maps, %zu tiles, %zu objects, %zu classes\n",
                total_maps, total_tiles, total_objects, class_hist.size());
    std::printf("map_test: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
