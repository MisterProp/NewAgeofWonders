// game_state.cpp - the AoW1 player roster + turn/day loop (Stage 2.3).
//
// See game_state.hpp for the format + the decompiled evidence. The roster lives
// in the scenario map's EObject graph at TAoWHSMap tag 0x15 (a TPlayerList); the
// reader reuses the shared property-table walk (aow/eobject.hpp) exactly like
// src/engine/map.cpp.
#include "aow/game_state.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <utility>
#include <vector>

#include "aow/endian.hpp"
#include "aow/eobject.hpp"
#include "aow/map_resources.hpp"
#include "aow/movement.hpp"          // object_covers_hex / hn_walk_hex
#include "aow/structure_capture.hpp"  // the ownership rules (docs/29)
#include "aow/unit_catalog.hpp"

namespace aow {
namespace {

using Table = EObjectTable;

constexpr std::size_t kNpos = static_cast<std::size_t>(-1);

// TAoWHSMap.ReadWrite tag that holds the roster (a TPlayerList).
constexpr std::uint32_t kRosterTag = 0x15;

std::size_t find_off(const Table& t, std::uint32_t tag) {
    return eobject_find(t, tag);
}

// A small xorshift32 PRNG. The original shuffles with the map's own RNG
// (`TAoWHSMap.Random`); we only need a deterministic, well-defined permutation
// so the port's turn order is reproducible and testable.
std::uint32_t xs32(std::uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

// Delphi's `System.ROUND` (AoWEPACK 0x55701048, imported from the Delphi RTL):
// round to nearest, ties to even. Every caller here passes a non-negative
// value; keep the generic sign handling anyway.
int delphi_round(double value) {
    const double floor = std::floor(value);
    const double frac = value - floor;
    int result = static_cast<int>(floor);
    if (frac > 0.5) {
        ++result;
    } else if (frac == 0.5) {
        // Ties go to the even neighbour - notably 2.5 -> 2 and 12.5 -> 12.
        if ((result & 1) != 0) ++result;
    }
    return result;
}

// Parses a `TPlayerMagicControl` (TPlayer tag 0x1f).
void parse_magic(const std::uint8_t* R, std::size_t n, Table& ptab,
                 std::size_t off, PlayerMagic& out) {
    const std::size_t p = ptab.base + off;
    Table mt;
    if (!read_eobject_table(R, n, p, mt)) return;
    out.present = true;
    const std::size_t m = find_off(mt, kMagicTagMana);
    if (m != kNpos && mt.base + m + 4 <= n) {
        out.mana = read_i32(R + mt.base + m);
    }
    const std::size_t pr = find_off(mt, kMagicTagPowerToResearch);
    if (pr != kNpos && mt.base + pr + 4 <= n) {
        out.power_to_research = read_i32(R + mt.base + pr);
    }
    const std::size_t cr = find_off(mt, kMagicTagCurrentResearch);
    if (cr != kNpos && mt.base + cr + 4 <= n) {
        out.current_research = read_i32(R + mt.base + cr);
    }
    const std::size_t rp = find_off(mt, kMagicTagResearchProgress);
    if (rp != kNpos && mt.base + rp + 4 <= n) {
        out.research_progress = read_i32(R + mt.base + rp);
    }
    // +0x18 tag 0x1b: starting mana, copied into +0x24 by StartNewMap.
    const std::size_t sm = find_off(mt, kMagicTagStartingMana);
    if (sm != kNpos && mt.base + sm + 4 <= n) {
        out.starting_mana = read_i32(R + mt.base + sm);
    }
    // tag 0x16 (the researched-spell IntegerList) is serialised through the
    // list's SaveToStream; its body is not decoded here (TODO(RE)) - the list
    // stays empty and `present` still records that the map carried one.
}

// Parses a player's TLeader (TPlayer rw tag 0x1e): a THero serialised inline
// (a bare property table - see the Leader comment in game_state.hpp). The tags
// are the THero ones the placed heroes use (src/engine/map.cpp): 7 = the face
// index, 8 = the THeroResource index, 10 = the name, 11 = the nickname.
void parse_leader(const std::uint8_t* R, std::size_t n, Table& ptab,
                  std::size_t off, Leader& out) {
    Table lt;
    if (!read_eobject_table(R, n, ptab.base + off, lt)) return;
    out.present = true;
    const std::size_t fi = find_off(lt, kHeroTagFaceIndex);
    if (fi != kNpos && lt.base + fi + 4 <= n) {
        out.face_index = read_i32(R + lt.base + fi);
    }
    const std::size_t hi = find_off(lt, kHeroTagHeroResourceIndex);
    if (hi != kNpos && lt.base + hi + 4 <= n) {
        out.hero_resource_index = read_i32(R + lt.base + hi);
    } else {
        out.hero_resource_index = -1;  // THero.Create's default
    }
    const std::size_t nm = find_off(lt, kHeroTagName);
    if (nm != kNpos) out.name = eobject_short_string(R, n, lt.base + nm);
    const std::size_t nk = find_off(lt, kHeroTagNickname);
    if (nk != kNpos) out.nickname = eobject_short_string(R, n, lt.base + nk);
    // TLeader rw tag 0x2e -> +0xa0: the sphere-pick byte list. The serialised
    // body is one wrapper byte, a u32 count, then that many sphere ids
    // (`TLeader.GetSpherePicks@5578b17c` counts matching ids).
    const std::size_t spheres = find_off(lt, 0x2e);
    if (spheres != kNpos && lt.base + spheres + 5 <= n) {
        const std::uint32_t cnt = read_u32(R + lt.base + spheres + 1);
        const std::size_t start = lt.base + spheres + 5;
        if (cnt <= 64 && start + cnt <= n) {
            out.sphere_picks.assign(R + start, R + start + cnt);
        }
    }
}

void parse_player(const std::uint8_t* R, std::size_t n, Table& tab,
                  Player& out) {
    const std::size_t idx = find_off(tab, kPlayerTagIndex);
    if (idx != kNpos && tab.base + idx < n) out.index = R[tab.base + idx];
    // rw tag 0x2 = +0xa5 = the race. It is a plain `rwByte`
    // (TEReadStorageStream.rwByte, vtable +0x30), whose absent-tag branch
    // writes 0 - so a map that never stored a race means Humans (0), never
    // 0xff. Only an explicit tag (the independents' 0xff) differs. This is the
    // general rule that gives Europe's unraced player 1 the Humans cell.
    const std::size_t race = find_off(tab, kPlayerTagRace);
    if (race != kNpos && tab.base + race < n) out.race = R[tab.base + race];
    // TPlayer rw tag 0x3 = +0xa7 = the player type (`hPickMap` 0x557e20c8).
    const std::size_t ty = find_off(tab, kPlayerTagType);
    if (ty != kNpos && tab.base + ty < n) {
        out.type = static_cast<PlayerType>(R[tab.base + ty]);
    }
    // tag 0x5 -> +0xac = the scenario's starting gold. The live treasury
    // (+0xc4) is seeded from it by StartNewMap; a scenario omits tag 0x10 and
    // the stream's absent-tag default is 0.
    const std::size_t gold = find_off(tab, kPlayerTagGold);
    if (gold != kNpos && tab.base + gold + 4 <= n) {
        out.start_gold = read_i32(R + tab.base + gold);
    }
    const std::size_t bi = find_off(tab, kPlayerTagBaseIncome);
    if (bi != kNpos && tab.base + bi + 4 <= n) {
        out.base_income = read_i32(R + tab.base + bi);
    }
    // tag 0x10 -> +0xc4 = the live gold treasury. `TPlayer.SetGems` is the
    // historical name of the setter; see the Player::gold comment.
    const std::size_t live_gold = find_off(tab, kPlayerTagGems);
    if (live_gold != kNpos && tab.base + live_gold + 4 <= n) {
        out.gold = read_i32(R + tab.base + live_gold);
    }
    const std::size_t tc = find_off(tab, kPlayerTagTurnCount);
    if (tc != kNpos && tab.base + tc + 4 <= n) {
        out.turn_count = read_i32(R + tab.base + tc);
    }
    const std::size_t fp = find_off(tab, kPlayerTagFoodPenalty);
    if (fp != kNpos && tab.base + fp < n) {
        out.food_penalty = static_cast<std::int8_t>(R[tab.base + fp]);
    }
    const std::size_t go = find_off(tab, kPlayerTagGameOver);
    if (go != kNpos && tab.base + go < n) out.game_over_status = R[tab.base + go];
    const std::size_t nm = find_off(tab, kPlayerTagName);
    if (nm != kNpos) out.name = eobject_short_string(R, n, tab.base + nm);
    const std::size_t c = find_off(tab, kPlayerTagColor);  // 0x14 color[3]
    if (c != kNpos && tab.base + c + 3 <= n) {
        out.color[0] = R[tab.base + c];
        out.color[1] = R[tab.base + c + 1];
        out.color[2] = R[tab.base + c + 2];
    }
    const std::size_t mg = find_off(tab, kPlayerTagMagic);
    if (mg != kNpos) parse_magic(R, n, tab, mg, out.magic);
    // TPlayer rw tag 0x1e = +0x58/+0xd4 = the player's TLeader (see Leader).
    const std::size_t ld = find_off(tab, kPlayerTagLeader);
    if (ld != kNpos) parse_leader(R, n, tab, ld, out.leader);
    // The tail of `TPlayer.ReadWrite` (0x5575314c): a record whose index byte
    // (+0xa6) is 0 *is* the independents, and the loader forces its type to 4
    // (`if index == 0 then +0xa7 = 4`). The index tag is itself a rwByte, so an
    // absent index is 0 too.
    if (out.index == 0) out.type = PlayerType::Independent;
}

// Walks map tag 0x15 (a TPlayerList): its tag 1 children table lists the
// `[u32 class id][table]` TPlayer records (class 0x20020).
void parse_roster(const std::uint8_t* R, std::size_t n, std::size_t list_off,
                  GameState& out) {
    Table pl;
    if (!read_eobject_table(R, n, list_off, pl)) return;
    const std::size_t c = find_off(pl, 1);
    if (c == kNpos) return;
    Table children;
    if (!read_eobject_table(R, n, pl.base + c, children)) return;
    for (const EObjectEntry& e : children.entries) {
        const std::size_t a = children.base + e.second;
        if (a + 4 > n) continue;
        if (read_u32(R + a) != kPlayerClassId) continue;
        Table pt;
        if (!read_eobject_table(R, n, a + 4, pt)) continue;
        Player p;
        parse_player(R, n, pt, p);
        out.players.push_back(std::move(p));
    }
}

// Counts the placed armies/units per player from the map (docs/13). An army's
// owner player is `AoWE.TArmy` rw tag 4 (`TArmy.GetPlayer` @ 0x5578d504 reads
// +0x12; a missing tag means owner 0 = the independents).
void tally_troops(const Map& map, GameState& out) {
    for (const MapLevel& lv : map.levels) {
        for (const MapTile& tile : lv.tiles) {
            for (const MapHs& hs : tile.objects) {
                if (hs.class_id != kMapArmyHsClassId) continue;
                const std::size_t who = hs.owner;
                if (who >= out.players.size()) continue;
                out.players[who].armies += 1;
                out.players[who].units += static_cast<int>(hs.units.size());
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Economy helpers (see game_state.hpp for the decompiled evidence)
// ---------------------------------------------------------------------------

// HSEngine.InitHNWalk@5560e5c8 + WalkToNextHN@5560e610 with radius 0: the HN
// spiral around a hex, `index` 0 = the centre, 1..6 = the first ring, ... .
// Same table/algorithm as aow::hn_walk_hex@movement.cpp, reproduced here so the
// game-state module does not depend on movement.cpp.
void hn_walk(int ox, int oy, int index, int& hx, int& hy) {
    static const int kEdgeDx[2][6] = {{1, 0, -1, -1, 0, 1},
                                      {1, 0, -1, -1, 0, 1}};
    static const int kEdgeDy[2][6] = {{0, 1, 0, -1, -1, -1},
                                      {1, 1, 1, 0, -1, 0}};
    int dir = 5, step = 0, radius = 0, px = ox, py = oy;
    for (int i = 1; i <= index; ++i) {
        const int p = px & 1;
        ++step;
        if (step < radius) {
            px += kEdgeDx[p][dir];
            py += kEdgeDy[p][dir];
            continue;
        }
        if (dir == 5) {
            ++radius;
            dir = 0;
            step = 0;
            px = ox;
            py = oy - radius;
            continue;
        }
        px += kEdgeDx[p][dir];
        py += kEdgeDy[p][dir];
        ++dir;
        step = 0;
    }
    hx = px;
    hy = py;
}

// HSEngine.HNtoHX@5560e08c's six neighbour offsets folded by CenterHNtoHX's
// column parity (the same table as aow::hex_neighbour).
bool hn_neighbour(int x, int y, int dir, int& nx, int& ny) {
    static const int kDx[2][6] = {{0, 1, 1, 0, -1, -1}, {0, 1, 1, 0, -1, -1}};
    static const int kDy[2][6] = {{-1, -1, 0, 1, 0, -1}, {-1, 0, 1, 1, 1, 0}};
    if (dir < 1 || dir > 6) return false;
    const int p = x & 1;
    nx = x + kDx[p][dir - 1];
    ny = y + kDy[p][dir - 1];
    return true;
}

// HSEngine.HXtoHN@5560df88 inverted through the same HN spiral. The indices we
// need here are all in the first few rings (a structure resource covers at most
// 19 hexes), so a bounded search is exact and cheap.
int hn_index_of(int x, int y) {
    for (int k = 0; k < 256; ++k) {
        int hx = 0, hy = 0;
        hn_walk(0, 0, k, hx, hy);
        if (hx == x && hy == y) return k;
    }
    return -1;
}

// TTerrainList.GetTerrain@55605054 returns the entry's low byte as a signed
// char; 0xfe (plain/no opinion) and 0xff (no entry) are negative.
bool terrain_entry_opinion(const MapResource& r, std::size_t i) {
    if (i >= r.terrain_list.size()) return false;
    return static_cast<std::int8_t>(r.terrain_list[i] & 0xff) >= 0;
}

// TCityResource.ActivateMsg@557adee8: the city's size (`TCityResource+0x78`,
// read by TCity.GetSize@557aa938) is the number of terrain-list entries whose
// terrain byte is >= 0. The shipped CITY1..CITY4 resources yield 1..4.
int resource_city_size(const MapResource& r) {
    int n = 0;
    for (std::size_t i = 0; i < r.terrain_list.size(); ++i) {
        if (terrain_entry_opinion(r, i)) ++n;
    }
    return n;
}

// TTerrainList.CreateSurroundingHNTable@556051e8: for every covered hex of
// the resource, walk its six neighbours; a neighbour that is outside the
// terrain list or whose entry has no opinion is a *surrounding* hex. The
// engine stores those HN indices and TPlayerCropStructure.GetCropCount@557a6480
// counts the crops on them.
std::vector<int> surrounding_hn(const MapResource& r) {
    std::vector<int> out;
    const int n = static_cast<int>(r.terrain_list.size());
    for (int i = 0; i < n; ++i) {
        if (!terrain_entry_opinion(r, static_cast<std::size_t>(i))) continue;
        int rx = 0, ry = 0;
        hn_walk(0, 0, i, rx, ry);
        for (int dir = 1; dir <= 6; ++dir) {
            int nx = 0, ny = 0;
            if (!hn_neighbour(rx, ry, dir, nx, ny)) continue;
            const int hn = hn_index_of(nx, ny);
            if (hn < 0) continue;
            if (hn < n &&
                terrain_entry_opinion(r, static_cast<std::size_t>(hn))) {
                continue;  // still part of the structure's own footprint
            }
            if (std::find(out.begin(), out.end(), hn) == out.end()) {
                out.push_back(hn);
            }
        }
    }
    return out;
}

// TCity.GetActiveAction@557a96a8 + City.ProductionIDToCityAction@557a7ce4.
// `TProductionControl+0x20` must be 2 for a queued item to be active. The
// produced item's +8 is the production id:
//   1 -> 8 (produce unit), 2 -> 7 (install unit), 10 -> 1 (merchandise),
//   11 -> 2 (upgrade), 12 -> 3 (fortify), 13 -> 4 (migrate), 14 -> 5 (loot).
int city_active_action(const MapHs& city) {
    if (city.production_state != 2 || city.production_queue.empty()) return 0;
    switch (city.production_queue.front().kind) {
        case 1:  return 8;
        case 2:  return 7;
        case 10: return 1;
        case 11: return 2;
        case 12: return 3;
        case 13: return 4;
        case 14: return 5;
        default: return 0;
    }
}

// TProductionPlace.GetIncome@557be500: when a non-merchandise item is actively
// produced (state 2, first item != 10) the +25% is not applied.
bool production_place_merchandise(const MapHs& p) {
    if (p.production_state != 2) return true;
    if (p.production_queue.empty()) return true;
    return p.production_queue.front().kind == 10;
}

// TCity.UpdateIncome@557abb94. `+25%` is the float at 0x557abc2c (0.25)
// rounded by System.ROUND; action 0 (idle) and 1 (merchandise) get it.
int city_income(int size, int crops, int action) {
    int v = size * 5 + crops * 2;
    if (action < 2) v += delphi_round(v * 0.25);
    return v;
}

int production_place_income(int base, bool merchandise) {
    int v = base;
    if (merchandise) v += delphi_round(v * 0.25);
    return v;
}

// True for the six magic nodes and the random node: their
// TProductionPlace.Create registers a TPlayerStructurePowerSource, not a gold
// income source. Shipyard and BuildersGuild do have a gold source.
bool production_place_is_magic_node(std::uint32_t class_id) {
    switch (class_id) {
        case 0x204b3:  // WaterNode.TWaterNode
        case 0x204ae:  // DeathNode.TDeathNode
        case 0x204ce:  // EarthNode.TEarthNode
        case 0x203de:  // FireNode.TFireNode
        case 0x20371:  // LifeNode.TLifeNode
        case 0x2094e:  // AirNode.TAirNode
        case 0x20822:  // RandomNode.TRandomNode
            return true;
        default:
            return false;
    }
}

// TProductionPlace.GetMagicSphere: a magic node overrides the base 8 with its
// sphere id. Life 1, Death 2, Earth 3, Air 4, Fire 5, Water 6 (the six
// GetMagicSphere getters). 0 = not a magic node.
int magic_node_sphere(std::uint32_t class_id) {
    switch (class_id) {
        case 0x20371: return 1;  // LifeNode.TLifeNode
        case 0x204ae: return 2;  // DeathNode.TDeathNode
        case 0x204ce: return 3;  // EarthNode.TEarthNode
        case 0x2094e: return 4;  // AirNode.TAirNode
        case 0x203de: return 5;  // FireNode.TFireNode
        case 0x204b3: return 6;  // WaterNode.TWaterNode
        default: return 0;
    }
}

// TLeader.GetSpherePicks@5578b17c: count the leader's picks in `sphere`.
// Sphere 0 is the special "all spheres" count (4), but the nodes always pass
// 1..6 here.
int leader_sphere_picks(const Leader& leader, int sphere) {
    if (sphere <= 0) return 0;
    int picks = 0;
    for (std::uint8_t s : leader.sphere_picks) {
        if (static_cast<int>(s) == sphere) ++picks;
    }
    return picks;
}

// THero.ExperienceToLevel@557876d4. The six thresholds at 0x558e84b8 are
// 15, 15, 20, 20, 25, 25 and apply per five-level band; GetLevelMax is 30.
int hero_experience_level(std::uint32_t experience) {
    static const int kThreshold[6] = {15, 15, 20, 20, 25, 25};
    const int max_level = 30;
    int level = 1;
    std::uint32_t total = 0;
    do {
        total += static_cast<std::uint32_t>(kThreshold[level / 5]);
        if (experience < total) return level;
        ++level;
    } while (level < max_level);
    return level;
}

bool has_ability_9b(const MapUnit& u) {
    // TAbilityOwner's bitmap is LSB-first (GetAbSet@5574e0e0). 0x9b = 155,
    // byte 19, bit 3.
    if (u.ability_count <= 0x9b) return false;
    const std::size_t byte = 0x9b / 8;
    if (byte >= u.ability_mask.size()) return false;
    return ((u.ability_mask[byte] >> (0x9b & 7)) & 1) != 0;
}

int unit_resource_level(const UnitCatalog* catalog, std::int32_t unit_id) {
    if (catalog == nullptr) return -1;
    for (const UnitEntry& e : catalog->units) {
        if (static_cast<std::int32_t>(e.id) == unit_id) {
            return static_cast<int>(e.level);
        }
    }
    return -1;
}

// TArmy.UpdateUpkeep@5578cc08: sum each carried unit's GetUpkeep.
int army_upkeep(const MapHs& army, const UnitCatalog* catalog) {
    int total = 0;
    for (const MapUnit& u : army.units) {
        if (u.class_id == kMapLeaderClassId) continue;  // TLeader.GetUpkeep = 0
        if (u.class_id == kMapHeroClassId) {
            // THero.GetUpkeep@557872c0: 5 + 2 * experience level.
            total += 5 + 2 * hero_experience_level(u.experience);
            continue;
        }
        // TAbstractUnit.GetUpkeep@5577fc64: ability 0x9b suppresses upkeep,
        // otherwise (TUnit.GetUnitLevel() + 1) * 2. TUnit.GetUnitLevel reads
        // the Unitres record's level (+0x2f), which the map itself does not
        // store - hence the catalog lookup.
        if (has_ability_9b(u)) continue;
        int level = unit_resource_level(catalog, u.resource);
        if (level < 0) level = 0;
        total += (level + 1) * 2;
    }
    return total;
}

}  // namespace

void compute_economy(const Map& map, const std::vector<MapResource>& resources,
                     const UnitCatalog* units, GameState& gs) {
    for (Player& p : gs.players) {
        p.building_income = 0;
        p.army_upkeep = 0;
        p.magic.power_income = 0;
        p.magic.mana_income = 0;
    }
    std::vector<int> power_income(gs.players.size(), 0);

    // A crop structure (city or farm) owns the crop tiles in its resource's
    // surrounding-hex table. TCrop.LinkToCropOwner@557a5dac first links a crop
    // to a neighbouring structure, then LinkToCrops@557a686c walks the
    // structure's own table and overwrites the owner - so a structure's table
    // is authoritative. Iterate in map (tile) order, like the engine's
    // activation pass; a later structure wins an overlapping tile.
    struct CropStructure {
        const MapHs* hs = nullptr;
        int x = 0;
        int y = 0;
        int level = 0;
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::vector<int> hn;
        int crops = 0;
    };
    std::vector<CropStructure> crop_structures;
    std::map<std::uint64_t, std::size_t> crop_owner;
    std::vector<std::uint64_t> crop_tiles;  // tiles carrying a Crops.TCrop

    auto tile_key = [](int level, std::size_t index) {
        return (static_cast<std::uint64_t>(level) << 32) |
               static_cast<std::uint64_t>(index);
    };

    for (std::size_t li = 0; li < map.levels.size(); ++li) {
        const MapLevel& lv = map.levels[li];
        if (lv.width == 0) continue;
        for (std::size_t ti = 0; ti < lv.tiles.size(); ++ti) {
            const MapTile& tile = lv.tiles[ti];
            bool has_crop = false;
            for (const MapHs& hs : tile.objects) {
                if (hs.class_id == kCropClassId) has_crop = true;
            }
            if (has_crop) {
                crop_tiles.push_back(tile_key(static_cast<int>(li), ti));
            }
            for (const MapHs& hs : tile.objects) {
                if (hs.class_id != kCityClassId && hs.class_id != kFarmClassId) {
                    continue;
                }
                const MapResource* r = find_map_resource(resources, hs.resource);
                if (r == nullptr) continue;
                CropStructure cs;
                cs.hs = &hs;
                cs.x = static_cast<int>(ti % lv.width);
                cs.y = static_cast<int>(ti / lv.width);
                cs.level = static_cast<int>(li);
                cs.width = lv.width;
                cs.height = lv.height;
                cs.hn = surrounding_hn(*r);
                crop_structures.push_back(std::move(cs));
            }
        }
    }

    for (std::size_t si = 0; si < crop_structures.size(); ++si) {
        const CropStructure& cs = crop_structures[si];
        for (int hn : cs.hn) {
            int ax = 0, ay = 0;
            hn_walk(cs.x, cs.y, hn, ax, ay);
            if (ax < 0 || ay < 0 ||
                static_cast<std::uint32_t>(ax) >= cs.width ||
                static_cast<std::uint32_t>(ay) >= cs.height) {
                continue;
            }
            const std::size_t ti = static_cast<std::size_t>(ay) * cs.width +
                                   static_cast<std::size_t>(ax);
            const std::uint64_t key = tile_key(cs.level, ti);
            if (std::find(crop_tiles.begin(), crop_tiles.end(), key) ==
                crop_tiles.end()) {
                continue;
            }
            crop_owner[key] = si;
        }
    }
    for (const auto& kv : crop_owner) crop_structures[kv.second].crops += 1;

    std::map<const MapHs*, int> crop_count;
    for (const CropStructure& cs : crop_structures) {
        crop_count[cs.hs] = cs.crops;
    }

    // Owned structures pay their income source's Income().
    for (std::size_t li = 0; li < map.levels.size(); ++li) {
        const MapLevel& lv = map.levels[li];
        if (lv.width == 0) continue;
        for (std::size_t ti = 0; ti < lv.tiles.size(); ++ti) {
            const MapTile& tile = lv.tiles[ti];
            for (const MapHs& hs : tile.objects) {
                if (!is_player_structure_class(hs.class_id)) continue;
                const std::size_t owner = hs.structure_owner;
                if (owner >= gs.players.size()) continue;
                if (hs.razed) continue;
                int income = 0;
                if (hs.class_id == kCityClassId) {
                    // TCity.UpdateIncome@557abb94: razed/migrating/building
                    // cities pay nothing; otherwise size*5 + crops*2, +25%
                    // while idle or producing merchandise.
                    const int action = city_active_action(hs);
                    if (action != 4) {
                        const MapResource* r =
                            find_map_resource(resources, hs.resource);
                        const int size = r ? resource_city_size(*r) : 0;
                        income = city_income(size, crop_count[&hs], action);
                    }
                } else if (hs.class_id == kFarmClassId) {
                    // TPlayerCropStructure.UpdateIncome@557a6230 / Farm
                    // CropsEnabled@557b2c88: 2 gold per owned crop tile.
                    if (tile.terrain != 5) income = crop_count[&hs] * 2;
                } else if (hs.class_id == kMineClassId) {
                    // Mine.TMine.GetIncome@557b43e4: flat 10.
                    income = 10;
                } else if (is_production_place_class(hs.class_id)) {
                    // TProductionPlace.GetBaseIncome@557be4e0 / GetIncome.
                    // A magic node has a power source, not a gold source.
                    if (!production_place_is_magic_node(hs.class_id)) {
                        const int base =
                            hs.class_id == kShipyardClassId ? 0 : 10;
                        income = production_place_income(
                            base, production_place_merchandise(hs));
                    }
                }
                gs.players[owner].building_income += income;

                // TPlayerMagicControl.GetPower@5577ca00: the registered
                // power sources. `PowerNode.GetPower@557d03c8` is flat 10;
                // a magic node's `TProductionPlace.GetPower@557be468` is
                // `picks*5 + 10` for the owner's picks in its sphere (0 picks
                // -> no power). Gold production places are sphere 8 -> 0.
                if (hs.class_id == kPowerNodeClassId) {
                    power_income[owner] += 10;
                } else {
                    const int sphere = magic_node_sphere(hs.class_id);
                    if (sphere != 0) {
                        const int picks =
                            leader_sphere_picks(gs.players[owner].leader, sphere);
                        if (picks > 0) power_income[owner] += picks * 5 + 10;
                    }
                }
            }
        }
    }

    // TPlayerMagicControl.GetManaIncome@5577c95c / GetNetPowerToMana@5577cee4:
    // net power minus the share sent to research, then the four AI multipliers
    // (1.25 / 1.5 / 1.75 / 2.0 at 0x5577cf70..0x5577cf7c).
    for (std::size_t i = 0; i < gs.players.size(); ++i) {
        Player& p = gs.players[i];
        p.magic.power_income = power_income[i];
        // GetNetPower@5577cec4 halves the power while a global enchantment's
        // bit 8 is set; that global state is not modelled here.
        const int net = p.magic.power_income;
        const int to_research =
            delphi_round(net * p.magic.power_to_research / 100.0);
        int mana = net - to_research;
        switch (p.type) {
            case PlayerType::AI2: mana = delphi_round(mana * 1.25); break;
            case PlayerType::AI3: mana = delphi_round(mana * 1.5); break;
            case PlayerType::AI6: mana = delphi_round(mana * 1.75); break;
            case PlayerType::AI7: mana = delphi_round(mana * 2.0); break;
            default: break;
        }
        p.magic.mana_income = mana;
    }

    // TArmy.UpdateUpkeep@5578cc08 / TPlayer.SetArmyUpkeep@557520d4.
    for (const MapLevel& lv : map.levels) {
        for (const MapTile& tile : lv.tiles) {
            for (const MapHs& hs : tile.objects) {
                if (hs.class_id != kMapArmyHsClassId) continue;
                const std::size_t owner = hs.owner;
                if (owner >= gs.players.size()) continue;
                gs.players[owner].army_upkeep += army_upkeep(hs, units);
            }
        }
    }

    for (std::size_t i = 0; i < gs.players.size(); ++i) gs.update_income(i);
}

// ---------------------------------------------------------------------------
// Gems.TGems - the gold piles (see aow/game_state.hpp, docs/28_ECONOMY.md).
// ---------------------------------------------------------------------------

std::uint32_t delphi_random(std::uint32_t& state, std::uint32_t range) {
    // Borland `System._RandInt` (Delphi 3..7), the routine behind
    // `System.Random(max)`:
    //   RandSeed := RandSeed * $08088405 + 1
    //   EAX      := high dword(RandSeed * max)
    // `TAoWHSMap.Random@5577827c` swaps the map's +0x230 in and out around the
    // call, so `state` is the map's own sequence (rw tag 0x1c).
    if (range == 0) return 0;  // Delphi raises; no caller passes 0
    state = state * 0x08088405u + 1u;
    return static_cast<std::uint32_t>(
        (static_cast<std::uint64_t>(state) * range) >> 32);
}

std::int32_t generate_gems_amount(std::int32_t size, std::uint32_t& state,
                                  std::uint8_t& size_out,
                                  std::int32_t current_amount) {
    // Gems.TGems.GenerateGemsAmount@557c5010:
    //   if +0x20 = 4 then +0x20 := Random(3) + 1
    //   case +0x20 of
    //     1: +0x1c := Random(25) + 25
    //     2: +0x1c := Random(25) + 50
    //     3: +0x1c := Random(50) + 100
    // The size is written back before the case, so a "random" 4 becomes a
    // concrete 1..3 in the object (and is stored as such on the next save).
    std::uint8_t s = static_cast<std::uint8_t>(size);
    if (s == 4) {
        s = static_cast<std::uint8_t>(delphi_random(state, 3) + 1);
    }
    size_out = s;
    switch (s) {
        case 1:
            return static_cast<std::int32_t>(delphi_random(state, 0x19)) + 0x19;
        case 2:
            return static_cast<std::int32_t>(delphi_random(state, 0x19)) + 0x32;
        case 3:
            return static_cast<std::int32_t>(delphi_random(state, 0x32)) + 100;
        default:
            // Unknown size class: the original leaves +0x1c untouched.
            return current_amount;
    }
}

int generate_gems_amounts(Map& map) {
    // `TPlayerControl.NewDay@55754da4` sends 0x20020001 to the map after the
    // day counter becomes 1 (`*(map+0x174) == 1` is the gate in
    // `Gems.TGems.MsgProc@557c50c4`). The message reaches each level, tile and
    // object in the order the container stores them; that is the order the
    // shared RNG is consumed in, so replaying it here reproduces the original's
    // amounts for a scenario (turn_order_mode 0 does no draw of its own during
    // StartNewMap/NewDay - a mode 1/2 map regenerates its order first).
    int n = 0;
    for (MapLevel& lv : map.levels) {
        for (MapTile& tile : lv.tiles) {
            for (MapHs& hs : tile.objects) {
                if (hs.class_id != kGemsClassId) continue;
                std::uint8_t size = hs.gems_size;
                hs.gems_gold = generate_gems_amount(
                    hs.gems_size, map.rand_seed, size, hs.gems_gold);
                hs.gems_size = size;
                ++n;
            }
        }
    }
    return n;
}

GoldPickup gems_army_entered(Map& map, GameState& gs, std::size_t level, int x,
                             int y, int player) {
    GoldPickup out;
    out.player = player;
    // `Gems.TGems.ArmyEntered@557c50f8` line 71: the whole routine is skipped
    // when the entering army's owner (`TArmy+0x12`) is 0 - the independents
    // never collect a pile.
    if (player <= 0) return out;
    // `TPlayerList.GetPlayers(index)` resolves the seat by its *id*, not by its
    // roster position (`GameState::index_of`). An unknown seat is a no-op.
    const std::size_t seat = gs.index_of(static_cast<std::uint8_t>(player));
    if (seat >= gs.players.size()) return out;
    if (level >= map.levels.size()) return out;
    MapLevel& lv = map.levels[level];
    if (x < 0 || y < 0 || static_cast<std::uint32_t>(x) >= lv.width ||
        static_cast<std::uint32_t>(y) >= lv.height) {
        return out;
    }
    MapTile& tile = lv.tiles[static_cast<std::size_t>(y) * lv.width +
                             static_cast<std::size_t>(x)];
    // The field delivers the 0x1102 message to every object on it in placement
    // order, and each TGems answers with ArmyEntered. Walk backwards so the
    // erase is cheap, then put `amounts` back into placement order.
    for (std::size_t i = tile.objects.size(); i-- > 0;) {
        MapHs& hs = tile.objects[i];
        if (hs.class_id != kGemsClassId) continue;
        const int amount = hs.gems_gold;
        // `TPlayer.SetGems@55751f38`: `player+0xc4 := player+0xc4 + amount`.
        // That field IS the gold treasury (docs/28_ECONOMY.md); the same setter
        // also fires the player's GemsChanged notify event and checks the gold
        // victory condition (map+0x14d == 2), which the port has no UI for.
        gs.players[seat].gold += amount;
        out.amounts.push_back(amount);
        // `(**(code **)(*param_1 + -4))(param_1, 1)`: vmtDestroy (the class
        // reference's -4 slot). The pile is destroyed even when it paid 0.
        tile.objects.erase(tile.objects.begin() +
                           static_cast<std::ptrdiff_t>(i));
    }
    std::reverse(out.amounts.begin(), out.amounts.end());
    return out;
}

std::vector<std::uint8_t> generate_turn_order(int player_count,
                                              std::uint32_t rng_seed) {
    // TAoWHSMap.GenerateNewTurnOrder (0x557782fc): the pool is 1..N; the order
    // starts with the independents (0), then a random draw from the pool is
    // appended and removed until it is empty.
    std::vector<std::uint8_t> order;
    order.push_back(0);
    if (player_count < 1) return order;
    std::vector<std::uint8_t> pool;
    for (int i = 1; i <= player_count; ++i) {
        pool.push_back(static_cast<std::uint8_t>(i));
    }
    std::uint32_t state = rng_seed ? rng_seed : 0x1234567u;
    while (!pool.empty()) {
        const std::size_t pick = xs32(state) % pool.size();
        order.push_back(pool[pick]);
        pool.erase(pool.begin() + static_cast<std::ptrdiff_t>(pick));
    }
    return order;
}

void GameState::update_income(std::size_t player_index) {
    if (player_index >= players.size()) return;
    Player& p = players[player_index];
    // TPlayer.UpdateIncome (0x55751f88): income starts at +0xb0 and adds every
    // registered TIncomeSource's Income() (the vtable +0x50 call). The sources
    // are the player's owned city/farm/mine/production-place structures, summed
    // into building_income by compute_economy().
    const std::int32_t v = p.base_income + p.building_income;
    // The four AI branches multiply by a float constant and round with Delphi's
    // System.ROUND; CPU King/Emperor then add their flat bonus (which the
    // percentage does NOT apply to). Constants at 0x5575205c/60/64 are 1.0,
    // 2.0 and 2.5; the Emperor branch is `x*3 + 100`.
    switch (p.type) {
        case PlayerType::AI2:  // CPU Knight: *1.0
            p.income = v;
            break;
        case PlayerType::AI3:  // CPU Lord: *2.0
            p.income = delphi_round(v * 2.0);
            break;
        case PlayerType::AI6:  // CPU King: *2.5 + 50
            p.income = delphi_round(v * 2.5) + 0x32;
            break;
        case PlayerType::AI7:  // CPU Emperor: *3 + 100
            p.income = delphi_round(v * 3.0) + 0x64;
            break;
        default:
            p.income = v;
            break;
    }
}

void GameState::start_new_map() {
    // TAoWHSMap.StartNewMap (0x55777d68): for every roster entry,
    // SetGems(player, player+0xac) - the live treasury starts at the scenario's
    // starting gold. CPU King (type 6) gets +250 and CPU Emperor (type 7) +500.
    for (Player& p : players) {
        p.gold = p.start_gold;
        // StartNewMap also seeds magic.mana (+0x24) from magic.starting_mana
        // (+0x18) and gives CPU King/Emperor +250/+500 mana.
        p.magic.mana = p.magic.starting_mana;
        if (p.type == PlayerType::AI6) {
            p.gold += 250;
            p.magic.mana += 250;
        } else if (p.type == PlayerType::AI7) {
            p.gold += 500;
            p.magic.mana += 500;
        }
    }
}

void GameState::begin_day(std::uint32_t rng_seed) {
    if (turn_order.empty()) {
        turn_order = generate_turn_order(player_count(), rng_seed);
        turn_in_order = 0;
    }
}

void GameState::new_day(std::uint32_t rng_seed) {
    // TPlayerControl.NewDay (0x55754da4): the day counter (map +0x174) advances,
    // then the turn position resets; the order list itself is regenerated only
    // when the map's turn-order mode is 2 (`GenerateNewTurnOrder`).
    ++day;
    if (turn_order_mode == 2 || turn_order.empty()) {
        turn_order = generate_turn_order(player_count(), rng_seed);
    }
    turn_in_order = 0;
    // Turn limit reached -> every player is out (TAoWHSMap_GameOver status 2).
    if (turn_limit > 0 && day >= turn_limit) {
        for (Player& p : players) p.game_over_status = 2;
    }
}

void GameState::begin_player_turn(std::size_t player_index) {
    if (player_index >= players.size()) return;
    Player& p = players[player_index];
    update_income(player_index);
    // TPlayerControl.NewTurn (0x557553c4): from day 2 on, credit income minus
    // upkeep; a deficit clamps gold at 0 and lowers the food penalty (-5, floor
    // -50), otherwise the penalty resets. The player's end-turn flag is cleared
    // in the same block (`TPlayer.SetEndTurn(player, false)`), so a seat always
    // starts its turn with the flag down.
    if (day != 1) {
        p.gold = p.gold + p.income - p.upkeep();
        if (p.gold < 0) {
            p.gold = 0;
            p.food_penalty = static_cast<std::int8_t>(p.food_penalty - 5);
            if (p.food_penalty < -50) p.food_penalty = -50;
        } else {
            p.food_penalty = 0;
        }
        set_end_turn(player_index, false);
    }
    // TPlayerControl.NewTurn posts the 0x20020002 "turn start" message outside
    // the day!=1 gold branch, and `TPlayerMagicControl.MsgProc@5577d264` answers
    // it with `TPlayerMagicControl.NewTurn@5577cc2c` when the message carries
    // this player's index. That routine ends with
    //     mana = mana + GetManaIncome() - GetManaUpkeep();
    //     if (mana < 0) { OutOfMana(-mana); mana = 0; }
    // The mana read-out's green "+N" is GetManaIncome, so without this step the
    // bar advertised income the treasury never received (a map whose starting
    // mana tag is 0 stayed at 0 forever).
    p.magic.mana += p.magic.mana_income - p.magic.mana_upkeep;
    if (p.magic.mana < 0) p.magic.mana = 0;
    // The player's turn counter always advances.
    p.turn_count += 1;
}

std::uint8_t GameState::current() const {
    if (turn_order.empty() || turn_in_order >= turn_order.size()) return 0xff;
    return turn_order[turn_in_order];
}

std::size_t GameState::index_of(std::uint8_t player_index) const {
    for (std::size_t i = 0; i < players.size(); ++i) {
        if (players[i].index == player_index) return i;
    }
    return players.size();
}

bool GameState::player_playing(std::uint8_t player_index) const {
    const std::size_t i = index_of(player_index);
    if (i >= players.size()) return false;
    // +0x45, the latch `TPlayerList.Update` files the active lists from and
    // `TTurnPlayerControl.EndTurn` tests. (A seat put out by the turn limit has
    // this cleared by `TPlayer.Validate`; `active()` is the separate +0x44.)
    return players[i].playing;
}

bool GameState::set_end_turn(std::size_t player_index, bool value) {
    if (player_index >= players.size()) return false;
    if (players[player_index].end_turn == value) return false;
    players[player_index].end_turn = value;
    // TAoWHSMap.TriggerEndTurnEvent (0x55776474): notify the map's event list.
    // The port has no listeners yet (the Events floater reads the seat itself),
    // so the flag is the whole observable effect.
    return true;
}

std::uint8_t GameState::advance_turn(std::uint32_t seed) {
    if (turn_order.empty()) {
        new_day(seed);
        if (turn_order.empty()) {
            current_player = 0xff;
            return 0xff;
        }
    }
    // TTurnPlayerControl.EndTurn (0x55756db8): walk the order from the player
    // who just ended. A seat that is not playing (+0x45 != 1) is started and
    // ended in the same pass (`TPlayerControl.NewTurn` + `EndTurn`), and the
    // first seat that *is* playing becomes the current player. Running off the
    // end of the order wraps into `NewDay` - the round only rolls over once
    // every player has had (and ended) their turn.
    std::size_t idx = turn_in_order;
    for (std::size_t guard = 0; guard <= turn_order.size(); ++guard) {
        ++idx;
        if (idx >= turn_order.size()) {
            new_day(seed);
            idx = 0;
            if (turn_order.empty()) {
                current_player = 0xff;
                return 0xff;
            }
        }
        const std::uint8_t c = turn_order[idx];
        const std::size_t pi = index_of(c);
        if (pi >= players.size() || !player_playing(c)) {
            if (pi < players.size()) {
                begin_player_turn(pi);
                set_end_turn(pi, true);
            }
            continue;
        }
        turn_in_order = idx;
        current_player = c;
        begin_player_turn(pi);
        return c;
    }
    current_player = 0xff;
    return 0xff;
}

void GameState::begin_first_turn(std::uint8_t seated) {
    if (turn_order.empty()) return;
    std::size_t pos = 0;
    for (std::size_t i = 0; i < turn_order.size(); ++i) {
        if (turn_order[i] == seated) {
            pos = i;
            break;
        }
    }
    turn_in_order = pos;
    current_player = turn_order[pos];
    const std::size_t pi = index_of(current_player);
    if (pi < players.size()) begin_player_turn(pi);
}

bool GameState::end_turn(std::uint8_t who) {
    // TAoWHSMap.EndTurn (0x5577838c): no seat -> refuse; while the event log is
    // executing and still holds entries -> refuse.
    if (who == 0xff) return false;
    if (event_log_busy && event_log_count != 0) return false;
    // FUN_5574757c: a busy player, or one that already ended, refuses.
    const std::size_t pi = index_of(who);
    if (pi >= players.size()) return false;
    if (players[pi].busy || players[pi].end_turn) return false;
    // TPlayerControl.EndTurn (0x5575530c): TPlayer.SetEndTurn(player, 1).
    set_end_turn(pi, true);
    // TTurnPlayerControl.EndTurn (0x55756db8): the hand-over happens only when
    // the player who ended *is* the one on turn.
    if (current() == who) advance_turn(rng_seed);
    return true;
}

bool GameState::cancel_end_turn(std::uint8_t who) {
    // TAoWHSMap.CancelEndTurn (0x557783c8) posts the end-turn token with the
    // cancel flag; TEndTurnTE.Execute (0x557473c4) then clears the flag. The
    // cancel is refused for a busy player or one that has not ended.
    if (who == 0xff) return false;
    const std::size_t pi = index_of(who);
    if (pi >= players.size()) return false;
    if (players[pi].busy || !players[pi].end_turn) return false;
    set_end_turn(pi, false);
    return true;
}

std::uint8_t apply_setup_player_types(GameState& gs,
                                      const std::uint8_t* race_type,
                                      std::size_t race_count) {
    // TSetupControl.SetupMap (0x557e0b64): for every settings record (one per map
    // player) it writes the screen's current type back into the roster -
    // `*(char *)(iVar4 + 0xa7) = *(char *)(iVar3 + 8)`. Entry 0 (the
    // independents) is never touched.
    for (Player& p : gs.players) {
        if (p.index == 0) continue;
        if (race_type != nullptr && static_cast<std::size_t>(p.race) < race_count) {
            p.type = static_cast<PlayerType>(race_type[p.race]);
        }
    }
    // TSetupControl.hPickMap (0x557e20c8): its local_5 flag starts set, so the
    // first Human slot keeps type 0 and every later Human gets AI1 (CPU Squire).
    // (ResetPlayerSettings 0x557e1378 reaches the same state: a reset slot whose
    // map default is Human becomes pidCPU1.)
    bool seen_human = false;
    std::uint8_t human = 0xff;
    for (Player& p : gs.players) {
        if (p.index == 0) continue;
        if (p.type != PlayerType::Human) continue;
        if (seen_human) {
            p.type = PlayerType::AI1;
        } else {
            seen_human = true;
            human = p.index;
        }
    }
    // SetupMap changes the player types before the scenario starts; the types
    // drive both the AI income multiplier (TPlayer.UpdateIncome) and the CPU
    // starting-gold bonus (TAoWHSMap.StartNewMap). Re-run both so a race marked
    // CPU King/Emperor at the setup screen gets the right economy.
    for (std::size_t i = 0; i < gs.players.size(); ++i) gs.update_income(i);
    gs.start_new_map();
    return human;
}

bool load_game_state(const Map& map, const std::vector<MapResource>& resources,
                     const UnitCatalog* units, GameState& out,
                     std::string& error) {
    out = GameState();
    if (map.region.empty()) {
        error = "map has no decoded region (load it with load_map first)";
        return false;
    }
    const std::uint8_t* R = map.region.data();
    const std::size_t n = map.region.size();

    Table outer;
    if (!read_eobject_table(R, n, 0, outer)) {
        error = "cannot read map header table";
        return false;
    }
    const std::size_t t0 = find_off(outer, 0);
    if (t0 == kNpos) {
        error = "map header has no top object";
        return false;
    }
    Table top;
    if (!read_eobject_table(R, n, outer.base + t0, top)) {
        error = "cannot read the map's top object";
        return false;
    }

    const std::size_t roster = find_off(top, kRosterTag);
    if (roster != kNpos) {
        parse_roster(R, n, top.base + roster, out);
    }
    tally_troops(map, out);
    // TAoWHSMap.StartNewMap@55777d68 seeds the live treasury from the scenario's
    // starting gold; then the map's structures/armies define the per-turn
    // income and upkeep. A map loaded without Release.hss still gets base
    // income and the start-gold seeding.
    compute_economy(map, resources, units, out);
    out.start_new_map();
    for (std::size_t i = 0; i < out.players.size(); ++i) out.update_income(i);
    out.loaded = true;
    return true;
}

bool load_game_state(const Map& map, GameState& out, std::string& error) {
    static const std::vector<MapResource> kNoResources;
    return load_game_state(map, kNoResources, nullptr, out, error);
}

// ---------------------------------------------------------------------------
// Building acquisition (docs/29_BUILDING_ACQUISITION.md)
//
// The recovered rules live in aow/structure_capture.hpp; this section is their
// implementation. The engine decompiles they mirror are in
// re/exports/capture/ (AoWEPACK.dpl, GhidraMCP).
// ---------------------------------------------------------------------------
namespace {

bool is_city(const MapHs& hs) { return hs.class_id == kCityClassId; }

// Node/altar class families used only for the English event-log name.
bool is_node_class(std::uint32_t c) {
    switch (c) {
    case 0x20371:  // LifeNode
    case 0x204ae:  // DeathNode
    case 0x204b3:  // WaterNode
    case 0x204ce:  // EarthNode
    case 0x203de:  // FireNode
    case 0x2094e:  // AirNode
    case 0x2019a:  // PowerNode
    case 0x20822:  // RandomNode
        return true;
    default: return false;
    }
}

bool is_altar_class(std::uint32_t c) {
    switch (c) {
    case 0x203f1:  // Altar
    case 0x20233:  // Altar of Life
    case 0x20235:  // Altar of Death
    case 0x2023a:  // Altar of Fire
    case 0x2023d:  // Altar of Lightning
    case 0x2024b:  // Altar of Ice
    case 0x2030c:  // Altar of Pestilence
        return true;
    default: return false;
    }
}

// Does this structure store its razed byte as a one-byte status `== 1`
// (TProductionPlace: rw tag 0x0a -> +0x48) instead of bit 0 (Mine, PowerNode,
// Farm, Tower, Altar)? TCity uses bit 1 of +0x44 and is handled separately.
bool razed_as_value(const MapHs& hs) {
    return is_production_place_class(hs.class_id);
}

void set_razed_byte(MapHs& hs, bool razed) {
    if (is_city(hs)) {
        hs.city_flags = static_cast<std::uint8_t>(
            razed ? (hs.city_flags | 0x02) : (hs.city_flags & ~0x02));
        return;
    }
    if (razed_as_value(hs)) {
        hs.structure_status = razed ? 1 : 0;
        return;
    }
    hs.structure_status = static_cast<std::uint8_t>(
        razed ? (hs.structure_status | 0x01) : (hs.structure_status & ~0x01));
}

// `TProductionControl.PlayerChanged@5575cef0` (TCity.SetPlayer runs it before
// the base SetPlayer): keep item 0 when a production is actively running, drop
// the rest of the queue; drop everything - item 0 included - when it is idle.
void city_production_player_changed(MapHs& city) {
    if (city.production_queue.size() > 1) {
        city.production_queue.erase(city.production_queue.begin() + 1,
                                    city.production_queue.end());
    }
    city.loop_queue = false;
    if (!city.production_queue.empty() && city.production_state != 2) {
        city.production_queue.erase(city.production_queue.begin());
        city.production_current_present = false;
        city.production_turns_left = 0;
        city.production_turns_total = 0;
        city.production_gold_paid = 0;
        city.production_mana_paid = 0;
        city.production_state = 0;
    }
}

// `TCity.SetPlayer`'s migrate-action removal (GetActiveAction() == 4): the
// production queue's item 0 is a migration, so it is freed.
void city_drop_migration(MapHs& city) {
    if (city.production_queue.empty()) return;
    if (city.production_queue.front().kind != 13) return;  // 13 = migrate
    city.production_queue.erase(city.production_queue.begin());
    city.production_current_present = false;
    city.production_turns_left = 0;
    city.production_turns_total = 0;
    city.production_gold_paid = 0;
    city.production_mana_paid = 0;
    city.production_state = 0;
}

// `TCity.SetRazed`/`TProductionPlace.SetRazed` also stop the production
// control: the queue and the running item are gone.
void city_production_clear(MapHs& city) {
    city.production_queue.clear();
    city.production_current_present = false;
    city.production_turns_left = 0;
    city.production_turns_total = 0;
    city.production_gold_paid = 0;
    city.production_mana_paid = 0;
    city.production_state = 0;
    city.loop_queue = false;
}

const MapHs* army_on_tile(const MapTile& tile) {
    for (auto it = tile.objects.rbegin(); it != tile.objects.rend(); ++it) {
        if (it->class_id != kMapArmyHsClassId) continue;
        if (it->units.empty()) continue;
        return &*it;
    }
    return nullptr;
}

// The structure's own hexes: `TStructure+0x54` (its TTerrainList entry count)
// walked from the home hex with `HSEngine.InitHNWalk` / `WalkToNextHN`, i.e.
// object_covers_hex's rule. Returns false for an unusable ref.
bool covered_hexes(const Map& map, const MapHs& hs, const StructureRef& ref,
                   const std::vector<MapResource>& res,
                   std::vector<std::pair<int, int>>& out) {
    out.clear();
    if (!ref.valid() || static_cast<std::size_t>(ref.level) >= map.levels.size()) {
        return false;
    }
    const MapLevel& lv = map.levels[static_cast<std::size_t>(ref.level)];
    out.emplace_back(ref.x, ref.y);
    const MapResource* r = find_map_resource(res, hs.resource);
    if (r == nullptr) return true;
    for (std::size_t k = 1; k < r->terrain_list.size(); ++k) {
        const std::uint16_t e = r->terrain_list[k];
        const int et = static_cast<std::int8_t>(e & 0xff);
        const int eo = static_cast<std::int8_t>((e >> 8) & 0xff);
        if (et < 0 && eo < 0) continue;
        int hx = 0, hy = 0;
        hn_walk_hex(ref.x, ref.y, static_cast<int>(k), hx, hy);
        if (hx < 0 || hy < 0 || hx >= static_cast<int>(lv.width) ||
            hy >= static_cast<int>(lv.height)) {
            continue;
        }
        out.emplace_back(hx, hy);
    }
    return true;
}

}  // namespace

bool structure_razed(const MapHs& hs) { return hs.razed; }
bool structure_building(const MapHs& hs) { return hs.building; }

std::uint8_t structure_flag_id(const MapHs& hs,
                               const std::vector<std::uint8_t>& player_races) {
    const int owner = static_cast<std::int8_t>(hs.structure_owner);
    // `TCity.UpdateFlagID@557ac8b0` and `TPlayerStructure.UpdateFlagID
    // @55760ed8`: a razed structure draws nothing unless a builder is at work,
    // and a city's +0x44 bit 0 also suppresses the flag.
    if (is_city(hs) && (hs.city_flags & 0x01) != 0) return kStructureFlagNone;
    if (hs.razed && !hs.building) return kStructureFlagNone;
    if (is_city(hs)) {
        // The city level occupies the flag byte's top two bits. The engine's
        // byte arithmetic is `(level - 1) * 0x40`; level 0 wraps to pole 3.
        const std::uint8_t pole = static_cast<std::uint8_t>(
            ((hs.city_level - 1) & 0x03) * 0x40);
        if (owner >= 1 && static_cast<std::size_t>(owner) < player_races.size()) {
            return static_cast<std::uint8_t>(player_races[owner] + pole);
        }
        return static_cast<std::uint8_t>(kStructureFlagNeutral + pole);
    }
    if (owner >= 1 && static_cast<std::size_t>(owner) < player_races.size()) {
        return player_races[owner];
    }
    // Every non-city TPlayerStructureResource clears +0x64, so an unowned mine
    // or node draws no flag at all.
    return kStructureFlagNone;
}

std::uint8_t structure_move_action(const MapHs& hs, int player) {
    if (hs.razed) return 0;
    const int owner = static_cast<std::int8_t>(hs.structure_owner);
    if (is_city(hs)) {
        // `TCity.GetMoveAction@557ad0d4`: a neutral city attacks unless its
        // production/owned bit 0 is set; `JoinAmount` decides between attack (1)
        // and peaceful join (2) - the port has no diplomacy, so it attacks.
        if (owner == 0) {
            if ((hs.city_flags & 0x01) == 0) return 1;
            return 0;
        }
        if (owner != player && owner != -1) return 1;
        return 0;
    }
    // `TPlayerStructure.GetMoveAction@55761620`: owner 0 counts as "walk in"
    // only because every non-city resource has +0x64 == 0; an army stepping on
    // it then claims it through UpdatePlayer.
    if (player != owner && owner != 0) return 1;
    return 0;
}

MapHs* structure_object(Map& map, const StructureRef& ref) {
    if (!ref.valid() || static_cast<std::size_t>(ref.level) >= map.levels.size()) {
        return nullptr;
    }
    MapLevel& lv = map.levels[static_cast<std::size_t>(ref.level)];
    if (ref.x < 0 || ref.y < 0 || static_cast<std::uint32_t>(ref.x) >= lv.width ||
        static_cast<std::uint32_t>(ref.y) >= lv.height) {
        return nullptr;
    }
    MapTile& t = lv.tiles[static_cast<std::size_t>(ref.y) * lv.width +
                          static_cast<std::size_t>(ref.x)];
    if (ref.object >= t.objects.size()) return nullptr;
    return &t.objects[ref.object];
}

const MapHs* structure_object(const Map& map, const StructureRef& ref) {
    if (!ref.valid() || static_cast<std::size_t>(ref.level) >= map.levels.size()) {
        return nullptr;
    }
    const MapLevel& lv = map.levels[static_cast<std::size_t>(ref.level)];
    if (ref.x < 0 || ref.y < 0 || static_cast<std::uint32_t>(ref.x) >= lv.width ||
        static_cast<std::uint32_t>(ref.y) >= lv.height) {
        return nullptr;
    }
    const MapTile& t = lv.tiles[static_cast<std::size_t>(ref.y) * lv.width +
                                static_cast<std::size_t>(ref.x)];
    if (ref.object >= t.objects.size()) return nullptr;
    return &t.objects[ref.object];
}

std::vector<StructureRef> structures_at(const Map& map, std::size_t level,
                                        int x, int y,
                                        const std::vector<MapResource>& res) {
    std::vector<StructureRef> out;
    if (level >= map.levels.size()) return out;
    const MapLevel& lv = map.levels[level];
    if (x < 0 || y < 0 || static_cast<std::uint32_t>(x) >= lv.width ||
        static_cast<std::uint32_t>(y) >= lv.height) {
        return out;
    }
    // Objects stored on the clicked tile, topmost (last placed) first.
    const MapTile& home = lv.tiles[static_cast<std::size_t>(y) * lv.width +
                                   static_cast<std::size_t>(x)];
    for (std::size_t i = home.objects.size(); i-- > 0;) {
        const MapHs& hs = home.objects[i];
        if (!is_player_structure_class(hs.class_id)) continue;
        out.push_back(StructureRef{static_cast<int>(level), x, y, i});
    }
    // Multi-hex buildings stored elsewhere whose footprint covers this hex,
    // nearest home hex first (TMultiHexMO.UpdateMapFields registration order).
    struct Cover {
        int dist = 0;
        StructureRef ref;
    };
    std::vector<Cover> covers;
    for (std::size_t ty = 0; ty < lv.height; ++ty) {
        for (std::size_t tx = 0; tx < lv.width; ++tx) {
            if (static_cast<int>(tx) == x && static_cast<int>(ty) == y) continue;
            const MapTile& t = lv.tiles[ty * lv.width + tx];
            for (std::size_t i = 0; i < t.objects.size(); ++i) {
                const MapHs& hs = t.objects[i];
                if (!is_player_structure_class(hs.class_id)) continue;
                if (!object_covers_hex(hs, res, static_cast<int>(tx),
                                       static_cast<int>(ty), x, y)) {
                    continue;
                }
                Cover c;
                c.dist = hex_distance(static_cast<int>(tx), static_cast<int>(ty),
                                      x, y);
                c.ref = StructureRef{static_cast<int>(level),
                                     static_cast<int>(tx),
                                     static_cast<int>(ty), i};
                covers.push_back(c);
            }
        }
    }
    std::stable_sort(covers.begin(), covers.end(),
                     [](const Cover& a, const Cover& b) {
                         return a.dist < b.dist;
                     });
    for (const Cover& c : covers) out.push_back(c.ref);
    return out;
}

StructureRef structure_at(const Map& map, std::size_t level, int x, int y,
                          const std::vector<MapResource>& res) {
    const std::vector<StructureRef> all = structures_at(map, level, x, y, res);
    return all.empty() ? StructureRef{} : all.front();
}

int structure_size_hexes(const MapResource* res) {
    if (res == nullptr) return 1;
    int n = 0;
    for (std::uint16_t e : res->terrain_list) {
        if (static_cast<std::int8_t>(e & 0xff) >= 0) ++n;
    }
    return n > 0 ? n : 1;
}

std::string structure_display_name(const MapHs& hs, const MapResource* res) {
    if (hs.class_id == kCityClassId) {
        return hs.city_name.empty() ? std::string("City") : hs.city_name;
    }
    if (res != nullptr) {
        switch (res->class_id) {
        case 0x2037b: return "Mine";
        case 0x2043f: return "Tower";
        case 0x2036f: return "Farm";
        case 0x204bf: return "Shipyard";
        case 0x2092f: return "Builder's Guild";
        case 0x2019b: return "Power Node";
        case 0x2063f: return "Reflecting Pool";
        case 0x203f2: return "Altar";
        default: break;
        }
    }
    if (is_node_class(hs.class_id)) return "Magic Node";
    if (is_altar_class(hs.class_id)) return "Altar";
    switch (hs.class_id) {
    case kMineClassId: return "Mine";
    case 0x2043e: return "Tower";
    case kFarmClassId: return "Farm";
    case kShipyardClassId: return "Shipyard";
    case kBuildersGuildClassId: return "Builder's Guild";
    case kProductionPlaceClassId: return "Production Place";
    case kPowerNodeClassId: return "Power Node";
    case 0x2063e: return "Reflecting Pool";
    case 0x203f1: return "Altar";
    default: return "Structure";
    }
}

RebuildInfo structure_get_rebuild_info(const MapHs& hs, const MapResource* res) {
    // `TCity.GetRebuildInfo@557ab50c`: size*25 gold, size+1 turns.
    if (hs.class_id == kCityClassId) {
        const int size = structure_size_hexes(res);
        return RebuildInfo{size * 25, size + 1};
    }
    // `TStructure.GetRebuildInfo@5575f18c` reads the structure resource's
    // +0x5a/+0x5b (gold, turns). The port does not parse those two bytes yet.
    return RebuildInfo{};
}

StructureChange structure_update_player(Map& map, const StructureRef& ref,
                                        const std::vector<std::uint8_t>& player_races,
                                        const std::vector<MapResource>& res) {
    StructureChange ch;
    MapHs* hs = structure_object(map, ref);
    if (hs == nullptr) return ch;
    if (!is_player_structure_class(hs->class_id)) return ch;
    std::vector<std::pair<int, int>> hexes;
    if (!covered_hexes(map, *hs, ref, res, hexes)) return ch;
    const MapLevel& lv = map.levels[static_cast<std::size_t>(ref.level)];

    // `TPlayerStructure.UpdatePlayer@55760980`: `TMapField.GetObject(0x20217)`
    // on every hex the structure covers. One owner (or none) wins the structure;
    // two different owners make it contested, which stores owner -1.
    int candidate = -1;
    bool conflict = false;
    for (const auto& h : hexes) {
        const MapTile& t = lv.tiles[static_cast<std::size_t>(h.second) * lv.width +
                                    static_cast<std::size_t>(h.first)];
        const MapHs* army = army_on_tile(t);
        if (army == nullptr) continue;
        const int owner = static_cast<std::int8_t>(army->owner);
        if (candidate == -1) {
            candidate = owner;
        } else if (owner != candidate) {
            conflict = true;
        }
    }
    if (candidate == -1) return ch;  // no army on the structure: keep the owner
    const int want = conflict ? -1 : candidate;
    if (want == static_cast<std::int8_t>(hs->structure_owner)) return ch;
    return structure_set_player(map, ref, want, player_races, res);
}

bool structure_has_foreign_army(const Map& map, const StructureRef& ref,
                                int player,
                                const std::vector<MapResource>& res) {
    const MapHs* hs = structure_object(map, ref);
    if (hs == nullptr) return false;
    std::vector<std::pair<int, int>> hexes;
    if (!covered_hexes(map, *hs, ref, res, hexes)) return false;
    const MapLevel& lv = map.levels[static_cast<std::size_t>(ref.level)];
    for (const auto& h : hexes) {
        const MapTile& t = lv.tiles[static_cast<std::size_t>(h.second) * lv.width +
                                    static_cast<std::size_t>(h.first)];
        const MapHs* army = army_on_tile(t);
        if (army == nullptr) continue;
        if (static_cast<std::int8_t>(army->owner) != player) return true;
    }
    return false;
}

StructureChange structure_set_player(Map& map, const StructureRef& ref,
                                     int new_owner,
                                     const std::vector<std::uint8_t>& player_races,
                                     const std::vector<MapResource>& res) {
    StructureChange ch;
    MapHs* hs = structure_object(map, ref);
    if (hs == nullptr) return ch;
    if (new_owner < -1) new_owner = -1;
    if (new_owner > 255) new_owner = 255;
    const int old = static_cast<std::int8_t>(hs->structure_owner);
    ch.old_owner = old;
    ch.new_owner = new_owner;
    if (new_owner == old) return ch;
    // `if (GetRazed() && !GetBuilding()) return;` - a ruin changes hands only
    // once a builder has re-erected it.
    if (hs->razed && !hs->building) {
        ch.refused_razed = true;
        return ch;
    }
    // City.TCity.SetPlayer@557aa2c8 does this before the base SetPlayer.
    if (is_city(*hs)) city_production_player_changed(*hs);
    hs->seen_state = 0;  // TPlayerStructure+0x31

    // TPlayerStructure.StructureCaptured@55760ac4 / TCity.StructureCaptured
    // @557aa0a4: the event log fires for a capture from a real player. Taking an
    // independent (owner 0) structure is silent, exactly like the engine. The
    // engine's other two gates are the scenario's "no capture logs" byte
    // (map+0x3c bit 1) and the structure's revealed bit (+0xc & 0x20); the port
    // has no scenario flag and every structure is treated as revealed.
    if (old != 0) {
        ch.capture_logged = true;
        const MapResource* r = find_map_resource(res, hs->resource);
        const std::string name = structure_display_name(*hs, r);
        ch.text = is_city(*hs) ? "The city of " + name + " has been captured."
                               : "The " + name + " has been captured.";
    }

    hs->structure_owner =
        static_cast<std::uint8_t>(new_owner < 0 ? 0xff : new_owner);
    // UpdateUnfog + UpdateExploration: the new owner has now seen the building.
    if (new_owner >= 0) {
        std::vector<std::pair<int, int>> hexes;
        if (covered_hexes(map, *hs, ref, res, hexes)) {
            MapLevel& lv = map.levels[static_cast<std::size_t>(ref.level)];
            for (const auto& h : hexes) {
                MapTile& t = lv.tiles[static_cast<std::size_t>(h.second) * lv.width +
                                      static_cast<std::size_t>(h.first)];
                t.explored |= 1u << (static_cast<unsigned>(new_owner) & 0x1fu);
            }
        }
    }
    // UpdateFlagID (+0x1fc / TCity +0x1fc).
    hs->flag_id = structure_flag_id(*hs, player_races);
    ch.changed = true;

    // The base SetPlayer always ends with UpdatePlayer (+0x220): the army that
    // took the structure keeps it, an empty structure keeps its new owner and a
    // contested multi-hex city becomes owner -1.
    const StructureChange after =
        structure_update_player(map, ref, player_races, res);
    // TCity.SetPlayer's tail: a running migration is discarded after the base
    // SetPlayer. (The AI action revalidation has no port equivalent.)
    if (is_city(*hs)) city_drop_migration(*hs);
    if (after.changed) {
        StructureChange out = after;
        if (!ch.text.empty()) out.text = ch.text;
        out.old_owner = ch.old_owner;
        out.capture_logged = ch.capture_logged || after.capture_logged;
        return out;
    }
    return ch;
}

StructureChange structure_finalize_combat(
    Map& map, const StructureRef& ref, StructureCombatOutcome outcome, int attacker,
    const std::vector<std::uint8_t>& player_races,
    const std::vector<MapResource>& res) {
    StructureChange ch;
    // `TPlayerStructure.FinalizeCombat@55760cd4` / `TCity.FinalizeCombat
    // @557acefc`: only results 3 (`won`) and 6 (`retreated`) hand the structure
    // to the attacker taken from combat data +0x24. A loss/draw/breach failure
    // leaves it untouched.
    if (outcome != StructureCombatOutcome::won &&
        outcome != StructureCombatOutcome::retreated) {
        return ch;
    }
    ch = structure_set_player(map, ref, attacker, player_races, res);
    // TCity.FinalizeCombat runs UpdatePlayer once more after SetPlayer; the base
    // SetPlayer already did so, so this is a no-op for the ordinary case.
    const StructureChange after =
        structure_update_player(map, ref, player_races, res);
    if (after.changed) {
        StructureChange out = after;
        if (!ch.text.empty()) out.text = ch.text;
        out.old_owner = ch.old_owner;
        out.capture_logged = ch.capture_logged || after.capture_logged;
        return out;
    }
    return ch;
}

StructureChange structure_set_razed(Map& map, const StructureRef& ref, bool razed,
                                    const std::vector<std::uint8_t>& player_races,
                                    const std::vector<MapResource>& res) {
    StructureChange ch;
    MapHs* hs = structure_object(map, ref);
    if (hs == nullptr) return ch;
    if (hs->razed == razed) return ch;
    if (razed) {
        // TCity.SetRazed@557ab47c / TMine.SetRazed@557b4648 / ...: SetPlayer(-1)
        // first (the razed bit is not set yet, so the transition is allowed),
        // then the class's razed byte. An army still standing on the hex makes
        // SetPlayer's UpdatePlayer pass hand it straight back - the engine does
        // exactly that, which is how a razing player keeps the ruin.
        ch = structure_set_player(map, ref, -1, player_races, res);
        set_razed_byte(*hs, true);
        hs->razed = true;
        if (is_city(*hs)) {
            hs->wall_type = 0;  // TCity.SetRazed -> SetWallType(0)
            city_production_clear(*hs);
        } else if (razed_as_value(*hs)) {
            city_production_clear(*hs);  // TProductionPlace.SetRazed stops it
        }
        hs->flag_id = kStructureFlagNone;
        if (ch.text.empty()) {
            const MapResource* r = find_map_resource(res, hs->resource);
            ch.text = "The " + structure_display_name(*hs, r) + " has been razed.";
        }
        ch.changed = true;
        ch.new_owner = static_cast<std::int8_t>(hs->structure_owner);
        return ch;
    }
    // Clearing the bit (`SetRazed(false)`) does not restore an owner.
    set_razed_byte(*hs, false);
    hs->razed = false;
    hs->flag_id = structure_flag_id(*hs, player_races);
    ch.changed = true;
    ch.old_owner = static_cast<std::int8_t>(hs->structure_owner);
    ch.new_owner = ch.old_owner;
    return ch;
}

StructureChange structure_complete_rebuild(
    Map& map, const StructureRef& ref, int player,
    const std::vector<std::uint8_t>& player_races,
    const std::vector<MapResource>& res) {
    StructureChange ch;
    MapHs* hs = structure_object(map, ref);
    if (hs == nullptr) return ch;
    // The construction TE finished: GetBuilding clears, the razed bit clears and
    // TPlayerStructure.ExecuteBuild/ExecuteRebuild then calls SetPlayer(builder).
    hs->building = false;
    if (hs->razed) structure_set_razed(map, ref, false, player_races, res);
    ch = structure_set_player(map, ref, player, player_races, res);
    if (ch.changed) {
        const MapResource* r = find_map_resource(res, hs->resource);
        ch.text = "The " + structure_display_name(*hs, r) + " has been rebuilt.";
    }
    return ch;
}

bool structure_city_check_rebellion(Map& map, const StructureRef& ref,
                                    int civil_status, int roll,
                                    const std::vector<std::uint8_t>& player_races,
                                    const std::vector<MapResource>& res) {
    const MapHs* hs = structure_object(map, ref);
    if (hs == nullptr) return false;
    if (hs->class_id != kCityClassId) return false;
    const int owner = static_cast<std::int8_t>(hs->structure_owner);
    if (owner == 0) return false;  // already independent
    if (hs->razed) return false;
    if (civil_status >= 2) return false;
    // `TCity.CheckRebellion@557ab534`: `TAoWHSMap.Random(100) <= 50` at status 0
    // (Unruly) and `<= 10` at status 1 (Unrest). `roll` is that 0..99 draw.
    const int threshold = civil_status == 0 ? 50 : 10;
    if (roll > threshold) return false;
    const StructureChange ch =
        structure_set_player(map, ref, 0, player_races, res);
    return ch.changed;
}

std::vector<std::uint8_t> map_player_races(const Map& map) {
    std::vector<std::uint8_t> races;
    if (map.region.empty()) return races;
    const std::uint8_t* R = map.region.data();
    const std::size_t n = map.region.size();
    EObjectTable outer;
    if (!read_eobject_table(R, n, 0, outer)) return races;
    const std::size_t t0 = eobject_find(outer, 0);
    if (t0 == kNpos) return races;
    EObjectTable top;
    if (!read_eobject_table(R, n, outer.base + t0, top)) return races;
    const std::size_t roster = eobject_find(top, kRosterTag);
    if (roster == kNpos) return races;
    GameState gs;
    parse_roster(R, n, top.base + roster, gs);
    races.reserve(gs.players.size());
    for (const Player& p : gs.players) races.push_back(p.race);
    return races;
}

}  // namespace aow