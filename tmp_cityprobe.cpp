// tmp_cityprobe.cpp - throwaway probe: dump every City.TCity of a map with its
// parsed fields (race / wall / level / flags / owner) plus the raw property
// table (tag / offset / first bytes) so the race parse can be checked by hand.
#include <cstdio>
#include <string>

#include "aow/eobject.hpp"
#include "aow/map_types.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: tmp_cityprobe <map> [level]\n");
        return 1;
    }
    const int only = (argc > 2) ? std::atoi(argv[2]) : -1;
    aow::Map m;
    std::string err;
    if (!aow::load_map(argv[1], m, err)) {
        std::printf("load error: %s\n", err.c_str());
        return 2;
    }
    std::printf("%s: %zu level(s), %ux%u hexes\n", argv[1], m.levels.size(),
                m.width, m.height);
    for (const aow::MapLevel& lv : m.levels) {
        if (only >= 0 && static_cast<int>(lv.index) != only) continue;
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                for (const aow::MapHs& hs :
                     lv.tiles[static_cast<std::size_t>(y) * lv.width + x].objects) {
                    if (hs.class_id != aow::kCityClassId) continue;
                    std::printf("\ncity L%u (%u,%u) res=%u race=%d wall=%d "
                                "clvl=%u cflags=0x%02x owner=%u payload=%zuB\n",
                                lv.index, x, y, hs.resource,
                                static_cast<int>(hs.race),
                                static_cast<int>(hs.wall_type), hs.city_level,
                                hs.city_flags, hs.structure_owner,
                                hs.payload.size());
                    const std::uint8_t* P = hs.payload.data();
                    const std::size_t N = hs.payload.size();
                    aow::EObjectTable t;
                    if (!aow::read_eobject_table(P, N, 4, t)) {
                        std::printf("  (no property table)\n");
                        continue;
                    }
                    std::printf("  class=0x%08x n=%zu\n",
                                aow::read_u32(P), t.entries.size());
                    for (const aow::EObjectEntry& e : t.entries) {
                        const std::size_t o = t.base + e.second;
                        std::printf("  tag=%-3u off=%-4zu", e.first, o);
                        if (o + 8 <= N) {
                            std::printf("  bytes=%02x %02x %02x %02x %02x %02x "
                                        "%02x %02x  u8=%u i32=%d",
                                        P[o], P[o + 1], P[o + 2], P[o + 3],
                                        P[o + 4], P[o + 5], P[o + 6], P[o + 7],
                                        P[o], aow::read_i32(P + o));
                        } else if (o < N) {
                            std::printf("  bytes=");
                            for (std::size_t k = o; k < N; ++k) {
                                std::printf("%02x ", P[k]);
                            }
                            std::printf(" u8=%u", P[o]);
                        } else {
                            std::printf("  (out of range)");
                        }
                        std::printf("\n");
                    }
                }
            }
        }
    }
    return 0;
}
