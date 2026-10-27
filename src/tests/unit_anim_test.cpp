// unit_anim_test.cpp - fidelity tests for the AoW1 unit image-sequence animation
// (the world-map walk). Locks the recovered sequence layout, the direction ->
// sequence mapping, the frame-index rule, the per-hex step count and the
// sub-hex walk, against the shipped Unitgfx.pfs and the decompiled engine
// (docs/16_UNIT_ANIM.md).
//
//   aow_unit_anim_tests [game-root]      (default: ..\..)
//
// Exits 0 when every check passes, 1 otherwise. stdlib only.
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/mapviz.hpp"
#include "aow/unit_anim.hpp"
#include "aow/unit_catalog.hpp"

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

}  // namespace

int main(int argc, char** argv) {
    const std::string root = (argc > 1) ? argv[1] : "..\\..";

    // -- unit_sequence_index (TUnitHS.Show@0x55784614) ------------------------
    // standing = dir-1 (0..5), moving = dir+5 (6..11); out of range -> the
    // TUnitHS.Create default facing 3.
    check(aow::unit_sequence_index(1, false) == 0, "stand dir 1 -> seq 0");
    check(aow::unit_sequence_index(6, false) == 5, "stand dir 6 -> seq 5");
    check(aow::unit_sequence_index(1, true) == 6, "walk dir 1 -> seq 6");
    check(aow::unit_sequence_index(6, true) == 11, "walk dir 6 -> seq 11");
    check(aow::unit_sequence_index(0, false) == 2, "dir 0 -> default facing 3");
    check(aow::unit_sequence_index(9, true) == 8, "dir 9 -> default facing 3");

    // -- unit_shadow_sequence_index (TUnitHS.ShowShadow@0x55784504) -----------
    // `list[dir + 0x17]` for a facing 1..6 (the shipped slots 24..29) and
    // `list[0x1b]` when the unit has no facing at all.
    check(aow::unit_shadow_sequence_index(1) == 24, "shadow dir 1 -> seq 24");
    check(aow::unit_shadow_sequence_index(4) == 27, "shadow dir 4 -> seq 27");
    check(aow::unit_shadow_sequence_index(6) == 29, "shadow dir 6 -> seq 29");
    check(aow::unit_shadow_sequence_index(0) == 0x1b,
          "shadow dir 0 -> seq 27 (the engine's fallback)");
    check(aow::unit_shadow_sequence_index(-1) == -1,
          "shadow dir -1 -> no sequence (the port skips the draw)");
    check(aow::unit_shadow_sequence_index(7) == -1,
          "shadow dir 7 -> no sequence (the port skips the draw)");

    // -- unit_move_steps (TUnitHS.GetMoveSteps@0x55784988) --------------------
    check(aow::unit_move_steps(0) == 12, "move class 0 -> 12 steps");
    check(aow::unit_move_steps(1) == 10, "move class 1 -> 10 steps");
    check(aow::unit_move_steps(2) == 8, "move class 2 -> 8 steps");
    check(aow::unit_move_steps(3) == 6, "move class 3 -> 6 steps");
    check(aow::unit_move_steps(9) == 8, "move class 9 -> 8 steps");

    // -- image_sequence_frame (GetFrameIndex@0x552186ec) ----------------------
    {
        // A walking sequence: mode 4 (frame table), period 1, 8 frames. The
        // engine's counter advances once every `period + 1` clock ticks, so the
        // frame holds for two ticks.
        aow::ImageSequence s;
        s.mode = 4;
        s.period = 1;
        s.frame_table = {401, 402, 403, 404, 405, 406, 407, 408};
        check(aow::image_sequence_frame(s, 73, 0, 0) == 401, "walk frame @clock 0");
        check(aow::image_sequence_frame(s, 73, 1, 0) == 401, "walk holds @clock 1");
        check(aow::image_sequence_frame(s, 73, 2, 0) == 402, "walk frame @clock 2");
        check(aow::image_sequence_frame(s, 73, 14, 0) == 408, "walk frame @clock 14");
        check(aow::image_sequence_frame(s, 73, 16, 0) == 401, "walk wraps @clock 16");
        // The per-unit phase shifts the cycle (the engine's `(ptr>>2)&0xffff`).
        check(aow::image_sequence_frame(s, 73, 0, 3) == 404, "phase shifts the cycle");
    }
    {
        // A standing sequence: mode 4, a single-frame table -> constant.
        aow::ImageSequence s;
        s.mode = 4;
        s.frame_table = {209};
        check(aow::image_sequence_frame(s, 73, 0, 0) == 209, "standing pose constant");
        check(aow::image_sequence_frame(s, 73, 99, 5) == 209, "standing pose constant 2");
    }
    {
        // A mode-0 sequence is its stored base frame.
        aow::ImageSequence s;
        s.mode = 0;
        s.base_frame = 1;
        check(aow::image_sequence_frame(s, 2, 0, 0) == 1, "mode 0 -> base frame");
    }
    {
        // A mode-1 sequence loops over the whole library.
        aow::ImageSequence s;
        s.mode = 1;
        s.period = 0;
        bool ok = true;
        for (int t = 0; t < 5; ++t) {
            ok = ok && aow::image_sequence_frame(s, 4, t, 0) == (t % 4);
        }
        check(ok, "mode 1 loops the library");
    }
    {
        // A mode-2 sequence ping-pongs over the library: abs((counter % 6) - 2)
        // for a 4-frame library (GetFrameIndex case 2).
        aow::ImageSequence s;
        s.mode = 2;
        const int expect[8] = {2, 1, 0, 1, 2, 3, 2, 1};
        bool ok = true;
        for (int t = 0; t < 8; ++t) {
            ok = ok && aow::image_sequence_frame(s, 4, t, 0) == expect[t];
        }
        check(ok, "mode 2 ping-pongs the library");
    }
    {
        // Modes 3 and 5 are the engine's "random frame" modes (GetFrameIndex
        // cases 3/5 call RandInt). The port has no map RNG, so it cycles the
        // library / frame table deterministically - the same `period + 1` step,
        // every frame shown, just not shuffled.
        aow::ImageSequence m5;
        m5.mode = 5;
        m5.period = 3;  // one table step every 4 clock ticks
        m5.frame_table = {5, 6, 7, 8, 9};
        check(aow::image_sequence_frame(m5, 0, 0, 0) == 5, "mode 5 -> table[0]");
        check(aow::image_sequence_frame(m5, 0, 4, 0) == 6, "mode 5 steps @4");
        check(aow::image_sequence_frame(m5, 0, 20, 0) == 5, "mode 5 wraps @20");
        aow::ImageSequence m3;
        m3.mode = 3;
        m3.period = 0;
        check(aow::image_sequence_frame(m3, 4, 2, 0) == 2, "mode 3 cycles lib");
    }
    check(aow::image_sequence_frame(aow::ImageSequence{}, 0, -1, 0) == -1,
          "negative clock draws nothing");

    // -- WalkAnimation (MoveFirst@0x55784830 / MoveNext@0x55784910) -----------
    {
        aow::WalkAnimation walk;
        const std::vector<std::pair<int, int>> steps = {
            {0, 0}, {1, 0}, {2, 0}};
        check(walk.start(steps, 4), "walk starts on a two-step path");
        check(walk.direction() == 3, "walk facing = first step direction (3)");
        check(walk.from_x() == 0 && walk.to_x() == 1, "walk starts 0,0 -> 1,0");
        // One hex step is 4 animation frames.
        walk.tick();
        check(walk.active(), "walk active after 1 frame");
        check(walk.t() > 0.2 && walk.t() < 0.3, "walk interpolates 1/4 through");
        walk.tick();
        walk.tick();
        walk.tick();
        check(walk.from_x() == 1 && walk.to_x() == 2, "walk advanced to the 2nd hex");
        check(walk.direction() == 2, "walk facing = 2nd step direction (2)");
        walk.tick();
        walk.tick();
        walk.tick();
        check(walk.active(), "walk still active before the last frame");
        check(!walk.tick(), "walk finishes after the final frame");
        check(walk.done() && !walk.active(), "walk done");
        check(walk.final_direction() == 2, "walk keeps the final facing");
    }
    check(!aow::WalkAnimation{}.start({{0, 0}}, 8), "a one-hex path does not walk");

    // -- walk_settle_phase (making the walk -> standing hand-off smooth) ------
    //
    // `TUnitHS.Show` frames the walking sequence off the *global* map clock, so
    // a walk is normally cut off wherever the cycle happens to be and the
    // standing pose then replaces a mid-stride pose in a single frame - the
    // "stops a little and then dashes" on arrival.  The shipped art is authored
    // for one hand-off: `<d>01..<d>08` (the cycle) then `<d>09` (the standing
    // pose), so the cycle's last pose is the settle pose to stop on.  A move
    // knows its own length, so it can *choose* the phase (the engine's is just
    // `(unit_ptr >> 2) & 0xffff`) that lands that pose on its last drawn frame.
    //
    //   phase = (cycle_length - 1 - (clock_last / spacing)) % cycle_length
    //   clock_last = ((total_substeps - 1) / frames_per_tick) * frames_per_tick
    //
    // 8-pose cycles at spacing 2 (the shipped walks), 2 frames per tick:
    check(aow::walk_settle_phase(8, 2, 8, 2) == 4, "1-hex/8-step move settles");
    check(aow::walk_settle_phase(16, 2, 8, 2) == 0, "2-hex/8-step move settles");
    check(aow::walk_settle_phase(24, 2, 8, 2) == 4,
          "3-hex/8-step move settles (the tutorial hero's move)");
    check(aow::walk_settle_phase(32, 2, 8, 2) == 0, "4-hex/8-step move settles");
    check(aow::walk_settle_phase(12, 2, 8, 2) == 2, "1-hex/12-step move settles");
    check(aow::walk_settle_phase(36, 2, 8, 2) == 6, "3-hex/12-step move settles");
    // Degenerate input: nothing to align, so the engine's phase 0.
    check(aow::walk_settle_phase(0, 2, 8, 2) == 0, "no move -> phase 0");
    check(aow::walk_settle_phase(-4, 2, 8, 2) == 0, "negative move -> phase 0");
    check(aow::walk_settle_phase(24, 0, 8, 2) == 0, "no frames per tick -> phase 0");
    check(aow::walk_settle_phase(24, 2, 1, 2) == 0, "single-pose cycle -> phase 0");
    check(aow::walk_settle_phase(24, 2, 8, 0) == 0, "zero spacing -> phase 0");
    {
        // The property the whole helper exists for, run over every path length
        // and step count a front-end can produce: driving a WalkAnimation the
        // way `tick_anim` / `game_update` do and framing the pose the way
        // `unit_pose_rgba` does, the *last* walking pose drawn before the walk
        // ends is always the cycle's final (settle) pose.
        aow::ImageSequence walk;
        walk.mode = 4;
        walk.period = 1;
        walk.frame_table = {401, 402, 403, 404, 405, 406, 407, 408};
        bool all_settle = true;
        int cases = 0;
        for (int moves = 1; moves <= 12 && all_settle; ++moves) {
            for (int steps = 6; steps <= 12; steps += 2) {
                const int total = moves * steps;
                const int phase = aow::walk_settle_phase(
                    total, aow::kAnimFramesPerTick, 8, 2);
                aow::WalkAnimation w;
                std::vector<std::pair<int, int>> hexes;
                for (int i = 0; i <= moves; ++i) hexes.emplace_back(i, 0);
                if (!w.start(hexes, steps)) {
                    all_settle = false;
                    break;
                }
                int clock = 0;
                int last_pose = -1;
                for (;;) {
                    bool ended = false;
                    for (int i = 0; i < aow::kAnimFramesPerTick; ++i) {
                        ++clock;
                        if (!w.tick()) {
                            ended = true;
                            break;
                        }
                    }
                    if (ended) break;  // the front-end draws the standing army
                    last_pose = aow::image_sequence_frame(walk, 73, clock, phase);
                    ++cases;
                }
                if (last_pose != 408) {
                    std::fprintf(stderr,
                                 "  move %d hexes x %d steps: last pose %d\n",
                                 moves, steps, last_pose);
                    all_settle = false;
                    break;
                }
            }
        }
        check(cases > 100, "the settle sweep ran over many draws");
        check(all_settle, "the aligned phase lands the walk's last pose");
    }

    // -- walk_settle_weight + WalkAnimation::remaining (the hand-off fade) ----
    // The engine swaps the walking sprite for the standing one in a single
    // frame; the port fades the standing pose in over the walk's last
    // `kSettleTicks` timer ticks (1/kSettleTicks .. 1), reaching 1 on the walk's
    // final draw - the pose the standing army the front-end draws next is.
    check(aow::walk_settle_weight(0, 2) == 0.0, "no walk left -> no fade");
    check(aow::walk_settle_weight(8, 2) == 0.0, "4 ticks left -> no fade yet");
    check(aow::walk_settle_weight(6, 2) == 1.0 / 3.0, "3 ticks left -> 1/3 settle");
    check(aow::walk_settle_weight(4, 2) == 2.0 / 3.0, "2 ticks left -> 2/3 settle");
    check(aow::walk_settle_weight(2, 2) == 1.0, "final walk draw -> full settle");
    check(aow::walk_settle_weight(2, 0) == 0.0, "no frames per tick -> no fade");
    {
        // `remaining()` is what drives that ramp: it counts the path's sub-steps
        // down and reaches zero exactly when the walk ends.
        aow::WalkAnimation w;
        const std::vector<std::pair<int, int>> steps = {{0, 0}, {1, 0}, {2, 0}};
        check(w.remaining() == 0, "no walk -> nothing remaining");
        check(w.start(steps, 8) && w.remaining() == 16,
              "a two-hex walk starts with 16 sub-steps left");
        for (int i = 0; i < 15; ++i) w.tick();
        check(w.remaining() == 1, "one sub-step left before the end");
        check(!w.tick() && w.remaining() == 0, "the walk ends with nothing left");
    }

    // -- the shipped Unitgfx.pfs sequence layout (docs/16_UNIT_ANIM.md) -------
    aow::UnitCatalog cat;
    std::string err;
    if (!aow::load_unit_catalog(root, cat, err)) {
        check(false, "load unit catalog (" + err + ")");
    } else {
        // Human Archer: gfx_index 0, 31 sequences, move class 1 (10 steps/hex).
        const aow::UnitGraphics* g = cat.find_gfx(0);
        check(g != nullptr, "Human Archer gfx resolves");
        if (g != nullptr) {
            check(g->sequences.size() == 31, "Human Archer has 31 sequences");
            check(g->move_class == 1, "Human Archer move class 1");
            if (g->sequences.size() >= 31) {
                // The standing pose of facing 1 is the single frame 409 ...
                check(g->sequences[0].mode == 4 &&
                          g->sequences[0].frame_table.size() == 1 &&
                          g->sequences[0].frame_table[0] == 409,
                      "seq 0 = standing facing 1 (frame 409)");
                // ... and the walking cycle of facing 1 is frames 401..408.
                const std::vector<std::int32_t> want = {401, 402, 403, 404,
                                                        405, 406, 407, 408};
                check(g->sequences[6].mode == 4 &&
                          g->sequences[6].frame_table == want,
                      "seq 6 = walking facing 1 (frames 401..408)");
                check(g->sequences[6].period == 1, "walking period 1");
                check(g->sequences[6].xoff == -16 && g->sequences[6].yoff == -24,
                      "walking pose anchored at (-16,-24)");
                // Facing 3 (the TUnitHS.Create default) is the 2xx block.
                check(g->sequences[2].frame_table.size() == 1 &&
                          g->sequences[2].frame_table[0] == 209,
                      "seq 2 = standing facing 3 (frame 209)");
                check(g->sequences[8].frame_table.front() == 201,
                      "seq 8 = walking facing 3 (frame 201)");
                // The shadow poses (24..29) come from the shared shadow library.
                check(g->sequences[24].library.find("USHADOW") != std::string::npos,
                      "seq 24 draws the shared shadow library");
                check(g->sequences[30].library.find("DEADUNIT") != std::string::npos,
                      "seq 30 draws the shared corpse library");
                // Every shadow slot is a *static* pose (`base_frame`) - the
                // archer's are all mode 0, so the map clock never changes them.
                bool shadow_slots = true;
                for (int i = 24; i <= 29; ++i) {
                    const aow::ImageSequence& s =
                        g->sequences[static_cast<std::size_t>(i)];
                    if (s.empty() || s.mode != 0 ||
                        s.library.find("USHADOW") == std::string::npos) {
                        shadow_slots = false;
                    }
                }
                check(shadow_slots, "the archer's shadow slots 24..29 are static");
            }
        }
        // Every unit that draws on the map must ship its 12 stand/walk poses.
        std::size_t no_anim = 0;
        for (const aow::UnitGraphics& gg : cat.graphics) {
            if (gg.sequences.size() < 12) { ++no_anim; continue; }
            for (int i = 0; i < 12; ++i) {
                if (gg.sequences[static_cast<std::size_t>(i)].empty()) {
                    ++no_anim;
                    break;
                }
            }
        }
        check(no_anim == 0, "every unit ships the 12 stand/walk poses");

        // -- the shipped shadow slots (TUnitHS.ShowShadow@0x55784504) -----------
        // Every *non-null* shadow slot draws `UNITS\USHADOW.ILB`; most are static
        // (mode 0) but the flyers ship a 4/6-frame table, so the clock matters
        // there. A NULL placeholder slot (the Incarnate's and the Black Dragon's
        // are all null) makes `TImageSequenceList.GetImageSequence` return 0, so
        // the engine's draw loop never runs - the unit casts no shadow, and
        // neither does the port. The records with only 24 sequences have no
        // shadow slot at all.
        std::size_t shadow_records = 0;
        std::size_t shadowless = 0;
        std::size_t null_shadows = 0;
        std::size_t animated_shadows = 0;
        bool shadow_libs_ok = true;
        for (const aow::UnitGraphics& gg : cat.graphics) {
            if (gg.sequences.size() <= 24) {
                ++shadowless;
                continue;
            }
            ++shadow_records;
            for (int i = 24; i <= 29; ++i) {
                if (static_cast<std::size_t>(i) >= gg.sequences.size()) break;
                const aow::ImageSequence& s =
                    gg.sequences[static_cast<std::size_t>(i)];
                if (s.empty()) {
                    ++null_shadows;
                    continue;
                }
                if (s.library.find("USHADOW") == std::string::npos) {
                    shadow_libs_ok = false;
                    std::fprintf(stderr, "  shadow slot %d of '%s': %s\n", i,
                                 gg.name.c_str(), s.library.c_str());
                }
                if ((s.mode & 0x7f) == 4 && s.frame_table.size() > 1) {
                    ++animated_shadows;
                }
            }
        }
        check(shadow_records > 150, "the shipped roster exposes its shadow slots (" +
                                        std::to_string(shadow_records) + ")");
        check(shadow_libs_ok, "every non-null shadow slot draws UNITS\\USHADOW.ILB");
        check(shadowless > 0, "the 24-sequence records cast no shadow (" +
                                  std::to_string(shadowless) + ")");
        check(null_shadows > 0, "a null shadow slot casts no shadow (" +
                                    std::to_string(null_shadows) + ")");
        check(animated_shadows > 0,
              "the flyers' shadows animate off the map clock (" +
                  std::to_string(animated_shadows) + ")");

        // The `bin/ShadowRef.png` unit: the Human leader (the "Human Hero
        // Leader" gfx record, `UNITS\HMN\HMNHE.ILB`) standing after a move
        // south (facing 4). Its shadow slot is 27 (`dir + 0x17`), a mode-0
        // sequence whose base frame is USHADOW 26 anchored at (-16,-21); the
        // sprite's own base+shift (15,39) puts the art at `HXtoHP + (-1,18)`,
        // which is exactly where the original game's screenshot has it.
        const aow::UnitGraphics* lead = nullptr;
        for (const aow::UnitGraphics& gg : cat.graphics) {
            if (gg.name == "Human Hero Leader") lead = &gg;
        }
        check(lead != nullptr, "the Human Hero Leader gfx record resolves");
        if (lead != nullptr && lead->sequences.size() >= 30) {
            const aow::ImageSequence& sh = lead->sequences[27];
            check(sh.mode == 0 && sh.base_frame == 26 && sh.xoff == -16 &&
                      sh.yoff == -21 && sh.library.find("USHADOW") != std::string::npos,
                  "the leader's facing-4 shadow is USHADOW 26 at (-16,-21)");
            // The whole resolution, through the hero table the map uses:
            // THeroResource 36 (Bloody Marauders' leader) -> Unitgfx 13.
            aow::MapUnit u;
            u.is_hero = true;
            u.resource = 36;
            std::vector<std::uint8_t> art;
            int w = 0, h = 0, vx = 0, vy = 0;
            std::string serr;
            const bool ok = aow::unit_shadow_rgba(cat, root, u, 4, 0, 0, art, w,
                                                  h, vx, vy, serr);
            check(ok && !art.empty(),
                  "the Human leader's facing-4 shadow decodes (" + serr + ")");
            check(ok && vx == -1 && vy == 18,
                  "the shadow lands at HXtoHP + (-1,18) (ShadowRef.png)");
            check(ok && w == 17 && h == 14, "USHADOW 26 is 17x14");
            // A unit with no facing at all (the engine's `TUnitHS+0x14 == 0`)
            // falls back to the same slot 27.
            std::vector<std::uint8_t> art0;
            int w0 = 0, h0 = 0, vx0 = 0, vy0 = 0;
            check(aow::unit_shadow_rgba(cat, root, u, 0, 0, 0, art0, w0, h0, vx0,
                                        vy0, serr) &&
                      art0 == art,
                  "a unit with no facing uses slot 27's shadow");
            // A facing the sequence list cannot hold draws nothing.
            std::vector<std::uint8_t> art7;
            int w7 = 0, h7 = 0, vx7 = 0, vy7 = 0;
            check(!aow::unit_shadow_rgba(cat, root, u, 7, 0, 0, art7, w7, h7,
                                         vx7, vy7, serr),
                  "an out-of-range facing casts no shadow");
        }

        // -- unit_walk_cycle + the settle phase over the shipped rosters ------
        // The pair `walk_settle_phase` needs is read straight off the unit's
        // `seq[dir+5]`; the shipped world-map walks are 8-pose frame tables with
        // period 1 (spacing 2).
        if (g != nullptr) {
            bool archer_ok = true;
            for (int dir = 1; dir <= 6; ++dir) {
                int n = 0, sp = 0;
                if (!aow::unit_walk_cycle(*g, dir, n, sp) || n != 8 || sp != 2) {
                    archer_ok = false;
                }
            }
            check(archer_ok, "Human Archer walk cycle: 8 poses, spacing 2");
        }
        // Every shipped walk settles: for each record, facing and move length
        // the aligned phase lands the cycle's final pose on the last drawn frame.
        // The walking slots are mode-4 cycles except for the units whose "walk"
        // is a *static* pose (Shadow, Incarnate, Sargon, the Fire Elemental and
        // Fire Sprite): those have no stride to finish, so `unit_walk_cycle`
        // declines them and the walk keeps the engine's own arbitrary phase.
        std::size_t cycles = 0;
        std::size_t other_walks = 0;
        bool every_settle = true;
        for (const aow::UnitGraphics& gg : cat.graphics) {
            const int steps = aow::unit_move_steps(gg.move_class);
            for (int dir = 1; dir <= 6; ++dir) {
                const int idx = aow::unit_sequence_index(dir, /*moving=*/true);
                if (idx < 0 || static_cast<std::size_t>(idx) >= gg.sequences.size()) {
                    ++other_walks;
                    continue;
                }
                const aow::ImageSequence& seq =
                    gg.sequences[static_cast<std::size_t>(idx)];
                int n = 0, sp = 0;
                if (!aow::unit_walk_cycle(gg, dir, n, sp)) {
                    // Expected exceptions: a static (mode 0) pose, and a
                    // one-entry mode-4 "cycle" (the Incarnate's facing 1), which
                    // never advances either - neither has a stride to finish.
                    if ((seq.mode & 0x7f) != 0 && seq.frame_table.size() > 1) {
                        ++other_walks;
                        std::fprintf(stderr,
                                     "  walk slot not a cycle: idx=%zu '%s' dir %d "
                                     "mode %u table %zu\n",
                                     gg.index, gg.name.c_str(), dir,
                                     static_cast<unsigned>(seq.mode & 0x7f),
                                     seq.frame_table.size());
                    }
                    continue;
                }
                ++cycles;
                for (int moves = 1; moves <= 5; ++moves) {
                    const int total = moves * steps;
                    const int phase = aow::walk_settle_phase(
                        total, aow::kAnimFramesPerTick, n, sp);
                    const int last_tick = (total - 1) / aow::kAnimFramesPerTick;
                    const int clock = last_tick * aow::kAnimFramesPerTick;
                    if (aow::image_sequence_frame(seq, 0, clock, phase) !=
                        seq.frame_table.back()) {
                        every_settle = false;
                    }
                }
            }
        }
        check(cycles > 1000, "the shipped roster exposes its mode-4 walk cycles (" +
                                 std::to_string(cycles) + ")");
        check(other_walks == 0,
              "every shipped walking slot is a mode-4 cycle or a static pose");
        check(every_settle, "every shipped mode-4 walk settles on its final pose");
    }

    // -- the animated pose over a shipped scenario (Tutorial.hsm) -------------
    {
        const std::string tpath = root + "/Scenario/Custom/Tutorial.hsm";
        aow::Map map;
        std::string merr;
        if (!aow::load_map(tpath, map, merr)) {
            check(false, "load Tutorial.hsm (" + merr + ")");
        } else if (cat.graphics.empty()) {
            check(false, "unit catalog needed for the pose check");
        } else {
            // The first army on the map (level 0).
            const aow::MapUnit* unit = nullptr;
            for (const aow::MapLevel& lv : map.levels) {
                for (const aow::MapTile& t : lv.tiles) {
                    for (const aow::MapHs& hs : t.objects) {
                        if (hs.class_id == aow::kMapArmyHsClassId &&
                            !hs.units.empty()) {
                            unit = &hs.units.front();
                        }
                    }
                    if (unit != nullptr) break;
                }
                if (unit != nullptr) break;
            }
            check(unit != nullptr, "Tutorial.hsm carries an army unit");
            if (unit != nullptr) {
                bool all_dirs = true;
                std::vector<std::uint8_t> pose;
                int w = 0, h = 0, vx = 0, vy = 0;
                for (int dir = 1; dir <= 6; ++dir) {
                    std::string perr;
                    if (!aow::unit_pose_rgba(cat, root, *unit, dir, true, 0, 0,
                                             pose, w, h, vx, vy, perr) ||
                        pose.empty() || w <= 0 || h <= 0) {
                        all_dirs = false;
                        std::fprintf(stderr, "  dir %d: %s\n", dir, perr.c_str());
                    }
                }
                check(all_dirs, "every facing has a decodable walking pose");
                std::string perr;
                std::vector<std::uint8_t> stand;
                int sw = 0, sh = 0, svx = 0, svy = 0;
                const bool sok = aow::unit_pose_rgba(
                    cat, root, *unit, 3, false, 0, 0, stand, sw, sh, svx, svy,
                    perr);
                check(sok && !stand.empty(), "the standing pose decodes");
                // Walking and standing poses differ (the walk cycle animates).
                check(!sok || stand != pose,
                      "a walking pose differs from the standing pose");
                // The *standing* pose is direction-dependent: the idle render
                // draws seq[dir-1] for the army's facing, so an army that just
                // walked keeps facing the way it arrived (dir 1 != dir 2).
                std::vector<std::uint8_t> s1, s2;
                int w1 = 0, h1 = 0, x1 = 0, y1 = 0, w2 = 0, h2 = 0, x2 = 0, y2 = 0;
                std::string e1, e2;
                const bool ok1 = aow::unit_pose_rgba(cat, root, *unit, 1, false, 0,
                                                     0, s1, w1, h1, x1, y1, e1);
                const bool ok2 = aow::unit_pose_rgba(cat, root, *unit, 2, false, 0,
                                                     0, s2, w2, h2, x2, y2, e2);
                check(ok1 && ok2 && !s1.empty() && !s2.empty() && s1 != s2,
                      "the standing pose follows the army's facing");
                // The production cards place the pose with the *frame's* own
                // offsets only (`TCityScreen.PreDrawProductions`, docs/27
                // section 3.3.1): the art is the same standing frame, but the
                // sequence's xoff/yoff are not added - and every shipped
                // standing pose carries (-16,-24), so the card anchor sits
                // exactly that much further down and to the right.
                std::vector<std::uint8_t> card;
                int cw = 0, ch = 0, cvx = 0, cvy = 0;
                std::string cerr;
                const bool cok = aow::unit_pose_sprite_rgba(
                    cat, root, *unit, 3, 0, 0, card, cw, ch, cvx, cvy, cerr);
                check(cok && card == stand && cw == sw && ch == sh,
                      "the card pose draws the same standing art");
                check(cok && cvx - svx == 16 && cvy - svy == 24,
                      "the card pose drops the sequence's (-16,-24) offset");
            }
        }
    }

    std::printf("unit_anim: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}