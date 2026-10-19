// inflate.cpp - zlib/DEFLATE decompressor (RFC 1950 + RFC 1951).
//
// Reference: RFC 1951 section 3.2.2 (puff.c-style canonical Huffman decode).
// stdlib only; no zlib dependency.
#include "aow/inflate.hpp"

namespace aow {
namespace {

constexpr int kMaxBits = 15;

struct Reader {
    const std::uint8_t* d;
    std::size_t n;
    std::size_t pos = 0;
    std::uint32_t buf = 0;
    int cnt = 0;

    bool bits(int need, std::uint32_t& out) {
        while (cnt < need) {
            if (pos >= n) return false;
            buf |= static_cast<std::uint32_t>(d[pos++]) << cnt;
            cnt += 8;
        }
        out = (need == 0) ? 0u : (buf & ((1u << need) - 1u));
        buf >>= need;
        cnt -= need;
        return true;
    }
    void align() {
        const int r = cnt & 7;
        buf >>= r;
        cnt -= r;
    }
};

// Builds canonical Huffman tables (count[len] + symbol[]). Returns 0 when the
// code is complete, >0 when incomplete-but-usable, <0 on error (over-subscribed).
int construct(const std::uint8_t* lengths, int n, std::uint16_t* count,
              std::uint16_t* symbol) {
    for (int i = 0; i <= kMaxBits; ++i) count[i] = 0;
    for (int i = 0; i < n; ++i) ++count[lengths[i]];
    if (count[0] == n) return 0;  // no codes at all
    int left = 1;
    for (int len = 1; len <= kMaxBits; ++len) {
        left <<= 1;
        left -= count[len];
        if (left < 0) return -1;
    }
    std::uint16_t offs[kMaxBits + 2];
    offs[1] = 0;
    for (int len = 1; len <= kMaxBits; ++len) offs[len + 1] = offs[len] + count[len];
    for (int i = 0; i < n; ++i) {
        if (lengths[i]) symbol[offs[lengths[i]]++] = static_cast<std::uint16_t>(i);
    }
    return left;
}

int decode(Reader& r, const std::uint16_t* count, const std::uint16_t* symbol) {
    int code = 0, first = 0, index = 0;
    for (int len = 1; len <= kMaxBits; ++len) {
        std::uint32_t b;
        if (!r.bits(1, b)) return -1;
        code |= static_cast<int>(b);
        const int c = count[len];
        if (code - c < first) return symbol[index + (code - first)];
        index += c;
        first += c;
        first <<= 1;
        code <<= 1;
    }
    return -1;
}

const std::uint16_t kLenBase[29] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,
    35,43,51,59,67,83,99,115,131,163,195,227,258};
const std::uint8_t kLenExtra[29] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,
    4,4,4,4,5,5,5,5,0};
const std::uint16_t kDistBase[30] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,
    193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
const std::uint8_t kDistExtra[30] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,
    9,9,10,10,11,11,12,12,13,13};

// Decodes one DEFLATE block; sets `final_block` from BFINAL.
bool inflate_block(Reader& r, std::vector<std::uint8_t>& out, bool& final_block) {
    std::uint16_t lcount[kMaxBits + 1], lsym[288];
    std::uint16_t dcount[kMaxBits + 1], dsym[30];
    std::uint8_t lengths[320];

    std::uint32_t bfinal, btype;
    if (!r.bits(1, bfinal) || !r.bits(2, btype)) return false;
    final_block = (bfinal != 0);

    if (btype == 0) {  // stored (uncompressed)
        r.align();
        std::uint32_t len, nlen;
        if (!r.bits(16, len) || !r.bits(16, nlen)) return false;
        if ((len ^ 0xFFFFu) != nlen) return false;
        for (std::uint32_t i = 0; i < len; ++i) {
            std::uint32_t byte;
            if (!r.bits(8, byte)) return false;
            out.push_back(static_cast<std::uint8_t>(byte));
        }
    } else if (btype == 1) {  // fixed Huffman
        for (int i = 0; i < 144; ++i) lengths[i] = 8;
        for (int i = 144; i < 256; ++i) lengths[i] = 9;
        for (int i = 256; i < 280; ++i) lengths[i] = 7;
        for (int i = 280; i < 288; ++i) lengths[i] = 8;
        if (construct(lengths, 288, lcount, lsym) < 0) return false;
        for (int i = 0; i < 30; ++i) lengths[i] = 5;
        if (construct(lengths, 30, dcount, dsym) < 0) return false;
    } else if (btype == 2) {  // dynamic Huffman
        std::uint32_t hlit, hdist, hclen;
        if (!r.bits(5, hlit) || !r.bits(5, hdist) || !r.bits(4, hclen)) return false;
        const int nlen = static_cast<int>(hlit) + 257;
        const int ndist = static_cast<int>(hdist) + 1;
        const int ncode = static_cast<int>(hclen) + 4;
        static const int order[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
        std::uint8_t clen[19] = {0};
        for (int i = 0; i < ncode; ++i) {
            std::uint32_t v;
            if (!r.bits(3, v)) return false;
            clen[order[i]] = static_cast<std::uint8_t>(v);
        }
        std::uint16_t ccount[kMaxBits + 1], csym[19];
        if (construct(clen, 19, ccount, csym) < 0) return false;
        int i = 0;
        while (i < nlen + ndist) {
            const int sym = decode(r, ccount, csym);
            if (sym < 0) return false;
            if (sym < 16) {
                lengths[i++] = static_cast<std::uint8_t>(sym);
                continue;
            }
            int rep;
            std::uint8_t val = 0;
            if (sym == 16) {
                if (i == 0) return false;
                val = lengths[i - 1];
                std::uint32_t e;
                if (!r.bits(2, e)) return false;
                rep = 3 + static_cast<int>(e);
            } else if (sym == 17) {
                std::uint32_t e;
                if (!r.bits(3, e)) return false;
                rep = 3 + static_cast<int>(e);
            } else {
                std::uint32_t e;
                if (!r.bits(7, e)) return false;
                rep = 11 + static_cast<int>(e);
            }
            if (i + rep > nlen + ndist) return false;
            while (rep-- > 0) lengths[i++] = val;
        }
        if (lengths[256] == 0) return false;  // no end-of-block code
        if (construct(lengths, nlen, lcount, lsym) < 0) return false;
        if (construct(lengths + nlen, ndist, dcount, dsym) < 0) return false;
    } else {
        return false;  // btype == 3 is invalid
    }

    for (;;) {
        const int sym = decode(r, lcount, lsym);
        if (sym < 0) return false;
        if (sym < 256) {
            out.push_back(static_cast<std::uint8_t>(sym));
        } else if (sym == 256) {
            break;  // end of block
        } else {
            const int li = sym - 257;
            if (li >= 29) return false;
            std::uint32_t extra = 0;
            if (kLenExtra[li] != 0 && !r.bits(kLenExtra[li], extra)) return false;
            const int length = kLenBase[li] + static_cast<int>(extra);
            const int di = decode(r, dcount, dsym);
            if (di < 0 || di >= 30) return false;
            std::uint32_t dextra = 0;
            if (kDistExtra[di] != 0 && !r.bits(kDistExtra[di], dextra)) return false;
            const int dist = kDistBase[di] + static_cast<int>(dextra);
            if (static_cast<std::size_t>(dist) > out.size()) return false;
            const std::size_t start = out.size() - static_cast<std::size_t>(dist);
            for (int k = 0; k < length; ++k) {
                out.push_back(out[start + static_cast<std::size_t>(k)]);
            }
        }
    }
    return true;
}

}  // namespace

bool looks_like_zlib(const std::uint8_t* in, std::size_t n) {
    if (n < 2) return false;
    if ((in[0] & 0x0F) != 8) return false;         // CM == deflate
    return ((in[0] << 8) | in[1]) % 31 == 0;       // FCHECK
}

bool zlib_inflate(const std::uint8_t* in, std::size_t n,
                  std::vector<std::uint8_t>& out) {
    if (!looks_like_zlib(in, n)) return false;
    Reader r{in + 2, n - 2};  // skip the 2-byte zlib header
    out.clear();
    for (;;) {
        bool final_block = false;
        if (!inflate_block(r, out, final_block)) return false;
        if (final_block) break;
    }
    return true;
}

}  // namespace aow
