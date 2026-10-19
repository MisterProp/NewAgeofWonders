// galaxy_it.cpp - Impulse-Tracker loader (port of galaxy.dll FUN_10020290).
//
// Only the "IMPM" branch of glxLoadMusic is reproduced here: the AoW songs all
// take that path.  Comments cite the original offsets/behaviour.
#include "aow/galaxy.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>

namespace aow {
namespace galaxy {
namespace {

std::uint16_t rd16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0] | (p[1] << 8));
}
std::uint32_t rd32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

// Galaxy's sample header (FUN_10027410 reader).  NOTE the field layout is the
// engine's own - not canonical IT: the name sits at 0x14 and the numeric
// fields start at 0x2E, which is exactly what the AoW files contain.
struct SmpHdr {
    std::uint8_t flags = 0, volume = 0, convert = 0, pan = 0;
    std::uint32_t length = 0, loopBegin = 0, loopEnd = 0, c5speed = 8363;
    std::uint32_t sustainBegin = 0, sustainEnd = 0, ptr = 0;
    char name[27] = {};
};

SmpHdr read_smp_header(const std::uint8_t* d, std::size_t n, std::uint32_t off) {
    SmpHdr h;
    if (static_cast<std::size_t>(off) + 0x50 > n) return h;
    const std::uint8_t* p = d + off;
    h.flags = p[0x12];
    h.volume = p[0x13];
    std::memcpy(h.name, p + 0x14, 26);
    h.name[26] = 0;
    h.convert = p[0x2E];
    h.pan = p[0x2F];
    h.length = rd32(p + 0x30);
    h.loopBegin = rd32(p + 0x34);
    h.loopEnd = rd32(p + 0x38);
    h.c5speed = rd32(p + 0x3C);
    h.sustainBegin = rd32(p + 0x40);
    h.sustainEnd = rd32(p + 0x44);
    h.ptr = rd32(p + 0x48);
    return h;
}

// Port of galaxy.dll FUN_10027410: reads the raw sample and builds the "SAMP"
// record.  Stereo data is de-interleaved (left half then right half) and
// unsigned data is biased to signed, then a 32-step ramp is written into the
// padding past the loop end and a fade-in over the first 32 frames, exactly as
// the original does to keep loops click-free.
Sample decode_sample(const std::uint8_t* d, std::size_t n, const SmpHdr& h) {
    Sample s;
    s.present = true;
    s.length = h.length;
    s.loopBegin = h.loopBegin;
    s.loopEnd = h.loopEnd;
    s.c5speed = h.c5speed ? h.c5speed : 8363;
    s.sustainBegin = h.sustainBegin;
    s.sustainEnd = h.sustainEnd;

    // flags: bit1 = unsigned, bit2 = 16-bit, bit3 = loop, bit4 = pingpong,
    //        bit6 = stereo (the loader always sets bit7).
    std::uint16_t f = 0x80;
    if (h.flags & 0x02) f |= 0x04;                        // 16-bit
    if (h.flags & 0x10) f |= 0x08;                        // loop
    if ((h.flags & 0x40) | (h.pan & 0x80)) f |= 0x10;     // pingpong
    if (h.flags & 0x04) f |= 0x40;                        // stereo
    if (!(h.convert & 0x01)) f |= 0x02;                   // unsigned
    s.flags = f;
    s.volume = static_cast<std::uint16_t>(h.volume << 9);  // loader: <<9 clamp 0x7FFF
    if (s.volume > 0x7FFF) s.volume = 0x7FFF;
    s.panning = static_cast<std::uint16_t>(h.pan << 9);
    if (s.panning > 0x7FFF) s.panning = 0x7FFF;

    if (s.length == 0) return s;
    const int bytes = s.is16() ? 2 : 1;
    const int chans = s.stereo() ? 2 : 1;
    std::size_t need = static_cast<std::size_t>(s.length) * bytes * chans;
    if (static_cast<std::size_t>(h.ptr) + need > n) return s;

    s.data.assign(static_cast<std::size_t>(s.length) * bytes * chans + 0x1F, 0);
    const std::uint8_t* src = d + h.ptr;
    if (s.stereo()) {
        // de-interleave: L block then R block
        for (std::uint32_t i = 0; i < s.length; ++i) {
            for (int c = 0; c < 2; ++c) {
                std::size_t to = (static_cast<std::size_t>(c) * s.length + i) * bytes;
                std::size_t from = (static_cast<std::size_t>(i) * 2 + c) * bytes;
                for (int b = 0; b < bytes; ++b) s.data[to + b] = src[from + b];
            }
        }
    } else {
        std::memcpy(s.data.data(), src, need);
    }

    // signedness
    if (s.flags & 0x02) {  // unsigned -> signed
        if (bytes == 1) {
            for (std::size_t i = 0; i < need; ++i) s.data[i] = static_cast<std::uint8_t>(s.data[i] - 0x80);
        } else {
            for (std::size_t i = 0; i + 1 < need; i += 2) {
                std::uint16_t v = static_cast<std::uint16_t>(rd16(s.data.data() + i) - 0x8000);
                s.data[i] = static_cast<std::uint8_t>(v & 0xFF);
                s.data[i + 1] = static_cast<std::uint8_t>(v >> 8);
            }
        }
    }
    // galaxy FUN_10027410 finishes by ramping the sample: a 32-step fade-in over
    // the first 0x20 frames, plus a fade-out written into the 0x1F padding past
    // the loop end (used by the mixer's boundary weight vector).  Skipping this
    // leaves clicks and a harsher timbre than the original.
    {
        const int bytes2 = bytes;   // 1 or 2
        for (int i = 0; i < 0x20 && static_cast<std::size_t>(i + 1) * bytes2 <= s.data.size(); ++i) {
            if (bytes2 == 1) {
                std::int32_t v = static_cast<std::int8_t>(s.data[i]) * i;
                s.data[i] = static_cast<std::uint8_t>((v + ((v >> 31) & 0x1F)) >> 5);
            } else {
                std::size_t o = static_cast<std::size_t>(i) * 2;
                if (o + 1 >= s.data.size()) break;
                std::int32_t w = static_cast<std::int16_t>(s.data[o] | (s.data[o + 1] << 8));
                std::int32_t v = w * i;
                std::int16_t r = static_cast<std::int16_t>((v + ((v >> 31) & 0x1F)) >> 5);
                s.data[o] = static_cast<std::uint8_t>(r & 0xFF);
                s.data[o + 1] = static_cast<std::uint8_t>((r >> 8) & 0xFF);
            }
        }
    }
    return s;
}

}  // namespace

bool load_it(const std::uint8_t* data, std::size_t size, Song& song, std::string* error) {
    auto fail = [&](const char* m) { if (error) *error = m; return false; };
    if (size < 0xC0 || std::memcmp(data, "IMPM", 4) != 0) return fail("not an IMPM module");
    const std::uint8_t* b = data;

    song = Song{};
    song.name.assign(reinterpret_cast<const char*>(b + 4), 26);
    song.name = song.name.c_str();

    int ordNum = rd16(b + 0x20), insNum = rd16(b + 0x22);
    int smpNum = rd16(b + 0x24), patNum = rd16(b + 0x26);
    const int flags = rd16(b + 0x2C);
    const bool insMode = (flags & 4) != 0;
    song.musicVolume = std::min<int>(b[0x30], 0x7F);
    song.initialSpeed = b[0x32] ? b[0x32] : 6;
    song.initialTempo = b[0x33] ? b[0x33] : 125;

    int read = ordNum < 0x100 ? ordNum : 0xFF;
    if (read < 0 || static_cast<std::size_t>(0xC0 + read) > size) return fail("truncated orders");
    std::memcpy(song.orders, b + 0xC0, read);
    song.songLength = read;

    std::size_t pos = 0xC0 + static_cast<std::size_t>(ordNum);
    auto read_off = [&](int count, std::vector<std::uint32_t>& out) -> bool {
        out.assign(count, 0);
        for (int i = 0; i < count; ++i) {
            if (pos + 4 > size) return false;
            out[i] = rd32(b + pos);
            pos += 4;
        }
        return true;
    };
    std::vector<std::uint32_t> insOff, smpOff, patOff;
    if (!read_off(insNum, insOff) || !read_off(smpNum, smpOff) || !read_off(patNum, patOff))
        return fail("truncated offset tables");

    for (int c = 0; c < 32; ++c) {
        std::uint8_t p = b[0x40 + c];
        if (p < 0x41) {
            int v = p << 1;
            if (v > 0x7F) v = 0x7F;
            song.initialPanning[c] = static_cast<std::uint8_t>(v);
        } else {
            song.initialPanning[c] = 0xA0;   // "channel disabled" marker
        }
    }

    int len = 0;
    while (len < 0x100 && song.orders[len] != 0xFF) ++len;
    if (len < song.songLength) song.songLength = len;
    song.songLength = std::max(0, song.songLength - 1);

    // ---- samples --------------------------------------------------------
    song.samples.assign(std::max(0, smpNum), Sample{});
    for (int i = 0; i < smpNum; ++i) {
        SmpHdr h = read_smp_header(b, size, smpOff[i]);
        if (h.length == 0) continue;
        song.samples[i] = decode_sample(b, size, h);
    }

    // ---- instruments (sample mode synthesises one per sample) ------------
    song.instruments.assign(std::max(0, insMode ? insNum : smpNum), Instrument{});
    if (!insMode) {
        for (int i = 0; i < smpNum; ++i) {
            Instrument& ins = song.instruments[i];
            ins.present = song.samples[i].present;
            ins.sampleMode = true;          // galaxy: one instrument per sample,
            ins.numSamples = 1;             // note map left empty
            ins.globalVolume = 0x7FFF;
            ins.sampleSlots.push_back(i);
        }
    } else {
        for (int i = 0; i < insNum; ++i) {
            if (static_cast<std::size_t>(insOff[i]) + 0x226 > size) continue;
            const std::uint8_t* p = b + insOff[i];
            if (std::memcmp(p, "IMPI", 4) != 0) continue;
            Instrument& ins = song.instruments[i];
            ins.present = true;
            ins.fadeout = rd16(p + 0x14);
            ins.globalVolume = static_cast<std::uint16_t>(
                std::min<int>(static_cast<int>(p[0x18]) << 9, 0x7FFF));
            ins.numSamples = p[0x1E];
            std::memcpy(ins.noteMap, p + 0x40, 120);   // keyboard table
            ins.sampleSlots.resize(ins.numSamples);
            for (int s = 0; s < ins.numSamples; ++s) ins.sampleSlots[s] = ins.noteMap[s];
            // envelopes (IT raw layout: vol @0x130, pan @0x182, pitch @0x1D4;
            // 82 bytes each - flags, num, loop begin/end, sustain begin/end,
            // then 25 nodes of {int8 value, uint16 tick})
            auto parse_env = [&](int eo, Envelope& env) {
                env.present = (p[eo] & 1) != 0;
                env.num = std::min<int>(p[eo + 1], 25);
                env.loopBegin = p[eo + 2];
                env.loopEnd = p[eo + 3];
                env.sustainBegin = p[eo + 4];
                env.sustainEnd = p[eo + 5];
                for (int k = 0; k < 25; ++k) {
                    env.nodes[k].value = static_cast<std::int8_t>(p[eo + 6 + k * 3]);
                    env.nodes[k].tick = rd16(p + eo + 6 + k * 3 + 1);
                }
                if (!env.present) env.num = 0;
            };
            parse_env(0x130, ins.volEnv);
            parse_env(0x182, ins.panEnv);
            parse_env(0x1D4, ins.pitchEnv);
        }
    }

    // ---- patterns (Galaxy remember-mask decode -> internal cell stream) ---
    song.patterns.assign(std::max(0, patNum), Pattern{});
    for (int idx = 0; idx <= song.songLength; ++idx) {
        std::uint32_t pv = song.orders[idx];
        if (pv >= 0xFE || pv >= static_cast<std::uint32_t>(patNum)) continue;
        Pattern& pat = song.patterns[pv];
        if (pat.present) continue;
        std::uint32_t off = patOff.empty() ? 0 : patOff[pv];
        if (off == 0 || static_cast<std::size_t>(off) + 8 > size) continue;
        std::uint32_t plen = rd16(b + off);
        int rows = rd16(b + off + 2);
        if (rows == 0) rows = 64;
        pat.present = true;
        pat.rows = rows;
        std::size_t q = off + 8;
        const std::size_t end = std::min<std::size_t>(size, off + 8 + plen);

        std::uint8_t remMask[kMaxChannels] = {}, remNote[kMaxChannels] = {};
        for (int r = 0; r < rows && q < end; ++r) {
            while (q < end && b[q] != 0) {
                std::uint32_t cb = b[q++];
                int ch = static_cast<int>((cb - 1) & 0x3F);
                if (ch >= kMaxChannels) break;
                std::uint32_t mask;
                if ((cb & 0x80) == 0) {
                    mask = remMask[ch];
                } else {
                    if (q >= end) break;
                    mask = b[q++];
                    remMask[ch] = static_cast<std::uint8_t>(mask);
                }
                std::uint8_t cellFlags = static_cast<std::uint8_t>(ch);
                std::uint8_t note = 0, instr = 0, vol = 0, effCmd = 0, effParam = 0;
                bool hasNote = false, hasVol = false, hasEff = false;
                if (mask & 0x01) { if (q >= end) break; note = b[q++]; remNote[ch] = note; hasNote = true; }
                else if (mask & 0x10) { note = remNote[ch]; hasNote = true; }
                if (mask & 0x02) { if (q >= end) break; instr = b[q++]; hasNote = true; }
                if (mask & 0x04) { if (q >= end) break; vol = b[q++]; hasVol = true; }
                if (mask & 0x08) {
                    if (q + 1 >= end) break;
                    effCmd = b[q++];
                    effParam = b[q++];
                    hasEff = true;
                }
                if (hasEff) cellFlags |= 0x80;
                if (hasNote) cellFlags |= 0x40;
                if (hasVol) cellFlags |= 0x20;
                pat.cells.push_back(cellFlags);
                if (hasEff) { pat.cells.push_back(effParam); pat.cells.push_back(effCmd); }
                if (hasNote) { pat.cells.push_back(instr); pat.cells.push_back(note); }
                if (hasVol) pat.cells.push_back(vol);
            }
            if (q < end) ++q;
            pat.cells.push_back(0);
        }
    }

    song.playerMode = (((flags & 8) >> 3) ^ 1) | 2;
    song.musicVoices = 1;
    return true;
}

bool load_it_file(const std::string& path, Song& song, std::string* error) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { if (error) *error = "cannot open " + path; return false; }
    std::vector<std::uint8_t> d((std::istreambuf_iterator<char>(f)),
                                std::istreambuf_iterator<char>());
    return load_it(d.data(), d.size(), song, error);
}

}  // namespace galaxy
}  // namespace aow
