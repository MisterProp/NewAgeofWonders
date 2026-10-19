// unit_anim.hpp - the AoW1 unit image-sequence animation (the world-map walk).
//
// Reconstructed 1:1 from ILPACK.dpl + AoWEPACK.dpl. Every rule cites the
// decompiled routine it came from; the full write-up is docs/16_UNIT_ANIM.md.
//
//   sequence record   = ImageLib.TImageSequence.LoadFromStream@0x55218a60
//   sequence list     = ImageLib.TImageSequenceList.LoadFromStream@0x552190d8
//   frame index       = ImageLib.TImageSequence.GetFrameIndex@0x552186ec
//   the draw          = AoWE.TUnitHS.Show@0x55784614 / ShowShadow@0x55784504
//   the direction     = AoWE.TUnitHS.SetDirection@0x5578480c / TMoveTE.GetDirection@0x5574771c
//   the sub-hex walk  = AoWE.TUnitHS.MoveFirst@0x55784830 / MoveNext@0x55784910
//   the step count    = AoWE.TUnitHS.GetMoveSteps@0x55784988
//
// A unit's sprite is one image library (`UNITS\<race>\<unit>.ILB`). Its
// `TUnitGFXResource` carries a `TImageSequenceList` (Unitgfx.pfs rw tag 5) whose
// entries are the animation poses. Every shipped unit ships the same 31-entry
// layout:
//
//   [ 0.. 5]  standing pose, facing 1..6            (dir - 1)
//   [ 6..11]  walking cycle, facing 1..6            (dir + 5)
//   [12..17]  an extra 4-frame pose per facing
//   [18..23]  a one-frame tail pose per facing
//   [24..29]  the ground shadow (`UNITS\USHADOW.ILB`), dir 0..5
//   [30]      the corpse (`UNITS\DEADUNIT.ILB`)
//
// `AoWE.TUnitHS.Show` selects the sequence by the facing byte (`TUnitHS+0x14`,
// set from the hex direction the unit is moving in) and whether it is moving
// (`TUnitHS+0x15`: `0xFF` = standing, `0..GetMoveSteps` = moving). The frame
// within the sequence comes from `GetFrameIndex(seq, clock, phase)` where
// `clock` is the map's animation clock (`AoWHSMap+0x40`) and `phase` is a
// per-unit offset (`(ptr >> 2) & 0xffff`).
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace aow {

// ---------------------------------------------------------------------------
// One `ImageLib.TImageSequence` (LoadFromStream@0x55218a60):
//   rwString library name -> +0x14
//   u8       +0x08
//   u8       period -> +0x18   (animation frames between pose frames, minus 1)
//   u8       mode   -> +0x19   (0 static, 1 loop, 2 ping-pong, 3 random,
//                               4 frame table, 5 random frame table)
//   u32      base frame -> +0x20
//   i32      xoff -> +0x24, yoff -> +0x28   (added to the hex's HXtoHP anchor)
//   mode 4/5: a TFrameTableList: u32 seed, u32 count, count * i32 frame ids
//   u8 has_chain; if set, one more TImageSequence follows -> +0x0c
// ---------------------------------------------------------------------------
struct ImageSequence {
    std::string library;                  // +0x14 the ILB this sequence draws
    std::uint8_t reserved = 0;            // +0x08
    std::uint8_t period = 0;              // +0x18
    std::uint8_t mode = 0;                // +0x19
    std::int32_t base_frame = 0;          // +0x20
    std::int32_t xoff = 0;                // +0x24
    std::int32_t yoff = 0;                // +0x28
    std::vector<std::int32_t> frame_table;  // +0x1c (modes 4 and 5)
    std::vector<ImageSequence> chain;     // +0x0c chained pose (usually empty)
    bool empty() const { return library.empty(); }
};

// Parses a serialized `TImageSequenceList` (a Unitgfx.pfs rw tag 5 blob, or an
// ability's tag 8). A `flag` byte of 0 introduces a full sequence; any other
// value is a NULL placeholder (kept so sequence indices line up). Returns false
// (with `error`) on a truncated blob.
bool parse_image_sequence_list(const std::uint8_t* data, std::size_t size,
                               std::vector<ImageSequence>& out,
                               std::string& error);

// The sequence index `AoWE.TUnitHS.Show@0x55784614` draws a unit with:
//   standing (`TUnitHS+0x15 == -1`):  seq[dir - 1]
//   moving   (`TUnitHS+0x15 != -1`):  seq[dir + 5]
// `direction` is the engine's hex facing 1..6 (`HSEngine.dHXtoHNfast@0x5560e338`,
// == aow::hex_direction). Out-of-range directions map to the engine's default 3.
int unit_sequence_index(int direction, bool moving);

// The ground-shadow sequence `AoWE.TUnitHS.ShowShadow@0x55784504` draws, from
// the very same list (the shipped slots 24..29):
//   facing 1..6 (`TUnitHS+0x14`)  ->  seq[dir + 0x17]
//   facing 0                      ->  seq[0x1b]   (27: the engine's fallback)
// A shadow is a **separate scene object** (`AoWE.TUnitShadowHS`), so it is not
// part of the body's pose: `TUnitShadowHS.GetShowPriority` returns the unit's
// own priority with the layer replaced by `0x64000000` (`(prio & 0x3ffffff) +
// 0x64000000`) against the unit's `0x68000000`, i.e. one layer *below* it - the
// shadow is composited before the body (and before the army's own size banner,
// which `TArmyHS.Show` draws after the sprite). Returns -1 for a facing the
// engine cannot index; the port then draws no shadow rather than reading past
// the sequence list.
int unit_shadow_sequence_index(int direction);

// The whole chain `TUnitHS.Show` walks from a selected sequence (seq +0x0c): the
// sequence itself followed by every chained pose, in order.
std::vector<const ImageSequence*> image_sequence_chain(const ImageSequence& seq);

// `ImageLib.TImageSequence.GetFrameIndex@0x552186ec` for the deterministic modes
// a world-map unit uses (0 = static, 1 = loop, 2 = ping-pong, 4 = frame table).
// `library_frames` is the linked ILB's image count (library +0x44); `clock` is
// the map animation clock and `phase` the per-unit offset. Returns the image
// slot id, or -1 when the sequence draws nothing.
int image_sequence_frame(const ImageSequence& seq, int library_frames,
                         int clock, int phase);

// `ImageLib.TImageSequence.GetUnloopedFrameIndex@0x552188a8` - the *one-shot*
// sibling of `GetFrameIndex`, which is what a water/ice hexagon drives
// (`AoWHex.TAoWWaterHexagon.ShowDynamic@5579afb4`). Same record, but `tick` is
// the caller's own counter (the hexagon's `sprite+0x18`), not the map clock,
// and the sequence does NOT wrap: it runs out and then draws nothing.
//
//   tick < 0                       -> -1   (the caller's counter is paused)
//   mode 0                         -> the stored base frame (`seq+0x20`)
//   mode 1                         -> `tick / (period + 1)`, and -1 once that
//                                     reaches the library's image count
//                                     (`seq+0x10 -> +0x44`)
//   mode 4                         -> `frame_table[tick / (period + 1)]`, and -1
//                                     once that reaches the table length
//   modes 2/3/5, anything else     -> -1   (the engine refuses them here - the
//                                     random/ping-pong modes only exist in the
//                                     looping resolver)
//
// `library_frames` is only read by mode 1.
int image_sequence_unlooped_frame(const ImageSequence& seq, int library_frames,
                                  int tick);

// `AoWE.TUnitHS.GetMoveSteps@0x55784988`: the animation frames one hex step
// takes, from the unit-gfx resource's rw tag 10 (`TUnitGFXResource+0x30`):
//   0 -> 12, 1 -> 10, 2 -> 8, 3 -> 6, anything else -> 8.
int unit_move_steps(std::uint8_t gfx_move_class);

// The animation clock `AoWHSMap+0x40`. The engine bumps it once per animation
// frame; the port drives it from its own frame timer.
constexpr int kAnimClockStep = 1;

// The port's walk-speed multiplier: how many engine animation frames one
// frame-timer tick advances. Each frame is one `TUnitHS.MoveNext@0x55784910`
// sub-step (`progress += 1`, so the travel speed) AND one `AoWHSMap+0x40` clock
// tick (so the walk cycle's frame rate) - the two are the same tick, which is
// why one number scales both. The engine advances one per frame; the port runs
// this at 2 to double both the unit's travel speed and the animation's playback
// speed. Set to 1 to reproduce the engine's own rate.
constexpr int kAnimFramesPerTick = 2;

// ---------------------------------------------------------------------------
// The settle phase: why the walk -> standing hand-off is otherwise a lurch, and
// the one number that makes it smooth.
//
// `TUnitHS.Show` frames the walking sequence off the *global* map clock
// (`AoWHSMap+0x40`) with a per-unit offset, so which pose of the 8-frame cycle
// is on screen when a move ends has nothing to do with the move: as likely as
// not the walk is cut off mid-stride. The next frame then draws the standing
// pose, i.e. the unit is swapped from "legs in mid-swing" to "planted" in a
// single frame - the visible stop-then-jump ("right before arriving it stops a
// little and then it dashes"). The engine's own phase is
// `(unit_ptr >> 2) & 0xffff`, i.e. already arbitrary per unit, and the shipped
// art is authored for exactly one hand-off: for every facing the image ids run
//   `<d>01..<d>08`  the walking cycle  (`seq[dir+5]`'s frame table), then
//   `<d>09`         the standing pose  (`seq[dir-1]`, the body TUnitHS draws
//                                     when `+0x15 == -1`) -
// so cycle frame 8 is the artist's *settle* pose, the pose a walk should stop
// on (it is the one that leads into the standing pose).
//
// A front-end therefore does not have to accept the clock's phase: it knows the
// whole move up front, so it can *choose* the phase that lands the cycle's last
// pose on the last animation frame it draws for that move. That is what
// `walk_settle_phase` returns - a drop-in replacement for the per-unit phase.
// ---------------------------------------------------------------------------
//
// `total_substeps` is the whole path's sub-step count
// (`(hexes - 1) * unit_move_steps`), `frames_per_tick` the animation frames the
// front-end advances per timer tick (`kAnimFramesPerTick`), `cycle_length` the
// walking sequence's frame-table length and `spacing` its `period + 1` (the two
// numbers `aow::unit_walk_cycle` reads off the unit's `seq[dir+5]`).
//
// The walk completes on the first tick whose frame count reaches
// `total_substeps`, so the last tick that still draws a walking pose is
// `k = (total_substeps - 1) / frames_per_tick`, at clock `k * frames_per_tick`.
// `image_sequence_frame` reads `counter = clock / spacing` and
// `frame_table[(counter + phase) % cycle_length]`, so the phase that makes that
// draw the cycle's final pose is `(cycle_length - 1 - counter) % cycle_length`.
//
// Returns 0 for degenerate input (nothing to align, so the engine's phase 0).
int walk_settle_phase(int total_substeps, int frames_per_tick, int cycle_length,
                      int spacing);

// How many timer ticks before the end of a walk the moving sprite starts
// cross-fading into the standing pose it hands over to. The engine swaps the
// two sprites in a single frame (`+0x15` goes to -1 and the next draw is
// `seq[dir-1]`), which is the step the eye reads as a lurch however nicely the
// cycle happens to be aligned; fading over the last few frames turns it into the
// unit settling down. Both poses are drawn at the same anchor and the same
// facing (a move ends facing the direction of its last step), so the blend
// cannot drift. Set to 0 to restore the engine's hard swap.
constexpr int kSettleTicks = 3;  // ~100 ms at the 30 fps frame timer

// The standing pose's weight on a walk's current animation frame, given the
// sub-steps still to walk (`WalkAnimation::remaining`) and how many the
// front-end advances per timer tick. It ramps 1/kSettleTicks .. 1 over the last
// `kSettleTicks` ticks and is 1 on the walk's final draw - where the standing
// army the front-end draws next is exactly that pose, so the swap is seamless.
// 0 when the fade is off or the walk has more than `kSettleTicks` ticks to go.
double walk_settle_weight(int remaining_substeps, int frames_per_tick);

// ---------------------------------------------------------------------------
// The sub-hex walk (`AoWE.TUnitHS.MoveFirst@0x55784830` + `MoveNext@0x55784910`).
//
// One hex step takes `GetMoveSteps` animation frames. `progress` (+0x15) runs
// 0..steps_per_hex; the sprite's anchor is the source hex's `HXtoHP` plus
// `round(one_hex_pixel_delta * progress / steps_per_hex)` (`TUnitHS.Show`, via
// `HSEngine.HNtoHP@0x5560e448`). This helper walks a whole path: it starts on
// the first step, advances one frame at a time, and reports the interpolated
// position / facing for the front-end to blit.
// ---------------------------------------------------------------------------
class WalkAnimation {
public:
    // `steps` is the path, source-first (aow::MovePath::steps x/y pairs).
    // `steps_per_hex` is `unit_move_steps`; <= 0 falls back to 8.
    // Returns false for a path shorter than two hexes.
    bool start(const std::vector<std::pair<int, int>>& steps,
               int steps_per_hex);

    bool active() const { return active_; }
    bool done() const { return !active_; }

    // Advances one animation frame. Returns false once the path is finished.
    bool tick();

    // The hex the unit is stepping off / onto (steps[index-1], steps[index]).
    int from_x() const { return from_x_; }
    int from_y() const { return from_y_; }
    int to_x() const { return to_x_; }
    int to_y() const { return to_y_; }

    // The interpolated position in [0,1] between the two hex origins.
    double t() const;

    // The sub-steps still to walk (0 once the path is done). A front-end uses
    // it to fade the walking pose into the standing pose it hands over to
    // (`walk_settle_weight`).
    int remaining() const;

    // The facing byte (`TUnitHS+0x14`) of the current step (1..6).
    int direction() const { return direction_; }
    // The facing the unit ends up with when the path completes.
    int final_direction() const { return final_direction_; }

private:
    bool active_ = false;
    std::vector<std::pair<int, int>> steps_;  // hexes
    std::vector<int> dirs_;                   // step i -> direction of i -> i+1
    std::size_t index_ = 1;                   // entering steps_[index_]
    int progress_ = 0;                        // TUnitHS+0x15
    int steps_per_hex_ = 8;
    int from_x_ = 0, from_y_ = 0, to_x_ = 0, to_y_ = 0;
    int direction_ = 3;
    int final_direction_ = 3;
};

}  // namespace aow