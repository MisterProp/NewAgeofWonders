// main.cpp - the AoW1 C++ port bootstrap CLI.
//
// This is intentionally tiny: it exists to prove the toolchain builds and to
// give the port a place to grow the re-implemented engine. Every subcommand
// corresponds to a stage of the reverse-engineering workflow documented in
// port/docs/03_ROADMAP.md.
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "aow/game_state.hpp"
#include "aow/ilb.hpp"
#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/mapviz.hpp"
#include "aow/movement.hpp"
#include "aow/pfs.hpp"
#include "aow/pfs_types.hpp"
#include "aow/png.hpp"
#include "aow/unit_catalog.hpp"

namespace {

int usage() {
    std::printf(
        "aow - Age of Wonders 1 (C++ port bootstrap)\n"
        "usage:\n"
        "  aow ilb <file.ILB>              list entries in an ILB image library\n"
        "  aow png <file.ILB> <idx|name> <out.png> [--id]\n"
        "                                  decode one image to a PNG file\n"
        "  aow pfs <file.PFS>              list the rule-table records\n"
        "  aow pfs <file.PFS> <index>      dump one record (fields + strings)\n"
        "  aow pfs <file.PFS> --typed      dump typed records for the table\n"
        "  aow abilities|spells|units|items|heroes <file.PFS>\n"
        "  aow herores|unitgfx|faces|itemgfx|texts|terrain <file.PFS>\n"
        "                                  dump one typed rule table\n"
        "  aow roster <game-dir>           load ALL units (Unitres.pfs +\n"
        "                                  Unitgfx.pfs + Release.hss + *.ahl)\n"
        "                                  and print the roster by race\n"
        "  aow map <file.HSM> [--preview] [--res <Release.hss>]\n"
        "                                  decode a scenario/campaign map; with\n"
        "                                  --res, resolve sprites to ILB names\n"
        "  aow mapunits <file.HSM> [--root <dir>] [--level N]\n"
        "                                  list the units/heroes an army on the\n"
        "                                  map carries (--root resolves sprites)\n"
        "  aow gamestate <file.HSM> [--turns N]\n"
        "                                  load the scenario's player roster + the\n"
        "                                  turn/day loop; --turns simulates N days\n"
        "  aow move <file.HSM> [--root <dir>] [--level N] [--to X,Y]\n"
        "                                  the human player's armies: move types,\n"
        "                                  move-point table + budget, and the path\n"
        "                                  (with the Cursors.ilb overlay frames)\n"
        "  aow mapres <Release.hss>        list the world terrain/object resources\n"
        "                                  (id -> class -> ILB)\n"
        "  aow mapviz <map.HSM> <out.png> [--res <Release.hss>] [--root <dir>]\n"
        "             [--level N] [--no-objects] [--max N]\n"
        "                                  render a map level to a PNG\n"
        "  aow tbmapviz <TCMaps\\map.HSM> <out.png> [--root <dir>] [--level N]\n"
        "             [--no-objects]        render a tactical battle map\n"
        "  aow version                     print tool version\n");
    return 2;
}

// Collapses control characters and truncates a string for one-line output.
std::string one_line(const std::string& s, std::size_t maxlen) {
    std::string o;
    for (char c : s) {
        o.push_back((c == '\r' || c == '\n' || c == '\t') ? ' ' : c);
        if (o.size() >= maxlen) {
            o += "...";
            break;
        }
    }
    return o;
}

int cmd_typed(const char* path, const char* kind);
int cmd_png(const char* ilb_path, const char* which, const char* out_path,
            bool by_id = false);

int cmd_pfs(const char* path, const char* which) {
    aow::PfsFile pfs;
    std::string err;
    if (!pfs.load(path, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    const std::vector<aow::PfsRecord>& recs = pfs.records();
    std::printf("%s\n", path);
    std::printf("  crc=%s  region=[0x%zx,0x%zx)  region_bytes=%zu\n",
                pfs.has_crc() ? (pfs.crc_ok() ? "ok" : "bad") : "none",
                pfs.region_offset(), pfs.region_offset() + pfs.region_size(),
                pfs.region_size());
    std::printf("  record-list  offset=0x%zx  base=0x%zx  entries=%zu  "
                "records=%zu\n",
                pfs.list_offset(), pfs.list_base(), pfs.list_entries(),
                recs.size());
    if (recs.empty()) {
        std::printf("  (no records)\n");
        return 0;
    }

    if (which != nullptr) {
        if (std::string(which) == "--typed") return cmd_typed(path, "auto");
        char* end = nullptr;
        const long idx = std::strtol(which, &end, 10);
        if (end == which || *end != '\0' || idx < 0 ||
            static_cast<std::size_t>(idx) >= recs.size()) {
            std::fprintf(stderr, "error: record index '%s' out of range 0..%zu\n",
                         which, recs.size() - 1);
            return 1;
        }
        const aow::PfsRecord& r = recs[static_cast<std::size_t>(idx)];
        std::printf("\nrecord %ld  tag=%u  file_off=0x%zx  size=%zu  "
                    "fields@%zu  fields=%zu  strings=%zu\n",
                    idx, r.tag, r.file_offset, r.size, r.fields_offset,
                    r.fields.size(), r.strings.size());
        for (const aow::PfsField& f : r.fields) {
            std::printf("  field  tag=%-4u off=0x%-4zx size=%-4zu\n", f.tag,
                        f.offset, f.size);
        }
        for (const aow::PfsString& s : r.strings) {
            std::printf("  string @0x%-4zx  \"%s\"\n", s.offset,
                        one_line(s.text, 200).c_str());
        }
        return 0;
    }

    for (std::size_t i = 0; i < recs.size(); ++i) {
        const aow::PfsRecord& r = recs[i];
        const std::string txt =
            r.strings.empty() ? std::string() : one_line(r.strings.front().text, 64);
        std::printf("  [%4zu] tag=%-4u off=0x%-6zx size=%-6zu fields=%-3zu %s\n",
                    i, r.tag, r.file_offset, r.size, r.fields.size(),
                    txt.c_str());
    }
    return 0;
}


// Compact "NN bytes" preview for an opaque blob field.
std::string blob_info(const std::vector<std::uint8_t>& b) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%zuB", b.size());
    return buf;
}

// aow pfs <file.PFS> --typed / aow abilities|spells|units|items|heroes <file>
int cmd_typed(const char* path, const char* kind) {
    aow::PfsFile pfs;
    std::string err;
    if (!pfs.load(path, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    std::string k = kind;
    if (k == "auto") {
        // Prefer an explicit record class id; otherwise vote across records by
        // tag signature (resource tables carry TEResource tags 1+2, abilities
        // tag 5, spells tag 15).
        const std::vector<aow::PfsRecord>& recs = pfs.records();
        for (const aow::PfsRecord& r : recs) {
            aow::RecordView v(r, pfs.bytes());
            const std::uint32_t c = v.class_id();
            if (c == 0) continue;
            if (c == aow::kUnitResourceClassId) {
                k = "units";
            } else if (c == aow::kItemClassId) {
                k = "items";
            } else if (c == aow::kHeroClassId) {
                k = "heroes";
            } else if (c == aow::kHeroResourceClassId) {
                k = "herores";
            } else if (c == aow::kUnitGfxClassId) {
                k = "unitgfx";
            } else if (c == aow::kItemGfxClassId) {
                k = "itemgfx";
            } else if (c == aow::kFaceResourceClassId) {
                k = "faces";
            } else if (c == aow::kTextItemClassId) {
                k = "texts";
            }
            break;
        }
        if (k == "auto") {
            std::size_t terrain = 0, abil = 0, spell = 0;
            for (const aow::PfsRecord& r : recs) {
                aow::RecordView v(r, pfs.bytes());
                if (v.has(1) && v.has(2)) {
                    ++terrain;
                } else if (v.has(5)) {
                    ++abil;
                } else if (v.has(15)) {
                    ++spell;
                }
            }
            k = (terrain >= abil && terrain >= spell)
                    ? "terrain"
                    : (abil >= spell ? "abilities" : "spells");
        }
    }

    if (k == "ability" || k == "abilities") {
        const std::vector<aow::Ability> v = aow::load_abilities(pfs);
        std::printf("%s  abilities=%zu\n", path, v.size());
        for (const aow::Ability& a : v) {
            std::printf(
                "  [%3zu] tag=%-4u cost=%-4u moveTypes=0x%04x snd=%d seq=%d"
                "  %s\n",
                a.index, a.tag, a.cost, a.move_types, a.has_sound ? 1 : 0,
                a.has_sequence ? 1 : 0, one_line(a.description, 58).c_str());
        }
        return 0;
    }
    if (k == "spell" || k == "spells") {
        const std::vector<aow::Spell> v = aow::load_spells(pfs);
        std::printf("%s  spells=%zu\n", path, v.size());
        for (const aow::Spell& s : v) {
            std::printf(
                "  [%3zu] tag=%-4u lvl=%-3u realm=%-3u mana=%-4u research=%-5u"
                " upkeep=%-4u fx=%s anim=%s  %s\n",
                s.index, s.tag, s.level, s.sphere, s.mana_cost,
                s.research_cost, s.upkeep, blob_info(s.effect_ref).c_str(),
                blob_info(s.animation).c_str(),
                one_line(s.description, 44).c_str());
        }
        return 0;
    }
    if (k == "unit" || k == "units") {
        const std::vector<aow::UnitStat> v = aow::load_units(pfs);
        std::printf("%s  units=%zu\n", path, v.size());
        for (const aow::UnitStat& u : v) {
            std::printf(
                "  [%3zu] tag=%-4u %s %s  gfx=%-4u lvl=%-3u obtain=%-5u "
                "transport=%u\n",
                u.index, u.tag, u.name.c_str(), u.qualifier.c_str(),
                u.gfx_index, u.level, u.obtain_value, u.transport_capacity);
            std::printf(
                "        atk=%-3u def=%-3u dmg=%-3u hp=%-3u mov=%-3u res=%-3u"
                "  race=%-3u type=%u\n",
                u.attack, u.defense, u.damage, u.hit_points, u.moves,
                u.resistance, u.race_id, u.type);
            std::printf("        desc: %s\n",
                        one_line(u.description, 64).c_str());
        }
        return 0;
    }
    if (k == "item" || k == "items") {
        const std::vector<aow::Item> v = aow::load_items(pfs);
        std::printf("%s  items=%zu\n", path, v.size());
        for (const aow::Item& it : v) {
            std::printf(
                "  [%3zu] tag=%-4u gfx=%-4u type=%-3u abil=%-3u obtain=%-5u "
                "bonus=%d lib=%-4u  %s\n",
                it.index, it.tag, it.gfx_index, it.item_type,
                it.ability_count, it.obtain_value, it.bonus ? 1 : 0,
                it.library_item, one_line(it.name, 40).c_str());
        }
        return 0;
    }
    if (k == "hero" || k == "heroes") {
        const std::vector<aow::Hero> v = aow::load_heroes(pfs);
        std::printf("%s  heroes=%zu\n", path, v.size());
        for (const aow::Hero& h : v) {
            std::printf(
                "  [%3zu] tag=%-4u face=%-4u abil=%-3u xp=%-6u  %s (%s)\n",
                h.index, h.tag, h.face_index, h.ability_count, h.experience,
                h.name.c_str(), h.nickname.c_str());
        }
        return 0;
    }
    if (k == "herores" || k == "heroresources") {
        const std::vector<aow::HeroResource> v = aow::load_hero_resources(pfs);
        std::printf("%s  heroResources=%zu\n", path, v.size());
        for (const aow::HeroResource& h : v) {
            std::printf(
                "  [%3zu] tag=%-4u gfx=%-4u class=%-3u  %s  "
                "atk=%u def=%u dmg=%u hits=%u mov=%u res=%u\n",
                h.index, h.tag, h.gfx_index, h.hero_class, h.name.c_str(),
                h.attack_skill, h.defense_skill, h.damage_skill,
                h.hits_skill, h.moves_skill, h.resistance_skill);
        }
        return 0;
    }
    if (k == "unitgfx" || k == "unitgfxs") {
        const std::vector<aow::UnitGfx> v = aow::load_unit_gfx(pfs);
        std::printf("%s  unitGfx=%zu\n", path, v.size());
        for (const aow::UnitGfx& g : v) {
            std::printf(
                "  [%3zu] tag=%-4u idx=%-5u frames=%-5s  %s  %s\n",
                g.index, g.tag, g.gfx_index,
                blob_info(g.sequence).c_str(), g.image_lib.c_str(),
                g.name.c_str());
        }
        return 0;
    }
    if (k == "faces" || k == "faceres") {
        const std::vector<aow::FaceResource> v = aow::load_faces(pfs);
        std::printf("%s  faces=%zu\n", path, v.size());
        for (const aow::FaceResource& f : v) {
            std::printf("  [%3zu] tag=%-4u idx=%-5u  %s\n", f.index, f.tag,
                        f.face_index, f.image_lib.c_str());
        }
        return 0;
    }
    if (k == "itemgfx") {
        const std::vector<aow::ItemGfx> v = aow::load_item_gfx(pfs);
        std::printf("%s  itemGfx=%zu\n", path, v.size());
        for (const aow::ItemGfx& g : v) {
            std::printf("  [%3zu] tag=%-4u idx=%-5u  %s\n", g.index, g.tag,
                        g.gfx_index, g.image_lib.c_str());
        }
        return 0;
    }
    if (k == "texts" || k == "text") {
        const std::vector<aow::TextItem> v = aow::load_texts(pfs);
        std::printf("%s  texts=%zu\n", path, v.size());
        for (const aow::TextItem& t : v) {
            std::printf("  [%3zu] tag=%-4u id=%-5u name=%-16s  %s\n", t.index,
                        t.tag, t.id, t.name.c_str(),
                        one_line(t.text, 48).c_str());
        }
        return 0;
    }
    if (k == "terrain" || k == "combres") {
        const std::vector<aow::TerrainRes> v = aow::load_terrain(pfs);
        std::printf("%s  terrain=%zu\n", path, v.size());
        for (const aow::TerrainRes& t : v) {
            std::printf("  [%3zu] tag=%-4u type=%-4u idx=%-5u v14=%u\n",
                        t.index, t.tag, t.res_type, t.res_index, t.value20);
        }
        return 0;
    }
    std::fprintf(stderr, "error: unknown typed table '%s'\n", kind);
    return 1;
}

int cmd_ilb(const char* path) {
    aow::IlbFile ilb;
    std::string err;
    if (!ilb.load(path, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    const aow::IlbHeader& h = ilb.header();
    std::printf("%s\n", path);
    std::printf("  v%.2f  hdr=%u  data_off=%u  total=%u  palettes=%u  "
                "entries=%zu\n",
                h.version, h.header_size, h.data_offset, h.total_size,
                h.palette_count, ilb.entries().size());
    for (const auto& e : ilb.entries()) {
        std::printf("  [%04zx] idx=%-4u class=0x%02x fmt=%u %ux%u  %uB  %s\n",
                    e.dir_offset, e.index, e.class_id, e.fmt, e.width,
                    e.height, e.data_size, e.name.c_str());
    }
    return 0;
}

int cmd_png(const char* ilb_path, const char* which, const char* out_path,
            bool by_id) {
    aow::IlbFile ilb;
    std::string err;
    if (!ilb.load(ilb_path, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    // <which> is a numeric entry index, an image name, or - with `--id` - the
    // entry *id* stored in the directory (which differs from the vector
    // position whenever a library reuses ids, e.g. Int\Scenes\title.ilb).
    std::size_t sel = ilb.entries().size();
    char* end = nullptr;
    const long v = std::strtol(which, &end, 10);
    const bool numeric = end && *end == '\0' && v >= 0;
    if (by_id) {
        for (std::size_t i = 0; i < ilb.entries().size(); ++i) {
            const aow::IlbEntry& e = ilb.entries()[i];
            if ((numeric && e.index == static_cast<std::uint32_t>(v)) ||
                (!numeric && e.name == which)) {
                sel = i;
                break;
            }
        }
    } else if (numeric && static_cast<std::size_t>(v) < ilb.entries().size()) {
        sel = static_cast<std::size_t>(v);
    } else {
        for (std::size_t i = 0; i < ilb.entries().size(); ++i) {
            if (ilb.entries()[i].name == which) { sel = i; break; }
        }
    }
    if (sel >= ilb.entries().size()) {
        std::fprintf(stderr, "error: no entry '%s' in %s\n", which, ilb_path);
        return 1;
    }

    std::vector<std::uint8_t> rgba;
    std::uint32_t w = 0, h = 0;
    if (!ilb.decode_rgb(sel, rgba, w, h, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    if (!aow::write_png_rgba(out_path, w, h, rgba, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    const aow::IlbEntry& e = ilb.entries()[sel];
    std::printf("wrote %s  %ux%u  class=0x%02x  entry '%s'\n", out_path, w, h,
                e.class_id, e.name.c_str());
    return 0;
}

// One ASCII symbol per terrain hexagon class (level-0 visual cross-check).
const char* terrain_symbol(std::uint32_t class_id) {
    switch (class_id) {
        case 0x2073d: return "~";  // AoWHex.TAoWWaterHexagon
        case 0x20170: return "^";  // IsoHex.TUpperIsometricHexagon
        case 0x20557: return "B";  // BorderHx.TBorderHexagon
        case 0x20152: return ".";  // Hexagon.THexagon
        default: return "?";
    }
}

int cmd_mapres(const char* path, std::uint32_t only_class) {
    std::vector<aow::MapResource> res;
    std::string err;
    if (!aow::load_map_resources(path, res, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    if (only_class != 0) {
        std::printf("%s  (records with class 0x%05x)\n", path, only_class);
        for (const aow::MapResource& r : res) {
            if (r.class_id != only_class) continue;
            std::printf("    id=0x%08x type=%-4u idx=%-4u [", r.id, r.res_type,
                        r.res_index);
            for (std::size_t i = 0; i < r.ilbs.size(); ++i) {
                std::printf("%s%s", i ? ", " : "", r.ilbs[i].c_str());
            }
            std::printf("]");
            // TMultiHexMOR placement (tags 12/13); see aow/map_resources.hpp.
            if (r.has_shift) {
                std::printf("  shift=(%d,%d)", r.dx_main, r.dy_main);
            }
            std::printf("\n");
        }
        return 0;
    }
    std::printf("%s\n  resources=%zu\n", path, res.size());
    std::map<std::uint32_t, std::size_t> by_class;
    for (const aow::MapResource& r : res) ++by_class[r.class_id];
    for (const auto& kv : by_class) {
        const char* n = aow::map_class_name(kv.first);
        std::printf("  class 0x%05x n=%-5zu %s\n", kv.first, kv.second,
                    n ? n : "class_unknown");
    }
    std::printf("  --- terrain hexagon resources ---\n");
    for (const aow::MapResource& r : res) {
        if (r.class_id == 0x20153 || r.class_id == 0x20171 ||
            r.class_id == 0x20558 || r.class_id == 0x2073e) {
            std::printf("    id=0x%08x class=0x%05x type=%-4u idx=%-4u terr=%-3d %s\n",
                        r.id, r.class_id, r.res_type, r.res_index,
                        r.has_terrain ? r.terrain : -1, r.ilb.c_str());
        }
    }
    return 0;
}

int cmd_mapviz(const char* map_path, const char* out_path,
               const std::string& res_path, const std::string& root, int level,
               bool objects, int max_dim, const std::string& debug_csv) {
    aow::Map m;
    std::string err;
    if (!aow::load_map(map_path, m, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    std::vector<aow::MapResource> res;
    if (!res_path.empty() && !aow::load_map_resources(res_path, res, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    aow::MapVizOptions opt;
    opt.level = level;
    opt.objects = objects;
    opt.max_dim = max_dim;
    opt.debug_csv = debug_csv;
    if (!aow::render_map_png(m, res, root, opt, out_path, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    std::printf("wrote %s  level=%d  %ux%u hexes  resources=%zu\n", out_path,
                level, m.width, m.height, res.size());
    return 0;
}

// Renders a tactical battle map (`TCMaps\*.HSM`) with the combat renderer.
// `combat` comes from <root>\Release\comb_res.pfs; `world` from Release.hss for
// the BorderHx frame. This is the head-less path AoWTBViewer also uses.
int cmd_tbmapviz(const char* map_path, const char* out_path,
                 const std::string& root, int level, bool objects) {
    aow::Map m;
    std::string err;
    if (!aow::load_map(map_path, m, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    const std::string combat_path = root + "\\Release\\comb_res.pfs";
    const std::string world_path = root + "\\Release\\Release.hss";
    std::vector<aow::MapResource> combat, world;
    if (!aow::load_combat_resources(combat_path, combat, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    if (!aow::load_map_resources(world_path, world, err)) {
        std::fprintf(stderr, "warning: %s (border frame unavailable)\n", err.c_str());
        world.clear();
    }
    aow::TbMapVizOptions opt;
    opt.level = level;
    opt.no_objects = !objects;
    if (!aow::render_tb_map_png(m, combat, world, root, opt, out_path, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    std::printf("wrote %s  level=%d  %ux%u hexes  combat resources=%zu\n",
                out_path, level, m.width, m.height, combat.size());
    return 0;
}


// Lists the armies a scenario places on its map and the units/heroes they
// carry (docs/13_UNITS_ON_MAP.md). With --root <game-dir> each unit's on-map
// sprite library is resolved through the unit catalog / Herores table.
int cmd_mapunits(const char* path, int level, const std::string& root) {
    aow::Map m;
    std::string err;
    if (!aow::load_map(path, m, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    if (level < 0 || static_cast<std::size_t>(level) >= m.levels.size()) {
        std::fprintf(stderr, "error: level %d out of range (%zu levels)\n", level,
                     m.levels.size());
        return 1;
    }
    aow::UnitCatalog cat;
    const bool have_cat =
        !root.empty() && aow::load_unit_catalog(root, cat, err);

    const aow::MapLevel& lv = m.levels[static_cast<std::size_t>(level)];
    std::printf("%s  level %d  %ux%u\n", path, level, lv.width, lv.height);
    std::size_t armies = 0, units = 0, heroes = 0;
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const aow::MapTile& t = lv.tiles[y * lv.width + x];
            for (const aow::MapHs& hs : t.objects) {
                if (hs.class_id != aow::kMapArmyHsClassId) continue;
                ++armies;
                std::printf("  hex(%u,%u) army (%zu unit%s)\n", x, y,
                            hs.units.size(), hs.units.size() == 1 ? "" : "s");
                for (const aow::MapUnit& u : hs.units) {
                    ++units;
                    const aow::UnitGraphics* g = nullptr;
                    std::string uname;
                    if (u.is_hero) {
                        ++heroes;
                        const std::uint32_t hi = u.resource < 0
                            ? 0u
                            : static_cast<std::uint32_t>(u.resource);
                        if (have_cat) g = cat.find_hero_gfx(hi);
                        uname = u.name;
                        if (!u.nickname.empty()) uname += " " + u.nickname;
                    } else if (have_cat) {
                        for (const aow::UnitEntry& e : cat.units) {
                            if (static_cast<std::int32_t>(e.id) == u.resource) {
                                g = e.gfx;
                                uname = e.display_name;
                                break;
                            }
                        }
                    }
                    std::printf(
                        "    %-4s class=0x%05x res=%-5d lvl=%-2u %-24s %s\n",
                        u.is_hero ? "hero" : "unit", u.class_id, u.resource,
                        u.unit_level, uname.c_str(),
                        g != nullptr ? g->sprite_lib.c_str() : "");
                }
            }
        }
    }
    std::printf("  %zu armies, %zu units (%zu heroes)\n", armies, units, heroes);
    return 0;
}

int cmd_map(const char* path, bool preview, const char* res_path) {
    aow::Map m;
    std::string err;
    if (!aow::load_map(path, m, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    // Optional: resolve sprite resource ids to their ILB names (Release.hss).
    std::vector<aow::MapResource> reslist;
    if (res_path != nullptr) {
        if (!aow::load_map_resources(res_path, reslist, err)) {
            std::fprintf(stderr, "warning: %s\n", err.c_str());
        } else {
            std::printf("  resources=%zu (%s)\n", reslist.size(), res_path);
        }
    }
    const char* cname = aow::map_class_name(m.class_id);
    std::printf("%s\n", path);
    std::printf("  container=%s  top_class=0x%x (%s)  region=%zu bytes\n",
                m.compressed ? "CFS/zlib" : "raw", m.class_id,
                cname ? cname : "?", m.region.size());
    if (!m.hss_file.empty()) {
        std::printf("  hss_file=%s\n", one_line(m.hss_file, 120).c_str());
    }
    // The scenario music playlist the in-game screen plays (AoW.exe FUN_00454d2c):
    // the tag-0x3c `Sound.TSonglist` wins when it has entries, otherwise the
    // tag-0x1d `AoWE.TStartupSongList` songs, otherwise "Songs\Heartwoo.it".
    {
        const std::vector<std::string>& songs =
            !m.songs_list.empty() ? m.songs_list : m.songs_startup;
        std::printf("  songs=%zu\n", songs.size());
        for (const std::string& s : songs) {
            std::printf("    %s\n", s.c_str());
        }
    }
    std::printf("  size=%ux%u hexes  levels=%zu\n", m.width, m.height,
                m.levels.size());

    for (const aow::MapLevel& lv : m.levels) {
        std::map<std::uint32_t, std::size_t> terr, obj;
        std::map<std::uint32_t, std::size_t> terr_res;  // tag-50 resource id -> count
        std::map<std::string, std::size_t> terr_ilb, obj_ilb;
        std::size_t nterr = 0, nobj = 0;
        for (const aow::MapTile& t : lv.tiles) {
            for (const aow::MapHs& hs : t.objects) {
                const aow::MapResource* r =
                    reslist.empty() ? nullptr
                                    : aow::find_map_resource(reslist, hs.resource);
                const std::string ilb = r ? r->ilb : std::string();
                if (hs.tag == 50) {
                    ++terr[hs.class_id];
                    ++terr_res[hs.resource];
                    ++nterr;
                    if (!ilb.empty()) ++terr_ilb[ilb];
                } else {
                    ++obj[hs.class_id];
                    ++nobj;
                    if (!ilb.empty()) ++obj_ilb[ilb];
                }
            }
        }
        std::printf("\n  level %zu: %ux%u  tiles=%zu  terrain=%zu  objects=%zu\n",
                    lv.index, lv.width, lv.height, lv.tiles.size(), nterr, nobj);
        std::printf("    terrain classes:\n");
        for (const auto& kv : terr) {
            const char* n = aow::map_class_name(kv.first);
            std::printf("      0x%05x %8zu  %s\n", kv.first, kv.second,
                        n ? n : "class_unknown");
        }
        std::printf("    object classes (top 15):\n");
        std::vector<std::pair<std::uint32_t, std::size_t>> v(obj.begin(), obj.end());
        std::sort(v.begin(), v.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        for (std::size_t i = 0; i < v.size() && i < 15; ++i) {
            const char* n = aow::map_class_name(v[i].first);
            std::printf("      0x%05x %8zu  %s\n", v[i].first, v[i].second,
                        n ? n : "class_unknown");
        }
        if (!reslist.empty()) {
            std::printf("    terrain ILBs:\n");
            for (const auto& kv : terr_ilb) {
                std::printf("      %-24s %8zu\n", kv.first.c_str(), kv.second);
            }
            std::printf("    terrain resource ids: %zu distinct\n",
                        terr_res.size());
            std::vector<std::pair<std::uint32_t, std::size_t>> tr(terr_res.begin(),
                                                                  terr_res.end());
            std::sort(tr.begin(), tr.end(),
                      [](const auto& a, const auto& b) { return a.second > b.second; });
            for (std::size_t i = 0; i < tr.size() && i < 20; ++i) {
                const aow::MapResource* r =
                    aow::find_map_resource(reslist, tr[i].first);
                std::printf("      0x%08x %8zu  idx=%-5u %s\n", tr[i].first,
                            tr[i].second, r ? r->res_index : 0u,
                            r ? r->ilb.c_str() : "(unresolved)");
            }
            std::printf("    object ILBs (top 15):\n");
            std::vector<std::pair<std::string, std::size_t>> ov(obj_ilb.begin(),
                                                                obj_ilb.end());
            std::sort(ov.begin(), ov.end(),
                      [](const auto& a, const auto& b) { return a.second > b.second; });
            for (std::size_t i = 0; i < ov.size() && i < 15; ++i) {
                std::printf("      %-24s %8zu\n", ov[i].first.c_str(), ov[i].second);
            }
        }
        if (preview && lv.width <= 128 && lv.height <= 128) {
            std::printf("    terrain preview ('~' water, '^' elevated, 'B' border,"
                        " '.' plain, '?' other):\n");
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                std::string row;
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    const aow::MapTile& t = lv.tiles[y * lv.width + x];
                    std::uint32_t cid = 0;
                    for (const aow::MapHs& hs : t.objects) {
                        if (hs.tag == 50) { cid = hs.class_id; break; }
                    }
                    row += terrain_symbol(cid);
                }
                std::printf("      %s\n", row.c_str());
            }
        }
    }
    return 0;
}

}  // namespace

// Loads every unit of the game from <root>/Release (Unitres.pfs + Unitgfx.pfs
// + Release.hss + *.ahl) and prints the roster, grouped by race.
int cmd_roster(const char* root) {
    aow::UnitCatalog cat;
    std::string err;
    if (!aow::load_unit_catalog(root, cat, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    std::printf("%s\n", root);
    std::printf("  units=%zu (no sprite: %zu)  unitGfx=%zu  leaderLists=%zu\n",
                cat.units.size(), cat.gfx_missing, cat.graphics.size(),
                cat.leader_lists.size());
    for (const aow::LeaderList& l : cat.leader_lists) {
        std::printf("  ahl: %-16s \"%s\"  tag14=%u  %zu bytes\n", l.file.c_str(),
                    l.name.c_str(), l.tag14, l.raw.size());
    }
    std::vector<int> races;
    for (const aow::UnitEntry& u : cat.units) {
        if (std::find(races.begin(), races.end(), static_cast<int>(u.race_id)) ==
            races.end()) {
            races.push_back(static_cast<int>(u.race_id));
        }
    }
    std::sort(races.begin(), races.end());
    for (int r : races) {
        const std::vector<const aow::UnitEntry*> list =
            cat.units_of_race(static_cast<std::uint8_t>(r));
        const std::string lib = list.empty() ? std::string() : list.front()->race_lib;
        std::printf("\n== %s (race %d)%s%s ==\n",
                    aow::race_label(static_cast<std::uint8_t>(r)).c_str(), r,
                    lib.empty() ? "" : "  body=", lib.c_str());
        for (const aow::UnitEntry* u : list) {
            std::printf(
                "  %-22s id=%-4u atk=%u def=%u dmg=%u hp=%u mov=%u res=%u "
                "lvl=%u gfx=%-4u %s\n",
                u->display_name.c_str(), u->id, u->attack, u->defense, u->damage,
                u->hit_points, u->moves, u->resistance, u->level, u->gfx_index,
                u->gfx ? u->gfx->sprite_lib.c_str() : "(no sprite)");
        }
    }
    return 0;
}

// `aow unitseq <Unitgfx.pfs> [res_index|all]`: dump the parsed tag-5
// `TImageSequenceList` (the unit's animation poses) for one record or every
// record. Prints the per-sequence library / mode / period / frame table so the
// parser can be checked against the shipped data.
int cmd_unitseq(const char* path, const char* which) {
    aow::PfsFile pfs;
    std::string err;
    if (!pfs.load(path, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    const std::vector<aow::UnitGfx> gfx = aow::load_unit_gfx(pfs);
    const bool all = (which == nullptr) || std::string(which) == "all";
    const long want = all ? -1 : std::strtol(which, nullptr, 0);
    std::printf("%s  unitGfx=%zu\n", path, gfx.size());
    for (const aow::UnitGfx& g : gfx) {
        if (!all && static_cast<long>(g.res_index) != want) continue;
        std::vector<aow::ImageSequence> seqs;
        std::string serr;
        const bool ok = aow::parse_image_sequence_list(
            g.sequence.data(), g.sequence.size(), seqs, serr);
        std::printf("  [idx=%u] %-20s moveClass=%u  %zu bytes -> %zu seqs%s\n",
                    g.res_index, g.name.c_str(), g.value10, g.sequence.size(),
                    seqs.size(), ok ? "" : ("  PARSE ERROR: " + serr).c_str());
        for (std::size_t i = 0; i < seqs.size(); ++i) {
            const aow::ImageSequence& s = seqs[i];
            if (s.empty()) {
                std::printf("    [%2zu] <null>\n", i);
                continue;
            }
            std::printf("    [%2zu] mode=%u per=%u base=%d xoff=%d yoff=%d "
                        "n=%zu  %s\n",
                        i, s.mode, s.period, s.base_frame, s.xoff, s.yoff,
                        s.frame_table.size(), s.library.c_str());
            if (!s.frame_table.empty()) {
                std::printf("          frames:");
                for (std::int32_t f : s.frame_table) std::printf(" %d", f);
                std::printf("\n");
            }
        }
    }
    return 0;
}

// Loads a scenario map's game state (the player roster + turn loop) and prints
// it. With --turns N it also runs N days of the recovered turn loop so the
// income/upkeep accounting is visible. See aow/game_state.hpp.
int cmd_gamestate(const char* path, int turns) {
    aow::Map map;
    std::string err;
    if (!aow::load_map(path, map, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    aow::GameState gs;
    if (!aow::load_game_state(map, gs, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    std::printf("%s\n", path);
    std::printf("  day=%d  turn_limit=%d  turn_order_mode=%d  players=%d\n",
                gs.day, gs.turn_limit, gs.turn_order_mode, gs.player_count());
    std::printf("  %-3s %-14s %-5s %-7s %-7s %-7s %-6s %-6s %-6s %-6s\n",
                "id", "name", "race", "gold", "income", "upkeep", "gems",
                "mana", "armies", "units");
    for (const aow::Player& p : gs.players) {
        std::printf("  %-3d %-14s %-5d %-7d %-7d %-7d %-6d %-6d %-6d %-6d%s\n",
                    static_cast<int>(p.index), p.display_name().c_str(),
                    static_cast<int>(p.race), p.gold, p.income, p.upkeep(),
                    p.gems, p.magic.mana, p.armies, p.units,
                    p.active() ? "" : "  (out)");
    }

    if (turns > 0) {
        std::printf("\n  simulating %d day(s):\n", turns);
        gs.begin_day(0x1000);
        for (int d = 0; d < turns; ++d) {
            // Run one whole day: every player in the turn order takes a turn.
            const std::size_t n = gs.turn_order.size();
            for (std::size_t i = 0; i < n; ++i) {
                gs.advance_turn(static_cast<std::uint32_t>(0x1000 + gs.day));
            }
            // advance_turn() past the last player has already begun the next
            // day (NewDay), so report the gold after the completed day.
            std::printf("  after day %d:", gs.day - 1);
            for (const aow::Player& p : gs.players) {
                std::printf("  %s=%d", p.display_name().c_str(), p.gold);
            }
            std::printf("\n");
        }
    }
    return 0;
}

// `aow move <map.HSM> [--root <dir>] [--level N] [--to X,Y]`
//
// Demonstrates the recovered world-map movement rules (docs/15_MOVEMENT.md):
// every army the human player owns, its units' move types (decoded from the
// TAbilityOwner ability bitmap), the army's folded move-point table and its
// this-turn budget, plus - with `--to X,Y` - the bounded path the engine's
// TMoveControl.CalculatePathEx would walk, one step per hex.
int cmd_move(const char* path, const std::string& root, int level, int to_x,
             int to_y) {
    aow::Map map;
    std::string err;
    if (!aow::load_map(path, map, err)) {
        std::fprintf(stderr, "error: %s\n", err.c_str());
        return 1;
    }
    if (level < 0 || static_cast<std::size_t>(level) >= map.levels.size()) {
        std::fprintf(stderr, "error: level %d out of range (%zu levels)\n", level,
                     map.levels.size());
        return 1;
    }
    std::vector<aow::MapResource> res;
    aow::UnitCatalog cat;
    if (!root.empty()) {
        std::string rerr;
        if (!aow::load_map_resources(root + "\\Release\\Release.hss", res,
                                     rerr)) {
            std::fprintf(stderr, "warning: %s\n", rerr.c_str());
        }
        std::string cerr_;
        if (!aow::load_unit_catalog(root, cat, cerr_)) {
            std::fprintf(stderr, "warning: %s\n", cerr_.c_str());
        }
    }
    const std::size_t filled = aow::fill_tile_terrain(map, level, res);
    aow::GameState gs;
    std::string gerr;
    aow::load_game_state(map, gs, gerr);
    const std::array<aow::MovePointTable, 256> tables =
        aow::build_move_point_tables();
    const aow::MapLevel& lv = map.levels[static_cast<std::size_t>(level)];

    std::printf("%s  level %d  %ux%u  terrain resolved=%zu/%zu\n", path, level,
                lv.width, lv.height, filled, lv.tiles.size());
    // Terrain / overlay histogram (the two TMapField axes the move-cost table
    // is indexed by). The overlay is the raw TTerrainList code: -1 = none
    // (plains), 0 = mountains, 1 = forest, 2 = hill, 3 = scrub, 4 = road/city,
    // 5 = bridge, ... (aow::move_point_cost adds 1 to get the table column).
    int terr_hist[16] = {0};
    int ov_hist[16] = {0};
    int ov_none = 0;
    for (const aow::MapTile& t : lv.tiles) {
        if (t.terrain >= 0 && t.terrain < 16) ++terr_hist[t.terrain];
        if (t.overlay < 0) ++ov_none;
        else if (t.overlay < 16) ++ov_hist[t.overlay];
    }
    std::printf("  terrain:");
    for (int i = 0; i < 16; ++i) {
        if (terr_hist[i] != 0) std::printf(" %d=%d", i, terr_hist[i]);
    }
    std::printf("\n  overlay:");
    if (ov_none != 0) std::printf(" none=%d", ov_none);
    for (int i = 0; i < 16; ++i) {
        if (ov_hist[i] != 0) std::printf(" %d=%d", i, ov_hist[i]);
    }
    // The overlay the tag-50 base hexagon resource alone reports (0x80..0xFF =
    // "no overlay" = -1), to separate it from the overlay objects' overrides.
    int base_ov[16] = {0};
    int base_ov_none = 0;
    for (const aow::MapTile& t : lv.tiles) {
        for (const aow::MapHs& hs : t.objects) {
            if (hs.tag != 50) continue;
            const aow::MapResource* r = aow::find_map_resource(res, hs.resource);
            if (r != nullptr && r->has_terrain) {
                if (r->overlay < 0) ++base_ov_none;
                else if (r->overlay < 16) ++base_ov[r->overlay];
            }
            break;
        }
    }
    std::printf("\n  base-hex overlay:");
    if (base_ov_none != 0) std::printf(" none=%d", base_ov_none);
    for (int i = 0; i < 16; ++i) {
        if (base_ov[i] != 0) std::printf(" %d=%d", i, base_ov[i]);
    }
    // The overlays come from the tile's overlay objects (a forest/hill/mountain
    // /road), not from the base hexagon: on Tutorial level 0 all 2304 tag-50
    // resources report 0xFE = "no overlay" (see docs/15_MOVEMENT.md).
    std::printf("\n  human players:");
    for (const aow::Player& p : gs.players) {
        if (p.type == aow::PlayerType::Human) {
            std::printf(" %d", static_cast<int>(p.index));
        }
    }
    std::printf("\n");
    int found = 0;
    for (std::uint32_t y = 0; y < lv.height; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            const aow::MapTile& t = lv.tiles[y * lv.width + x];
            for (const aow::MapHs& hs : t.objects) {
                if (hs.class_id != aow::kMapArmyHsClassId || hs.units.empty()) {
                    continue;
                }
                bool human = false;
                for (const aow::Player& p : gs.players) {
                    if (p.index == hs.owner && p.type == aow::PlayerType::Human) {
                        human = true;
                    }
                }
                if (!human) continue;
                ++found;
                std::vector<std::uint8_t> mts, mps;
                for (const aow::MapUnit& u : hs.units) {
                    mts.push_back(aow::unit_move_types(u));
                    mps.push_back(aow::unit_move_points(cat, u));
                }
                const aow::ArmyMove am = aow::army_move(tables, mts, mps);
                std::printf("\n  army at (%u,%u)  owner player %d  %zu unit(s)  "
                            "hex terrain=%d overlay=%d\n",
                            x, y, static_cast<int>(hs.owner), hs.units.size(),
                            t.terrain, t.overlay);
                for (std::size_t i = 0; i < hs.units.size(); ++i) {
                    const aow::MapUnit& u = hs.units[i];
                    std::printf("    unit[%zu] class=0x%05x res=%d hero=%d "
                                "abilities=%u  moves=%s  movepoints=%u\n",
                                i, u.class_id, u.resource, u.is_hero ? 1 : 0,
                                u.ability_count,
                                aow::move_types_string(mts[i]).c_str(), mps[i]);
                }
                // The raw TTerrainList overlay codes (docs/15_MOVEMENT.md §2):
                // -1 = none (grassland), 1 = forest, 2 = hill, 0 = mountains,
                // 4 = road, 5 = bridge. 0xFF = impassable for this army.
                std::printf("    army move types=%s  budget=%u  "
                            "grass=%u forest=%u hill=%u mountain=%u road=%u "
                            "water=%u\n",
                            aow::move_types_string(am.move_types).c_str(),
                            am.budget, aow::move_point_cost(am.table, 1, -1),
                            aow::move_point_cost(am.table, 1, 1),
                            aow::move_point_cost(am.table, 1, 2),
                            aow::move_point_cost(am.table, 1, 0),
                            aow::move_point_cost(am.table, 1, 4),
                            aow::move_point_cost(am.table, 0, -1));
                if (to_x >= 0 && to_y >= 0) {
                    const aow::MovePath pth = aow::find_move_path(
                        am.table, lv, static_cast<int>(x), static_cast<int>(y),
                        to_x, to_y);
                    if (pth.steps.empty()) {
                        std::printf("    path to (%d,%d): unreachable\n", to_x,
                                    to_y);
                    } else {
                        std::printf("    path to (%d,%d): %zu hexes, cost %d, "
                                    "reachable=%d, turns=%d\n",
                                    to_x, to_y, pth.steps.size(), pth.total_cost,
                                    pth.reached ? 1 : 0,
                                    aow::path_turns(pth.total_cost, am.budget,
                                                    am.budget));
                        for (std::size_t i = 0; i + 1 < pth.steps.size(); ++i) {
                            const int dir = aow::path_arrow_dir(pth, i);
                            std::printf("      step %zu (%d,%d)->(%d,%d) dir=%d "
                                        "cost=%d frame=0x%02x\n",
                                        i, pth.steps[i].x, pth.steps[i].y,
                                        pth.steps[i + 1].x, pth.steps[i + 1].y,
                                        dir, pth.steps[i + 1].cost,
                                        aow::path_arrow_frame(
                                            dir, aow::path_arrow_bright(
                                                     pth, i, am.budget)));
                        }
                        const aow::MoveStep& d = pth.steps.back();
                        std::printf("      target (%d,%d) frame=0x%02x\n", d.x, d.y,
                                    aow::path_target_frame(am.budget >=
                                                           pth.total_cost));
                    }
                }
            }
        }
    }
    if (found == 0) std::printf("  (no human army on this level)\n");
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) return usage();
    std::string cmd = argv[1];
    if (cmd == "version") {
        std::printf("aow port bootstrap 0.1\n");
        return 0;
    }
    if (cmd == "ilb") {
        if (argc < 3) return usage();
        return cmd_ilb(argv[2]);
    }
    if (cmd == "png") {
        if (argc < 5) return usage();
        bool by_id = false;
        for (int i = 5; i < argc; ++i) {
            if (std::string(argv[i]) == "--id") by_id = true;
        }
        return cmd_png(argv[2], argv[3], argv[4], by_id);
    }
    if (cmd == "pfs") {
        if (argc < 3) return usage();
        return cmd_pfs(argv[2], argc >= 4 ? argv[3] : nullptr);
    }
    if (cmd == "map") {
        if (argc < 3) return usage();
        bool preview = false;
        const char* res = nullptr;
        for (int i = 3; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--preview") {
                preview = true;
            } else if (a == "--res" && i + 1 < argc) {
                res = argv[++i];
            }
        }
        return cmd_map(argv[2], preview, res);
    }
    if (cmd == "mapres") {
        if (argc < 3) return usage();
        const std::uint32_t only = (argc >= 4)
            ? static_cast<std::uint32_t>(std::strtoul(argv[3], nullptr, 0))
            : 0u;
        return cmd_mapres(argv[2], only);
    }
    if (cmd == "mapunits") {
        if (argc < 3) return usage();
        int level = 0;
        std::string root;
        for (int i = 3; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--root" && i + 1 < argc) root = argv[++i];
            else if (a == "--level" && i + 1 < argc) level = std::atoi(argv[++i]);
        }
        return cmd_mapunits(argv[2], level, root);
    }
    if (cmd == "mapviz") {
        if (argc < 4) return usage();
        std::string res, root = "..", debug_csv;
        int level = 0, max_dim = 0;
        bool objects = true;
        for (int i = 4; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--res" && i + 1 < argc) res = argv[++i];
            else if (a == "--root" && i + 1 < argc) root = argv[++i];
            else if (a == "--level" && i + 1 < argc) level = std::atoi(argv[++i]);
            else if (a == "--max" && i + 1 < argc) max_dim = std::atoi(argv[++i]);
            else if (a == "--debug-csv" && i + 1 < argc) debug_csv = argv[++i];
            else if (a == "--no-objects") objects = false;
        }
        return cmd_mapviz(argv[2], argv[3], res, root, level, objects, max_dim,
                          debug_csv);
    }
    if (cmd == "tbmapviz") {
        if (argc < 4) return usage();
        std::string root = "..";
        int level = 0;
        bool objects = true;
        for (int i = 4; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--root" && i + 1 < argc) root = argv[++i];
            else if (a == "--level" && i + 1 < argc) level = std::atoi(argv[++i]);
            else if (a == "--no-objects") objects = false;
        }
        return cmd_tbmapviz(argv[2], argv[3], root, level, objects);
    }
    if (cmd == "roster") {
        if (argc < 3) return usage();
        return cmd_roster(argv[2]);
    }
    if (cmd == "unitseq") {
        if (argc < 3) return usage();
        return cmd_unitseq(argv[2], argc > 3 ? argv[3] : "all");
    }
    if (cmd == "gamestate") {
        if (argc < 3) return usage();
        int turns = 0;
        for (int i = 3; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--turns" && i + 1 < argc) turns = std::atoi(argv[++i]);
        }
        return cmd_gamestate(argv[2], turns);
    }
    if (cmd == "move") {
        if (argc < 3) return usage();
        std::string root;
        int level = 0;
        int to_x = -1, to_y = -1;
        for (int i = 3; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--root" && i + 1 < argc) root = argv[++i];
            else if (a == "--level" && i + 1 < argc) level = std::atoi(argv[++i]);
            else if (a == "--to" && i + 1 < argc) {
                const std::string v = argv[++i];
                const std::size_t c = v.find(',');
                if (c != std::string::npos) {
                    to_x = std::atoi(v.substr(0, c).c_str());
                    to_y = std::atoi(v.substr(c + 1).c_str());
                }
            }
        }
        return cmd_move(argv[2], root, level, to_x, to_y);
    }
    if (cmd == "abilities" || cmd == "spells" || cmd == "units" ||
        cmd == "items" || cmd == "heroes" || cmd == "herores" ||
        cmd == "unitgfx" || cmd == "faces" || cmd == "itemgfx" ||
        cmd == "texts" || cmd == "terrain") {
        if (argc < 3) return usage();
        return cmd_typed(argv[2], cmd.c_str());
    }
    return usage();
}
