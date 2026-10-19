// audio_test.cpp - head-less validation of the AoW1 audio decoders.
//
// Loads the shipped .IT songs, .WAV samples and .VAG files, renders/decodes
// them and asserts the results are non-trivial.  Needs no sound device.
//
//   aow_audio_tests <game-root>
#include "aow/audio.hpp"
#include "aow/galaxy.hpp"
#include "aow/openmpt.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static int g_pass = 0;
static int g_fail = 0;

static void check(bool cond, const std::string& what) {
    if (cond) {
        ++g_pass;
    } else {
        ++g_fail;
        std::printf("FAIL: %s\n", what.c_str());
    }
}

static std::vector<std::string> find(const fs::path& root, const std::string& ext) {
    std::vector<std::string> out;
    std::error_code ec;
    if (!fs::exists(root, ec)) return out;
    for (auto it = fs::recursive_directory_iterator(
             root, fs::directory_options::skip_permission_denied, ec);
         !ec && it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (it->is_regular_file(ec) && it->path().extension() == ext)
            out.push_back(it->path().string());
    }
    return out;
}

static double rms_of(const std::vector<std::int16_t>& s) {
    if (s.empty()) return 0.0;
    double e = 0.0;
    for (std::int16_t v : s) e += static_cast<double>(v) * v;
    return std::sqrt(e / s.size());
}

int main(int argc, char** argv) {
    fs::path root = argc > 1 ? fs::path(argv[1]) : fs::current_path();

    // --- Samples: decode the shipped .WAV / .VAG sound effects ------------
    std::vector<std::string> sfx = find(root / "Sfx", ".WAV");
    for (auto& v : find(root / "Sfx", ".VAG")) sfx.push_back(v);
    std::sort(sfx.begin(), sfx.end());
    check(!sfx.empty(), "found Sfx samples");
    int loaded = 0, silent = 0, unsupported = 0, seen = 0;
    std::size_t step = std::max<std::size_t>(1, sfx.size() / 40);
    for (std::size_t i = 0; i < sfx.size() && seen < 40; i += step, ++seen) {
        aow::PcmBuffer b;
        std::string err;
        if (!aow::load_sample(sfx[i], b, &err) || b.frames() == 0) { ++unsupported; continue; }
        ++loaded;
        if (rms_of(b.samples) <= 10.0) ++silent;
    }
    check(loaded > 0, "decoded some samples");
    check(silent == 0, "decoded samples are non-silent (" + std::to_string(silent) + " silent)");
    std::printf("  samples: seen=%d loaded=%d unsupported=%d (library=%zu)\n",
                seen, loaded, unsupported, sfx.size());

    // --- IT: render every song and require real audio ---------------------
    auto songs = find(root / "Songs", ".it");
    for (auto& s : find(root / "Songs", ".IT")) songs.push_back(s);
    check(!songs.empty(), "found Songs/**/*.IT");
    int it_ok = 0;
    for (const auto& s : songs) {
        aow::galaxy::Player mod;
        std::string err;
        if (!mod.load(s, &err)) { std::printf("  load failed %s: %s\n", s.c_str(), err.c_str()); continue; }
        mod.set_loop(false);
        mod.start();
        std::vector<std::int16_t> buf(2048 * 2);
        double peak_rms = 0.0;
        int frames = 0;
        const int target = 44100 * 20;  // 20 seconds
        while (frames < target && mod.playing()) {
            mod.render(buf.data(), 2048);
            peak_rms = std::max(peak_rms, rms_of(buf));
            frames += 2048;
        }
        if (peak_rms > 20.0) ++it_ok;
        if (peak_rms < 200.0)
            std::printf("  note: quiet song %s peak_rms=%.1f\n", s.c_str(), peak_rms);
    }
    check(it_ok == static_cast<int>(songs.size()),
          "all IT songs produce audio (" + std::to_string(it_ok) + "/" +
              std::to_string(songs.size()) + ")");

    // --- IT via libopenmpt: the same songs through the reference decoder ---
    // This is the backend aow_audio uses by default; require it to load and
    // render every shipped song, and to expose title/duration metadata.
    int omp_ok = 0;
    for (const auto& s : songs) {
        aow::openmpt::Player mod;
        std::string err;
        if (!mod.load(s, &err)) {
            std::printf("  openmpt load failed %s: %s\n", s.c_str(), err.c_str());
            continue;
        }
        mod.set_loop(false);
        mod.start();
        std::vector<std::int16_t> buf(2048 * 2);
        double peak_rms = 0.0;
        int frames = 0;
        const int target = 44100 * 20;  // 20 seconds
        while (frames < target && mod.playing()) {
            mod.render(buf.data(), 2048);
            peak_rms = std::max(peak_rms, rms_of(buf));
            frames += 2048;
        }
        if (peak_rms > 20.0 && !mod.title().empty()) ++omp_ok;
        if (peak_rms < 200.0)
            std::printf("  note: quiet song (openmpt) %s peak_rms=%.1f\n", s.c_str(), peak_rms);
    }
    check(omp_ok == static_cast<int>(songs.size()),
          "libopenmpt plays all IT songs (" + std::to_string(omp_ok) + "/" +
              std::to_string(songs.size()) + ")");
    if (!songs.empty()) {
        aow::openmpt::Player mod;
        std::string err;
        const bool loaded = mod.load(songs.front(), &err);
        check(loaded, "libopenmpt loads " + songs.front());
        if (loaded) {
            check(!mod.title().empty(), "libopenmpt reports a module title");
            check(mod.duration_seconds() > 0.0, "libopenmpt reports a duration");
            check(mod.order_count() > 0, "libopenmpt reports order count");
            check(mod.module_type() == "it", "libopenmpt identifies the IT format");
        }
    }
    std::printf("  libopenmpt: %s\n", aow::openmpt::library_version().c_str());

    // --- WAV writer round-trip -------------------------------------------
    {
        aow::PcmBuffer b;
        b.sample_rate = 44100;
        b.samples = {100, -100, 200, -200, 300, -300};
        std::string err;
        fs::path tmp = fs::temp_directory_path() / "aow_audio_test.wav";
        check(aow::write_wav(tmp.string(), b, &err), "write_wav");
        aow::PcmBuffer r;
        check(aow::load_wav(tmp.string(), r, &err) && r.samples == b.samples,
              "WAV round-trip preserves samples");
        std::error_code ec;
        fs::remove(tmp, ec);
    }

    std::printf("audio tests: %d passed, %d failed (songs=%d, samples=%zu)\n",
                g_pass, g_fail, static_cast<int>(songs.size()), sfx.size());
    return g_fail == 0 ? 0 : 1;
}
