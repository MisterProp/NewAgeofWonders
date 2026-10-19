// movement.cpp - world-map unit movement (see aow/movement.hpp).
#include "aow/movement.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <queue>
#include <utility>

#include "aow/unit_catalog.hpp"

namespace aow {

namespace {

// Dijkstra queue entry: the accumulated cost and the hex index.
struct Node {
    int cost = 0;
    int index = 0;
    bool operator>(const Node& o) const { return cost > o.cost; }
};

// The six neighbour offsets, indexed [column parity][dir-1]. Straight from
// HSEngine.HNtoHX@5560e08c + the CenterHNtoHX@5560e17c column-stagger fold: an
// odd column moves one row down for an odd dx.
constexpr int kNeighbourDx[2][6] = {{0, 1, 1, 0, -1, -1}, {0, 1, 1, 0, -1, -1}};
constexpr int kNeighbourDy[2][6] = {{-1, -1, 0, 1, 0, -1}, {-1, 0, 1, 1, 1, 0}};

}  // namespace

// ---------------------------------------------------------------------------
// Tables
// ---------------------------------------------------------------------------
std::array<MovePointTable, 256> build_move_point_tables() {
    std::array<MovePointTable, 256> out;
    for (MovePointTable& t : out) t.fill(kImpassable);
    for (int mt = 0; mt < 256; ++mt) {
        MovePointTable& dst = out[static_cast<std::size_t>(mt)];
        for (int terrain = 0; terrain < kTerrainCount; ++terrain) {
            for (int overlay = 0; overlay < kOverlayCount; ++overlay) {
                const std::size_t idx =
                    static_cast<std::size_t>(terrain) * 16 +
                    static_cast<std::size_t>(overlay);
                // FUN_55744b2c: start at 0x7F and take the cheapest selected
                // source table (skipping 0xFF); 0x7F means "none applied".
                std::uint8_t v = 0x7F;
                for (int k = 0; k < 8; ++k) {
                    const std::uint8_t bit = kMovePointSourceTableBits[k];
                    if ((mt & bit) == 0) continue;
                    const std::uint8_t s = kMovePointSourceTables[k][idx];
                    if (s != kImpassable && s < v) v = s;
                }
                dst[idx] = (v == 0x7F) ? kImpassable : v;
            }
        }
    }
    return out;
}

std::uint8_t move_types_from_abilities(const std::array<bool, 256>& has) {
    auto ab = [&](unsigned id) { return has[id]; };
    std::uint8_t out = ab(0) ? std::uint8_t{1} : std::uint8_t{0};
    if (ab(1) || ab(0xa5) || ab(0x3b)) out |= 2;
    if (ab(2) || ab(0x17) || ab(0xa4) || ab(0xa6)) out |= 4;
    if (ab(3) || ab(0x9d)) out |= 8;
    if (ab(4) || ab(0x9d)) out |= 0x10;
    if (ab(5) || ab(0x9d)) out |= 0x20;
    if (ab(7) || ab(0xa3)) out |= 0x40;
    if (ab(0x2a)) out |= 0x80;
    return out;
}

MovePointTable unit_move_point_table(
    const std::array<MovePointTable, 256>& tables, std::uint8_t move_types) {
    // TAbstractUnit.CreateMovePointTable@0x5577fdc4 copies
    // MovePointTables[MoveTypesToInt(move_types)] (an identity mapping, see
    // AoWE.MoveTypesToInt@0x5575aec0). A fresh table is all-0xFF when the unit
    // has no movement ability at all.
    return tables[move_types];
}

MovePointTable unit_move_point_table(
    const std::array<MovePointTable, 256>& tables, std::uint8_t move_types,
    const std::array<bool, 256>& has, const PlayerMoveTraits& traits) {
    // TAbstractUnit.CreateMovePointTable@0x5577fdc4. The engine gates the whole
    // tweak block on the unit's player index (`(char)param_1[9] > 0`), so a caller
    // modelling a player-0 (neutral) unit should pass an all-false `has`.
    MovePointTable t = tables[move_types];
    // Note: the engine also clears move-type bit 0x80 when `AoWHSMap+0x120 != 0`
    // (that field is not written by any exported routine - it is set by the
    // scenario/AoW.exe, so the port leaves it at 0; see docs/15_MOVEMENT.md).
    // road cost 2: `(TPlayer+0x54) & 2` forces the *road* column (column 5, raw
    // overlay code 4) to 2 on every terrain row that is not impassable.
    if (traits.road_cost_2) {
        for (int terrain = 0; terrain < kTerrainCount; ++terrain) {
            const std::size_t idx = static_cast<std::size_t>(terrain) * 16 + 5;
            if (t[idx] != kImpassable) t[idx] = 2;
        }
    }
    // abilities 0x17 | 1 | 0xa5: the player/ability movement-cost trait. With
    // `!cost_flag_a && cost_flag_b` all positive costs double; with
    // `cost_flag_a && !cost_flag_b` all costs >2 become (v+1)>>1; the other two
    // combinations of the two flags do nothing.
    if (has[0x17] || has[1] || has[0xa5]) {
        if (!traits.cost_flag_a && traits.cost_flag_b) {
            for (std::uint8_t& v : t) {
                if (v != kImpassable && v > 0) v = static_cast<std::uint8_t>(v * 2);
            }
        } else if (traits.cost_flag_a && !traits.cost_flag_b) {
            for (std::uint8_t& v : t) {
                if (v != kImpassable && v > 2) {
                    v = static_cast<std::uint8_t>((v + 1) >> 1);
                }
            }
        }
    }
    // ability 0x98: every cost >2 drops by 1, floored at 2.
    if (has[0x98]) {
        for (std::uint8_t& v : t) {
            if (v == kImpassable) continue;
            int x = v;
            if (x > 2) {
                --x;
                if (x < 2) x = 2;
            }
            v = static_cast<std::uint8_t>(x);
        }
    }
    // ability 0x84: every positive cost gains 2.
    if (has[0x84]) {
        for (std::uint8_t& v : t) {
            if (v != kImpassable && v > 0) {
                v = static_cast<std::uint8_t>(v + 2);
            }
        }
    }
    return t;
}

void maximize_move_point_table(MovePointTable& dst, const MovePointTable& src) {
    // AoWE.MaximizeMovePointTable@0x55744d34:
    //   if (dst != 0xFF && (dst < src || src == 0xFF)) dst = src;
    for (std::size_t i = 0; i < dst.size(); ++i) {
        const std::uint8_t s = src[i];
        if (dst[i] != kImpassable && (dst[i] < s || s == kImpassable)) {
            dst[i] = s;
        }
    }
}

MovePointTable army_move_point_table(const std::vector<MovePointTable>& units) {
    // TArmy.CreateMovePointTable@0x5578dc40.
    MovePointTable out;
    out.fill(1);
    for (const MovePointTable& u : units) maximize_move_point_table(out, u);
    if (units.empty()) out.fill(kImpassable);
    return out;
}

ArmyMove army_move(const std::array<MovePointTable, 256>& tables,
                   const std::vector<std::uint8_t>& unit_move_types,
                   const std::vector<std::uint8_t>& unit_move_points) {
    return army_move(tables, unit_move_types, unit_move_points,
                     std::vector<std::array<bool, 256>>{}, PlayerMoveTraits{});
}

ArmyMove army_move(const std::array<MovePointTable, 256>& tables,
                   const std::vector<std::uint8_t>& unit_move_types,
                   const std::vector<std::uint8_t>& unit_move_points,
                   const std::vector<std::array<bool, 256>>& unit_abilities,
                   const PlayerMoveTraits& traits) {
    ArmyMove out;
    std::vector<MovePointTable> per_unit;
    per_unit.reserve(unit_move_types.size());
    for (std::size_t i = 0; i < unit_move_types.size(); ++i) {
        const std::uint8_t mt = unit_move_types[i];
        if (i < unit_abilities.size()) {
            per_unit.push_back(unit_move_point_table(tables, mt, unit_abilities[i],
                                                     traits));
        } else {
            per_unit.push_back(unit_move_point_table(tables, mt));
        }
        out.move_types |= mt;
    }
    out.table = army_move_point_table(per_unit);
    out.budget = army_move_points(unit_move_points);
    out.valid = !unit_move_types.empty();
    return out;
}

std::uint8_t army_move_points(const std::vector<std::uint8_t>& unit_move_points) {
    // TArmy.MovePoints@0x5578d8d0: 0 for an empty selection, else
    // min(0x32, smallest unit move points).
    if (unit_move_points.empty()) return 0;
    std::uint8_t v = kArmyMovePointsCap;
    for (std::uint8_t mp : unit_move_points) v = std::min(v, mp);
    return v;
}

std::uint8_t move_point_cost(const MovePointTable& table, int terrain,
                            int overlay) {
    if (terrain < 0 || terrain >= kTerrainCount) return kImpassable;
    // `overlay` is the *resource* TTerrainList code the field carries (-1 = none,
    // 0 = mountains, 1 = forest/dense vegetation, 2 = hill, 3 = scrub, 4 = road /
    // city, 5 = bridge, 6 = rubble, 8 = bones, 9 = vegetation), but the cost
    // table's column 0 is "no overlay" - so both engine lookups add one:
    //
    //   TAbstractUnit.MovePointCost@0x55780848 returns acStack_106[i], and
    //     acStack_106 is the 256-byte table CreateMovePointTable wrote one byte
    //     EARLIER (`local_107`), i.e. it reads table[i + 1];
    //   TMoveControl.DefaultMoveOnMovePointProc@0x557454d8 reads
    //     `*(char *)(mc + field->terrain*0x10 + 0x31 + field->overlay)` - the
    //     table starts at `mc + 0x30`, so a field overlay of -1 ("none") selects
    //     column 0 and a mountain (code 0) selects column 1.
    //
    // See docs/15_MOVEMENT.md §2 ("The overlay code is one below the table column").
    const int column = overlay + 1;
    if (column < 0 || column >= kOverlayCount) return kImpassable;
    return table[static_cast<std::size_t>(terrain) * 16 +
                 static_cast<std::size_t>(column)];
}

std::uint8_t clamp_move_points(int value) {
    if (value < 0) return 0;
    if (value > 0xFF) return 0xFF;
    return static_cast<std::uint8_t>(value);
}

// ---------------------------------------------------------------------------
// Per-unit abilities
// ---------------------------------------------------------------------------
std::array<bool, 256> decode_ability_bitmap(
    const std::vector<std::uint8_t>& mask, std::uint32_t count) {
    std::array<bool, 256> out{};
    for (std::uint32_t i = 0; i < count && i < 256; ++i) {
        const std::size_t byte = i >> 3;
        if (byte >= mask.size()) break;
        out[i] = ((mask[byte] >> (i & 7)) & 1) != 0;  // GetAbSet@0x5574e0e0
    }
    return out;
}

std::array<bool, 256> decode_ability_list(
    const std::vector<std::uint8_t>& payload) {
    // The payload is `[table][data]` (docs/01_FORMATS.md, "Object region"):
    //   u8 n; if n & 0x80: (n & 0x7f) compact entries + u32 wide-entry count;
    //   n * u16 compact entries (tag = low byte, offset = high byte);
    //   wide * (u32 tag, u32 offset);  data base follows.
    std::vector<std::uint8_t> mask;
    std::uint32_t count = 0;
    if (payload.empty()) return {};
    std::size_t p = 0;
    std::uint8_t n = payload[p++];
    std::uint32_t wide = 0;
    std::uint32_t compact = n;
    if ((n & 0x80) != 0) {
        compact = n & 0x7f;
        if (p + 4 > payload.size()) return {};
        wide = static_cast<std::uint32_t>(payload[p]) |
               (static_cast<std::uint32_t>(payload[p + 1]) << 8) |
               (static_cast<std::uint32_t>(payload[p + 2]) << 16) |
               (static_cast<std::uint32_t>(payload[p + 3]) << 24);
        p += 4;
    }
    if (p + static_cast<std::size_t>(compact) * 2 +
            static_cast<std::size_t>(wide) * 8 > payload.size()) {
        return {};
    }
    std::uint32_t off2 = 0xFFFFFFFF, off3 = 0xFFFFFFFF;
    for (std::uint32_t i = 0; i < compact; ++i) {
        const std::uint32_t tag = payload[p];
        const std::uint32_t off = payload[p + 1];
        p += 2;
        if (tag == 2) off2 = off;
        if (tag == 3) off3 = off;
    }
    for (std::uint32_t i = 0; i < wide; ++i) {
        const std::uint32_t tag = static_cast<std::uint32_t>(payload[p]) |
                                  (static_cast<std::uint32_t>(payload[p + 1]) << 8) |
                                  (static_cast<std::uint32_t>(payload[p + 2]) << 16) |
                                  (static_cast<std::uint32_t>(payload[p + 3]) << 24);
        const std::uint32_t off = static_cast<std::uint32_t>(payload[p + 4]) |
                                  (static_cast<std::uint32_t>(payload[p + 5]) << 8) |
                                  (static_cast<std::uint32_t>(payload[p + 6]) << 16) |
                                  (static_cast<std::uint32_t>(payload[p + 7]) << 24);
        p += 8;
        if (tag == 2) off2 = off;
        if (tag == 3) off3 = off;
    }
    const std::size_t base = p;  // the data area starts here
    if (off2 != 0xFFFFFFFF && base + off2 + 2 <= payload.size()) {
        count = static_cast<std::uint32_t>(payload[base + off2]) |
                (static_cast<std::uint32_t>(payload[base + off2 + 1]) << 8);
    }
    if (off3 != 0xFFFFFFFF) {
        const std::size_t bytes = (static_cast<std::size_t>(count) + 7) / 8;
        const std::size_t q = base + off3;
        if (bytes > 0 && q + bytes <= payload.size()) {
            mask.assign(payload.begin() + static_cast<std::ptrdiff_t>(q),
                        payload.begin() + static_cast<std::ptrdiff_t>(q + bytes));
        }
    }
    return decode_ability_bitmap(mask, count);
}

std::uint8_t unit_move_types(const MapUnit& unit) {
    return move_types_from_abilities(
        decode_ability_bitmap(unit.ability_mask, unit.ability_count));
}

std::uint8_t unit_move_points(const UnitCatalog& catalog, const MapUnit& unit) {
    if (unit.is_hero) {
        const std::uint32_t hi =
            unit.resource < 0 ? 0u : static_cast<std::uint32_t>(unit.resource);
        const auto it = catalog.hero_moves.find(hi);
        if (it != catalog.hero_moves.end()) return it->second;
        return 0;
    }
    for (const UnitEntry& e : catalog.units) {
        if (static_cast<std::int32_t>(e.id) == unit.resource) return e.moves;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Hex geometry
// ---------------------------------------------------------------------------
int hex_direction(int x0, int y0, int x1, int y1) {
    // HSEngine.dHXtoHNfast@5560e338, verbatim.
    if (x1 == x0) {
        if (y1 < y0) return 1;
        if (y0 < y1) return 4;
        return 0;
    }
    if ((x0 & 1) == 0) {
        if (x1 < x0) return (y1 < y0) ? 6 : 5;
        return (y1 < y0) ? 2 : 3;
    }
    if (x1 < x0) return (y0 < y1) ? 5 : 6;
    return (y0 < y1) ? 3 : 2;
}

int hex_distance(int x0, int y0, int x1, int y1) {
    // HSEngine.dHXtoRad@5560dcf4: the axis-aligned column-staggered distance.
    int dx = x1 - x0;
    if (dx < 0) dx = -dx;
    int dy = y1 - y0;
    if (dy < 0) dy = -dy;
    if (dy < dx) {
        const int half = (dx + 1) >> 1;
        if ((dx & 1) != 0 &&
            (((x0 & 1) == 0 && y0 < y1) || ((x1 & 1) == 0 && y1 < y0))) {
            ++dy;
        }
        if (half < dy) dx += dy - half;
    } else {
        // dHXtoRad: `bVar1` is set when dx is odd and the row pair is NOT
        // already aligned by the column stagger, i.e. an odd column steps down.
        const bool odd = (dx & 1) != 0 &&
                         (((x0 & 1) == 0 && y1 > y0) ||
                          ((x1 & 1) == 0 && y0 > y1));
        dx = odd ? ((dx + 1) >> 1) + dy : (dx >> 1) + dy;
    }
    return dx;
}

bool hex_neighbour(int x, int y, int dir, int& nx, int& ny) {
    if (dir < 1 || dir > 6) return false;
    const int p = x & 1;
    nx = x + kNeighbourDx[p][dir - 1];
    ny = y + kNeighbourDy[p][dir - 1];
    return true;
}

void hn_walk_hex(int x, int y, int index, int& hx, int& hy) {
    // HSEngine.InitHNWalk@5560e5c8 + HSEngine.WalkToNextHN@5560e610, started with
    // radius 0. The walk state is (dir, step, radius, x, y); the six ring-edge
    // steps come from DAT_5562e250 (HSEPack file 0x02d050), indexed
    // `[edge][x parity]` with 8 bytes per entry (dx, dy) - the column stagger is
    // baked into the odd-parity row:
    //
    //   parity 0 (even x): (1,0) (0,1) (-1,0) (-1,-1) (0,-1) (1,-1)
    //   parity 1 (odd x):  (1,1) (0,1) (-1,1) (-1,0)  (0,-1) (1,0)
    //
    // InitHNWalk(x, y, 0) puts the walk at the object's own hex with dir 5, so the
    // first WalkToNextHN opens ring 1 at (x, y-1) (N); each later step advances
    // along the current edge until it has taken `radius` of them, then turns. The
    // result is the spiral `(0,0), (0,-1), (1,-1), (1,0), (0,1), (-1,0), (-1,-1),
    // (0,-2), ...`, which agrees with HSEngine.CenterHNtoHX@5560e17c - the mapping
    // Mountain.TMountainMO.CanPlace@556a2e54 uses for the same pairing.
    static const int kEdgeDx[2][6] = {{1, 0, -1, -1, 0, 1}, {1, 0, -1, -1, 0, 1}};
    static const int kEdgeDy[2][6] = {{0, 1, 0, -1, -1, -1}, {1, 1, 1, 0, -1, 0}};
    int dir = 5, step = 0, radius = 0, px = x, py = y;
    for (int i = 1; i <= index; ++i) {
        const int p = px & 1;
        ++step;
        if (step < radius) {  // the edge still has steps left
            px += kEdgeDx[p][dir];
            py += kEdgeDy[p][dir];
            continue;
        }
        if (dir == 5) {  // the sixth edge ended: open the next ring at N
            ++radius;
            dir = 0;
            step = 0;
            px = x;
            py = y - radius;
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

// ---------------------------------------------------------------------------
// Terrain resolution
// ---------------------------------------------------------------------------
std::size_t fill_tile_terrain(Map& map, std::size_t level,
                              const std::vector<MapResource>& resources) {
    if (level >= map.levels.size()) return 0;
    MapLevel& lv = map.levels[level];
    std::size_t filled = 0;
    for (MapTile& tile : lv.tiles) {
        tile.terrain = -1;
        tile.overlay = -1;  // -1 = "no overlay" (plains), not 0 (= mountains!)
        // The engine asks the field's sprites from the TOP of the stack down
        // (`TMapField.UpdateTerrainType@55607df4` / `UpdateOverlayType@55607e48`
        // walk the sprite array in reverse, stopping at the first non-negative
        // answer). The terrain and the overlay are answered *independently*, but
        // with one crucial difference: `TMapObject.GetTerrain@55609780` returns
        // "no opinion" (-1) whenever the object's flag byte (`+0xc`) has bit 4
        // set - i.e. a *decoration* object never defines the field's terrain.
        //
        // That gate matters: e.g. `Crops.bmp` / `PAD1.ILB` / `STR_FARM.ILB`
        // carry `terrain 0` (= water!) in their tag-7 (`TT=ff00`) even though
        // they sit on grassland (`H_gr.BMP`, `TT=fe01`). Without the gate the
        // crops would override the grass hexagon and turn whole swathes of land
        // into impassable water. The port cannot read the runtime `+0xc` flag,
        // so it reproduces the engine's result directly: the field's **terrain**
        // comes from the base hexagon (tag 50) only; every object sprite
        // (tag >= 51) contributes only its **overlay** (forest/hill/mountain/
        // road/city/...). A tile with no tag-50 hexagon falls back to the
        // topmost object that does carry a terrain.
        //
        // The stored bytes are the RAW TTerrainList codes (see
        // aow::move_point_cost): terrain 0 water, 1 grass, ...; overlay -1 =
        // none, 0 = mountains, 1 = forest, 2 = hill, 4 = road/city, 5 = bridge.
        // `TMapObject.GetTerrain/GetOverlay` treat the byte as a *signed char*,
        // so 0x80..0xFF means "no opinion".
        auto opinion_terrain = [&](const MapResource* r) {
            return r != nullptr && r->has_terrain && r->terrain >= 0 &&
                   r->terrain < 0x80;
        };
        for (std::size_t i = tile.objects.size(); i-- > 0;) {
            const MapHs& hs = tile.objects[i];
            if (hs.tag != 50) continue;
            const MapResource* r = find_map_resource(resources, hs.resource);
            if (opinion_terrain(r)) { tile.terrain = r->terrain; break; }
        }
        if (tile.terrain < 0) {  // no base hexagon answered - fall back
            for (std::size_t i = tile.objects.size(); i-- > 0;) {
                const MapHs& hs = tile.objects[i];
                const MapResource* r = find_map_resource(resources, hs.resource);
                if (opinion_terrain(r)) { tile.terrain = r->terrain; break; }
            }
        }
        for (std::size_t i = tile.objects.size(); i-- > 0;) {
            const MapHs& hs = tile.objects[i];
            const MapResource* r = find_map_resource(resources, hs.resource);
            if (r == nullptr || !r->has_terrain) continue;
            if (tile.overlay < 0 && r->overlay >= 0) {
                // A `Mountain.TMountainMO` answers **0 (mountains)** for its own
                // hex too - `Mountain.TMountainMO.GetOverlay@557a2dc8` ignores the
                // index (and its resource may declare a hill for entry 0). See the
                // second pass for the override's body.
                tile.overlay = hs.class_id == kMountainMoClassId ? 0 : r->overlay;
                break;
            }
        }
        if (tile.terrain >= 0) ++filled;
    }

    // Second pass: the hexes a *multi-hex* object covers. Such an object (a
    // mountain, a big city) is stored on a single hex but its `TTerrainList`
    // carries one entry per covered hex, and `TMultiHexMO.OverrideTerrain
    // @5560b2d4` writes entry i onto covered hex i (the walk order of
    // `hn_walk_hex`, i.e. HSEngine.InitHNWalk/WalkToNextHN). Without this pass
    // only the hex the object stands on gets an opinion, so a mountain's other
    // peaks - the hexes its art actually covers - stayed plain grassland and were
    // walkable by any unit.
    //
    // A covered hex is stamped with the object's own opinion for it - except for
    // a `Mountain.TMountainMO`, which forces *mountains* (see below). The
    // *terrain* byte is deliberately not stamped: several of these lists carry a
    // garbage terrain (a city resource's entry is `0 | 4 << 8` = water | city),
    // and the port's rule is that the terrain comes from the tag-50 hexagon only
    // (see the note above).
    const std::uint32_t w = lv.width;
    const int hgt = static_cast<int>(lv.height);
    const int wid = static_cast<int>(w);
    for (std::size_t ti = 0; ti < lv.tiles.size(); ++ti) {
        const MapTile& src = lv.tiles[ti];
        if (src.objects.empty()) continue;
        const int x = static_cast<int>(ti % w);
        const int y = static_cast<int>(ti / w);
        for (const MapHs& hs : src.objects) {
            const MapResource* r = find_map_resource(resources, hs.resource);
            if (r == nullptr || r->terrain_list.size() < 2) continue;
            // A `Mountain.TMountainMO` (class 0x20168) does NOT answer with the
            // entry of the hex it is asked about: it *overrides*
            // `TMapObject.GetOverlay` - `Mountain.TMountainMO.GetOverlay
            // @557a2dc8` is `list := res->terrain_list; v := GetOverlay(list);
            // if (v < 0) return v; return 0;` - and so answers **0 (mountains)**
            // for *every* covered hex as soon as its own entry 0 carries an
            // overlay opinion; the covered-hex index is ignored. A mountain is
            // therefore a wall over its whole footprint, not only over the
            // entries that declare 0 themselves (its slopes ship forest/hill).
            //
            // The hexes it covers are the ones `TMultiHexMO.UpdateMapFields
            // @5560af6c` registers it on: that walk stops at the entry count and
            // registers (then re-runs the field's `UpdateOverlayType`) only when
            // the entry has an opinion - terrain OR overlay - so an entry that
            // ships 0xFEFE/0xFFFF covers nothing. `Cave.TCave.GetOverlay
            // @557b3878` and `Sign.TSign.GetOverlay@557a44e8` override the
            // accessor to "-1 = no opinion" instead, and `Terrain.TTerrainMO`
            // (trees, rocks, the hill mounds) does not override it at all - which
            // is why a hill mound keeps its own answer (a hill) even under a
            // mountain, exactly as the original game plays.
            const bool mountain_mo = hs.class_id == kMountainMoClassId;
            for (std::size_t k = 0; k < r->terrain_list.size(); ++k) {
                const std::uint16_t e = r->terrain_list[k];
                const int et = static_cast<std::int8_t>(e & 0xff);
                const int eo = static_cast<std::int8_t>((e >> 8) & 0xff);
                if (et < 0 && eo < 0) continue;  // no opinion: hex not covered
                const int claim = (mountain_mo && r->overlay >= 0) ? 0 : eo;
                if (claim < 0) continue;  // 0x80..0xFF = "no opinion" (signed)
                int hx = 0, hy = 0;
                hn_walk_hex(x, y, static_cast<int>(k), hx, hy);
                if (hx < 0 || hy < 0 || hx >= wid || hy >= hgt) continue;
                MapTile& dst = lv.tiles[static_cast<std::size_t>(hy) * w +
                                        static_cast<std::size_t>(hx)];
                // `UpdateOverlayType@55607e48` resolves a field's overlay from its
                // sprites top-down, by *show priority*, and a multi-hex mountain
                // outranks the tree or rock it stands over - so its opinion
                // replaces a natural feature (0 mountains, 1 forest, 2 hill,
                // 3 scrub, 6 rubble, 8 bones, 9 vegetation). Two objects can claim
                // one hex: `Mountain.TMountainMO.ValidMountainOverlap@556a2de0`
                // accepts an overlap only when their opinions differ, and the
                // engine then keeps the topmost sprite's. The port cannot compare
                // show priorities, so it lets the *impassable* claim win - a hex a
                // mountain counts as its own stays a mountain however the objects
                // are visited - and applies a natural feature only to a hex nothing
                // has answered for yet. A road/city (4) or a bridge (5) is a
                // man-made crossing that outranks the terrain (a mountain pass
                // stays open), so an overlay 4/5 is never replaced.
                if (claim == 0) {
                    if (dst.overlay != 4 && dst.overlay != 5) dst.overlay = 0;
                } else if (dst.overlay < 0) {
                    dst.overlay = claim;
                }
            }
        }
    }
    return filled;
}

// ---------------------------------------------------------------------------
// The path
// ---------------------------------------------------------------------------
MovePath find_move_path(int width, int height, int source_x, int source_y,
                        int dest_x, int dest_y, int bound,
                        const TileCostFn& cost) {
    MovePath path;
    if (width <= 0 || height <= 0) return path;
    if (source_x < 0 || source_y < 0 || source_x >= width || source_y >= height) {
        return path;
    }
    const std::size_t n =
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    const int src = source_y * width + source_x;
    const int kBig = kUnreachableMovePoints;
    std::vector<int> dist(n, kBig);
    dist[static_cast<std::size_t>(src)] = 0;

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    pq.push(Node{0, src});
    while (!pq.empty()) {
        const Node cur = pq.top();
        pq.pop();
        if (cur.cost > dist[static_cast<std::size_t>(cur.index)]) continue;
        const int cx = cur.index % width;
        const int cy = cur.index / width;
        for (int dir = 1; dir <= 6; ++dir) {
            int nx = 0, ny = 0;
            if (!hex_neighbour(cx, cy, dir, nx, ny)) continue;
            if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;
            const std::uint8_t step = cost(nx, ny);
            if (step == kImpassable) continue;
            const int nd = cur.cost + step;
            if (nd > bound) continue;  // the TMoveControl+0x2c bound
            const int ni = ny * width + nx;
            if (nd < dist[static_cast<std::size_t>(ni)]) {
                dist[static_cast<std::size_t>(ni)] = nd;
                pq.push(Node{nd, ni});
            }
        }
    }

    if (dest_x < 0 || dest_y < 0 || dest_x >= width || dest_y >= height) {
        return path;
    }
    const int dst = dest_y * width + dest_x;
    if (dist[static_cast<std::size_t>(dst)] == kBig) {
        return path;  // CalculatePathEx clears the list
    }
    // Walk the gradient back from the destination, exactly as
    // TMoveControl.CalculatePathEx@0x557460b0 does: at each hex pick the FIRST
    // neighbour (directions 1..6 in order) whose accumulated move points equal
    // `current - step_cost`, where `step_cost` is this hex's own MovePointCost
    // (the cost to enter it). If no neighbour answers, the engine clears the list
    // (bVar2) - so this returns an empty path. The result is source-first.
    std::vector<int> rev;  // destination-first while walking back
    int cur = dst;
    for (int guard = 0; guard <= static_cast<int>(n); ++guard) {
        if (guard == static_cast<int>(n)) return path;  // safety: no cycle possible
        rev.push_back(cur);
        if (cur == src) break;
        const int cx = cur % width;
        const int cy = cur / width;
        const int step = cost(cx, cy);  // cost to ENTER this hex
        const int target = dist[static_cast<std::size_t>(cur)] - step;
        int next = -1;
        for (int dir = 1; dir <= 6 && next < 0; ++dir) {
            int nx = 0, ny = 0;
            if (!hex_neighbour(cx, cy, dir, nx, ny)) continue;
            if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;
            const int ni = ny * width + nx;
            if (dist[static_cast<std::size_t>(ni)] == target) next = ni;
        }
        if (next < 0) return path;  // no gradient step -> CalculatePathEx clears
        cur = next;
    }
    std::reverse(rev.begin(), rev.end());
    path.steps.reserve(rev.size());
    for (int i : rev) {
        MoveStep s;
        s.x = i % width;
        s.y = i / width;
        s.cost = dist[static_cast<std::size_t>(i)];
        path.steps.push_back(s);
    }
    path.total_cost = dist[static_cast<std::size_t>(dst)];
    path.reached = true;
    return path;
}

MovePath find_move_path(const MovePointTable& table, const MapLevel& level,
                        int source_x, int source_y, int dest_x, int dest_y,
                        int bound) {
    const int w = static_cast<int>(level.width);
    auto cost = [&](int x, int y) -> std::uint8_t {
        const std::size_t i = static_cast<std::size_t>(y) *
                                  static_cast<std::size_t>(w) +
                              static_cast<std::size_t>(x);
        if (i >= level.tiles.size()) return kImpassable;
        const MapTile& t = level.tiles[i];
        return move_point_cost(table, t.terrain, t.overlay);
    };
    return find_move_path(w, static_cast<int>(level.height), source_x, source_y,
                          dest_x, dest_y, bound, cost);
}

// ---------------------------------------------------------------------------
// Overlay frames
// ---------------------------------------------------------------------------
int path_arrow_dir(const MovePath& path, std::size_t i) {
    if (i + 1 >= path.steps.size()) return 0;
    const MoveStep& a = path.steps[i];
    const MoveStep& b = path.steps[i + 1];
    return hex_direction(b.x, b.y, a.x, a.y);  // the reverse step
}

int path_arrow_frame(int dir, bool within_budget) {
    return (within_budget ? kArrowFrameBrightAnchor : kArrowFrameDimAnchor) + dir;
}

bool path_arrow_bright(const MovePath& path, std::size_t i, int budget) {
    if (i >= path.steps.size()) return false;
    // ShowMovePaths' running sum makes the arrow bright while the accumulated
    // cost from the source to steps[i] is within this turn's move points.
    return budget >= path.steps[i].cost;
}

int path_target_frame(bool within_budget) {
    return within_budget ? kTargetFrameBright : kTargetFrameDim;
}

int path_turns(int total_cost, int budget, int per_turn) {
    // ShowMovePaths: ((total - budget) - 1) / per_turn + 1, only when the path
    // does not fit this turn.
    if (per_turn <= 0) return 1;
    if (budget >= total_cost) return 0;
    return ((total_cost - budget) - 1) / per_turn + 1;
}

int path_budget_stop_index(const MovePath& path, int budget) {
    // AoWE.TMovepath.MovePointsToPos@0x557638d8: the hex the army reaches after
    // spending `budget` move points. Source-first, that is the last step whose
    // cumulative cost still fits; -1 when the whole path fits (or is too short).
    if (path.steps.size() < 2) return -1;
    if (budget >= path.total_cost) return -1;
    int stop = -1;
    for (std::size_t i = 1; i < path.steps.size(); ++i) {
        if (path.steps[i].cost > budget) break;
        stop = static_cast<int>(i);
    }
    return stop;
}

// ---------------------------------------------------------------------------
// Naming
// ---------------------------------------------------------------------------
const char* move_type_name(std::uint8_t move_type) {
    switch (move_type) {
        case kMoveWalk: return "walk";
        case kMoveWater: return "water";
        case kMoveFly: return "fly";
        case kMoveForest: return "forest";
        case kMoveCave: return "cave";
        case kMoveMountain: return "mountain";
        case kMoveBit6: return "bit6";
        case kMoveBit7: return "bit7";
        default: return "?";
    }
}

std::string move_types_string(std::uint8_t move_types) {
    std::string out;
    for (int k = 0; k < 8; ++k) {
        const std::uint8_t bit = kMovePointSourceTableBits[k];
        if ((move_types & bit) == 0) continue;
        if (!out.empty()) out += '|';
        out += move_type_name(bit);
    }
    return out.empty() ? std::string("none") : out;
}

}  // namespace aow
