// gamerecorder.cpp - `GameRecorder`: record an Age of Wonders window to a GIF.
//
// A small, dependency-free Win32 tool (GDI + comdlg32 only) that turns the game
// window into an animated GIF:
//
//   * pick the window (the game's AoW.exe window is selected automatically),
//   * pick the frame rate (presets 5..30 fps; any 1..50 is accepted),
//   * Start / Pause / Stop - Stop then asks where to save the .gif.
//
// Frames are grabbed from the screen with BitBlt (works for DirectDraw titles)
// or with PrintWindow (works when the window is covered, for GDI windows),
// quantised to a per-frame 256-colour palette (median cut, optional
// Floyd-Steinberg dithering) and streamed into a GIF89a file, so a long
// recording never has to sit in RAM.
//
// The same code has head-less modes (used by the smoke test):
//
//   GameRecorder.exe --record --title "Age of Wonders" --fps 10 --seconds 5
//                    --out clip.gif [--scale 100] [--method screen|printwindow]
//                    [--dither] [--no-loop] [--front]
//   GameRecorder.exe --selftest clip.gif
//
// Windows-only: Win32 + GDI + comdlg32, like the other port viewers.
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Older SDKs do not know these PrintWindow flags.
#ifndef PW_CLIENTONLY
#define PW_CLIENTONLY 0x00000001
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

namespace {

const wchar_t kAppTitle[] = L"GameRecorder - Age of Wonders GIF recorder";
const wchar_t kWindowClass[] = L"GameRecorderWindow";

// Client area of the (fixed-size) window; see create_controls() below.
constexpr int kClientW = 660;
constexpr int kClientH = 612;

// Worker -> UI notifications.
enum : UINT {
    WM_APP_PROGRESS = WM_APP + 1,  // wParam = frames captured so far
    WM_APP_PREVIEW = WM_APP + 2,   // a fresh preview frame is in RecorderState
    WM_APP_FINISHED = WM_APP + 3,  // wParam = 1 on success, 0 on error
};

HWND g_hwnd = nullptr;

// ------------------------------------------------------------------- helpers
std::string to_acp(const std::wstring& w) {
    if (w.empty()) return {};
    const int n = WideCharToMultiByte(CP_ACP, 0, w.c_str(),
                                      static_cast<int>(w.size()), nullptr, 0,
                                      nullptr, nullptr);
    std::string s(static_cast<std::size_t>(n), '\0');
    WideCharToMultiByte(CP_ACP, 0, w.c_str(), static_cast<int>(w.size()),
                        s.data(), n, nullptr, nullptr);
    return s;
}

std::wstring lower_copy(std::wstring s) {
    for (wchar_t& c : s) c = static_cast<wchar_t>(std::towlower(c));
    return s;
}

std::uint64_t ms_between(std::chrono::steady_clock::time_point a,
                         std::chrono::steady_clock::time_point b) {
    if (b <= a) return 0;
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count());
}

// ----------------------------------------------------------------- GIF writer
// GIF89a writer: one full-frame image per frame, a local colour table each
// time (a scene change never has to live with the previous scene's palette),
// no transparency, disposal method 1 ("leave in place").  Frames are written
// to disk as they are captured, so memory use is one frame plus a few small
// lookup tables regardless of the recording length.
class GifWriter {
public:
    GifWriter() = default;
    ~GifWriter() { close(); }
    GifWriter(const GifWriter&) = delete;
    GifWriter& operator=(const GifWriter&) = delete;

    bool open(const std::wstring& path, int width, int height, int delay_cs,
              bool loop);
    bool add_frame(const std::uint8_t* rgb, bool dither);
    bool close();

    std::uint64_t frames() const { return frames_; }
    std::uint64_t bytes() const { return bytes_; }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    struct Bin {
        std::uint8_t r, g, b;
        std::uint32_t count;
    };
    struct Box {
        int begin = 0, end = 0;
        std::uint64_t count = 0;
        int rmin = 255, rmax = 0, gmin = 255, gmax = 0, bmin = 255, bmax = 0;
    };

    bool write(const void* data, std::size_t n);
    bool write_byte(std::uint8_t v) { return write(&v, 1); }

    void build_palette(const std::uint8_t* rgb);
    void quantize(const std::uint8_t* rgb, bool dither);
    int nearest_color(int r, int g, int b) const;

    void lzw_reset();
    int lzw_find(std::uint32_t key) const;
    void lzw_insert(std::uint32_t key, int code);
    void lzw_write_code(int code, int bits);
    void lzw_flush_block();
    void lzw_flush_bits();
    void lzw_encode(const std::uint8_t* pixels, std::size_t n, int min_code_size);

    static constexpr int kHashSize = 8192;

    std::FILE* f_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    int delay_cs_ = 10;
    bool loop_ = true;
    std::uint64_t frames_ = 0;
    std::uint64_t bytes_ = 0;

    // Per-frame quantiser state (allocated once in open()).
    std::vector<std::uint32_t> hist_count_;
    std::vector<std::uint32_t> hist_r_;
    std::vector<std::uint32_t> hist_g_;
    std::vector<std::uint32_t> hist_b_;
    std::vector<Bin> bins_;
    std::vector<int16_t> nearest_;  // RGB555 -> palette index (-1 = unknown)
    std::vector<std::uint8_t> palette_;  // 256 * RGB
    int palette_size_ = 0;
    std::vector<std::uint8_t> indices_;
    std::vector<int> err_cur_r_, err_cur_g_, err_cur_b_;
    std::vector<int> err_next_r_, err_next_g_, err_next_b_;

    // LZW state.
    std::vector<std::uint32_t> lzw_keys_;
    std::vector<std::uint16_t> lzw_vals_;
    std::vector<std::uint8_t> lzw_block_;
    std::uint32_t bit_buf_ = 0;
    int bit_count_ = 0;
};

bool GifWriter::write(const void* data, std::size_t n) {
    if (f_ == nullptr) return false;
    if (n == 0) return true;
    if (std::fwrite(data, 1, n, f_) != n) return false;
    bytes_ += n;
    return true;
}

bool GifWriter::open(const std::wstring& path, int width, int height,
                     int delay_cs, bool loop) {
    if (f_ != nullptr) close();
    if (width <= 0 || height <= 0 || width > 65535 || height > 65535) {
        return false;
    }
    f_ = _wfopen(path.c_str(), L"wb");
    if (f_ == nullptr) return false;

    width_ = width;
    height_ = height;
    delay_cs_ = std::max(2, delay_cs);  // 0/1 cs is treated as 10 by browsers
    loop_ = loop;
    frames_ = 0;
    bytes_ = 0;

    hist_count_.assign(32768, 0);
    hist_r_.assign(32768, 0);
    hist_g_.assign(32768, 0);
    hist_b_.assign(32768, 0);
    bins_.clear();
    bins_.reserve(32768);
    nearest_.assign(32768, -1);
    palette_.assign(256 * 3, 0);
    palette_size_ = 0;
    indices_.assign(static_cast<std::size_t>(width) * height, 0);
    const std::size_t err_w = static_cast<std::size_t>(width) + 2;
    err_cur_r_.assign(err_w, 0);
    err_cur_g_.assign(err_w, 0);
    err_cur_b_.assign(err_w, 0);
    err_next_r_.assign(err_w, 0);
    err_next_g_.assign(err_w, 0);
    err_next_b_.assign(err_w, 0);
    lzw_keys_.assign(kHashSize, 0xffffffffu);
    lzw_vals_.assign(kHashSize, 0);
    lzw_block_.clear();
    lzw_block_.reserve(256);

    // Logical screen descriptor: no global colour table, 8-bit colour
    // resolution, background 0, square pixels.
    const char signature[] = "GIF89a";
    const std::uint8_t lsd[7] = {
        static_cast<std::uint8_t>(width & 0xff),
        static_cast<std::uint8_t>((width >> 8) & 0xff),
        static_cast<std::uint8_t>(height & 0xff),
        static_cast<std::uint8_t>((height >> 8) & 0xff),
        0x70, 0x00, 0x00};
    if (!write(signature, 6) || !write(lsd, sizeof(lsd))) {
        close();
        return false;
    }
    if (loop_) {
        // NETSCAPE2.0 application extension: loop count 0 = forever.
        static const std::uint8_t loop_ext[] = {
            0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E',
            '2', '.', '0', 0x03, 0x01, 0x00, 0x00, 0x00};
        if (!write(loop_ext, sizeof(loop_ext))) {
            close();
            return false;
        }
    }
    return true;
}

bool GifWriter::add_frame(const std::uint8_t* rgb, bool dither) {
    if (f_ == nullptr || rgb == nullptr) return false;
    quantize(rgb, dither);

    int bits = 1;
    while ((1 << bits) < palette_size_ && bits < 8) ++bits;
    const int table_size = 1 << bits;

    // Graphic control extension: disposal 1, delay in 1/100 s, no transparency.
    const std::uint8_t gce[8] = {
        0x21, 0xF9, 0x04, 0x04,
        static_cast<std::uint8_t>(delay_cs_ & 0xff),
        static_cast<std::uint8_t>((delay_cs_ >> 8) & 0xff),
        0x00, 0x00};
    if (!write(gce, sizeof(gce))) return false;

    // Image descriptor with a local colour table.
    const std::uint8_t id[10] = {
        0x2C, 0, 0, 0, 0,
        static_cast<std::uint8_t>(width_ & 0xff),
        static_cast<std::uint8_t>((width_ >> 8) & 0xff),
        static_cast<std::uint8_t>(height_ & 0xff),
        static_cast<std::uint8_t>((height_ >> 8) & 0xff),
        static_cast<std::uint8_t>(0x80 | (bits - 1))};
    if (!write(id, sizeof(id))) return false;

    for (int i = 0; i < table_size; ++i) {
        const int p = (i < palette_size_) ? i : palette_size_ - 1;
        const std::uint8_t entry[3] = {palette_[p * 3 + 0],
                                       palette_[p * 3 + 1],
                                       palette_[p * 3 + 2]};
        if (!write(entry, sizeof(entry))) return false;
    }

    const int min_code_size = std::max(2, bits);
    if (!write_byte(static_cast<std::uint8_t>(min_code_size))) return false;
    lzw_encode(indices_.data(), indices_.size(), min_code_size);
    ++frames_;
    return true;
}

bool GifWriter::close() {
    if (f_ == nullptr) return true;
    const bool ok = write_byte(0x3B);  // trailer
    if (std::fclose(f_) != 0) return false;
    f_ = nullptr;
    return ok;
}

// Median-cut palette for one frame.  A 5-bit-per-channel histogram keeps the
// pass cheap; each bin keeps the sum of the exact 8-bit values, so the palette
// entries are true averages (pixel art with <= 256 distinct colours is
// reproduced exactly).
void GifWriter::build_palette(const std::uint8_t* rgb) {
    std::fill(hist_count_.begin(), hist_count_.end(), 0u);
    std::fill(hist_r_.begin(), hist_r_.end(), 0u);
    std::fill(hist_g_.begin(), hist_g_.end(), 0u);
    std::fill(hist_b_.begin(), hist_b_.end(), 0u);

    const std::size_t n = static_cast<std::size_t>(width_) * height_;
    for (std::size_t i = 0; i < n; ++i) {
        const unsigned r = rgb[i * 3 + 0];
        const unsigned g = rgb[i * 3 + 1];
        const unsigned b = rgb[i * 3 + 2];
        const std::uint32_t key =
            ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
        ++hist_count_[key];
        hist_r_[key] += r;
        hist_g_[key] += g;
        hist_b_[key] += b;
    }

    bins_.clear();
    for (std::uint32_t key = 0; key < 32768; ++key) {
        const std::uint32_t c = hist_count_[key];
        if (c == 0) continue;
        bins_.push_back(Bin{static_cast<std::uint8_t>(hist_r_[key] / c),
                            static_cast<std::uint8_t>(hist_g_[key] / c),
                            static_cast<std::uint8_t>(hist_b_[key] / c), c});
    }

    const auto make_box = [this](int begin, int end) {
        Box box;
        box.begin = begin;
        box.end = end;
        for (int i = begin; i < end; ++i) {
            const Bin& c = bins_[static_cast<std::size_t>(i)];
            box.count += c.count;
            box.rmin = std::min<int>(box.rmin, c.r);
            box.rmax = std::max<int>(box.rmax, c.r);
            box.gmin = std::min<int>(box.gmin, c.g);
            box.gmax = std::max<int>(box.gmax, c.g);
            box.bmin = std::min<int>(box.bmin, c.b);
            box.bmax = std::max<int>(box.bmax, c.b);
        }
        return box;
    };

    std::vector<Box> boxes;
    boxes.reserve(256);
    if (!bins_.empty()) boxes.push_back(make_box(0, static_cast<int>(bins_.size())));

    while (boxes.size() < 256) {
        int pick = -1;
        std::uint64_t best = 0;
        for (std::size_t i = 0; i < boxes.size(); ++i) {
            const Box& b = boxes[i];
            if (b.end - b.begin > 1 && b.count > best) {
                best = b.count;
                pick = static_cast<int>(i);
            }
        }
        if (pick < 0) break;

        Box box = boxes[static_cast<std::size_t>(pick)];
        const int rr = box.rmax - box.rmin;
        const int gr = box.gmax - box.gmin;
        const int br = box.bmax - box.bmin;
        const int channel = (rr >= gr && rr >= br) ? 0 : (gr >= br ? 1 : 2);
        std::sort(bins_.begin() + box.begin, bins_.begin() + box.end,
                  [channel](const Bin& x, const Bin& y) {
                      if (channel == 0) return x.r < y.r;
                      if (channel == 1) return x.g < y.g;
                      return x.b < y.b;
                  });

        const std::uint64_t half = box.count / 2;
        std::uint64_t acc = 0;
        int mid = box.end - 1;
        for (int i = box.begin; i < box.end - 1; ++i) {
            acc += bins_[static_cast<std::size_t>(i)].count;
            if (acc >= half) {
                mid = i + 1;
                break;
            }
        }
        if (mid <= box.begin) mid = box.begin + 1;
        if (mid >= box.end) mid = box.end - 1;
        boxes[static_cast<std::size_t>(pick)] = make_box(box.begin, mid);
        boxes.push_back(make_box(mid, box.end));
    }

    palette_size_ = 0;
    for (const Box& b : boxes) {
        std::uint64_t count = 0, sr = 0, sg = 0, sb = 0;
        for (int i = b.begin; i < b.end; ++i) {
            const Bin& c = bins_[static_cast<std::size_t>(i)];
            count += c.count;
            sr += static_cast<std::uint64_t>(c.r) * c.count;
            sg += static_cast<std::uint64_t>(c.g) * c.count;
            sb += static_cast<std::uint64_t>(c.b) * c.count;
        }
        if (count == 0 || palette_size_ >= 256) continue;
        palette_[palette_size_ * 3 + 0] = static_cast<std::uint8_t>(sr / count);
        palette_[palette_size_ * 3 + 1] = static_cast<std::uint8_t>(sg / count);
        palette_[palette_size_ * 3 + 2] = static_cast<std::uint8_t>(sb / count);
        ++palette_size_;
    }
    if (palette_size_ == 0) {
        palette_[0] = palette_[1] = palette_[2] = 0;
        palette_size_ = 1;
    }
    if (palette_size_ == 1) {  // a GIF table always has at least 2 entries
        palette_[3] = palette_[0];
        palette_[4] = palette_[1];
        palette_[5] = palette_[2];
        palette_size_ = 2;
    }
}

int GifWriter::nearest_color(int r, int g, int b) const {
    int best = 0;
    std::uint32_t best_d = 0xffffffffu;
    for (int i = 0; i < palette_size_; ++i) {
        const int dr = r - palette_[i * 3 + 0];
        const int dg = g - palette_[i * 3 + 1];
        const int db = b - palette_[i * 3 + 2];
        const std::uint32_t d = static_cast<std::uint32_t>(
            2 * dr * dr + 4 * dg * dg + 3 * db * db);
        if (d < best_d) {
            best_d = d;
            best = i;
            if (d == 0) break;
        }
    }
    return best;
}

void GifWriter::quantize(const std::uint8_t* rgb, bool dither) {
    build_palette(rgb);
    std::fill(nearest_.begin(), nearest_.end(), static_cast<int16_t>(-1));

    const int w = width_;
    const int h = height_;
    if (!dither) {
        for (int y = 0; y < h; ++y) {
            const std::uint8_t* s = rgb + static_cast<std::size_t>(y) * w * 3;
            std::uint8_t* d = indices_.data() + static_cast<std::size_t>(y) * w;
            for (int x = 0; x < w; ++x, s += 3) {
                const int r = s[0], g = s[1], b = s[2];
                const std::uint32_t key =
                    ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
                int idx = nearest_[key];
                if (idx < 0) {
                    idx = nearest_color(r, g, b);
                    nearest_[key] = static_cast<int16_t>(idx);
                }
                d[x] = static_cast<std::uint8_t>(idx);
            }
        }
        return;
    }

    // Floyd-Steinberg error diffusion, serpentine order.  Errors are kept as
    // 1/16ths in int arrays indexed by x + 1 (so x-1 .. x+1 are in range).
    std::fill(err_cur_r_.begin(), err_cur_r_.end(), 0);
    std::fill(err_cur_g_.begin(), err_cur_g_.end(), 0);
    std::fill(err_cur_b_.begin(), err_cur_b_.end(), 0);
    for (int y = 0; y < h; ++y) {
        std::fill(err_next_r_.begin(), err_next_r_.end(), 0);
        std::fill(err_next_g_.begin(), err_next_g_.end(), 0);
        std::fill(err_next_b_.begin(), err_next_b_.end(), 0);
        const int dir = (y & 1) ? -1 : 1;
        for (int k = 0; k < w; ++k) {
            const int x = (dir > 0) ? k : (w - 1 - k);
            const std::uint8_t* s =
                rgb + (static_cast<std::size_t>(y) * w + x) * 3;
            int r = std::clamp<int>(s[0] + err_cur_r_[x + 1] / 16, 0, 255);
            int g = std::clamp<int>(s[1] + err_cur_g_[x + 1] / 16, 0, 255);
            int b = std::clamp<int>(s[2] + err_cur_b_[x + 1] / 16, 0, 255);

            const std::uint32_t key =
                ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
            int idx = nearest_[key];
            if (idx < 0) {
                idx = nearest_color(r, g, b);
                nearest_[key] = static_cast<int16_t>(idx);
            }
            indices_[static_cast<std::size_t>(y) * w + x] =
                static_cast<std::uint8_t>(idx);

            const int er = r - palette_[idx * 3 + 0];
            const int eg = g - palette_[idx * 3 + 1];
            const int eb = b - palette_[idx * 3 + 2];
            if (dir > 0) {
                err_cur_r_[x + 2] += er * 7;
                err_next_r_[x] += er * 3;
                err_next_r_[x + 1] += er * 5;
                err_next_r_[x + 2] += er;
                err_cur_g_[x + 2] += eg * 7;
                err_next_g_[x] += eg * 3;
                err_next_g_[x + 1] += eg * 5;
                err_next_g_[x + 2] += eg;
                err_cur_b_[x + 2] += eb * 7;
                err_next_b_[x] += eb * 3;
                err_next_b_[x + 1] += eb * 5;
                err_next_b_[x + 2] += eb;
            } else {
                err_cur_r_[x] += er * 7;
                err_next_r_[x + 2] += er * 3;
                err_next_r_[x + 1] += er * 5;
                err_next_r_[x] += er;
                err_cur_g_[x] += eg * 7;
                err_next_g_[x + 2] += eg * 3;
                err_next_g_[x + 1] += eg * 5;
                err_next_g_[x] += eg;
                err_cur_b_[x] += eb * 7;
                err_next_b_[x + 2] += eb * 3;
                err_next_b_[x + 1] += eb * 5;
                err_next_b_[x] += eb;
            }
        }
        err_cur_r_.swap(err_next_r_);
        err_cur_g_.swap(err_next_g_);
        err_cur_b_.swap(err_next_b_);
    }
}

// ------------------------------------------------------------- LZW (GIF variant)
void GifWriter::lzw_reset() {
    std::fill(lzw_keys_.begin(), lzw_keys_.end(), 0xffffffffu);
}

int GifWriter::lzw_find(std::uint32_t key) const {
    std::uint32_t i = (key * 2654435761u) & (kHashSize - 1);
    for (;;) {
        const std::uint32_t k = lzw_keys_[i];
        if (k == key) return lzw_vals_[i];
        if (k == 0xffffffffu) return -1;
        i = (i + 1) & (kHashSize - 1);
    }
}

void GifWriter::lzw_insert(std::uint32_t key, int code) {
    std::uint32_t i = (key * 2654435761u) & (kHashSize - 1);
    while (lzw_keys_[i] != 0xffffffffu) i = (i + 1) & (kHashSize - 1);
    lzw_keys_[i] = key;
    lzw_vals_[i] = static_cast<std::uint16_t>(code);
}

void GifWriter::lzw_flush_block() {
    if (lzw_block_.empty()) return;
    write_byte(static_cast<std::uint8_t>(lzw_block_.size()));
    write(lzw_block_.data(), lzw_block_.size());
    lzw_block_.clear();
}

void GifWriter::lzw_write_code(int code, int bits) {
    bit_buf_ |= static_cast<std::uint32_t>(code) << bit_count_;
    bit_count_ += bits;
    while (bit_count_ >= 8) {
        lzw_block_.push_back(static_cast<std::uint8_t>(bit_buf_ & 0xff));
        bit_buf_ >>= 8;
        bit_count_ -= 8;
        if (lzw_block_.size() == 255) lzw_flush_block();
    }
}

void GifWriter::lzw_flush_bits() {
    if (bit_count_ > 0) {
        lzw_block_.push_back(static_cast<std::uint8_t>(bit_buf_ & 0xff));
        bit_buf_ = 0;
        bit_count_ = 0;
        if (lzw_block_.size() == 255) lzw_flush_block();
    }
    lzw_flush_block();
    write_byte(0x00);  // end of the LZW data sub-blocks
}

void GifWriter::lzw_encode(const std::uint8_t* pixels, std::size_t n,
                           int min_code_size) {
    lzw_reset();
    lzw_block_.clear();
    bit_buf_ = 0;
    bit_count_ = 0;

    const int clear_code = 1 << min_code_size;
    const int eoi_code = clear_code + 1;
    int code_size = min_code_size + 1;
    int next_code = eoi_code + 1;

    lzw_write_code(clear_code, code_size);
    if (n == 0) {
        lzw_write_code(eoi_code, code_size);
        lzw_flush_bits();
        return;
    }

    int cur = pixels[0];
    for (std::size_t i = 1; i < n; ++i) {
        const int k = pixels[i];
        const std::uint32_t key =
            (static_cast<std::uint32_t>(cur) << 8) | static_cast<std::uint32_t>(k);
        const int found = lzw_find(key);
        if (found >= 0) {
            cur = found;
            continue;
        }

        lzw_write_code(cur, code_size);
        // GIF's "early change": widen the code as soon as the next free code
        // no longer fits.  This check runs after the code was emitted and
        // before the insert below, matching the decoder's one-entry lag.
        if (next_code > (1 << code_size) - 1 && code_size < 12) ++code_size;

        if (next_code < 4096) {
            lzw_insert(key, next_code);
            ++next_code;
        } else {
            lzw_write_code(clear_code, code_size);
            lzw_reset();
            code_size = min_code_size + 1;
            next_code = eoi_code + 1;
        }
        cur = k;
    }

    lzw_write_code(cur, code_size);
    lzw_write_code(eoi_code, code_size);
    lzw_flush_bits();
}

// -------------------------------------------------------------- screen capture
// Captures the client area of one window.  "Screen" mode copies the pixels the
// window currently shows on the desktop (the only reliable way for DirectDraw
// games); "PrintWindow" asks the window to draw itself into a memory DC, which
// also works when another window covers it (GDI windows only).
class WindowCapturer {
public:
    WindowCapturer() = default;
    ~WindowCapturer();
    WindowCapturer(const WindowCapturer&) = delete;
    WindowCapturer& operator=(const WindowCapturer&) = delete;

    bool init(HWND target, int method);
    bool capture();
    bool to_rgb(std::uint8_t* out, int out_w, int out_h);

    int width() const { return w_; }
    int height() const { return h_; }
    const std::uint8_t* bgra() const {
        return static_cast<const std::uint8_t*>(bits_);
    }

private:
    bool ensure(int w, int h);
    bool ensure_scaled(int w, int h);

    HWND target_ = nullptr;
    int method_ = 0;  // 0 = screen BitBlt, 1 = PrintWindow
    HDC screen_ = nullptr;
    HDC mem_ = nullptr;
    HBITMAP bmp_ = nullptr;
    void* bits_ = nullptr;
    int w_ = 0;
    int h_ = 0;

    HDC scaled_dc_ = nullptr;
    HBITMAP scaled_bmp_ = nullptr;
    void* scaled_bits_ = nullptr;
    int scaled_w_ = 0;
    int scaled_h_ = 0;
};

WindowCapturer::~WindowCapturer() {
    // Delete the DCs first: a bitmap that is still selected into a DC cannot
    // be deleted, and DeleteDC does not delete its selected bitmap.
    if (scaled_dc_ != nullptr) DeleteDC(scaled_dc_);
    if (scaled_bmp_ != nullptr) DeleteObject(scaled_bmp_);
    if (mem_ != nullptr) DeleteDC(mem_);
    if (bmp_ != nullptr) DeleteObject(bmp_);
    if (screen_ != nullptr) ReleaseDC(nullptr, screen_);
}

bool WindowCapturer::init(HWND target, int method) {
    target_ = target;
    method_ = method;
    screen_ = GetDC(nullptr);
    if (screen_ == nullptr) return false;
    mem_ = CreateCompatibleDC(screen_);
    if (mem_ == nullptr) return false;
    scaled_dc_ = CreateCompatibleDC(screen_);
    return scaled_dc_ != nullptr;
}

bool WindowCapturer::ensure(int w, int h) {
    if (w == w_ && h == h_ && bits_ != nullptr) return true;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen_, &bmi, DIB_RGB_COLORS, &bits,
                                   nullptr, 0);
    if (bmp == nullptr || bits == nullptr) return false;
    SelectObject(mem_, bmp);
    if (bmp_ != nullptr) DeleteObject(bmp_);  // no longer selected
    bmp_ = bmp;
    bits_ = bits;
    w_ = w;
    h_ = h;
    return true;
}

bool WindowCapturer::ensure_scaled(int w, int h) {
    if (w == scaled_w_ && h == scaled_h_ && scaled_bits_ != nullptr) return true;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen_, &bmi, DIB_RGB_COLORS, &bits,
                                   nullptr, 0);
    if (bmp == nullptr || bits == nullptr) return false;
    SelectObject(scaled_dc_, bmp);
    if (scaled_bmp_ != nullptr) DeleteObject(scaled_bmp_);
    scaled_bmp_ = bmp;
    scaled_bits_ = bits;
    scaled_w_ = w;
    scaled_h_ = h;
    return true;
}

bool WindowCapturer::capture() {
    if (target_ == nullptr || !IsWindow(target_)) return false;
    RECT rc{};
    if (!GetClientRect(target_, &rc)) return false;
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return false;
    if (!ensure(w, h)) return false;

    if (method_ == 0) {
        POINT origin{0, 0};
        if (!ClientToScreen(target_, &origin)) return false;
        // CAPTUREBLT also grabs layered windows (tooltips, cursor overlays).
        return BitBlt(mem_, 0, 0, w, h, screen_, origin.x, origin.y,
                      SRCCOPY | CAPTUREBLT) != FALSE;
    }
    if (PrintWindow(target_, mem_, PW_CLIENTONLY | PW_RENDERFULLCONTENT)) {
        return true;
    }
    return PrintWindow(target_, mem_, 0) != FALSE;
}

bool WindowCapturer::to_rgb(std::uint8_t* out, int out_w, int out_h) {
    if (out == nullptr || bits_ == nullptr) return false;

    const std::uint8_t* src = static_cast<const std::uint8_t*>(bits_);
    if (out_w == w_ && out_h == h_) {
        for (int y = 0; y < h_; ++y) {
            const std::uint8_t* s = src + static_cast<std::size_t>(y) * w_ * 4;
            std::uint8_t* d = out + static_cast<std::size_t>(y) * out_w * 3;
            for (int x = 0; x < w_; ++x, s += 4, d += 3) {
                d[0] = s[2];
                d[1] = s[1];
                d[2] = s[0];
            }
        }
        return true;
    }

    // The window was resized while recording (or the user picked a scale):
    // fit the capture into the fixed GIF frame, letterboxing with black.
    if (!ensure_scaled(out_w, out_h)) return false;
    std::memset(scaled_bits_, 0,
                static_cast<std::size_t>(out_w) * out_h * sizeof(std::uint32_t));
    const double fx = static_cast<double>(out_w) / w_;
    const double fy = static_cast<double>(out_h) / h_;
    const double f = std::min(fx, fy);
    const int dw = std::max(1, static_cast<int>(std::lround(w_ * f)));
    const int dh = std::max(1, static_cast<int>(std::lround(h_ * f)));
    const int dx = (out_w - dw) / 2;
    const int dy = (out_h - dh) / 2;
    SetStretchBltMode(scaled_dc_, HALFTONE);
    SetBrushOrgEx(scaled_dc_, 0, 0, nullptr);
    if (!StretchBlt(scaled_dc_, dx, dy, dw, dh, mem_, 0, 0, w_, h_, SRCCOPY)) {
        return false;
    }

    const std::uint8_t* s2 = static_cast<const std::uint8_t*>(scaled_bits_);
    for (int y = 0; y < out_h; ++y) {
        const std::uint8_t* s = s2 + static_cast<std::size_t>(y) * out_w * 4;
        std::uint8_t* d = out + static_cast<std::size_t>(y) * out_w * 3;
        for (int x = 0; x < out_w; ++x, s += 4, d += 3) {
            d[0] = s[2];
            d[1] = s[1];
            d[2] = s[0];
        }
    }
    return true;
}

// -------------------------------------------------------------------- recorder
struct RecorderConfig {
    HWND target = nullptr;
    int fps = 10;
    int scale_percent = 100;
    int method = 0;  // 0 = screen BitBlt, 1 = PrintWindow
    bool dither = false;
    bool loop = true;
    bool bring_to_front = false;
    std::wstring path;
    int max_seconds = 0;  // 0 = until stopped (head-less runs pass a limit)
};

struct RecorderState {
    std::atomic<bool> stop{false};
    std::atomic<bool> paused{false};
    std::atomic<std::uint64_t> frames{0};
    std::atomic<std::uint64_t> bytes{0};
    std::atomic<std::uint64_t> elapsed_ms{0};
    std::atomic<int> width{0};
    std::atomic<int> height{0};
    std::mutex preview_mutex;
    std::vector<std::uint8_t> preview;  // BGRA, top-left origin
    int preview_w = 0;
    int preview_h = 0;
};

// The recording loop shared by the GUI worker thread and the head-less
// `--record` mode.  Captures at the requested rate, streams each frame into
// the GIF and returns when stopped, when `max_seconds` is reached, or on error.
bool record_loop(const RecorderConfig& cfg, RecorderState& st,
                 std::wstring& error) {
    // NB: stop/paused are reset by the caller before the thread starts.  They
    // must not be touched here, or a Stop pressed immediately after Start
    // would be overwritten by the worker.
    st.frames = 0;
    st.bytes = 0;
    st.elapsed_ms = 0;

    if (cfg.target == nullptr || !IsWindow(cfg.target)) {
        error = L"The target window no longer exists.";
        return false;
    }
    if (cfg.bring_to_front) {
        ShowWindow(cfg.target, SW_RESTORE);
        SetForegroundWindow(cfg.target);
        SetWindowPos(cfg.target, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    if (IsIconic(cfg.target)) {
        error = L"The target window is minimised - restore it first.";
        return false;
    }

    WindowCapturer cap;
    if (!cap.init(cfg.target, cfg.method)) {
        error = L"Could not open the screen for capture.";
        return false;
    }
    if (!cap.capture()) {
        error = L"Could not capture the window (it may be minimised or not "
                L"drawn yet).";
        return false;
    }

    const int native_w = cap.width();
    const int native_h = cap.height();
    const int scale = std::clamp(cfg.scale_percent, 5, 100);
    const int rec_w =
        std::max(2, static_cast<int>(std::lround(native_w * scale / 100.0)));
    const int rec_h =
        std::max(2, static_cast<int>(std::lround(native_h * scale / 100.0)));
    if (rec_w > 65535 || rec_h > 65535) {
        error = L"The window is too large for a GIF.";
        return false;
    }
    st.width = rec_w;
    st.height = rec_h;

    const double fps = std::clamp(cfg.fps, 1, 50);
    const int delay_cs = std::max(2, static_cast<int>(std::lround(100.0 / fps)));
    GifWriter gif;
    if (!gif.open(cfg.path, rec_w, rec_h, delay_cs, cfg.loop)) {
        error = L"Could not create the temporary GIF file:\n" + cfg.path;
        return false;
    }

    std::vector<std::uint8_t> rgb(static_cast<std::size_t>(rec_w) * rec_h * 3);
    using clock = std::chrono::steady_clock;
    const auto period = std::chrono::duration<double>(1.0 / fps);
    auto seg_start = clock::now();
    std::uint64_t active_before = 0;
    bool was_paused = false;
    auto next = seg_start;
    auto last_preview = seg_start - std::chrono::seconds(1);
    std::uint64_t frames = 0;
    bool ok = true;

    while (!st.stop) {
        const auto now = clock::now();
        if (st.paused) {
            if (!was_paused) {
                active_before += ms_between(seg_start, now);
                was_paused = true;
            }
            st.elapsed_ms = active_before;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }
        if (was_paused) {
            was_paused = false;
            seg_start = now;
            next = now;
        }
        if (cfg.max_seconds > 0 &&
            active_before + ms_between(seg_start, now) >=
                static_cast<std::uint64_t>(cfg.max_seconds) * 1000) {
            break;
        }

        if (!cap.capture()) {
            error = L"Capture failed - the window was closed or minimised.";
            ok = false;
            break;
        }
        if (!cap.to_rgb(rgb.data(), rec_w, rec_h)) {
            error = L"Converting the captured frame failed.";
            ok = false;
            break;
        }
        if (!gif.add_frame(rgb.data(), cfg.dither)) {
            error = L"Writing the GIF failed (is the disk full?).";
            ok = false;
            break;
        }

        ++frames;
        st.frames = frames;
        st.bytes = gif.bytes();
        st.elapsed_ms = active_before + ms_between(seg_start, now);

        if (now - last_preview >= std::chrono::milliseconds(150)) {
            {
                std::lock_guard<std::mutex> lock(st.preview_mutex);
                const std::uint8_t* src = cap.bgra();
                st.preview.assign(
                    src, src + static_cast<std::size_t>(cap.width()) *
                                   cap.height() * 4);
                st.preview_w = cap.width();
                st.preview_h = cap.height();
            }
            if (g_hwnd != nullptr) PostMessageW(g_hwnd, WM_APP_PREVIEW, 0, 0);
            last_preview = now;
        }
        if (g_hwnd != nullptr) {
            PostMessageW(g_hwnd, WM_APP_PROGRESS,
                         static_cast<WPARAM>(frames), 0);
        }

        next += std::chrono::duration_cast<clock::duration>(period);
        for (;;) {
            if (st.stop) break;
            const auto t = clock::now();
            if (t >= next) break;
            auto left = std::chrono::duration_cast<std::chrono::milliseconds>(
                            next - t).count();
            if (left > 10) left = 10;
            std::this_thread::sleep_for(
                std::chrono::milliseconds(std::max<long long>(left, 1)));
            if (st.paused) {
                next = clock::now();
                break;
            }
        }
        // If the machine fell far behind, resync instead of bursting frames.
        if (clock::now() - next >
            std::chrono::duration_cast<clock::duration>(period * 4)) {
            next = clock::now();
        }
    }

    if (!gif.close() && ok) {
        error = L"Could not finish the GIF file.";
        ok = false;
    }
    st.elapsed_ms = active_before + ms_between(seg_start, clock::now());
    return ok;
}

// -------------------------------------------------------------------- the GUI
enum : int {
    IDC_WINDOW = 1001,
    IDC_REFRESH,
    IDC_METHOD,
    IDC_SCALE,
    IDC_TOFRONT,
    IDC_DITHER,
    IDC_FPS,
    IDC_LOOP,
    IDC_START,
    IDC_PAUSE,
    IDC_STOP,
    IDC_STATUS,
    IDC_HINT,
    IDC_FPS_HINT,
    IDC_HOTKEYS,
};

struct WindowEntry {
    HWND hwnd = nullptr;
    std::wstring title;
    std::wstring cls;
};

HWND g_window_combo = nullptr;
HWND g_method_combo = nullptr;
HWND g_scale_combo = nullptr;
HWND g_fps_combo = nullptr;
HWND g_fps_hint = nullptr;
HWND g_refresh_btn = nullptr;
HWND g_start_btn = nullptr;
HWND g_pause_btn = nullptr;
HWND g_stop_btn = nullptr;
HWND g_status = nullptr;
HWND g_tofront_check = nullptr;
HWND g_dither_check = nullptr;
HWND g_loop_check = nullptr;
HWND g_hotkey_check = nullptr;
bool g_hotkeys = false;
HFONT g_font = nullptr;
HBRUSH g_bg = nullptr;
std::vector<WindowEntry> g_windows;

RecorderState g_rec;
std::thread g_worker;
std::atomic<bool> g_recording{false};
std::wstring g_temp_path;
std::wstring g_worker_error;
std::wstring g_note;
bool g_close_after_save = false;

RECT preview_rect() {
    RECT r{};
    r.left = 22;
    r.top = 362;
    r.right = 22 + 616;
    r.bottom = 362 + 228;
    return r;
}

std::wstring format_elapsed(std::uint64_t ms) {
    wchar_t buf[64] = L"";
    swprintf(buf, 64, L"%.1f s", static_cast<double>(ms) / 1000.0);
    return buf;
}

std::wstring format_size(std::uint64_t bytes) {
    wchar_t buf[64] = L"";
    if (bytes >= 1024ull * 1024ull) {
        swprintf(buf, 64, L"%.1f MB",
                 static_cast<double>(bytes) / (1024.0 * 1024.0));
    } else if (bytes >= 1024) {
        swprintf(buf, 64, L"%.0f KB", static_cast<double>(bytes) / 1024.0);
    } else {
        swprintf(buf, 64, L"%llu B", static_cast<unsigned long long>(bytes));
    }
    return buf;
}

void update_status() {
    if (g_status == nullptr) return;
    std::wstring text;
    if (g_recording) {
        text = g_rec.paused ? L"Paused" : L"Recording";
        text += L" - " + std::to_wstring(g_rec.frames.load()) + L" frames - " +
                format_elapsed(g_rec.elapsed_ms.load()) + L" - " +
                format_size(g_rec.bytes.load());
        const int w = g_rec.width.load();
        const int h = g_rec.height.load();
        if (w > 0 && h > 0) {
            text += L" - " + std::to_wstring(w) + L"x" + std::to_wstring(h);
        }
    } else {
        text = g_note.empty() ? L"Ready." : g_note;
    }
    SetWindowTextW(g_status, text.c_str());
}

void set_controls_enabled(bool idle) {
    EnableWindow(g_window_combo, idle);
    EnableWindow(g_refresh_btn, idle);
    EnableWindow(g_method_combo, idle);
    EnableWindow(g_scale_combo, idle);
    EnableWindow(g_fps_combo, idle);
    EnableWindow(g_tofront_check, idle);
    EnableWindow(g_dither_check, idle);
    EnableWindow(g_loop_check, idle);
    EnableWindow(g_hotkey_check, idle);
    EnableWindow(g_start_btn, idle);
    EnableWindow(g_pause_btn, !idle);
    EnableWindow(g_stop_btn, !idle);
}

// F6/F7 are registered system-wide only while a recording runs, so the user
// can pause/stop a fullscreen game without alt-tabbing to this window (which
// would minimise the game and break a screen capture).
void set_global_hotkeys(bool on) {
    if (on == g_hotkeys) return;
    if (on) {
        if (RegisterHotKey(g_hwnd, 1, 0, VK_F6) &&
            RegisterHotKey(g_hwnd, 2, 0, VK_F7)) {
            g_hotkeys = true;
        } else {
            UnregisterHotKey(g_hwnd, 1);
            UnregisterHotKey(g_hwnd, 2);
        }
    } else {
        UnregisterHotKey(g_hwnd, 1);
        UnregisterHotKey(g_hwnd, 2);
        g_hotkeys = false;
    }
}

struct EnumCtx {
    std::vector<WindowEntry>* out = nullptr;
    HWND self = nullptr;
};

BOOL CALLBACK enum_window_proc(HWND hwnd, LPARAM lp) {
    EnumCtx* ctx = reinterpret_cast<EnumCtx*>(lp);
    if (hwnd == ctx->self || !IsWindowVisible(hwnd)) return TRUE;
    if (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;

    wchar_t cls[128] = L"";
    GetClassNameW(hwnd, cls, 128);
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return TRUE;
    }
    const int len = GetWindowTextLengthW(hwnd);
    if (len <= 0) return TRUE;
    std::wstring title(static_cast<std::size_t>(len) + 1, L'\0');
    GetWindowTextW(hwnd, title.data(), len + 1);
    title.resize(wcslen(title.c_str()));
    if (title.empty()) return TRUE;

    ctx->out->push_back(WindowEntry{hwnd, title, cls});
    return TRUE;
}

void collect_windows(std::vector<WindowEntry>& out, HWND self) {
    out.clear();
    EnumCtx ctx{&out, self};
    EnumWindows(enum_window_proc, reinterpret_cast<LPARAM>(&ctx));
}

HWND selected_target() {
    const int sel = ComboBox_GetCurSel(g_window_combo);
    if (sel < 0) return nullptr;
    const LRESULT data = SendMessageW(g_window_combo, CB_GETITEMDATA, sel, 0);
    if (data < 0 || static_cast<std::size_t>(data) >= g_windows.size()) {
        return nullptr;
    }
    return g_windows[static_cast<std::size_t>(data)].hwnd;
}

void refresh_window_list() {
    std::wstring keep;
    const int sel = ComboBox_GetCurSel(g_window_combo);
    if (sel >= 0) {
        const LRESULT data = SendMessageW(g_window_combo, CB_GETITEMDATA, sel, 0);
        if (data >= 0 && static_cast<std::size_t>(data) < g_windows.size()) {
            keep = g_windows[static_cast<std::size_t>(data)].title;
        }
    }

    collect_windows(g_windows, g_hwnd);
    ComboBox_ResetContent(g_window_combo);

    int pick = -1;
    for (std::size_t i = 0; i < g_windows.size(); ++i) {
        std::wstring label = g_windows[i].title;
        if (label.size() > 96) label = label.substr(0, 93) + L"...";
        const int item = static_cast<int>(SendMessageW(
            g_window_combo, CB_ADDSTRING, 0,
            reinterpret_cast<LPARAM>(label.c_str())));
        if (item < 0) continue;
        SendMessageW(g_window_combo, CB_SETITEMDATA, item,
                     static_cast<LPARAM>(i));
        if (pick < 0 && !keep.empty() &&
            _wcsicmp(keep.c_str(), g_windows[i].title.c_str()) == 0) {
            pick = item;
        }
    }

    if (pick < 0) {
        for (std::size_t i = 0; i < g_windows.size(); ++i) {
            const std::wstring title = lower_copy(g_windows[i].title);
            const std::wstring cls = lower_copy(g_windows[i].cls);
            if (title.find(L"age of wonders") != std::wstring::npos ||
                title.find(L"aow") != std::wstring::npos ||
                cls.find(L"aow") != std::wstring::npos) {
                pick = static_cast<int>(i);
                break;
            }
        }
    }
    if (pick < 0 && !g_windows.empty()) pick = 0;
    if (pick >= 0) ComboBox_SetCurSel(g_window_combo, pick);
    update_status();
}

int read_fps() {
    wchar_t buf[32] = L"";
    GetWindowTextW(g_fps_combo, buf, 32);
    wchar_t* end = nullptr;
    const long v = wcstol(buf, &end, 10);
    if (end == buf) return -1;
    while (*end == L' ') ++end;
    if (*end != L'\0') return -1;
    if (v < 1 || v > 50) return -1;
    return static_cast<int>(v);
}

void update_fps_hint() {
    const int fps = read_fps();
    wchar_t buf[160] = L"";
    if (fps < 0) {
        wcscpy(buf, L"Enter a whole frame rate between 1 and 50 fps.");
    } else {
        const int delay = std::max(2, static_cast<int>(std::lround(100.0 / fps)));
        swprintf(buf, 160,
                 L"GIF delay %d/100 s - plays at about %.1f fps.  "
                 L"GIF cannot store more than ~50 fps.",
                 delay, 100.0 / delay);
    }
    SetWindowTextW(g_fps_hint, buf);
}

// Save-as dialog.  The recording itself goes to a temp file first, so the
// user can pick the destination only when they stop.
bool save_dialog(std::wstring& out) {
    wchar_t file[MAX_PATH] = L"";
    SYSTEMTIME now{};
    GetLocalTime(&now);
    swprintf(file, MAX_PATH,
             L"aow_%04u-%02u-%02u_%02u-%02u-%02u.gif",
             now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
             now.wSecond);

    wchar_t dir[MAX_PATH] = L"";
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_MYVIDEO, nullptr,
                                SHGFP_TYPE_CURRENT, dir)) ||
        dir[0] == L'\0') {
        GetCurrentDirectoryW(MAX_PATH, dir);
    }

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hwnd;
    ofn.lpstrFilter = L"Animated GIF (*.gif)\0*.gif\0All files (*.*)\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = dir;
    ofn.lpstrDefExt = L"gif";
    ofn.lpstrTitle = L"Save the recording as an animated GIF";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER |
                OFN_NOCHANGEDIR;
    if (!GetSaveFileNameW(&ofn)) return false;
    out = file;
    return true;
}

void maybe_close() {
    if (g_close_after_save) {
        g_close_after_save = false;
        DestroyWindow(g_hwnd);
    }
}

void start_recording() {
    if (g_recording) return;
    HWND target = selected_target();
    if (target == nullptr || !IsWindow(target)) {
        MessageBoxW(g_hwnd,
                    L"Pick the game window first.\n\n"
                    L"If it is not in the list, start the game and press "
                    L"Refresh (F8).",
                    L"GameRecorder", MB_ICONINFORMATION);
        return;
    }
    const int fps = read_fps();
    if (fps < 0) {
        MessageBoxW(g_hwnd,
                    L"The frame rate must be a whole number between 1 and 50.",
                    L"GameRecorder", MB_ICONINFORMATION);
        return;
    }

    int scale_idx = ComboBox_GetCurSel(g_scale_combo);
    if (scale_idx < 0) scale_idx = 0;
    static const int kScales[] = {100, 75, 50, 25};
    const int scale = kScales[std::clamp(scale_idx, 0, 3)];

    int method_idx = ComboBox_GetCurSel(g_method_combo);
    if (method_idx < 0) method_idx = 0;
    method_idx = std::clamp(method_idx, 0, 1);

    if (IsIconic(target)) ShowWindow(target, SW_RESTORE);
    if (Button_GetCheck(g_tofront_check) == BST_CHECKED) {
        SetForegroundWindow(target);
        SetWindowPos(target, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    wchar_t temp_dir[MAX_PATH] = L"";
    if (GetTempPathW(MAX_PATH, temp_dir) == 0) {
        temp_dir[0] = L'.';
        temp_dir[1] = L'\0';
    }
    wchar_t name[160] = L"";
    swprintf(name, 160, L"GameRecorder_%lu_%llu.gif",
             static_cast<unsigned long>(GetCurrentProcessId()),
             static_cast<unsigned long long>(GetTickCount64()));
    g_temp_path = std::wstring(temp_dir) + name;

    g_rec.stop = false;
    g_rec.paused = false;
    g_rec.frames = 0;
    g_rec.bytes = 0;
    g_rec.elapsed_ms = 0;
    g_rec.width = 0;
    g_rec.height = 0;
    {
        std::lock_guard<std::mutex> lock(g_rec.preview_mutex);
        g_rec.preview.clear();
        g_rec.preview_w = 0;
        g_rec.preview_h = 0;
    }
    g_worker_error.clear();
    g_note.clear();

    RecorderConfig cfg;
    cfg.target = target;
    cfg.fps = fps;
    cfg.scale_percent = scale;
    cfg.method = method_idx;
    cfg.dither = Button_GetCheck(g_dither_check) == BST_CHECKED;
    cfg.loop = Button_GetCheck(g_loop_check) == BST_CHECKED;
    cfg.path = g_temp_path;

    try {
        g_worker = std::thread([cfg]() {
            std::wstring err;
            const bool ok = record_loop(cfg, g_rec, err);
            g_worker_error = err;
            PostMessageW(g_hwnd, WM_APP_FINISHED, ok ? 1 : 0, 0);
        });
    } catch (...) {
        MessageBoxW(g_hwnd, L"Could not start the recording thread.",
                    L"GameRecorder", MB_ICONERROR);
        return;
    }

    g_recording = true;
    set_controls_enabled(false);
    set_global_hotkeys(Button_GetCheck(g_hotkey_check) == BST_CHECKED);
    SetWindowTextW(g_pause_btn, L"Pause (F6)");
    update_status();
    SetFocus(g_hwnd);
}

void stop_recording() {
    if (!g_recording || g_rec.stop) return;
    g_rec.stop = true;
    SetWindowTextW(g_status, L"Finishing the GIF...");
    EnableWindow(g_stop_btn, FALSE);
    EnableWindow(g_pause_btn, FALSE);
}

void toggle_pause() {
    if (!g_recording) return;
    const bool paused = !g_rec.paused.load();
    g_rec.paused = paused;
    SetWindowTextW(g_pause_btn, paused ? L"Resume (F6)" : L"Pause (F6)");
    update_status();
}

// Runs on the UI thread once the worker has closed the GIF.  Joining here is
// instant because the worker posts WM_APP_FINISHED as its last action.
void finish_recording(bool ok) {
    if (g_worker.joinable()) g_worker.join();
    set_global_hotkeys(false);
    g_recording = false;
    g_rec.paused = false;
    set_controls_enabled(true);
    EnableWindow(g_stop_btn, FALSE);
    SetWindowTextW(g_pause_btn, L"Pause (F6)");

    const std::uint64_t frames = g_rec.frames.load();
    const std::wstring error = g_worker_error;
    const std::wstring temp = g_temp_path;
    g_temp_path.clear();

    if (!ok) {
        if (!temp.empty()) DeleteFileW(temp.c_str());
        g_note = L"Recording failed.";
        update_status();
        MessageBoxW(g_hwnd,
                    error.empty() ? L"The recording failed." : error.c_str(),
                    L"GameRecorder", MB_ICONERROR);
        maybe_close();
        return;
    }
    if (frames == 0) {
        if (!temp.empty()) DeleteFileW(temp.c_str());
        g_note = L"Nothing was captured.";
        update_status();
        MessageBoxW(g_hwnd,
                    L"Nothing was captured - the window was not drawn.",
                    L"GameRecorder", MB_ICONINFORMATION);
        maybe_close();
        return;
    }

    std::wstring dest;
    if (save_dialog(dest)) {
        if (MoveFileExW(temp.c_str(), dest.c_str(),
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) {
            g_note = L"Saved " + std::to_wstring(frames) + L" frames to " + dest;
            const std::wstring msg =
                L"Saved " + std::to_wstring(frames) + L" frames to:\n" + dest;
            MessageBoxW(g_hwnd, msg.c_str(), L"GameRecorder",
                        MB_ICONINFORMATION);
        } else {
            g_note = L"Save failed - the recording is still at " + temp;
            const std::wstring msg =
                L"Could not move the recording to:\n" + dest +
                L"\n\nIt is still at:\n" + temp;
            MessageBoxW(g_hwnd, msg.c_str(), L"GameRecorder", MB_ICONERROR);
        }
    } else {
        g_note = L"Save cancelled - the recording is still at " + temp;
        const std::wstring msg =
            L"Save cancelled. The recording was kept at:\n" + temp;
        MessageBoxW(g_hwnd, msg.c_str(), L"GameRecorder",
                    MB_ICONINFORMATION);
    }
    update_status();
    maybe_close();
}

// ------------------------------------------------------------------- controls
HWND make_control(HWND parent, const wchar_t* cls, const wchar_t* text,
                  DWORD style, int x, int y, int w, int h, int id) {
    HWND c = CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style, x,
                             y, w, h, parent,
                             reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                             GetModuleHandleW(nullptr), nullptr);
    if (c != nullptr && g_font != nullptr) {
        SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(g_font), TRUE);
    }
    return c;
}

void create_controls(HWND hwnd) {
    g_font = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                         DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                         CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    if (g_font == nullptr) {
        g_font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }

    // --- 1. target ---------------------------------------------------------
    make_control(hwnd, L"BUTTON", L" 1. What to record ", BS_GROUPBOX, 12, 10,
                 636, 152, 0);
    make_control(hwnd, L"STATIC", L"Window:", SS_LEFT, 26, 38, 60, 18, 0);
    g_window_combo = make_control(hwnd, L"COMBOBOX", L"",
                                  CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                                  90, 34, 430, 260, IDC_WINDOW);
    g_refresh_btn = make_control(hwnd, L"BUTTON", L"Refresh (F8)",
                                 BS_PUSHBUTTON | WS_TABSTOP, 528, 33, 108, 26,
                                 IDC_REFRESH);
    make_control(hwnd, L"STATIC", L"Capture:", SS_LEFT, 26, 72, 60, 18, 0);
    g_method_combo = make_control(hwnd, L"COMBOBOX", L"",
                                  CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                                  90, 68, 210, 140, IDC_METHOD);
    SendMessageW(g_method_combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(L"Screen (BitBlt)"));
    SendMessageW(g_method_combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(L"PrintWindow (covered OK)"));
    ComboBox_SetCurSel(g_method_combo, 0);

    g_tofront_check = make_control(
        hwnd, L"BUTTON", L"Bring the game to the front on start",
        BS_AUTOCHECKBOX | WS_TABSTOP, 316, 70, 320, 20, IDC_TOFRONT);
    Button_SetCheck(g_tofront_check, BST_CHECKED);

    make_control(hwnd, L"STATIC", L"Scale:", SS_LEFT, 26, 104, 60, 18, 0);
    g_scale_combo = make_control(hwnd, L"COMBOBOX", L"",
                                 CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                                 90, 100, 90, 140, IDC_SCALE);
    SendMessageW(g_scale_combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(L"100%"));
    SendMessageW(g_scale_combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(L"75%"));
    SendMessageW(g_scale_combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(L"50%"));
    SendMessageW(g_scale_combo, CB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(L"25%"));
    ComboBox_SetCurSel(g_scale_combo, 0);

    g_dither_check = make_control(hwnd, L"BUTTON",
                                  L"Dither (Floyd-Steinberg)",
                                  BS_AUTOCHECKBOX | WS_TABSTOP, 196, 102, 250,
                                  20, IDC_DITHER);

    make_control(hwnd, L"STATIC",
                 L"Screen capture records what is visible: keep the game in "
                 L"front. PrintWindow can capture a covered window, but some "
                 L"DirectDraw games return black.",
                 SS_LEFT, 90, 128, 546, 28, IDC_HINT);

    // --- 2. recording ------------------------------------------------------
    make_control(hwnd, L"BUTTON", L" 2. Recording ", BS_GROUPBOX, 12, 172, 636,
                 116, 0);
    make_control(hwnd, L"STATIC", L"Frame rate:", SS_LEFT, 26, 198, 76, 18, 0);
    g_fps_combo = make_control(hwnd, L"COMBOBOX", L"",
                               CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP, 104,
                               194, 70, 220, IDC_FPS);
    static const wchar_t* kPresets[] = {L"5", L"8", L"10", L"12",
                                        L"15", L"20", L"25", L"30"};
    for (const wchar_t* preset : kPresets) {
        SendMessageW(g_fps_combo, CB_ADDSTRING, 0,
                     reinterpret_cast<LPARAM>(preset));
    }
    SetWindowTextW(g_fps_combo, L"10");
    make_control(hwnd, L"STATIC", L"fps", SS_LEFT, 180, 198, 28, 18, 0);
    g_fps_hint = make_control(hwnd, L"STATIC", L"", SS_LEFT, 214, 198, 420, 18,
                              IDC_FPS_HINT);

    g_loop_check = make_control(hwnd, L"BUTTON", L"Loop forever",
                                BS_AUTOCHECKBOX | WS_TABSTOP, 104, 222, 160,
                                20, IDC_LOOP);
    Button_SetCheck(g_loop_check, BST_CHECKED);

    g_hotkey_check = make_control(
        hwnd, L"BUTTON", L"Global F6 / F7 while recording",
        BS_AUTOCHECKBOX | WS_TABSTOP, 280, 222, 300, 20, IDC_HOTKEYS);
    Button_SetCheck(g_hotkey_check, BST_CHECKED);

    g_status = make_control(hwnd, L"STATIC", L"Ready.", SS_LEFT, 26, 246, 608,
                            34, IDC_STATUS);

    // --- buttons -----------------------------------------------------------
    g_start_btn = make_control(hwnd, L"BUTTON", L"Start (F5)",
                               BS_DEFPUSHBUTTON | WS_TABSTOP, 12, 298, 150, 34,
                               IDC_START);
    g_pause_btn = make_control(hwnd, L"BUTTON", L"Pause (F6)",
                               BS_PUSHBUTTON | WS_TABSTOP, 172, 298, 150, 34,
                               IDC_PAUSE);
    g_stop_btn = make_control(hwnd, L"BUTTON", L"Stop & Save (F7)",
                              BS_PUSHBUTTON | WS_TABSTOP, 332, 298, 170, 34,
                              IDC_STOP);
    make_control(hwnd, L"STATIC", L"F5 / F6 / F7   F8 = refresh", SS_RIGHT,
                 500, 306, 148, 18, 0);

    // --- 3. preview --------------------------------------------------------
    make_control(hwnd, L"BUTTON", L" 3. Preview ", BS_GROUPBOX, 12, 342, 636,
                 258, 0);
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            g_hwnd = hwnd;
            g_bg = CreateSolidBrush(RGB(246, 246, 246));
            create_controls(hwnd);
            refresh_window_list();
            update_fps_hint();
            update_status();
            set_controls_enabled(true);
            EnableWindow(g_pause_btn, FALSE);
            EnableWindow(g_stop_btn, FALSE);
            return 0;

        case WM_HOTKEY:
            if (wp == 1) {  // F6
                toggle_pause();
                return 0;
            }
            if (wp == 2) {  // F7
                stop_recording();
                return 0;
            }
            break;

        case WM_COMMAND: {
            const int id = LOWORD(wp);
            const int code = HIWORD(wp);
            if (id == IDC_START) {
                start_recording();
                return 0;
            }
            if (id == IDC_PAUSE) {
                toggle_pause();
                return 0;
            }
            if (id == IDC_STOP) {
                stop_recording();
                return 0;
            }
            if (id == IDC_REFRESH) {
                refresh_window_list();
                return 0;
            }
            if (id == IDC_FPS && (code == CBN_EDITCHANGE || code == CBN_SELCHANGE)) {
                update_fps_hint();
                return 0;
            }
            break;
        }

        case WM_APP_PROGRESS:
            update_status();
            return 0;

        case WM_APP_PREVIEW: {
            RECT r = preview_rect();
            InvalidateRect(hwnd, &r, FALSE);
            return 0;
        }

        case WM_APP_FINISHED:
            finish_recording(wp != 0);
            return 0;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN: {
            HDC dc = reinterpret_cast<HDC>(wp);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(30, 30, 30));
            return reinterpret_cast<LRESULT>(g_bg);
        }

        case WM_ERASEBKGND:
            return 1;  // WM_PAINT fills the whole client area

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            if (g_bg != nullptr) FillRect(dc, &rc, g_bg);

            const RECT pr = preview_rect();
            HBRUSH black = CreateSolidBrush(RGB(20, 20, 20));
            FillRect(dc, &pr, black);
            DeleteObject(black);

            std::vector<std::uint8_t> frame;
            int pw = 0;
            int ph = 0;
            {
                std::lock_guard<std::mutex> lock(g_rec.preview_mutex);
                frame = g_rec.preview;
                pw = g_rec.preview_w;
                ph = g_rec.preview_h;
            }

            if (pw > 0 && ph > 0 && !frame.empty()) {
                const int avail_w = pr.right - pr.left;
                const int avail_h = pr.bottom - pr.top;
                const double f = std::min(
                    static_cast<double>(avail_w) / pw,
                    static_cast<double>(avail_h) / ph);
                const int dw = std::max(1, static_cast<int>(pw * f));
                const int dh = std::max(1, static_cast<int>(ph * f));
                const int dx = pr.left + (avail_w - dw) / 2;
                const int dy = pr.top + (avail_h - dh) / 2;

                BITMAPINFO bmi{};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = pw;
                bmi.bmiHeader.biHeight = -ph;  // top-down
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;
                SetStretchBltMode(dc, HALFTONE);
                SetBrushOrgEx(dc, 0, 0, nullptr);
                StretchDIBits(dc, dx, dy, dw, dh, 0, 0, pw, ph, frame.data(),
                              &bmi, DIB_RGB_COLORS, SRCCOPY);
            } else {
                SetBkMode(dc, TRANSPARENT);
                SetTextColor(dc, RGB(170, 170, 170));
                RECT tr = pr;
                DrawTextW(dc,
                          L"No preview yet - pick the game window and press "
                          L"Start.",
                          -1, &tr,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
            if (g_recording) {
                const int answer = MessageBoxW(
                    hwnd,
                    L"The recording is still running.\n\n"
                    L"Yes   - stop it, save the GIF, then close\n"
                    L"No    - discard the recording and close\n"
                    L"Cancel - keep recording",
                    L"GameRecorder", MB_YESNOCANCEL | MB_ICONQUESTION);
                if (answer == IDCANCEL) return 0;
                if (answer == IDYES) {
                    g_close_after_save = true;
                    stop_recording();
                    return 0;
                }
                g_rec.stop = true;
                if (g_worker.joinable()) g_worker.join();
                g_recording = false;
                if (!g_temp_path.empty()) DeleteFileW(g_temp_path.c_str());
                g_temp_path.clear();
            }
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_rec.stop = true;
            if (g_worker.joinable()) g_worker.join();
            set_global_hotkeys(false);
            if (g_font != nullptr) {
                DeleteObject(g_font);
                g_font = nullptr;
            }
            if (g_bg != nullptr) {
                DeleteObject(g_bg);
                g_bg = nullptr;
            }
            PostQuitMessage(0);
            return 0;

        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------- head-less CLI
struct CliOptions {
    bool help = false;
    bool selftest = false;
    bool record = false;
    std::wstring out;
    std::wstring title;
    int fps = 10;
    int seconds = 5;
    int scale = 100;
    int method = 0;
    bool dither = false;
    bool loop = true;
    bool front = false;
};

bool attach_console() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) return false;
    std::freopen("CONOUT$", "w", stdout);
    std::freopen("CONOUT$", "w", stderr);
    return true;
}

CliOptions parse_cli(int argc, wchar_t** argv) {
    CliOptions o;
    for (int i = 1; i < argc; ++i) {
        std::wstring arg = argv[i];
        std::wstring value;
        const std::size_t eq = arg.find(L'=');
        if (eq != std::wstring::npos) {
            value = arg.substr(eq + 1);
            arg.resize(eq);
        }
        const auto next_value = [&](std::wstring& v) {
            if (!value.empty()) {
                v = value;
            } else if (i + 1 < argc) {
                v = argv[++i];
            }
        };
        if (arg == L"--help" || arg == L"-h" || arg == L"/?") {
            o.help = true;
        } else if (arg == L"--selftest") {
            o.selftest = true;
            std::wstring v;
            next_value(v);
            if (!v.empty()) o.out = v;
        } else if (arg == L"--record") {
            o.record = true;
        } else if (arg == L"--out") {
            next_value(o.out);
        } else if (arg == L"--title") {
            next_value(o.title);
        } else if (arg == L"--fps") {
            std::wstring v;
            next_value(v);
            o.fps = _wtoi(v.c_str());
        } else if (arg == L"--seconds") {
            std::wstring v;
            next_value(v);
            o.seconds = _wtoi(v.c_str());
        } else if (arg == L"--scale") {
            std::wstring v;
            next_value(v);
            o.scale = _wtoi(v.c_str());
        } else if (arg == L"--method") {
            std::wstring v;
            next_value(v);
            const std::wstring m = lower_copy(v);
            o.method = (m == L"printwindow" || m == L"pw") ? 1 : 0;
        } else if (arg == L"--dither") {
            o.dither = true;
        } else if (arg == L"--no-loop") {
            o.loop = false;
        } else if (arg == L"--front") {
            o.front = true;
        }
    }
    return o;
}

bool find_window_by_title(const std::wstring& needle, HWND& out) {
    std::vector<WindowEntry> windows;
    collect_windows(windows, nullptr);
    if (!needle.empty()) {
        const std::wstring want = lower_copy(needle);
        for (const WindowEntry& w : windows) {
            if (lower_copy(w.title).find(want) != std::wstring::npos) {
                out = w.hwnd;
                return true;
            }
        }
        return false;
    }
    for (const WindowEntry& w : windows) {
        const std::wstring title = lower_copy(w.title);
        const std::wstring cls = lower_copy(w.cls);
        if (title.find(L"age of wonders") != std::wstring::npos ||
            cls.find(L"aow") != std::wstring::npos) {
            out = w.hwnd;
            return true;
        }
    }
    return false;
}

bool run_selftest(const std::wstring& path) {
    const int w = 320;
    const int h = 200;
    const int frames = 24;
    GifWriter gif;
    if (!gif.open(path, w, h, 8, true)) {
        std::printf("selftest: cannot create %s\n", to_acp(path).c_str());
        return false;
    }
    std::vector<std::uint8_t> rgb(static_cast<std::size_t>(w) * h * 3);
    for (int f = 0; f < frames; ++f) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const std::size_t i =
                    (static_cast<std::size_t>(y) * w + x) * 3;
                rgb[i + 0] = static_cast<std::uint8_t>((x + f * 7) & 0xff);
                rgb[i + 1] = static_cast<std::uint8_t>((y * 2 + f * 5) & 0xff);
                rgb[i + 2] =
                    static_cast<std::uint8_t>(((x ^ y) + f * 11) & 0xff);
            }
        }
        // Solid blocks give the palette checks exact, easy-to-verify colours.
        for (int b = 0; b < 3; ++b) {
            const int bx = 10 + ((f * 12 + b * 90) % (w - 60));
            const int by = 20 + b * 55;
            for (int y = by; y < by + 40 && y < h; ++y) {
                for (int x = bx; x < bx + 50 && x < w; ++x) {
                    const std::size_t i =
                        (static_cast<std::size_t>(y) * w + x) * 3;
                    rgb[i + 0] = static_cast<std::uint8_t>(b == 0 ? 255 : 0);
                    rgb[i + 1] = static_cast<std::uint8_t>(b == 1 ? 255 : 0);
                    rgb[i + 2] = static_cast<std::uint8_t>(b == 2 ? 255 : 0);
                }
            }
        }
        // Exercise both quantiser paths (plain + Floyd-Steinberg).
        if (!gif.add_frame(rgb.data(), (f % 3) == 2)) {
            std::printf("selftest: add_frame failed at frame %d\n", f);
            return false;
        }
    }
    if (!gif.close()) {
        std::printf("selftest: close failed\n");
        return false;
    }
    std::printf("selftest: wrote %llu frames (%dx%d), %llu bytes -> %s\n",
                static_cast<unsigned long long>(gif.frames()), w, h,
                static_cast<unsigned long long>(gif.bytes()),
                to_acp(path).c_str());
    return true;
}

int run_cli_record(const CliOptions& o) {
    HWND target = nullptr;
    if (!find_window_by_title(o.title, target)) {
        std::printf("GameRecorder: no window matching \"%s\".\n",
                    to_acp(o.title).c_str());
        std::printf("Open windows:\n");
        std::vector<WindowEntry> windows;
        collect_windows(windows, nullptr);
        for (const WindowEntry& w : windows) {
            std::printf("  %s\n", to_acp(w.title).c_str());
        }
        return 1;
    }

    RecorderConfig cfg;
    cfg.target = target;
    cfg.fps = std::clamp(o.fps, 1, 50);
    cfg.scale_percent = std::clamp(o.scale, 5, 100);
    cfg.method = std::clamp(o.method, 0, 1);
    cfg.dither = o.dither;
    cfg.loop = o.loop;
    cfg.bring_to_front = o.front;
    cfg.max_seconds = std::max(1, o.seconds);
    cfg.path = o.out.empty() ? L"aow_clip.gif" : o.out;

    std::printf("GameRecorder: recording at %d fps for %d s -> %s\n", cfg.fps,
                cfg.max_seconds, to_acp(cfg.path).c_str());

    RecorderState st;
    std::wstring error;
    if (!record_loop(cfg, st, error)) {
        std::printf("GameRecorder: failed: %s\n", to_acp(error).c_str());
        return 2;
    }
    std::printf("GameRecorder: wrote %llu frames (%dx%d), %llu bytes -> %s\n",
                static_cast<unsigned long long>(st.frames.load()),
                st.width.load(), st.height.load(),
                static_cast<unsigned long long>(st.bytes.load()),
                to_acp(cfg.path).c_str());
    return 0;
}

HICON load_app_icon() {
    wchar_t exe[MAX_PATH] = L"";
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    std::wstring dir = exe;
    const std::size_t slash = dir.find_last_of(L"\\/");
    if (slash != std::wstring::npos) dir.resize(slash);
    const std::wstring candidates[] = {dir + L"\\..\\..\\Age of Wonders.ico",
                                       dir + L"\\Age of Wonders.ico"};
    for (const std::wstring& p : candidates) {
        HICON icon = static_cast<HICON>(
            LoadImageW(nullptr, p.c_str(), IMAGE_ICON, 0, 0,
                       LR_LOADFROMFILE | LR_DEFAULTSIZE));
        if (icon != nullptr) return icon;
    }
    return LoadIconW(nullptr, IDI_APPLICATION);
}

}  // namespace

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    SetProcessDPIAware();

    int argc = 0;
    wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    const CliOptions opts = parse_cli(argc, argv);
    if (argv != nullptr) LocalFree(argv);

    if (opts.help) {
        const wchar_t* text =
            L"GameRecorder - record an Age of Wonders window to a GIF\n\n"
            L"GameRecorder.exe\n"
            L"    Opens the recorder: pick the game window and the frame rate,\n"
            L"    press Start (F5), Pause (F6), Stop & Save (F7).\n"
            L"    With 'Global F6 / F7' ticked, pause/stop also work while the\n"
            L"    game window has the focus (needed for a fullscreen game).\n\n"
            L"GameRecorder.exe --record [--title TEXT] [--fps N] [--seconds N]\n"
            L"                  [--out FILE.gif] [--scale 25..100]\n"
            L"                  [--method screen|printwindow] [--dither]\n"
            L"                  [--no-loop] [--front]\n"
            L"    Records head-lessly (no window).  --title matches a window\n"
            L"    title substring; the default looks for Age of Wonders.\n\n"
            L"GameRecorder.exe --selftest [FILE.gif]\n"
            L"    Encodes a synthetic animation and exits (smoke test).\n\n"
            L"Notes:\n"
            L"  * The GIF delay is stored in 1/100 s, so 10 fps is exact and\n"
            L"    12 fps plays at 12.5 fps; values above 50 fps are clamped.\n"
            L"  * 'Screen' capture records what is visible on the desktop;\n"
            L"    keep the game window in front (the recorder does that for\n"
            L"    you when 'Bring the game to the front' is ticked).\n"
            L"  * 'PrintWindow' can capture a covered window, but DirectDraw\n"
            L"    games may return black - use Screen for those.\n";
        if (attach_console()) {
            std::printf("%s\n", to_acp(text).c_str());
        } else {
            MessageBoxW(nullptr, text, L"GameRecorder", MB_ICONINFORMATION);
        }
        return 0;
    }

    if (opts.selftest) {
        attach_console();
        const std::wstring out =
            opts.out.empty() ? L"gamerecorder_selftest.gif" : opts.out;
        return run_selftest(out) ? 0 : 2;
    }

    if (opts.record) {
        attach_console();
        return run_cli_record(opts);
    }

    const HICON icon = load_app_icon();
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;  // WM_PAINT fills the window
    wc.lpszClassName = kWindowClass;
    wc.hIcon = icon;
    wc.hIconSm = icon;
    if (RegisterClassExW(&wc) == 0) return 1;

    RECT rc{0, 0, kClientW, kClientH};
    AdjustWindowRectEx(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                                WS_MINIMIZEBOX,
                       FALSE, 0);
    g_hwnd = CreateWindowExW(
        0, kWindowClass, kAppTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInst, nullptr);
    if (g_hwnd == nullptr) return 1;

    ShowWindow(g_hwnd, (nShow != 0) ? nShow : SW_SHOWNORMAL);
    UpdateWindow(g_hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        // F5/F6/F7/F8 work no matter which child control has the focus.
        if (msg.message == WM_KEYDOWN) {
            if (msg.wParam == VK_F5) {
                start_recording();
                continue;
            }
            if (msg.wParam == VK_F6) {
                toggle_pause();
                continue;
            }
            if (msg.wParam == VK_F7) {
                stop_recording();
                continue;
            }
            if (msg.wParam == VK_F8) {
                refresh_window_list();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
