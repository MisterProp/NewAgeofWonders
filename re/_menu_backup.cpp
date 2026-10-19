// menu.cpp - `aow_menu`: a 1:1 replica of the Age of Wonders 1 title screen
// (the main menu) built from the game's own assets and its recovered geometry.
//
// Everything here comes from the shipped data plus the reverse-engineering
// written up in docs/06_TITLE_SCREEN.md:
//
//   assets   Int\Scenes\title.ilb   (TTitleScene.TitleSceneIL)
//              [0]  ScrollBackg5.BMP   1514x432  the panning backdrop
//              [3]  LogoTitleFinal.BMP  331x175  the "AGE OF WONDERS" logo
//              [10..24] OrbFrame.BMP     82x125   5 menu orbs x 3 states
//              [25] OrbFrame.BMP (TSprite16 overlay)
//            Int\Fonts\Age10.ILB     (FontModule.Age10, glyph index == ASCII)
//            Songs\TITLE.IT          (the title music, started by AoW.exe)
//
//   code     AoW.exe  0x0041a030 WindowDrawSurfaceEx -> 0x00419c84 (the draw)
//                     0x0041ae08 frame tick (counters +0x64/+0x68, credits)
//                     0x00419488 the five-orb hit test
//                     0x0041a150 TitleSceneShow: 640x480 + Songs\TITLE.IT
//
// The scene is 480 px tall and centred vertically on the display, and as wide
// as the display; the five orbs sit on a 120 px pitch centred on that width.
//
//   aow_menu.exe [--root <dir>] [--width N] [--height N]
//                [--render <out.png> --frame N] [--frames N] [--no-music]
//                [--captions a,b,c,d,e] [--overlay]
//
// Windows-only: Win32 + GDI + WinMM (via aow::AudioOut). The asset/engine code
// it calls stays stdlib-only, as required by the port's policy.
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

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include "aow/audio.hpp"
#include "aow/galaxy.hpp"
#include "aow/ilb.hpp"
#include "aow/png.hpp"

namespace fs = std::filesystem;

namespace {

const wchar_t kAppTitle[] = L"aow_menu - Age of Wonders 1 title screen";

// ------------------------------------------------------------- recovered spec
// Frame tick (AoW.exe 0x0041ae08; it increments the scene's +0x64 and +0x68).
constexpr int kFadeInStart = 0x19;    //  25 frames: logo fade-in begins
constexpr int kFadeInEnd = 0x4B;      //  75 frames: fade-in done (alpha 100)
constexpr int kFadeOutBegin = 0x2DA;  // 730 frames: logo fade-out begins
constexpr int kTitleEnd = 0x2EE;      // 750 frames: title phase ends

// Composition (AoW.exe 0x00419c84 and the hit test 0x00419488).
constexpr int kSceneHeight = 0x1E0;   // 480
constexpr int kHalfScene = 0xF0;      // 240 -> top = height/2 - 240
constexpr int kButtonPitch = 0x78;    // 120 px
constexpr int kButtonY = 0x163;       // 355 within the scene
constexpr int kLabelY = 0x1BD;        // 445 within the scene
constexpr int kLogoDy = 0x0A;         //  10 within the scene
constexpr int kScrollBias = 0x1EA;    // 490 px of initial backdrop scroll
constexpr int kButtonCount = 5;       // the hit test loops 0..4
constexpr int kOrbBase = 10;          // first OrbFrame entry
constexpr int kOrbStride = 3;         // three state frames per orb
constexpr int kAlphaFull = 100;       // fade fraction scale in the object code

// Asset indices inside Int\Scenes\title.ilb.
constexpr int kImgBackground = 0;
constexpr int kImgLogo = 3;       // solid LogoTitleFinal (class 0x11)
constexpr int kImgOrbOverlay = 25;  // TSprite16 orb overlay


// ------------------------------------------------------------------- sprites
// One decoded ILB frame. Kept as straight RGBA - the same bytes
// aow::IlbFile::decode_rgb() produces - so the composition math mirrors the
// object code's blit calls.
struct Sprite {
    bool ok = false;
    std::uint32_t w = 0, h = 0;
    std::vector<std::uint8_t> rgba;
    // The engine blits a sprite so that its visual box lands at
    // (x + base_x, y + base_y); see aow/ilb.hpp.
    std::int32_t base_x = 0, base_y = 0;
    std::uint32_t advance = 0;  // descriptor width (bitmap-font advance)
};

class Canvas {
public:
    Canvas(int w, int h)
        : w_(w), h_(h), px_(static_cast<std::size_t>(w) * h * 4, 0) {}

    int width() const { return w_; }
    int height() const { return h_; }

    void clear(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
        for (std::size_t i = 0; i < px_.size(); i += 4) {
            px_[i] = b; px_[i + 1] = g; px_[i + 2] = r; px_[i + 3] = 255;
        }
    }

    // Alpha-blits `s` at (x + base_x, y + base_y), clipped to
    // [clip_l, clip_t, clip_r, clip_b). `alpha100` is the fade fraction the
    // title code hands to ShowOpaque / ShowBlend (0..100).
    void blit(const Sprite& s, int x, int y, int alpha100,
              int clip_l, int clip_t, int clip_r, int clip_b) {
        if (!s.ok || alpha100 <= 0) return;
        const int dx0 = x + s.base_x;
        const int dy0 = y + s.base_y;
        clip_l = std::max(clip_l, 0);
        clip_t = std::max(clip_t, 0);
        clip_r = std::min(clip_r, w_);
        clip_b = std::min(clip_b, h_);
        for (std::uint32_t sy = 0; sy < s.h; ++sy) {
            const int dy = dy0 + static_cast<int>(sy);
            if (dy < clip_t || dy >= clip_b) continue;
            for (std::uint32_t sx = 0; sx < s.w; ++sx) {
                const int dx = dx0 + static_cast<int>(sx);
                if (dx < clip_l || dx >= clip_r) continue;
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                int a = p[3];
                if (a == 0) continue;
                if (alpha100 < kAlphaFull) a = a * alpha100 / kAlphaFull;
                if (a == 0) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                if (a >= 255) {
                    d[0] = p[2]; d[1] = p[1]; d[2] = p[0]; d[3] = 255;
                } else {
                    const int ia = 255 - a;
                    d[0] = static_cast<std::uint8_t>(
                        (p[2] * a + d[0] * ia) / 255);
                    d[1] = static_cast<std::uint8_t>(
                        (p[1] * a + d[1] * ia) / 255);
                    d[2] = static_cast<std::uint8_t>(
                        (p[0] * a + d[2] * ia) / 255);
                    d[3] = 255;
                }
            }
        }
    }

    // Flat-coloured glyph blit: TImageLibraryFont.SetColor keeps the glyph's
    // coverage and applies the requested colour, exactly as the engine fonts
    // render (no anti-aliasing).
    void glyph(const Sprite& s, int x, int y, std::uint8_t r, std::uint8_t g,
               std::uint8_t b) {
        if (!s.ok) return;
        const int dx0 = x + s.base_x;
        const int dy0 = y + s.base_y;
        for (std::uint32_t sy = 0; sy < s.h; ++sy) {
            const int dy = dy0 + static_cast<int>(sy);
            if (dy < 0 || dy >= h_) continue;
            for (std::uint32_t sx = 0; sx < s.w; ++sx) {
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                if (p[3] == 0) continue;
                const int dx = dx0 + static_cast<int>(sx);
                if (dx < 0 || dx >= w_) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                d[0] = b; d[1] = g; d[2] = r; d[3] = 255;
            }
        }
    }

    std::vector<std::uint8_t> to_rgba() const {
        std::vector<std::uint8_t> out(px_.size());
        for (std::size_t i = 0; i < px_.size(); i += 4) {
            out[i] = px_[i + 2];
            out[i + 1] = px_[i + 1];
            out[i + 2] = px_[i];
            out[i + 3] = px_[i + 3];
        }
        return out;
    }

    // Raw BGRA top-down buffer - exactly what a 32bpp BI_RGB DIB wants.
    const std::uint8_t* data() const { return px_.data(); }

private:
    int w_, h_;
    std::vector<std::uint8_t> px_;
};

// The five title-menu entries, in the order the handler at 0x0041ae94
// dispatches them (0 = leftmost orb).  The engine loads the hover captions from
// a runtime resource-string array (AoW.exe data 0x0045a010, five AnsiString
// slots) whose text ids are not yet resolved - see docs/06_TITLE_SCREEN.md.
const char* kDefaultCaptions[kButtonCount] = {
    "New Game", "Load Game", "Multiplayer", "Options", "Exit",
};

std::string acp_from_w(const std::wstring& w) {
    if (w.empty()) return {};
    const int n = WideCharToMultiByte(CP_ACP, 0, w.c_str(),
                                      static_cast<int>(w.size()), nullptr, 0,
                                      nullptr, nullptr);
    std::string s(static_cast<std::size_t>(n), '\0');
    WideCharToMultiByte(CP_ACP, 0, w.c_str(), static_cast<int>(w.size()),
                        s.data(), n, nullptr, nullptr);
    return s;
}

// ---------------------------------------------------------------- the screen
struct Scene {
    aow::IlbFile title_il;   // Int\Scenes\title.ilb  (TitleSceneIL)
    aow::IlbFile font_il;    // Int\Fonts\Age10.ILB   (FontModule.Age10)

    Sprite background;                       // [0]  ScrollBackg5
    Sprite logo;                             // [3]  LogoTitleFinal (solid)
    Sprite orb[kButtonCount][kOrbStride];    // [10 + 3i + state]
    Sprite orb_overlay;                      // [25] TSprite16 overlay
    std::vector<Sprite> glyphs;              // indexed by ASCII code

    int disp_w = 640, disp_h = 480;
    bool overlay = false;                    // the engine +0x6d == 2 effect
    std::string captions[kButtonCount];

    // Input state, mirroring the scene object's fields.
    int cursor_index = -1;  // +0x7c : index under the cursor
    int armed_index = -1;   // +0x80 : index recorded on mouse-down
    int mouse_x = 0, mouse_y = 0;
    std::uint32_t counter64 = 0;  // +0x64 : backdrop + logo frame counter
    std::uint32_t counter68 = 0;  // +0x68 : credits timing counter

    // --- geometry recovered from 0x00419c84 --------------------------------
    int scene_top() const { return disp_h / 2 - kHalfScene; }
    int scene_bottom() const { return scene_top() + kSceneHeight; }
    int button_left(int i) const {
        // ((orb_width + left + right) >> 1) - 320 + i*120, with left = 0 and
        // right = display width.
        return ((static_cast<int>(orb[0][0].w) + disp_w) >> 1) - 320
               + i * kButtonPitch;
    }
    int logo_left() const { return (disp_w - static_cast<int>(logo.w)) >> 1; }

    bool load(const std::string& root, std::string* error) {
        std::string err;
        const fs::path base(root);
        const std::string t =
            (base / "Int" / "Scenes" / "title.ilb").string();
        if (!title_il.load(t, err)) { *error = err + " (" + t + ")"; return false; }
        const std::string f =
            (base / "Int" / "Fonts" / "Age10.ILB").string();
        if (!font_il.load(f, err)) { *error = err + " (" + f + ")"; return false; }

        auto decode = [&](const aow::IlbFile& ilb, std::size_t idx,
                          Sprite& out) -> bool {
            std::uint32_t w = 0, h = 0;
            if (!ilb.decode_rgb(idx, out.rgba, w, h, err)) return false;
            out.w = w; out.h = h; out.ok = true;
            const aow::IlbEntry& e = ilb.entries()[idx];
            out.base_x = e.base_x;
            out.base_y = e.base_y;
            out.advance = e.width;
            return true;
        };

        if (!decode(title_il, kImgBackground, background)) {
            *error = "background: " + err; return false;
        }
        if (!decode(title_il, kImgLogo, logo)) {
            *error = "logo: " + err; return false;
        }
        for (int i = 0; i < kButtonCount; ++i) {
            for (int s = 0; s < kOrbStride; ++s) {
                const std::size_t idx = static_cast<std::size_t>(
                    kOrbBase + i * kOrbStride + s);
                if (!decode(title_il, idx, orb[i][s])) {
                    *error = "orb frame: " + err; return false;
                }
            }
        }
        decode(title_il, kImgOrbOverlay, orb_overlay);  // optional overlay

        // Bitmap font: glyph index == ASCII code (FontModule.Age10).
        glyphs.resize(256);
        for (std::size_t k = 0; k < font_il.entries().size(); ++k) {
            const std::uint32_t code = font_il.entries()[k].index;
            if (code < 256 && !glyphs[code].ok) decode(font_il, k, glyphs[code]);
        }

        for (int i = 0; i < kButtonCount; ++i) captions[i] = kDefaultCaptions[i];
        return true;
    }

    // The five-orb hit test (AoW.exe 0x00419488). Returns 0..4, or -1.
    int hit_test(int x, int y) const {
        const int bottom = scene_bottom();
        if (!(bottom - 125 < y && y < bottom)) return -1;
        const int w = static_cast<int>(orb[0][0].w);
        const int base = ((w + disp_w) >> 1) - 320;
        for (int i = 0; i < kButtonCount; ++i) {
            const int left = base + i * kButtonPitch;
            if (left < x && x < left + w) return i;
        }
        return -1;
    }

    // Orb state frame: 0 = up, 1 = lit, 2 = down - per the draw at 0x00419e89.
    int orb_state(int i, int hover) const {
        if (i == cursor_index && i == hover) return 2;
        if (cursor_index == -1 && i == armed_index) return 1;
        return 0;
    }

    // Logo fade fraction 0..100 (0 hides it). Mirrors 0x00419d88..0x00419e45.
    int logo_alpha() const {
        if (counter68 >= static_cast<std::uint32_t>(kTitleEnd)) return 0;
        if (counter64 <= static_cast<std::uint32_t>(kFadeInStart)) return 0;
        if (counter64 < static_cast<std::uint32_t>(kFadeInEnd)) {
            return static_cast<int>(counter64 - kFadeInStart) * 2;
        }
        if (counter68 <= static_cast<std::uint32_t>(kFadeOutBegin)) return 100;
        return static_cast<int>(kTitleEnd - counter68) * 5;
    }

    // Backdrop offset: x = -(((counter-25) + 490) mod image_width).
    int backdrop_x() const {
        const std::uint32_t c =
            counter64 > static_cast<std::uint32_t>(kFadeInStart)
                ? counter64 - kFadeInStart : 0u;
        return -static_cast<int>((c + kScrollBias) % background.w);
    }

    int text_width(const std::string& s) const {
        int total = 0;
        for (unsigned char c : s) {
            const Sprite& g = glyphs[c];
            if (g.ok) total += static_cast<int>(g.advance);
        }
        return total;
    }

    void draw_text(Canvas& cv, int cx, int y, const std::string& s,
                   std::uint8_t r, std::uint8_t g, std::uint8_t b) const {
        int pen = cx - text_width(s) / 2;
        for (unsigned char c : s) {
            const Sprite& gl = glyphs[c];
            if (!gl.ok) continue;
            cv.glyph(gl, pen, y, r, g, b);
            pen += static_cast<int>(gl.advance);
        }
    }

    // One frame of AoW.exe 0x00419c84.
    Canvas render(int hover) const {
        Canvas cv(disp_w, disp_h);
        cv.clear(0, 0, 0);  // the form's colour / MaskColor is black

        const int top = scene_top();
        const int bottom = scene_bottom();

        // 1. The panning backdrop, blitted twice so it wraps seamlessly.
        const int bx = backdrop_x();
        cv.blit(background, bx, top, 100, 0, top, disp_w, bottom);
        cv.blit(background, bx + static_cast<int>(background.w), top, 100,
                0, top, disp_w, bottom);

        // 2. The logo, faded in and out on the recovered timeline.
        const int alpha = logo_alpha();
        if (alpha > 0) {
            cv.blit(logo, logo_left(), top + kLogoDy, alpha, 0, top, disp_w,
                    bottom);
        }

        // 3. The five orbs on their 120 px pitch.
        for (int i = 0; i < kButtonCount; ++i) {
            cv.blit(orb[i][orb_state(i, hover)], button_left(i), top + kButtonY,
                    100, 0, top, disp_w, bottom);
            if (overlay && orb_overlay.ok) {
                cv.blit(orb_overlay, button_left(i), top + kButtonY, 100,
                        0, top, disp_w, bottom);
            }
        }

        // 4. The hovered orb's caption, centred on it in white (the object code
        //    sets 0x00FFFFFF before drawing).
        if (hover >= 0) {
            const int orb_w = static_cast<int>(orb[0][0].w);
            const int cx = button_left(hover) + orb_w / 2;
            draw_text(cv, cx, top + kLabelY, captions[hover], 255, 255, 255);
        }
        return cv;
    }
};

// ------------------------------------------------------------------ app/win32
struct App {
    std::string root;
    Scene scene;
    HWND hwnd = nullptr;
    int client_w = 0, client_h = 0;
    bool running = true;
    int frames_done = 0;
    int frames_limit = 0;      // 0 = unlimited
    bool headless = false;
    std::string render_png;
    int render_frame = 90;     // frame to render head-less (logo fully faded in)
    bool music = true;
    std::string music_path;
    std::thread audio_thread;
    std::atomic<bool> audio_stop{false};
};

App g;

// Detects the game root by walking up from `start` until Int\Scenes\title.ilb
// exists, so dropping the exe next to `Int/` "just works" (mirrors aow_view).
std::string find_root(std::string start) {
    std::error_code ec;
    fs::path p = start;
    for (int i = 0; i < 6 && !p.empty(); ++i) {
        if (fs::exists(p / "Int" / "Scenes" / "title.ilb", ec)) return p.string();
        fs::path parent = p.parent_path();
        if (parent == p) break;
        p = parent;
    }
    return start;
}

void play_music(const std::string& path) {
    aow::AudioOut out;
    std::string err;
    if (!out.open(44100, 2, 16, &err)) {
        std::fprintf(stderr, "menu: audio device unavailable (%s)\n", err.c_str());
        return;
    }
    aow::galaxy::Player mod(44100);
    if (!mod.load(path, &err)) {
        std::fprintf(stderr, "menu: cannot load %s (%s)\n", path.c_str(),
                     err.c_str());
        return;
    }
    mod.start();
    constexpr int kFrames = 22050;  // 0.5 s blocks
    std::vector<std::int16_t> chunk(kFrames * 2);
    while (!g.audio_stop.load()) {
        mod.render(chunk.data(), kFrames);
        out.play_raw(chunk.data(), kFrames);
    }
}

void do_action(int index) {
    // The five actions of AoW.exe 0x0041ae94. The replica reports them and
    // leaves the engine side to the rest of the port.
    static const char* kWhat[kButtonCount] = {
        "new game", "load game", "multiplayer", "options", "exit",
    };
    if (index < 0 || index >= kButtonCount) return;
    std::printf("menu: %s\n", kWhat[index]);
    std::fflush(stdout);
    if (index == 4) PostQuitMessage(0);
}

void blit_to_window(HDC hdc) {
    if (g.client_w <= 0 || g.client_h <= 0) return;
    const int hover = g.scene.hit_test(g.scene.mouse_x, g.scene.mouse_y);
    const Canvas cv = g.scene.render(hover);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cv.width();
    bmi.bmiHeader.biHeight = -cv.height();  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetStretchBltMode(hdc, COLORONCOLOR);
    StretchDIBits(hdc, 0, 0, g.client_w, g.client_h, 0, 0, cv.width(),
                  cv.height(), cv.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, 1, 33, nullptr);  // 30 fps - the form's FrameRate
        return 0;
    case WM_TIMER: {
        Scene& s = g.scene;
        s.counter64++;
        s.counter68++;
        // The real screen rolls the credits after frame 750 and then returns to
        // the logo; the replica restarts the title timeline there.
        if (s.counter68 >= static_cast<std::uint32_t>(kTitleEnd)) {
            s.counter64 = 0;
            s.counter68 = 0;
        }
        ++g.frames_done;
        InvalidateRect(hwnd, nullptr, FALSE);
        if (g.frames_limit && g.frames_done >= g.frames_limit) {
            PostQuitMessage(0);
        }
        return 0;
    }
    case WM_SIZE:
        g.client_w = LOWORD(lp);

// ---------------------------------------------------------------------- main
void print_usage() {
    std::printf(
        "aow_menu - Age of Wonders 1 title-screen replica\n"
        "usage: aow_menu [options]\n"
        "  --root <dir>          game install root (default: exe dir, then ..)\n"
        "  --width N             display width  (default 640)\n"
        "  --height N            display height (default 480)\n"
        "  --render <out.png>    render one frame head-less and exit\n"
        "  --frame N             frame to render with --render (default 90)\n"
        "  --frames N            quit after N frames (smoke test)\n"
        "  --captions a,b,c,d,e  hover captions for the five orbs\n"
        "  --overlay             enable the engine's TSprite16 orb overlay\n"
        "  --no-music            do not start Songs\\TITLE.IT\n");
}

bool split_captions(const std::string& s) {
    std::vector<std::string> parts;
    std::size_t start = 0;
    for (;;) {
        const std::size_t comma = s.find(',', start);
        parts.push_back(s.substr(start, comma == std::string::npos
                                            ? std::string::npos
                                            : comma - start));
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    if (parts.size() != static_cast<std::size_t>(kButtonCount)) return false;
    for (int i = 0; i < kButtonCount; ++i) g.scene.captions[i] = parts[i];
    return true;
}

int run_headless() {
    g.scene.counter64 = static_cast<std::uint32_t>(g.render_frame);
    g.scene.counter68 = static_cast<std::uint32_t>(g.render_frame);
    const int hover = -1;
    const Canvas cv = g.scene.render(hover);
    std::string err;
    if (!aow::write_png_rgba(g.render_png, cv.width(), cv.height(),
                             cv.to_rgba(), err)) {
        std::fprintf(stderr, "menu: %s\n", err.c_str());
        return 1;
    }
    std::printf("wrote %s  %dx%d  frame=%d  scene_top=%d  orb0_x=%d\n",
                g.render_png.c_str(), cv.width(), cv.height(), g.render_frame,
                g.scene.scene_top(), g.scene.button_left(0));
    return 0;
}

}  // namespace

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    std::string root;
    int width = 640, height = 480;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    for (int i = 1; i < argc; ++i) {
        const std::string a = acp_from_w(argv[i]);
        std::string v;
        auto next = [&]() { if (i + 1 < argc) v = acp_from_w(argv[++i]); };
        if (a == "--root") { next(); root = v; }
        else if (a == "--width") { next(); width = std::atoi(v.c_str()); }
        else if (a == "--height") { next(); height = std::atoi(v.c_str()); }
        else if (a == "--render") { next(); g.render_png = v; g.headless = true; }
        else if (a == "--frame") { next(); g.render_frame = std::atoi(v.c_str()); }
        else if (a == "--frames") { next(); g.frames_limit = std::atoi(v.c_str()); }
        else if (a == "--captions") {
            next();
            if (!split_captions(v)) {
                std::fprintf(stderr,
                             "menu: --captions needs five comma-separated "
                             "values\n");
                return 2;
            }
        } else if (a == "--overlay") { g.scene.overlay = true; }
        else if (a == "--no-music") { g.music = false; }
        else if (a == "-h" || a == "--help") { print_usage(); return 0; }
    }
    if (argv) LocalFree(argv);

    if (root.empty()) {
        char exe[MAX_PATH] = {0};
        GetModuleFileNameA(nullptr, exe, MAX_PATH);
        root = find_root(fs::path(exe).parent_path().string());
    }
    g.root = root;
    g.scene.disp_w = width > 0 ? width : 640;
    g.scene.disp_h = height > 0 ? height : 480;

    std::string err;
    if (!g.scene.load(root, &err)) {
        std::fprintf(stderr, "menu: %s\n", err.c_str());
        MessageBoxW(nullptr,
                    L"Could not load the title-screen assets.\n"
                    L"Run aow_menu from the game folder, or pass --root <dir>.",
                    kAppTitle, MB_ICONERROR);
        return 1;
    }
    if (g.headless) return run_headless();

    g.music_path = (fs::path(root) / "Songs" / "TITLE.IT").string();
    if (g.music && fs::exists(g.music_path)) {
        g.audio_thread = std::thread(play_music, g.music_path);
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = L"AoWMenuWindow";
    if (RegisterClassExW(&wc) == 0) return 1;

    RECT rc{0, 0, g.scene.disp_w, g.scene.disp_h};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g.hwnd = CreateWindowExW(0, wc.lpszClassName, kAppTitle,
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             rc.right - rc.left, rc.bottom - rc.top, nullptr,
                             nullptr, hInst, nullptr);
    if (!g.hwnd) return 1;

    ShowWindow(g.hwnd, nShow);
    UpdateWindow(g.hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g.audio_stop.store(true);
    if (g.audio_thread.joinable()) g.audio_thread.join();
    return 0;
}

        g.client_h = HIWORD(lp);
        return 0;
    case WM_MOUSEMOVE: {
        Scene& s = g.scene;
        s.mouse_x = GET_X_LPARAM(lp);
        s.mouse_y = GET_Y_LPARAM(lp);
        // WindowUpdate stores the hovered index in +0x7c.
        s.cursor_index = s.hit_test(s.mouse_x, s.mouse_y);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        Scene& s = g.scene;
        s.mouse_x = GET_X_LPARAM(lp);
        s.mouse_y = GET_Y_LPARAM(lp);
        // WindowMouseDown stores the hit index in +0x80.
        s.armed_index = s.hit_test(s.mouse_x, s.mouse_y);
        return 0;
    }
    case WM_LBUTTONUP: {
        Scene& s = g.scene;
        s.mouse_x = GET_X_LPARAM(lp);
        s.mouse_y = GET_Y_LPARAM(lp);
        const int hit = s.hit_test(s.mouse_x, s.mouse_y);
        // WindowMouseUp fires when the hit index still matches +0x7c, then
        // clears it.
        if (hit >= 0 && hit == s.cursor_index) {
            do_action(hit);
        }
        s.cursor_index = -1;
        return 0;
    }
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        blit_to_window(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;  // we paint every pixel
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
}

