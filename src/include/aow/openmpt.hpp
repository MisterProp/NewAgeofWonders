// openmpt.hpp - libopenmpt-backed module player for the AoW1 port.
//
// The 1999 game plays its soundtrack (Songs\*.IT, Impulse Tracker) through
// galaxy.dll, the "Galaxy Sound System".  We recovered and ported that engine
// (aow/galaxy.hpp) instruction-by-instruction, but the .IT files themselves are
// standard tracker modules and can be played by a battle-tested library instead:
// **libopenmpt** (https://lib.openmpt.org), the reference module decoder behind
// OpenMPT.
//
// This header exposes `aow::openmpt::Player`, a thin wrapper over the libopenmpt
// C API (libopenmpt/libopenmpt.h) that deliberately mirrors the interface of
// `aow::galaxy::Player`:
//
//     load / start / stop / playing / set_loop / set_start_order
//     set_master_volume / render / title
//
// so the showcase and the title screen can pick a backend without changing any
// call site.  Unlike the Galaxy port (which is stdlib-only and cross-platform),
// this backend links libopenmpt and therefore pulls in the library's own runtime
// DLLs next to the executable (see build.ps1 / docs/05_AUDIO.md).
#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace aow {
namespace openmpt {

// Live module player backed by libopenmpt.  Renders interleaved stereo 16-bit
// PCM at the sample rate given to the constructor (the game's "sound quality"
// is 48000 Hz; the showcase/menu use 44100).
class Player {
public:
    explicit Player(int sampleRate = 44100);
    ~Player();
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    // Loads an .IT / .S3M / .MOD / ... module from `path`.  Returns true on
    // success; `error` receives the libopenmpt error message on failure.
    bool load(const std::string& path, std::string* error = nullptr);

    void start();       // (re)start playback from the beginning / start order
    void stop();        // silence immediately
    bool playing() const;

    void set_loop(bool loop);           // loop forever vs. stop at the end
    void set_start_order(int order);    // 0-based order to start from
    void set_master_volume(int vol0_128);// 0..128, 128 = unity (Galaxy scale)

    // Renders `frames` stereo frames (L,R interleaved).  At the end of a
    // non-looping song the tail is filled with silence and playing() goes false.
    void render(std::int16_t* out, std::size_t frames);

    const std::string& title() const;      // module title (metadata "title")
    const std::string& module_type() const;// e.g. "it", "s3m"
    double duration_seconds() const;       // -1 when unknown
    int order_count() const;

private:
    struct State;
    std::unique_ptr<State> s_;
};

// Returns the linked libopenmpt library version string (e.g. "0.8.3").
std::string library_version();

}  // namespace openmpt
}  // namespace aow
