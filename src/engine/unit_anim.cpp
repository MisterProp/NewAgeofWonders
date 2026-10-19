// unit_anim.cpp - the AoW1 unit image-sequence animation (see aow/unit_anim.hpp).
#include "aow/unit_anim.hpp"

namespace aow {
namespace {

// HSEngine.dHXtoHNfast@0x5560e338, verbatim (a copy of aow::hex_direction, kept
// local so this module does not depend on the movement engine's cost tables).
int hex_direction_from(int x0, int y0, int x1, int y1) {
    if (x1 == x0) {
        if (y1 < y0) return 1;
        if (y0 < y1) return 4;
        return 0;
    }
    if ((x0 & 1) == 0) {
        if (x1 < x0) return (y1 < y0) ? 6 : 5;
        return (y1 < y0) ? 2 : 3;
    }
    if (x1 < x0) return (y0 < y1) ? 5 : 6;
    return (y0 < y1) ? 3 : 2;
}

// A little-endian cursor over a serialized EObject blob. Every read is bounds-
// checked; the first overrun latches `bad` so the parser can bail out once.
struct Reader {
    const std::uint8_t* data;
    std::size_t size;
    std::size_t off = 0;
    bool bad = false;

    bool need(std::size_t n) {
        if (bad || off + n > size) {
            bad = true;
            return false;
        }
        return true;
    }
    std::uint8_t u8() {
        if (!need(1)) return 0;
        return data[off++];
    }
    std::uint32_t u32() {
        if (!need(4)) return 0;
        std::uint32_t v = static_cast<std::uint32_t>(data[off]) |
                          (static_cast<std::uint32_t>(data[off + 1]) << 8) |
                          (static_cast<std::uint32_t>(data[off + 2]) << 16) |
                          (static_cast<std::uint32_t>(data[off + 3]) << 24);
        off += 4;
        return v;
    }
    std::int32_t i32() { return static_cast<std::int32_t>(u32()); }
    // rwString (FUN_5520d1e0): a u32 byte length, then the bytes.
    std::string str() {
        const std::uint32_t n = u32();
        if (!need(n)) return std::string();
        std::string s(reinterpret_cast<const char*>(data + off), n);
        off += n;
        return s;
    }
};

// ImageLib.TFrameTableList.LoadFromStream@0x55219f1c: u32 seed, u32 count,
// count * i32 frame ids.
void read_frame_table(Reader& r, ImageSequence& seq) {
    (void)r.u32();  // seed (the engine re-randomises the base frame from this)
    const std::uint32_t n = r.u32();
    seq.frame_table.reserve(n);
    for (std::uint32_t i = 0; i < n; ++i) {
        if (r.bad) break;
        seq.frame_table.push_back(r.i32());
    }
}

// ImageLib.TImageSequence.LoadFromStream@0x55218a60.
void read_sequence(Reader& r, ImageSequence& seq) {
    seq.library = r.str();
    seq.reserved = r.u8();       // +0x08
    seq.period = r.u8();         // +0x18
    seq.mode = r.u8();           // +0x19
    seq.base_frame = r.i32();    // +0x20
    seq.xoff = r.i32();          // +0x24
    seq.yoff = r.i32();          // +0x28
    if ((static_cast<std::uint8_t>(seq.mode - 4)) < 2) {  // mode 4 or 5
        read_frame_table(r, seq);
    }
    const std::uint8_t has_chain = r.u8();
    if (has_chain != 0 && !r.bad) {
        ImageSequence child;
        read_sequence(r, child);
        seq.chain.push_back(std::move(child));
    }
}

int positive_mod(int a, int n) {
    if (n <= 0) return 0;
    int m = a % n;
    if (m < 0) m += n;
    return m;
}

}  // namespace

bool parse_image_sequence_list(const std::uint8_t* data, std::size_t size,
                               std::vector<ImageSequence>& out,
                               std::string& error) {
    out.clear();
    error.clear();
    if (data == nullptr || size < 4) {
        error = "image-sequence list is empty";
        return false;
    }
    Reader r{data, size};
    const std::uint32_t count = r.u32();
    out.resize(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        const std::uint8_t flag = r.u8();
        if (flag != 0) continue;  // a NULL placeholder (kept for index parity)
        read_sequence(r, out[i]);
        if (r.bad) break;
    }
    if (r.bad) {
        error = "image-sequence list is truncated";
        out.clear();
        return false;
    }
    return true;
}

int unit_sequence_index(int direction, bool moving) {
    // TUnitHS.Create@0x55783da4 leaves the facing byte at 3.
    if (direction < 1 || direction > 6) direction = 3;
    return moving ? direction + 5 : direction - 1;
}

int unit_shadow_sequence_index(int direction) {
    // TUnitHS.ShowShadow@0x55784504:
    //   if (*(byte *)(unit + 0x14) == 0) seq = list[0x1b];
    //   else                            seq = list[dir + 0x17];
    // The facing byte is 1..6 for a placed army (`TUnitHS.Create` leaves 3), 0
    // only when the unit has no facing at all - the engine then falls back to
    // slot 27, which is the same sequence facing 4 uses.
    if (direction == 0) return 0x1b;
    if (direction < 1 || direction > 6) return -1;
    return direction + 0x17;
}

std::vector<const ImageSequence*> image_sequence_chain(
    const ImageSequence& seq) {
    std::vector<const ImageSequence*> out;
    const ImageSequence* cur = &seq;
    while (cur != nullptr) {
        out.push_back(cur);
        cur = cur->chain.empty() ? nullptr : &cur->chain.front();
    }
    return out;
}

int image_sequence_frame(const ImageSequence& seq, int library_frames,
                         int clock, int phase) {
    if (clock < 0) return -1;
    const int mode = seq.mode & 0x7f;
    const int spacing = static_cast<int>(seq.period) + 1;
    // The engine keeps a per-sequence frame counter that advances once every
    // `period + 1` clock changes (GetFrameIndex's +0x34/+0x18 countdown); with a
    // fixed frame rate that is exactly `clock / (period + 1)`.
    const int counter = spacing > 0 ? clock / spacing : clock;
    switch (mode) {
        case 0:  // static: the stored base frame
            return seq.base_frame;
        case 1: {  // loop over the whole library
            if (library_frames <= 0) return -1;
            return positive_mod(counter + phase, library_frames);
        }
        case 2: {  // ping-pong over the whole library
            if (library_frames <= 0) return -1;
            const int span = (library_frames - 1) * 2;
            const int u =
                positive_mod(counter + phase, span) - (library_frames - 2);
            return u < 0 ? -u : u;
        }
        case 3:
            // "Random frame from the library": the engine draws `RandInt(count)`
            // every `period + 1` clock changes (GetFrameIndex case 3). The port
            // has no access to the map's RNG, so it *cycles* the library
            // deterministically - same period, every frame shown, just not
            // shuffled (a frozen frame would not animate at all).
            if (library_frames <= 0) return -1;
            return positive_mod(counter + phase, library_frames);
        case 4: {  // frame table
            const int n = static_cast<int>(seq.frame_table.size());
            if (n <= 0) return -1;
            return seq.frame_table[static_cast<std::size_t>(
                positive_mod(counter + phase, n))];
        }
        case 5: {
            // "Random frame-table entry" (GetFrameIndex case 5): the same
            // deterministic cycle as mode 4 - see the note on case 3.
            const int n = static_cast<int>(seq.frame_table.size());
            if (n <= 0) return -1;
            return seq.frame_table[static_cast<std::size_t>(
                positive_mod(counter + phase, n))];
        }
        default:
            return seq.base_frame;
    }
}

int image_sequence_unlooped_frame(const ImageSequence& seq, int library_frames,
                                  int tick) {
    // ImageLib.TImageSequence.GetUnloopedFrameIndex@0x552188a8. The engine tests
    // the *raw* mode byte for 0 and masks 0x7f for the switch, so a mode of 0x80
    // is "not static" and falls to the default (-1); reproduce that literally.
    if (tick < 0) return -1;
    if (seq.mode == 0) return seq.base_frame;  // static: the stored base frame
    const int spacing = static_cast<int>(seq.period) + 1;
    if (spacing <= 0) return -1;  // period is a byte, so this cannot happen
    switch (seq.mode & 0x7f) {
        case 1: {  // the library's frames, once through
            const int i = tick / spacing;
            return i < library_frames ? i : -1;
        }
        case 4: {  // the frame table, once through
            const int i = tick / spacing;
            const int n = static_cast<int>(seq.frame_table.size());
            return i < n ? seq.frame_table[static_cast<std::size_t>(i)] : -1;
        }
        default:
            // The engine's cases 2/3/5 set the result to -1 here (a one-shot
            // caller cannot use ping-pong or a random pick).
            return -1;
    }
}

int unit_move_steps(std::uint8_t gfx_move_class) {
    switch (gfx_move_class) {
        case 0: return 12;
        case 1: return 10;
        case 2: return 8;
        case 3: return 6;
        default: return 8;
    }
}

// ---------------------------------------------------------------------------
// walk_settle_phase (see aow/unit_anim.hpp for the full write-up).
//
// The engine's phase (`(unit_ptr >> 2) & 0xffff`) is arbitrary, so a walk is
// normally cut off wherever the 8-frame cycle happens to be - and the standing
// pose then replaces a mid-stride pose in a single frame. The shipped art runs
// `<d>01..<d>08` (the cycle) straight into `<d>09` (the standing pose), so the
// cycle's *last* pose is the settle pose to stop on. Choosing the phase makes
// the cycle finish on it.
// ---------------------------------------------------------------------------
int walk_settle_phase(int total_substeps, int frames_per_tick, int cycle_length,
                      int spacing) {
    if (total_substeps <= 0 || frames_per_tick <= 0 || cycle_length <= 1 ||
        spacing <= 0) {
        return 0;  // nothing to align: no stride to finish
    }
    // The last tick that still draws a walking pose (the tick that reaches the
    // path's end draws the standing army instead - the walk is over by then).
    const int last_tick = (total_substeps - 1) / frames_per_tick;
    const int clock = last_tick * frames_per_tick;
    // `image_sequence_frame`'s own counter rule (the `+0x30` countdown), so the
    // phase this returns is exactly what that draw will read.
    const int counter = clock / spacing;
    return positive_mod(cycle_length - 1 - counter, cycle_length);
}

// The hand-off fade (see aow/unit_anim.hpp). The engine swaps the walking sprite
// for the standing one in a single frame; ramping the standing pose's weight in
// over the last `kSettleTicks` timer ticks dissolves that swap.
double walk_settle_weight(int remaining_substeps, int frames_per_tick) {
    if (kSettleTicks <= 0 || frames_per_tick <= 0 || remaining_substeps <= 0) {
        return 0.0;  // the fade is off, or the walk is over
    }
    const int ticks_left =
        (remaining_substeps + frames_per_tick - 1) / frames_per_tick;
    if (ticks_left > kSettleTicks) return 0.0;   // still striding
    if (ticks_left <= 1) return 1.0;             // the walk's final draw
    return static_cast<double>(kSettleTicks - ticks_left + 1) /
           static_cast<double>(kSettleTicks);
}

// ---------------------------------------------------------------------------
// WalkAnimation (AoWE.TUnitHS.MoveFirst@0x55784830 / MoveNext@0x55784910).
// ---------------------------------------------------------------------------
bool WalkAnimation::start(const std::vector<std::pair<int, int>>& steps,
                          int steps_per_hex) {
    *this = WalkAnimation{};
    if (steps.size() < 2) return false;
    steps_ = steps;
    steps_per_hex_ = steps_per_hex > 0 ? steps_per_hex : 8;
    dirs_.resize(steps_.size() - 1, 0);
    for (std::size_t i = 0; i + 1 < steps_.size(); ++i) {
        dirs_[i] = hex_direction_from(steps_[i].first, steps_[i].second,
                                     steps_[i + 1].first, steps_[i + 1].second);
    }
    index_ = 1;
    progress_ = 0;
    // MoveFirst@0x55784830 sets the facing to the step's direction.
    direction_ = dirs_[0] >= 1 && dirs_[0] <= 6 ? dirs_[0] : 3;
    final_direction_ = direction_;
    from_x_ = steps_[0].first;
    from_y_ = steps_[0].second;
    to_x_ = steps_[1].first;
    to_y_ = steps_[1].second;
    active_ = true;
    return true;
}

bool WalkAnimation::tick() {
    if (!active_) return false;
    // MoveNext: increment +0x15; a step completes when it reaches GetMoveSteps,
    // then the next step begins with a fresh facing.
    ++progress_;
    if (progress_ >= steps_per_hex_) {
        from_x_ = to_x_;
        from_y_ = to_y_;
        progress_ = 0;
        ++index_;
        if (index_ >= steps_.size()) {
            active_ = false;
            return false;
        }
        direction_ = dirs_[index_ - 1] >= 1 && dirs_[index_ - 1] <= 6
                         ? dirs_[index_ - 1]
                         : 3;
        final_direction_ = direction_;
        to_x_ = steps_[index_].first;
        to_y_ = steps_[index_].second;
    }
    return true;
}

double WalkAnimation::t() const {
    if (steps_per_hex_ <= 0) return 0.0;
    double v = static_cast<double>(progress_) /
               static_cast<double>(steps_per_hex_);
    if (v < 0.0) v = 0.0;
    if (v > 1.0) v = 1.0;
    return v;
}

int WalkAnimation::remaining() const {
    if (!active_ || steps_per_hex_ <= 0) return 0;
    const int total =
        static_cast<int>(steps_.size() - 1) * steps_per_hex_;
    const int done =
        (static_cast<int>(index_) - 1) * steps_per_hex_ + progress_;
    const int left = total - done;
    return left > 0 ? left : 0;
}

}  // namespace aow