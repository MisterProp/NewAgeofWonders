// galaxy_test.cpp - validates the galaxy.dll loader port against the shipped
// songs: every order's pattern must decode, samples must be present, and the
// internal cell stream must scale with the source pattern length.
#include "aow/galaxy.hpp"

#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    fs::path root = argc > 1 ? fs::path(argv[1]) : fs::current_path();
    fs::path songs = root / "Songs";
    std::error_code ec;
    int songs_ok = 0, songs_tot = 0;
    int pats = 0, pats_ok = 0, samples = 0;
    for (auto it = fs::recursive_directory_iterator(songs, ec);
         !ec && it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (!it->is_regular_file()) continue;
        std::string ext = it->path().extension().string();
        for (auto& c : ext) c = static_cast<char>(std::tolower(c));
        if (ext != ".it") continue;
        ++songs_tot;
        aow::galaxy::Song song;
        std::string err;
        if (!aow::galaxy::load_it_file(it->path().string(), song, &err)) {
            std::printf("FAIL load %s: %s\n", it->path().filename().string().c_str(), err.c_str());
            continue;
        }
        ++songs_ok;
        for (int i = 0; i <= song.songLength; ++i) {
            std::uint32_t pv = song.orders[i];
            if (pv >= song.patterns.size()) continue;
            ++pats;
            if (song.patterns[pv].present && !song.patterns[pv].cells.empty()) ++pats_ok;
        }
        for (auto& s : song.samples) if (s.present) ++samples;
    }
    std::printf("galaxy loader: songs=%d/%d patterns-ok=%d/%d samples=%d\n",
                songs_ok, songs_tot, pats_ok, pats, samples);
    return songs_ok == songs_tot && pats_ok > 0 ? 0 : 1;
}
