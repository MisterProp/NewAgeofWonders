// pfs.cpp - implementation of the .PFS rule-data reader.
//
// See pfs.hpp for the format and re/exports/enginep/ for the decompiled
// reference (Engine.TPropertyTable.LoadFromStream / SaveToStream, GetCRC32,
// TEngine.ReadFromFileCRC, TEReadStorageStream.*).
#include "aow/endian.hpp"
#include "aow/pfs.hpp"

#include <fstream>
#include <iterator>
#include <utility>

namespace aow {

std::uint32_t PfsFile::crc32(const std::uint8_t* data, std::size_t n) {
    static std::uint32_t table[256];
    static bool init = false;
    if (!init) {
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int k = 0; k < 8; ++k) {
                c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            }
            table[i] = c;
        }
        init = true;
    }
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < n; ++i) {
        crc = table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFu;
}

namespace {

using Entry = std::pair<std::uint32_t, std::uint32_t>;  // (tag, offset)

struct Table {
    std::vector<Entry> entries;
    std::size_t base = 0;  // position just past the table
};

// Parses a property table at 'pos' in [R, R+n). False if out of bounds.
bool read_table(const std::uint8_t* R, std::size_t n, std::size_t pos,
                Table& out) {
    if (pos >= n) return false;
    const std::uint8_t b = R[pos];
    std::size_t p = pos + 1;
    std::uint64_t small = 0;
    std::uint64_t large = 0;
    if ((b & 0x80) != 0) {
        small = static_cast<std::uint64_t>(b & 0x7F);
        if (p + 4 > n) return false;
        large = read_u32(R + p);
        p += 4;
    } else {
        small = b;
    }
    if (small + large > 50000) return false;
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

bool in_bounds(const Table& t, std::size_t n) {
    for (const Entry& e : t.entries) {
        if (t.base + e.second > n) return false;
    }
    return true;
}

// A "record list" table: >= 2 entries with strictly increasing, in-bounds
// offsets. The real lists always start at offset 0.
bool is_list(const Table& t, std::size_t n) {
    if (t.entries.size() < 2) return false;
    for (std::size_t i = 0; i + 1 < t.entries.size(); ++i) {
        if (t.entries[i].second >= t.entries[i + 1].second) return false;
    }
    return t.base + t.entries.back().second <= n;
}

bool is_printable(const std::uint8_t* p, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
        const std::uint8_t c = p[i];
        if (!((c >= 32 && c < 127) || c == 9 || c == 10 || c == 13)) {
            return false;
        }
    }
    return true;
}

}  // namespace

bool PfsFile::load(const std::string& path, std::string& error) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "cannot open " + path;
        return false;
    }
    data_.assign(std::istreambuf_iterator<char>(in),
                 std::istreambuf_iterator<char>());

    has_crc_ = false;
    crc_ok_ = false;
    region_off_ = 0;
    region_size_ = data_.size();
    list_off_ = 0;
    list_base_ = 0;
    list_entries_ = 0;
    records_.clear();

    // Container: crc2 | region | crc1. The older variant has no CRCs.
    if (data_.size() >= 8) {
        const std::uint32_t c2 = crc32(data_.data() + 4, data_.size() - 4);
        const std::uint32_t c1 = crc32(data_.data() + 4, data_.size() - 8);
        if (read_u32(data_.data()) == c2 &&
            read_u32(data_.data() + data_.size() - 4) == c1) {
            has_crc_ = true;
            crc_ok_ = true;
            region_off_ = 4;
            region_size_ = data_.size() - 8;
        }
    }

    const std::uint8_t* R = data_.data() + region_off_;
    const std::size_t n = region_size_;
    if (n == 0) return true;

    // Collect candidate tables with a small bounded DFS from the region root.
    struct Cand {
        std::size_t pos;
        Table t;
    };
    std::vector<Cand> cands;
    std::vector<std::uint8_t> seen(n, 0);
    std::vector<std::pair<std::size_t, int>> stack;
    stack.emplace_back(0, 0);
    while (!stack.empty()) {
        const std::size_t pos = stack.back().first;
        const int depth = stack.back().second;
        stack.pop_back();
        if (depth > 6 || pos >= n || seen[pos] != 0) continue;
        seen[pos] = 1;
        Table t;
        if (!read_table(R, n, pos, t)) continue;
        cands.push_back(Cand{pos, t});
        if (!in_bounds(t, n)) continue;
        for (const Entry& e : t.entries) {
            const std::size_t tgt = t.base + e.second;
            if (tgt >= n || seen[tgt] != 0) continue;
            Table sub;
            if (!read_table(R, n, tgt, sub)) continue;
            if (!in_bounds(sub, n)) continue;
            if (is_list(sub, n) || sub.entries.size() <= 4) {
                stack.emplace_back(tgt, depth + 1);
            }
        }
    }

    // The record list: the largest well-formed table whose data starts at 0.
    const Cand* best = nullptr;
    for (const Cand& c : cands) {
        if (!is_list(c.t, n)) continue;
        if (c.t.entries.empty() || c.t.entries.front().second != 0) continue;
        if (best == nullptr || c.t.entries.size() > best->t.entries.size()) {
            best = &c;
        }
    }
    if (best == nullptr) return true;  // no records (e.g. General.pfs)

    list_off_ = best->pos;
    list_base_ = best->t.base;
    list_entries_ = best->t.entries.size();
    const std::vector<Entry>& E = best->t.entries;

    for (std::size_t i = 0; i < E.size(); ++i) {
        PfsRecord rec;
        rec.tag = E[i].first;
        const std::size_t s = best->t.base + E[i].second;
        const std::size_t e =
            (i + 1 < E.size()) ? best->t.base + E[i + 1].second : n;
        rec.file_offset = region_off_ + s;
        rec.size = e - s;

        // Field table at offset 0, or after a u32 class id at offset 4.
        for (int start = 0; start <= 4; start += 4) {
            if (static_cast<std::size_t>(start) >= rec.size) continue;
            Table rt;
            if (!read_table(R + s, rec.size, static_cast<std::size_t>(start),
                            rt)) {
                continue;
            }
            if (rt.entries.empty()) continue;
            std::size_t prev = 0;
            bool ok = true;
            for (const Entry& f : rt.entries) {
                if (rt.base + f.second > rec.size || f.second < prev) {
                    ok = false;
                    break;
                }
                prev = f.second;
            }
            if (!ok) continue;
            rec.fields_offset = static_cast<std::size_t>(start);
            for (std::size_t j = 0; j < rt.entries.size(); ++j) {
                PfsField fld;
                fld.tag = rt.entries[j].first;
                fld.offset = rt.base + rt.entries[j].second;
                fld.size = ((j + 1 < rt.entries.size())
                                ? rt.base + rt.entries[j + 1].second
                                : rec.size) -
                           fld.offset;
                rec.fields.push_back(fld);
            }
            break;  // the first valid start wins
        }

        // u32-length-prefixed strings (descriptions, names, asset refs).
        const std::uint8_t* rb = R + s;
        std::size_t k = 0;
        while (k + 4 <= rec.size) {
            const std::uint32_t len = read_u32(rb + k);
            if (len >= 1 && len <= 4000 && k + 4 + len <= rec.size &&
                is_printable(rb + k + 4, len)) {
                PfsString ps;
                ps.offset = k;
                ps.text.assign(reinterpret_cast<const char*>(rb + k + 4), len);
                rec.strings.push_back(std::move(ps));
                k += 4 + len;
                continue;
            }
            ++k;
        }
        records_.push_back(std::move(rec));
    }
    return true;
}

}  // namespace aow

