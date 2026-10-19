// audio_showcase.cpp - "aow_audio": a showcase player for AoW1 music & SFX.
//
// Replays the game's own audio (see docs/05_AUDIO.md): Songs\*.IT through
// **libopenmpt** by default and Sfx\**\*.WAV / *.VAG through the sample
// decoders.  The reconstructed Galaxy player (aow::galaxy::Player) is still
// reachable with `--backend galaxy` for A/B comparison with the reference
// engine.
//
// Modes:
//   aow_audio                      interactive playlist (needs a sound device)
//   aow_audio --list               print the inventory and exit
//   aow_audio --render <outdir>    render every song to WAV (head-less,
//                                  works with no audio device)
//   aow_audio --seconds N          seconds to render per song (default 20)
//   aow_audio --sfx <name>         play one SFX and exit
//   aow_audio --backend <name>     module engine: openmpt (default) | galaxy
//   aow_audio --root <dir>         game install root (default: exe dir\..)
#include "aow/audio.hpp"
#include "aow/galaxy.hpp"
#include "aow/openmpt.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
namespace {
bool key_hit() { return _kbhit() != 0; }
int key_get() { return _getch(); }
}  // namespace
#else
namespace {
bool key_hit() { return false; }
int key_get() { return 0; }
}  // namespace
#endif

namespace fs = std::filesystem;
using aow::AudioOut;
using aow::PcmBuffer;

// Which module engine decodes the .IT/.S3M music.  libopenmpt is the default;
// the reconstructed Galaxy player stays available for comparison.
enum class Backend { OpenMpt, Galaxy };

// Both aow::openmpt::Player and aow::galaxy::Player expose the same methods, so
// the showcase can drive either through this tiny interface.
struct MusicPlayer {
    virtual ~MusicPlayer() = default;
    virtual bool load(const std::string& path, std::string* err) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool playing() const = 0;
    virtual void set_loop(bool loop) = 0;
    virtual void set_master_volume(int v) = 0;
    virtual void render(std::int16_t* out, std::size_t frames) = 0;
    virtual std::string title() const = 0;
};

template <class P>
struct MusicPlayerT : MusicPlayer {
    P p;
    explicit MusicPlayerT(int rate) : p(rate) {}
    bool load(const std::string& path, std::string* err) override { return p.load(path, err); }
    void start() override { p.start(); }
    void stop() override { p.stop(); }
    bool playing() const override { return p.playing(); }
    void set_loop(bool loop) override { p.set_loop(loop); }
    void set_master_volume(int v) override { p.set_master_volume(v); }
    void render(std::int16_t* out, std::size_t frames) override { p.render(out, frames); }
    std::string title() const override { return p.title(); }
};

static std::unique_ptr<MusicPlayer> make_player(Backend b, int rate) {
    if (b == Backend::Galaxy)
        return std::unique_ptr<MusicPlayer>(new MusicPlayerT<aow::galaxy::Player>(rate));
    return std::unique_ptr<MusicPlayer>(new MusicPlayerT<aow::openmpt::Player>(rate));
}

static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

static std::vector<std::string> list_files(const fs::path& root,
                                           const std::vector<std::string>& exts) {
    std::vector<std::string> out;
    std::error_code ec;
    if (!fs::exists(root, ec)) return out;
    for (auto it = fs::recursive_directory_iterator(
             root, fs::directory_options::skip_permission_denied, ec);
         !ec && it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (!it->is_regular_file(ec)) continue;
        std::string ext = to_lower(it->path().extension().string());
        if (std::find(exts.begin(), exts.end(), ext) != exts.end())
            out.push_back(it->path().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

static std::string file_name(const std::string& p) {
    return fs::path(p).filename().string();
}

struct Inventory {
    fs::path root;
    std::vector<std::string> songs;
    std::vector<std::string> sfx;
};

static Inventory scan(const fs::path& root) {
    Inventory inv;
    inv.root = root;
    inv.songs = list_files(root / "Songs", {".it", ".s3m"});
    inv.sfx = list_files(root / "Sfx", {".wav", ".vag"});
    return inv;
}

static void print_inventory(const Inventory& inv) {
    std::cout << "game root : " << inv.root.string() << "\n";
    std::cout << "music     : " << inv.songs.size() << " module(s)\n";
    for (std::size_t i = 0; i < inv.songs.size(); ++i)
        std::cout << "  [" << i << "] " << file_name(inv.songs[i]) << "\n";
    std::cout << "sfx       : " << inv.sfx.size() << " sample(s)\n";
    std::size_t show = std::min<std::size_t>(inv.sfx.size(), 12);
    for (std::size_t i = 0; i < show; ++i)
        std::cout << "  (" << i << ") " << file_name(inv.sfx[i]) << "\n";
    if (inv.sfx.size() > show) std::cout << "  ... and " << (inv.sfx.size() - show)
                                         << " more\n";
}

// Renders `seconds` of a song to a WAV file.  Returns frames rendered.
static std::size_t render_song(Backend backend, const std::string& path,
                               const std::string& out, double seconds,
                               std::string* err) {
    std::unique_ptr<MusicPlayer> mod = make_player(backend, 44100);
    if (!mod->load(path, err)) return 0;
    mod->set_loop(false);
    mod->start();
    const std::size_t total = static_cast<std::size_t>(44100.0 * seconds);
    PcmBuffer buf;
    buf.sample_rate = 44100;
    buf.samples.resize(total * 2);
    std::size_t done = 0;
    while (done < total && mod->playing()) {
        std::size_t n = std::min<std::size_t>(4096, total - done);
        mod->render(buf.samples.data() + done * 2, n);
        done += n;
    }
    buf.samples.resize(done * 2);
    if (!aow::write_wav(out, buf, err)) return 0;
    return done;
}

static int usage() {
    std::cout << "aow_audio - Age of Wonders audio showcase\n"
                 "  --root <dir>      game install root (default: exe dir\\..)\n"
                 "  --list            list music & sfx and exit\n"
                 "  --render <dir>    render each song to <dir>/<name>.wav\n"
                 "  --seconds N       seconds per song when rendering (default 20)\n"
                 "  --sfx <name>      play a single sfx (substring match) and exit\n"
                 "  --backend <name>  module engine: openmpt (default) | galaxy\n"
                 "  (no args)         interactive playlist\n";
    return 0;
}

static fs::path default_root(const char* argv0) {
    std::error_code ec;
    fs::path here = fs::absolute(argv0, ec).parent_path();
    for (fs::path up : {here / ".." / "..", here / "..", here}) {
        if (fs::exists(up / "Songs", ec)) return fs::weakly_canonical(up, ec);
    }
    return fs::current_path(ec);
}

static int play_sfx(const Inventory& inv, const std::string& needle) {
    std::string want = to_lower(needle);
    std::string found;
    for (const auto& s : inv.sfx)
        if (to_lower(file_name(s)).find(want) != std::string::npos) { found = s; break; }
    if (found.empty()) { std::cout << "no sfx matching '" << needle << "'\n"; return 1; }
    PcmBuffer buf;
    std::string err;
    if (!aow::load_sample(found, buf, &err)) {
        std::cout << "decode failed: " << err << "\n";
        return 1;
    }
    std::cout << "playing " << file_name(found) << "  (" << buf.frames()
              << " frames @ " << buf.sample_rate << " Hz)\n";
    AudioOut out;
    if (!out.open(buf.sample_rate, 2, 16, &err)) {
        std::cout << "audio device unavailable (" << err << ")\n";
        return 2;
    }
    out.play(buf);
    return 0;
}

static int interactive(const Inventory& inv, Backend backend) {
    if (inv.songs.empty()) { std::cout << "no songs found under " << inv.root.string() << "\n"; return 1; }
    AudioOut out;
    std::string err;
    if (!out.open(44100, 2, 16, &err)) {
        std::cout << "audio device unavailable: " << err << "\n"
                  << "Use head-less mode instead:  aow_audio --render out --seconds 20\n";
        return 2;
    }
    int idx = 0, vol = 100;
    // start on a track that is known to play (some songs still have a volume bug)
    for (std::size_t i = 0; i < inv.songs.size(); ++i) {
        std::string n = to_lower(file_name(inv.songs[i]));
        if (n == "title.it") { idx = static_cast<int>(i); break; }
    }
    std::unique_ptr<MusicPlayer> mod = make_player(backend, 44100);
    if (!mod->load(inv.songs[idx], &err)) { std::cout << "load failed: " << err << "\n"; return 1; }
    mod->set_loop(true);
    mod->set_master_volume(vol);
    mod->start();

    const int kFrames = 22050;   // 0.5 s blocks, played through AudioOut::play()
    std::vector<std::int16_t> chunk(kFrames * 2);
    std::cout << "\n== " << mod->title() << " ==   [" << file_name(inv.songs[idx]) << "]\n"
              << "keys: n next | p prev | w/s volume | r restart | f random sfx | q quit\n";
    bool quit = false;
    double meterAcc = 0.0;
    int meterPeak = 0;
    while (!quit) {
        mod->render(chunk.data(), kFrames);
        for (std::size_t i = 0; i < chunk.size(); ++i) {
            int a = chunk[i] < 0 ? -chunk[i] : chunk[i];
            if (a > meterPeak) meterPeak = a;
        }
        meterAcc += kFrames;
        if (meterAcc >= 44100) {          // once per second
            std::cout << "[level] peak=" << meterPeak << "  playing=" << mod->playing()
                      << "  device=" << out.is_open() << std::endl;
            meterPeak = 0;
            meterAcc = 0;
        }
        {
            // Stream (don't drain): write_block only reuses a buffer that has
            // reported WHDR_DONE, so this is gapless.
            out.play_raw(chunk.data(), kFrames);
        }
        while (key_hit()) {
            int k = std::tolower(key_get());
            if (k == 'q' || k == 27) { quit = true; break; }
            if (k == 'w') { vol = std::min(128, vol + 8); mod->set_master_volume(vol); std::cout << "vol " << vol << "\n"; }
            if (k == 's') { vol = std::max(0, vol - 8); mod->set_master_volume(vol); std::cout << "vol " << vol << "\n"; }
            if (k == 'r') { mod->start(); }
            if (k == 'n' || k == 'p') {
                idx = (idx + (k == 'n' ? 1 : inv.songs.size() - 1)) % static_cast<int>(inv.songs.size());
                out.stop();
                if (mod->load(inv.songs[idx], &err)) {
                    mod->set_loop(true);
                    mod->set_master_volume(vol);
                    mod->start();
                    std::cout << "\n== " << mod->title() << " ==   [" << file_name(inv.songs[idx]) << "]\n";
                }
            }
            if (k == 'f' && !inv.sfx.empty()) {
                const std::string& s = inv.sfx[static_cast<std::size_t>(std::rand()) % inv.sfx.size()];
                PcmBuffer sfx;
                if (aow::load_sample(s, sfx, &err)) {
                    std::cout << "sfx: " << file_name(s) << "\n";
                    out.play_raw(sfx.samples.data(), std::min<std::size_t>(sfx.frames(), 44100 * 2));
                }
            }
        }
    }
    out.stop();
    return 0;
}

int main(int argc, char** argv) {
    std::string root, renderDir, sfxName, backendName = "openmpt";
    bool list = false;
    double seconds = 20.0;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--root" && i + 1 < argc) root = argv[++i];
        else if (a == "--render" && i + 1 < argc) renderDir = argv[++i];
        else if (a == "--sfx" && i + 1 < argc) sfxName = argv[++i];
        else if (a == "--backend" && i + 1 < argc) backendName = to_lower(argv[++i]);
        else if (a == "--seconds" && i + 1 < argc) seconds = std::atof(argv[++i]);
        else if (a == "--list") list = true;
        else if (a == "-h" || a == "--help") return usage();
        else { std::cout << "unknown option: " << a << "\n"; return usage(); }
    }
    Backend backend = Backend::OpenMpt;
    if (backendName == "galaxy") backend = Backend::Galaxy;
    else if (backendName != "openmpt") {
        std::cout << "unknown backend: " << backendName << " (use openmpt|galaxy)\n";
        return usage();
    }
    fs::path r = root.empty() ? default_root(argv[0]) : fs::path(root);
    Inventory inv = scan(r);

    if (list) {
        std::cout << "backend   : " << backendName;
        if (backend == Backend::OpenMpt)
            std::cout << " (libopenmpt " << aow::openmpt::library_version() << ")";
        std::cout << "\n";
        print_inventory(inv);
        return 0;
    }
    if (!sfxName.empty()) return play_sfx(inv, sfxName);
    if (!renderDir.empty()) {
        std::error_code ec;
        fs::create_directories(renderDir, ec);
        if (inv.songs.empty()) { std::cout << "no songs found\n"; return 1; }
        int ok = 0;
        for (const auto& s : inv.songs) {
            std::string stem = fs::path(s).stem().string();
            std::string outp = (fs::path(renderDir) / (stem + ".wav")).string();
            std::string err;
            std::size_t n = render_song(backend, s, outp, seconds, &err);
            if (n) { std::cout << "rendered " << stem << " (" << n / 44100 << "s)\n"; ++ok; }
            else std::cout << "FAILED " << stem << ": " << err << "\n";
        }
        std::cout << "done: " << ok << "/" << inv.songs.size() << " songs\n";
        return ok ? 0 : 1;
    }
    if (inv.songs.empty() && inv.sfx.empty()) {
        std::cout << "no audio found under " << inv.root.string() << " (use --root)\n";
        return 1;
    }
    return interactive(inv, backend);
}
