// tmp_gr/ref.cpp - render a song at 48 kHz for comparison with the game capture.
#include "aow/audio.hpp"
#include "aow/galaxy.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 3) return 1;
    int rate = argc > 3 ? std::atoi(argv[3]) : 48000;
    int secs = argc > 4 ? std::atoi(argv[4]) : 60;
    aow::galaxy::Player p(rate);
    std::string err;
    if (!p.load(argv[1], &err)) { std::printf("load fail %s\n", err.c_str()); return 1; }
    p.set_loop(false);
    p.set_master_volume(128);
    if (argc > 5) p.set_start_order(std::atoi(argv[5]));
    p.start();
    aow::PcmBuffer buf;
    buf.sample_rate = rate;
    buf.samples.resize(static_cast<std::size_t>(rate) * secs * 2);
    std::size_t done = 0;
    const std::size_t total = static_cast<std::size_t>(rate) * secs;
    while (done < total && p.playing()) {
        std::size_t n = total - done < 4096 ? total - done : 4096;
        p.render(buf.samples.data() + done * 2, n);
        done += n;
    }
    buf.samples.resize(done * 2);
    if (!aow::write_wav(argv[2], buf, &err)) { std::printf("write fail %s\n", err.c_str()); return 1; }
    std::printf("wrote %s (%zu frames @ %d)\n", argv[2], done, rate);
    return 0;
}
