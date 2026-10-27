// mapres_test.cpp - Stage 2.2 tests for the world resource table and renderer.
//
//   * Release.hss exposes the (data-version dependent) TMapObjectResourceList
//     (~440 records in this GOG install; the docs' older "1142" was a
//     different data build, like the 188->142 unit counts),
//   * resource ids resolve to their ILB library names (id 1 -> EARTHHEX.ILB,
//     0x007b134f -> WATERHEX.ILB),
//   * the four terrain hexagon resource classes are present,
//   * map_resource_frames maps each (res_type, library) run's res_index to that
//     library's frame (H_gr.BMP 9..20 -> 0..11, H_Border.BMP 78..89 -> 0..11),
//   * a scenario renders to a non-empty PNG.
//
//   aow_mapres_tests [game-root]        (default: ..)
//
// Exits 0 when every check passes. stdlib only.
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "aow/ilb.hpp"
#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/mapviz.hpp"
#include "aow/movement.hpp"

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

bool is_terrain_class(std::uint32_t c) {
    return c == 0x20153 || c == 0x20171 || c == 0x20558 || c == 0x2073e;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..";
    const std::string hss = (fs::path(root) / "Release/Release.hss").string();

    std::vector<aow::MapResource> res;
    std::string err;
    if (!aow::load_map_resources(hss, res, err)) {
        check(false, "load resources (" + err + ")");
    } else {
        // The record count is data-version dependent (this install: ~440),
        // so assert a robust lower bound rather than the docs' stale 1142.
        check(res.size() >= 400, "resource list has >= 400 entries");

        const aow::MapResource* earth = aow::find_map_resource(res, 1);
        check(earth != nullptr && earth->ilb == "EARTHHEX.ILB",
              "resource id 1 resolves to EARTHHEX.ILB");
        check(earth != nullptr && earth->class_id == 0x20171,
              "resource id 1 is IsoHex.TUpperIsometricHexagonResource");
        const aow::MapResource* water = aow::find_map_resource(res, 0x007b134f);
        check(water != nullptr && water->ilb == "WATERHEX.ILB",
              "resource 0x007b134f resolves to WATERHEX.ILB");
        check(aow::find_map_resource(res, 0xdeadbeef) == nullptr,
              "an unknown id resolves to null");

        std::size_t terrain = 0, named = 0;
        for (const aow::MapResource& r : res) {
            if (is_terrain_class(r.class_id)) ++terrain;
            if (!r.ilb.empty()) ++named;
        }
        check(terrain >= 100, "terrain hexagon resources are present");
        check(named * 2 > res.size(), "most resources carry an ILB name");

        // map_resource_frames: each (res_type, library-stem) run maps res_index
        // to that library's frame. The terrain res_type 11 H_gr.BMP run 9..20 is
        // exactly the 12 frames of H_Gr.ILB; H_Border.BMP 78..89 is 12 frames
        // (its library is engine-loaded, not on disk); H_st.BMP 30..39 is 10.
        const std::map<std::uint32_t, std::size_t> frames =
            aow::map_resource_frames(res);
        auto frame_of = [&](std::uint32_t id, std::size_t& out) {
            const auto it = frames.find(id);
            if (it == frames.end()) return false;
            out = it->second;
            return true;
        };
        std::size_t f = 0;
        check(frame_of(0x06cf8f4b, f) && f == 0,
              "H_gr.BMP res_index 9 maps to frame 0");
        check(frame_of(0x0b29ad3b, f) && f == 11,
              "H_gr.BMP res_index 20 maps to frame 11 (12-frame run)");
        check(frame_of(0x00000020, f) && f == 0,
              "H_Border.BMP res_index 78 maps to frame 0");
        check(frame_of(0x017ef2bd, f) && f == 11,
              "H_Border.BMP res_index 89 maps to frame 11");
        check(frame_of(0x005322d5, f) && f == 9,
              "H_st.BMP res_index 39 maps to frame 9 (10-frame run)");
        // H_Wa2.BMP appears in two runs (40..48 and 102..107); the second run
        // restarts at frame 0 instead of smearing the first run's base.
        check(frame_of(0x0241c42b, f) && f == 0,
              "H_Wa2.BMP res_index 40 maps to frame 0");
        check(frame_of(0x00092bf0, f) && f == 0,
              "H_Wa2.BMP res_index 102 restarts a run at frame 0");
        check(frame_of(0x007b134f, f) && f == 0,
              "WATERHEX.ILB maps to frame 0");

        // --- Crops (Crops.TCrop / Crops.TCropResource) -----------------------
        // Release.hss has exactly one crop resource, id 41 (0x29). It carries
        // its Crops.bmp library INLINE (rw tag 8) and its TMultiHexMOR shift is
        // (-15,-9); the library's image ids are sparse: 1..4 (grass/desert/
        // snow/steppe), 12 (upper/underground) and the Undead block 31..35,41.
        const aow::MapResource* crop_res = aow::find_map_resource(res, 41);
        check(crop_res != nullptr &&
                  crop_res->class_id == aow::kCropResourceClassId,
              "resource id 41 is Crops.TCropResource (0x2036b)");
        check(crop_res != nullptr && crop_res->has_shift &&
                  crop_res->dx_main == -15 && crop_res->dy_main == -9,
              "the crop resource anchors at HXtoHP + (-15,-9)");
        check(crop_res != nullptr && !crop_res->ilb_data.empty(),
              "the crop resource carries its image library inline");
        if (crop_res != nullptr && !crop_res->ilb_data.empty()) {
            aow::IlbFile crop_ilb;
            std::string ierr;
            check(crop_ilb.load_memory(crop_res->ilb_data.data(),
                                       crop_res->ilb_data.size(), ierr),
                  "the inline crop library parses (" + ierr + ")");
            auto has_id = [&](std::uint32_t id) {
                for (const aow::IlbEntry& e : crop_ilb.entries()) {
                    if (e.index == id) return true;
                }
                return false;
            };
            check(!has_id(0), "Crops.bmp has no slot 0 (why crops used to vanish)");
            check(has_id(1) && has_id(2) && has_id(3) && has_id(4) &&
                      has_id(12),
                  "Crops.bmp has the per-terrain ids 1..4 and 12");
            check(has_id(31) && has_id(32) && has_id(33) && has_id(34) &&
                      has_id(35) && has_id(41),
                  "Crops.bmp has the Undead block 31..35 and 41");
        }
        // Crops.TCrop.GetTerrainTypeImage@0x557a5f14: the frame is the hex's
        // terrain number, +0x1e when the owner's GetRace() is 0x0b (Undead).
        check(aow::crop_image_index(1, 5) == 1,
              "a grass crop owned by a Halfling uses frame 1");
        check(aow::crop_image_index(1, 0x0b) == 0x1f,
              "a grass crop owned by the Undead uses frame 31");
        check(aow::crop_image_index(5, 0x0b) == 0x23,
              "a wasteland crop owned by the Undead uses frame 35");
        check(aow::crop_image_index(12, 5) == 12,
              "an upper/underground crop uses frame 12");
        check(aow::crop_image_index(12, 0x0b) == 42,
              "the Undead upper/underground id is 42 (no image -> crop freed)");
        check(aow::crop_image_index(-1, 5) == -1,
              "an unknown terrain selects no crop image");

        // A run's length must not exceed its library's entry count: the
        // H_gr.BMP block is exactly H_Gr.ILB's 12 entries.
        std::size_t gr_ids = 0;
        for (const aow::MapResource& r : res) {
            if (r.res_type == 11 && r.class_id == 0x20153 && r.ilb == "H_gr.BMP") {
                ++gr_ids;
            }
        }
        check(gr_ids == 12, "H_gr.BMP block has 12 records (== H_Gr.ILB entries)");

        // --- Race resources + city sprite selection (Phase 4d).
        // TAoWWHSSet rw tag 10 is the TRaceResourceList; each TRaceResource's
        // rw tag 8 names its image library, and the list order IS the faction
        // index a TCity's rw tag 0xd refers to.
        const std::vector<std::string> race_libs =
            aow::race_image_libraries(res);
        check(race_libs.size() == 12,
              "the game pack carries 12 race resources");
        check(race_libs.size() > 10 && race_libs[0] == "RACES\\RHUMAN.ILB" &&
                  race_libs[9] == "RACES\\RORC.ILB" &&
                  race_libs[10] == "RACES\\RGOBLIN.ILB",
              "race list order is the faction index (0 Human, 9 Orc, 10 Goblin)");

        // AoWE.TRaceResource.GetCityImage@55759ebc: slot and block rules.
        check(aow::city_image_slot(1, 1) == 1 * 10 + 1 + 0x45,
              "GetCityImage surface slot = terrain*10 + size + 0x45");
        check(aow::city_image_slot(3, 5) == 3 + 0x3b,
              "GetCityImage terrain 5 slot = size + 0x3b");
        // GetCityImage's last argument is the *map field level* (TMapField+0x12,
        // TMapField.GetXYL's high byte), not the city's wall type: level 0 =
        // surface -> the race library's overworld block, non-zero = underground
        // -> its underground block.
        check(aow::city_image_fallback_slot(2, 0) == 2 + 0x32 - 1,
              "GetCityImage surface (level 0) fallback = size + 0x31");
        check(aow::city_image_fallback_slot(2, 1) == 2 + 0x3c - 1,
              "GetCityImage underground (level 1) fallback = size + 0x3b");
        check(aow::city_image_fallback_slot(2, 1) !=
                  aow::city_image_fallback_slot(2, 0),
              "GetCityImage picks the underground block on a non-zero level");
        check(aow::city_terrain_block(1) == 0 && aow::city_terrain_block(3) == 0x14 &&
                  aow::city_terrain_block(8) == 0x28,
              "city block: surface 0, snow 0x14, wasteland/earth/rock/lava 0x28");

        const aow::MapResource* c1 = aow::find_map_resource(res, 22);
        const aow::MapResource* c3 = aow::find_map_resource(res, 58);
        check(c1 != nullptr && aow::city_resource_size(*c1) == 1,
              "resource 22 is City1 (size 1)");
        check(c3 != nullptr && aow::city_resource_size(*c3) == 3,
              "resource 58 is City3 (size 3)");

        // --- The body's race (Phase 4e follow-up): a city whose rw tag 0xd is
        // unset (-1/0xff) draws the race of its *owner* - the race
        // `TCity.UpdateFlagID` puts on its flag - instead of the city resource's
        // `C_random` "?" placeholder. Unowned (or unknown-owner) cities keep the
        // placeholder. Roster: index 0 = the Independents (race 0xff).
        const std::vector<std::uint8_t> roster = {0xff, 0, 1, 2};  // Indy/Human/Azrac/Lizard
        const std::size_t races = race_libs.size();
        check(aow::city_body_race(5, 1, roster, races) == 5,
              "city_body_race: a race the map set wins over the owner's");
        check(aow::city_body_race(-1, 1, roster, races) == 0,
              "city_body_race: an unset race takes the owner's (Human)");
        check(aow::city_body_race(-1, 2, roster, races) == 1 &&
                  aow::city_body_race(-1, 3, roster, races) == 2,
              "city_body_race: owner 2/3 -> Azrac/Lizard");
        check(aow::city_body_race(-1, 0, roster, races) == -1,
              "city_body_race: an unowned city keeps the C_random placeholder");
        check(aow::city_body_race(-1, 255, roster, races) == -1,
              "city_body_race: an owner that is not the Independents' is rejected");
        check(aow::city_body_race(-1, 9, roster, races) == -1,
              "city_body_race: an owner outside the roster is rejected");
        check(aow::city_body_race(-1, 1, {0xff, 0xff}, races) == -1,
              "city_body_race: an owner whose race is itself unset is rejected");
        check(aow::city_body_race(-1, 1, {0xff, 0xff}, 0) == -1,
              "city_body_race: no race libraries -> no race-specific body");
    }

    // --- ILB reloading ------------------------------------------------------
    // One IlbFile is reused for the selected unit's face library (R_Faces.ILB
    // for race faces, UNITS.ILB for machines/monsters). An entry's blob_offset
    // belongs to the file it was parsed from, so loading a second library must
    // replace the directory - appending it made decode_rgb() read the new file
    // at the old offsets and sheared the portrait.
    {
        const fs::path faces = fs::path(root) / "Images/Faces";
        const std::string rf = (faces / "R_Faces.ILB").string();
        const std::string uf = (faces / "UNITS.ILB").string();
        std::string ierr;
        aow::IlbFile fresh;
        const bool fresh_ok = fresh.load(rf, ierr);
        check(fresh_ok, "R_Faces.ILB loads (" + ierr + ")");
        aow::IlbFile reuse;
        std::string rerr;
        const bool first = reuse.load(rf, rerr);
        check(first, "R_Faces.ILB loads into the reusable object (" + rerr + ")");
        check(reuse.entries().size() == fresh.entries().size(),
              "the first load parses R_Faces.ILB's 12 race faces");
        std::string uerr;
        const bool units = reuse.load(uf, uerr);
        check(units, "UNITS.ILB loads into the reusable object (" + uerr + ")");
        check(reuse.entries().size() == 91,
              "reloading replaces the ILB directory instead of appending");
        std::string back_err;
        const bool back = reuse.load(rf, back_err);
        check(back, "R_Faces.ILB reloads (" + back_err + ")");
        check(reuse.entries().size() == fresh.entries().size(),
              "reloading R_Faces.ILB restores exactly its 12 entries");
        // Decode the dwarf face (id 6) from the reused object and from a fresh
        // one; they must be the same picture. With a leaked UNITS.ILB entry the
        // lookup finds that stale entry first and decodes R_Faces bytes at its
        // blob offset (the sheared portrait).
        auto decode_id = [](aow::IlbFile& il, std::uint32_t id,
                            std::vector<std::uint8_t>& px, std::uint32_t& w,
                            std::uint32_t& h, std::string& e) {
            for (std::size_t i = 0; i < il.entries().size(); ++i) {
                if (il.entries()[i].index != id) continue;
                return il.decode_rgb(i, px, w, h, e);
            }
            e = "no entry with the requested id";
            return false;
        };
        std::vector<std::uint8_t> re_px, fr_px;
        std::uint32_t re_w = 0, re_h = 0, fr_w = 0, fr_h = 0;
        std::string re_err, fr_err;
        const bool re_ok = decode_id(reuse, 6, re_px, re_w, re_h, re_err);
        const bool fr_ok = decode_id(fresh, 6, fr_px, fr_w, fr_h, fr_err);
        check(re_ok && fr_ok, "the dwarf race face decodes after the reloads");
        check(re_ok && fr_ok && re_w == 103 && re_h == 128,
              "the dwarf race face is 103x128");
        check(re_ok && fr_ok && re_w == fr_w && re_h == fr_h && re_px == fr_px,
              "a reused IlbFile decodes the same race face as a fresh one");
    }

    aow::Map m;
    const std::string mp = (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
    if (!aow::load_map(mp, m, err)) {
        check(false, "load Tutorial.hsm (" + err + ")");
    } else {
        aow::MapVizOptions opt;
        opt.level = 0;
        opt.max_dim = 480;
        const std::string out =
            (fs::temp_directory_path() / "aow_mapviz_test.png").string();
        std::error_code ec;
        if (!aow::render_map_png(m, res, root, opt, out, err)) {
            check(false, "render_map_png (" + err + ")");
        } else {
            check(fs::exists(out, ec) && fs::file_size(out, ec) > 1000,
                  "render_map_png wrote a non-empty PNG");
            fs::remove(out, ec);
        }

        // --- Phase 1: every hex must be covered (the near-black backdrop must
        // not show through at any hex centre). Sample each hex centre at full
        // resolution.
        aow::MapVizOptions full;
        full.level = 0;
        full.objects = true;
        std::vector<std::uint8_t> rgba;
        std::uint32_t fw = 0, fh = 0;
        if (!aow::render_map_rgba(m, res, root, full, rgba, fw, fh, err)) {
            check(false, "render_map_rgba full-res (" + err + ")");
        } else {
            const aow::MapLevel& lv = m.levels[0];
            // Engine grid (HSEngine.HXtoHP@8E9B8701): 48x32 hexagons on a 32x32
            // pitch, odd columns staggered down 16px, field x-offset 8. The hex
            // centre is the sprite box's top-left + (tile_w/2, tile_h/2). This
            // mirrors the engine formula independently of mapviz's own origin().
            const int pitch = 32, half = 16, field_x = 8, tw = 48, th = 32;
            std::size_t centres = 0, gaps = 0;
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const int ix = static_cast<int>(x);
                    const int cx = field_x + ix * pitch + tw / 2;
                    const int cy = static_cast<int>(y) * pitch +
                                   (ix & 1) * half + th / 2;
                    if (cx < 0 || cy < 0 || cx >= static_cast<int>(fw) ||
                        cy >= static_cast<int>(fh)) {
                        continue;
                    }
                    const std::size_t o =
                        (static_cast<std::size_t>(cy) * fw + cx) * 4;
                    ++centres;
                    if (rgba[o] == 12 && rgba[o + 1] == 12 && rgba[o + 2] == 20) {
                        ++gaps;
                    }
                }
            }
            check(centres > 0, "hex centres were sampled (Phase 1)");
            check(gaps == 0, "no hex centre shows the backdrop (Phase 1)");
        }

        // --- City rendering (Phase 4d). Tutorial L0 carries two cities: a
        // walled goblin town at (32,21) and a halfling city at (16,25). Their
        // rw tags 0xd/0xf are the race / wall byte, each sits on a Pad.TPad
        // (the "hill"), and the body must come from the race's RACES\R*.ILB.
        {
            const aow::MapLevel& lv = m.levels[0];
            auto object_of = [&](std::uint32_t x, std::uint32_t y,
                                 std::uint32_t cls) -> const aow::MapHs* {
                for (const aow::MapHs& hs :
                     lv.tiles[static_cast<std::size_t>(y) * lv.width + x].objects) {
                    if (hs.class_id == cls) return &hs;
                }
                return nullptr;
            };
            const aow::MapHs* gob = object_of(32, 21, aow::kCityClassId);
            const aow::MapHs* hal = object_of(16, 25, aow::kCityClassId);
            check(gob != nullptr && gob->race == 10,
                  "Tutorial (32,21) is a Goblin city (rw tag 0xd)");
            check(hal != nullptr && hal->race == 5,
                  "Tutorial (16,25) is a Halfling city (rw tag 0xd)");
            check(gob != nullptr && gob->wall_type == 1,
                  "Tutorial (32,21) is walled (rw tag 0xf)");
            check(hal != nullptr && hal->wall_type == 0,
                  "Tutorial (16,25) is unwalled");
            check(object_of(32, 21, aow::kPadClassId) != nullptr &&
                      object_of(16, 25, aow::kPadClassId) != nullptr,
                  "every Tutorial city sits on a Pad.TPad hill object");

            const std::string ccity =
                (fs::temp_directory_path() / "aow_city.csv").string();
            aow::MapVizOptions copt;
            copt.level = 0;
            copt.debug_csv = ccity;
            std::vector<std::uint8_t> crgba;
            std::uint32_t cw = 0, chh = 0;
            if (!aow::render_map_rgba(m, res, root, copt, crgba, cw, chh, err)) {
                check(false, "render Tutorial L0 for the city check (" + err + ")");
            } else {
                std::ifstream in(ccity);
                std::string line, gob_city, hal_city;
                std::size_t bodies = 0, race_bodies = 0, walls = 0, pads = 0;
                std::size_t crop_rows = 0;
                bool crop_grass = false;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("city,32,21,", 0) == 0) {
                        gob_city += line + "\n";
                    }
                    if (line.rfind("city,16,25,", 0) == 0) {
                        hal_city += line + "\n";
                    }
                    if (line.rfind("object,", 0) == 0 &&
                        line.find("PAD") != std::string::npos) {
                        ++pads;
                    }
                    if (line.rfind("crop,", 0) == 0) {
                        ++crop_rows;
                        // (16,24) is one of the three crop tiles beside the
                        // Halfling city; the hex is grass, so GetTerrainTypeImage
                        // selects Crops.bmp id 1 (a 43x36 decoded sprite).
                        if (line.rfind(
                                "crop,16,24,41,Crops.bmp,1,crops.bmp,43,36,1",
                                0) == 0) {
                            crop_grass = true;
                        }
                    }
                    if (line.rfind("city,", 0) != 0) continue;
                    if (line.find("RACES\\") != std::string::npos) {
                        ++bodies;
                        if (line.find("c_") != std::string::npos) ++race_bodies;
                    }
                    if (line.find("_wall.bmp") != std::string::npos) ++walls;
                }
                std::error_code ecc;
                fs::remove(ccity, ecc);
                check(bodies == 2 && race_bodies == 2,
                      "both Tutorial cities draw a race-specific body "
                      "(RACES\\R*.ILB)");
                // Both cities are on level 0 (the surface), so the race body
                // must come from the *overworld* block (slots 50..53) even for
                // the walled Goblin town. Slots 60..63 are the underground
                // block (GetCityImage's level argument, TMapField+0x12).
                check(gob_city.find("RACES\\RGOBLIN.ILB,50,c_goblin.bmp") !=
                          std::string::npos,
                      "Tutorial (32,21): a walled surface Goblin city draws the "
                      "overworld body (slot 50)");
                check(gob_city.find("RACES\\RGOBLIN.ILB,60,") ==
                          std::string::npos,
                      "Tutorial (32,21): no underground body on the surface");
                check(hal_city.find("RACES\\RHALFLNG.ILB,52,c_hlflng.bmp") !=
                          std::string::npos,
                      "Tutorial (16,25): the unwalled surface Halfling city "
                      "draws the overworld body (slot 52)");
                check(walls >= 2,
                      "the walled goblin city draws its O_wall layers");
                check(pads >= 2, "both Tutorial cities draw a Pad (hill)");
                check(crop_rows == 7,
                      "Tutorial L0 draws all 7 Crops.TCrop tiles");
                check(crop_grass,
                      "the grass crop at (16,24) resolves Crops.bmp frame 1");
            }
        }

        // --- The owner's race for a city the map left unset (Phase 4e
        // follow-up). `Barondir Bay.HSM` has a walled 4-hex Human town at
        // (12,20): its rw tag 0xd is -1 (the editor's "Random") while its
        // rw tag 5 makes it player 3's - and player 3 of that map's roster is
        // Human (`aow_tools gamestate`: 1 Lizard, 2 Azrac, 3 Human, 4
        // Frostling), so it flies `RACES\RHUMAN.ILB`. Its body must therefore be
        // the Human city art (`RHUMAN.ILB` slot 63 = size 4, walled) and not the
        // The city is on the surface (level 0), so GetCityImage's fallback asks
        // RHUMAN for size + 0x31 = slot 53 - which the Human library does not
        // ship (its only city block is the underground 60..63) - and
        // UpdateImages then draws the *city resource's* own `C_human.BMP` body
        // (`CITY4.ILB` slot 0). The body must therefore NOT be slot 63 (the
        // underground art) and not the `C_random` "?" placeholder, while the
        // three unowned race-less cities of the same map keep the placeholder.
        {
            aow::Map bm;
            const std::string bmp_path =
                (fs::path(root) / "Scenario/Custom/Barondir Bay.HSM").string();
            if (!aow::load_map(bmp_path, bm, err)) {
                check(false, "load Barondir Bay.HSM (" + err + ")");
            } else {
                // A large building is stored on its home hex but covers every
                // hex of its resource's TTerrainList (TMultiHexMO.UpdateMapFields):
                // the size-4 city at (12,20) covers (12,20), (12,19), (13,19)
                // and (11,19), while a size-1 city covers only its own hex.
                const aow::MapLevel& lv = bm.levels[0];
                auto city_at = [&](std::uint32_t x,
                                   std::uint32_t y) -> const aow::MapHs* {
                    for (const aow::MapHs& hs :
                         lv.tiles[static_cast<std::size_t>(y) * lv.width + x].objects) {
                        if (hs.class_id == aow::kCityClassId) return &hs;
                    }
                    return nullptr;
                };
                const aow::MapHs* big = city_at(12, 20);
                const aow::MapResource* bigr =
                    big == nullptr ? nullptr
                                   : aow::find_map_resource(res, big->resource);
                check(big != nullptr && bigr != nullptr &&
                          aow::city_resource_size(*bigr) == 4,
                      "Barondir Bay (12,20) is the size-4 capital");
                if (big != nullptr && bigr != nullptr) {
                    check(aow::object_covers_hex(*big, res, 12, 20, 12, 20) &&
                              aow::object_covers_hex(*big, res, 12, 20, 12, 19) &&
                              aow::object_covers_hex(*big, res, 12, 20, 13, 19) &&
                              aow::object_covers_hex(*big, res, 12, 20, 11, 19),
                          "the size-4 capital covers all four of its tiles");
                    check(!aow::object_covers_hex(*big, res, 12, 20, 14, 20),
                          "the size-4 capital does not cover a tile outside it");
                }
                const aow::MapHs* small = city_at(11, 12);
                if (small != nullptr) {
                    check(aow::object_covers_hex(*small, res, 11, 12, 11, 12) &&
                              !aow::object_covers_hex(*small, res, 11, 12, 11, 13),
                          "a size-1 city covers only its own hex");
                }
                const std::string bcsv =
                    (fs::temp_directory_path() / "aow_city_owner.csv").string();
                aow::MapVizOptions bopt;
                bopt.level = 0;
                bopt.debug_csv = bcsv;
                std::vector<std::uint8_t> brgba;
                std::uint32_t bw = 0, bh = 0;
                if (!aow::render_map_rgba(bm, res, root, bopt, brgba, bw, bh,
                                          err)) {
                    check(false, "render Barondir Bay L0 (" + err + ")");
                } else {
                    std::ifstream in(bcsv);
                    std::string line, owner_city, unowned_city;
                    std::getline(in, line);  // header
                    while (std::getline(in, line)) {
                        // A city logs three rows (back wall, body, front wall);
                        // keep them all and search the body row inside.
                        if (line.rfind("city,12,20,", 0) == 0) {
                            owner_city += line + "\n";
                        }
                        if (line.rfind("city,29,20,", 0) == 0) {
                            unowned_city += line + "\n";
                        }
                    }
                    std::error_code ebc;
                    fs::remove(bcsv, ebc);
                    check(owner_city.find("CITY4.ILB,0,c_human.bmp") !=
                              std::string::npos,
                          "Barondir Bay (12,20): a race-less Human-owned "
                          "surface city draws the overworld Human body");
                    check(owner_city.find("RACES\\RHUMAN.ILB,63,") ==
                              std::string::npos,
                          "Barondir Bay (12,20): no underground race body on "
                          "the surface");
                    check(owner_city.find("c_random.bmp") == std::string::npos,
                          "Barondir Bay (12,20): no C_random placeholder");
                    check(unowned_city.find(
                              "CITY1.ILB,1,c_random.bmp") != std::string::npos,
                          "Barondir Bay (29,20): an unowned race-less city keeps "
                          "the C_random placeholder");
                }
            }
        }
        // --- The other direction: an underground city (level 1) must use the
        // race library's underground block (slots 60..63), even when it is
        // unwalled. Dwarf1.CSM's level 1 carries a walled Goblin city at
        // (32,19) and an unwalled Goblin city at (79,22), both on level 1.
        {
            aow::Map dm;
            const std::string dpath =
                (fs::path(root) / "Scenario/Campaign/Dwarf1.CSM").string();
            if (!aow::load_map(dpath, dm, err)) {
                check(false, "load Dwarf1.CSM for the city level check (" + err +
                                 ")");
            } else if (dm.levels.size() < 2) {
                check(false,
                      "Dwarf1.CSM has a level 1 for the city level check");
            } else {
                const std::string dcsv =
                    (fs::temp_directory_path() / "aow_city_level1.csv").string();
                aow::MapVizOptions dopt;
                dopt.level = 1;
                dopt.max_dim = 200;
                dopt.debug_csv = dcsv;
                std::vector<std::uint8_t> drgba;
                std::uint32_t dw = 0, dh = 0;
                if (!aow::render_map_rgba(dm, res, root, dopt, drgba, dw, dh,
                                          err)) {
                    check(false, "render Dwarf1 L1 for the city level check (" +
                                     err + ")");
                } else {
                    std::ifstream in(dcsv);
                    std::string line, walled, unwalled;
                    std::getline(in, line);  // header
                    while (std::getline(in, line)) {
                        if (line.rfind("city,32,19,", 0) == 0) {
                            walled += line + "\n";
                        }
                        if (line.rfind("city,79,22,", 0) == 0) {
                            unwalled += line + "\n";
                        }
                    }
                    std::error_code edc;
                    fs::remove(dcsv, edc);
                    check(walled.find(
                              "RACES\\RGOBLIN.ILB,63,c_goblin.bmp") !=
                              std::string::npos,
                          "Dwarf1 L1 (32,19): a walled underground city draws "
                          "the underground body (slot 63)");
                    check(walled.find("CITY4.ILB,45,u_wall.bmp") !=
                              std::string::npos,
                          "Dwarf1 L1 (32,19): an underground city draws U_wall "
                          "layers");
                    check(unwalled.find(
                              "RACES\\RGOBLIN.ILB,60,c_goblin.bmp") !=
                              std::string::npos,
                          "Dwarf1 L1 (79,22): an unwalled underground city "
                          "draws the underground body (slot 60)");
                    check(unwalled.find("RACES\\RGOBLIN.ILB,50,") ==
                              std::string::npos,
                          "Dwarf1 L1 (79,22): no overworld body underground");
                }
            }
        }



        // --- Phase 0/2: the diagnostic CSV must show no terrain hex drawing a
        // transition overlay (`t_*` / `zt_*`) instead of a base hexagon.
        const std::string csvp =
            (fs::temp_directory_path() / "aow_mapviz_debug.csv").string();
        aow::MapVizOptions dbg;
        dbg.level = 0;
        dbg.max_dim = 480;
        dbg.debug_csv = csvp;
        std::vector<std::uint8_t> rgba2;
        std::uint32_t dw2 = 0, dh2 = 0;
        if (!aow::render_map_rgba(m, res, root, dbg, rgba2, dw2, dh2, err)) {
            check(false, "render_map_rgba with debug_csv (" + err + ")");
        } else {
            std::ifstream in(csvp);
            std::string line;
            std::size_t terrain_rows = 0, transitions = 0;
            std::size_t object_rows = 0, object_ok = 0, tree_ok = 0;
            std::size_t border_rows = 0, border_ok = 0;
            std::getline(in, line);  // header
            while (std::getline(in, line)) {
                const bool is_terrain = line.rfind("terrain,", 0) == 0;
                const bool is_object = line.rfind("object,", 0) == 0;
                if (!is_terrain && !is_object) continue;
                // columns: layer,x,y,resource,ilb,frame,entry,w,h,ok
                std::size_t p = 0;
                int col = 0;
                std::string entry, ilb;
                bool ok = false;
                for (std::size_t i = 0; i <= line.size(); ++i) {
                    if (i == line.size() || line[i] == ',') {
                        const std::string f = line.substr(p, i - p);
                        if (col == 4) ilb = f;
                        if (col == 6) entry = f;
                        if (col == 9) ok = (f == "1");
                        ++col;
                        p = i + 1;
                    }
                }
                if (is_terrain) {
                    ++terrain_rows;
                    if (entry.rfind("t_", 0) == 0 || entry.rfind("zt_", 0) == 0) {
                        ++transitions;
                    }
                    // The map's outer frame (`H_Border.BMP`, ~44% of a 48x48
                    // map) has no library on disk; it must fall back to the
                    // water hexagon library (see mapviz object_library_alias).
                    if (ilb == "H_Border.BMP") {
                        ++border_rows;
                        if (ok) ++border_ok;
                    }
                } else {
                    ++object_rows;
                    if (ok) {
                        ++object_ok;
                        if (ilb == "gr_tree.BMP") ++tree_ok;
                    }
                }
            }
            fs::remove(csvp, ec);
            check(terrain_rows > 0, "debug_csv produced terrain rows (Phase 0)");
            check(transitions == 0,
                  "no terrain hex drew a transition overlay (Phase 2)");
            check(tree_ok > 0,
                  "logical object library gr_tree.BMP resolves (Phase 2.5)");
            check(object_ok * 2 > object_rows,
                  "most object sprites resolve (Phase 2.5)");
            check(border_rows > 0 && border_ok == border_rows,
                  "every H_Border.BMP frame hex resolves to a real sprite");
        }
    }

    // --- Phase 3: sprite placement fields (`base` margins +0x28/+0x2c and the
    // `shift` +0x50/+0x54) parse out of the ILB descriptor. The engine blits a
    // sprite's content at (anchor + base + shift)
    // (re/exports/ilpack/TSprite16.ShowOpaque.c, TRLESprite16.GetVisualDimensions.c).
    // The base terrain hexagon is the calibration point: content 48x32 at
    // shift (0,0), i.e. it anchors on the tile origin returned by HXtoHP.
    {
        std::string ilberr;
        const std::string hexlib =
            (fs::path(root) / "Images/TCombat/Base_hex/H_Gr.ILB").string();
        aow::IlbFile hex;
        if (!hex.load(hexlib, ilberr) || hex.entries().empty()) {
            check(false, "load H_Gr.ILB for placement fields (" + ilberr + ")");
        } else {
            const aow::IlbEntry& h = hex.entries().front();
            check(h.base_x == 0 && h.base_y == 0,
                  "H_Gr base hexagon has zero base margins");
            check(h.xshift == 0 && h.yshift == 0,
                  "H_Gr base hexagon has zero shift (anchors on the tile origin)");
            check(h.rle_w == 48 && h.rle_h == 32,
                  "H_Gr base hexagon content is the 48x32 hexagon");
        }

        const std::string treelib =
            (fs::path(root) / "Images/TCombat/Gr/zgr_Tre.ILB").string();
        aow::IlbFile tree;
        if (!tree.load(treelib, ilberr) || tree.entries().empty()) {
            check(false, "load zgr_Tre.ILB for placement fields (" + ilberr + ")");
        } else {
            bool any_shift = false;
            for (const aow::IlbEntry& t : tree.entries()) {
                if (t.xshift != 0 || t.yshift != 0) any_shift = true;
            }
            check(any_shift,
                  "zgr_Tre object sprites carry a non-zero xshift/yshift");
        }
    }

    // --- Palettes: the byte order comes from each palette's u32 format tag
    // (GFXE.TGFXEPalette.Remap @ Gfxepack.dpl 0x5500ba34; GetEntry @ 0x5500ba2c).
    // 0x88801B18 stores entries (R,G,B) and 0x88809318 stores (B,G,R); every
    // shipped palette is 0x88801B18. The 8-bit unit libraries are the check:
    // their sprites are warm (brown horse, red plume), so decoding them with the
    // wrong order swaps R/B and turns the whole sprite blue (mean B > mean R).
    {
        std::string ilberr;
        const std::string cavalry =
            (fs::path(root) / "Images/Units/Orc/orchr.ILB").string();
        aow::IlbFile lib;
        if (!lib.load(cavalry, ilberr) || lib.entries().empty()) {
            check(false, "load orchr.ILB for palette order (" + ilberr + ")");
        } else {
            check(!lib.palette_formats().empty() &&
                      lib.palette_formats().front() == 0x88801B18u,
                  "orchr.ILB palette tag is 0x88801B18 -> entries are (R,G,B)");
            long sum_r = 0, sum_b = 0, n = 0;
            for (std::size_t i = 0; i < lib.entries().size(); ++i) {
                std::vector<std::uint8_t> rgba;
                std::uint32_t w = 0, h = 0;
                if (!lib.decode_rgb(i, rgba, w, h, ilberr)) continue;
                for (std::size_t p = 0; p + 3 < rgba.size(); p += 4) {
                    if (rgba[p + 3] == 0) continue;
                    sum_r += rgba[p + 0];
                    sum_b += rgba[p + 2];
                    ++n;
                }
            }
            check(n > 0, "orchr.ILB decoded opaque pixels");
            check(sum_r > sum_b,
                  "orchr.ILB is warm (mean R > mean B): (R,G,B), not blue-tinted");
        }
    }

    // --- Phase 3: the engine object anchor (`object_sprite_origin` in mapviz).
    // Evidence: re/exports/hsepack/HSEngine.TMapObject.Show.c (single-hex anchor
    // `HXtoHP - 8`, cancelled by mapviz's kHexFieldMargin grid offset) and
    // HSEngine.TMultiHexMO.Show.c (multi-hex anchor `HXtoHP + dXmainP/dYmainP`);
    // the sprite's decoded base+shift (Sprite::vx/vy) is added on top in both.
    {
        const aow::MapResource* no_shift = aow::find_map_resource(res, 0x06cf8f4b);
        check(no_shift != nullptr && !no_shift->has_shift,
              "H_gr base hexagon resource carries no multi-hex shift");
        int ox = 0, oy = 0;
        aow::object_sprite_origin(100, 50, no_shift, 7, -3, ox, oy);
        check(ox == 107 && oy == 47,
              "single-hex object anchor = hex origin + base+shift");

        const aow::MapResource* mnt = aow::find_map_resource(res, 0x0a75015a);
        check(mnt != nullptr && mnt->has_shift,
              "0x0a75015a mountain resource carries the multi-hex shift");
        if (mnt != nullptr) {
            aow::object_sprite_origin(100, 50, mnt, 2, 1, ox, oy);
            check(ox == 100 + mnt->dx_main + 2 + aow::kHexFieldMargin &&
                      oy == 50 + mnt->dy_main + 1,
                  "multi-hex object anchor = hex origin + dXmainP/dYmainP + base+shift");
        }
    }

    // --- Phase 4: land terrain-transition overlays. Evidence:
    // re/exports/aowepack/AoWHex.TAoWHexagon.UpdateTransition.c - a land hex
    // blends toward an upper neighbour of a different terrain with the HEXTRANS
    // frame `own*16 + 6 + edge + variant*3` (own = 1..5 = grass/desert/snow/
    // stone/wall, matching T_GR/T_DE/T_SN/T_ST/T_WL at idx 22/38/54/70/86).
    {
        const std::string tpath =
            (fs::path(root) / "Scenario/Campaign/Dwarf1.CSM").string();
        aow::Map tm;
        std::string terr2;
        if (!aow::load_map(tpath, tm, terr2)) {
            check(false, "load Dwarf1.CSM for the transition check (" + terr2 + ")");
        } else {
            const std::string tcsv =
                (fs::temp_directory_path() / "aow_trans.csv").string();
            aow::MapVizOptions topt;
            topt.level = 0;
            topt.max_dim = 300;
            topt.debug_csv = tcsv;
            std::vector<std::uint8_t> tr;
            std::uint32_t tw = 0, th = 0;
            if (!aow::render_map_rgba(tm, res, root, topt, tr, tw, th, terr2)) {
                check(false, "render Dwarf1 for the transition check (" + terr2 + ")");
            } else {
                std::ifstream in(tcsv);
                std::string line;
                std::size_t trans = 0, bad = 0, bad_ilb = 0;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("transition,", 0) != 0) continue;
                    ++trans;
                    if (line.find("HEXTRANS.ILB") == std::string::npos) ++bad_ilb;
                    // frame is field 6 (layer,x,y,resource,ilb,frame,...).
                    const std::size_t p = line.find("HEXTRANS.ILB,");
                    if (p == std::string::npos) { ++bad; continue; }
                    const long f = std::strtol(line.c_str() + p + 13, nullptr, 10);
                    // own=1..5 -> frame in own*16+6 .. own*16+14 -> 22..94.
                    if (f < 22 || f > 94) ++bad;
                }
                std::error_code ec4;
                fs::remove(tcsv, ec4);
                (void)tw; (void)th;
                check(trans > 100, "Dwarf1 level 0 draws land terrain transitions");
                check(bad == 0,
                      "every transition frame is in a HEXTRANS terrain block (22..94)");
                check(bad_ilb == 0, "every transition comes from HEXTRANS.ILB");
            }
        }
    }

    // --- Phase 5: raised terrain (the underground level) uses
    // IsoHex.TUpperIsometricHexagon hexes (class 0x20170) whose library is
    // EARTHHEX/ROCKHEX. Evidence: re/exports/hsepack/IsoHex.TIsometricHexagon.Show.c
    // (TUpperIsometricHexagon.Show@55619e24) draws the base at
    // `(sprite+0x15 >> 6) + 0x28` -> EARTHHEX slots 40..43 = `hu_er.BMP`, and the
    // library is indexed by the record's image-slot id (EARTHHEX skips 33..39).
    {
        const std::string upath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map um;
        std::string uerr;
        if (!aow::load_map(upath, um, uerr)) {
            check(false, "load Tutorial.hsm for the upper-hex check (" + uerr + ")");
        } else {
            const std::string ucsv =
                (fs::temp_directory_path() / "aow_upper.csv").string();
            aow::MapVizOptions uopt;
            uopt.level = 1;
            uopt.max_dim = 300;
            uopt.debug_csv = ucsv;
            std::vector<std::uint8_t> ur;
            std::uint32_t uw = 0, uh = 0;
            if (!aow::render_map_rgba(um, res, root, uopt, ur, uw, uh, uerr)) {
                check(false, "render Tutorial L1 for the upper-hex check (" + uerr + ")");
            } else {
                std::ifstream in(ucsv);
                std::string line;
                std::size_t earth = 0, bad = 0;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.find("EARTHHEX.ILB") == std::string::npos) continue;
                    ++earth;
                    if (line.find("hu_er.bmp") == std::string::npos) ++bad;
                    const std::size_t p = line.find("EARTHHEX.ILB,");
                    if (p == std::string::npos) { ++bad; continue; }
                    const long f = std::strtol(line.c_str() + p + 13, nullptr, 10);
                    if (f < 40 || f > 43) ++bad;
                }
                std::error_code ec5;
                fs::remove(ucsv, ec5);
                (void)uw; (void)uh;
                check(earth > 500, "Tutorial level 1 is built from EARTHHEX hexes");
                check(bad == 0,
                      "every upper-isometric hex draws an `hu_er` frame 40..43");
            }
        }
    }

    // --- Image-sequence lists. A multi-hex object resource carries its own
    // sequence (rw tags 9/10) naming the library AND the frame
    // (TImageSequenceList/TImageSequence.LoadFromStream; ShowComplexImage draws it
    // at anchor + seq xoff/yoff). The three grass "mountain" resources are
    // MOUNTAIN.ILB frames 26 / 8 / 9 - 26 being the 60x50 HILL mound, which the
    // old empirical `mountain_block()+2+want%3` drew as a 93x66 mountain.
    {
        const aow::MapResource* m0 = aow::find_map_resource(res, 0x0a75015a);
        const aow::MapResource* m1 = aow::find_map_resource(res, 0x0cba6db5);
        const aow::MapResource* m2 = aow::find_map_resource(res, 0x0809390e);
        check(m0 != nullptr && m0->has_seq && m0->seq_lib == "MOUNTAIN\\MOUNTAIN.ILB",
              "a mountain resource carries its sequence library");
        check(m0 != nullptr && m0->has_seq && m0->seq_frame == 26,
              "grass mountain res_index 0 is MOUNTAIN.ILB frame 26 (the hill mound)");
        check(m1 != nullptr && m1->has_seq && m1->seq_frame == 8,
              "grass mountain res_index 1 is MOUNTAIN.ILB frame 8");
        check(m2 != nullptr && m2->has_seq && m2->seq_frame == 9,
              "grass mountain res_index 2 is MOUNTAIN.ILB frame 9");
        check(m0 != nullptr && m0->has_seq && m0->seq_dx == -13 && m0->seq_dy == -13,
              "the sequence carries its own pixel offset");
    }

    // --- Structures carry their own image sequence, and its xoff/yoff are what
    // place them (AoWE.TStructureResource.Show@557605dc -> ShowComplexImage). The
    // tag-9 head is followed by one housekeeping byte for City3 but two for
    // Mine/Tower/WizardsTower, which the old fixed-offset parse missed - so the
    // wizard tower fell back to (0,0) and dropped ~90px below its hex.
    {
        const aow::MapResource* mine = aow::find_map_resource(res, 23);
        const aow::MapResource* tower = aow::find_map_resource(res, 192266807);
        const aow::MapResource* wtwr = aow::find_map_resource(res, 97718693);
        check(mine != nullptr && mine->has_seq && mine->seq_lib == "STR_MINE.ILB" &&
                  mine->seq_dx == -16 && mine->seq_dy == -22,
              "the mine resource's sequence parses (xoff/yoff -16,-22)");
        check(tower != nullptr && tower->has_seq && tower->seq_lib == "STR_TOWR.ILB" &&
                  tower->seq_dx == -9 && tower->seq_dy == -48,
              "the tower resource's sequence parses (xoff/yoff -9,-48)");
        check(wtwr != nullptr && wtwr->has_seq && wtwr->seq_lib == "STR_WTWR.ILB" &&
                  wtwr->seq_dx == -10 && wtwr->seq_dy == -90,
              "the wizard tower's sequence grounds its 120px sprite (-10,-90)");
    }

    // --- Structures whose sequence list opens with an additive EFFECT. The
    // Shipyard (class 0x204bf) lists a `SMOKE.ILB` sequence chained in front of
    // its dock, so the old "first name in the blob" scan recorded SMOKE.ILB as
    // the body - `mapviz` then resolved the smoke library, `is_transient_effect()`
    // dropped the whole object and no port/shipyard was ever drawn. The list's
    // entry 1 is the smoke sequence and its chain (`+0xc`) is `PRO_SYRD.ILB`, the
    // dock; `aow::body_sequence` skips effect libraries and returns it.
    {
        const aow::MapResource* syrd = aow::find_map_resource(res, 60);  // 0x3c
        check(syrd != nullptr && syrd->sequences.size() >= 2,
              "the Shipyard resource parses its image-sequence list");
        check(syrd != nullptr && syrd->sequences.size() >= 2 &&
                  syrd->sequences[1].library == "SMOKE.ILB" &&
                  !syrd->sequences[1].chain.empty(),
              "the Shipyard list opens with a SMOKE.ILB effect sequence");
        const aow::ImageSequence* body =
            syrd != nullptr ? aow::body_sequence(*syrd) : nullptr;
        check(body != nullptr && body->library == "PRO_SYRD.ILB",
              "body_sequence skips the effect and returns the PRO_SYRD dock");
        check(syrd != nullptr && syrd->has_seq &&
                  syrd->seq_lib == "PRO_SYRD.ILB" && syrd->seq_dx == -19 &&
                  syrd->seq_dy == -25,
              "the Shipyard dock carries its own pixel offset (-19,-25)");
        check(aow::is_effect_library("SMOKE.ILB") &&
                  !aow::is_effect_library("PRO_SYRD.ILB") &&
                  !aow::is_effect_library("STR_TOWR.ILB"),
              "is_effect_library recognises the additive effect libraries");
    }

    // --- Inline image libraries. A map-object resource whose stored string is
    // only a logical label carries its sprites INLINE (TMapObjectResource rw
    // tag 8 = a complete ILB image), because no file or ILB entry answers to that
    // name and TILManagerList.LinkToIL matches exact paths only.
    {
        const aow::MapResource* tree = nullptr;
        std::size_t inline_libs = 0;
        for (const aow::MapResource& r : res) {
            if (!r.ilb_data.empty()) ++inline_libs;
            if (r.ilb == "gr_tree.BMP" && tree == nullptr) tree = &r;
        }
        check(inline_libs > 100, "many resources carry an inline ILB image");
        check(tree != nullptr, "a gr_tree.BMP resource exists");
        if (tree != nullptr) {
            check(!tree->ilb_data.empty(),
                  "gr_tree.BMP carries its library inline");
            aow::IlbFile ilb;
            std::string ie;
            const bool ok = !tree->ilb_data.empty() &&
                            ilb.load_memory(tree->ilb_data.data(),
                                            tree->ilb_data.size(), ie);
            check(ok && !ilb.entries().empty(),
                  "the inline ILB parses to at least one image");
            if (ok && !ilb.entries().empty()) {
                std::string nm = ilb.entries().front().name;
                for (char& c : nm) c = static_cast<char>(std::tolower(
                    static_cast<unsigned char>(c)));
                check(nm == "gr_tree.bmp",
                      "the inline library's first entry is named after the resource");
            }
        }
    }

    // --- Mountain terrain block (`mountain_block` in src/engine/mapviz.cpp).
    // MOUNTAIN.ILB's block is chosen by the resource `res_type`; the mapping is
    // pinned by cross-referencing each map's mountain types against its terrain
    // tiles (tools/mtmap.ps1). A snow map must not draw desert mountains.
    {
        aow::Map dm;
        std::string derr;
        const std::string dpath =
            (fs::path(root) / "Scenario/Campaign/Dwarf1.CSM").string();
        if (!aow::load_map(dpath, dm, derr)) {
            check(false, "load Dwarf1.CSM for the mountain-block check (" + derr + ")");
        } else {
            const std::string dcsv =
                (fs::temp_directory_path() / "aow_dwarf_mtn.csv").string();
            aow::MapVizOptions dopt;
            dopt.level = 0;
            dopt.max_dim = 200;
            dopt.debug_csv = dcsv;
            std::vector<std::uint8_t> dr;
            std::uint32_t dw = 0, dh = 0;
            if (!aow::render_map_rgba(dm, res, root, dopt, dr, dw, dh, derr)) {
                check(false, "render Dwarf1 level 0 (" + derr + ")");
            } else {
                std::ifstream in(dcsv);
                std::string line;
                std::size_t mtns = 0, desert = 0, snow = 0;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.find("MOUNTAIN\\MOUNTAIN.ILB") == std::string::npos) {
                        continue;
                    }
                    ++mtns;
                    if (line.find("mnt_de.bmp") != std::string::npos) ++desert;
                    if (line.find("mnt_sn.bmp") != std::string::npos) ++snow;
                }
                std::error_code ec2;
                fs::remove(dcsv, ec2);
                (void)dw; (void)dh;
                check(mtns > 100, "Dwarf1 level 0 draws mountains");
                check(snow > 0, "a snow map draws Mnt_sn mountains (res_type 14)");
                check(desert == 0,
                      "a map with no desert tiles draws no Mnt_de mountains");
            }
        }
    }

    // --- Multi-hex resource placement (tags 12/13 = TMultiHexMOR
    // `dXmainP`/`dYmainP`). Evidence: re/exports/hsepack/
    // HSEngine.TMultiHexMOR.ReadWrite.c reads four ints (+0x34..+0x40) with
    // tags 12..15, `TMultiHexMOR.SetdXmainP/SetdYmainP` write +0x34/+0x38, and
    // HSEngine.TMultiHexMO.Show.c draws the sprite at HXtoHP(hex) + those
    // fields. Only the multi-hex subclasses (Mountain.TMountainMOR 0x20169,
    // Terrain.TTerrainMOR 0x20167) carry them; plain terrain hexagons
    // (Hexagon.THexagonResource 0x20153) do not.
    {
        const aow::MapResource* mnt = aow::find_map_resource(res, 0x0a75015a);
        check(mnt != nullptr && mnt->class_id == 0x20169,
              "0x0a75015a is a Mountain.TMountainMOR resource");
        check(mnt != nullptr && mnt->has_shift,
              "mountain resources carry the TMultiHexMOR shift (tag 12/13)");

        const aow::MapResource* tree = aow::find_map_resource(res, 87);
        check(tree != nullptr && tree->ilb == "sn_tree.BMP",
              "resource id 87 is the sn_tree.BMP terrain object");
        check(tree != nullptr && tree->has_shift,
              "terrain-object resources carry the TMultiHexMOR shift");

        const aow::MapResource* grass = aow::find_map_resource(res, 0x06cf8f4b);
        check(grass != nullptr && grass->class_id == 0x20153,
              "0x06cf8f4b is a Hexagon.THexagonResource");
        check(grass != nullptr && !grass->has_shift,
              "plain terrain hexagons carry no TMultiHexMOR shift");
    }

    // --- Phase 4b: roads. Road objects (engine class 0x20164 Road.TRoad) carry
    // their overlay library INLINE (Road.TRoadResource class 0x20165, rw tag 8,
    // entries `Rd_*.BMP` at slots 1..8). The transition byte comes from
    // Road.TRoad.UpdateTransitions@55616f8c and the frames from
    // Road.TRoadResource.Show@5561713c. See src/engine/mapviz.cpp (road pass).
    {
        const aow::MapResource* road = aow::find_map_resource(res, 38);
        check(road != nullptr && road->class_id == 0x20165,
              "resource id 38 is a Road.TRoadResource (0x20165)");
        check(road != nullptr && road->ilb == "Rd_gr.BMP",
              "the grass road resource names Rd_gr.BMP");
        check(road != nullptr && !road->ilb_data.empty(),
              "the road resource carries its overlay library inline (tag 8)");
        if (road != nullptr && !road->ilb_data.empty()) {
            aow::IlbFile ilb;
            std::string ie;
            const bool ok = ilb.load_memory(road->ilb_data.data(),
                                            road->ilb_data.size(), ie);
            std::size_t slots = 0;
            bool names_ok = true, dims_ok = true;
            for (const aow::IlbEntry& e : ilb.entries()) {
                ++slots;
                std::string nm = e.name;
                for (char& c : nm) c = static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c)));
                if (nm != "rd_gr.bmp") names_ok = false;
                if (e.width != 60 || e.height != 50) dims_ok = false;
            }
            check(ok && slots == 8,
                  "the inline road library holds exactly 8 overlay frames");
            check(names_ok, "every road overlay entry is named after the resource");
            check(dims_ok, "every road overlay uses a 60x50 placement cell");
        }

        // The transition byte (Road.TRoad.UpdateTransitions). A 5x5 grid, road
        // at (2,2) (even column -> no stagger fold) plus selected neighbours.
        std::vector<std::uint8_t> g(25, 0);
        auto at = [&](int x, int y) -> std::uint8_t& {
            return g[static_cast<std::size_t>(y) * 5 + x];
        };
        at(2, 2) = 1;
        check(aow::road_transition_byte(2, 2, 5, 5, g) == 0,
              "an isolated road has transition byte 0");
        at(3, 2) = 1;  // E (dir 3)
        at(1, 2) = 1;  // W (dir 5)
        check(aow::road_transition_byte(2, 2, 5, 5, g) == 0x40,
              "an E/W road collapses to the 0x40 straight bit");
        at(3, 2) = 0; at(1, 2) = 0;
        at(3, 1) = 1;  // NE (dir 2)
        at(1, 1) = 1;  // NW (dir 6)
        check(aow::road_transition_byte(2, 2, 5, 5, g) == 0x80,
              "a NE/NW road collapses to the 0x80 straight bit");
        at(3, 1) = 0; at(1, 1) = 0;
        at(2, 1) = 1;  // N (dir 1)
        at(2, 3) = 1;  // S (dir 4)
        check(aow::road_transition_byte(2, 2, 5, 5, g) == 0x09,
              "an N/S road sets direction bits 0 and 3");

        // The frame selection (Road.TRoadResource.Show): 1..6 = arms, 7/8 = the
        // two straights, 0 also draws frame 7 (isolated).
        std::vector<std::size_t> fr;
        aow::road_frames(0x00, fr);
        check(fr.size() == 1 && fr[0] == 7, "byte 0 draws frame 7 (isolated)");
        aow::road_frames(0x40, fr);
        check(fr.size() == 1 && fr[0] == 7, "the 0x40 straight draws frame 7");
        aow::road_frames(0x80, fr);
        check(fr.size() == 1 && fr[0] == 8, "the 0x80 straight draws frame 8");
        aow::road_frames(0x09, fr);
        check(fr.size() == 2 && fr[0] == 1 && fr[1] == 4,
              "an N/S road draws the N (1) and S (4) arms");

        // Roads also connect toward *structures* (message 0x1131,
        // AoWE.TStructure.MapFieldMsgProc@5575f730): a city/building is
        // road-connectable, terrain decorations/mountains/crops/units are not.
        check(aow::road_connectable_object(0x20108),
              "a City.TCity tile is road-connectable");
        check(aow::road_connectable_object(0x2043e),
              "a Tower.TTower tile is road-connectable");
        check(aow::road_connectable_object(0x2037a),
              "a Mine.TMine tile is road-connectable");
        check(!aow::road_connectable_object(0x20166),
              "a Terrain.TTerrainMO (tree/rock) tile is not road-connectable");
        check(!aow::road_connectable_object(0x20168),
              "a Mountain.TMountainMO tile is not road-connectable");
        check(!aow::road_connectable_object(aow::kRoadClassId),
              "a road tile is not a structure");
    }

    // Integration: Tutorial level 0 places grass roads; every drawn road overlay
    // must come from the inline Rd_gr library at frame 1..8 and resolve.
    {
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map rmap;
        std::string rerr;
        if (!aow::load_map(tpath, rmap, rerr)) {
            check(false, "load Tutorial.hsm for the road check (" + rerr + ")");
        } else {
            const std::string rcsv =
                (fs::temp_directory_path() / "aow_road.csv").string();
            aow::MapVizOptions ropt;
            ropt.level = 0;
            ropt.debug_csv = rcsv;
            std::vector<std::uint8_t> rr;
            std::uint32_t rw = 0, rh = 0;
            if (!aow::render_map_rgba(rmap, res, root, ropt, rr, rw, rh, rerr)) {
                check(false, "render Tutorial L0 for the road check (" + rerr + ")");
            } else {
                std::ifstream in(rcsv);
                std::string line;
                std::size_t roads = 0, bad_name = 0, bad_frame = 0, bad_ok = 0;
                std::vector<long> fr_18_21, fr_18_23;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("road,", 0) != 0) continue;
                    ++roads;
                    const std::size_t p = line.find("Rd_gr.BMP,");
                    if (p == std::string::npos) { ++bad_name; continue; }
                    const long f = std::strtol(line.c_str() + p + 10, nullptr, 10);
                    if (f < 1 || f > 8) ++bad_frame;
                    if (line.rfind("road,18,21,", 0) == 0) fr_18_21.push_back(f);
                    if (line.rfind("road,18,23,", 0) == 0) fr_18_23.push_back(f);
                    const std::size_t last = line.find_last_of(',');
                    if (last == std::string::npos || last + 1 >= line.size() ||
                        line[last + 1] != '1') {
                        ++bad_ok;
                    }
                }
                // The Tutorial has a mine (STR_MINE) at (18,22) with a road on
                // both sides; each road must now draw the arm leading into it
                // (the engine's TStructure 0x1131 field query).
                auto has_frame = [](const std::vector<long>& fs, long want) {
                    for (long f : fs) {
                        if (f == want) return true;
                    }
                    return false;
                };
                check(has_frame(fr_18_21, 4),
                      "road (18,21) draws a S arm toward the mine at (18,22)");
                check(has_frame(fr_18_23, 1),
                      "road (18,23) draws a N arm toward the mine at (18,22)");
                std::error_code ec6;
                fs::remove(rcsv, ec6);
                (void)rw; (void)rh;
                check(roads > 30, "Tutorial level 0 draws grass road overlays");
                check(bad_name == 0, "every road overlay comes from Rd_gr.BMP");
                check(bad_frame == 0, "every road overlay frame is in 1..8");
                check(bad_ok == 0, "every road overlay sprite resolved (`ok=1`)");
            }
        }
    }

    // --- Phase 4d: bridges. HxBridge.THexagonBridge (engine class 0x2087a) is a
    // *single* sprite whose three-frame library (`Str_brdg.bmp` surface /
    // `U_brdg.bmp` underground) is carried INLINE (HxBridge.THexagonBridgeResource
    // class 0x2087b, rw tag 8). The scenario stores only the resource id; the
    // engine recomputes the orientation from the two opposite land neighbours
    // (HxBridge.THexagonBridge.UpdateTransitions@55799fd8 -> DirectionValid
    // @55799c04) and draws library slot `direction` (Show@55799a70) at
    // HXtoHP + (-0x17,-0x14). See src/engine/mapviz.cpp (bridge pass).
    {
        std::size_t nbridge = 0;
        bool have_surface = false, have_under = false, libs_ok = true;
        for (const aow::MapResource& r : res) {
            if (r.class_id != 0x2087b) continue;
            ++nbridge;
            if (r.ilb == "Str_brdg.bmp") have_surface = true;
            if (r.ilb == "U_brdg.bmp") have_under = true;
            if (r.ilb_data.empty()) { libs_ok = false; continue; }
            aow::IlbFile ilb;
            std::string ie;
            if (!ilb.load_memory(r.ilb_data.data(), r.ilb_data.size(), ie) ||
                ilb.entries().size() != 3) {
                libs_ok = false;
                continue;
            }
            for (const aow::IlbEntry& e : ilb.entries()) {
                if (e.width != 78 || e.height != 65) libs_ok = false;
            }
        }
        check(nbridge == 5,
              "Release.hss carries 5 HxBridge.THexagonBridgeResource records");
        check(have_surface && have_under,
              "the bridge resources use Str_brdg.bmp and U_brdg.bmp");
        check(libs_ok, "every bridge resource has a 3-frame 78x65 inline library");

        // AoWE.Land@5575aea4: 0 water, 6 ice, 9 lava, 10 dirt, 13 upper-ice only.
        check(!aow::bridge_land(0) && !aow::bridge_land(6) &&
              !aow::bridge_land(9) && !aow::bridge_land(10) &&
              !aow::bridge_land(13),
              "water/ice/lava/dirt/upper-ice are not land");
        check(aow::bridge_land(1) && aow::bridge_land(2) && aow::bridge_land(3) &&
              aow::bridge_land(4) && aow::bridge_land(5) && aow::bridge_land(7) &&
              aow::bridge_land(8) && aow::bridge_land(11) && aow::bridge_land(12) &&
              aow::bridge_land(14) && aow::bridge_land(15),
              "every other terrain code is land");

        check(aow::kHexBridgeAnchorDx == -0x17 && aow::kHexBridgeAnchorDy == -0x14,
              "the bridge anchor is HXtoHP + (-0x17,-0x14)");

        // hex_bridge_direction: a 5x5 grid with the bridge in even column 2 (so
        // the neighbour offsets carry no column-stagger fold).
        std::vector<std::uint8_t> g(25, 0);
        auto at = [&](int x, int y) -> std::uint8_t& {
            return g[static_cast<std::size_t>(y) * 5 + x];
        };
        check(aow::hex_bridge_direction(2, 2, 5, 5, g) == 0xff,
              "a bridge with no land neighbours is invalid (0xff)");
        at(2, 1) = 1;  // N
        check(aow::hex_bridge_direction(2, 2, 5, 5, g) == 0xff,
              "one land neighbour is not enough");
        at(2, 3) = 1;  // S
        check(aow::hex_bridge_direction(2, 2, 5, 5, g) == 0,
              "land N and S -> direction 0 (N/S)");
        at(3, 1) = 1; at(1, 2) = 1;  // NE, SW
        check(aow::hex_bridge_direction(2, 2, 5, 5, g) == 1,
              "land NE and SW -> direction 1 (NE/SW)");
        at(3, 2) = 1; at(1, 1) = 1;  // SE, NW (N/S still land)
        check(aow::hex_bridge_direction(2, 2, 5, 5, g) == 1,
              "with two valid pairs the engine's order 1,2,0 picks 1");
        at(3, 1) = 0; at(1, 2) = 0;  // drop NE/SW
        check(aow::hex_bridge_direction(2, 2, 5, 5, g) == 2,
              "land SE and NW -> direction 2 (SE/NW)");
        at(2, 1) = 0; at(2, 3) = 0; at(3, 2) = 0; at(1, 1) = 0;
        check(aow::hex_bridge_direction(0, 0, 5, 5, g) == 0xff,
              "a corner hex whose neighbours are off-map is invalid");
    }

    // Integration: Dwarf1.CSM levels 1/2 place 6 bridges each. Every drawn bridge
    // must come from the inline U_brdg.bmp library, use a frame 0..2 (the
    // recomputed orientation) and resolve.
    {
        const std::string dpath =
            (fs::path(root) / "Scenario/Campaign/Dwarf1.CSM").string();
        aow::Map dmap;
        std::string derr;
        if (!aow::load_map(dpath, dmap, derr)) {
            check(false, "load Dwarf1.CSM for the bridge check (" + derr + ")");
        } else if (dmap.levels.size() >= 2) {
            const std::string bcsv =
                (fs::temp_directory_path() / "aow_bridge.csv").string();
            aow::MapVizOptions bopt;
            bopt.level = 1;
            bopt.debug_csv = bcsv;
            std::vector<std::uint8_t> br;
            std::uint32_t bw = 0, bh = 0;
            if (!aow::render_map_rgba(dmap, res, root, bopt, br, bw, bh, derr)) {
                check(false,
                      "render Dwarf1 L1 for the bridge check (" + derr + ")");
            } else {
                std::ifstream in(bcsv);
                std::string line;
                std::size_t bridges = 0, bad_name = 0, bad_frame = 0, bad_ok = 0;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("bridge,", 0) != 0) continue;
                    ++bridges;
                    const std::size_t p = line.find("U_brdg.bmp,");
                    if (p == std::string::npos) {
                        ++bad_name;
                    } else {
                        const long f = std::strtol(line.c_str() + p + 11,
                                                   nullptr, 10);
                        if (f < 0 || f > 2) ++bad_frame;
                    }
                    const std::size_t last = line.find_last_of(',');
                    if (last == std::string::npos || last + 1 >= line.size() ||
                        line[last + 1] != '1') {
                        ++bad_ok;
                    }
                }
                std::error_code ecb;
                fs::remove(bcsv, ecb);
                (void)bw; (void)bh;
                check(bridges == 6, "Dwarf1 level 1 draws all 6 bridge objects");
                check(bad_name == 0, "every bridge uses the inline U_brdg.bmp");
                check(bad_frame == 0,
                      "every bridge frame is a computed 0..2 direction");
                check(bad_ok == 0, "every bridge sprite resolved (`ok=1`)");
            }
        }
    }

    // TTerrainList (rw tag 7) - the hexagon resource's terrain type, the input the
    // transition/coast logic compares. TTerrainList.LoadFromStream serialises
    // `u32 count` + `count` u16; GetTerrain(list,0) returns entry 0's low byte:
    // 0 water, 1 grass, 2 desert, 3 snow, 4 steppe, 5 wasteland, 6 ice, 7 earth,
    // 8 rock, 14 coast, 15 border. Evidence:
    // re/exports/hsepack/HSEngine.TTerrainList.{LoadFromStream,GetTerrain}.c.
    {
        struct Tr {
            std::uint32_t id;
            int terr;
            const char* what;
        } trs[] = {
            {0x007b134f, 0, "WATERHEX.ILB water"},
            {0x06cf8f4b, 1, "H_gr.BMP grass"},
            {0x0296b6d7, 2, "H_de.BMP desert"},
            {0x0d81b96a, 6, "H_ic.BMP ice"},
            {0x00af9569, 6, "ICEHEX.ILB ice"},
            {0x00000018, 14, "H_co.BMP coast"},
            {0x061b0a02, 15, "H_Border.BMP border"},
            {0x00000001, 7, "EARTHHEX.ILB earth"},
        };
        for (const Tr& t : trs) {
            const aow::MapResource* r = aow::find_map_resource(res, t.id);
            check(r != nullptr && r->has_terrain && r->terrain == t.terr,
                  std::string("terrain of ") + t.what + " is " +
                      std::to_string(t.terr));
        }
    }

    // Water-hexagon shoreline shelf selection (IsoHex.TLowerIsometricHexagon.
    // ShowDynamic + AoWHex.TAoWWaterHexagon.UpdateTransitions): a 6-bit
    // flat-land-neighbour mask -> the WATERHEX/ICEHEX image slots to blit, all at
    // y = HXtoHP - 0x20 or -0x1f.
    {
        std::vector<aow::ShorePiece> p;
        aow::water_shore_pieces(0, 0, p);
        check(p.empty(), "no flat-land neighbour -> no shoreline shelf");
        aow::water_shore_pieces(0x01, 0, p);  // land due N
        check(p.size() == 1 && p[0].frame == 0x0d,
              "N land edge selects slot 0x0d");
        aow::water_shore_pieces(0x02, 0, p);  // land NE
        check(p.size() == 1 && p[0].frame == 0x11,
              "NE land edge selects slot 0x11");
        aow::water_shore_pieces(0x04, 0, p);  // land E
        check(p.size() == 1 && p[0].frame == 0x0f,
              "E land edge selects slot 0x0f");
        aow::water_shore_pieces(0x08, 0, p);  // land S
        check(p.size() == 1 && p[0].frame == 0x17,
              "S land edge selects slot 0x17");
        // Invariant over every mask/phase: frames land in the `t_wa-gr` slot run
        // 0..0x1f and the anchor is one of the two engine offsets.
        bool inv = true;
        for (int m = 0; m < 64; ++m) {
            for (int v = 0; v < 3; ++v) {
                aow::water_shore_pieces(m, v, p);
                for (const aow::ShorePiece& s : p) {
                    if (s.frame < 0 || s.frame > 0x1f) inv = false;
                    if (s.dy != -0x20 && s.dy != -0x1f) inv = false;
                }
            }
        }
        check(inv, "every shoreline piece is a slot 0..0x1f at -0x20/-0x1f");
    }

    // Integration: Tutorial level 0 is an island, so its water border tiles draw
    // the WATERHEX shoreline shelf toward the grass/steppe land. Every coast piece
    // must come from WATERHEX.ILB (slot < 32) and resolve.
    {
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map cmap;
        std::string cerr;
        if (!aow::load_map(tpath, cmap, cerr)) {
            check(false, "load Tutorial.hsm for the coast check (" + cerr + ")");
        } else {
            const std::string ccsv =
                (fs::temp_directory_path() / "aow_coast.csv").string();
            aow::MapVizOptions copt;
            copt.level = 0;
            copt.objects = false;
            copt.debug_csv = ccsv;
            std::vector<std::uint8_t> cr;
            std::uint32_t cw = 0, ch = 0;
            if (!aow::render_map_rgba(cmap, res, root, copt, cr, cw, ch, cerr)) {
                check(false, "render Tutorial L0 for the coast check (" + cerr + ")");
            } else {
                std::ifstream in(ccsv);
                std::string line;
                std::size_t coast = 0, bad_name = 0, bad_frame = 0, bad_ok = 0;
                std::size_t foam = 0, foam_bad = 0;
                std::map<int, std::size_t> wave;  // the water base-tile slots used
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    const std::size_t wh = line.find("WATERHEX.ILB,");
                    const long frame = (wh == std::string::npos)
                                           ? -1
                                           : std::strtol(line.c_str() + wh + 13,
                                                         nullptr, 10);
                    if (line.rfind("coast,", 0) == 0) {
                        ++coast;
                        if (wh == std::string::npos) {
                            ++bad_name;
                            continue;
                        }
                        if (frame < 0 || frame > 0x1f) ++bad_frame;
                        const std::size_t last = line.find_last_of(',');
                        if (last == std::string::npos || last + 1 >= line.size() ||
                            line[last + 1] != '1') {
                            ++bad_ok;
                        }
                        continue;
                    }
                    // AoWHex.TAoWWaterHexagon.Show@5579aeac: the `T_Ic-Wa2` foam
                    // where this water hex borders ice (Tutorial L0 has none).
                    if (line.rfind("foam,", 0) == 0) {
                        ++foam;
                        if (wh == std::string::npos ||
                            (frame != aow::kWaterFoamBit0Frame &&
                             frame != aow::kWaterFoamBit1Frame &&
                             frame != aow::kWaterFoamBit5Frame)) {
                            ++foam_bad;
                        }
                        continue;
                    }
                    // The base tile: one of the 16 H_Wa2 wave frames at 0x8c..0x9b.
                    if (line.rfind("terrain,", 0) == 0 && wh != std::string::npos) {
                        ++wave[static_cast<int>(frame)];
                    }
                }
                std::error_code ec7;
                fs::remove(ccsv, ec7);
                (void)cw; (void)ch;
                check(coast > 100, "Tutorial L0 draws water shoreline shelf pieces");
                check(bad_name == 0, "every shelf piece comes from WATERHEX.ILB");
                check(bad_frame == 0, "every shelf piece slot is in 0..0x1f");
                check(bad_ok == 0, "every shelf piece sprite resolved (`ok=1`)");
                // `Show`'s foam needs a *neighbour of the hexagon's own water/ice
                // family with a different terrain* (UpdateTransitions' nesting: the
                // +0x15 bit is only set in the `TLowerIsometricHexagon != 0` branch),
                // so a map with no ice draws none - not even along the map border.
                check(foam == 0 && foam_bad == 0,
                      "Tutorial L0 has no ice, so no water hexagon draws foam "
                      "(" + std::to_string(foam) + " rows)");
                bool wave_ok = !wave.empty();
                for (const auto& kv : wave) {
                    if (kv.first < 0x8c || kv.first > 0x9b) wave_ok = false;
                }
                check(wave_ok,
                      "every water base tile is a H_Wa2 wave frame (0x8c..0x9b)");
                check(wave.size() >= 12,
                      "the ocean spreads over the wave phases (" +
                          std::to_string(wave.size()) + "/16 slots used)");
            }
        }
    }

    // The water hexagon's foam needs ICE: `Show@5579aeac` draws the three
    // `T_Ic-Wa2` slots (0x23/0x24/0x25) for `sprite+0x15` bits 0/1/5, which
    // `UpdateTransitions@5579ad98` only sets for a neighbour of the hexagon's own
    // water/ice family (`IsoHex.TLowerIsometricHexagon`) whose terrain *differs*.
    // Icedale ships both, and its ICEHEX.ILB carries the same 115-entry layout as
    // WATERHEX.ILB - so an ice hexagon draws the foam too, from its own library.
    {
        const std::string ipath =
            (fs::path(root) / "Scenario/Custom/Icedale.hsm").string();
        aow::Map imap;
        std::string ierr;
        if (!aow::load_map(ipath, imap, ierr)) {
            check(false, "load Icedale.hsm for the foam check (" + ierr + ")");
        } else {
            const std::string icsv =
                (fs::temp_directory_path() / "aow_ice.csv").string();
            aow::MapVizOptions iopt;
            iopt.level = 0;
            iopt.objects = false;
            iopt.debug_csv = icsv;
            std::vector<std::uint8_t> ir;
            std::uint32_t iw = 0, ih = 0;
            if (!aow::render_map_rgba(imap, res, root, iopt, ir, iw, ih, ierr)) {
                check(false, "render Icedale L0 for the foam check (" + ierr + ")");
            } else {
                std::ifstream in(icsv);
                std::string line;
                std::size_t foam = 0, bad = 0, ice = 0, ice_wave_bad = 0;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("foam,", 0) == 0) {
                        ++foam;
                        // The debug CSV's entry column names the resolved sprite:
                        // both libraries ship the foam under `T_Ic-Wa2.BMP`.
                        if (line.find("t_ic-wa2.bmp") == std::string::npos) ++bad;
                        continue;
                    }
                    if (line.rfind("terrain,", 0) == 0 &&
                        line.find("ICEHEX.ILB,") != std::string::npos) {
                        ++ice;
                        const std::size_t p = line.find("ICEHEX.ILB,");
                        const long f =
                            std::strtol(line.c_str() + p + 11, nullptr, 10);
                        if (f < 0x8c || f > 0x9b) ++ice_wave_bad;
                    }
                }
                std::error_code ec8;
                fs::remove(icsv, ec8);
                check(foam > 20,
                      "Icedale's water/ice boundary draws the T_Ic-Wa2 foam (" +
                          std::to_string(foam) + " sprites)");
                check(bad == 0, "every foam sprite is a T_Ic-Wa2 entry");
                check(ice > 100 && ice_wave_bad == 0,
                      "an ice hexagon draws H_Wa2-style wave frames from "
                      "ICEHEX.ILB too (" + std::to_string(ice) + " tiles)");
                std::printf("water foam: %zu sprites on Icedale L0, %zu ice tiles\n",
                            foam, ice);
            }
        }
    }

    // Integration: placed armies (docs/13_UNITS_ON_MAP.md). Tutorial level 0's
    // four AoWE.TArmyHS objects must each draw their lead unit's sprite: the
    // debug CSV names the resolved library entry, so every unit row resolves
    // (`ok=1`) and the Giant-led army comes from GIANT.ILB. Each army also casts
    // its ground shadow (`AoWE.TUnitShadowHS` -> `TUnitHS.ShowShadow@0x55784504`,
    // docs/16_UNIT_ANIM.md section 10): one `shadow` row per army, out of
    // `UNITS\USHADOW.ILB`.
    {
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map umap;
        std::string uerr;
        if (!aow::load_map(tpath, umap, uerr)) {
            check(false, "load Tutorial.hsm for the unit check (" + uerr + ")");
        } else {
            const std::string ucsv =
                (fs::temp_directory_path() / "aow_units.csv").string();
            aow::MapVizOptions uopt;
            uopt.level = 0;
            uopt.debug_csv = ucsv;
            std::vector<std::uint8_t> ur;
            std::uint32_t uw = 0, uh = 0;
            if (!aow::render_map_rgba(umap, res, root, uopt, ur, uw, uh, uerr)) {
                check(false, "render Tutorial L0 for the unit check (" + uerr + ")");
            } else {
                std::ifstream in(ucsv);
                std::string line;
                std::size_t units = 0, bad_ok = 0, giant = 0;
                std::size_t shadows = 0, bad_shadows = 0;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("shadow,", 0) == 0) {
                        ++shadows;
                        const std::size_t last = line.find_last_of(',');
                        if (last == std::string::npos || last + 1 >= line.size() ||
                            line[last + 1] != '1' ||
                            line.find("ushadow.bmp") == std::string::npos) {
                            ++bad_shadows;
                        }
                        continue;
                    }
                    if (line.rfind("unit,", 0) != 0) continue;
                    ++units;
                    const std::size_t last = line.find_last_of(',');
                    if (last == std::string::npos || last + 1 >= line.size() ||
                        line[last + 1] != '1') {
                        ++bad_ok;
                    }
                    // The Giant's army carries `army_direction` 5, so the engine's
                    // standing pose is seq[dir-1] = seq 4 -> GIANT.ILB frame 609
                    // (TUnitHS.Show@0x55784614; docs/16_UNIT_ANIM.md).
                    if (line.find(",giant609.bmp") != std::string::npos) ++giant;
                }
                std::error_code ec8;
                fs::remove(ucsv, ec8);
                (void)uw; (void)uh;
                check(units == 4, "Tutorial L0 draws its four army lead-unit sprites");
                check(bad_ok == 0, "every army lead-unit sprite resolved (`ok=1`)");
                check(giant == 1,
                      "the Giant-led army draws its standing pose giant609.bmp");
                check(shadows == 4,
                      "Tutorial L0 casts one ground shadow per army (" +
                          std::to_string(shadows) + ")");
                check(bad_shadows == 0,
                      "every shadow row resolves to UNITS\\USHADOW.ILB");
            }
        }
    }

    // Integration: the unit `bin/ShadowRef.png` shows (docs/16_UNIT_ANIM.md
    // section 10). Bloody Marauders' Human leader army (THeroResource 36, the
    // "Human Hero Leader" gfx record) stands on hex (31,45) facing 3, so
    // `TUnitHS.ShowShadow` picks slot 26 (`dir + 0x17`) - a mode-0 sequence whose
    // base frame is USHADOW 7, anchored at `seq.xoff/yoff (-16,-23) + the
    // sprite's base+shift (12,41)` = `HXtoHP + (-4,18)`. Its facing-4 sibling
    // (slot 27 -> USHADOW 26 at `HXtoHP + (-1,18)`) is the sprite the reference
    // screenshot has under the same unit after a move south; `unit_anim_test`
    // locks that one directly.
    {
        const std::string bpath =
            (fs::path(root) / "Scenario/Custom/Bloody Marauders.hsm").string();
        aow::Map bmap;
        std::string berr;
        if (!aow::load_map(bpath, bmap, berr)) {
            check(false, "load Bloody Marauders.hsm for the shadow check (" +
                             berr + ")");
        } else {
            const std::string bcsv =
                (fs::temp_directory_path() / "aow_shadow.csv").string();
            aow::MapVizOptions bopt;
            bopt.level = 0;
            bopt.debug_csv = bcsv;
            std::vector<std::uint8_t> br;
            std::uint32_t bw = 0, bh = 0;
            if (!aow::render_map_rgba(bmap, res, root, bopt, br, bw, bh, berr)) {
                check(false, "render Bloody Marauders L0 for the shadow check (" +
                                 berr + ")");
            } else {
                std::ifstream in(bcsv);
                std::string line;
                std::size_t leader = 0;
                std::size_t shadows = 0;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("shadow,", 0) == 0) ++shadows;
                    if (line.rfind("shadow,31,45,36,UNITS,7,ushadow.bmp,26,13,1",
                                   0) == 0) {
                        ++leader;
                    }
                }
                std::error_code ec9;
                fs::remove(bcsv, ec9);
                (void)bw; (void)bh;
                check(shadows > 20,
                      "Bloody Marauders L0 casts a shadow per army (" +
                          std::to_string(shadows) + ")");
                check(leader == 1,
                      "the Human leader's shadow is USHADOW 7 (26x13) at (31,45)");
            }
        }
    }

    // --- Building animations (docs/17_BUILDING_ANIM.md). A structure's sprite is
    // the image-sequence CHAIN ShowComplexImage walks from the object's cell
    // image: a static body plus animated `.next` sequences - the mine's sparkle
    // (`STR_MINE.ILB` mode 5, frames 10..15), the wizard tower's glowing orb
    // (`STR_WTWR.ILB` mode 4, frames 10..21). Each sequence's frame comes from
    // ImageLib.TImageSequence.GetFrameIndex@0x552186ec with the map clock
    // (`AoWHSMap+0x40`) and the per-object phase.
    {
        // The phase the engine hands GetFrameIndex for a structure:
        // AoWE.TStructureResource.Show@557605dc passes hex_x + hex_y*0x10.
        check(aow::object_anim_phase(5, 3) == 5 + 3 * 0x10,
              "the object animation phase is hex_x + hex_y*0x10");
        check(aow::object_anim_phase(0, 0) == 0, "phase is 0 on the origin hex");
    }
    {
        // The mine: a mode-0 body chained to a mode-5 sparkle in the same library.
        const aow::MapResource* mine = aow::find_map_resource(res, 23);
        const aow::ImageSequence* body =
            mine != nullptr ? aow::body_sequence(*mine) : nullptr;
        check(body != nullptr && body->mode == 0 &&
                  body->library == "STR_MINE.ILB",
              "the mine's body sequence is the static STR_MINE.ILB");
        if (body != nullptr) {
            const std::vector<const aow::ImageSequence*> chain =
                aow::image_sequence_chain(*body);
            check(chain.size() == 2 && chain[1] != nullptr &&
                      chain[1]->mode == 5 &&
                      chain[1]->library == "STR_MINE.ILB" &&
                      chain[1]->frame_table.size() == 6 &&
                      chain[1]->frame_table.front() == 10,
                  "the mine's body chains an animated mode-5 sparkle (10..15)");
        }
    }
    {
        // The wizard's tower: a static body chained to a mode-4 12-frame orb.
        const aow::MapResource* wtwr = aow::find_map_resource(res, 97718693);
        const aow::ImageSequence* body =
            wtwr != nullptr ? aow::body_sequence(*wtwr) : nullptr;
        check(body != nullptr && body->mode == 0 &&
                  body->library == "STR_WTWR.ILB",
              "the wizard tower's body sequence is the static STR_WTWR.ILB");
        if (body != nullptr) {
            const std::vector<const aow::ImageSequence*> chain =
                aow::image_sequence_chain(*body);
            check(chain.size() == 2 && chain[1] != nullptr &&
                      chain[1]->mode == 4 &&
                      chain[1]->frame_table.size() == 12 &&
                      chain[1]->frame_table.front() == 10 &&
                      chain[1]->xoff == -11 && chain[1]->yoff == -89,
                  "the wizard tower chains the mode-4 orb (frames 10..21)");
        }
    }

    {
        // The renderer: the animated overlays are left out of the cached level
        // (no_animated_overlays) and composited per map frame by
        // draw_structure_overlays; the frames advance with the clock.
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map bmap;
        std::string berr;
        if (!aow::load_map(tpath, bmap, berr)) {
            check(false, "load Tutorial.hsm for the building-anim check (" +
                             berr + ")");
        } else {
            aow::MapVizOptions bopt;
            bopt.level = 0;
            bopt.no_animated_overlays = true;
            std::vector<std::uint8_t> base;
            std::uint32_t bw = 0, bh = 0;
            if (!aow::render_map_rgba(bmap, res, root, bopt, base, bw, bh,
                                      berr)) {
                check(false, "render Tutorial L0 without the animated "
                             "overlays (" + berr + ")");
            } else {
                std::vector<std::uint8_t> a = base, b = base;
                int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
                std::string oerr;
                const bool ok0 = aow::draw_structure_overlays(
                    bmap, res, root, 0, 0, a, bw, bh, false, x0, y0, x1, y1,
                    oerr);
                check(ok0 && x1 > x0 && y1 > y0,
                      "the animated buildings composite into the level");
                std::string oerr2;
                const bool ok12 = aow::draw_structure_overlays(
                    bmap, res, root, 0, 12, b, bw, bh, false, x0, y0, x1, y1,
                    oerr2);
                check(ok12, "the animated buildings composite at clock 12");
                check(ok0 && ok12 && a != b,
                      "the building animation advances with the map clock");
                check(a != base, "the animated overlays add pixels to the level");
            }
        }
    }

    {
        // The Structure floater's fade background hides ONE placed object. The
        // object's rw field tag (`MapHs::tag`) is only its slot inside a hex
        // record, so every tile reuses it; the renderer must match the object's
        // unique map-file offset instead. Barondir Bay has 19 cities, several of
        // them sharing that field tag - hiding by tag used to fade them all.
        const std::string bpath =
            (fs::path(root) / "Scenario/Custom/Barondir Bay.HSM").string();
        aow::Map bmap;
        std::string berr;
        if (!aow::load_map(bpath, bmap, berr)) {
            check(false, "load Barondir Bay.HSM for the object-mask check (" +
                             berr + ")");
        } else if (bmap.levels.empty()) {
            check(false, "Barondir Bay has no level for the object-mask check");
        } else {
            std::size_t city_offset = static_cast<std::size_t>(-1);
            std::uint32_t city_tag = 0;
            int cities = 0, same_tag = 0;
            for (const aow::MapTile& t : bmap.levels[0].tiles) {
                for (const aow::MapHs& hs : t.objects) {
                    if (hs.class_id != aow::kCityClassId) continue;
                    ++cities;
                    if (city_offset == static_cast<std::size_t>(-1)) {
                        city_offset = hs.offset;
                        city_tag = hs.tag;
                    } else if (hs.tag == city_tag) {
                        ++same_tag;
                    }
                }
            }
            check(cities >= 2,
                  "Barondir Bay carries several cities (" +
                      std::to_string(cities) + ")");
            check(same_tag > 0,
                  "Barondir Bay has another city with the same field tag (" +
                      std::to_string(same_tag) + ")");
            aow::MapVizOptions opt;
            opt.level = 0;
            opt.no_animated_overlays = true;
            opt.no_units = true;
            std::vector<std::uint8_t> base, masked;
            std::uint32_t w = 0, h = 0, mw = 0, mh = 0;
            std::string rerr;
            const bool ok =
                aow::render_map_rgba(bmap, res, root, opt, base, w, h, rerr);
            check(ok, "render Barondir Bay L0 for the object-mask check (" +
                          rerr + ")");
            if (ok) {
                opt.hide_object_level = 0;
                opt.hide_object_offset = city_offset;
                std::string merr;
                const bool mok =
                    aow::render_map_rgba(bmap, res, root, opt, masked, mw, mh,
                                         merr);
                check(mok && mw == w && mh == h,
                      "render Barondir Bay L0 with one city hidden (" + merr +
                          ")");
                if (mok && mw == w && mh == h && masked.size() == base.size()) {
                    std::size_t changed = 0;
                    int x0 = static_cast<int>(w), y0 = static_cast<int>(h);
                    int x1 = -1, y1 = -1;
                    for (std::uint32_t y = 0; y < h; ++y) {
                        for (std::uint32_t x = 0; x < w; ++x) {
                            const std::size_t p =
                                (static_cast<std::size_t>(y) * w + x) * 4;
                            if (base[p] == masked[p] &&
                                base[p + 1] == masked[p + 1] &&
                                base[p + 2] == masked[p + 2] &&
                                base[p + 3] == masked[p + 3]) {
                                continue;
                            }
                            ++changed;
                            if (static_cast<int>(x) < x0) x0 = static_cast<int>(x);
                            if (static_cast<int>(y) < y0) y0 = static_cast<int>(y);
                            if (static_cast<int>(x) > x1) x1 = static_cast<int>(x);
                            if (static_cast<int>(y) > y1) y1 = static_cast<int>(y);
                        }
                    }
                    const int bw = x1 >= x0 ? x1 - x0 + 1 : 0;
                    const int bh = y1 >= y0 ? y1 - y0 + 1 : 0;
                    std::printf(
                        "object mask: %d cities, %d share tag %u; one city "
                        "hides %zu px in %dx%d box\n",
                        cities, same_tag, city_tag, changed, bw, bh);
                    check(changed > 100,
                          "hiding the selected city changes its pixels (" +
                              std::to_string(changed) + ")");
                    check(bw > 0 && bh > 0 && bw < 300 && bh < 300,
                          "hide_object_offset hides only the selected city, "
                          "not every city sharing its field tag (box " +
                              std::to_string(bw) + "x" + std::to_string(bh) +
                              ")");
                }
            }
        }
    }

    {
        // The debug CSV names the animated overlay rows and their frames change
        // with the clock (the wizard tower's orb wtwr05.bmp at clock 0).
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map cmap;
        std::string cerr;
        if (!aow::load_map(tpath, cmap, cerr)) {
            check(false, "load Tutorial.hsm for the overlay CSV check (" +
                             cerr + ")");
        } else {
            std::vector<std::string> ov0, ov12;
            bool rendered = true;
            for (int pass = 0; pass < 2; ++pass) {
                const std::string csv =
                    (fs::temp_directory_path() /
                     (pass == 0 ? "aow_ov0.csv" : "aow_ov12.csv")).string();
                aow::MapVizOptions copt;
                copt.level = 0;
                copt.clock = pass == 0 ? 0 : 12;
                copt.debug_csv = csv;
                std::vector<std::uint8_t> cr;
                std::uint32_t cw = 0, ch = 0;
                std::string rerr;
                if (!aow::render_map_rgba(cmap, res, root, copt, cr, cw, ch,
                                          rerr)) {
                    rendered = false;
                }
                std::ifstream in(csv);
                std::string line;
                std::vector<std::string>& out = pass == 0 ? ov0 : ov12;
                std::getline(in, line);  // header
                while (std::getline(in, line)) {
                    if (line.rfind("overlay,", 0) == 0) out.push_back(line);
                }
                std::error_code eca;
                fs::remove(csv, eca);
            }
            check(rendered, "render Tutorial L0 for the overlay CSV check");
            check(!ov0.empty(), "the debug CSV logs the animated overlay rows");
            check(ov0.size() == ov12.size(),
                  "the same overlays draw at clock 0 and clock 12");
            bool changed = false, saw_wtwr = false;
            for (std::size_t i = 0; i < ov0.size(); ++i) {
                if (i < ov12.size() && ov0[i] != ov12[i]) changed = true;
                if (ov0[i].find("STR_WTWR.ILB") != std::string::npos &&
                    ov0[i].find("wtwr05.bmp") != std::string::npos) {
                    saw_wtwr = true;
                }
            }
            check(changed, "the overlay frames change with the clock");
            check(saw_wtwr,
                  "the wizard tower's orb (wtwr05.bmp) draws at clock 0");
        }
    }

    // --- Every building sprite must be background-free. The engine always blits
    // a class-0x16 (TSprite16) with GFXE.BltColorKey16, and the key is the u16
    // in the descriptor right after the shift fields; decode_rgb turns it into
    // alpha. This walks each shipped building the renderer draws - its body plus
    // every chained overlay sequence - and fails if any frame is still opaque.
    {
        std::map<std::string, std::string> ilbs;  // lowercase file name -> path
        std::error_code ec;
        const fs::path imgs = fs::path(root) / "Images";
        if (fs::is_directory(imgs, ec)) {
            for (auto it = fs::recursive_directory_iterator(
                     imgs, fs::directory_options::skip_permission_denied, ec);
                 it != fs::recursive_directory_iterator(); it.increment(ec)) {
                if (ec) break;
                if (!it->is_regular_file()) continue;
                std::string ext = it->path().extension().string();
                std::string name = it->path().filename().string();
                for (char& c : ext) c = static_cast<char>(std::tolower(
                    static_cast<unsigned char>(c)));
                for (char& c : name) c = static_cast<char>(std::tolower(
                    static_cast<unsigned char>(c)));
                if (ext == ".ilb") ilbs.emplace(name, it->path().string());
            }
        }
        std::size_t buildings = 0, frames = 0, opaque = 0, keyed = 0;
        std::string first_opaque;
        for (const aow::MapResource& r : res) {
            if (!r.has_seq) continue;
            const aow::ImageSequence* body = aow::body_sequence(r);
            if (body == nullptr) continue;
            ++buildings;
            for (const aow::ImageSequence* s : aow::image_sequence_chain(*body)) {
                if (s->library.empty() || aow::is_effect_library(s->library)) {
                    continue;
                }
                std::string base = fs::path(s->library).filename().string();
                for (char& c : base) c = static_cast<char>(std::tolower(
                    static_cast<unsigned char>(c)));
                const auto lit = ilbs.find(base);
                if (lit == ilbs.end()) continue;
                aow::IlbFile lib;
                std::string lerr;
                if (!lib.load(lit->second, lerr)) continue;
                const int f = aow::image_sequence_frame(
                    *s, static_cast<int>(lib.entries().size()), 0, 0);
                if (f < 0) continue;
                std::size_t pos = lib.entries().size();
                for (std::size_t i = 0; i < lib.entries().size(); ++i) {
                    if (lib.entries()[i].index == static_cast<std::uint32_t>(f)) {
                        pos = i;
                        break;
                    }
                }
                if (pos >= lib.entries().size()) continue;
                const aow::IlbEntry& ie = lib.entries()[pos];
                // Guard the decoder against a bogus frame id: a mode-4/5 table can
                // name an out-of-range slot.
                if (ie.rle_w > 1024 || ie.rle_h > 1024 || ie.width > 1024 ||
                    ie.height > 1024) {
                    continue;
                }
                if (ie.class_id == 0x16 && ie.has_color_key) {
                    ++keyed;
                }
                std::vector<std::uint8_t> px;
                std::uint32_t w = 0, h = 0;
                if (!lib.decode_rgb(pos, px, w, h, lerr)) continue;
                ++frames;
                bool has_alpha = false;
                for (std::size_t i = 3; i < px.size(); i += 4) {
                    if (px[i] == 0) { has_alpha = true; break; }
                }
                if (!has_alpha) {
                    ++opaque;
                    if (opaque <= 8) {
                        if (!first_opaque.empty()) first_opaque += ", ";
                        first_opaque += s->library + " f" +
                                        std::to_string(f) + " cls0x" +
                                        std::to_string(ie.class_id);
                    }
                }
            }
        }
        check(buildings > 20, "the building audit walks the shipped structures");
        check(frames > 40, "the building audit decodes their body + chain frames");
        check(keyed > 0, "class-0x16 building frames carry the colour key");
        check(opaque == 0,
              "no building sprite keeps its background (" + first_opaque + ")");
        std::printf("building audit: %zu structures, %zu frames, %zu keyed, "
                    "%zu opaque\n", buildings, frames, keyed, opaque);
    }

    // --- The glow blend (docs/17_BUILDING_ANIM.md section 5b). A class-0x16
    // (TSprite16) sequence overlay is drawn through the GFXE *brighten* table -
    // `ShowComplexImage` calls the image's vtable+0x84 slot and for a mask-less
    // TSprite16 that is `TSprite16.ShowTransparent@0x55211ea0` ->
    // `ShowBlended@0x55211b64`s table, i.e. `dst = max(dst, src)` per channel -
    // NOT an opaque blit (which painted the glow's dark surround over the map).
    // Measured against the original game's screenshot of `Bloody Marauders.hsm`
    // (docs/refs/glow_ref.png): on the glow pixels an opaque blit scores a mean
    // |delta| of 33.1, the brighten blend 12.5.
    {
        const std::string bpath =
            (fs::path(root) / "Scenario/Custom/Bloody Marauders.hsm").string();
        aow::Map gmap;
        std::string gerr;
        if (!aow::load_map(bpath, gmap, gerr)) {
            check(false, "load Bloody Marauders.hsm for the glow check (" +
                             gerr + ")");
        } else {
            aow::MapVizOptions gopt;
            gopt.level = 0;
            gopt.no_animated_overlays = true;
            std::vector<std::uint8_t> base;
            std::uint32_t gw = 0, gh = 0;
            if (!aow::render_map_rgba(gmap, res, root, gopt, base, gw, gh,
                                      gerr)) {
                check(false, "render Bloody Marauders.hsm without the glows (" +
                                 gerr + ")");
            } else {
                std::vector<std::uint8_t> glow = base;
                int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
                std::string oerr;
                const bool ok = aow::draw_structure_overlays(
                    gmap, res, root, 0, 0, glow, gw, gh, false, x0, y0, x1, y1,
                    oerr);
                std::size_t drawn = 0, brighter = 0, darker = 0;
                long long sum_signed = 0;
                for (std::size_t i = 0; i + 3 < glow.size(); i += 4) {
                    const bool same =
                        glow[i] == base[i] && glow[i + 1] == base[i + 1] &&
                        glow[i + 2] == base[i + 2];
                    for (int k = 0; k < 3; ++k) {
                        sum_signed += static_cast<long long>(glow[i + k]) -
                                      static_cast<long long>(base[i + k]);
                    }
                    if (same) continue;
                    ++drawn;
                    bool up = false, down = false;
                    for (int k = 0; k < 3; ++k) {
                        if (glow[i + k] > base[i + k]) up = true;
                        if (glow[i + k] < base[i + k]) down = true;
                    }
                    if (up) ++brighter;
                    if (down) ++darker;
                }
                check(ok && drawn > 200, "the glows draw over the map");
                check(brighter > darker,
                      "the animated overlays brighten the map more than they "
                      "darken it (brighten blend; " + std::to_string(brighter) +
                          " up vs " + std::to_string(darker) + " down px)");
                std::printf("glow blend: %zu px drawn, %zu brighter, %zu darker, "
                            "mean signed delta %.2f\n", drawn, brighter, darker,
                            static_cast<double>(sum_signed) /
                                (3.0 * static_cast<double>(glow.size() / 4)));
            }
        }
    }

    // --- The water hexagon's animation (docs/18_WATER_ANIM.md). --------------
    //
    // ImageLib.TImageSequence.GetUnloopedFrameIndex@0x552188a8 - the one-shot
    // resolver `AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4` drives with the
    // hexagon's own counter. Unlike GetFrameIndex it never wraps: the sequence
    // runs out and then draws nothing.
    {
        aow::ImageSequence s;
        s.mode = 0;
        s.base_frame = 7;
        check(aow::image_sequence_unlooped_frame(s, 100, 0) == 7 &&
                  aow::image_sequence_unlooped_frame(s, 100, 999) == 7,
              "a static water sequence always draws its base frame");
        s.mode = 1;    // the library's frames, once through
        s.period = 2;  // -> one frame every period+1 = 3 ticks
        check(aow::image_sequence_unlooped_frame(s, 10, 0) == 0 &&
                  aow::image_sequence_unlooped_frame(s, 10, 2) == 0 &&
                  aow::image_sequence_unlooped_frame(s, 10, 3) == 1 &&
                  aow::image_sequence_unlooped_frame(s, 10, 29) == 9,
              "a mode-1 water sequence steps every period+1 ticks");
        check(aow::image_sequence_unlooped_frame(s, 10, 30) == -1,
              "a mode-1 water sequence stops at the library's frame count");
        s.mode = 4;
        s.frame_table = {10, 11, 12};
        check(aow::image_sequence_unlooped_frame(s, 10, 0) == 10 &&
                  aow::image_sequence_unlooped_frame(s, 10, 3) == 11 &&
                  aow::image_sequence_unlooped_frame(s, 10, 6) == 12 &&
                  aow::image_sequence_unlooped_frame(s, 10, 9) == -1,
              "a mode-4 water sequence walks its frame table once");
        s.mode = 2;
        check(aow::image_sequence_unlooped_frame(s, 10, 5) == -1,
              "the ping-pong/random modes draw nothing unlooped");
        s.mode = 5;
        check(aow::image_sequence_unlooped_frame(s, 10, 5) == -1,
              "a mode-5 water sequence draws nothing unlooped either");
        s.mode = 0;
        check(aow::image_sequence_unlooped_frame(s, 10, -1) == -1,
              "a negative counter draws nothing");
    }

    // The shipped water resource's sequence list, entry by entry (the entries the
    // roll can select: 0x14..0x1d ripples, 0x1e..0x27 fish chains, 0x32..0x3a
    // SEFFECT sparkles).
    {
        const aow::MapResource* w = aow::find_map_resource(res, 0x007b134f);
        const aow::MapResource* ic = aow::find_map_resource(res, 0x00af9569);
        check(w != nullptr && w->sequences.size() == 59,
              "the water resource carries the 59-entry animation list");
        if (w != nullptr && w->sequences.size() == 59) {
            check(w->sequences[0].mode == 0 &&
                      w->sequences[0].library == "WATERHEX.ILB",
                  "water entry 0 is the static WATERHEX base/shelf library");
            const aow::ImageSequence& rip = w->sequences[20];
            check(rip.mode == 4 && rip.period == 2 &&
                      rip.library == "WATERANM.ILB" && rip.xoff == -2 &&
                      rip.yoff == -6,
                  "water entry 20 is a mode-4 WATERANM ripple at (-2,-6)");
            check(rip.frame_table.size() == 18 && rip.frame_table.front() == 10 &&
                      rip.frame_table.back() == 57,
                  "entry 20's frame table is the shipped 18-entry table");
            bool dup = true;
            for (int i = 0; i < 5; ++i) {
                if (w->sequences[static_cast<std::size_t>(20 + i)].frame_table !=
                    w->sequences[static_cast<std::size_t>(25 + i)].frame_table) {
                    dup = false;
                }
            }
            check(dup, "entries 20..29 are five ripple tables shipped twice");
            const aow::ImageSequence& fish = w->sequences[30];
            const std::vector<const aow::ImageSequence*> fc =
                aow::image_sequence_chain(fish);
            std::string fdesc;
            for (std::size_t i = 0; i < fc.size(); ++i) {
                fdesc += (i == 0 ? "[" : ",") + std::to_string(fc[i]->mode) +
                         "/" + std::to_string(fc[i]->frame_table.size()) + ":";
                if (!fc[i]->frame_table.empty()) {
                    fdesc += std::to_string(fc[i]->frame_table.front()) + ".." +
                             std::to_string(fc[i]->frame_table.back());
                }
            }
            fdesc += "]";
            // The fish (`Fish1-00..17`), then a splash (three blank frames, then
            // `FishSplas1-00..04`) and then the big splash (`FishSplas1-05..11`)
            // - the same record the engine walks with the counter, so the three
            // overlap in time.
            check(fish.mode == 4 && !fish.frame_table.empty() &&
                      fish.frame_table.front() == 100 &&
                      fish.frame_table.back() == 117 && fc.size() == 3 &&
                      !fc[1]->frame_table.empty() &&
                      fc[1]->frame_table.back() == 124 &&
                      !fc[2]->frame_table.empty() &&
                      fc[2]->frame_table.back() == 131,
                  "water entry 30 is the fish -> splash -> splash chain " + fdesc);
            check(fc[0] != nullptr &&
                      fc[0]->library.find("WATERANM") != std::string::npos,
                  "the fish chain draws WATERANM.ILB");
            const aow::ImageSequence& fx = w->sequences[50];
            check(fx.mode == 4 &&
                      fx.library.find("SEFFECT.ILB") != std::string::npos,
                  "water entry 50 is a TCOMBAT\\OBJECTS\\SEFFECT sparkle");
        }
        check(ic != nullptr && ic->sequences.size() == 1 &&
                  ic->sequences[0].mode == 0 &&
                  ic->sequences[0].library == "ICEHEX.ILB",
              "the ice resource has a single static ICEHEX sequence");
    }

    // The three phase values the engine takes from object ADDRESSES or RandInt.
    {
        std::map<int, std::size_t> hist;
        bool in_range = true;
        for (int y = 0; y < 16; ++y) {
            for (int x = 0; x < 16; ++x) {
                const int f = aow::water_base_frame(x, y);
                if (f < 0x8c || f > 0x9b) in_range = false;
                ++hist[f];
                if (aow::water_hex_variant(x, y) < 0 ||
                    aow::water_hex_variant(x, y) > 3) {
                    in_range = false;
                }
                if (aow::water_base_phase(x, y) < 0 ||
                    aow::water_base_phase(x, y) > 0xf) {
                    in_range = false;
                }
                if (aow::water_shore_variant(x, y) < 0 ||
                    aow::water_shore_variant(x, y) > 2) {
                    in_range = false;
                }
            }
        }
        check(in_range,
              "the water phases stay in range (base frame 0x8c..0x9b, variant "
              "0..3, shelf 0..2)");
        check(hist.size() >= 12,
              "the ocean spreads over the wave phases (" +
                  std::to_string(hist.size()) + "/16 slots used)");
        check(aow::water_base_frame(3, 7) ==
                  ((aow::water_base_phase(3, 7) + 3 + 7 * 2 +
                    aow::water_hex_variant(3, 7)) & 0xf) +
                      0x8c,
              "the base frame is ((phase + x + y*2 + variant) & 0xf) + 0x8c, "
              "deterministically");
    }

    // The counter (`sprite+0x18`) and the roll (`sprite+0x19`): Create seeds the
    // counter just below zero, every map frame adds one, the entry is rolled on
    // the frame it reaches exactly 0, and once its chain runs out the counter is
    // parked at `-(0x14|10) - RandInt(0x19)`.
    {
        const aow::WaterAnimOptions opt;
        // The narrow branch picks a ripple, or - on open water only, at
        // RandInt(700) odds - the fish chain. It never picks a sparkle, and a
        // water hex with a flat-land neighbour never rolls the fish.
        int ripples = 0, fish = 0, sparks = 0, closed_other = 0;
        for (int y = 0; y < 60; ++y) {
            for (int x = 0; x < 60; ++x) {
                const int i = aow::water_sequence_roll(x, y, true, 0, opt);
                if (i >= 0x14 && i <= 0x1d) ++ripples;
                else if (i >= 0x1e && i <= 0x27) ++fish;
                else ++sparks;
                const int c = aow::water_sequence_roll(x, y, false, 0, opt);
                if (c < 0x14 || c > 0x1d) ++closed_other;
            }
        }
        check(ripples > 0 && ripples > fish && sparks == 0,
              "the narrow roll picks a ripple, rarely the fish, never a sparkle "
              "(" + std::to_string(ripples) + " ripple / " +
                  std::to_string(fish) + " fish)");
        check(closed_other == 0,
              "a water hex with a land neighbour never rolls the fish");
        aow::WaterAnimOptions wide = opt;
        wide.wide_range = true;
        int w_fish = 0, w_sparks = 0;
        for (int y = 0; y < 60; ++y) {
            for (int x = 0; x < 60; ++x) {
                const int i = aow::water_sequence_roll(x, y, true, 0, wide);
                if (i >= 0x32) ++w_sparks;
                else if (i >= 0x1e && i <= 0x27) ++w_fish;
            }
        }
        check(w_sparks > 0 && w_fish > 0,
              "the wide branch rolls the SEFFECT sparkles and the fish too");

        // The gate: with engine+0x6d == 0 nothing advances.
        aow::WaterAnimOptions off = opt;
        off.dynamic = false;
        aow::WaterHexAnim g = aow::water_hex_create(2, 3);
        const int before = g.counter;
        check(aow::water_hex_advance(g, 2, 3, true, false, off).index == -1 &&
                  g.counter == before,
              "the water counter does not advance while engine+0x6d == 0");

        // Find a hex whose first roll is a ripple, then time it.
        const aow::MapResource* wr = aow::find_map_resource(res, 0x007b134f);
        int hx = -1, hy = -1, index = -1;
        for (int y = 0; y < 48 && index < 0; ++y) {
            for (int x = 0; x < 48; ++x) {
                aow::WaterHexAnim st = aow::water_hex_create(x, y);
                if (st.counter < -0x14 || st.counter > -1) continue;
                aow::WaterTick tk;
                for (int t = 0; t < 64; ++t) {
                    tk = aow::water_hex_advance(st, x, y, true, false, opt);
                    if (tk.index >= 0) break;
                    aow::water_hex_finish(st, x, y, false, opt);
                }
                if (tk.index >= 0x14 && tk.index <= 0x1d && tk.counter == 0) {
                    hx = x;
                    hy = y;
                    index = tk.index;
                    break;
                }
            }
        }
        check(hx >= 0, "a water hexagon's first roll is a 0x14..0x1d ripple");
        if (hx >= 0 && wr != nullptr) {
            const aow::ImageSequence& seq =
                wr->sequences[static_cast<std::size_t>(index)];
            // period 2 -> one frame every 3 ticks, so the entry stays live for
            // 3 * frames ticks (that is exactly why GetUnloopedFrameIndex returns
            // -1 after that: the chain is exhausted and the counter is parked).
            const int live = 3 * static_cast<int>(seq.frame_table.size());
            aow::WaterHexAnim st = aow::water_hex_create(hx, hy);
            int live_ticks = 0, first = -1;
            bool started = false;
            for (int t = 0; t < 400; ++t) {
                const aow::WaterTick tk =
                    aow::water_hex_advance(st, hx, hy, true, false, opt);
                const bool drew = tk.index >= 0 && tk.counter < live;
                if (drew) {
                    if (!started) first = t;
                    started = true;
                    ++live_ticks;
                }
                aow::water_hex_finish(st, hx, hy, drew, opt);
                if (started && !drew) break;
            }
            check(live_ticks == live,
                  "the ripple is live for exactly 3 x the frame table (" +
                      std::to_string(live_ticks) + " vs " +
                      std::to_string(live) + ")");
            // The counter starts at `-1 - RandInt(0x14)` and is incremented
            // *before* it is tested, so the ripple's first frame is the call that
            // brings it to exactly 0: `-counter0 - 1` calls in.
            check(first == -aow::water_hex_create(hx, hy).counter - 1,
                  "the first ripple starts on the frame the counter reaches 0");
            const int sleep = -st.counter;
            check(sleep >= 0x14 && sleep <= 0x14 + 0x18,
                  "the sleep after the ripple is 0x14 + RandInt(0x19)");
        }

        // The rule replays identically (the engine's RNG stand-in is positional).
        aow::WaterHexAnim a = aow::water_hex_create(hx, hy);
        aow::WaterHexAnim b = aow::water_hex_create(hx, hy);
        bool same = true;
        for (int t = 0; t < 240; ++t) {
            const aow::WaterTick ta =
                aow::water_hex_advance(a, hx, hy, true, false, opt);
            const aow::WaterTick tb =
                aow::water_hex_advance(b, hx, hy, true, false, opt);
            if (ta.index != tb.index || ta.counter != tb.counter) same = false;
            aow::water_hex_finish(a, hx, hy, ta.index >= 0, opt);
            aow::water_hex_finish(b, hx, hy, tb.index >= 0, opt);
        }
        check(same, "the water animation replays identically");
    }

    // Integration: the ripple overlay composites into the cached level only on
    // the map frames the counters say, uses only the water resource's own art,
    // and goes through the class-0x16 brighten blend (so it can never darken a
    // pixel). This is also the live front-ends' path: a level rendered with
    // `no_animated_overlays` plus one `draw_water_overlays` call per map frame.
    {
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map wmap;
        std::string werr;
        if (!aow::load_map(tpath, wmap, werr)) {
            check(false, "load Tutorial.hsm for the water animation (" + werr + ")");
        } else {
            aow::MapVizOptions wopt;
            wopt.level = 0;
            wopt.no_animated_overlays = true;
            std::vector<std::uint8_t> base;
            std::uint32_t ww = 0, wh = 0;
            if (!aow::render_map_rgba(wmap, res, root, wopt, base, ww, wh, werr)) {
                check(false,
                      "render Tutorial L0 for the water animation (" + werr + ")");
            } else {
                int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
                std::vector<std::uint8_t> f0 = base;
                check(aow::draw_water_overlays(wmap, res, root, 0, 0, f0, ww, wh,
                                               false, x0, y0, x1, y1, werr) &&
                          x1 <= x0,
                      "at map frame 0 every water hexagon is still asleep");
                int tick = -1;
                std::size_t darkened = 0, alpha_changed = 0, total_changed = 0;
                for (int t = 1; t <= 60 && tick < 0; ++t) {
                    std::vector<std::uint8_t> f = base;
                    if (!aow::draw_water_overlays(wmap, res, root, 0, t, f, ww,
                                                  wh, false, x0, y0, x1, y1,
                                                  werr)) {
                        break;
                    }
                    std::size_t changed = 0;
                    darkened = 0;
                    alpha_changed = 0;
                    for (std::size_t i = 0; i + 3 < f.size(); i += 4) {
                        bool diff = false;
                        for (int k = 0; k < 3; ++k) {
                            if (f[i + k] != base[i + k]) diff = true;
                            if (f[i + k] < base[i + k]) ++darkened;
                        }
                        if (f[i + 3] != base[i + 3]) ++alpha_changed;
                        if (diff) ++changed;
                    }
                    total_changed = changed;
                    if (changed > 0) tick = t;
                }
                check(tick > 0, "a water ripple is up within 60 map frames");
                std::printf("water animation: first ripple at map frame %d, "
                            "%zu px brightened, 0 darkened\n", tick, total_changed);
                if (tick > 0) {
                    check(darkened == 0,
                          "the water ripple only brightens the level (the "
                          "class-0x16 blend)");
                    check(alpha_changed == 0,
                          "the water ripple leaves the canvas alpha alone");
                    int cx0 = 0, cy0 = 0, cx1 = 0, cy1 = 0;
                    std::vector<std::uint8_t> f2 = base;
                    check(aow::draw_water_overlays(
                              wmap, res, root, 0, tick, f2, ww, wh, false, cx0,
                              cy0, cx1, cy1, werr, aow::WaterAnimOptions{}, 0, 0,
                              1, 1) &&
                              cx1 <= cx0,
                          "a viewport clip with no water hexes composites nothing");
                    // The live front-ends call this once per map frame with an
                    // increasing tick and their viewport as the clip: it must stay
                    // a cheap re-composite (one counter step per hexagon, only the
                    // visible sprites blitted), never a level re-render.
                    std::vector<std::uint8_t> ft = base;
                    const auto t0 = std::chrono::steady_clock::now();
                    const int first = tick + 1;
                    for (int t = 0; t < 30; ++t) {
                        int bx0 = 0, by0 = 0, bx1 = 0, by1 = 0;
                        aow::draw_water_overlays(
                            wmap, res, root, 0, first + t, ft, ww, wh, false, bx0,
                            by0, bx1, by1, werr, aow::WaterAnimOptions{}, 0, 0,
                            512, 512);
                    }
                    const auto t1 = std::chrono::steady_clock::now();
                    const double ms =
                        std::chrono::duration<double, std::milli>(t1 - t0).count() /
                        30.0;
                    std::printf("water animation: 30 clipped live frames, %.2f ms "
                                "each (level %ux%u)\n", ms, ww, wh);
                }
            }
        }
    }

    // ---------------------------------------------------------------------
    // The owner flag of a takeable (ownable) structure - docs/19_STRUCTURE_FLAGS.md
    // ---------------------------------------------------------------------
    {
        // The class subtree was read out of AoWEPACK.dpl's Delphi VMTs (every
        // class stores `vmtParent` at `vmt-36` as `parentVmt-76`) and confirmed
        // against each class's decompiled ClassID getter. A resource in the tree
        // draws an owner flag; a plain TStructureResource (Wizard's Tower, Cave,
        // Ruin, Pyramid, Gems, ...) never does.
        const std::uint32_t psr[] = {
            0x2034d,  // AoWE.TPlayerStructureResource (base)
            0x2034f, 0x203bb, 0x20823,  // TPlayerCropStructureResource /
                                        // TProductionPlaceResource / TRandomNodeResource
            0x20109,  // City.TCityResource
            0x2036f,  // Farm.TFarmResource
            0x2037b,  // Mine.TMineResource
            0x2043f,  // Tower.TTowerResource
            0x2019b,  // PowerNode.TPowerNodeResource
            0x2063f,  // ReflectingPool.TReflectingPoolResource
            0x204bf,  // Shipyard.TShipyardResource
            0x2092f,  // BldGuild.TBuildersGuildResource
            0x204b4, 0x204af, 0x204cf, 0x203df, 0x20372, 0x2094f,  // the nodes
            0x203f2,  // Altar.TAltarResource
            0x20234, 0x20236, 0x2023b, 0x2023e, 0x2024c, 0x2030d,  // the altars
        };
        bool all = true;
        for (std::uint32_t c : psr) all = all && aow::is_player_structure_resource(c);
        check(all, "every TPlayerStructureResource subclass is recognised");
        check(!aow::is_player_structure_resource(0x2034b) &&  // TStructureResource
                  !aow::is_player_structure_resource(0x2089f) &&  // TWizardsTowerResource
                  !aow::is_player_structure_resource(0x2037f) &&  // Cave.TCaveResource
                  !aow::is_player_structure_resource(0x203af),    // Gems.TGemsResource
              "a TStructureResource subclass is not an ownable structure");

        const std::uint32_t pso[] = {
            0x2034c, 0x2034e, 0x203ba, 0x20822, 0x20108, 0x2036e, 0x204be,
            0x2092e, 0x204b3, 0x204ae, 0x204ce, 0x203de, 0x20371, 0x2094e,
            0x2037a, 0x2043e, 0x2019a, 0x2063e, 0x203f1, 0x20233, 0x20235,
            0x2023a, 0x2023d, 0x2024b, 0x2030c,
        };
        all = true;
        for (std::uint32_t c : pso) all = all && aow::is_player_structure_class(c);
        check(all, "every TPlayerStructure subclass is recognised as ownable");
        check(!aow::is_player_structure_class(0x2089e) &&   // TWizardsTower
                  !aow::is_player_structure_class(0x2037e) &&  // TCave
                  !aow::is_player_structure_class(0x203ae) &&  // TGems
                  !aow::is_player_structure_class(0x2019b),    // a *resource* class id
              "a TStructure (or a resource class id) is not an ownable object");
        check(sizeof(psr) / sizeof(psr[0]) == sizeof(pso) / sizeof(pso[0]),
              "the object and resource subtrees have the same shape");

        // The two runtime flag bytes come from each class's chain of `Create`
        // calls, so they are per-class constants.
        auto by_lib = [&](const std::string& lib) -> const aow::MapResource* {
            for (const aow::MapResource& r : res) {
                if (!r.is_race && r.ilb == lib) return &r;
            }
            return nullptr;
        };
        const aow::MapResource* mine = aow::find_map_resource(res, 23);
        check(mine != nullptr && mine->is_player_structure &&
                  !mine->flag_pad_anchored && !mine->flag_neutral_when_unowned,
              "TMineResource: ownable, no pad re-anchor and no neutral flag");
        int dx = 0, dy = 0;
        aow::player_structure_flag_anchor(*mine, dx, dy);
        check(dx == 0x14 && dy == -0x23,
              "a mine keeps TPlayerStructureResource.Create's (0x14,-0x23) anchor");

        const aow::MapResource* tower = by_lib("STR_TOWR.ILB");
        if (tower != nullptr) {
            check(tower->is_player_structure && tower->flag_pad_anchored,
                  "TTowerResource re-anchors its flag from the pad index");
            check(aow::structure_pad_index(*tower) >= 1,
                  "a structure's pad index counts its terrain entries");
        }
        for (const char* lib : {"CITY1.ILB", "CITY2.ILB", "CITY3.ILB",
                                "CITY4.ILB", "STR_MINE.ILB", "STR_TOWR.ILB",
                                "PAD1.ILB"}) {
            const aow::MapResource* r = by_lib(lib);
            if (r == nullptr) continue;
            int fx = 0, fy = 0;
            aow::player_structure_flag_anchor(*r, fx, fy);
            std::printf("flag: %-12s class 0x%05x ownable=%d pad=%d anchor=(%d,%d) "
                        "neutral=%d\n",
                        lib, r->class_id, r->is_player_structure ? 1 : 0,
                        aow::structure_pad_index(*r), fx, fy,
                        r->flag_neutral_when_unowned ? 1 : 0);
        }
        // A city is the only structure that keeps drawing a flag while nobody
        // owns it (`resource+0x64`); its flag byte also carries the
        // `(level-1) << 6` pole-select bits (`TCity.UpdateFlagID@557ac8b0`).
        const aow::MapResource* city = by_lib("CITY1.ILB");
        check(city != nullptr && city->flag_neutral_when_unowned,
              "TCityResource keeps the neutral flag while unowned");

        // --- resolve_structure_flag ---------------------------------------
        std::vector<std::uint8_t> races(16, 0);
        std::vector<std::string> rlibs(16);
        for (std::size_t i = 0; i < rlibs.size(); ++i) {
            rlibs[i] = "RACES\\R" + std::to_string(i) + ".ILB";
        }
        races[0] = 0xff;  // the independents
        races[1] = 9;     // player 1 plays the Orcs
        aow::StructureFlag sf;
        const int px5 = 8 + 32 * 5;  // HXtoHP.x of hex (5, y)
        bool ok = aow::resolve_structure_flag(mine, 0x2037a /*Mine.TMine*/, 1, 0, 0,
                                             races, rlibs, 0, px5, sf);
        check(ok && sf.flag_library == rlibs[9] &&
                  sf.pole_library == "Flags.ILB" && sf.dx == 0x14 &&
                  sf.dy == -0x23 && sf.sprite_dx == -0x15 &&
                  sf.sprite_dy == -0x10,
              "a mine owned by the Orcs draws the Orc flag on Flags.ILB's pole "
              "at resource (+0x14,-0x23), flag sprite at (-0x15,-0x10)");
        if (ok) {
            const bool city_style = aow::structure_pad_index(*mine) < 2;
            const int step = aow::structure_flag_step(0, px5, city_style);
            const std::size_t want =
                (city_style ? aow::kOwnedFlagFrameCity
                            : aow::kOwnedFlagFrameStructure) +
                static_cast<std::size_t>(step);
            check(sf.flag_frame == want, "the owned flag frame follows the clock");
        }
        // ...and an *unowned* mine draws nothing (`+0x64 == 0`).
        check(!aow::resolve_structure_flag(mine, 0x2037a, 0, 0, 0, races, rlibs, 0,
                                           px5, sf),
              "an unowned mine draws no flag");
        // A non-ownable structure never draws one.
        const aow::MapResource* cave = by_lib("STR_CAVE.ILB");
        if (cave != nullptr) {
            check(!aow::resolve_structure_flag(cave, 0x2037e, 1, 0, 0, races, rlibs,
                                               0, px5, sf),
                  "a TStructure (cave) never draws an owner flag");
        }
        // A *razed* city (rw tag 0x16 bit 1, `TCity.GetRazed@557ab474`) draws
        // nothing either - `TCity.UpdateFlagID` only allows it while the city is
        // being built, which no scenario file stores.
        if (city != nullptr) {
            check(aow::resolve_structure_flag(city, 0x20108, 2, 1, 0x02, races, rlibs,
                                              0, px5, sf) == false,
                  "a razed city draws no flag");
            // An unowned city keeps the neutral flag (Greys) and, being a
            // size-1 City1 (pad index 1), the fixed ShowFlag pole.
            bool cok = aow::resolve_structure_flag(city, 0x20108, 0, 1, 0, races,
                                                   rlibs, 0, px5, sf);
            check(cok && sf.pole_frame == aow::kFlagPoleFrameCity &&
                      sf.flag_library == "Flags.ILB" &&
                      sf.flag_frame >= aow::kNeutralFlagFrameCity &&
                      sf.flag_frame < aow::kNeutralFlagFrameCity + 12,
                  "an unowned city draws the neutral flag on the city pole");
        }

        // --- the timing rule ----------------------------------------------
        // `clock` is `map_clock + HXtoHP.x`; using px 0 isolates the rule.
        check(aow::structure_flag_step(0, 0, false) == 0 &&
                  aow::structure_flag_step(0, 0, true) == 0,
              "both flag styles start at step 0");
        check(aow::structure_flag_step(1, 0, false) == 0 &&
                  aow::structure_flag_step(2, 0, false) == 1 &&
                  aow::structure_flag_step(11, 0, false) == 5,
              "ShowStructureFlag advances every 2 clock ticks (clock >> 1)");
        check(aow::structure_flag_step(3, 0, true) == 0 &&
                  aow::structure_flag_step(4, 0, true) == 1 &&
                  aow::structure_flag_step(23, 0, true) == 5,
              "ShowFlag advances every 4 clock ticks (clock >> 2)");
        check(aow::structure_flag_step(0, 40, false) == 8 &&
                  aow::structure_flag_step(0, 8, false) == 4,
              "HXtoHP.x is folded into the flag clock (columns are out of phase)");

        // --- the renderer --------------------------------------------------
        // A live front-end keeps the level rendered with `no_animated_overlays`
        // and re-composites the flags once per map frame. Tutorial level 0 has a
        // Goblin city at hex (32,21) (resource 22 -> City1 + RGOBLIN), so that
        // hex must gain pixels when the overlays are composited, and change again
        // when the clock advances.
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map tmap;
        std::string terr;
        if (aow::load_map(tpath, tmap, terr)) {
            aow::MapVizOptions opt;
            opt.level = 0;
            opt.no_animated_overlays = true;
            std::vector<std::uint8_t> base;
            std::uint32_t bw = 0, bh = 0;
            std::string rerr;
            bool rendered =
                aow::render_map_rgba(tmap, res, root, opt, base, bw, bh, rerr);
            check(rendered, "the flag test renders Tutorial level 0");
            if (rendered) {
                const int bx0 = 1010, by0 = 610, bx1 = 1070, by1 = 700;
                auto draw_at = [&](int clock, std::vector<std::uint8_t>& out,
                                   int& dirty_ok) {
                    out = base;
                    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
                    std::string oerr;
                    bool ok = aow::draw_structure_overlays(
                        tmap, res, root, 0, clock, out, bw, bh, false, x0, y0, x1,
                        y1, oerr);
                    // The flag of hex (32,21) must be inside the dirty box.
                    dirty_ok = (ok && x0 <= bx0 && y0 <= by0 && x1 >= bx1 &&
                                y1 >= by1)
                                   ? 1
                                   : 0;
                    return ok;
                };
                auto pixels_in_box = [&](const std::vector<std::uint8_t>& a,
                                         const std::vector<std::uint8_t>& b) {
                    std::size_t n = 0;
                    for (int y = by0; y < by1; ++y) {
                        for (int x = bx0; x < bx1; ++x) {
                            const std::size_t i =
                                (static_cast<std::size_t>(y) * bw + x) * 4;
                            if (a[i] != b[i] || a[i + 1] != b[i + 1] ||
                                a[i + 2] != b[i + 2]) {
                                ++n;
                            }
                        }
                    }
                    return n;
                };
                std::vector<std::uint8_t> f0, f16;
                int dirty0 = 0, dirty16 = 0;
                const bool ok0 = draw_at(0, f0, dirty0);
                const bool ok16 = draw_at(16, f16, dirty16);
                check(ok0 && ok16, "the flag overlays composite");
                check(dirty0 == 1 && dirty16 == 1,
                      "the composited dirty box covers the city's flag");
                const std::size_t added = pixels_in_box(base, f0);
                check(added > 0, "the owner flag adds pixels to the city's hex");
                const std::size_t advanced = pixels_in_box(f0, f16);
                check(advanced > 0,
                      "the flag animates: the map clock advances its frame");
                std::printf("flag: city (32,21) box gained %zu px, %zu changed "
                            "between clock 0 and 16\n", added, advanced);
            }
        }
    }

    // --- units over the buildings (docs/13_UNITS_ON_MAP.md) -----------------
    // A live front-end keeps the armies out of its cached level
    // (`MapVizOptions::no_units` leaves out the bodies *and* their ground
    // shadows) and re-composites them every map frame with `draw_unit_overlays`,
    // AFTER the buildings' animated overlays and the water: the engine sorts a
    // unit above every map object (`AoWE.TUnitHS.CalcShowPriority` = 0x68000000),
    // so a unit standing next to a building must never be covered by it.
    // Tutorial L0's four armies lock the contract:
    //   * `no_units` leaves every army (and its shadow) out of the level,
    //   * `draw_unit_overlays` reproduces exactly the level's own unit pixels,
    //   * it puts them back over whatever a building already painted,
    //   * it honours the viewport clip and the walk's hidden army.
    {
        const std::string tpath =
            (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
        aow::Map tmap;
        std::string terr;
        if (!aow::load_map(tpath, tmap, terr)) {
            check(false, "load Tutorial.hsm for the unit-overlay check (" + terr +
                             ")");
        } else {
            // The level a live front-end caches: no animated frames, no armies.
            const std::string lcsv =
                (fs::temp_directory_path() / "aow_unit_layer.csv").string();
            aow::MapVizOptions lopt;
            lopt.level = 0;
            lopt.no_animated_overlays = true;
            lopt.no_units = true;
            lopt.debug_csv = lcsv;
            std::vector<std::uint8_t> base;
            std::uint32_t bw = 0, bh = 0;
            std::string lerr;
            const bool lrendered =
                aow::render_map_rgba(tmap, res, root, lopt, base, bw, bh, lerr);
            check(lrendered,
                  "the no_units level renders Tutorial L0 (" + lerr + ")");
            if (lrendered) {
                std::size_t armies = 0;
                {
                    std::ifstream in(lcsv);
                    std::string line;
                    std::getline(in, line);  // header
                    while (std::getline(in, line)) {
                        if (line.rfind("unit,", 0) == 0 ||
                            line.rfind("shadow,", 0) == 0) {
                            ++armies;
                        }
                    }
                }
                std::error_code ecU;
                fs::remove(lcsv, ecU);
                check(armies == 0,
                      "no_units leaves every army out of the cached level (" +
                          std::to_string(armies) + " rows)");
                // The same level WITH its armies - what a one-shot render bakes.
                aow::MapVizOptions fopt = lopt;
                fopt.no_units = false;
                const std::string fcsv =
                    (fs::temp_directory_path() / "aow_unit_banner.csv").string();
                fopt.debug_csv = fcsv;
                std::vector<std::uint8_t> full;
                std::uint32_t fw = 0, fh = 0;
                std::string ferr;
                const bool frendered = aow::render_map_rgba(
                    tmap, res, root, fopt, full, fw, fh, ferr);
                check(frendered,
                      "the same level renders with its armies (" + ferr + ")");
                // TArmyHS.Show's size banner: every army draws one (the Tutorial
                // has four), and the two TLeader-led parties (Trodsen's
                // Halflings, Borok's Goblins) draw the crown. Frame numbers lock
                // the decompiled formulas: race shield count+0x1d, ownerless
                // Flags shield count+0x27, crown race+0x50.
                std::size_t banner_rows = 0, crown_rows = 0;
                {
                    std::ifstream in(fcsv);
                    std::string line;
                    while (std::getline(in, line)) {
                        if (line.rfind("banner,", 0) == 0) ++banner_rows;
                        if (line.rfind("crown,", 0) == 0) ++crown_rows;
                    }
                }
                std::error_code ecB;
                fs::remove(fcsv, ecB);
                check(banner_rows == 4,
                      "every Tutorial army draws its size shield (" +
                          std::to_string(banner_rows) + " rows)");
                check(crown_rows == 2,
                      "a TLeader-led army draws the crown (" +
                          std::to_string(crown_rows) + " rows)");
                // The per-frame composite: `base` + the unit overlay must be
                // pixel-identical (RGB) to that level - the same sprites, the same
                // anchors, the same shadow-before-body order and the same blends.
                std::vector<std::uint8_t> canvas = base;
                int ux0 = 0, uy0 = 0, ux1 = 0, uy1 = 0;
                std::string oerr;
                const bool odrawn = aow::draw_unit_overlays(
                    tmap, root, 0, 0, canvas, bw, bh, false, ux0, uy0, ux1, uy1,
                    oerr);
                check(odrawn,
                      "draw_unit_overlays composites the armies (" + oerr + ")");
                std::size_t diff = 0;
                if (frendered && odrawn && canvas.size() == full.size()) {
                    for (std::size_t i = 0; i + 3 < canvas.size(); i += 4) {
                        if (canvas[i] != full[i] ||
                            canvas[i + 1] != full[i + 1] ||
                            canvas[i + 2] != full[i + 2]) {
                            ++diff;
                        }
                    }
                }
                check(diff == 0, "the unit overlay reproduces the level's own " +
                                     std::to_string(diff) + " differing px");
                check(ux1 > ux0 && uy1 > uy0,
                      "the unit overlay reports a dirty box");
                // The Giant-led army at (33,16): its standing pose is GIANT.ILB
                // frame 609 (32x39) at `hex_origin(33,16) + (vx,vy)`.
                std::vector<std::uint8_t> spr;
                int sw = 0, sh = 0, vx = 0, vy = 0;
                std::string perr;
                int gx = 0, gy = 0;
                aow::hex_origin(33, 16, gx, gy);
                const bool pose = aow::army_sprite_rgba(
                    tmap, res, root, 0, 33, 16, spr, sw, sh, vx, vy, perr);
                check(pose, "the Giant's standing pose resolves (" + perr + ")");
                if (pose) {
                    const int ox = gx + vx, oy = gy + vy;
                    check(ux0 <= ox && uy0 <= oy && ux1 >= ox + sw &&
                              uy1 >= oy + sh,
                          "the dirty box covers the Giant's sprite");
                    // A building's animated overlay (or a ripple) composited over
                    // the unit's hex: the unit must come back on top of it. Paint
                    // the whole sprite box magenta first, then composite.
                    std::vector<std::uint8_t> over = base;
                    for (int y = oy - 8; y < oy + sh + 8; ++y) {
                        for (int x = ox - 8; x < ox + sw + 8; ++x) {
                            if (x < 0 || y < 0 || x >= static_cast<int>(bw) ||
                                y >= static_cast<int>(bh)) {
                                continue;
                            }
                            const std::size_t i =
                                (static_cast<std::size_t>(y) * bw + x) * 4;
                            over[i] = 255;
                            over[i + 1] = 0;
                            over[i + 2] = 255;
                        }
                    }
                    int bx0 = 0, by0 = 0, bx1 = 0, by1 = 0;
                    std::string berr;
                    const bool over_drawn = aow::draw_unit_overlays(
                        tmap, root, 0, 0, over, bw, bh, false, bx0, by0, bx1,
                        by1, berr);
                    std::size_t opaque = 0, still = 0, wrong = 0, magenta = 0;
                    for (int y = 0; y < sh; ++y) {
                        for (int x = 0; x < sw; ++x) {
                            const std::size_t si =
                                (static_cast<std::size_t>(y) * sw + x) * 4;
                            const std::size_t di =
                                (static_cast<std::size_t>(oy + y) * bw +
                                 static_cast<std::size_t>(ox + x)) * 4;
                            if (spr[si] == 255 && spr[si + 1] == 0 &&
                                spr[si + 2] == 255 && spr[si + 3] == 255) {
                                ++magenta;  // the fake colour: must not exist
                            }
                            if (spr[si + 3] != 255) continue;
                            ++opaque;
                            if (over[di] == 255 && over[di + 1] == 0 &&
                                over[di + 2] == 255) {
                                ++still;
                            } else if (over[di] != full[di] ||
                                       over[di + 1] != full[di + 1] ||
                                       over[di + 2] != full[di + 2]) {
                                // The army's size shield is drawn *after* the body
                                // by `TArmyHS.Show`, so a few body pixels inside
                                // the shield's box legitimately show the banner -
                                // the one-shot level (`full`) has them too.
                                ++wrong;
                            }
                        }
                    }
                    check(over_drawn,
                          "the unit overlay composites over the building (" +
                              berr + ")");
                    check(magenta == 0,
                          "the test's fake building colour is not in the "
                          "Giant's art");
                    check(opaque > 100, "the Giant's pose has opaque pixels (" +
                                            std::to_string(opaque) + ")");
                    check(still == 0,
                          "the unit covers every building pixel it overlaps (" +
                              std::to_string(still) + " left)");
                    check(wrong == 0,
                          "the unit's opaque pixels match the level's own art "
                          "+ size banner (" +
                              std::to_string(wrong) + " blended)");
                    std::printf("unit overlay: Giant %dx%d at (%d,%d), %zu opaque "
                                "px, dirty %d,%d..%d,%d\n",
                                sw, sh, ox, oy, opaque, ux0, uy0, ux1, uy1);
                    // The clip a live front-end passes (its viewport): a viewport
                    // that holds no army must leave the canvas untouched ...
                    {
                        std::vector<std::uint8_t> far = base;
                        int fx0 = 0, fy0 = 0, fx1 = 0, fy1 = 0;
                        std::string cerr2;
                        const bool clipped = aow::draw_unit_overlays(
                            tmap, root, 0, 0, far, bw, bh, false, fx0, fy0, fx1,
                            fy1, cerr2, -1, -1, -1, 0, 0, 192, 192);
                        check(clipped && fx1 <= fx0 && fy1 <= fy0,
                              "the clip rejects the armies outside the viewport");
                        check(far == base,
                              "a clipped-out army is not composited");
                    }
                    // ... and the walk's hidden army: the moving sprite stands in
                    // for it, so its pixels must not come back.
                    {
                        std::vector<std::uint8_t> hid = base;
                        int hx0 = 0, hy0 = 0, hx1 = 0, hy1 = 0;
                        std::string herr;
                        const bool hidden = aow::draw_unit_overlays(
                            tmap, root, 0, 0, hid, bw, bh, false, hx0, hy0, hx1,
                            hy1, herr, /*hide_level=*/0, /*hide_x=*/33,
                            /*hide_y=*/16);
                        std::size_t drawn_at_giant = 0;
                        for (int y = 0; y < sh; ++y) {
                            for (int x = 0; x < sw; ++x) {
                                const std::size_t i =
                                    (static_cast<std::size_t>(oy + y) * bw +
                                     static_cast<std::size_t>(ox + x)) * 4;
                                if (hid[i] != base[i] ||
                                    hid[i + 1] != base[i + 1] ||
                                    hid[i + 2] != base[i + 2]) {
                                    ++drawn_at_giant;
                                }
                            }
                        }
                        check(hidden && drawn_at_giant == 0,
                              "the walk's hidden army is not re-composited (" +
                                  std::to_string(drawn_at_giant) + " px)");
                    }
                }
            }
        }
    }

    std::printf("mapres_test: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}