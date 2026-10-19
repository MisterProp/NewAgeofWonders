// eobject.hpp - the Engine "EObject" property table (shared by the map and
// resource loaders).
//
// AoW1 serialises its object graphs (`.PFS`, `.HSM`/`.CSM`/`.HSS`) as a byte
// `n` (bit 0x80 => a u32 large-count follows), then `n & 0x7f` compact entries
// (u16: tag = low byte, offset = high byte), then `large_count` wide entries
// (u32 tag, u32 offset). Each entry maps a property tag to an offset relative
// to the table's data base (the byte just past the table). See
// docs/01_FORMATS.md and src/engine/pfs.cpp for the original walk.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "aow/endian.hpp"

namespace aow {

using EObjectEntry = std::pair<std::uint32_t, std::uint32_t>;  // (tag, offset)

struct EObjectTable {
    std::vector<EObjectEntry> entries;
    std::size_t base = 0;  // position just past the table
};

// Parses a property table at `pos` in [R, R+n). False if out of bounds.
inline bool read_eobject_table(const std::uint8_t* R, std::size_t n,
                               std::size_t pos, EObjectTable& out) {
    if (pos >= n) return false;
    const std::uint8_t b = R[pos];
    std::size_t p = pos + 1;
    std::uint64_t small = 0, large = 0;
    if ((b & 0x80) != 0) {
        small = b & 0x7F;
        if (p + 4 > n) return false;
        large = read_u32(R + p);
        p += 4;
    } else {
        small = b;
    }
    if (small + large > 100000) return false;
    if (p + small * 2 + large * 8 > n) return false;
    out.entries.clear();
    out.entries.reserve(static_cast<std::size_t>(small + large));
    for (std::uint64_t i = 0; i < small; ++i) {
        const std::uint16_t w = read_u16(R + p);
        p += 2;
        out.entries.emplace_back(w & 0xFFu, w >> 8);
    }
    for (std::uint64_t i = 0; i < large; ++i) {
        out.entries.emplace_back(read_u32(R + p), read_u32(R + p + 4));
        p += 8;
    }
    out.base = p;
    return true;
}

// Offset for `tag`, or npos.
inline std::size_t eobject_find(const EObjectTable& t, std::uint32_t tag) {
    for (const EObjectEntry& e : t.entries) {
        if (e.first == tag) return e.second;
    }
    return static_cast<std::size_t>(-1);
}

// Reads a rwString (u8 length; 0xFF => u32 length) at `off` in [R,R+n).
inline std::string eobject_short_string(const std::uint8_t* R, std::size_t n,
                                        std::size_t off) {
    if (off >= n) return {};
    std::size_t p = off;
    std::size_t len = R[p++];
    if (len == 0xFF) {
        if (p + 4 > n) return {};
        len = read_u32(R + p);
        p += 4;
    }
    if (len > n - p) len = n - p;
    return std::string(reinterpret_cast<const char*>(R + p), len);
}

}  // namespace aow
