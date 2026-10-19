// ilb.hpp - Age of Wonders .ILB image-library container.
//
// Layout (little-endian) reconstructed from ILPACK.dpl (ImageLib unit). See
// port/re/exports/ilpack/ for the decompiled reference:
//   TCustomImageLibrary.LoadFromStream / LoadFromFile
//   TPaletteManager.LoadFromStream, GFXE.TGFXEPalette.LoadFromStream
//   TLibraryImage.ReadImageData, TImage08.ReadImageData, ReadImageFromStream
//
//   0x00 char[4]  magic  = 04 'I' 'L' 'B'
//   0x04 u32      flags
//   0x08 f32      format version (retail assets use 4.0)
//   0x0C u32      header size (24 for v4)
//   0x10 u32      data offset  (start of the pixel-data region)
//   0x14 u32      total file size
//
// [header_size, data_offset) is the "directory":
//    u32 palette_count, then palette_count * (u32 + 1024) byte palettes,
//    then a run of image records terminated by an index of 0xFFFFFFFF:
//        u32 index, u32 class_id, <descriptor>, u32 reference
//    The descriptor starts with u8 fmt + a u32-length-prefixed name; the
//    class-specific tail is variable, so records are located by signature.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace aow {

struct IlbHeader {
    std::uint32_t flags = 0;
    float version = 0.0f;
    std::uint32_t header_size = 0;
    std::uint32_t data_offset = 0;
    std::uint32_t total_size = 0;
    std::uint32_t palette_count = 0;
    std::uint32_t palette_bytes = 0;  // palette_count * (4 + 1024)
};

struct IlbEntry {
    std::uint32_t index = 0;      // image slot id
    std::uint32_t class_id = 0;   // TLibraryImage subclass tag
    std::uint8_t fmt = 0;         // descriptor revision (0..3)
    std::uint32_t width = 0;      // descriptor field +0x18
    std::uint32_t height = 0;     // descriptor field +0x1c
    std::uint32_t data_size = 0;  // descriptor field +0x10 (pixel/RLE bytes)
    std::size_t dir_offset = 0;   // record offset within the directory
    std::size_t blob_offset = 0;  // offset of the pixel data in the file
    std::string name;             // e.g. "UnitFr.BMP"

    // Sprite RLE rectangle (classes 0x02 TRLESprite08 and 0x11 TRLESprite16).
    // The RLE bitmap can be smaller than the base +0x18/+0x1c slot.
    std::uint32_t rle_w = 0;
    std::uint32_t rle_h = 0;
    std::uint16_t rle_escape = 0;  // 0x02: +0x4C byte; 0x11: +0x48 word
    std::uint32_t palette_index = 0;      // index into the palette manager
    std::size_t inline_palette_off = 0;   // absolute file offset, 0 = none

    // Class 0x16 (TSprite16) colour key. The engine always draws a TSprite16 with
    // a colour-keyed blit - `ImageLib.TSprite16.ShowOpaque@0x5521193c` calls
    // `GFXE.BltColorKey16@0x55202050` (and `TSprite16.ShowTransparent` a keyed
    // blend) - and the key is the u16 stored in the descriptor right after the
    // shift fields, i.e. the same slot class 0x11/0x12 use for their RLE escape
    // word. Pixels equal to it are transparent. `0x0000` is a legitimate key
    // (black), so `has_color_key` gates it. Verified against the sprite
    // backgrounds: `Cursors.ilb` id 0 -> 0x418C, `General.ILB` 3dHex -> 0x05A9,
    // `Str_mine.ILB` frame 10 -> 0x0000, `Str_wtwr.ILB` Wtwr00 -> 0x0000.
    std::uint16_t color_key = 0;
    bool has_color_key = false;

    // Sprite placement. The engine blits a sprite so that its *visual box*
    // (the non-transparent rectangle) lands at
    //   (param_x + base_x + xshift, param_y + base_y + yshift)
    // where param_x/param_y is the caller's anchor. Evidence:
    //   re/exports/ilpack/TSprite16.ShowOpaque.c  (dest = param + base + shift)
    //   re/exports/ilpack/TRLESprite16.GetVisualDimensions.c
    //     (visual left = base_x + xshift, top = base_y + yshift, size = rle_w x rle_h)
    // base_x/base_y are the TLibraryImage left/top margins (descriptor +0x28/
    // +0x2c, present for every class); xshift/yshift are the class shift fields
    // (+0x50/+0x54 for 0x11, +0x5c/+0x60 for 0x16). All are signed: content may
    // extend left of / above the anchor.
    std::int32_t base_x = 0;  // descriptor +0x28
    std::int32_t base_y = 0;  // descriptor +0x2c
    std::int32_t xshift = 0;  // class shift x (+0x50 / +0x5c)
    std::int32_t yshift = 0;  // class shift y (+0x54 / +0x60)
};

class IlbFile {
public:
    // Loads and parses an .ILB file. Returns false and sets 'error' on failure.
    bool load(const std::string& path, std::string& error);

    // Parses an .ILB image held in memory. Map-object resources embed their own
    // image library inline in Release.hss (TMapObjectResource rw tag 8) - it is a
    // complete ILB image (magic `04 'I' 'L' 'B'`, version 4.0), see
    // aow/map_resources.hpp. Returns false and sets 'error' on failure.
    bool load_memory(const std::uint8_t* data, std::size_t size,
                     std::string& error);
    const IlbHeader& header() const { return header_; }
    const std::vector<IlbEntry>& entries() const { return entries_; }
    // 256-entry palettes (each 1024 bytes) from the palette manager. The byte
    // order of each entry is given by the matching palette_formats_ tag below.
    const std::vector<std::vector<std::uint8_t>>& palettes() const {
        return palettes_;
    }
    // The u32 stored in front of each 1024-byte palette. GFXE.TGFXEPalette.Remap
    // (Gfxepack.dpl 0x5500ba34) switches on it to pick the entry byte order:
    //   0x88801B18 -> bytes are (R,G,B)   [every shipped palette]
    //   0x88809318 -> bytes are (B,G,R)
    // A palette entry is therefore (R,G,B) unless its tag is 0x88809318.
    const std::vector<std::uint32_t>& palette_formats() const {
        return palette_formats_;
    }

    // Decodes entry `index` into an 8-bit RGB buffer (3 bytes per pixel) and
    // reports its width/height. Supports the raw 16-bit picture class (0x10)
    // and the rectangle-RLE 8-bit sprite class (0x02, fmt 2).
    bool decode_rgb(std::size_t index, std::vector<std::uint8_t>& rgb,
                    std::uint32_t& width, std::uint32_t& height,
                    std::string& error) const;

private:
    IlbHeader header_;
    std::vector<IlbEntry> entries_;
    std::vector<std::vector<std::uint8_t>> palettes_;
    std::vector<std::uint32_t> palette_formats_;  // tag per palette (byte order)
    std::vector<std::uint8_t> data_;  // whole file, kept for decoding
};

// Enumerates an ILB's image names by reading only its header + directory (the
// pixel-data region is never touched). Used to resolve a map resource's image
// name (e.g. "gr_tree.BMP") to the library that actually contains it, when no
// file carries that stem. `names` is ordered by entry index. Returns false and
// sets `error` on failure.
bool ilb_entry_names(const std::string& path, std::vector<std::string>& names,
                     std::string& error);

}  // namespace aow
