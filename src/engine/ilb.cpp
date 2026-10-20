// ilb.cpp - implementation of the .ILB image-library reader.
#include "aow/endian.hpp"
#include "aow/ilb.hpp"

#include <fstream>
#include <iterator>

namespace aow {

namespace {
constexpr std::uint8_t kMagic[4] = {0x04, 'I', 'L', 'B'};
constexpr std::uint32_t kPaletteBytes = 4 + 1024;  // u32 header + 256 RGBA
constexpr std::uint32_t kRecordEnd = 0xFFFFFFFFu;

// TGFXEPalette entry byte order, taken from the u32 in front of each 1024-byte
// palette. GFXE.TGFXEPalette.Remap (Gfxepack.dpl 0x5500ba34) switches on it and
// converts the entry to RGB565 as follows (555 branch shown):
//   0x88801B18: (b0&0xf8)<<7 | (b1&0xf8)<<2 | (b2>>3)   -> bytes are (R,G,B)
//   0x88809318: (b2&0xf8)<<7 | (b1&0xf8)<<2 | (b0>>3)   -> bytes are (B,G,R)
// Every palette shipped in this install carries 0x88801B18 (see docs/01_FORMATS).
constexpr std::uint32_t kPalFormatRgb = 0x88801B18u;  // entry bytes = (R,G,B)
constexpr std::uint32_t kPalFormatBgr = 0x88809318u;  // entry bytes = (B,G,R)
// TImageNode.ClassID() = 0x100 (Ilpack.dpl 0xd298). A node wraps the image
// record(s) of one entry and carries the entry's index in its own header.
constexpr std::uint32_t kNodeClass = 0x100;

bool is_printable_ascii(const std::uint8_t* p, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        if (p[i] < 32 || p[i] >= 127) return false;
    }
    return true;
}

// A record begins with: u32 index, u32 class_id, u8 fmt, u32 name_len, name.
// class_id values seen in the retail assets: 0x02, 0x10, 0x11, 0x12, 0x16.
struct RecordSig {
    std::uint32_t index = 0;
    std::uint32_t class_id = 0;
    std::uint8_t fmt = 0;
    std::uint32_t name_len = 0;
};

bool read_record_sig(const std::uint8_t* d, std::size_t size, std::size_t off,
                     RecordSig& out) {
    if (off + 17 > size) return false;
    const std::uint32_t class_id = read_u32(d + off + 4);
    if (class_id < 1 || class_id > 64) return false;
    const std::uint8_t fmt = d[off + 8];
    if (fmt > 3) return false;
    const std::uint32_t name_len = read_u32(d + off + 9);
    if (name_len < 1 || name_len > 300) return false;
    if (off + 13 + name_len > size) return false;
    if (!is_printable_ascii(d + off + 13, name_len)) return false;
    out.index = read_u32(d + off);
    out.class_id = class_id;
    out.fmt = fmt;
    out.name_len = name_len;
    return true;
}
}  // namespace

bool IlbFile::load(const std::string& path, std::string& error) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "cannot open " + path;
        return false;
    }
    std::vector<std::uint8_t> d((std::istreambuf_iterator<char>(in)),
                                std::istreambuf_iterator<char>());
    return load_memory(d.data(), d.size(), error);
}

bool IlbFile::load_memory(const std::uint8_t* data, std::size_t size,
                          std::string& error) {
    std::vector<std::uint8_t> d(data, data + size);
    if (d.size() < 0x18) {
        error = "file too small to be an ILB";
        return false;
    }
    for (int i = 0; i < 4; ++i) {
        if (d[i] != kMagic[i]) {
            error = "bad ILB magic";
            return false;
        }
    }

    header_ = IlbHeader{};
    header_.flags = read_u32(&d[0x04]);
    header_.version = read_f32(&d[0x08]);
    header_.header_size = read_u32(&d[0x0C]);
    header_.data_offset = read_u32(&d[0x10]);
    header_.total_size = read_u32(&d[0x14]);

    const std::size_t hdr = header_.header_size ? header_.header_size : 0x18;
    const std::size_t dir_end = header_.data_offset ? header_.data_offset
                                                    : d.size();
    if (hdr > d.size() || dir_end > d.size() || hdr >= dir_end) {
        error = "implausible ILB header offsets";
        return false;
    }
    const std::uint8_t* dir = &d[hdr];
    const std::size_t dir_size = dir_end - hdr;

    // Palette manager: u32 count, then count * (u32 + 1024) byte palettes.
    if (dir_size < 4) {
        error = "truncated ILB directory";
        return false;
    }
    header_.palette_count = read_u32(dir);
    header_.palette_bytes = header_.palette_count * kPaletteBytes;
    std::size_t p = 4 + header_.palette_bytes;
    if (p > dir_size) {
        error = "palette table runs past the directory";
        return false;
    }
    palettes_.clear();
    palette_formats_.clear();
    for (std::uint32_t k = 0; k < header_.palette_count; ++k) {
        // Each record is [u32 format tag][1024 entry bytes]; the tag picks the
        // entry byte order (see kPalFormatRgb / kPalFormatBgr above).
        const std::size_t tag_at = 4 + static_cast<std::size_t>(k) * kPaletteBytes;
        palette_formats_.push_back(read_u32(dir + tag_at));
        const std::size_t at = tag_at + 4;
        palettes_.emplace_back(dir + at, dir + at + 1024);
    }

    // A few assets carry a small prologue before the first record; resync.
    RecordSig sig;
    if (!read_record_sig(dir, dir_size, p, sig)) {
        for (std::size_t k = 1; k <= 16 && p + k + 17 <= dir_size; ++k) {
            if (read_record_sig(dir, dir_size, p + k, sig)) {
                p += k;
                break;
            }
        }
    }

    // TImageNode libraries. AoW's bitmap fonts store every glyph as
    //     u32 index, u32 class_id = 0x100, { u32 class_id, <image> }, u32 -1
    // and the wrapped image records carry no index of their own, so the walker
    // below - which matches "u32 index, u32 class_id, ..." - lands on the
    // node's *class id* and would report every entry as 0x100 / 0xFFFF. When
    // the first record is such a node, take each entry's index from the u32 in
    // front of the node instead. For a font that value is the glyph's character
    // code, which is exactly how TImageLibraryFont.SetTextEx addresses a glyph
    // (`Get(c)`, Ilpack.dpl 0x55214698). Ordinary libraries are untouched.
    bool node_lib = false;
    std::uint32_t node_index = 0;
    {
        RecordSig first;
        if (read_record_sig(dir, dir_size, p, first) &&
            first.index == kNodeClass && p >= 4) {
            node_lib = true;
            node_index = read_u32(dir + p - 4);
        }
    }

    // Mixed libraries (the UI sprite libraries, e.g. Int\Icons.ILB and
    // Int\Scenes\TitleM.ILB) interleave a TImageNode header before each
    // multi-layer sprite:
    //     u32 reference, u32 node_index, u32 class_id = 0x100,
    //     u32 layer0_class_id, <descriptor>,
    //     u32 0, u32 layer1_class_id, <descriptor>, u32 reference
    // The signature walk below lands on the node header (sig.index reads the
    // 0x100 class id), and the second layer is a plain record whose index field
    // is 0. Both layers belong to the node, so stamp the node's index (the u32
    // just before the class id) onto them. This is exactly what
    // TCustomImageLibrary.SetImage receives: LoadFromStream reads the node index
    // and one TImageNode per entry, whose Draw blits both layers.
    bool mixed_nodes = false;
    std::uint32_t mixed_index = 0;

    // Walk the record chain, terminated by an index of 0xFFFFFFFF. The
    // class-specific descriptor tail is variable, so the next record is found
    // by its signature rather than by computing a size.
    std::size_t blob = header_.data_offset;  // pixel blobs are concatenated
    while (read_record_sig(dir, dir_size, p, sig)) {
        if (sig.index == kRecordEnd) break;
        if (node_lib && sig.index == kNodeClass && p >= 4) {
            node_index = read_u32(dir + p - 4);  // next wrapped entry
        }
        if (!node_lib) {
            if (sig.index == kNodeClass && p >= 4) {
                mixed_nodes = true;
                mixed_index = read_u32(dir + p - 4);
            } else if (!(mixed_nodes && sig.index == 0)) {
                mixed_nodes = false;
            }
        }

        IlbEntry e;
        e.index = (node_lib || mixed_nodes) ? (node_lib ? node_index : mixed_index)
                                            : sig.index;
        e.class_id = sig.class_id;
        e.fmt = sig.fmt;
        e.dir_offset = p;
        e.name.assign(reinterpret_cast<const char*>(dir + p + 13),
                      sig.name_len);

        // Deterministic prefix of the descriptor (TLibraryImage.ReadImageData):
        //   +0x18 width, +0x1c height, +0x28, +0x2c, +0x34, +0x16 (u8),
        //   +0x10 serialized data size.
        const std::size_t q = p + 13 + sig.name_len;
        if (q + 25 <= dir_size) {
            e.width = read_u32(dir + q);
            e.height = read_u32(dir + q + 4);
            e.base_x = read_i32(dir + q + 8);   // +0x28 left margin
            e.base_y = read_i32(dir + q + 12);  // +0x2c top margin
            e.data_size = read_u32(dir + q + 21);
        }
        // TRLESprite08 (class 0x02): the RLE rectangle, escape byte and palette
        // follow the base descriptor and the TImage08 palette flag.
        if (sig.class_id == 0x02 && e.data_size) {
            std::size_t c = q + 20 + 1 + 4;  // base through the u32 at +0x10
            if (sig.fmt > 1) c += 12;
            if (sig.fmt > 2) c += 8;
            if (c + 1 <= dir_size) {
                const std::size_t pal_off = hdr + c;  // absolute file offset
                const std::uint8_t flag = dir[c++];   // +0x40
                if (flag == 0) {
                    if (c + 4 <= dir_size) e.palette_index = read_u32(dir + c);
                    c += 4;
                } else {
                    e.inline_palette_off = pal_off + 1 + 4;
                    c += 4 + 1024;                    // u32 + inline palette
                }
                if (c + 20 <= dir_size) {
                    e.rle_w = read_u32(dir + c);
                    e.rle_h = read_u32(dir + c + 4);
                    // Same placement fields as every other sprite class: the
                    // visual box lands at (param + base + shift). The glyphs of
                    // the bitmap fonts (Age8/Age8s/Age10/Age10s/Age10tb) are
                    // class 0x02, tightly cropped, so this shift is the *only*
                    // vertical metric that puts them on a common baseline; the
                    // x-height glyphs carry 2 and the caps 0. Skipping it (as the
                    // parser did) parks every glyph on its cell top.
                    e.xshift = read_i32(dir + c + 8);   // +0x50 shift x
                    e.yshift = read_i32(dir + c + 12);  // +0x54 shift y
                    e.rle_escape = dir[c + 16];       // +0x4C low byte
                }
            }
        } else if ((sig.class_id == 0x11 || sig.class_id == 0x12) &&
                   e.data_size) {
            // TRLESprite16 / TTransparentRLESprite16 (0x12 derives from 0x11):
            // RLE rect at +0x58/+0x5C, shift at +0x50/+0x54, escape word at +0x48.
            std::size_t c = q + 20 + 1 + 4;
            if (sig.fmt > 1) c += 12;
            if (sig.fmt > 2) c += 8;
            c += 4;  // TImage16 palette word at +0x44
            if (c + 20 <= dir_size) {
                e.rle_w = read_u32(dir + c);
                e.rle_h = read_u32(dir + c + 4);
                e.xshift = read_i32(dir + c + 8);   // +0x50
                e.yshift = read_i32(dir + c + 12);  // +0x54
                e.rle_escape = read_u16(dir + c + 16);
            }
        } else if (sig.class_id == 0x16 && e.data_size) {
            // TSprite16: raw 16-bit bitmap; its size lives at +0x64/+0x68 and the
            // shift at +0x5c/+0x60. The u16 right after the shift is the class's
            // colour key (BltColorKey16's key - see aow/ilb.hpp).
            std::size_t c = q + 20 + 1 + 4;
            if (sig.fmt > 1) c += 12;
            if (sig.fmt > 2) c += 8;
            c += 4;  // TImage16 palette word at +0x44
            if (c + 8 <= dir_size) {
                e.rle_w = read_u32(dir + c);
                e.rle_h = read_u32(dir + c + 4);
                if (c + 20 <= dir_size) {
                    e.xshift = read_i32(dir + c + 8);   // +0x5c
                    e.yshift = read_i32(dir + c + 12);  // +0x60
                }
                if (c + 18 <= dir_size) {
                    e.color_key = read_u16(dir + c + 16);  // BltColorKey16's key
                    e.has_color_key = true;
                }
            }
        }
        e.blob_offset = blob;
        blob += e.data_size;
        entries_.push_back(std::move(e));

        std::size_t next = dir_size;
        bool found = false;
        for (std::size_t o = q; o + 17 <= dir_size; ++o) {
            RecordSig s2;
            if (read_record_sig(dir, dir_size, o, s2)) {
                next = o;
                found = true;
                break;
            }
        }
        if (!found) break;
        p = next;
    }
    data_ = std::move(d);  // kept for decode_rgb()
    return true;
}

bool ilb_entry_names(const std::string& path, std::vector<std::string>& names,
                     std::string& error) {
    names.clear();
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "cannot open " + path;
        return false;
    }
    std::uint8_t head[0x18];
    in.read(reinterpret_cast<char*>(head), sizeof(head));
    if (in.gcount() != static_cast<std::streamsize>(sizeof(head))) {
        error = "file too small to be an ILB";
        return false;
    }
    for (int i = 0; i < 4; ++i) {
        if (head[i] != kMagic[i]) {
            error = "bad ILB magic";
            return false;
        }
    }
    const std::size_t hdr = read_u32(&head[0x0C]);
    const std::size_t dir_end = read_u32(&head[0x10]);
    const std::size_t hdr_sz = hdr ? hdr : 0x18;
    in.clear();
    in.seekg(0, std::ios::end);
    const std::size_t file_size = static_cast<std::size_t>(in.tellg());
    if (hdr_sz > file_size || dir_end > file_size || hdr_sz >= dir_end) {
        error = "implausible ILB header offsets";
        return false;
    }
    // The directory (palette table + records, with their entry names) lives in
    // [hdr, dir_end); reading up to dir_end avoids the pixel blobs entirely.
    std::vector<std::uint8_t> buf(dir_end);
    in.clear();
    in.seekg(0, std::ios::beg);
    in.read(reinterpret_cast<char*>(buf.data()),
            static_cast<std::streamsize>(dir_end));
    if (in.gcount() != static_cast<std::streamsize>(dir_end)) {
        error = "short read on ILB directory";
        return false;
    }
    const std::uint8_t* dir = buf.data() + hdr_sz;
    const std::size_t dir_size = dir_end - hdr_sz;
    if (dir_size < 4) {
        error = "truncated ILB directory";
        return false;
    }
    const std::uint32_t palettes = read_u32(dir);
    std::size_t p = 4 + static_cast<std::size_t>(palettes) * kPaletteBytes;
    if (p > dir_size) {
        error = "palette table runs past the directory";
        return false;
    }
    RecordSig sig;
    if (!read_record_sig(dir, dir_size, p, sig)) {
        for (std::size_t k = 1; k <= 16 && p + k + 17 <= dir_size; ++k) {
            if (read_record_sig(dir, dir_size, p + k, sig)) {
                p += k;
                break;
            }
        }
    }
    while (read_record_sig(dir, dir_size, p, sig)) {
        if (sig.index == kRecordEnd) break;
        names.emplace_back(reinterpret_cast<const char*>(dir + p + 13),
                           sig.name_len);
        const std::size_t q = p + 13 + sig.name_len;
        std::size_t next = dir_size;
        bool found = false;
        for (std::size_t o = q; o + 17 <= dir_size; ++o) {
            RecordSig s2;
            if (read_record_sig(dir, dir_size, o, s2)) {
                next = o;
                found = true;
                break;
            }
        }
        if (!found) break;
        p = next;
    }
    return true;
}

namespace {
// 16-bit DirectDraw pixel (RGB565) -> 8-bit RGB.
inline void rgb565(std::uint16_t v, std::uint8_t* out) {
    out[0] = static_cast<std::uint8_t>(((v >> 11) & 0x1F) * 255 / 31);
    out[1] = static_cast<std::uint8_t>(((v >> 5) & 0x3F) * 255 / 63);
    out[2] = static_cast<std::uint8_t>((v & 0x1F) * 255 / 31);
}

// The dominant colour of an RGBA sprite's 1-pixel border ring, packed 0xRRGGBB.
// Returns false when no single colour covers at least half the ring - a
// full-bleed picture is then left alone. `set_key` in the menu screens
// (src/game/menu.cpp) applies the same rule to a mask-less class-0x16 sprite.
bool dominant_border_color(const std::vector<std::uint8_t>& rgba,
                           std::uint32_t w, std::uint32_t h,
                           std::uint32_t& out) {
    if (w < 3 || h < 3) return false;
    std::vector<std::uint32_t> ring;
    ring.reserve(2 * (w + h));
    auto push = [&](std::uint32_t x, std::uint32_t y) {
        const std::uint8_t* p =
            &rgba[(static_cast<std::size_t>(y) * w + x) * 4];
        if (p[3] == 0) return;
        ring.push_back((static_cast<std::uint32_t>(p[0]) << 16) |
                       (static_cast<std::uint32_t>(p[1]) << 8) | p[2]);
    };
    for (std::uint32_t x = 0; x < w; ++x) {
        push(x, 0);
        push(x, h - 1);
    }
    for (std::uint32_t y = 1; y + 1 < h; ++y) {
        push(0, y);
        push(w - 1, y);
    }
    if (ring.empty()) return false;
    std::uint32_t best = 0, bestn = 0;
    for (std::size_t i = 0; i < ring.size(); ++i) {
        std::uint32_t n = 0;
        for (std::size_t j = 0; j < ring.size(); ++j) {
            if (ring[j] == ring[i]) ++n;
        }
        if (n > bestn) {
            bestn = n;
            best = ring[i];
        }
    }
    if (bestn * 2 < ring.size()) return false;  // no dominant background
    out = best;
    return true;
}
}  // namespace

bool IlbFile::decode_rgb(std::size_t index, std::vector<std::uint8_t>& rgba,
                         std::uint32_t& width, std::uint32_t& height,
                         std::string& error) const {
    if (index >= entries_.size()) {
        error = "entry index out of range";
        return false;
    }
    const IlbEntry& e = entries_[index];
    if (e.blob_offset + e.data_size > data_.size()) {
        error = "entry data runs past end of file";
        return false;
    }
    const std::uint8_t* blob = data_.data() + e.blob_offset;
    const std::size_t n = e.data_size;

    // Emits a raw RGB565 bitmap. A class-0x16 sprite is always colour-keyed (the
    // engine blits it with `GFXE.BltColorKey16`), so pixels equal to its key are
    // left fully transparent; every other raw picture is opaque.
    auto emit_raw16 = [&](std::uint32_t w, std::uint32_t h, bool keyed) {
        width = w;
        height = h;
        const std::size_t px = static_cast<std::size_t>(w) * h;
        rgba.assign(px * 4, 0);
        std::size_t removed = 0;
        for (std::size_t i = 0; i < px; ++i) {
            const std::uint16_t v = read_u16(blob + i * 2);
            if (keyed && e.has_color_key && v == e.color_key) {
                ++removed;
                continue;
            }
            rgb565(v, &rgba[i * 4]);
            rgba[i * 4 + 3] = 0xFF;
        }
        // A few shipped class-0x16 frames carry a *stale* key that does not occur
        // in the bitmap at all - the blended glow effects (MISC_OBJ's gold
        // glitter, EPL_DNG2's dungeon mouth) draw over black. Keying the stored
        // value removes nothing there, so the sprite's dominant border colour is
        // keyed instead (the menu screens' rule for a mask-less sprite); a
        // full-bleed picture has no dominant border and is left untouched.
        if (keyed && removed == 0) {
            std::uint32_t border = 0;
            if (dominant_border_color(rgba, w, h, border)) {
                const std::uint8_t br = static_cast<std::uint8_t>((border >> 16) & 0xFF);
                const std::uint8_t bg = static_cast<std::uint8_t>((border >> 8) & 0xFF);
                const std::uint8_t bb = static_cast<std::uint8_t>(border & 0xFF);
                for (std::size_t i = 0; i < px; ++i) {
                    std::uint8_t* p = &rgba[i * 4];
                    if (p[3] != 0 && p[0] == br && p[1] == bg && p[2] == bb) {
                        p[3] = 0;
                    }
                }
            }
        }
    };

    // --- TSprite16 (class 0x16): raw 16-bit RGB565, colour-keyed -------------
    // The stored blob is simply w*h*2 little-endian RGB565 words, where w/h are
    // the RLE-rect fields (+0x64/+0x68; the base +0x18/+0x1c slot can be larger,
    // e.g. a 5x5 `GRSparks00` inside a 20x20 slot).
    if (e.class_id == 0x16) {
        if (e.rle_w && e.rle_h &&
            n == static_cast<std::size_t>(e.rle_w) * e.rle_h * 2) {
            emit_raw16(e.rle_w, e.rle_h, true);
            return true;
        }
        if (e.width && e.height &&
            n == static_cast<std::size_t>(e.width) * e.height * 2) {
            emit_raw16(e.width, e.height, true);
            return true;
        }
    }

    // --- raw 16-bit picture (TPicture16, class 0x10, and a few others) ---
    // No mask: the engine draws a TPicture16 with a plain opaque blit
    // (`TPicture16.ShowOpaque@0x5520ef4c` has no colour key).
    if (e.class_id != 0x16 && e.width && e.height &&
        n == static_cast<std::size_t>(e.width) * e.height * 2) {
        emit_raw16(e.width, e.height, false);
        return true;
    }

    // --- TRLESprite16 (class 0x11) / TTransparentRLESprite16 (0x12) ---------
    // Length-prefixed rows of 16-bit RLE.
    // Per FUN_55210b54 (decompiled with the __register convention): each row
    // begins with a u32 length (including the header); within a row a u16 token
    // equal to the escape word is followed by a run length in *bytes*
    // (count/2 transparent pixels), any other u16 is one opaque RGB565 pixel.
    if ((e.class_id == 0x11 || e.class_id == 0x12) && e.rle_w && e.rle_h) {
        width = e.rle_w;
        height = e.rle_h;
        const std::size_t total = static_cast<std::size_t>(width) * height;
        std::vector<std::uint8_t> idx(total * 3, 0);  // RGB
        std::vector<std::uint8_t> alpha(total, 0);
        std::size_t p = 0;
        for (std::uint32_t y = 0; y < height; ++y) {
            if (p + 4 > n) { error = "sprite RLE overran its data"; return false; }
            const std::uint32_t len = read_u32(blob + p);
            if (len < 4 || p + len > n) {
                error = "sprite RLE overran its data";
                return false;
            }
            const std::size_t end = p + len;
            std::size_t i = p + 4;
            std::uint32_t x = 0;
            while (x < width) {
                if (i + 2 > end) break;  // row exhausted
                const std::uint16_t tok = read_u16(blob + i);
                i += 2;
                if (tok == e.rle_escape) {
                    if (i + 2 > end) break;
                    const std::uint16_t cnt = read_u16(blob + i);
                    i += 2;
                    x += cnt / 2;  // run length is in bytes
                } else {
                    if (x < width) {
                        const std::size_t o = static_cast<std::size_t>(y) * width + x;
                        rgb565(tok, &idx[o * 3]);
                        alpha[o] = 0xFF;
                    }
                    ++x;
                }
            }
            p = (p + len + 3) & ~static_cast<std::size_t>(3);
        }
        rgba.assign(total * 4, 0);
        for (std::size_t i = 0; i < total; ++i) {
            rgba[i * 4 + 0] = idx[i * 3 + 0];
            rgba[i * 4 + 1] = idx[i * 3 + 1];
            rgba[i * 4 + 2] = idx[i * 3 + 2];
            rgba[i * 4 + 3] = alpha[i];
        }
        return true;
    }

    // --- TRLESprite08 (class 0x02): rectangle RLE of 8-bit palette indices ---
    // Per FUN_55216ae8 decompiled with the __register convention:
    //   for each of rle_h rows: skip a 4-byte row header, then fill rle_w
    //   pixels; a token equal to the escape byte is followed by a run length
    //   (transparent pixels), any other byte is one opaque palette index.
    if (e.class_id == 0x02 && e.rle_w && e.rle_h) {
        width = e.rle_w;
        height = e.rle_h;
        const std::size_t total = static_cast<std::size_t>(width) * height;
        std::vector<std::uint8_t> idx(total, 0xFF);  // 0xFF = transparent
        std::size_t p = 0;
        for (std::uint32_t y = 0; y < height; ++y) {
            p += 4;  // fixed per-row header
            std::uint32_t x = 0;
            while (x < width) {
                if (p >= n) {
                    error = "sprite RLE overran its data";
                    return false;
                }
                const std::uint8_t b = blob[p++];
                if (b == e.rle_escape) {
                    if (p >= n) {
                        error = "sprite RLE overran its data";
                        return false;
                    }
                    x += blob[p++];
                } else {
                    if (x < width) {
                        idx[static_cast<std::size_t>(y) * width + x] = b;
                    }
                    ++x;
                }
            }
        }

        // Palette: inline in the record, or looked up in the palette manager.
        // Its own u32 format tag (immediately before the 1024 bytes) decides the
        // per-entry byte order - see kPalFormatRgb / kPalFormatBgr.
        const std::uint8_t* pal = nullptr;
        std::uint32_t pal_fmt = kPalFormatRgb;
        if (e.inline_palette_off &&
            e.inline_palette_off + 1024 <= data_.size()) {
            pal = data_.data() + e.inline_palette_off;
            if (e.inline_palette_off >= 4) {
                pal_fmt = read_u32(data_.data() + e.inline_palette_off - 4);
            }
        } else if (e.palette_index < palettes_.size() &&
                   palettes_[e.palette_index].size() >= 1024) {
            pal = palettes_[e.palette_index].data();
            if (e.palette_index < palette_formats_.size()) {
                pal_fmt = palette_formats_[e.palette_index];
            }
        }
        const bool bgr = (pal_fmt == kPalFormatBgr);
        rgba.assign(total * 4, 0);
        for (std::size_t i = 0; i < total; ++i) {
            std::uint8_t* o = &rgba[i * 4];
            const std::uint8_t v = idx[i];
            if (v == 0xFF) continue;  // transparent
            if (!pal) {
                o[0] = o[1] = o[2] = v;  // greyscale fallback
                o[3] = 0xFF;
                continue;
            }
            // Entries are (R,G,B) for the 0x88801B18 tag (every shipped ILB) and
            // (B,G,R) for 0x88809318. The old code always swapped, which tinted
            // every 8-bit unit sprite blue (R<->B).
            if (bgr) {
                o[0] = pal[v * 4 + 2];
                o[1] = pal[v * 4 + 1];
                o[2] = pal[v * 4 + 0];
            } else {
                o[0] = pal[v * 4 + 0];
                o[1] = pal[v * 4 + 1];
                o[2] = pal[v * 4 + 2];
            }
            o[3] = 0xFF;
        }
        return true;
    }

    error = "unsupported image encoding (class 0x" +
            std::to_string(e.class_id) + ")";
    return false;
}

}  // namespace aow


