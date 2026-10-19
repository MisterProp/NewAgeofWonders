// inflate.hpp - dependency-free zlib (RFC 1950/1951) decompressor.
//
// Scenario/campaign maps (`.HSM` / `.CSM`) are stored as `'CFS\0'` + u8
// version + a zlib stream (see docs/01_FORMATS.md, "HSM / CSM / HSS").
// The port is stdlib-only, so we implement DEFLATE by hand, mirroring the
// "dependency-free" policy of png.cpp (which only needs the *stored* path).
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace aow {

// Inflates a zlib stream: a 2-byte zlib header, one or more DEFLATE blocks,
// then a 4-byte Adler-32 checksum (the checksum is not verified).
// Returns false on malformed input.
bool zlib_inflate(const std::uint8_t* in, std::size_t n,
                  std::vector<std::uint8_t>& out);

// Convenience: true when [in,n) starts with a zlib header (CMF/FLG valid).
bool looks_like_zlib(const std::uint8_t* in, std::size_t n);

}  // namespace aow
