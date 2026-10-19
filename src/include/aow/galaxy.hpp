// galaxy.hpp - the "Galaxy Sound System" audio engine, reimplemented.
//
// Faithful modern-C++ port of the engine the 1999 game used: the audio logic
// lives in `galaxy.dll` (loaded at runtime by SoundP.dpl).  This header holds
// the recovered state model; galaxy_it.cpp ports the Impulse-Tracker loader
// (galaxy.dll FUN_10020290), galaxy_seq.cpp the sequencer (FUN_1000d120),
// galaxy_fx.cpp the effect processor (FUN_1000e8c0) and galaxy_mix.cpp the
// voice mixer.
//
// References below are to the decompiled routines in re/exports/Galaxy/.
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace aow {
namespace galaxy {

constexpr int kMaxChannels = 64;   // glxChannels / voice slots
constexpr int kMaxMusic = 32;      // glxMusicVoices ceiling

// One decoded sample ("SAMP" record, galaxy.dll FUN_10027410).  Audio is
// stored de-interleaved - left half then right half for stereo - exactly as
// the original does.
struct Sample {
    bool present = false;
    std::uint16_t flags = 0;    // +0x0B: bit1 unsigned, bit2 16-bit, bit3 loop,
                                //        bit4 pingpong, bit6 stereo, bit7 set
    std::uint32_t length = 0;   // frames per channel
    std::uint32_t loopBegin = 0, loopEnd = 0;
    std::uint32_t c5speed = 8363;
    std::uint16_t volume = 0;   // +0x28 default volume (0..0x7FFF)
    std::uint16_t panning = 0;  // +0x26
    std::uint32_t sustainBegin = 0, sustainEnd = 0;
    bool is16() const { return (flags & 0x04) != 0; }
    bool stereo() const { return (flags & 0x40) != 0; }
    bool looping() const { return (flags & 0x08) != 0; }
    bool pingpong() const { return (flags & 0x10) != 0; }
    std::vector<std::uint8_t> data;  // length*channels*(is16?2:1) bytes
};

// Envelope (galaxy instrument +0x0E volume / +0x3E panning / +0x6E pitch).
struct Envelope {
    bool present = false;
    int num = 0;             // node count
    int loopBegin = 0, loopEnd = 0, sustainBegin = 0, sustainEnd = 0;
    struct Node { int value = 0; int tick = 0; };
    Node nodes[25];
    int fadeout = 0;         // +0x3C / +0x6C / +0x9C
    bool enabled() const { return present && num >= 2; }
};

// One instrument ("INS" record).  Sample-mode modules synthesise one
// instrument per sample with a 1:1 note map, as the original loader does.
struct Instrument {
    bool present = false;
    bool sampleMode = false;          // galaxy FUN_10020290 sample branch
    std::uint16_t fadeout = 0;
    std::uint16_t globalVolume = 0;   // +0xE6
    std::uint8_t noteMap[120] = {};   // +0x2A : note -> sample slot
    int numSamples = 0;               // +0x148
    std::vector<int> sampleSlots;     // +0x14A : numSamples entries (index into song.samples)
    Envelope volEnv, panEnv, pitchEnv;
};

// Patterns are kept in Galaxy's internal cell stream: byte 0 of each cell is
// (channel | flags) with 0x80 = effect, 0x40 = note+instrument, 0x20 = volume;
// payloads follow in that order; 0x00 ends a row.
struct Pattern {
    bool present = false;
    int rows = 64;
    std::vector<std::uint8_t> cells;
};

struct Song {
    std::string name;                  // glxSongName
    std::uint8_t orders[256] = {};
    int songLength = 0;                // glxSongLength
    int initialSpeed = 6;              // glxInitialSpeed  (0x30 default in the DLL)
    int initialTempo = 125;            // glxInitialTempo  (0x78 default in the DLL)
    int musicVolume = 127;             // glxMusicVolume
    int playerMode = 2;                // glxPlayerMode (bit1 = linear/IT)
    std::uint8_t initialPanning[kMaxMusic] = {};
    int musicVoices = 0;               // glxMusicVoices

    std::vector<Sample> samples;
    std::vector<Instrument> instruments;
    std::vector<Pattern> patterns;
};

// Ports galaxy.dll FUN_10020290 (the "IMPM" branch of glxLoadMusic).
// Returns true on success; `error` receives a message on failure.
bool load_it(const std::uint8_t* data, std::size_t size, Song& song,
             std::string* error = nullptr);
bool load_it_file(const std::string& path, Song& song, std::string* error = nullptr);

// One playback voice - the engine's 160-byte per-voice record.  Offsets and
// semantics recovered from galaxy.dll (FUN_1000e8c0 note trigger,
// FUN_1000f750 restart, FUN_1000f960 volume scaling, glxResetMusic defaults).
// Several overlapping arrays inside the DLL are just typed views of this one
// record (byte arrays at {base + voice*0xA0}, int views at +voice*0x28 ints,
// short views at +voice*0x50 shorts).
struct Voice {
    std::uint8_t instrument = 0;    // +0x0C  current instrument (glxInstruments index)
    std::uint8_t note = 0;          // +0x0D  current note (Galaxy note-1)
    std::uint8_t sampleSlot = 0;    // +0x0E  instrument sample slot
    std::uint8_t noteActive = 0;    // +0x0F  "note running" flag
    std::uint8_t enable = 0;        // +0x10  channel enable (glxStartMusic sets 1)

    std::uint16_t period = 0xBC;    // +0x0C..  glxResetMusic sets 0x00BC
    std::uint16_t volEnvState = 20000;   // +0x12
    std::uint16_t panEnvState = 0x4000;  // +0x14
    std::uint16_t pitchEnvState = 0x4000;// +0x16
    std::uint16_t volume = 0x7FFF;       // +0x1A  per-note volume (0..0x7FFF)
    std::uint16_t fadeVol = 0;           // +0x1C
    std::uint16_t panning = 0x28;        // +0x1E
    std::uint8_t noteVolume = 255;       // +0x3F  volume-column scale (0..255)
    std::uint8_t channelVolume = 64;     // +0x41

    int instrumentPtr = 0;          // +0x20  -> instrument record
    int samplePtr = 0;              // +0x42  -> sample record
    std::uint32_t sampleLen = 0;    // +0x46
    std::uint16_t loopCounter = 0;  // +0x4A
    std::uint16_t sampleFlags = 0;  // +0x4C
    std::uint32_t position = 0;     // +0x4E  current frame index
    double posf = 0.0;              // fixed-point play cursor (galaxy keeps 16.16)
    int dir = 1;                    // +pitch direction (pingpong loops)
    std::uint32_t loopBegin = 0;    // +0x52
    std::uint32_t loopEnd = 0;      // +0x56
    std::uint32_t length = 0;       // +0x5A
    std::uint32_t c5speed = 0;      // +0x5E
    std::int32_t step = 1 << 16;    // +0x62  16.16 fixed-point mixer step

    std::uint16_t env1 = 0;         // +0x70..  volume/pan/pitch envelope cursors
    std::uint16_t env2 = 0;
    std::uint16_t envVolume = 0x7FFF;   // volume-envelope output (galaxy +0x24)
    int envTick = 0;                    // volume-envelope tick
    std::uint16_t fadeVolume = 0x7FFF;  // fadeout counter (galaxy +0x2A)
    bool keyOff = false;                // note-off seen (galaxy: byte 0x0C bit7)
    std::uint8_t sampleFlagsCopy = 0;   // +0x68
    std::uint8_t effect = 0;        // +0x86  current internal effect
    std::uint8_t effectParam = 0;   // +0x87
    std::uint8_t noteCut = 0xFF;    // SDx / SCx pending tick
};

// Galaxy's internal effect numbers (what FUN_10020290 maps the IT letters to).
enum : std::uint8_t {
    FX_ARPEGGIO = 0x00, FX_PORTA_UP = 0x01, FX_PORTA_DOWN = 0x02,
    FX_TONE_PORTA = 0x03, FX_VIBRATO = 0x04, FX_TONE_PORTA_VOLSLIDE = 0x05,
    FX_VIBRATO_VOLSLIDE = 0x06, FX_TREMOLO = 0x07, FX_SET_PANNING = 0x08,
    FX_OFFSET = 0x09, FX_VOLSLIDE = 0x0A, FX_POSITION_JUMP = 0x0B,
    FX_PATTERN_BREAK = 0x0D, FX_EXTENDED = 0x0E, FX_SPEED_TEMPO = 0x0F,
};

// Live player: sequencer (FUN_1000d120) + note trigger/effects (FUN_1000e8c0)
// + voices/envelopes + mixer (FUN_1000c330).  Renders interleaved stereo
// 16-bit PCM at the given rate.
class Player {
public:
    explicit Player(int sampleRate = 44100);
    ~Player();
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    bool load(const std::string& path, std::string* error = nullptr);
    void start();
    void stop();
    bool playing() const;
    void set_loop(bool loop);
    void set_start_order(int order);
    void set_master_volume(int vol0_128);   // 0..128
    void render(std::int16_t* out, std::size_t frames);

    const Song& song() const;
    const std::string& title() const;

private:
    struct State;
    std::unique_ptr<State> s_;
};

}  // namespace galaxy
}  // namespace aow
