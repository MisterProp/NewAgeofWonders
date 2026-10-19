// png.hpp - minimal, dependency-free PNG writer for the AoW1 port.
//
// The tools and the engine are stdlib-only by project policy, so this writer
// emits a spec-valid PNG using *stored* (uncompressed) DEFLATE blocks plus the
// required CRC-32 / Adler-32 checksums. Files are larger than a real encoder's
// but correct and viewable everywhere.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace aow {

// Writes an 8-bit RGBA (4 bytes per pixel, row-major, no padding) PNG.
bool write_png_rgba(const std::string& path, std::uint32_t width,
                    std::uint32_t height, const std::vector<std::uint8_t>& rgba,
                    std::string& error);

}  // namespace aow
