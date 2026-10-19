// pfs.hpp - Age of Wonders .PFS rule-data container.
//
// The 12 .PFS files in Release/ hold the game's rule tables (abilities,
// spells, items, heroes, unit resources, unit graphics, combat resources,
// faces, item graphics, text, general). They are a generic engine *object
// graph* serialization (Engine.TEStorageStream / TPropertyTable from
// Enginep.dpl), not a bespoke record format, so the reader below parses the
// container and then walks it.
//
// File layout (little-endian). Offsets are file offsets.
//
//   u32 crc2                       CRC-32 of bytes [4, size)
//   bytes[4 .. size-4)             the object region      <-- the payload
//   u32 crc1                       CRC-32 of bytes [4, size-4)
//
// `HEROES.PFS` and `ITEMS.PFS` are an older variant that omits both CRCs;
// for those the whole file is the object region. `General.pfs` is an empty
// (all-zero, 5-byte) stub.
//
// The object region is an "EObject": a property table followed by a data area:
//
//   <table>  base=...
//   <data>   the fields/sub-objects live at base + entry.offset
//
// A property table is a byte `n` (bit 0x80 flags that a u32 large-count
// follows), then `n & 0x7f` compact entries, then `large_count` wide entries:
//   compact entry: u16  (tag = low byte, offset = high byte)
//   wide    entry: u32 tag, u32 offset
// Each entry maps a property tag to an offset relative to the table's data
// base. An entry's value may itself be another table (a nested object) or
// raw data. The top-level record list is the table with the most entries whose
// offsets are strictly increasing and start at 0; each of its entries points
// at one record.
//
// A record is `[table][data]`, optionally preceded by a u32 class id (when the
// list creates its elements from a class rather than reusing an instance), so
// record fields are located at `0` or `4`. Field *semantics* (which tag is a
// name, a cost, ...) are class-specific; this reader exposes the raw (tag,
// offset, size) triples plus the u32-length-prefixed strings found in each
// record, which is what the rule text and asset references are stored as.
//
// Reference: port/re/exports/enginep/ (TPropertyTable.LoadFromStream,
// TEngine.ReadFromFileCRC, TEReadStorageStream.*, GetCRC32).
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace aow {

// One field of a record: a (tag -> byte range) entry of the record's property
// table. `offset` is relative to the record start; `size` is the number of
// bytes available for the field (up to the next field, or the record end).
struct PfsField {
    std::uint32_t tag = 0;
    std::size_t offset = 0;
    std::size_t size = 0;
};

// A u32-length-prefixed string found inside a record (u32 length, then bytes).
struct PfsString {
    std::size_t offset = 0;  // offset within the record
    std::string text;
};

// One element of the file's top-level record list.
struct PfsRecord {
    std::uint32_t tag = 0;        // record id (tag in the list table)
    std::size_t file_offset = 0;  // absolute offset of the record in the file
    std::size_t size = 0;         // record length in bytes
    std::size_t fields_offset = 0;  // 0 or 4 - where the field table starts
    std::vector<PfsField> fields;    // parsed field table (may be empty)
    std::vector<PfsString> strings;  // strings found in the record
};

class PfsFile {
public:
    // Loads and parses a .PFS file. Returns false and sets 'error' on failure.
    bool load(const std::string& path, std::string& error);

    // Container.
    bool has_crc() const { return has_crc_; }
    bool crc_ok() const { return crc_ok_; }
    std::size_t region_offset() const { return region_off_; }
    std::size_t region_size() const { return region_size_; }

    // Record list (within the region).
    std::size_t list_offset() const { return list_off_; }
    std::size_t list_base() const { return list_base_; }
    std::size_t list_entries() const { return list_entries_; }
    const std::vector<PfsRecord>& records() const { return records_; }

    const std::vector<std::uint8_t>& bytes() const { return data_; }

    // Standard CRC-32 (IEEE 802.3), as used by the container.
    static std::uint32_t crc32(const std::uint8_t* data, std::size_t n);

private:
    std::vector<std::uint8_t> data_;
    bool has_crc_ = false;
    bool crc_ok_ = false;
    std::size_t region_off_ = 0;
    std::size_t region_size_ = 0;
    std::size_t list_off_ = 0;
    std::size_t list_base_ = 0;
    std::size_t list_entries_ = 0;
    std::vector<PfsRecord> records_;
};

}  // namespace aow
