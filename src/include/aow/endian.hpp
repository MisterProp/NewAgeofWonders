// endian.hpp - little-endian binary readers for the AoW1 port.
//
// AoW1 is a 32-bit little-endian (i386) game. All on-disk integers and floats
// are little-endian regardless of the host, so we read them explicitly.
#pragma once

#include <cstdint>
#include <cstring>
#include <string>

namespace aow {

inline std::uint32_t read_u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) |
           (static_cast<std::uint32_t>(p[3]) << 24);
}

inline std::uint16_t read_u16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0]) |
           static_cast<std::uint16_t>(p[1] << 8);
}

inline float read_f32(const std::uint8_t* p) {
    std::uint32_t bits = read_u32(p);
    float f;
    std::memcpy(&f, &bits, sizeof(f));
    return f;
}

inline std::int32_t read_i32(const std::uint8_t* p) {
    return static_cast<std::int32_t>(read_u32(p));
}

// Reads a NUL-terminated string starting at offset, within [0, size).
inline std::string read_cstr(const std::uint8_t* base, std::size_t size,
                             std::size_t off) {
    std::string s;
    while (off < size && base[off] != 0) {
        s.push_back(static_cast<char>(base[off]));
        ++off;
    }
    return s;
}

}  // namespace aow
