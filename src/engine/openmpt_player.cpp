// openmpt_player.cpp - libopenmpt-backed module player.
//
// Part of the AoW1 C++ port.  See include/aow/openmpt.hpp and docs/05_AUDIO.md.
//
// The game's soundtrack is ordinary Impulse-Tracker (.IT) / Scream-Tracker
// (.S3M) music; rather than rely on our reconstructed Galaxy player we hand the
// files to libopenmpt, which decodes them faithfully.  This file is the whole
// adapter: it keeps the same shape as aow::galaxy::Player so callers can switch
// backends without edits.
#include "aow/openmpt.hpp"

#include <libopenmpt/libopenmpt.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

namespace aow {
namespace openmpt {
namespace {

void set_err(std::string* e, const std::string& msg) {
    if (e) *e = msg;
}

// File name without directory or extension (the fallback song title).
std::string stem_of(const std::string& path) {
    std::size_t slash = path.find_last_of("/\\");
    std::string name = (slash == std::string::npos) ? path : path.substr(slash + 1);
    std::size_t dot = name.find_last_of('.');
    return (dot == std::string::npos) ? name : name.substr(0, dot);
}

// Galaxy volumes are 0..128 with 128 = unity.  libopenmpt's master gain is in
// milliBel with 0 = unity, so map with 2000*log10(vol/128).
std::int32_t volume_to_millibel(int vol) {
    if (vol <= 0) return -10000;  // -100 dB: effectively silent
    if (vol >= 128) return 0;     // unity
    return static_cast<std::int32_t>(std::lround(2000.0 * std::log10(vol / 128.0)));
}

}  // namespace

struct Player::State {
    int rate = 44100;
    openmpt_module* mod = nullptr;
    bool playing = false;
    bool loop = true;
    int startOrder = 0;
    int volume = 128;  // Galaxy scale (128 = unity)
    std::string path, title, type;
    double duration = -1.0;
    int orders = 0;

    void apply_volume() {
        if (!mod) return;
        openmpt_module_set_render_param(
            mod, OPENMPT_MODULE_RENDER_MASTERGAIN_MILLIBEL, volume_to_millibel(volume));
    }
    void apply_repeat() {
        if (!mod) return;
        // repeat_count -1 = loop forever, 0 = play once and stop at the end.
        openmpt_module_set_repeat_count(mod, loop ? -1 : 0);
    }
};

Player::Player(int sampleRate) : s_(new State) {
    s_->rate = sampleRate > 0 ? sampleRate : 44100;
}

Player::~Player() {
    if (s_->mod) openmpt_module_destroy(s_->mod);
}

bool Player::load(const std::string& path, std::string* error) {
    State& st = *s_;
    st.playing = false;
    if (st.mod) {
        openmpt_module_destroy(st.mod);
        st.mod = nullptr;
    }
    st.title.clear();
    st.type.clear();
    st.duration = -1.0;
    st.orders = 0;
    st.path = path;

    std::ifstream f(path, std::ios::binary);
    if (!f) {
        set_err(error, "cannot open " + path);
        return false;
    }
    std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(f)),
                                   std::istreambuf_iterator<char>());
    if (data.empty()) {
        set_err(error, "empty file: " + path);
        return false;
    }

    int err = OPENMPT_ERROR_OK;
    const char* errmsg = nullptr;
    st.mod = openmpt_module_create_from_memory2(data.data(), data.size(),
                                                nullptr, nullptr, nullptr, nullptr,
                                                &err, &errmsg, nullptr);
    if (!st.mod) {
        std::string msg = errmsg ? errmsg : "unknown libopenmpt error";
        set_err(error, "libopenmpt: " + msg + " (" + path + ")");
        if (errmsg) openmpt_free_string(errmsg);
        return false;
    }

    // Metadata strings are UTF-8 and owned by libopenmpt: copy then free.
    const char* t = openmpt_module_get_metadata(st.mod, "title");
    st.title = (t && *t) ? t : stem_of(path);
    if (t) openmpt_free_string(t);
    const char* ty = openmpt_module_get_metadata(st.mod, "type");
    st.type = ty ? ty : "";
    if (ty) openmpt_free_string(ty);
    st.duration = openmpt_module_get_duration_seconds(st.mod);
    st.orders = openmpt_module_get_num_orders(st.mod);

    st.apply_volume();
    st.apply_repeat();
    return true;
}

void Player::start() {
    State& st = *s_;
    if (!st.mod) return;
    st.apply_repeat();
    st.apply_volume();
    // Start at the requested order, row 0 (ignored if out of range).
    openmpt_module_set_position_order_row(st.mod, st.startOrder, 0);
    st.playing = true;
}

void Player::stop() { s_->playing = false; }

bool Player::playing() const { return s_->playing; }

void Player::set_loop(bool loop) {
    State& st = *s_;
    st.loop = loop;
    st.apply_repeat();
}

void Player::set_start_order(int order) { s_->startOrder = order < 0 ? 0 : order; }

void Player::set_master_volume(int v) {
    State& st = *s_;
    st.volume = std::max(0, std::min(128, v));
    st.apply_volume();
}

void Player::render(std::int16_t* out, std::size_t frames) {
    State& st = *s_;
    if (!st.mod || !st.playing || frames == 0) {
        if (frames) std::memset(out, 0, frames * 2 * sizeof(std::int16_t));
        return;
    }
    std::size_t done = 0;
    while (done < frames) {
        std::size_t n = openmpt_module_read_interleaved_stereo(
            st.mod, st.rate, frames - done, out + done * 2);
        if (n == 0) {  // end of song
            st.playing = false;
            std::memset(out + done * 2, 0, (frames - done) * 2 * sizeof(std::int16_t));
            return;
        }
        done += n;
    }
}

const std::string& Player::title() const { return s_->title; }
const std::string& Player::module_type() const { return s_->type; }
double Player::duration_seconds() const { return s_->duration; }
int Player::order_count() const { return s_->orders; }

std::string library_version() {
    const char* v = openmpt_get_string("library_version");
    std::string out = v ? v : "";
    if (v) openmpt_free_string(v);
    return out;
}

}  // namespace openmpt
}  // namespace aow
