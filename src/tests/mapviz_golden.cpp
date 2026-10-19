// mapviz_golden.cpp - Phase 0 golden-render harness for the map viewer.
//
// Renders a fixed set of maps at level 0 to PNGs under a stable output folder
// (`<game-root>/port/re/golden`) so the result can be eyeballed against a real
// in-game / editor screenshot. Also usable as a smoke test: it exits non-zero
// if any map fails to render.
//
//   aow_mapviz_golden [game-root] [out-dir]
//
// Exits 0 when every map rendered. stdlib only.
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/mapviz.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..";
    const std::string outdir =
        (argc > 2) ? argv[2]
                   : (fs::path(root) / "port" / "re" / "golden").string();
    std::error_code ec;
    fs::create_directories(outdir, ec);

    const std::string hss = (fs::path(root) / "Release/Release.hss").string();
    std::vector<aow::MapResource> res;
    std::string err;
    if (!aow::load_map_resources(hss, res, err)) {
        std::fprintf(stderr, "mapviz_golden: %s\n", err.c_str());
        return 1;
    }

    const char* maps[] = {
        "Scenario/Custom/Tutorial.hsm",
        "Scenario/Custom/Underground Titans.hsm",
        "Scenario/Campaign/Dwarf1.CSM",
    };

    int rendered = 0, failed = 0;
    for (const char* rel : maps) {
        const std::string mp = (fs::path(root) / rel).string();
        aow::Map m;
        if (!aow::load_map(mp, m, err)) {
            std::fprintf(stderr, "mapviz_golden: cannot load %s (%s)\n", rel,
                         err.c_str());
            ++failed;
            continue;
        }
        aow::MapVizOptions opt;
        opt.level = 0;
        const std::string stem = fs::path(rel).stem().string();
        const std::string out =
            (fs::path(outdir) / (stem + "_L0.png")).string();
        if (!aow::render_map_png(m, res, root, opt, out, err)) {
            std::fprintf(stderr, "mapviz_golden: render %s failed (%s)\n", rel,
                         err.c_str());
            ++failed;
            continue;
        }
        std::error_code e2;
        const auto sz = fs::file_size(out, e2);
        std::printf("mapviz_golden: %s -> %s (%llu bytes)\n", rel, out.c_str(),
                    static_cast<unsigned long long>(sz));
        if (sz <= 1000) ++failed;
        ++rendered;
    }

    std::printf("mapviz_golden: %d rendered, %d failed\n", rendered, failed);
    return failed == 0 ? 0 : 1;
}
