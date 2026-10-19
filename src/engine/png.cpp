// png.cpp - minimal PNG writer (stored DEFLATE, no external dependencies).
#include "aow/png.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>

namespace aow {
namespace {

// Standard CRC-32 (IEEE 802.3), as used by PNG chunks.
std::uint32_t crc32(const std::uint8_t* data, std::size_t len,
                    std::uint32_t crc = 0xFFFFFFFFu) {
    static std::uint32_t table[256];
    static bool init = false;
    if (!init) {
        for (std::uint32_t n = 0; n < 256; ++n) {
            std::uint32_t c = n;
            for (int k = 0; k < 8; ++k) {
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            }
            table[n] = c;
        }
        init = true;
    }
    for (std::size_t i = 0; i < len; ++i) {
        crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}

// Adler-32, required at the end of the zlib stream.
std::uint32_t adler32(const std::uint8_t* data, std::size_t len) {
    std::uint32_t a = 1, b = 0;
    for (std::size_t i = 0; i < len; ++i) {
        a = (a + data[i]) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

void put_u32(std::vector<std::uint8_t>& out, std::uint32_t v) {
    out.push_back(static_cast<std::uint8_t>(v >> 24));
    out.push_back(static_cast<std::uint8_t>(v >> 16));
    out.push_back(static_cast<std::uint8_t>(v >> 8));
    out.push_back(static_cast<std::uint8_t>(v));
}

void write_chunk(std::vector<std::uint8_t>& out, const char type[4],
                 const std::vector<std::uint8_t>& data) {
    put_u32(out, static_cast<std::uint32_t>(data.size()));
    const std::size_t crc_start = out.size();
    out.insert(out.end(), type, type + 4);
    out.insert(out.end(), data.begin(), data.end());
    const std::uint32_t crc =
        crc32(out.data() + crc_start, out.size() - crc_start) ^ 0xFFFFFFFFu;
    put_u32(out, crc);
}

}  // namespace

bool write_png_rgba(const std::string& path, std::uint32_t width,
                    std::uint32_t height,
                    const std::vector<std::uint8_t>& rgba, std::string& error) {
    if (rgba.size() != static_cast<std::size_t>(width) * height * 4) {
        error = "rgba buffer size does not match width*height*4";
        return false;
    }

    // Raw image: each scanline is prefixed with a filter byte (0 = none).
    std::vector<std::uint8_t> raw;
    raw.reserve((static_cast<std::size_t>(width) * 4 + 1) * height);
    for (std::uint32_t y = 0; y < height; ++y) {
        raw.push_back(0);
        const std::uint8_t* row =
            rgba.data() + static_cast<std::size_t>(y) * width * 4;
        raw.insert(raw.end(), row, row + static_cast<std::size_t>(width) * 4);
    }

    // zlib stream: 2-byte header, stored DEFLATE blocks, Adler-32.
    std::vector<std::uint8_t> z;
    z.push_back(0x78);  // CMF: deflate, 32K window
    z.push_back(0x01);  // FLG: no dict, fastest; (0x7801) % 31 == 0
    std::size_t off = 0;
    while (off < raw.size()) {
        const std::size_t chunk = std::min<std::size_t>(65535, raw.size() - off);
        const bool last = (off + chunk) == raw.size();
        z.push_back(last ? 0x01 : 0x00);  // BFINAL + BTYPE=00 (stored)
        z.push_back(static_cast<std::uint8_t>(chunk & 0xFF));
        z.push_back(static_cast<std::uint8_t>(chunk >> 8));
        z.push_back(static_cast<std::uint8_t>(~chunk & 0xFF));
        z.push_back(static_cast<std::uint8_t>((~chunk >> 8) & 0xFF));
        z.insert(z.end(), raw.begin() + off, raw.begin() + off + chunk);
        off += chunk;
    }
    put_u32(z, adler32(raw.data(), raw.size()));

    std::vector<std::uint8_t> png = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    std::vector<std::uint8_t> ihdr;
    put_u32(ihdr, width);
    put_u32(ihdr, height);
    ihdr.push_back(8);  // bit depth
    ihdr.push_back(6);  // colour type: truecolour + alpha (RGBA)
    ihdr.push_back(0);  // compression
    ihdr.push_back(0);  // filter
    ihdr.push_back(0);  // interlace
    write_chunk(png, "IHDR", ihdr);
    write_chunk(png, "IDAT", z);
    write_chunk(png, "IEND", {});

    std::ofstream out(path, std::ios::binary);
    if (!out) {
        error = "cannot open " + path + " for writing";
        return false;
    }
    out.write(reinterpret_cast<const char*>(png.data()),
              static_cast<std::streamsize>(png.size()));
    if (!out) {
        error = "write failed for " + path;
        return false;
    }
    return true;
}

}  // namespace aow
