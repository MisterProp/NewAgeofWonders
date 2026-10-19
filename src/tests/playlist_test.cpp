// playlist_test.cpp - the scenario music playlist (Stage 3).
//
// Verifies the port of the game's music player against the shipped data:
//   * aow::scenario_playlist builds the in-game song list exactly as AoW.exe
//     FUN_00454d2c does (the tag-0x3c list wins, else tag 0x1d with missing
//     files dropped, else the default "Songs\Heartwoo.it"), and
//   * aow::PlaylistPlayer walks that list in order and wraps back to the first
//     song at the end (Sound.TSongPlayer.Update), so every song is heard.
//
//   aow_playlist_tests [game-root]        (default: ..)
//
// Head-less: libopenmpt decodes the songs but nothing is played. stdlib +
// libopenmpt only.
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "aow/map_types.hpp"
#include "aow/playlist.hpp"

namespace fs = std::filesystem;

namespace {

int g_fail = 0;
int g_pass = 0;

void check(bool ok, const std::string& what) {
    if (ok) {
        ++g_pass;
    } else {
        ++g_fail;
        std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    }
}

std::string leaf(const std::string& p) { return fs::path(p).filename().string(); }

std::string upper(std::string s) {
    for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..";

    // --- scenario_playlist: Tutorial.hsm (tag 0x1d set, tag 0x3c empty) -----
    aow::Map tut;
    std::string err;
    const std::string tutp =
        (fs::path(root) / "Scenario/Custom/Tutorial.hsm").string();
    if (!aow::load_map(tutp, tut, err)) {
        check(false, "load Tutorial.hsm (" + err + ")");
        std::printf("playlist_test: %d passed, %d failed\n", g_pass, g_fail);
        return 1;
    }
    const std::vector<std::string> tut_list = aow::scenario_playlist(tut, root);
    check(tut_list.size() == 2, "Tutorial.hsm playlist has 2 songs");
    if (tut_list.size() == 2) {
        check(upper(leaf(tut_list[0])) == "CLOUDS.IT",
              "Tutorial.hsm song 1 is CLOUDS.IT");
        check(upper(leaf(tut_list[1])) == "REMARCH.IT",
              "Tutorial.hsm song 2 is REMARCH.IT");
    }

    // --- scenario_playlist: Divide and Conquer.hsm (tag 0x3c wins) ----------
    aow::Map dac;
    const std::string dacp =
        (fs::path(root) / "Scenario/Custom/Divide and Conquer.hsm").string();
    if (aow::load_map(dacp, dac, err)) {
        const std::vector<std::string> l = aow::scenario_playlist(dac, root);
        check(l.size() == 7, "Divide and Conquer.hsm playlist has 7 songs");
        check(!l.empty() && upper(leaf(l[0])) == "CONTENT.IT",
              "Divide and Conquer.hsm song 1 is CONTENT.IT (tag 0x3c order)");
    } else {
        check(false, "load Divide and Conquer.hsm (" + err + ")");
    }

    // --- scenario_playlist: missing files are dropped (FileNotFound filter) --
    {
        aow::Map m;
        m.songs_startup = {"SONGS\\CLOUDS.IT",
                           "SONGS\\NOPE_DOES_NOT_EXIST.IT",
                           "SONGS\\REMARCH.IT"};
        const std::vector<std::string> l = aow::scenario_playlist(m, root);
        check(l.size() == 2, "missing song dropped from the tag-0x1d list");
        check(l.size() == 2 && upper(leaf(l[0])) == "CLOUDS.IT" &&
                  upper(leaf(l[1])) == "REMARCH.IT",
              "the surviving songs keep their order");
    }

    // --- scenario_playlist: an empty map falls back to Songs\Heartwoo.it -----
    {
        aow::Map m;
        const std::vector<std::string> l = aow::scenario_playlist(m, root);
        check(l.size() == 1 && upper(leaf(l[0])) == "HEARTWOO.IT",
              "an empty playlist falls back to Songs\\Heartwoo.it");
    }

    // --- PlaylistPlayer: the songs play in order and the list wraps ---------
    {
        aow::PlaylistPlayer player(44100);
        player.set_songs(tut_list);
        check(player.current_index() == -1, "the player starts with no song");
        constexpr int kFrames = 4096;
        std::vector<std::int16_t> buf(static_cast<std::size_t>(kFrames) * 2);
        std::vector<int> starts;         // the index at each song start
        std::vector<std::string> names;  // ...and its file name
        bool was_playing = false;
        // Bound the walk so a stuck file cannot hang the test (~30 min audio).
        const long long max_frames = 44100LL * 60 * 30;
        for (long long f = 0;
             f < max_frames && starts.size() < 3; f += kFrames) {
            player.render(buf.data(), kFrames);
            const bool now = player.playing();
            if (now && !was_playing) {
                starts.push_back(player.current_index());
                names.push_back(leaf(player.songs()[static_cast<std::size_t>(
                    player.current_index())]));
            }
            was_playing = now;
        }
        check(starts.size() >= 3, "the player advanced through at least 3 songs");
        if (starts.size() >= 3) {
            check(starts[0] == 0 && starts[1] == 1 && starts[2] == 0,
                  "songs play in order (0,1) and the list wraps back to 0");
            check(upper(names[0]) == "CLOUDS.IT" &&
                      upper(names[1]) == "REMARCH.IT" &&
                      upper(names[2]) == "CLOUDS.IT",
                  "the wrapped song is the first song again");
        }
    }

    std::printf("playlist_test: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
