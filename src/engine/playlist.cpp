// playlist.cpp - a port of the game's Sound.TSongPlayer (see playlist.hpp).
#include "aow/playlist.hpp"

#include <cstring>
#include <filesystem>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

namespace aow {

std::vector<std::string> scenario_playlist(const Map& map,
                                           const std::string& game_root) {
    const fs::path root(game_root);
    std::vector<std::string> out;
    if (!map.songs_list.empty()) {
        // Tag 0x3c (Sound.TSonglist) is used as-is.
        for (const std::string& s : map.songs_list) out.push_back((root / s).string());
    } else {
        // Tag 0x1d (AoWE.TStartupSongList): skip any song whose file is missing,
        // which is the engine's "FileNotFound" title filter.
        for (const std::string& s : map.songs_startup) {
            std::error_code ec;
            if (fs::exists(root / s, ec)) out.push_back((root / s).string());
        }
    }
    if (out.empty()) out.push_back((root / "Songs\\Heartwoo.it").string());
    return out;
}

PlaylistPlayer::PlaylistPlayer(int sample_rate) : mod_(sample_rate) {}

void PlaylistPlayer::set_songs(std::vector<std::string> paths) {
    songs_ = std::move(paths);
    index_ = -1;
    started_ = false;
    mod_.stop();
}

void PlaylistPlayer::start_current() {
    started_ = false;
    if (songs_.empty()) return;
    if (index_ < 0 || index_ >= static_cast<int>(songs_.size())) index_ = 0;
    mod_.set_loop(false);  // the player, not the module, loops the list
    // Try each remaining song once; drop any that cannot be decoded.
    const int attempts = static_cast<int>(songs_.size());
    for (int a = 0; a < attempts && !songs_.empty(); ++a) {
        if (index_ >= static_cast<int>(songs_.size())) index_ = 0;
        if (mod_.load(songs_[static_cast<std::size_t>(index_)], &error_)) {
            mod_.start();
            started_ = true;
            return;
        }
        songs_.erase(songs_.begin() + index_);
        if (index_ >= static_cast<int>(songs_.size())) index_ = 0;
    }
}

void PlaylistPlayer::render(std::int16_t* out, std::size_t frames) {
    if (!started_ && !songs_.empty()) start_current();
    if (started_ && !songs_.empty()) {
        mod_.render(out, frames);
        if (!mod_.playing()) {  // song ended -> advance (TSongPlayer.Update)
            index_ = (index_ + 1) % static_cast<int>(songs_.size());
            started_ = false;
        }
        return;
    }
    if (frames) std::memset(out, 0, frames * 2 * sizeof(std::int16_t));
}

}  // namespace aow
