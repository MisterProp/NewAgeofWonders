// galaxy_player.cpp - sequencer + note trigger + effects + mixer.
//
// Ports galaxy.dll FUN_1000d120 (sequencer), FUN_1000e8c0 (row/effect
// processor, IT path), FUN_1000f960 (volume scaling) and FUN_1000c330 (mixer).
// The voice record and pitch maths are the recovered ones (see galaxy.hpp).
#include "aow/galaxy.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace aow {
namespace galaxy {

namespace {

// galaxy's sin panning table (DAT_10074EF0): 0x8000 entries, sin(pi/2 * i/N).
inline int sinTable(int i) {
    if (i < 0) i = 0;
    if (i > 0x7FFF) i = 0x7FFF;
    return static_cast<int>(std::sin(i * (3.14159265358979323846 / 2.0) / 32767.0) * 32767.0);
}

// Note tables used by the original: standard MOD periods (DAT_100447E0) and
// the 768-entry linear frequency table (DAT_100449C0).
const std::uint16_t kNotePeriod[120] = {
    27392, 25696, 24064, 22720, 21376, 20096, 18944, 17920, 16896, 15872, 14976, 14080,
    13696, 12848, 12032, 11360, 10688, 10048,  9472,  8960,  8448,  7936,  7488,  7040,
     6848,  6424,  6016,  5680,  5344,  5024,  4736,  4480,  4224,  3968,  3744,  3520,
     3424,  3212,  3008,  2840,  2672,  2512,  2368,  2240,  2112,  1984,  1872,  1760,
     1712,  1606,  1504,  1420,  1336,  1256,  1184,  1120,  1056,   992,   936,   880,
      856,   803,   752,   710,   668,   628,   592,   560,   528,   496,   468,   440,
      428,   401,   376,   355,   334,   314,   296,   280,   264,   248,   234,   220,
      214,   200,   188,   177,   167,   157,   148,   140,   132,   124,   117,   110,
      107,   100,    94,    88,    83,    78,    74,    70,    66,    62,    58,    55,
       53,    50,    47,    44,    41,    39,    37,    35,    33,    31,    29,    27,
};

}  // namespace

struct Player::State {
    int rate = 44100;
    Song song;
    Voice v[32];

    // per-channel memories (galaxy DAT_10060xxx / DAT_1006a8xx arrays)
    std::uint8_t curNote[32] = {};
    std::uint8_t curInstr[32] = {};
    std::uint8_t curVol[32] = {};
    std::uint8_t volSlideMem[32] = {};
    std::uint8_t portaUpMem[32] = {}, portaDownMem[32] = {}, tonePortaMem[32] = {};
    std::uint8_t vibParamMem[32] = {};
    std::uint8_t retrigMem[32] = {};
    std::uint8_t offsetMem[32] = {};
    int targetNote[32] = {};

    // sequencer
    int order = 0, row = 0, tick = 0;
    int speed = 6, tempo = 125;
    int nextOrder = -1, nextRow = -1;
    bool jump = false, brk = false;
    int patternDelay = 0;
    std::size_t cellPos = 0;
    bool playing = false, loop = true;
    int startOrder = 0;
    // galaxy drives the sequencer from a 16.16 accumulator (DAT_10074eac /
    // DAT_10074d0c): acc -= samples<<16, fire a tick when acc < 0x10000.
    std::uint32_t tickLen = 882u << 16;
    std::uint32_t tickAcc = 882u << 16;
    double tickSamples = 882.0, tickCounter = 0.0;
    int masterVol = 128;
    std::uint16_t musicVol = 127;

    bool linear() const { return (song.playerMode & 1) == 0; }

    const Pattern* curPattern() const {
        if (order < 0 || order > song.songLength) return nullptr;
        std::uint32_t pv = song.orders[order];
        if (pv >= song.patterns.size() || !song.patterns[pv].present) return nullptr;
        return &song.patterns[pv];
    }

    void setTickLength() {
        // galaxy FUN_1000d120: tickLen(16.16) = rate*5 / (tempo*2)
        std::uint32_t num = static_cast<std::uint32_t>(rate) * 5u;
        std::uint32_t den = static_cast<std::uint32_t>(tempo) * 2u;
        if (den == 0) den = 1;
        tickLen = ((num / den) << 16) | (((num % den) << 16) / den);
        tickAcc = tickLen;
    }

    void resetVoices() {
        for (int i = 0; i < 32; ++i) {
            v[i] = Voice{};
            // glxStartMusic enables the music voices (voice byte +0x10 = 1);
            // the 0xA0 pan byte is only a "no panning" marker, not a disable.
            v[i].enable = 1;
            int p = song.initialPanning[i];
            if (p == 0xA0) p = 0x28;
            v[i].panning = static_cast<std::uint16_t>(p * 256);
        }
        std::memset(curNote, 0, sizeof(curNote));
        std::memset(curInstr, 0, sizeof(curInstr));
        std::memset(curVol, 0, sizeof(curVol));
        std::memset(volSlideMem, 0, sizeof(volSlideMem));
        std::memset(portaUpMem, 0, sizeof(portaUpMem));
        std::memset(portaDownMem, 0, sizeof(portaDownMem));
        std::memset(tonePortaMem, 0, sizeof(tonePortaMem));
        std::memset(vibParamMem, 0, sizeof(vibParamMem));
        std::memset(retrigMem, 0, sizeof(retrigMem));
        std::memset(offsetMem, 0, sizeof(offsetMem));
        for (int i = 0; i < 32; ++i) targetNote[i] = -1;
    }

    void start() {
        speed = song.initialSpeed > 0 ? song.initialSpeed : 6;
        tempo = song.initialTempo > 0 ? song.initialTempo : 125;
        order = startOrder;
        row = 0;
        tick = 0;
        nextOrder = -1;
        nextRow = -1;
        jump = brk = false;
        patternDelay = 0;
        cellPos = 0;
        tickCounter = 0.0;
        setTickLength();
        resetVoices();
        playing = true;
        while (order <= song.songLength && song.orders[order] >= 0xFE) ++order;
        if (order > song.songLength) { playing = false; if (loop) start(); }
    }

    void trigger(int ch, int note, int instr);
    void processTick();
    void readRow();
    void applyEffect(int ch, std::uint8_t fx, std::uint8_t param, bool tick0);
    void mixBlock(std::int16_t* out, int frames);
    void advanceOrder();
};

// ---- note trigger (galaxy FUN_1000e8c0 / FUN_1000f750) -------------------
void Player::State::trigger(int ch, int itNote, int instr) {
    if (itNote < 0 || itNote > 0x77) return;
    int gi = instr - 1;
    if (gi < 0 || gi >= static_cast<int>(song.instruments.size())) return;
    const Instrument& ins = song.instruments[gi];
    if (!ins.present) return;
    // Sample mode: galaxy maps the note through an *empty* note map; the sample
    // is the one addressed by the instrument column (one instrument per sample).
    int slot = ins.sampleMode ? gi : ins.noteMap[itNote];
    if (slot < 0 || slot >= static_cast<int>(song.samples.size())) return;
    const Sample& s = song.samples[slot];
    if (!s.present || s.data.empty()) return;

    Voice& v = this->v[ch];
    v.instrument = static_cast<std::uint8_t>(gi);
    v.note = static_cast<std::uint8_t>(itNote);
    v.sampleSlot = static_cast<std::uint8_t>(slot);
    v.noteActive = 1;
    v.volume = 0x7FFF;
    v.fadeVol = 0;
    v.envVolume = 0x7FFF;
    v.fadeVolume = 0x7FFF;
    v.envTick = 0;
    v.sampleFlags = s.flags;
    v.length = s.length;
    v.sampleLen = s.length;
    v.loopBegin = s.loopBegin;
    v.loopEnd = s.loopEnd;
    v.c5speed = s.c5speed ? s.c5speed : 8363;
    v.position = 0;
    v.posf = 0.0;
    v.dir = 1;
    v.loopCounter = 0;
    if (s.flags & 0x20) v.panning = s.panning;         // sample default panning
    // Pitch: galaxy's tables reduce exactly to the IT formula
    //   freq = c5speed * 2^((note - 60) / 12)   (both linear and amiga).
    double freq = static_cast<double>(v.c5speed) * std::pow(2.0, (itNote - 60) / 12.0);
    if (freq < 1.0) freq = 1.0;
    if (freq > rate * 0.5) freq = rate * 0.5;
    v.step = static_cast<std::int32_t>(freq / rate * 65536.0);
    if (std::getenv("AOW_MIX_DEBUG"))
        std::fprintf(stderr, "[trig] ch=%d note=%d slot=%d c5=%u len=%u fl=%04x step=%d data=%d,%d,%d,%d,%d,%d\n",
                     ch, itNote, slot, v.c5speed, v.length, s.flags, v.step,
                     static_cast<int>(s.data[0]), static_cast<int>(s.data[1]),
                     static_cast<int>(s.data[2]), static_cast<int>(s.data[3]),
                     static_cast<int>(s.data[4]), static_cast<int>(s.data[5]));
    // linear period (galaxy: P = (0x78 - note) * 0x100 with note = IT note + 1)
    v.period = static_cast<std::uint16_t>((0x78 - (itNote + 1)) * 256);
    v.noteCut = 0xFF;
}

// ---- order/pattern advance (galaxy FUN_1000d120) -------------------------
void Player::State::advanceOrder() {
    if (jump && nextOrder >= 0 && nextOrder <= song.songLength) order = nextOrder;
    else ++order;
    jump = false;
    row = brk && nextRow >= 0 ? nextRow : 0;
    brk = false;
    nextOrder = -1;
    nextRow = -1;
    tick = 0;
    cellPos = 0;
    if (order > song.songLength) {
        if (loop) order = 0;
        else { playing = false; return; }
    }
    int guard = 0;
    while (guard++ < 1024 && order <= song.songLength && song.orders[order] >= 0xFE) ++order;
    if (order > song.songLength) {
        if (loop) order = 0;
        else playing = false;
    }
}

// ---- one tick ------------------------------------------------------------
void Player::State::processTick() {
    if (tick == 0) readRow();
    else {
        for (int ch = 0; ch < 32; ++ch) {
            Voice& v = this->v[ch];
            if (!v.enable) continue;
            std::uint8_t fx = v.effect;
            if (fx) applyEffect(ch, fx, v.effectParam, false);
        }
    }
    // recompute the mixer step from the current period (galaxy: step = freq/rate)
    for (int ch = 0; ch < 32; ++ch) {
        Voice& vv = v[ch];
        if (!vv.enable || !vv.noteActive) continue;
        if (vv.noteCut != 0xFF && tick == vv.noteCut) {
            vv.noteVolume = 0;
            vv.noteActive = 0;
            continue;
        }
        double period = vv.period ? vv.period : (0x78 - (vv.note + 1)) * 256.0;
        double freq = static_cast<double>(vv.c5speed) * std::pow(2.0, (15104.0 - period) / 3072.0);
        if (freq < 1.0) freq = 1.0;
        if (freq > rate * 0.5) freq = rate * 0.5;
        vv.step = static_cast<std::int32_t>(freq / rate * 65536.0);
    }
    // volume envelope + fadeout, exactly as galaxy's mixer applies them
    for (int ch = 0; ch < 32; ++ch) {
        Voice& vv = v[ch];
        if (!vv.enable || !vv.noteActive) continue;
        const Instrument& ins = song.instruments[vv.instrument];
        if (ins.present && ins.volEnv.enabled()) {
            const Envelope& e = ins.volEnv;
            int i = 0;
            while (i < e.num - 1 && e.nodes[i + 1].tick <= vv.envTick) ++i;
            int t0 = e.nodes[i].tick, t1 = e.nodes[i + 1].tick;
            int n0 = e.nodes[i].value, n1 = e.nodes[i + 1].value;
            double f = (t1 > t0) ? static_cast<double>(vv.envTick - t0) / (t1 - t0) : 0.0;
            int val = static_cast<int>(n0 + (n1 - n0) * f);          // 0..64
            if (val < 0) val = 0;
            if (val > 64) val = 64;
            vv.envVolume = static_cast<std::uint16_t>(val * 512);
            if (vv.envTick < e.nodes[e.num - 1].tick) ++vv.envTick;
        }
        if (vv.keyOff && ins.present && ins.fadeout > 0) {
            vv.fadeVolume = vv.fadeVolume > ins.fadeout
                                ? static_cast<std::uint16_t>(vv.fadeVolume - ins.fadeout)
                                : 0;
            if (vv.fadeVolume == 0) { vv.noteActive = 0; vv.envVolume = 0; }
        }
    }
    ++tick;
    if (tick >= speed) {
        tick = 0;
        if (patternDelay > 0) --patternDelay;
        else if (brk || jump) advanceOrder();
        else ++row;
    }
}


// ---- row processing (galaxy FUN_1000e8c0, tick 0) ------------------------
void Player::State::readRow() {
    const Pattern* pat = curPattern();
    if (!pat) { advanceOrder(); pat = curPattern(); if (!pat) { playing = false; return; } }
    const std::vector<std::uint8_t>& c = pat->cells;
    if (cellPos >= c.size()) { advanceOrder(); return; }
    while (cellPos < c.size()) {
        std::uint8_t f = c[cellPos];
        if (f == 0) { ++cellPos; break; }
        int ch = f & 0x1F;
        std::size_t p = cellPos + 1;
        std::uint8_t note = 0, instr = 0, vol = 0, eff = 0, param = 0;
        bool hasNote = false, hasVol = false, hasEff = false;
        if (f & 0x80) { if (p + 1 >= c.size()) break; param = c[p]; eff = c[p + 1]; p += 2; hasEff = true; }
        if (f & 0x40) { if (p + 1 >= c.size()) break; instr = c[p]; note = c[p + 1]; p += 2; hasNote = true; }
        if (f & 0x20) { if (p >= c.size()) break; vol = c[p]; p += 1; hasVol = true; }
        cellPos = p;

        if (hasEff) { v[ch].effect = eff; v[ch].effectParam = param; }
        if (hasNote) {
            if (note) curNote[ch] = note;
            if (instr) curInstr[ch] = instr;
            if (note == 0xFF) {                 // galaxy: note off -> 0x80
                v[ch].noteActive = 0;
            } else if (note == 0xFE) {          // galaxy: note cut (vol 0)
                v[ch].noteVolume = 0;
                v[ch].noteActive = 0;
            } else if (note && curInstr[ch]) {
                if (eff == 7) targetNote[ch] = note;          // G: glide, no retrigger
                else trigger(ch, note, curInstr[ch]);
            } else if (instr) {
                // instrument-only cell: change the instrument, do NOT retrigger
                // (galaxy only triggers on a note; retriggering here made notes
                // repeat excessively)
            }
        }
        if (hasVol && vol <= 64) {
            curVol[ch] = vol;
            v[ch].volume = static_cast<std::uint16_t>(vol * 512);   // 0..64 -> 0..0x7FFF
        }
        std::uint8_t fx = hasEff ? eff : v[ch].effect;
        std::uint8_t fp = hasEff ? param : v[ch].effectParam;
        if (fx) applyEffect(ch, fx, fp, true);
    }
}

// ---- effects (raw IT letters; galaxy's internal mapping applied here) -----
void Player::State::applyEffect(int ch, std::uint8_t it, std::uint8_t p, bool tick0) {
    if (it < 1 || it > 26) return;
    Voice& v = this->v[ch];
    auto slide = [&](double units) { v.period = static_cast<std::uint16_t>(
        std::max(0.0, std::min(65535.0, v.period + units))); };
    switch (it) {
        case 1:  // A - set speed
            if (tick0 && p >= 1 && p <= 0x20) { speed = p; setTickLength(); }
            break;
        case 2:  // B - position jump
            if (tick0) { nextOrder = p; jump = true; }
            break;
        case 3:  // C - pattern break
            if (tick0) { brk = true; nextRow = (p >> 4) * 10 + (p & 0x0F); }
            break;
        case 4: {  // D - volume slide (operates on the 0..0x7FFF note volume)
            if (p) volSlideMem[ch] = p;
            std::uint8_t x = volSlideMem[ch];
            int d = 0;
            if (tick0) {
                if ((x & 0xF0) == 0xF0) d = (x & 0x0F) * 512;        // DxF fine up
                else if ((x & 0x0F) == 0x0F) d = -(x >> 4) * 512;    // DFy fine down
            } else if ((x & 0xF0) != 0xF0 && (x & 0x0F) != 0x0F) {
                d = (x & 0xF0) ? (x >> 4) * 512 : -(x & 0x0F) * 512;
            }
            if (d) {
                int nv = static_cast<int>(v.volume) + d;
                v.volume = static_cast<std::uint16_t>(nv < 0 ? 0 : nv > 0x7FFF ? 0x7FFF : nv);
            }
            break;
        }
        case 6:    // F - portamento up
        case 5: {  // E - portamento down
            bool up = (it == 6);
            if (p) { if (up) portaUpMem[ch] = p; else portaDownMem[ch] = p; }
            int x = up ? portaUpMem[ch] : portaDownMem[ch];
            double amt = 0;
            if (tick0) {
                if ((x & 0xF0) == 0xF0) amt = (x & 0x0F) * 4.0;
                else if ((x & 0xF0) == 0xE0) amt = (x & 0x0F);
            } else if ((x & 0xF0) != 0xF0 && (x & 0xF0) != 0xE0) {
                amt = x * 4.0;
            }
            if (amt != 0) slide(up ? -amt : amt);
            break;
        }
        case 7:  // G - tone portamento
            if (p) tonePortaMem[ch] = p;
            if (targetNote[ch] >= 0 && v.noteActive) {
                double tgt = static_cast<double>(0x78 - (targetNote[ch] + 1)) * 256.0;
                double amt = tonePortaMem[ch] * 4.0;
                if (!tick0) {
                    if (v.period > tgt) v.period = static_cast<std::uint16_t>(std::max(tgt, v.period - amt));
                    else v.period = static_cast<std::uint16_t>(std::min(tgt, v.period + amt));
                }
            }
            break;
        case 10:  // J - arpeggio
            if (tick0) { v.effect = 10; v.effectParam = p; }
            break;
        case 15:  // O - sample offset
            if (tick0) {
                offsetMem[ch] = p;
                v.posf = static_cast<double>(p) * 256.0;
                v.position = static_cast<std::uint32_t>(v.posf);
            }
            break;
        case 17:  // Q - retrigger
            if (p) retrigMem[ch] = p;
            if (!tick0 && (retrigMem[ch] & 0x0F) && (tick % (retrigMem[ch] & 0x0F)) == 0)
                v.position = 0;
            break;
        case 19: {  // S - special
            int sub = p >> 4, val = p & 0x0F;
            if (tick0) {
                if (sub == 8) v.panning = static_cast<std::uint16_t>(val * 0x7FFF / 15);  // S8x panning
                else if (sub == 0xC) v.noteCut = static_cast<std::uint8_t>(val);
                else if (sub == 0xE) patternDelay = val;
                else if (sub == 0x6) patternDelay = val;
            }
            break;
        }
        case 20:  // T - set tempo
            if (tick0 && p >= 0x20) { tempo = p; setTickLength(); }
            break;
        case 24:  // X - set panning
            if (tick0 && p <= 64) v.panning = static_cast<std::uint16_t>(p * 0x7FFF / 64);
            break;
        default:
            break;
    }
}

// ---- mixer (galaxy FUN_1000c330) -----------------------------------------
void Player::State::mixBlock(std::int16_t* out, int frames) {
    static long dbgCount = 0;
    if (std::getenv("AOW_MIX_DEBUG") && (dbgCount++ < 8)) {
        for (int ch = 0; ch < 4; ++ch) {
            const Voice& vv = v[ch];
            std::fprintf(stderr, "[mix] ch=%d en=%d act=%d pos=%u step=%d vol=%u env=%u pan=%u\n",
                         ch, vv.enable, vv.noteActive, vv.position, vv.step,
                         vv.volume, vv.envVolume, vv.panning);
        }
    }
    for (int f = 0; f < frames; ++f) {
        double l = 0.0, r = 0.0;
        for (int ch = 0; ch < 32; ++ch) {
            Voice& vv = v[ch];
            if (!vv.enable || !vv.noteActive) continue;
            const Sample& s = song.samples[vv.sampleSlot];
            if (!s.present || s.data.empty() || s.length == 0) { vv.noteActive = 0; continue; }
            if (vv.posf >= s.length) {
                if (s.looping() && s.loopEnd > s.loopBegin && s.loopEnd <= s.length)
                    vv.posf = static_cast<double>(s.loopBegin);
                else { vv.noteActive = 0; continue; }
            }
            const int bytes = s.is16() ? 2 : 1;
            auto fetch = [&](int c, std::uint32_t i) -> double {
                std::size_t off = (static_cast<std::size_t>(c) * s.length + i) * bytes;
                if (off + static_cast<std::size_t>(bytes) > s.data.size()) return 0.0;
                if (bytes == 1) return static_cast<double>(static_cast<std::int8_t>(s.data[off])) / 128.0;
                std::int16_t w = static_cast<std::int16_t>(s.data[off] | (s.data[off + 1] << 8));
                return static_cast<double>(w) / 32768.0;
            };
            const std::uint32_t idx = static_cast<std::uint32_t>(vv.posf);
            const double frac = vv.posf - idx;
            const std::uint32_t idx1 = (idx + 1 < s.length) ? idx + 1 : idx;
            auto lerp = [&](int c) -> double {
                double a = fetch(c, idx), b = fetch(c, idx1);
                return a + (b - a) * frac;
            };
            const double sl = lerp(0);
            const double sr = s.stereo() ? lerp(1) : sl;
            // galaxy FUN_1000c330 gain chain, verbatim:
            //   ((((((v+0x12 * gvol * gvol * 2)>>15) * v+0x18)>>15) * v+0x1A)>>15)
            //     * v+0x24>>15) * v+0x2A>>15) * v+0x6C>>15
            // v+0x12 is the per-voice volume, 20000 after glxResetMusic.
            const unsigned voiceVol = 20000;
            unsigned g = (voiceVol * static_cast<unsigned>(musicVol) * musicVol * 2u) >> 15;
            g = (g * vv.volume) >> 15;                                  // +0x18 note volume
            g = (g * (static_cast<unsigned>(vv.channelVolume) * 512u)) >> 15;  // +0x1A
            g = (g * vv.envVolume) >> 15;                               // +0x24 envelope
            g = (g * vv.fadeVolume) >> 15;                              // +0x2A fade
            g = (g * 0x4000u) >> 15;                                    // +0x66 (pan-env output)
            g = (g * static_cast<unsigned>(masterVol) * 256u) >> 15;    // user volume
            int pan = vv.panning;                       // already 0..0x7FFF
            if (pan < 0) pan = 0;
            if (pan > 0x7FFF) pan = 0x7FFF;
            // galaxy passes sinTable[0x7FFF-pan] as the first output gain and
            // sinTable[pan] as the second (see FUN_1000c330's mixer call).
            double lg = (sinTable(0x7FFF - pan) * static_cast<double>(g)) / 32768.0 / 32768.0;
            double rg = (sinTable(pan) * static_cast<double>(g)) / 32768.0 / 32768.0;
            l += sl * lg;
            r += sr * rg;
            vv.posf += (vv.dir < 0 ? -vv.step : vv.step) / 65536.0;
            if (s.looping() && s.loopEnd > s.loopBegin && s.loopEnd <= s.length) {
                if (s.pingpong()) {
                    if (vv.dir > 0 && vv.posf >= s.loopEnd) {
                        vv.posf = 2.0 * s.loopEnd - vv.posf;
                        vv.dir = -1;
                    } else if (vv.dir < 0 && vv.posf <= s.loopBegin) {
                        vv.posf = 2.0 * s.loopBegin - vv.posf;
                        vv.dir = 1;
                    }
                } else if (vv.posf >= s.loopEnd) {
                    vv.posf = s.loopBegin + (vv.posf - s.loopEnd);
                }
            }
            vv.position = static_cast<std::uint32_t>(vv.posf);
        }
        auto clip = [](double x) {
            int v = static_cast<int>(x * 32767.0);
            return static_cast<std::int16_t>(v < -32768 ? -32768 : v > 32767 ? 32767 : v);
        };
        out[f * 2] = clip(l);
        out[f * 2 + 1] = clip(r);
    }
}

void Player::render(std::int16_t* out, std::size_t frames) {
    State& st = *s_;
    if (!st.playing) {
        std::memset(out, 0, frames * 2 * sizeof(std::int16_t));
        return;
    }
    while (frames > 0) {
        if (st.tickAcc < 0x10000u) {
            st.processTick();
            st.tickAcc += st.tickLen;
            if (!st.playing) {
                std::memset(out, 0, frames * 2 * sizeof(std::int16_t));
                return;
            }
        }
        std::uint32_t whole = (st.tickAcc - 0x10000u) >> 16;   // samples left in this tick
        std::size_t n = std::min<std::size_t>(frames, whole ? whole : 1);
        st.mixBlock(out, static_cast<int>(n));
        st.tickAcc -= static_cast<std::uint32_t>(n << 16);
        out += n * 2;
        frames -= n;
    }
}

Player::Player(int sampleRate) : s_(new State) { s_->rate = sampleRate > 0 ? sampleRate : 44100; }
Player::~Player() = default;

bool Player::load(const std::string& path, std::string* error) {
    return galaxy::load_it_file(path, s_->song, error);
}
void Player::start() { s_->start(); }
void Player::stop() { s_->playing = false; }
bool Player::playing() const { return s_->playing; }
void Player::set_loop(bool loop) { s_->loop = loop; }
void Player::set_start_order(int order) { s_->startOrder = order < 0 ? 0 : order; }
void Player::set_master_volume(int v) { s_->masterVol = std::max(0, std::min(128, v)); }
const Song& Player::song() const { return s_->song; }
const std::string& Player::title() const { return s_->song.name; }

}  // namespace galaxy
}  // namespace aow
