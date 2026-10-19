// unitviewer.cpp - the AoW1 unit viewer (Win32 + GDI).
//
// Loads every unit of the game through aow::load_unit_catalog (Unitres.pfs +
// Unitgfx.pfs + Release.hss + *.ahl) and lets you browse them:
//
//   <- / -> / mouse wheel   previous / next unit   (its idle sprite)
//   , / .                   step the sprite frames (walk, attack, ...)
//   click / Enter           open the unit's detail page
//   Esc / Backspace         back to the browser
//   Home / End              first / last unit
//
// The detail page shows the name, the description, the seven combat stats and
// the unit's attributes. Sprites are decoded from each unit's UNITS\*.ILB with
// aow::IlbFile::decode_rgb and alpha-composited over the panel background so
// GDI's 32-bit BI_RGB blit shows them correctly.
#include <windows.h>
#include <shellapi.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <map>
#include <string>
#include <vector>

#include "aow/ilb.hpp"
#include "aow/png.hpp"
#include "aow/unit_catalog.hpp"

namespace {

constexpr wchar_t kAppTitle[] = L"Age of Wonders - Unit Viewer";
constexpr int kWinW = 960;
constexpr int kWinH = 640;
constexpr int kMargin = 24;
const COLORREF kBgWindow = RGB(12, 14, 20);
const COLORREF kBgPanel = RGB(26, 30, 44);
const COLORREF kColTitle = RGB(240, 220, 150);
const COLORREF kColText = RGB(214, 220, 232);
const COLORREF kColDim = RGB(150, 158, 176);
const COLORREF kColAccent = RGB(120, 200, 255);

struct Args {
    std::string root = "..";
    bool help = false;
    std::string png;      // head-less: render one view to this PNG and exit
    bool png_detail = false;
    int unit = -1;        // head-less: unit index to show
    int frame = -1;       // head-less: sprite frame to show
};

std::string to_narrow(const std::wstring& w) {
    if (w.empty()) return {};
    const int n = WideCharToMultiByte(CP_ACP, 0, w.c_str(),
                                      static_cast<int>(w.size()), nullptr, 0,
                                      nullptr, nullptr);
    std::string s(static_cast<std::size_t>(n), '\0');
    WideCharToMultiByte(CP_ACP, 0, w.c_str(), static_cast<int>(w.size()),
                        &s[0], n, nullptr, nullptr);
    return s;
}

std::wstring to_wide(const std::string& s) {
    if (s.empty()) return {};
    const int n = MultiByteToWideChar(CP_ACP, 0, s.c_str(),
                                      static_cast<int>(s.size()), nullptr, 0);
    std::wstring w(static_cast<std::size_t>(n), L'\0');
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), static_cast<int>(s.size()),
                        &w[0], n);
    return w;
}

Args parse_args() {
    Args a;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    for (int i = 1; i < argc; ++i) {
        const std::wstring w = argv[i];
        const std::string n = to_narrow(w);
        if (w == L"--help" || w == L"-h" || w == L"/?") {
            a.help = true;
        } else if (n == "--png" && i + 1 < argc) {
            a.png = to_narrow(argv[++i]);
        } else if (n == "--detail") {
            a.png_detail = true;
        } else if (n == "--unit" && i + 1 < argc) {
            a.unit = std::atoi(to_narrow(argv[++i]).c_str());
        } else if (n == "--frame" && i + 1 < argc) {
            a.frame = std::atoi(to_narrow(argv[++i]).c_str());
        } else if (!n.empty() && n[0] != '-') {
            a.root = n;
        }
    }
    if (argv != nullptr) LocalFree(argv);
    return a;
}

// ------------------------------------------------------------------ state
aow::UnitCatalog g_cat;
std::string g_root = "..";
int g_sel = 0;
int g_frame = 0;
bool g_detail = false;
int g_scale = 5;
HWND g_hwnd = nullptr;
HFONT g_font_title = nullptr;
HFONT g_font_head = nullptr;
HFONT g_font_body = nullptr;
HFONT g_font_mono = nullptr;

struct Frame {
    int w = 0;
    int h = 0;
    std::vector<std::uint8_t> bgra;  // opaque, composited over kBgPanel
};
struct SpriteSet {
    bool tried = false;
    std::vector<Frame> frames;
};
std::map<std::string, SpriteSet> g_sprites;

// Alpha-composites an RGBA sprite (from IlbFile::decode_rgb) over `bg` into an
// opaque BGRA buffer (what StretchDIBits with BI_RGB expects).
void composite(const std::vector<std::uint8_t>& rgba, int w, int h, COLORREF bg,
               std::vector<std::uint8_t>& out) {
    const int br = GetRValue(bg), bgr = GetGValue(bg), bb = GetBValue(bg);
    out.assign(static_cast<std::size_t>(w) * h * 4, 0);
    for (std::size_t i = 0; i < static_cast<std::size_t>(w) * h; ++i) {
        const int a = rgba[i * 4 + 3];
        const int r = (rgba[i * 4 + 0] * a + br * (255 - a)) / 255;
        const int g = (rgba[i * 4 + 1] * a + bgr * (255 - a)) / 255;
        const int b = (rgba[i * 4 + 2] * a + bb * (255 - a)) / 255;
        out[i * 4 + 0] = static_cast<std::uint8_t>(b);
        out[i * 4 + 1] = static_cast<std::uint8_t>(g);
        out[i * 4 + 2] = static_cast<std::uint8_t>(r);
        out[i * 4 + 3] = 255;
    }
}

// The decoded frames of one sprite library, decoded on first use.
const SpriteSet& get_sprites(const std::string& lib) {
    SpriteSet& set = g_sprites[lib];
    if (set.tried || lib.empty()) return set;
    set.tried = true;
    aow::IlbFile ilb;
    std::string err;
    const std::string path = g_root + "\\Images\\" + lib;
    if (!ilb.load(path, err)) return set;
    for (std::size_t i = 0; i < ilb.entries().size() && set.frames.size() < 256;
         ++i) {
        std::vector<std::uint8_t> rgba;
        std::uint32_t w = 0, h = 0;
        if (!ilb.decode_rgb(i, rgba, w, h, err)) continue;
        if (w == 0 || h == 0 || w > 512 || h > 512) continue;
        Frame f;
        f.w = static_cast<int>(w);
        f.h = static_cast<int>(h);
        composite(rgba, f.w, f.h, kBgPanel, f.bgra);
        set.frames.push_back(std::move(f));
    }
    return set;
}

const aow::UnitEntry* selected() {
    if (g_cat.units.empty()) return nullptr;
    g_sel = std::min(std::max(0, g_sel), static_cast<int>(g_cat.units.size()) - 1);
    return &g_cat.units[static_cast<std::size_t>(g_sel)];
}

// The sprite library of the selected unit (empty when it has none).
std::string selected_sprite_lib() {
    const aow::UnitEntry* u = selected();
    return (u != nullptr && u->gfx != nullptr) ? u->gfx->sprite_lib : std::string();
}

void clamp_frame() {
    const std::string lib = selected_sprite_lib();
    const SpriteSet& s = get_sprites(lib);
    const int n = static_cast<int>(s.frames.size());
    if (n <= 0) {
        g_frame = 0;
        return;
    }
    g_frame = std::min(std::max(0, g_frame), n - 1);
}

// ------------------------------------------------------------------ drawing
HFONT make_font(int height, bool bold, const wchar_t* face) {
    return CreateFontW(height, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, FALSE, FALSE,
                       FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, VARIABLE_PITCH, face);
}

void draw_text(HDC dc, HFONT font, COLORREF col, int x, int y, int w,
               const std::wstring& text, UINT flags = DT_LEFT | DT_SINGLELINE) {
    if (font != nullptr) SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, col);
    RECT r{x, y, x + w, y + 100000};
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &r, flags);
}

// Blits a composited frame scaled by an integer factor (nearest neighbour).
void draw_frame(HDC dc, const Frame& f, int x, int y, int scale) {
    if (f.w <= 0 || f.h <= 0) return;
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = f.w;
    bmi.bmiHeader.biHeight = -f.h;  // top-down rows
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    SetStretchBltMode(dc, COLORONCOLOR);
    StretchDIBits(dc, x, y, f.w * scale, f.h * scale, 0, 0, f.w, f.h,
                  f.bgra.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
}

void paint_browse(HDC dc, const RECT& rc) {
    const aow::UnitEntry* u = selected();
    const int w = rc.right - rc.left;
    if (u == nullptr) {
        draw_text(dc, g_font_head, kColText, 24, 24, w - 48,
                  L"No units loaded. Pass the game folder as an argument.");
        return;
    }
    const std::string lib = selected_sprite_lib();
    const SpriteSet& sprites = get_sprites(lib);
    clamp_frame();

    // Left: the sprite centred in a framed panel.
    const int panel_w = 380;
    RECT spr{kMargin, 96, kMargin + panel_w, rc.bottom - kMargin};
    HBRUSH br = CreateSolidBrush(kBgPanel);
    FillRect(dc, &spr, br);
    DeleteObject(br);
    if (!sprites.frames.empty()) {
        const Frame& f = sprites.frames[static_cast<std::size_t>(g_frame)];
        const int sx = spr.left + (panel_w - f.w * g_scale) / 2;
        const int sy = spr.top + ((spr.bottom - spr.top) - f.h * g_scale) / 2;
        draw_frame(dc, f, sx, sy, g_scale);
    } else {
        draw_text(dc, g_font_body, kColDim, spr.left + 20, spr.top + 20,
                  panel_w - 40, L"(no sprite)");
    }

    draw_text(dc, g_font_title, kColTitle, kMargin, 20, w - 48,
              to_wide(u->display_name), DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
    wchar_t pos[96];
    _snwprintf_s(pos, _TRUNCATE, L"unit %d / %d    %hs", g_sel + 1,
                 static_cast<int>(g_cat.units.size()), u->race_name.c_str());
    draw_text(dc, g_font_body, kColDim, kMargin, 62, w - 48, pos);

    // Right: quick stat block.
    const int px = spr.right + 28;
    const int pw = rc.right - px - kMargin;
    const wchar_t* kStats[] = {L"Attack", L"Defense", L"Damage", L"Hits",
                               L"Moves", L"Resistance", L"Level"};
    const unsigned kv[] = {u->attack, u->defense, u->damage, u->hit_points,
                           u->moves, u->resistance, u->level};
    int y = 112;
    for (int i = 0; i < 7; ++i) {
        wchar_t line[64];
        _snwprintf_s(line, _TRUNCATE, L"%-12s %u", kStats[i], kv[i]);
        draw_text(dc, g_font_mono, kColText, px, y, pw, line);
        y += 30;
    }
    y += 16;
    if (!sprites.frames.empty()) {
        wchar_t fr[64];
        _snwprintf_s(fr, _TRUNCATE, L"frame %d / %d   (, . to step)",
                     g_frame + 1, static_cast<int>(sprites.frames.size()));
        draw_text(dc, g_font_body, kColDim, px, y, pw, fr);
        y += 30;
    }
    draw_text(dc, g_font_body, kColAccent, px, y + 40, pw,
              L"Click or press Enter for the unit's page.",
              DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
}

void paint_detail(HDC dc, const RECT& rc) {
    const aow::UnitEntry* u = selected();
    if (u == nullptr) return;
    const int w = rc.right - rc.left;

    draw_text(dc, g_font_title, kColTitle, kMargin, 18, w - 48,
              to_wide(u->display_name), DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
    wchar_t sub[256];
    _snwprintf_s(sub, _TRUNCATE, L"#%u    %hs    level %u", u->id,
                 u->race_name.c_str(), u->level);
    draw_text(dc, g_font_body, kColDim, kMargin, 60, w - 48, sub);

    // Description block.
    RECT body{kMargin, 96, rc.right - kMargin, 258};
    HBRUSH br = CreateSolidBrush(kBgPanel);
    FillRect(dc, &body, br);
    DeleteObject(br);
    draw_text(dc, g_font_body, kColText, body.left + 14, body.top + 12,
              body.right - body.left - 28, to_wide(u->description),
              DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

    // Stats grid (two columns).
    const wchar_t* kStats[] = {L"Attack", L"Defense", L"Damage", L"Hits",
                               L"Moves", L"Resistance", L"Level", L"Upkeep"};
    const unsigned kv[] = {u->attack, u->defense, u->damage, u->hit_points,
                           u->moves, u->resistance, u->level,
                           static_cast<unsigned>((u->level + 1) * 2)};
    for (int i = 0; i < 8; ++i) {
        wchar_t line[64];
        _snwprintf_s(line, _TRUNCATE, L"%-12s %u", kStats[i], kv[i]);
        draw_text(dc, g_font_mono, kColText, kMargin + (i % 2) * 200,
                  288 + (i / 2) * 28, 190, line);
    }

    // Attributes column.
    const int ax = kMargin + 440;
    wchar_t attr[512];
    _snwprintf_s(attr, _TRUNCATE,
                 L"Race          %hs\n"
                 L"Unit type     %u\n"
                 L"Gender        %u\n"
                 L"Size          %u\n"
                 L"Blood type    %u\n"
                 L"Alignment     %u\n"
                 L"Transport     %u\n"
                 L"Obtain value  %u\n"
                 L"Sprite        %hs\n"
                 L"Face library  %hs",
                 u->race_name.c_str(), u->unit_type, u->gender, u->unit_size,
                 u->blood_type, u->alignment, u->transport_capacity,
                 u->obtain_value,
                 u->gfx ? u->gfx->sprite_lib.c_str() : "(none)",
                 u->gfx ? u->gfx->image_lib.c_str() : "(none)");
    draw_text(dc, g_font_mono, kColText, ax, 288, rc.right - ax - kMargin, attr,
              DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

    draw_text(dc, g_font_body, kColAccent, kMargin, rc.bottom - 40, w - 48,
              L"Esc / Backspace to go back.");
}

void on_paint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    HBRUSH bg = CreateSolidBrush(kBgWindow);
    FillRect(dc, &rc, bg);
    DeleteObject(bg);
    if (g_detail) {
        paint_detail(dc, rc);
    } else {
        paint_browse(dc, rc);
    }
    EndPaint(hwnd, &ps);
}

void repaint() { InvalidateRect(g_hwnd, nullptr, FALSE); }

void step_unit(int delta) {
    const int n = static_cast<int>(g_cat.units.size());
    if (n == 0) return;
    g_sel = ((g_sel + delta) % n + n) % n;
    g_frame = 0;
    clamp_frame();
    repaint();
}

void step_frame(int delta) {
    const std::string lib = selected_sprite_lib();
    const SpriteSet& s = get_sprites(lib);
    const int n = static_cast<int>(s.frames.size());
    if (n <= 0) return;
    g_frame = ((g_frame + delta) % n + n) % n;
    repaint();
}

void update_title() {
    const aow::UnitEntry* u = selected();
    std::wstring t = kAppTitle;
    if (u != nullptr) {
        t += L"  -  ";
        t += to_wide(u->display_name);
    }
    SetWindowTextW(g_hwnd, t.c_str());
}

void create_fonts() {
    g_font_title = make_font(-30, true, L"Georgia");
    g_font_head = make_font(-20, true, L"Segoe UI");
    g_font_body = make_font(-18, false, L"Segoe UI");
    g_font_mono = make_font(-17, false, L"Consolas");
}
void destroy_fonts() {
    if (g_font_title != nullptr) DeleteObject(g_font_title);
    if (g_font_head != nullptr) DeleteObject(g_font_head);
    if (g_font_body != nullptr) DeleteObject(g_font_body);
    if (g_font_mono != nullptr) DeleteObject(g_font_mono);
    g_font_title = g_font_head = g_font_body = g_font_mono = nullptr;
}

// Paints one view into an off-screen 32-bit DIB and writes it as a PNG. This is
// the head-less path (same painting code as the window), so the render can be
// checked without a display - matching aow_view's `--render`.
bool render_png(const std::string& out, int w, int h, bool detail,
                std::string& error) {
    create_fonts();
    HDC dc = CreateCompatibleDC(nullptr);
    if (dc == nullptr) {
        error = "CreateCompatibleDC failed";
        destroy_fonts();
        return false;
    }
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (bmp == nullptr || bits == nullptr) {
        error = "CreateDIBSection failed";
        DeleteDC(dc);
        destroy_fonts();
        return false;
    }
    const HGDIOBJ old = SelectObject(dc, bmp);
    const RECT rc{0, 0, w, h};
    HBRUSH bg = CreateSolidBrush(kBgWindow);
    FillRect(dc, &rc, bg);
    DeleteObject(bg);
    if (detail) {
        paint_detail(dc, rc);
    } else {
        paint_browse(dc, rc);
    }
    SelectObject(dc, old);

    const std::uint8_t* p = static_cast<const std::uint8_t*>(bits);
    std::vector<std::uint8_t> rgba(static_cast<std::size_t>(w) * h * 4);
    for (std::size_t i = 0; i < static_cast<std::size_t>(w) * h; ++i) {
        rgba[i * 4 + 0] = p[i * 4 + 2];  // B,G,R,A -> R,G,B,A
        rgba[i * 4 + 1] = p[i * 4 + 1];
        rgba[i * 4 + 2] = p[i * 4 + 0];
        rgba[i * 4 + 3] = 255;
    }
    DeleteObject(bmp);
    DeleteDC(dc);
    destroy_fonts();
    return aow::write_png_rgba(out, static_cast<std::uint32_t>(w),
                               static_cast<std::uint32_t>(h), rgba, error);
}

}  // namespace

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            g_hwnd = hwnd;
            create_fonts();
            update_title();
            return 0;

        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lp);
            mmi->ptMinTrackSize.x = 760;
            mmi->ptMinTrackSize.y = 520;
            return 0;
        }

        case WM_PAINT:
            on_paint(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;  // on_paint fills the background

        case WM_KEYDOWN:
            switch (wp) {
                case VK_LEFT:  step_unit(-1); return 0;
                case VK_RIGHT: step_unit(1); return 0;
                case VK_HOME:
                    g_sel = 0; g_frame = 0; clamp_frame(); repaint(); return 0;
                case VK_END:
                    g_sel = static_cast<int>(g_cat.units.size()) - 1;
                    g_frame = 0; clamp_frame(); repaint(); return 0;
                case VK_RETURN:
                    g_detail = !g_detail; repaint(); return 0;
                case VK_ESCAPE:
                case VK_BACK:
                    if (g_detail) { g_detail = false; repaint(); }
                    else if (wp == VK_ESCAPE) DestroyWindow(hwnd);
                    return 0;
                case VK_OEM_COMMA:  step_frame(-1); return 0;
                case VK_OEM_PERIOD: step_frame(1); return 0;
                case VK_ADD: case VK_OEM_PLUS:
                    g_scale = std::min(12, g_scale + 1); repaint(); return 0;
                case VK_SUBTRACT: case VK_OEM_MINUS:
                    g_scale = std::max(2, g_scale - 1); repaint(); return 0;
                default: break;
            }
            break;

        case WM_MOUSEWHEEL:
            step_unit(GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1);
            return 0;

        case WM_LBUTTONDOWN:
            // Browse -> detail; a further click goes back.
            g_detail = !g_detail;
            repaint();
            return 0;

        case WM_DESTROY:
            destroy_fonts();
            PostQuitMessage(0);
            return 0;

        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    const Args a = parse_args();
    if (a.help) {
        MessageBoxW(nullptr,
                    L"aow_unitview - Age of Wonders 1 unit viewer\n\n"
                    L"aow_unitview.exe [game folder]\n\n"
                    L"<- / -> / wheel   previous / next unit\n"
                    L", / .             step the sprite frames\n"
                    L"click / Enter     open the unit page\n"
                    L"Esc / Backspace   back / quit\n"
                    L"Home / End        first / last unit\n"
                    L"+ / -             zoom the sprite",
                    L"aow_unitview", MB_ICONINFORMATION);
        return 0;
    }
    g_root = a.root;

    std::string err;
    if (!aow::load_unit_catalog(g_root, g_cat, err)) {
        const std::string m = "Could not load the unit data from\n" + g_root +
                              "\n\n" + err +
                              "\n\nRun:  aow_unitview.exe <game folder>";
        if (a.png.empty()) {  // never block a head-less run on a dialog
            MessageBoxW(nullptr, to_wide(m).c_str(), L"aow_unitview",
                        MB_ICONERROR);
        }
        return 1;
    }

    if (a.unit >= 0) g_sel = a.unit;
    clamp_frame();
    if (a.frame >= 0) {
        g_frame = a.frame;
        clamp_frame();
    }

    // Head-less: render one view to a PNG and exit (used by the tests).
    if (!a.png.empty()) {
        g_detail = a.png_detail;
        std::string rerr;
        if (!render_png(a.png, kWinW, kWinH, g_detail, rerr)) {
            std::fprintf(stderr, "render failed: %s\n", rerr.c_str());
            return 2;
        }
        return 0;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"AoWUnitViewer";
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = wc.hIcon;
    if (RegisterClassExW(&wc) == 0) return 1;

    g_hwnd = CreateWindowExW(0, wc.lpszClassName, kAppTitle,
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             kWinW, kWinH, nullptr, nullptr, hInst, nullptr);
    if (g_hwnd == nullptr) return 1;

    ShowWindow(g_hwnd, (nShow != 0) ? nShow : SW_SHOWNORMAL);
    UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
