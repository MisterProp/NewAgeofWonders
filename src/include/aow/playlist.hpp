// playlist.hpp - the game's music player: a scenario's song list, played in
// order (a port of Sound.TSongPlayer, SoundP.dpl).
//
// The 1999 game keeps a single global `Sound.SongPlayer` (a `TSongPlayer`).
// AoW.exe feeds it a list of songs and it plays them one after another:
// `TSongPlayer.Update` (SoundP.dpl 0x55405bf0) advances to the next song when
// the current one stops playing, and wraps back to the first at the end, so
// every song in a map's playlist is heard and the list then repeats. Each song
// is decoded once (no per-song looping).
//
// Two lists are fed to it:
//   * the title screen: the single song `Songs\TITLE.IT` (AoW.exe
//     TitleSceneShow), which therefore loops;
//   * the in-game screen: the scenario's own playlist (AoW.exe FUN_00454d2c),
//     read from the map by `scenario_playlist` below.
//
// This file is the headless, platform-independent half - give it a list and
// pull PCM with render(). The Win32 audio thread in game/menu.cpp drives it.
// See docs/09_GAME_SCREEN.md and docs/05_AUDIO.md.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "aow/map_types.hpp"
#include "aow/openmpt.hpp"

namespace aow {

// Builds the song list the in-game screen plays for `map`, exactly as AoW.exe
// FUN_00454d2c (0x00454d2c) does: the map's tag-0x3c `Sound.TSonglist` wins
// when it has entries; otherwise the tag-0x1d `AoWE.TStartupSongList`
// ("original") songs are copied, skipping any whose file is missing (the engine
// filters on the song's "FileNotFound" title); if neither yields a song the
// default `Songs\Heartwoo.it` is used. The map stores names relative to the game
// root ("SONGS\CLOUDS.IT"), so each is resolved against `game_root`; the result
// holds full paths in the list's own order.
std::vector<std::string> scenario_playlist(const Map& map,
                                           const std::string& game_root);

class PlaylistPlayer {
public:
    explicit PlaylistPlayer(int sample_rate = 44100);

    // Replaces the playlist and restarts at its first song. `paths` are full
    // file paths; an empty list means silence.
    void set_songs(std::vector<std::string> paths);

    // Renders `frames` interleaved stereo frames (L,R), advancing to the next
    // song when the current one ends. Silence while the list is empty or every
    // entry failed to load.
    void render(std::int16_t* out, std::size_t frames);

    const std::vector<std::string>& songs() const { return songs_; }
    int current_index() const { return index_; }  // -1 = nothing playing
    bool playing() const { return started_; }
    const std::string& last_error() const { return error_; }

private:
    // Loads and starts songs_[index_], dropping any entry that cannot be
    // decoded (so a broken song is not retried forever).
    void start_current();

    openmpt::Player mod_;
    std::vector<std::string> songs_;
    int index_ = -1;
    bool started_ = false;
    std::string error_;
};

}  // namespace aow
