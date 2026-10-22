// tbviewer.cpp - AoWTBViewer: tactical battle map viewer.
//
// A small Win32/GDI front-end over aow::render_tb_map_rgba. It lists every
// TCMaps\*.HSM in the install, renders the selected map at 1:1, and lets the
// camera pan (arrow keys / WASD / mouse drag) and zoom (wheel / +/- / F fit).
//
// Head-less:
//   AoWTBViewer.exe --list --root <game dir>
//   AoWTBViewer.exe <TCMaps\map.HSM> --root <dir> --render out.png
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/mapviz.hpp"

namespace fs = std::filesystem;

namespace {

constexpr int kListWidth = 260;
constexpr int kStatusHeight = 26;

struct MapEntry {
    std::string path;
    std::string name;
};

struct App {
    HINSTANCE inst = nullptr;
    HWND win = nullptr;
    HWND list = nullptr;
    std::string root;
    std::vector<MapEntry> maps;
    int selected = -1;

    std::vector<aow::MapResource> combat;
    std::vector<aow::MapResource> world;

    aow::Map map;
    std::vector<std::uint8_t> rgba;
    std::uint32_t img_w = 0, img_h = 0;
    std::vector<std::uint8_t> dib_bgra;
    HBITMAP dib = nullptr;
    HDC memdc = nullptr;
    void* dib_bits = nullptr;
    HFONT font = nullptr;

    double zoom = 1.0;
    double cam_x = 0.0, cam_y = 0.0;
    bool dragging = false;
    POINT drag_pt{0, 0};
    std::string status;
};

App g;

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

std::string exe_dir() {
    wchar_t buf[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (n == 0) return ".";
    fs::path p(std::wstring(buf, n));
    return p.parent_path().string();
}

std::string trim_number(double v) {
    char b[64];
    std::snprintf(b, sizeof(b), "%.2f", v);
    return b;
}

// Resolves the game root: an explicit --root, else the exe directory when it
// carries Images/TCMaps, else its parent (port/bin -> port -> game root).
std::string resolve_root(const std::string& requested) {
    if (!requested.empty()) return requested;
    fs::path base = exe_dir();
    fs::path candidates[3] = {base, base.parent_path(), base.parent_path().parent_path()};
    for (const fs::path& c : candidates) {
        std::error_code ec;
        if (fs::exists(c / "TCMaps", ec) &&
            fs::exists(c / "Images" / "TCombat", ec)) {
            return c.string();
        }
    }
    return base.string();
}

void scan_maps() {
    g.maps.clear();
    std::error_code ec;
    const fs::path dir = fs::path(g.root) / "TCMaps";
    if (!fs::exists(dir, ec)) return;
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
        if (!it->is_regular_file(ec)) continue;
        const std::string ext = lower(it->path().extension().string());
        if (ext != ".hsm") continue;
        MapEntry e;
        e.path = it->path().string();
        e.name = it->path().stem().string();
        g.maps.push_back(std::move(e));
    }
    std::sort(g.maps.begin(), g.maps.end(), [](const MapEntry& a, const MapEntry& b) {
        return lower(a.name) < lower(b.name);
    });
}

bool load_resources(std::string& error) {
    g.combat.clear();
    g.world.clear();
    if (!aow::load_combat_resources(g.root + "\\Release\\comb_res.pfs", g.combat,
                                    error)) {
        return false;
    }
    std::string werr;
    if (!aow::load_map_resources(g.root + "\\Release\\Release.hss", g.world, werr)) {
        g.world.clear();  // border frame may fall back; the map still renders
    }
    return true;
}

void destroy_dib() {
    if (g.dib != nullptr) DeleteObject(g.dib);
    if (g.memdc != nullptr) DeleteDC(g.memdc);
    g.dib = nullptr;
    g.memdc = nullptr;
    g.dib_bits = nullptr;
    g.dib_bgra.clear();
}

bool build_dib(std::string& error) {
    destroy_dib();
    if (g.img_w == 0 || g.img_h == 0) {
        error = "empty image";
        return false;
    }
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = static_cast<LONG>(g.img_w);
    bi.bmiHeader.biHeight = -static_cast<LONG>(g.img_h);  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    g.memdc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    g.dib = CreateDIBSection(g.memdc, &bi, DIB_RGB_COLORS, &g.dib_bits, nullptr, 0);
    if (g.dib == nullptr || g.memdc == nullptr) {
        error = "CreateDIBSection failed";
        return false;
    }
    SelectObject(g.memdc, g.dib);
    g.dib_bgra.resize(static_cast<std::size_t>(g.img_w) * g.img_h * 4);
    for (std::size_t i = 0; i < g.dib_bgra.size(); i += 4) {
        g.dib_bgra[i] = g.rgba[i + 2];
        g.dib_bgra[i + 1] = g.rgba[i + 1];
        g.dib_bgra[i + 2] = g.rgba[i];
        g.dib_bgra[i + 3] = 255;
    }
    std::memcpy(g.dib_bits, g.dib_bgra.data(), g.dib_bgra.size());
    return true;
}

void fit_view() {
    RECT rc;
    GetClientRect(g.win, &rc);
    const int vw = std::max(1, static_cast<int>(rc.right) - kListWidth);
    const int vh = std::max(1, static_cast<int>(rc.bottom) - kStatusHeight);
    const double zx = static_cast<double>(vw) / std::max(1u, g.img_w);
    const double zy = static_cast<double>(vh) / std::max(1u, g.img_h);
    g.zoom = std::min(zx, zy);
    if (g.zoom > 1.0) g.zoom = 1.0;
    g.cam_x = (static_cast<double>(g.img_w) - vw / g.zoom) * 0.5;
    g.cam_y = (static_cast<double>(g.img_h) - vh / g.zoom) * 0.5;
}

void clamp_cam() {
    RECT rc;
    GetClientRect(g.win, &rc);
    const double vw = std::max(1, static_cast<int>(rc.right) - kListWidth);
    const double vh = std::max(1, static_cast<int>(rc.bottom) - kStatusHeight);
    if (g.zoom < 0.01) g.zoom = 0.01;
    const double vis_w = vw / g.zoom;
    const double vis_h = vh / g.zoom;
    if (vis_w >= g.img_w) g.cam_x = (g.img_w - vis_w) * 0.5;
    else g.cam_x = std::clamp(g.cam_x, 0.0, g.img_w - vis_w);
    if (vis_h >= g.img_h) g.cam_y = (g.img_h - vis_h) * 0.5;
    else g.cam_y = std::clamp(g.cam_y, 0.0, g.img_h - vis_h);
}

void set_status(const std::string& text) {
    g.status = text;
    InvalidateRect(g.win, nullptr, FALSE);
}

bool load_selected(int index) {
    if (index < 0 || static_cast<std::size_t>(index) >= g.maps.size()) return false;
    const MapEntry& e = g.maps[static_cast<std::size_t>(index)];
    std::string err;
    aow::Map m;
    if (!aow::load_map(e.path, m, err)) {
        MessageBoxA(g.win, err.c_str(), "AoWTBViewer - load failed", MB_ICONERROR);
        return false;
    }
    aow::TbMapVizOptions opt;
    opt.level = 0;
    std::vector<std::uint8_t> rgba;
    std::uint32_t w = 0, h = 0;
    if (!aow::render_tb_map_rgba(m, g.combat, g.world, g.root, opt, rgba, w, h,
                                 err)) {
        MessageBoxA(g.win, err.c_str(), "AoWTBViewer - render failed", MB_ICONERROR);
        return false;
    }
    g.map = std::move(m);
    g.rgba.swap(rgba);
    g.img_w = w;
    g.img_h = h;
    if (!build_dib(err)) {
        MessageBoxA(g.win, err.c_str(), "AoWTBViewer - DIB failed", MB_ICONERROR);
        return false;
    }
    g.selected = index;
    fit_view();
    clamp_cam();
    set_status(e.name + "   " + std::to_string(g.map.width) + "x" +
               std::to_string(g.map.height) + " hexes   zoom " +
               trim_number(g.zoom * 100.0) + "%   arrows/drag pan, wheel zoom, F fit");
    return true;
}

void paint_window(HWND win) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(win, &ps);
    RECT rc;
    GetClientRect(win, &rc);
    HBRUSH bg = CreateSolidBrush(RGB(24, 26, 32));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    const int vx = kListWidth;
    const int vy = 0;
    const int vw = std::max(0, static_cast<int>(rc.right) - kListWidth);
    const int vh = std::max(0, static_cast<int>(rc.bottom) - kStatusHeight);
    if (g.memdc != nullptr && vw > 0 && vh > 0) {
        const double z = g.zoom;
        const int dw = static_cast<int>(g.img_w * z + 0.5);
        const int dh = static_cast<int>(g.img_h * z + 0.5);
        const int dx = vx - static_cast<int>(g.cam_x * z + 0.5);
        const int dy = vy - static_cast<int>(g.cam_y * z + 0.5);
        SetStretchBltMode(hdc, COLORONCOLOR);
        SetBrushOrgEx(hdc, 0, 0, nullptr);
        StretchBlt(hdc, dx, dy, dw, dh, g.memdc, 0, 0,
                   static_cast<int>(g.img_w), static_cast<int>(g.img_h), SRCCOPY);
    }

    // Status bar.
    RECT sb{0, rc.bottom - kStatusHeight, rc.right, rc.bottom};
    HBRUSH bar = CreateSolidBrush(RGB(40, 44, 52));
    FillRect(hdc, &sb, bar);
    DeleteObject(bar);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(220, 224, 230));
    RECT tr = sb;
    tr.left += 8;
    DrawTextA(hdc, g.status.c_str(), -1, &tr,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    EndPaint(win, &ps);
}


void zoom_at(int mx, int my, double factor) {
    const int vx = std::max(1, mx - kListWidth);
    const int vy = std::max(1, my);
    const double ix = g.cam_x + vx / g.zoom;
    const double iy = g.cam_y + vy / g.zoom;
    g.zoom = std::clamp(g.zoom * factor, 0.1, 8.0);
    g.cam_x = ix - vx / g.zoom;
    g.cam_y = iy - vy / g.zoom;
    clamp_cam();
}

void pan_by(double dx, double dy) {
    g.cam_x += dx / g.zoom;
    g.cam_y += dy / g.zoom;
    clamp_cam();
}

LRESULT CALLBACK wnd_proc(HWND win, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            g.win = win;
            g.list = CreateWindowExA(
                WS_EX_CLIENTEDGE, "LISTBOX", "",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY |
                    LBS_NOINTEGRALHEIGHT,
                0, 0, kListWidth, 400, win, reinterpret_cast<HMENU>(1001),
                g.inst, nullptr);
            for (const MapEntry& e : g.maps) {
                SendMessageA(g.list, LB_ADDSTRING, 0,
                             reinterpret_cast<LPARAM>(e.name.c_str()));
            }
            g.font = CreateFontA(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
            if (g.font != nullptr) {
                SendMessageA(g.list, WM_SETFONT,
                             reinterpret_cast<WPARAM>(g.font), TRUE);
            }
            return 0;
        }
        case WM_SIZE: {
            RECT rc;
            GetClientRect(win, &rc);
            if (g.list != nullptr) {
                MoveWindow(g.list, 0, 0, kListWidth,
                           std::max(0, static_cast<int>(rc.bottom) -
                                           kStatusHeight),
                           TRUE);
            }
            clamp_cam();
            InvalidateRect(win, nullptr, FALSE);
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wp) == 1001 && HIWORD(wp) == LBN_SELCHANGE) {
                const int sel = static_cast<int>(
                    SendMessageA(g.list, LB_GETCURSEL, 0, 0));
                if (sel >= 0) {
                    load_selected(sel);
                    SetFocus(win);  // arrows/WASD now pan the camera
                }
            }
            return 0;
        }
        case WM_PAINT:
            paint_window(win);
            return 0;
        case WM_MOUSEWHEEL: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(win, &pt);
            if (pt.x >= kListWidth && pt.y < 32000) {
                const int delta = GET_WHEEL_DELTA_WPARAM(wp);
                zoom_at(pt.x, pt.y, delta > 0 ? 1.2 : 1.0 / 1.2);
                set_status("zoom " + trim_number(g.zoom * 100.0) + "%");
            }
            return 0;
        }
        case WM_LBUTTONDOWN: {
            const int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            if (x >= kListWidth && y < 32000) {
                g.dragging = true;
                g.drag_pt = POINT{x, y};
                SetCapture(win);
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (g.dragging) {
                const int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
                const int dx = x - g.drag_pt.x;
                const int dy = y - g.drag_pt.y;
                g.drag_pt = POINT{x, y};
                pan_by(-dx, -dy);
                InvalidateRect(win, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP:
            if (g.dragging) {
                g.dragging = false;
                ReleaseCapture();
            }
            return 0;
        case WM_KEYDOWN: {
            const int step = 48;
            switch (wp) {
                case VK_LEFT: case 'A': pan_by(-step, 0); break;
                case VK_RIGHT: case 'D': pan_by(step, 0); break;
                case VK_UP: case 'W': pan_by(0, -step); break;
                case VK_DOWN: case 'S': pan_by(0, step); break;
                case VK_ADD: case VK_OEM_PLUS:
                    zoom_at(kListWidth + 1, 1, 1.2); break;
                case VK_SUBTRACT: case VK_OEM_MINUS:
                    zoom_at(kListWidth + 1, 1, 1.0 / 1.2); break;
                case 'F': fit_view(); break;
                case '0':
                    g.zoom = 1.0;
                    g.cam_x = 0;
                    g.cam_y = 0;
                    clamp_cam();
                    break;
                default: break;
            }
            InvalidateRect(win, nullptr, FALSE);
            return 0;
        }
        case WM_DESTROY:
            destroy_dib();
            if (g.font != nullptr) DeleteObject(g.font);
            g.font = nullptr;
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcA(win, msg, wp, lp);
}


int run_gui(const std::string& start_map) {
    WNDCLASSA wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = g.inst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = "AoWTBViewerWindow";
    RegisterClassA(&wc);

    g.win = CreateWindowExA(
        0, wc.lpszClassName, "Age of Wonders - Tactical Battle Map Viewer",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1360, 900, nullptr,
        nullptr, g.inst, nullptr);
    if (g.win == nullptr) return 1;

    int start_index = -1;
    if (!start_map.empty()) {
        std::error_code ec;
        const fs::path want(start_map);
        for (std::size_t i = 0; i < g.maps.size(); ++i) {
            if (fs::equivalent(fs::path(g.maps[i].path), want, ec)) {
                start_index = static_cast<int>(i);
                break;
            }
        }
        if (start_index < 0) {
            for (std::size_t i = 0; i < g.maps.size(); ++i) {
                if (lower(fs::path(g.maps[i].path).filename().string()) ==
                    lower(want.filename().string())) {
                    start_index = static_cast<int>(i);
                    break;
                }
            }
        }
    }
    if (start_index < 0) start_index = 0;

    ShowWindow(g.win, SW_SHOW);
    UpdateWindow(g.win);
    if (start_index >= 0 && !g.maps.empty()) {
        SendMessageA(g.list, LB_SETCURSEL, static_cast<WPARAM>(start_index), 0);
        load_selected(start_index);
        SetFocus(g.win);
    } else {
        set_status("no TCMaps\\*.HSM found under " + g.root);
    }

    MSG msg;
    while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}

}  // namespace

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int) {
    g.inst = inst;
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        std::freopen("CONOUT$", "w", stdout);
        std::freopen("CONOUT$", "w", stderr);
    }
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::string root_arg, map_arg, render_path;
    bool list_only = false;
    if (argv != nullptr) {
        for (int i = 1; i < argc; ++i) {
            const std::wstring w = argv[i];
            const std::string a(w.begin(), w.end());
            if ((a == "--root" || a == "-r") && i + 1 < argc) {
                const std::wstring v = argv[++i];
                root_arg.assign(v.begin(), v.end());
            } else if (a == "--render" && i + 1 < argc) {
                const std::wstring v = argv[++i];
                render_path.assign(v.begin(), v.end());
            } else if (a == "--list") {
                list_only = true;
            } else if (!a.empty() && a[0] != '-') {
                map_arg = a;
            }
        }
        LocalFree(argv);
    }

    g.root = resolve_root(root_arg);
    std::string err;
    if (!load_resources(err)) {
        MessageBoxA(nullptr, err.c_str(), "AoWTBViewer - resources",
                    MB_ICONERROR);
        return 1;
    }
    scan_maps();

    if (list_only) {
        std::printf("%s  (%zu tactical maps)\n", g.root.c_str(), g.maps.size());
        for (const MapEntry& e : g.maps) std::printf("  %s\n", e.name.c_str());
        return 0;
    }
    if (!render_path.empty()) {
        if (map_arg.empty()) {
            std::fprintf(stderr, "error: --render needs a map path\n");
            return 2;
        }
        aow::Map m;
        if (!aow::load_map(map_arg, m, err)) {
            std::fprintf(stderr, "error: %s\n", err.c_str());
            return 1;
        }
        aow::TbMapVizOptions opt;
        if (!aow::render_tb_map_png(m, g.combat, g.world, g.root, opt,
                                    render_path, err)) {
            std::fprintf(stderr, "error: %s\n", err.c_str());
            return 1;
        }
        std::printf("wrote %s\n", render_path.c_str());
        return 0;
    }
    return run_gui(map_arg);
}

