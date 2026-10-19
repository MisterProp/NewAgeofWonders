// audio.hpp - dependency-free audio playback for the AoW1 port.
//
// This re-implements, in modern C++, the audio path the 1999 game used.  The
// original stack (recovered with Ghidra, see docs/05_AUDIO.md) is:
//
//     game code  ->  SoundP.dpl  ->  galaxy.dll (Galaxy Sound System 4.00)
//     Sound.PlaySample / PlaySongEx   glxLoadSample / glxStartSample
//                                     glxLoadMusic  / glxStartMusic
//
// galaxy.dll is a tracker/module player: it streams Impulse-Tracker music
// (Songs\*.IT) and mixes WAV samples (Sfx\*.WAV).  The decoders here cover the
// three audio formats shipped with the game:
//
//     * RIFF/WAVE PCM       - Sfx\**\*.WAV   (8/16-bit, mono/stereo)
//     * Impulse Tracker     - Songs\*.IT     (IT 2.14/2.15, sample+instrument)
//     * PlayStation VAG     - Sfx\Kain\*.VAG (4-bit ADPCM, 44.1 kHz)
//
// Output uses the WinMM waveOut API, i.e. the same "Windows wave output driver"
// family the original Galaxy engine listed, so no third-party library is
// required (the port is stdlib-only by policy).
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace aow {

// A block of interleaved 16-bit stereo PCM.
struct PcmBuffer {
    int sample_rate = 44100;
    std::vector<std::int16_t> samples;  // interleaved L,R

    std::size_t frames() const { return samples.size() / 2; }
    void clear() { samples.clear(); }
};

// --- Decoders -------------------------------------------------------------

// Loads a RIFF/WAVE PCM file (8/16-bit, mono or stereo) as stereo 16-bit.
bool load_wav(const std::string& path, PcmBuffer& out, std::string* error = nullptr);

// Loads a PlayStation VAG (4-bit ADPCM) file as stereo 16-bit.
bool load_vag(const std::string& path, PcmBuffer& out, std::string* error = nullptr);

// Loads any sample by sniffing the magic: RIFF/WAVE or PlayStation VAG.
// (The game ships files named *.VAG that are in fact RIFF/WAVE.)
bool load_sample(const std::string& path, PcmBuffer& out, std::string* error = nullptr);

// Writes a PcmBuffer as a 16-bit stereo RIFF/WAVE file.
bool write_wav(const std::string& path, const PcmBuffer& in, std::string* error = nullptr);

// --- Impulse Tracker module player ---------------------------------------
//
// A faithful re-implementation of the Galaxy Sound System module player:
// orders/patterns, speed/tempo, sample & instrument mode, volume/panning
// envelopes, and the IT effect set (A,B,C,D,E,F,G,H,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y).
// The Impulse-Tracker player this header used to declare (class ItModule) has
// been retired.  Music is now decoded either by **libopenmpt** (aow/openmpt.hpp,
// class aow::openmpt::Player - the default backend) or by the faithful galaxy.dll
// port (aow/galaxy.hpp, class aow::galaxy::Player, selectable for A/B).

// --- Live output (WinMM waveOut) -----------------------------------------
class AudioOut {
public:
    AudioOut();
    ~AudioOut();
    AudioOut(const AudioOut&) = delete;
    AudioOut& operator=(const AudioOut&) = delete;

    bool open(int sample_rate = 44100, int channels = 2, int bits = 16,
              std::string* error = nullptr);
    void close();
    bool is_open() const;

    // Plays a whole buffer, waiting until the device has drained it.
    void play(const PcmBuffer& buf);
    void play_raw(const std::int16_t* data, std::size_t frames);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace aow
