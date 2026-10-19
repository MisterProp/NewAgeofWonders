// viewer.cpp - `aow_view`: a double-clickable GUI front-end for the AoW1 map
// renderer (Stage 2.2).
//
// Double-clicking `aow_view.exe` opens the native "Open" dialog; pick a
// `.HSM` / `.CSM` scenario map and the port renders it (terrain hexagons and
// object sprites resolved through Release.hss - see aow/mapviz.hpp) into a
// scrollable, zoomable window.
//
//   aow_view.exe [map] [--root <dir>] [--res <Release.hss>] [--level N]
//                [--no-objects] [--render <out.png>] [--max N]
//
// `--render <out.png>` renders head-less (no window) and exits - that is the
// mode the tests use. Otherwise the viewer window opens.
//
// The game root (the folder that holds `Release/Release.hss`) is auto-detected
// by walking up from the map file and then from this executable, so dropping
// the exe next to `Release/` "just works".
//
// Windows-only: Win32 + GDI + comdlg32. The rest of the port stays stdlib-only.
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
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <string>
#include <vector>

#include "aow/game_state.hpp"
#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/mapviz.hpp"
#include "aow/movement.hpp"
#include "aow/png.hpp"
#include "aow/unit_catalog.hpp"

namespace fs = std::filesystem;

namespace {

const wchar_t kAppTitle[] = L"aow_view - Age of Wonders 1 map viewer";

// ---------------------------------------------------------------- conversions
// The engine APIs take `std::string` and open it with std::ifstream, i.e. the
// native ANSI code page, so both directions use CP_ACP and round-trip exactly.
std::wstring w_from_acp(const std::string& s) {
    if (s.empty()) return {};
    const int n = MultiByteToWideChar(CP_ACP, 0, s.c_str(),
                                      static_cast<int>(s.size()), nullptr, 0);
    std::wstring w(static_cast<std::size_t>(n), L'\0');
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), static_cast<int>(s.size()),
                        w.data(), n);
    return w;
}

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

// ------------------------------------------------------------------ app state
enum : int {
    IDM_OPEN = 1001,
    IDM_RELOAD,
    IDM_SAVE,
    IDM_EXIT,
    IDM_OBJECTS,
    IDM_ZIN,
    IDM_ZOUT,
    IDM_Z100,
    IDM_ZFIT,
    IDM_ABOUT,
    IDM_LEVEL_BASE = 1100,  // + level index (maps have <= 4 levels)
};

// ------------------------------------------------------------------ movement
// The world-map movement feature (Stage 2.4, docs/15_MOVEMENT.md). The viewer
// keeps a pristine copy of the rendered map (`base`) and composites the
// Cursors.ilb path overlay / the walk-animation sprite into `bgra`, which
// on_paint blits. `dirty` is the image-space rectangle the last overlay touched,
// so restoring the base stays cheap on a big map.
struct MoveState {
    aow::GameState gs;                 // the scenario roster
    aow::UnitCatalog cat;              // Unitres/Unitres/Herores join
    std::array<aow::MovePointTable, 256> tables{};  // MovePointTables
    bool have_data = false;            // roster + terrain resolved

    std::uint8_t human = 0xff;         // the local human player's index
    bool selected = false;             // an army is selected
    int sel_x = -1, sel_y = -1;        // the selected army's hex
    aow::ArmyMove army;                // its folded table / budget / move types
    std::wstring status;               // title-bar feedback

    int hover_x = -1, hover_y = -1;    // the hovered hex
    aow::MovePath path;                // the hover path (source-first)
    bool have_overlay = false;         // `path` is drawn into `bgra`

    // Walk animation (aow/unit_anim.hpp). The army's data is relocated at once;
    // this animates its *sprite* across the path: each hex step takes
    // `GetMoveSteps` animation frames, the pose comes from the unit's own
    // walking sequence for the step's direction, and the anchor interpolates
    // between the two hex origins.
    bool anim = false;
    aow::WalkAnimation walk;
    int anim_clock = 0;                // AoWHSMap+0x40
    int anim_phase = 0;                // the per-unit (ptr>>2)&0xffff offset
    aow::MapUnit anim_unit;            // the moving army's lead unit (its poses)
    aow::MapHs anim_army;              // the whole army (its size banner)
    std::vector<std::uint8_t> sprite;  // the current pose (RGBA)
    int sprite_w = 0, sprite_h = 0, sprite_vx = 0, sprite_vy = 0;
    int dest_x = -1, dest_y = -1;      // the animation's destination

    std::vector<std::uint8_t> base;  // pristine canvas (BGRA)
    int dirty_x0 = 0, dirty_y0 = 0, dirty_x1 = 0, dirty_y1 = 0;
    bool dirty_valid = false;  // the dirty_* rectangle is live
    int drawn_scale = 1;       // the sprite scale the current overlay used

    // The map animation clock `AoWHSMap+0x40`: HSEngine.THSMap.NewFrame@5560d758
    // adds one per map frame. The building animations (a mine's sparkle, a wizard
    // tower's orb) are resolved from it, so the viewer ticks it on its own timer
    // and re-composites only the animated overlays (a whole level re-render is
    // ~28 ms). See docs/17_BUILDING_ANIM.md.
    int map_clock = 0;

    // While the walk animation plays, the army on its destination hex is left
    // out of the render so it is never drawn twice.
    int hide_level = -1;
    int hide_x = -1;
    int hide_y = -1;
};

struct App {
    std::string root;      // game root: the folder holding Release/Release.hss
    std::string res_path;  // <root>/Release/Release.hss
    std::string map_path;  // currently open map

    aow::Map map;
    std::vector<aow::MapResource> res;

    std::vector<std::uint8_t> bgra;  // canvas, top-down BGRA (GDI order)
    int cw = 0, ch = 0;              // canvas size in pixels

    int level = 0;
    bool objects = true;
    bool have_map = false;

    double zoom = 1.0;            // device px per image px
    int pan_x = 0, pan_y = 0;     // client top-left, in image px

    bool drag = false;
    bool drag_moved = false;  // the drag exceeded the click threshold
    POINT drag_pt{};
    int drag_px = 0, drag_py = 0;

    MoveState mv;
};

// The walk animation timer. One timer fire advances the walk by one animation
// frame (the engine's `TUnitHS.MoveNext` per frame), so a hex step takes
// `unit_move_steps` fires (6..12) - `kAnimStepMs` is the frame interval.
constexpr UINT_PTR kAnimTimer = 1;
constexpr UINT kAnimStepMs = 33;
// The map-clock timer: one fire advances `AoWHSMap+0x40` by one, so the building
// animations run at the engine's map frame rate. It only re-composites the
// animated overlays and the armies (draw_structure_anim), never the whole level.
constexpr UINT_PTR kMapTimer = 2;
constexpr UINT kMapStepMs = 33;

App g;
HWND g_hwnd = nullptr;
HMENU g_view_menu = nullptr;

const wchar_t kMapFilter[] =
    L"Age of Wonders maps (*.hsm;*.csm)\0*.hsm;*.csm\0"
    L"Game packs (*.hss)\0*.hss\0"
    L"All files (*.*)\0*.*\0\0";

// ----------------------------------------------------------- root detection
bool has_release(const fs::path& dir) {
    std::error_code ec;
    return fs::exists(dir / "Release" / "Release.hss", ec);
}

// Walks up from `start` (a file or directory) looking for Release/Release.hss.
std::string find_root_from(fs::path start) {
    std::error_code ec;
    if (!fs::is_directory(start, ec)) start = start.parent_path();
    for (int i = 0; i < 8 && !start.empty(); ++i) {
        if (has_release(start)) return start.string();
        const fs::path up = start.parent_path();
        if (up == start) break;
        start = up;
    }
    return {};
}

std::string exe_dir() {
    wchar_t buf[MAX_PATH] = L"";
    const DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (n == 0) return {};
    return fs::path(std::wstring(buf, n)).parent_path().string();
}

// ------------------------------------------------------------ headless output
// A GUI-subsystem binary has no console; when run from a terminal we borrow the
// parent's so `--render` can report progress.
void attach_parent_console() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) return;
    std::FILE* f = nullptr;
    (void)f;
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

// ------------------------------------------------------------------- rendering
// Re-renders the current level into g.bgra (BGRA, top-down - what StretchDIBits
// wants for a 32bpp BI_RGB DIB).
void mark_dirty(int x0, int y0, int x1, int y1);
// Composites the level's animated building overlays (mapviz's
// draw_structure_overlays), the water's ripples/fish/sparkles and the armies
// (draw_unit_overlays - the last world layer) into g.bgra at the current map
// clock.
void draw_structure_anim();

bool rerender() {
    g.bgra.clear();
    g.cw = g.ch = 0;
    if (!g.have_map) return true;

    aow::MapVizOptions opt;
    opt.level = g.level;
    opt.objects = g.objects;
    opt.max_dim = 0;  // full resolution; the viewer zooms instead
    // The walk animation hides the army on the hex it is moving onto.
    opt.hide_army_level = g.mv.hide_level;
    opt.hide_army_x = g.mv.hide_x;
    opt.hide_army_y = g.mv.hide_y;
    // The animated building sequences are left out of the cached level and
    // composited per map frame by draw_structure_anim (the engine re-draws its
    // whole scene each frame; a level re-render here is ~28 ms). The armies are
    // left out too: they are the last world layer, so a unit is never covered by
    // a building's animated sprite (docs/13_UNITS_ON_MAP.md).
    opt.no_animated_overlays = true;
    opt.no_units = true;

    std::vector<std::uint8_t> rgba;
    std::uint32_t w = 0, h = 0;
    std::string err;
    if (!aow::render_map_rgba(g.map, g.res, g.root, opt, rgba, w, h, err)) {
        MessageBoxW(g_hwnd, w_from_acp("Could not render this map:\n\n" + err).c_str(),
                    L"aow_view", MB_ICONERROR);
        return false;
    }
    g.cw = static_cast<int>(w);
    g.ch = static_cast<int>(h);
    g.bgra.assign(rgba.size(), 0);
    for (std::size_t i = 0; i + 3 < rgba.size(); i += 4) {
        g.bgra[i + 0] = rgba[i + 2];  // B
        g.bgra[i + 1] = rgba[i + 1];  // G
        g.bgra[i + 2] = rgba[i + 0];  // R
        g.bgra[i + 3] = 0;            // unused by BI_RGB
    }
    // The movement overlay composites into g.bgra on top of this pristine copy.
    g.mv.base = g.bgra;
    g.mv.dirty_valid = false;
    // The animated buildings are not in the pristine copy; composite their
    // current frame over it (the map-clock timer keeps them moving).
    draw_structure_anim();
    if (!g.mv.anim) g.mv.have_overlay = false;
    return true;
}

bool load_resources(bool complain) {
    if (!g.res.empty()) return true;
    if (g.res_path.empty()) {
        if (complain) {
            MessageBoxW(g_hwnd,
                        L"Release\\Release.hss was not found.\n\n"
                        L"Put aow_view.exe next to the game's Release folder, or "
                        L"start it with --root <game folder>.",
                        L"aow_view", MB_ICONERROR);
        }
        return false;
    }
    std::string err;
    if (!aow::load_map_resources(g.res_path, g.res, err)) {
        if (complain) {
            MessageBoxW(g_hwnd, w_from_acp("Could not read the resource table:\n" +
                                           g.res_path + "\n\n" + err).c_str(),
                        L"aow_view", MB_ICONERROR);
        }
        return false;
    }
    return true;
}

// ------------------------------------------------------------------ window UI
void update_title() {
    std::wstring t = L"aow_view";
    if (g.have_map) {
        t = w_from_acp(fs::path(g.map_path).filename().string());
        t += L"  -  level " + std::to_wstring(g.level);
        if (g.map.levels.size() > static_cast<std::size_t>(g.level)) {
            const aow::MapLevel& lv = g.map.levels[static_cast<std::size_t>(g.level)];
            t += L"  (" + std::to_wstring(lv.width) + L"x" +
                 std::to_wstring(lv.height) + L" hexes)";
        }
        t += L"  " + std::to_wstring(g.cw) + L"x" + std::to_wstring(g.ch) + L"px";
        wchar_t zb[32] = L"";
        std::swprintf(zb, 32, L"  %d%%", static_cast<int>(g.zoom * 100 + 0.5));
        t += zb;
        if (!g.objects) t += L"  [no objects]";
        // Movement feedback: the selection, its budget and the hovered path.
        if (g.mv.selected) {
            t += L"  |  army (" + std::to_wstring(g.mv.sel_x) + L"," +
                 std::to_wstring(g.mv.sel_y) + L")  mp " +
                 std::to_wstring(static_cast<int>(g.mv.army.budget));
            if (g.mv.have_overlay && !g.mv.path.empty()) {
                t += L"  -> (" + std::to_wstring(g.mv.hover_x) + L"," +
                     std::to_wstring(g.mv.hover_y) + L")  path " +
                     std::to_wstring(g.mv.path.total_cost) + L" mp";
                if (g.mv.path.total_cost > g.mv.army.budget) {
                    t += L" (" + std::to_wstring(aow::path_turns(
                                     g.mv.path.total_cost, g.mv.army.budget,
                                     g.mv.army.budget)) + L" turn(s))";
                }
            } else if (g.mv.hover_x >= 0) {
                t += L"  -> (" + std::to_wstring(g.mv.hover_x) + L"," +
                     std::to_wstring(g.mv.hover_y) + L")  no path";
            }
        } else {
            t += L"  |  click one of your armies (N cycles, Esc deselects)";
        }
        if (!g.mv.status.empty()) t += L"  -  " + g.mv.status;
    } else {
        t += L"  -  File > Open to pick a map";
    }
    SetWindowTextW(g_hwnd, t.c_str());
}

void clamp_pan() {
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    const int cw = static_cast<int>(rc.right - rc.left);
    const int ch = static_cast<int>(rc.bottom - rc.top);
    const double tw = g.cw * g.zoom, th = g.ch * g.zoom;
    const int max_x = std::max(0, static_cast<int>(std::ceil((tw - cw) / g.zoom)));
    const int max_y = std::max(0, static_cast<int>(std::ceil((th - ch) / g.zoom)));
    g.pan_x = std::min(std::max(0, g.pan_x), max_x);
    g.pan_y = std::min(std::max(0, g.pan_y), max_y);
}

void update_scrollbars() {
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    const int cw = static_cast<int>(rc.right - rc.left);
    const int ch = static_cast<int>(rc.bottom - rc.top);
    const int tw = static_cast<int>(g.cw * g.zoom);
    const int th = static_cast<int>(g.ch * g.zoom);

    SCROLLINFO si{};
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;

    si.nMax = (tw > 0) ? tw - 1 : 0;
    si.nPage = static_cast<UINT>(std::max(0, cw));
    si.nPos = static_cast<int>(g.pan_x * g.zoom);
    SetScrollInfo(g_hwnd, SB_HORZ, &si, TRUE);

    si.nMax = (th > 0) ? th - 1 : 0;
    si.nPage = static_cast<UINT>(std::max(0, ch));
    si.nPos = static_cast<int>(g.pan_y * g.zoom);
    SetScrollInfo(g_hwnd, SB_VERT, &si, TRUE);
}

void update_level_menu() {
    if (g_view_menu == nullptr) return;
    while (GetMenuItemCount(g_view_menu) > 0) {
        DeleteMenu(g_view_menu, 0, MF_BYPOSITION);
    }

    const int n = g.have_map ? static_cast<int>(g.map.levels.size()) : 0;
    if (n == 0) {
        AppendMenuW(g_view_menu, MF_STRING | MF_GRAYED, IDM_LEVEL_BASE,
                    L"(no map open)");
    } else {
        for (int i = 0; i < n; ++i) {
            std::wstring label = L"Level &" + std::to_wstring(i);
            if (i == 0) label += L"  (surface)";
            AppendMenuW(g_view_menu,
                        MF_STRING | ((i == g.level) ? MF_CHECKED : 0),
                        static_cast<UINT_PTR>(IDM_LEVEL_BASE + i), label.c_str());
        }
    }
    AppendMenuW(g_view_menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(g_view_menu, MF_STRING | (g.objects ? MF_CHECKED : 0), IDM_OBJECTS,
                L"Show &objects\tCtrl+D");
    AppendMenuW(g_view_menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(g_view_menu, MF_STRING, IDM_ZIN, L"Zoom &in\t+");
    AppendMenuW(g_view_menu, MF_STRING, IDM_ZOUT, L"Zoom &out\t-");
    AppendMenuW(g_view_menu, MF_STRING, IDM_Z100, L"Zoom &100%\t0");
    AppendMenuW(g_view_menu, MF_STRING, IDM_ZFIT, L"&Fit to window\tF");
    DrawMenuBar(g_hwnd);
}

void refresh(bool reset_pan) {
    if (reset_pan) g.pan_x = g.pan_y = 0;
    clamp_pan();
    update_scrollbars();
    update_title();
    InvalidateRect(g_hwnd, nullptr, FALSE);
}

// ------------------------------------------------------------------- movement
// See docs/15_MOVEMENT.md. The pristine canvas lives in `g.mv.base`; `g.bgra`
// carries the composited overlay.

// Restores the rectangle the last overlay painted from the pristine canvas.
void restore_dirty() {
    if (!g.mv.dirty_valid || g.bgra.empty() || g.mv.base.size() != g.bgra.size()) {
        g.mv.dirty_valid = false;
        return;
    }
    const int x0 = std::max(0, g.mv.dirty_x0);
    const int y0 = std::max(0, g.mv.dirty_y0);
    const int x1 = std::min(g.cw, g.mv.dirty_x1);
    const int y1 = std::min(g.ch, g.mv.dirty_y1);
    g.mv.dirty_valid = false;
    if (x1 <= x0 || y1 <= y0) return;
    const std::size_t row = static_cast<std::size_t>(x1 - x0) * 4;
    for (int y = y0; y < y1; ++y) {
        const std::size_t off =
            (static_cast<std::size_t>(y) * static_cast<std::size_t>(g.cw) +
             static_cast<std::size_t>(x0)) * 4;
        std::memcpy(g.bgra.data() + off, g.mv.base.data() + off, row);
    }
}

void mark_dirty(int x0, int y0, int x1, int y1) {
    if (!g.mv.dirty_valid) {
        g.mv.dirty_x0 = x0;
        g.mv.dirty_y0 = y0;
        g.mv.dirty_x1 = x1;
        g.mv.dirty_y1 = y1;
        g.mv.dirty_valid = true;
        return;
    }
    g.mv.dirty_x0 = std::min(g.mv.dirty_x0, x0);
    g.mv.dirty_y0 = std::min(g.mv.dirty_y0, y0);
    g.mv.dirty_x1 = std::max(g.mv.dirty_x1, x1);
    g.mv.dirty_y1 = std::max(g.mv.dirty_y1, y1);
}

// The overlay sprite scale: the game draws the arrows 1:1 on screen, so when the
// viewer has zoomed the map out the overlay is drawn larger in image space to
// compensate (never smaller - a 19x14 arrow must not shrink to a few pixels).
// Bounded so a redraw stays cheap even at the 5% zoom floor.
int overlay_scale() {
    if (g.zoom >= 1.0) return 1;
    int s = static_cast<int>(1.0 / g.zoom + 0.5);
    if (s < 1) s = 1;
    if (s > 8) s = 8;
    return s;
}

void mark_hex(int hx, int hy) {
    int sx = 0, sy = 0;
    aow::hex_origin(hx, hy, sx, sy);
    // The scaled sprite is centred on the hex centre, so its half-extent is
    // ~(32 * scale) from the hex origin.
    const int pad = 32 * std::max(1, g.mv.drawn_scale) + 32;
    mark_dirty(sx - pad, sy - pad, sx + pad, sy + pad);
}

// Composites the level's animated building overlays into g.bgra at the current
// map clock (mapviz's draw_structure_overlays; see docs/17_BUILDING_ANIM.md),
// then the water hexagons' ripples/fish/sparkles (draw_water_overlays; see
// docs/18_WATER_ANIM.md) and finally the armies (draw_unit_overlays; see
// docs/13_UNITS_ON_MAP.md) - the engine's own layer order, so a unit is never
// covered by a building's animated sprite or a ripple. The overlays' own bounding
// box grows the dirty rectangle, so the next redraw_overlay restores it from the
// pristine `base` before re-compositing.
//
// The water and unit requests are clipped to the visible viewport: the water
// hexagons and the armies are spread over the whole map, so their sprites' union
// box would otherwise be the entire map and the restore below would copy the whole
// canvas every frame. Every water hexagon's counter is still advanced (the per-hex
// state lives in mapviz), only the compositing is clipped.
void draw_structure_anim() {
    if (g.bgra.empty() || !g.objects || !g.have_map) return;
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    std::string err;
    if (aow::draw_structure_overlays(g.map, g.res, g.root, g.level,
                                     g.mv.map_clock, g.bgra, g.cw, g.ch,
                                     /*bgra=*/true, x0, y0, x1, y1, err)) {
        if (x1 > x0 && y1 > y0) mark_dirty(x0, y0, x1, y1);
    }
    RECT rc{};
    if (g_hwnd == nullptr || !GetClientRect(g_hwnd, &rc)) {
        rc.left = rc.top = 0;
        rc.right = g.cw;
        rc.bottom = g.ch;
    }
    const double z = (g.zoom > 0.0) ? g.zoom : 1.0;
    const int vw = static_cast<int>(std::ceil((rc.right - rc.left) / z));
    const int vh = static_cast<int>(std::ceil((rc.bottom - rc.top) / z));
    int wx0 = 0, wy0 = 0, wx1 = 0, wy1 = 0;
    if (!aow::draw_water_overlays(g.map, g.res, g.root, g.level,
                                  g.mv.map_clock, g.bgra, g.cw, g.ch,
                                  /*bgra=*/true, wx0, wy0, wx1, wy1, err,
                                  aow::WaterAnimOptions{}, g.pan_x - 32,
                                  g.pan_y - 32, g.pan_x + vw + 32,
                                  g.pan_y + vh + 32)) {
        return;
    }
    if (wx1 > wx0 && wy1 > wy0) mark_dirty(wx0, wy0, wx1, wy1);
    // The armies last, over the buildings and the water. While a walk plays the
    // destination army stays hidden - the moving sprite (draw_anim_sprite) stands
    // in for it.
    int ux0 = 0, uy0 = 0, ux1 = 0, uy1 = 0;
    if (!aow::draw_unit_overlays(g.map, g.root, g.level, g.mv.map_clock, g.bgra,
                                 g.cw, g.ch, /*bgra=*/true, ux0, uy0, ux1, uy1,
                                 err, g.mv.hide_level, g.mv.hide_x, g.mv.hide_y,
                                 g.pan_x - 32, g.pan_y - 32, g.pan_x + vw + 32,
                                 g.pan_y + vh + 32)) {
        return;
    }
    if (ux1 > ux0 && uy1 > uy0) mark_dirty(ux0, uy0, ux1, uy1);
}

const aow::MapLevel* cur_level() {
    if (!g.have_map || g.level < 0 ||
        static_cast<std::size_t>(g.level) >= g.map.levels.size()) {
        return nullptr;
    }
    return &g.map.levels[static_cast<std::size_t>(g.level)];
}

aow::MapHs* army_on_hex(int x, int y) {
    const aow::MapLevel* lv = cur_level();
    if (lv == nullptr || x < 0 || y < 0 ||
        static_cast<std::uint32_t>(x) >= lv->width ||
        static_cast<std::uint32_t>(y) >= lv->height) {
        return nullptr;
    }
    aow::MapTile& t =
        g.map.levels[static_cast<std::size_t>(g.level)]
            .tiles[static_cast<std::size_t>(y) * lv->width +
                   static_cast<std::size_t>(x)];
    for (aow::MapHs& hs : t.objects) {
        if (hs.class_id == aow::kMapArmyHsClassId && !hs.units.empty()) return &hs;
    }
    return nullptr;
}

bool is_human_army(const aow::MapHs& hs) {
    return g.mv.human != 0xff && hs.owner == g.mv.human;
}

// Loads (or reloads) the movement data for the current map/level: the roster,
// the unit catalogue, the terrain of every hex and the MovePointTables.
void mv_load() {
    const std::vector<std::uint8_t> keep_base = g.mv.base;
    g.mv = MoveState();
    g.mv.base = keep_base;
    g.mv.tables = aow::build_move_point_tables();
    if (!g.have_map || g.root.empty()) return;
    std::string err;
    aow::load_game_state(g.map, g.mv.gs, err);
    aow::load_unit_catalog(g.root, g.mv.cat, err);
    aow::fill_tile_terrain(g.map, static_cast<std::size_t>(g.level), g.res);
    for (const aow::Player& p : g.mv.gs.players) {
        if (p.type == aow::PlayerType::Human) {
            g.mv.human = p.index;
            break;
        }
    }
    g.mv.have_data = !g.mv.gs.players.empty();
}

// Fills g.mv.army for the army standing on (x,y). False when it is not the
// human's.
bool select_army(int x, int y) {
    aow::MapHs* a = army_on_hex(x, y);
    if (a == nullptr || !is_human_army(*a)) return false;
    std::vector<std::uint8_t> mts, mps;
    for (const aow::MapUnit& u : a->units) {
        mts.push_back(aow::unit_move_types(u));
        mps.push_back(aow::unit_move_points(g.mv.cat, u));
    }
    g.mv.army = aow::army_move(g.mv.tables, mts, mps);
    g.mv.selected = g.mv.army.valid;
    g.mv.sel_x = x;
    g.mv.sel_y = y;
    return g.mv.selected;
}

// Client pixels -> image pixels (the same transform on_paint uses).
bool image_from_client(int cx, int cy, double& ix, double& iy) {
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    const int cw = rc.right - rc.left;
    const int ch = rc.bottom - rc.top;
    const int tw = static_cast<int>(g.cw * g.zoom);
    const int th = static_cast<int>(g.ch * g.zoom);
    const int off_x = std::max(0, (cw - tw) / 2);
    const int off_y = std::max(0, (ch - th) / 2);
    ix = g.pan_x + (cx - off_x) / g.zoom;
    iy = g.pan_y + (cy - off_y) / g.zoom;
    return cx >= off_x && cy >= off_y && ix >= 0 && iy >= 0 && ix < g.cw &&
           iy < g.ch;
}

// The hex under a client point: the nearest hex centre to the image point (the
// inverse of HXtoHP; on the axis-aligned 48x32, 32px-pitch grid nearest-centre
// is equivalent to the engine's own hit test).
bool pick_hex(int cx, int cy, int& hx, int& hy) {
    const aow::MapLevel* lv = cur_level();
    if (lv == nullptr) return false;
    double ix = 0, iy = 0;
    if (!image_from_client(cx, cy, ix, iy)) return false;
    const int xf = static_cast<int>(
        std::floor((ix - aow::kHexFieldMargin) / aow::kHexPitchX));
    double best = 1e30;
    hx = hy = -1;
    for (int dx = -1; dx <= 1; ++dx) {
        const int x = xf + dx;
        if (x < 0 || static_cast<std::uint32_t>(x) >= lv->width) continue;
        const int yf = static_cast<int>(
            std::floor((iy - (x & 1) * aow::kHexStagger) / aow::kHexPitchY));
        for (int dy = -1; dy <= 1; ++dy) {
            const int y = yf + dy;
            if (y < 0 || static_cast<std::uint32_t>(y) >= lv->height) continue;
            int sx = 0, sy = 0;
            aow::hex_origin(x, y, sx, sy);
            const double ddx = ix - (sx + 24.0);
            const double ddy = iy - (sy + 16.0);
            const double d = ddx * ddx + ddy * ddy;
            if (d < best) {
                best = d;
                hx = x;
                hy = y;
            }
        }
    }
    return hx >= 0;
}

void redraw_overlay();

// Draws the moving army's sprite between the two hexes of the current step,
// using its own walking sequence for the step's direction (aow/unit_anim.hpp).
void draw_anim_sprite() {
    if (!g.mv.anim || !g.mv.walk.active()) return;
    int ax = 0, ay = 0, bx = 0, by = 0;
    aow::hex_origin(g.mv.walk.from_x(), g.mv.walk.from_y(), ax, ay);
    aow::hex_origin(g.mv.walk.to_x(), g.mv.walk.to_y(), bx, by);
    const double t = g.mv.walk.t();
    const int ix = static_cast<int>(ax + (bx - ax) * t);
    const int iy = static_cast<int>(ay + (by - ay) * t);
    // The engine's anchor (TUnitHS.Show@0x55784614): HXtoHP(hex) + the
    // sequence's xoff/yoff + the pose's own base+shift. Over the walk's last
    // few animation frames the walking pose fades into the standing pose it
    // hands over to (aow::walk_settle_weight), so the frame finish_move draws
    // the army on its new hex is a settle, not a pose snap.
    std::string err;
    // The army's ground shadow travels with it: `TArmyHS.ShowShadow@0x557912a8`
    // derives the very same anchor (HXtoHP + the sub-hex offset) and the shadow
    // object sits one show-priority layer below the body, so it is composited
    // first, at the walker's interpolated position.
    {
        std::vector<std::uint8_t> shadow;
        int shw = 0, shh = 0, shvx = 0, shvy = 0;
        if (aow::unit_shadow_rgba(g.mv.cat, g.root, g.mv.anim_unit,
                                  g.mv.walk.direction(), g.mv.anim_clock,
                                  g.mv.anim_phase, shadow, shw, shh, shvx,
                                  shvy, err)) {
            const int sox = ix + shvx;
            const int soy = iy + shvy;
            aow::blit_shadow_bgra(g.bgra, g.cw, g.ch, shadow, shw, shh, sox,
                                  soy);
            mark_dirty(sox, soy, sox + shw, soy + shh);
        }
    }
    g.mv.sprite.clear();
    const double settle =
        aow::walk_settle_weight(g.mv.walk.remaining(), aow::kAnimFramesPerTick);
    if (!aow::unit_settle_pose_rgba(g.mv.cat, g.root, g.mv.anim_unit,
                                    g.mv.walk.direction(), g.mv.anim_clock,
                                    g.mv.anim_phase, settle, g.mv.sprite,
                                    g.mv.sprite_w, g.mv.sprite_h, g.mv.sprite_vx,
                                    g.mv.sprite_vy, err)) {
        g.mv.sprite.clear();
        return;
    }
    const int ox = ix + g.mv.sprite_vx;
    const int oy = iy + g.mv.sprite_vy;
    aow::blit_sprite_bgra(g.bgra, g.cw, g.ch, g.mv.sprite, g.mv.sprite_w,
                          g.mv.sprite_h, ox, oy);
    mark_dirty(ox, oy, ox + g.mv.sprite_w, oy + g.mv.sprite_h);
    // `TArmyHS.Show` draws the army's size banner after the body, so it travels
    // with the walking sprite (the destination army stays hidden meanwhile).
    std::vector<std::uint8_t> banner;
    int bw = 0, bh = 0, bvx = 0, bvy = 0;
    if (aow::army_banner_rgba(g.map, g.mv.anim_army, g.root, ix, iy, banner,
                              bw, bh, bvx, bvy, err)) {
        const int bx = ix + bvx;
        const int by = iy + bvy;
        aow::blit_sprite_bgra(g.bgra, g.cw, g.ch, banner, bw, bh, bx, by);
        mark_dirty(bx, by, bx + bw, by + bh);
    }
}

// Restores the previous overlay and repaints the current one (the hover path or
// the walk-animation sprite).
void redraw_overlay() {
    if (g.bgra.empty()) return;
    restore_dirty();
    // The animated buildings are re-composited every map frame (their previous
    // rectangle was just restored from `base`).
    draw_structure_anim();
    if (g.mv.anim) {
        draw_anim_sprite();
    } else if (g.mv.have_overlay && !g.mv.path.empty()) {
        std::string err;
        g.mv.drawn_scale = overlay_scale();
        aow::draw_move_path_overlay(g.bgra, g.cw, g.ch, g.root, g.mv.path,
                                    g.mv.army.budget, g.mv.army.budget, err,
                                    true, g.mv.drawn_scale);
        for (const aow::MoveStep& s : g.mv.path.steps) mark_hex(s.x, s.y);
    }
    InvalidateRect(g_hwnd, nullptr, FALSE);
}

void set_hover_hex(int hx, int hy) {
    if (hx == g.mv.hover_x && hy == g.mv.hover_y) return;
    g.mv.hover_x = hx;
    g.mv.hover_y = hy;
    g.mv.path = aow::MovePath();
    g.mv.have_overlay = false;
    if (hx >= 0 && hy >= 0 && g.mv.selected) {
        const aow::MapLevel* lv = cur_level();
        if (lv != nullptr) {
            // The engine's flood-fill bound; a generous 0xFF lets the whole
            // beyond-this-turn trail be computed (dim arrows + dim X).
            g.mv.path = aow::find_move_path(g.mv.army.table, *lv, g.mv.sel_x,
                                            g.mv.sel_y, hx, hy, 0xFF);
            g.mv.have_overlay = !g.mv.path.empty();
        }
    }
    redraw_overlay();
    update_title();
}

void deselect_army() {
    if (g.mv.anim) return;
    g.mv.selected = false;
    g.mv.sel_x = g.mv.sel_y = -1;
    g.mv.path = aow::MovePath();
    g.mv.have_overlay = false;
    g.mv.hover_x = g.mv.hover_y = -1;
    redraw_overlay();
    update_title();
}

void centre_on(int hx, int hy) {
    int sx = 0, sy = 0;
    aow::hex_origin(hx, hy, sx, sy);
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    g.pan_x = sx + 24 - static_cast<int>((rc.right - rc.left) / (2 * g.zoom));
    g.pan_y = sy + 16 - static_cast<int>((rc.bottom - rc.top) / (2 * g.zoom));
    refresh(false);
}

// Selects the next human army on this level (and centres the view on it).
void cycle_army() {
    const aow::MapLevel* lv = cur_level();
    if (lv == nullptr || g.mv.human == 0xff) return;
    std::vector<std::pair<int, int>> found;
    for (std::uint32_t y = 0; y < lv->height; ++y) {
        for (std::uint32_t x = 0; x < lv->width; ++x) {
            const aow::MapTile& t =
                lv->tiles[static_cast<std::size_t>(y) * lv->width + x];
            for (const aow::MapHs& hs : t.objects) {
                if (hs.class_id == aow::kMapArmyHsClassId && !hs.units.empty() &&
                    is_human_army(hs)) {
                    found.emplace_back(static_cast<int>(x), static_cast<int>(y));
                }
            }
        }
    }
    if (found.empty()) {
        g.mv.status = L"no army of yours on this level";
        update_title();
        return;
    }
    std::size_t next = 0;
    for (std::size_t i = 0; i < found.size(); ++i) {
        if (found[i].first == g.mv.sel_x && found[i].second == g.mv.sel_y) {
            next = i + 1;
            break;
        }
    }
    if (next >= found.size()) next = 0;
    g.mv.selected = false;
    if (select_army(found[next].first, found[next].second)) {
        centre_on(found[next].first, found[next].second);
    }
    update_title();
}

void finish_move() {
    g.mv.anim = false;
    KillTimer(g_hwnd, kAnimTimer);
    g.mv.sprite.clear();
    g.mv.hide_level = -1;  // the army is drawn again, now on its new hex
    rerender();  // the army is now drawn on its new hex
    const int dx = g.mv.dest_x;
    const int dy = g.mv.dest_y;
    g.mv.path = aow::MovePath();
    g.mv.have_overlay = false;
    g.mv.hover_x = g.mv.hover_y = -1;
    g.mv.selected = false;
    if (select_army(dx, dy)) {
        g.mv.status = L"moved to (" + std::to_wstring(dx) + L"," +
                      std::to_wstring(dy) + L")";
    }
    refresh(false);
}

// Relocates the selected army to the hovered hex and walks it there. Only a hex
// the army can reach THIS turn moves (the engine otherwise reports "Not enough
// move points").
void start_move() {
    if (!g.mv.selected || g.mv.path.empty() || !g.mv.path.reached) return;
    if (g.mv.army.budget < g.mv.path.total_cost) {
        g.mv.status = L"not enough move points this turn";
        update_title();
        return;
    }
    const int dest_x = g.mv.path.steps.back().x;
    const int dest_y = g.mv.path.steps.back().y;
    const int src_x = g.mv.sel_x;
    const int src_y = g.mv.sel_y;
    if (dest_x == src_x && dest_y == src_y) return;
    // The direction the army leaves the source hex in (TUnitHS rw tag 9).
    const aow::MoveStep& a = g.mv.path.steps[g.mv.path.steps.size() - 2];
    const int facing = aow::hex_direction(a.x, a.y, dest_x, dest_y);
    // Move the AoWE.TArmyHS between the field tables.
    const aow::MapLevel& lv = *cur_level();
    aow::MapTile& from =
        g.map.levels[static_cast<std::size_t>(g.level)]
            .tiles[static_cast<std::size_t>(src_y) * lv.width +
                   static_cast<std::size_t>(src_x)];
    aow::MapHs moved;
    bool got = false;
    for (std::size_t i = 0; i < from.objects.size(); ++i) {
        if (from.objects[i].class_id == aow::kMapArmyHsClassId &&
            !from.objects[i].units.empty() &&
            is_human_army(from.objects[i])) {
            moved = std::move(from.objects[i]);
            from.objects.erase(from.objects.begin() +
                               static_cast<std::ptrdiff_t>(i));
            got = true;
            break;
        }
    }
    if (!got) return;
    if (facing >= 1 && facing <= 6) {
        moved.army_direction = static_cast<std::uint8_t>(facing);
    }
    aow::MapTile& to =
        g.map.levels[static_cast<std::size_t>(g.level)]
            .tiles[static_cast<std::size_t>(dest_y) * lv.width +
                   static_cast<std::size_t>(dest_x)];
    to.objects.push_back(std::move(moved));

    // The moved army is now on the destination hex; its lead unit is the sprite
    // the walk animates (docs/13_UNITS_ON_MAP.md).
    if (!to.objects.empty() && !to.objects.back().units.empty()) {
        g.mv.anim_unit = to.objects.back().units.front();
        g.mv.anim_army = to.objects.back();
    }
    // TUnitHS.GetMoveSteps@0x55784988 keys the per-hex frame count off the
    // unit-gfx resource's rw tag 10.
    int steps_per_hex = 8;
    if (const aow::UnitGraphics* ug =
            aow::find_unit_graphics(g.mv.cat, g.mv.anim_unit)) {
        steps_per_hex = aow::unit_move_steps(ug->move_class);
    }
    std::vector<std::pair<int, int>> hexes;
    hexes.reserve(g.mv.path.steps.size());
    for (const aow::MoveStep& s : g.mv.path.steps) hexes.emplace_back(s.x, s.y);
    // Render once with the destination army hidden; the moving sprite stands in
    // for it while it walks.
    g.mv.hide_level = g.level;
    g.mv.hide_x = dest_x;
    g.mv.hide_y = dest_y;
    const bool ok = rerender();
    if (!ok) {
        g.mv.hide_level = -1;
        return;
    }
    g.mv.anim_clock = 0;
    // The engine's per-unit phase is `(ptr >> 2) & 0xffff`; it is arbitrary, so
    // a walk is normally cut off mid-stride and the standing pose then replaces
    // a mid-stride pose in one frame - the lurch on arrival.  This move knows
    // its own length, so align the walking cycle with it: the stride finishes on
    // the walk's last pose (`<d>08`), the pose the standing pose (`<d>09`)
    // follows, and the unit settles into the idle sprite instead of snapping.
    // Falls back to the engine's own per-unit phase when the unit ships no
    // mode-4 walking cycle.
    g.mv.anim_phase = (src_x * 7 + src_y * 13) & 0xffff;
    {
        int cycle_len = 0, spacing = 0;
        if (aow::unit_walk_cycle(g.mv.cat, g.mv.anim_unit, facing, cycle_len,
                                 spacing)) {
            g.mv.anim_phase = aow::walk_settle_phase(
                static_cast<int>(hexes.size() - 1) * steps_per_hex,
                aow::kAnimFramesPerTick, cycle_len, spacing);
        }
    }
    g.mv.dest_x = dest_x;
    g.mv.dest_y = dest_y;
    if (!g.mv.walk.start(hexes, steps_per_hex)) {
        g.mv.hide_level = -1;
        rerender();
        return;
    }
    g.mv.anim = true;
    SetTimer(g_hwnd, kAnimTimer, kAnimStepMs, nullptr);
    redraw_overlay();
}

void tick_anim() {
    if (!g.mv.anim) {
        KillTimer(g_hwnd, kAnimTimer);
        return;
    }
    // Each timer fire advances `kAnimFramesPerTick` animation frames: the map clock
    // (the walk cycle's frame rate) and the sub-hex progress (the travel speed)
    // move together, so the one multiplier doubles both (TUnitHS.MoveNext).
    // `draw_anim_sprite` picks the pose from the clock and the step's direction.
    for (int i = 0; i < aow::kAnimFramesPerTick; ++i) {
        g.mv.anim_clock += aow::kAnimClockStep;
        if (!g.mv.walk.tick()) {
            finish_move();
            return;
        }
    }
    redraw_overlay();
}

// --------------------------------------------------------------------- painting
void on_paint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);
    HBRUSH bg = CreateSolidBrush(RGB(16, 16, 24));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    if (!g.bgra.empty() && g.cw > 0 && g.ch > 0) {
        const int cw = static_cast<int>(rc.right - rc.left);
        const int ch = static_cast<int>(rc.bottom - rc.top);
        const int tw = static_cast<int>(g.cw * g.zoom);
        const int th = static_cast<int>(g.ch * g.zoom);
        const int off_x = std::max(0, (cw - tw) / 2);  // centre a small image
        const int off_y = std::max(0, (ch - th) / 2);

        int sx = g.pan_x, sy = g.pan_y;
        sx = std::min(sx, g.cw);
        sy = std::min(sy, g.ch);
        int sw = static_cast<int>((cw - off_x) / g.zoom) + 2;
        int sh = static_cast<int>((ch - off_y) / g.zoom) + 2;
        sw = std::min(sw, g.cw - sx);
        sh = std::min(sh, g.ch - sy);

        if (sw > 0 && sh > 0) {
            BITMAPINFO bmi{};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = g.cw;
            bmi.bmiHeader.biHeight = -g.ch;  // negative: top-down rows
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            SetStretchBltMode(hdc, COLORONCOLOR);  // nearest neighbour (crisp)
            StretchDIBits(hdc, off_x, off_y,
                          static_cast<int>(sw * g.zoom),
                          static_cast<int>(sh * g.zoom), sx, sy, sw, sh,
                          g.bgra.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
        }
    }
    EndPaint(hwnd, &ps);
}

// ------------------------------------------------------------------------ zoom
// Changes the zoom keeping the image point under (ax, ay) - client pixels - in
// place.
void set_zoom(double z, int ax, int ay) {
    z = std::min(8.0, std::max(0.05, z));
    if (std::fabs(z - g.zoom) < 1e-9) return;
    const double ix = g.pan_x + ax / g.zoom;
    const double iy = g.pan_y + ay / g.zoom;
    g.zoom = z;
    g.pan_x = static_cast<int>(ix - ax / z);
    g.pan_y = static_cast<int>(iy - ay / z);
    refresh(false);
}

void zoom_centre(double factor) {
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    set_zoom(g.zoom * factor, static_cast<int>((rc.right - rc.left) / 2),
             static_cast<int>((rc.bottom - rc.top) / 2));
}

void zoom_fit() {
    if (g.cw <= 0 || g.ch <= 0) return;
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    const int cw = std::max(1, static_cast<int>(rc.right - rc.left));
    const int ch = std::max(1, static_cast<int>(rc.bottom - rc.top));
    g.zoom = std::min(static_cast<double>(cw) / g.cw,
                      static_cast<double>(ch) / g.ch);
    g.zoom = std::min(8.0, std::max(0.05, g.zoom));
    refresh(true);
}

void zoom_100() {
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    set_zoom(1.0, static_cast<int>((rc.right - rc.left) / 2),
             static_cast<int>((rc.bottom - rc.top) / 2));
}

// Fits down only: a map that already fits stays at 100%. The floor keeps the
// world map readable - the original game always draws it 1:1 (a hex is 32 px),
// and below ~50% the 48x32 hexagons (and the movement arrows drawn on them)
// collapse into a few pixels. F still fits the whole map on demand.
void zoom_initial() {
    if (g.cw <= 0 || g.ch <= 0) return;
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    const int cw = std::max(1, static_cast<int>(rc.right - rc.left));
    const int ch = std::max(1, static_cast<int>(rc.bottom - rc.top));
    const double fit = std::min(static_cast<double>(cw) / g.cw,
                                static_cast<double>(ch) / g.ch);
    g.zoom = std::min(1.0, std::max(0.5, fit));
    refresh(true);
}

void scroll_by(int dx_img, int dy_img) {
    g.pan_x += dx_img;
    g.pan_y += dy_img;
    refresh(false);
}

// ------------------------------------------------------------- map / level ops
void set_level(int lv) {
    if (!g.have_map) return;
    if (lv < 0 || static_cast<std::size_t>(lv) >= g.map.levels.size()) return;
    if (lv == g.level) return;
    g.level = lv;
    // The new level has its own hexes and its own armies: drop the selection and
    // reload the movement data (the terrain resolution is per level).
    g.mv.selected = false;
    g.mv.sel_x = g.mv.sel_y = -1;
    g.mv.path = aow::MovePath();
    g.mv.have_overlay = false;
    g.mv.hover_x = g.mv.hover_y = -1;
    mv_load();
    rerender();
    update_level_menu();
    refresh(true);
}

bool open_map(const std::string& path) {
    aow::Map m;
    std::string err;
    if (!aow::load_map(path, m, err)) {
        MessageBoxW(g_hwnd, w_from_acp("Could not read the map:\n" + path +
                                       "\n\n" + err).c_str(),
                    L"aow_view", MB_ICONERROR);
        return false;
    }
    // If no root was configured, try to derive one from the map we just opened.
    if (g.res_path.empty()) {
        const std::string r = find_root_from(path);
        if (!r.empty()) {
            g.root = r;
            g.res_path = (fs::path(r) / "Release" / "Release.hss").string();
        }
    }
    if (!load_resources(true)) return false;

    g.map = std::move(m);
    g.map_path = path;
    g.have_map = true;
    g.level = 0;
    mv_load();
    if (!rerender()) {
        g.have_map = false;
        return false;
    }
    update_level_menu();
    zoom_initial();
    // The map-clock timer (AoWHSMap+0x40): it advances the clock and re-composites
    // the animated buildings each frame (draw_structure_anim), so they animate
    // live like the engine's own scene redraw.
    SetTimer(g_hwnd, kMapTimer, kMapStepMs, nullptr);
    // Auto-select the human player's first army on this level and centre the
    // view on it, so the feature is discoverable.
    if (g.mv.human != 0xff) {
        const aow::MapLevel* lv = cur_level();
        bool found = false;
        for (std::uint32_t y = 0; lv != nullptr && y < lv->height && !found; ++y) {
            for (std::uint32_t x = 0; x < lv->width && !found; ++x) {
                const aow::MapTile& t =
                    lv->tiles[static_cast<std::size_t>(y) * lv->width + x];
                for (const aow::MapHs& hs : t.objects) {
                    if (hs.class_id == aow::kMapArmyHsClassId &&
                        !hs.units.empty() && is_human_army(hs)) {
                        if (select_army(static_cast<int>(x),
                                        static_cast<int>(y))) {
                            centre_on(static_cast<int>(x),
                                      static_cast<int>(y));
                        }
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    update_title();
    return true;
}

// -------------------------------------------------------------------- dialogs
bool choose_map(std::string& out) {
    wchar_t buf[MAX_PATH] = L"";
    std::wstring init_dir;
    if (!g.map_path.empty()) {
        const std::wstring name = w_from_acp(fs::path(g.map_path).filename().string());
        wcsncpy(buf, name.c_str(), MAX_PATH - 1);
        init_dir = w_from_acp(fs::path(g.map_path).parent_path().string());
    } else if (!g.root.empty()) {
        const fs::path scen = fs::path(g.root) / "Scenario";
        init_dir = w_from_acp(fs::exists(scen) ? scen.string() : g.root);
    }

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hwnd;
    ofn.lpstrFilter = kMapFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = buf;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = init_dir.empty() ? nullptr : init_dir.c_str();
    ofn.lpstrTitle = L"Open an Age of Wonders map";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER |
                OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&ofn)) return false;
    out = acp_from_w(buf);
    return true;
}

bool choose_save(std::string& out) {
    wchar_t buf[MAX_PATH] = L"";
    const std::string stem =
        g.map_path.empty() ? "map" : fs::path(g.map_path).stem().string();
    const std::string def = stem + "_L" + std::to_string(g.level) + ".png";
    const std::wstring wdef = w_from_acp(def);
    wcsncpy(buf, wdef.c_str(), MAX_PATH - 1);
    std::wstring init_dir;
    if (!g.map_path.empty()) {
        init_dir = w_from_acp(fs::path(g.map_path).parent_path().string());
    }

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hwnd;
    ofn.lpstrFilter = L"PNG image (*.png)\0*.png\0All files (*.*)\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = buf;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = init_dir.empty() ? nullptr : init_dir.c_str();
    ofn.lpstrDefExt = L"png";
    ofn.lpstrTitle = L"Save the rendered map as PNG";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER |
                OFN_NOCHANGEDIR;
    if (!GetSaveFileNameW(&ofn)) return false;
    out = acp_from_w(buf);
    return true;
}

// Folder picker, used only when Release/Release.hss could not be located.
std::string browse_for_root() {
    BROWSEINFOW bi{};
    bi.hwndOwner = g_hwnd;
    bi.lpszTitle =
        L"Select the Age of Wonders 1 folder (the one containing Release\\Release.hss)";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST idl = SHBrowseForFolderW(&bi);
    if (idl == nullptr) return {};
    wchar_t path[MAX_PATH] = L"";
    const bool ok = SHGetPathFromIDListW(idl, path) != FALSE;
    CoTaskMemFree(idl);
    return ok ? acp_from_w(path) : std::string();
}

void do_save_png() {
    if (g.bgra.empty()) {
        MessageBoxW(g_hwnd, L"Open a map first.", L"aow_view", MB_ICONINFORMATION);
        return;
    }
    std::string out;
    if (!choose_save(out)) return;

    std::vector<std::uint8_t> rgba(g.bgra.size());
    for (std::size_t i = 0; i + 3 < g.bgra.size(); i += 4) {
        rgba[i + 0] = g.bgra[i + 2];
        rgba[i + 1] = g.bgra[i + 1];
        rgba[i + 2] = g.bgra[i + 0];
        rgba[i + 3] = 255;
    }
    std::string err;
    if (aow::write_png_rgba(out, static_cast<std::uint32_t>(g.cw),
                            static_cast<std::uint32_t>(g.ch), rgba, err)) {
        MessageBoxW(g_hwnd, w_from_acp("Saved:\n" + out).c_str(), L"aow_view",
                    MB_ICONINFORMATION);
    } else {
        MessageBoxW(g_hwnd, w_from_acp("Save failed:\n" + err).c_str(), L"aow_view",
                    MB_ICONERROR);
    }
}

void do_open() {
    std::string p;
    if (choose_map(p)) open_map(p);
}

void do_about() {
    MessageBoxW(
        g_hwnd,
        L"aow_view - Age of Wonders 1 map viewer (port Stage 2.2)\n\n"
        L"Renders a scenario/campaign map (.HSM/.CSM) using the world sprite\n"
        L"table in Release\\Release.hss.\n\n"
        L"File > Open...\tCtrl+O\n"
        L"File > Save PNG...\tCtrl+S\n"
        L"View > Level 0/1/2\tsurface / underground\n"
        L"View > Show objects\tCtrl+D\n"
        L"Mouse wheel / + / -\tzoom (at the cursor)\n"
        L"Drag with the left button, or the arrow keys\tpan\n"
        L"F / 0\tfit to window / 100%\n"
        L"PageUp / PageDown\tprevious / next level\n\n"
        L"World-map movement (the port's Stage 2.4 - docs/15_MOVEMENT.md):\n"
        L"  Your army on the map is auto-selected and the view centred on it.\n"
        L"  Left-click one of your armies\tselect it\n"
        L"  Hover a hex\tpreviews the path with the game's own Cursors.ilb\n"
        L"                       arrows; a dim arrow / dim X means the hex is\n"
        L"                       beyond this turn's move points\n"
        L"  Left-click a hex\twalks the army there (if reachable this turn)\n"
        L"  N\tcycles your armies; Escape deselects\n"
        L"  The map opens at >=50% so the hexes (and the arrows) stay readable;\n"
        L"  F fits the whole map, the wheel zooms.\n\n"
        L"You can also drag a .hsm/.csm file onto this window.",
        L"About aow_view", MB_ICONINFORMATION);
}

// ----------------------------------------------------------------------- menu
void build_menu() {
    HMENU bar = CreateMenu();

    HMENU file = CreatePopupMenu();
    AppendMenuW(file, MF_STRING, IDM_OPEN, L"&Open map...\tCtrl+O");
    AppendMenuW(file, MF_STRING, IDM_RELOAD, L"&Reload\tF5");
    AppendMenuW(file, MF_STRING, IDM_SAVE, L"&Save PNG...\tCtrl+S");
    AppendMenuW(file, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(file, MF_STRING, IDM_EXIT, L"E&xit\tCtrl+Q");
    AppendMenuW(bar, MF_POPUP, reinterpret_cast<UINT_PTR>(file), L"&File");

    g_view_menu = CreatePopupMenu();
    AppendMenuW(bar, MF_POPUP, reinterpret_cast<UINT_PTR>(g_view_menu), L"&View");

    HMENU help = CreatePopupMenu();
    AppendMenuW(help, MF_STRING, IDM_ABOUT, L"&About aow_view");
    AppendMenuW(bar, MF_POPUP, reinterpret_cast<UINT_PTR>(help), L"&Help");

    SetMenu(g_hwnd, bar);
}

HACCEL build_accels() {
    ACCEL a[] = {
        {FVIRTKEY | FCONTROL, 'O', IDM_OPEN},
        {FVIRTKEY | FCONTROL, 'S', IDM_SAVE},
        {FVIRTKEY | FCONTROL, 'Q', IDM_EXIT},
        {FVIRTKEY | FCONTROL, 'D', IDM_OBJECTS},
        {FVIRTKEY, VK_F5, IDM_RELOAD},
        {FVIRTKEY, VK_ADD, IDM_ZIN},
        {FVIRTKEY, VK_SUBTRACT, IDM_ZOUT},
    };
    return CreateAcceleratorTableW(a, static_cast<int>(sizeof(a) / sizeof(a[0])));
}

void do_command(HWND hwnd, int id) {
    if (id >= IDM_LEVEL_BASE && id < IDM_LEVEL_BASE + 64) {
        set_level(id - IDM_LEVEL_BASE);
        return;
    }
    switch (id) {
        case IDM_OPEN: do_open(); break;
        case IDM_RELOAD:
            if (g.have_map) open_map(g.map_path);
            break;
        case IDM_SAVE: do_save_png(); break;
        case IDM_EXIT: DestroyWindow(hwnd); break;
        case IDM_OBJECTS:
            g.objects = !g.objects;
            rerender();
            update_level_menu();
            refresh(false);
            break;
        case IDM_ZIN: zoom_centre(1.25); break;
        case IDM_ZOUT: zoom_centre(1.0 / 1.25); break;
        case IDM_Z100: zoom_100(); break;
        case IDM_ZFIT: zoom_fit(); break;
        case IDM_ABOUT: do_about(); break;
        default: break;
    }
}

// ------------------------------------------------------------------ window proc
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            // g_hwnd is assigned only once CreateWindowExW returns, so the
            // helpers below (which use it) need it set here first.
            g_hwnd = hwnd;
            build_menu();
            update_level_menu();
            DragAcceptFiles(hwnd, TRUE);
            update_title();
            return 0;

        case WM_SIZE:
            clamp_pan();
            update_scrollbars();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lp);
            mmi->ptMinTrackSize.x = 480;
            mmi->ptMinTrackSize.y = 320;
            return 0;
        }

        case WM_PAINT:
            on_paint(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;  // on_paint fills the background

        case WM_COMMAND:
            do_command(hwnd, static_cast<int>(LOWORD(wp)));
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(hwnd, &pt);
            const int delta = GET_WHEEL_DELTA_WPARAM(wp);
            set_zoom(g.zoom * ((delta > 0) ? 1.25 : 1.0 / 1.25), pt.x, pt.y);
            return 0;
        }

        case WM_LBUTTONDOWN:
            SetCapture(hwnd);
            g.drag = true;
            g.drag_moved = false;
            g.drag_pt.x = GET_X_LPARAM(lp);
            g.drag_pt.y = GET_Y_LPARAM(lp);
            g.drag_px = g.pan_x;
            g.drag_py = g.pan_y;
            return 0;

        case WM_MOUSEMOVE:
            if (g.drag) {
                const int dx = GET_X_LPARAM(lp) - g.drag_pt.x;
                const int dy = GET_Y_LPARAM(lp) - g.drag_pt.y;
                if (std::abs(dx) > 3 || std::abs(dy) > 3) g.drag_moved = true;
                g.pan_x = g.drag_px - static_cast<int>(dx / g.zoom);
                g.pan_y = g.drag_py - static_cast<int>(dy / g.zoom);
                refresh(false);
            } else if (g.mv.selected && !g.mv.anim) {
                // Hover: preview the movement trail with the game's own arrows.
                int hx = -1, hy = -1;
                pick_hex(GET_X_LPARAM(lp), GET_Y_LPARAM(lp), hx, hy);
                set_hover_hex(hx, hy);
            }
            return 0;

        case WM_TIMER:
            if (wp == kAnimTimer) {
                tick_anim();
                return 0;
            }
            if (wp == kMapTimer) {
                // HSEngine.THSMap.NewFrame@5560d758: one clock tick per map frame.
                ++g.mv.map_clock;
                redraw_overlay();
                return 0;
            }
            break;

        case WM_LBUTTONUP:
            if (g.drag) {
                const bool was_drag = g.drag_moved;
                g.drag = false;
                g.drag_moved = false;
                ReleaseCapture();
                // A click (no drag) selects an army, or moves the selected one.
                if (!was_drag && !g.mv.anim) {
                    int hx = -1, hy = -1;
                    pick_hex(GET_X_LPARAM(lp), GET_Y_LPARAM(lp), hx, hy);
                    if (hx >= 0) {
                        aow::MapHs* a = army_on_hex(hx, hy);
                        if (a != nullptr && is_human_army(*a)) {
                            g.mv.status.clear();
                            select_army(hx, hy);
                            set_hover_hex(-1, -1);
                            update_title();
                        } else if (g.mv.selected) {
                            set_hover_hex(hx, hy);
                            start_move();
                        } else {
                            deselect_army();
                        }
                    }
                }
            }
            return 0;

        case WM_HSCROLL:
        case WM_VSCROLL: {
            const int bar = (msg == WM_HSCROLL) ? SB_HORZ : SB_VERT;
            SCROLLINFO si{};
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, bar, &si);
            int pos = si.nPos;
            switch (LOWORD(wp)) {
                case SB_LINELEFT: pos -= 24; break;
                case SB_LINERIGHT: pos += 24; break;
                case SB_PAGELEFT: pos -= static_cast<int>(si.nPage); break;
                case SB_PAGERIGHT: pos += static_cast<int>(si.nPage); break;
                case SB_THUMBTRACK:
                case SB_THUMBPOSITION: pos = si.nTrackPos; break;
                default: return 0;
            }
            const int maxpos = std::max(0, static_cast<int>(si.nMax) -
                                               static_cast<int>(si.nPage) + 1);
            pos = std::min(std::max(0, pos), maxpos);
            si.fMask = SIF_POS;
            si.nPos = pos;
            SetScrollInfo(hwnd, bar, &si, TRUE);
            if (bar == SB_HORZ) {
                g.pan_x = static_cast<int>(pos / g.zoom);
            } else {
                g.pan_y = static_cast<int>(pos / g.zoom);
            }
            clamp_pan();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_KEYDOWN:
            switch (wp) {
                case VK_LEFT: scroll_by(-32, 0); return 0;
                case VK_RIGHT: scroll_by(32, 0); return 0;
                case VK_UP: scroll_by(0, -32); return 0;
                case VK_DOWN: scroll_by(0, 32); return 0;
                case VK_ADD: case VK_OEM_PLUS: zoom_centre(1.25); return 0;
                case VK_SUBTRACT: case VK_OEM_MINUS: zoom_centre(1.0 / 1.25); return 0;
                case 'F': zoom_fit(); return 0;
                case '0': case VK_NUMPAD0: zoom_100(); return 0;
                case 'O': do_open(); return 0;
                case VK_PRIOR: set_level(g.level - 1); return 0;
                case VK_NEXT: set_level(g.level + 1); return 0;
                // World-map movement: cycle the human's armies / deselect.
                case 'N': cycle_army(); return 0;
                case VK_ESCAPE: deselect_army(); return 0;
                default: break;
            }
            break;

        case WM_DROPFILES: {
            HDROP hd = reinterpret_cast<HDROP>(wp);
            wchar_t buf[MAX_PATH] = L"";
            if (DragQueryFileW(hd, 0, buf, MAX_PATH) != 0) {
                open_map(acp_from_w(buf));
            }
            DragFinish(hd);
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hwnd, kAnimTimer);
            PostQuitMessage(0);
            return 0;

        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// -------------------------------------------------------------- command line
struct Args {
    std::string map;
    std::string root;
    std::string res;
    std::string render;
    std::string debug_csv;  // --debug-csv: dump per-hex resolution to a CSV
    int level = 0;
    int max_dim = 0;  // --max: head-less render size cap (0 = full resolution)
    // --clock N: the map animation clock `AoWHSMap+0x40` a head-less render bakes
    // the building animations at (see MapVizOptions::clock). The default 0 is the
    // deterministic frame.
    int clock = 0;
    // --no-anim: leave the ANIMATED building sequences out of the render
    // (MapVizOptions::no_animated_overlays), i.e. the terrain + static bodies
    // only. Used to measure what a glow overlay adds on top of the terrain.
    bool no_anim = false;
    bool objects = true;
    bool help = false;
    // Head-less proof mode: draw the human's selected army's movement path (the
    // game's own Cursors.ilb arrows + X) to this hex into the rendered PNG.
    int move_x = -1, move_y = -1;  // --move X,Y
    int army_x = -1, army_y = -1;  // --army X,Y (default: the first human army)
};

Args parse_args() {
    Args a;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv == nullptr) return a;
    for (int i = 1; i < argc; ++i) {
        const std::string s = acp_from_w(argv[i]);
        if (s == "--root" && i + 1 < argc) {
            a.root = acp_from_w(argv[++i]);
        } else if (s == "--res" && i + 1 < argc) {
            a.res = acp_from_w(argv[++i]);
        } else if (s == "--render" && i + 1 < argc) {
            a.render = acp_from_w(argv[++i]);
        } else if (s == "--debug-csv" && i + 1 < argc) {
            a.debug_csv = acp_from_w(argv[++i]);
        } else if (s == "--level" && i + 1 < argc) {
            a.level = std::atoi(acp_from_w(argv[++i]).c_str());
        } else if (s == "--max" && i + 1 < argc) {
            a.max_dim = std::atoi(acp_from_w(argv[++i]).c_str());
        } else if (s == "--clock" && i + 1 < argc) {
            a.clock = std::atoi(acp_from_w(argv[++i]).c_str());
        } else if (s == "--no-anim") {
            a.no_anim = true;
        } else if (s == "--no-objects") {
            a.objects = false;
        } else if ((s == "--move" || s == "--army") && i + 1 < argc) {
            const std::string v = acp_from_w(argv[++i]);
            const std::size_t c = v.find(',');
            if (c != std::string::npos) {
                const int px = std::atoi(v.substr(0, c).c_str());
                const int py = std::atoi(v.substr(c + 1).c_str());
                if (s == "--move") {
                    a.move_x = px;
                    a.move_y = py;
                } else {
                    a.army_x = px;
                    a.army_y = py;
                }
            }
        } else if (s == "--help" || s == "-h" || s == "/?") {
            a.help = true;
        } else if (!s.empty() && s[0] != '-') {
            a.map = s;
        }
    }
    LocalFree(argv);
    return a;
}

// Resolves g.root / g.res_path from the command line, the map, or this exe.
void setup_paths(const Args& a) {
    if (!a.res.empty()) {
        g.res_path = a.res;
        g.root = !a.root.empty() ? a.root : find_root_from(a.res);
    } else if (!a.root.empty()) {
        g.root = a.root;
        g.res_path = (fs::path(a.root) / "Release" / "Release.hss").string();
    } else if (!a.map.empty()) {
        g.root = find_root_from(a.map);
    }
    if (g.root.empty() || g.res_path.empty()) {
        const std::string r = find_root_from(exe_dir());
        if (!r.empty()) {
            g.root = r;
            g.res_path = (fs::path(r) / "Release" / "Release.hss").string();
        }
    }
    if (g.res_path.empty() && !g.root.empty()) {
        g.res_path = (fs::path(g.root) / "Release" / "Release.hss").string();
    }
}

// Head-less mode (`--render out.png`): no window, usable from tests/scripts.
int run_headless(const Args& a) {
    attach_parent_console();
    if (!load_resources(false)) {
        std::fprintf(stderr, "aow_view: cannot load resources (%s)\n",
                     g.res_path.empty() ? "Release.hss not found" : g.res_path.c_str());
        return 1;
    }
    aow::Map m;
    std::string err;
    if (!aow::load_map(a.map, m, err)) {
        std::fprintf(stderr, "aow_view: %s\n", err.c_str());
        return 1;
    }
    aow::MapVizOptions opt;
    opt.level = a.level;
    opt.objects = a.objects;
    opt.max_dim = a.max_dim;
    opt.debug_csv = a.debug_csv;
    // The building animations are baked at this clock (a one-shot render has no
    // map-clock timer). `--clock N` renders the N-th map frame's animation state.
    opt.clock = a.clock;
    opt.no_animated_overlays = a.no_anim;
    if (a.move_x < 0 || a.move_y < 0) {
        if (!aow::render_map_png(m, g.res, g.root, opt, a.render, err)) {
            std::fprintf(stderr, "aow_view: render failed: %s\n", err.c_str());
            return 1;
        }
        std::printf("aow_view: wrote %s (level %d of %zu, %zu resources)\n",
                    a.render.c_str(), a.level, m.levels.size(), g.res.size());
        return 0;
    }

    // `--move X,Y`: the movement-trail proof - render the map, then blit the
    // engine's own Cursors.ilb arrows along the path and the X on the target.
    aow::fill_tile_terrain(m, static_cast<std::size_t>(a.level), g.res);
    aow::GameState gs;
    aow::UnitCatalog cat;
    std::string ignored;
    aow::load_game_state(m, gs, ignored);
    aow::load_unit_catalog(g.root, cat, ignored);
    std::uint8_t human = 0xff;
    for (const aow::Player& p : gs.players) {
        if (p.type == aow::PlayerType::Human) human = p.index;
    }
    const std::array<aow::MovePointTable, 256> tables =
        aow::build_move_point_tables();
    const aow::MapLevel& lv = m.levels[static_cast<std::size_t>(a.level)];
    int sx = a.army_x, sy = a.army_y;
    const aow::MapHs* army = nullptr;
    for (std::uint32_t y = 0; y < lv.height && army == nullptr; ++y) {
        for (std::uint32_t x = 0; x < lv.width; ++x) {
            for (const aow::MapHs& hs : lv.tiles[y * lv.width + x].objects) {
                if (hs.class_id != aow::kMapArmyHsClassId || hs.units.empty()) {
                    continue;
                }
                if (a.army_x >= 0) {
                    if (static_cast<int>(x) == a.army_x &&
                        static_cast<int>(y) == a.army_y) {
                        army = &hs;
                        sx = static_cast<int>(x);
                        sy = static_cast<int>(y);
                    }
                } else if (hs.owner == human) {
                    army = &hs;
                    sx = static_cast<int>(x);
                    sy = static_cast<int>(y);
                }
                if (army != nullptr) break;
            }
            if (army != nullptr) break;
        }
    }
    if (army == nullptr) {
        std::fprintf(stderr, "aow_view: no army to move from\n");
        return 1;
    }
    std::vector<std::uint8_t> mts, mps;
    std::vector<std::array<bool, 256>> abs;
    for (const aow::MapUnit& u : army->units) {
        mts.push_back(aow::unit_move_types(u));
        mps.push_back(aow::unit_move_points(cat, u));
        abs.push_back(aow::decode_ability_bitmap(u.ability_mask, u.ability_count));
    }
    const aow::ArmyMove am = aow::army_move(tables, mts, mps, abs);
    const aow::MovePath path = aow::find_move_path(am.table, lv, sx, sy,
                                                   a.move_x, a.move_y, 0xFF);
    std::vector<std::uint8_t> rgba;
    std::uint32_t w = 0, h = 0;
    if (!aow::render_map_rgba(m, g.res, g.root, opt, rgba, w, h, err)) {
        std::fprintf(stderr, "aow_view: render failed: %s\n", err.c_str());
        return 1;
    }
    if (!aow::draw_move_path_overlay(rgba, static_cast<int>(w),
                                     static_cast<int>(h), g.root, path,
                                     am.budget, am.budget, err, false)) {
        std::fprintf(stderr, "aow_view: overlay failed: %s\n", err.c_str());
        return 1;
    }
    if (!aow::write_png_rgba(a.render, w, h, rgba, err)) {
        std::fprintf(stderr, "aow_view: write failed: %s\n", err.c_str());
        return 1;
    }
    std::printf("aow_view: wrote %s - army (%d,%d) -> (%d,%d), %zu hexes, "
                "cost %d, budget %d, move types %s\n",
                a.render.c_str(), sx, sy, a.move_x, a.move_y, path.steps.size(),
                path.total_cost, static_cast<int>(am.budget),
                aow::move_types_string(am.move_types).c_str());
    return 0;
}

void show_usage() {
    MessageBoxW(
        nullptr,
        L"aow_view - Age of Wonders 1 map viewer\n\n"
        L"aow_view.exe [map] [--root <game folder>] [--res <Release.hss>]\n"
        L"             [--level N] [--no-objects] [--render <out.png>]\n"
        L"             [--max N] [--clock N] [--debug-csv <out.csv>]\n"
        L"             [--move X,Y] [--army X,Y]\n\n"
        L"--clock bakes the building animations at map clock N (AoWHSMap+0x40;\n"
        L"default 0).\n"
        L"--move draws a movement path (the game's Cursors.ilb arrows + X) into\n"
        L"the rendered PNG, from --army (default: the human player's first army)\n"
        L"to X,Y.\n\n"
        L"Run it with no arguments to pick a map with the Open dialog.",
        L"aow_view", MB_ICONINFORMATION);
}

}  // namespace

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    const Args a = parse_args();
    if (a.help) {
        show_usage();
        return 0;
    }
    setup_paths(a);
    if (!a.render.empty()) return run_headless(a);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"AoWMapViewer";
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = wc.hIcon;
    if (RegisterClassExW(&wc) == 0) return 1;

    g_hwnd = CreateWindowExW(WS_EX_ACCEPTFILES, wc.lpszClassName, kAppTitle,
                             WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
                             CW_USEDEFAULT, CW_USEDEFAULT, 1120, 780, nullptr,
                             nullptr, hInst, nullptr);
    if (g_hwnd == nullptr) return 1;

    ShowWindow(g_hwnd, (nShow != 0) ? nShow : SW_SHOWNORMAL);
    UpdateWindow(g_hwnd);

    // Resolve Release.hss up-front: if it is missing, offer to locate the game
    // folder before the user picks a map.
    if (!load_resources(false)) {
        const int r = MessageBoxW(
            g_hwnd,
            L"Release\\Release.hss was not found next to this program.\n\n"
            L"Locate the Age of Wonders 1 folder now?",
            L"aow_view", MB_ICONQUESTION | MB_YESNO);
        if (r == IDYES) {
            const std::string root = browse_for_root();
            if (!root.empty()) {
                g.root = root;
                g.res_path = (fs::path(root) / "Release" / "Release.hss").string();
                load_resources(true);  // failure is non-fatal: flat fallbacks
            }
        }
    }

    HACCEL acc = build_accels();
    g.objects = a.objects;

    if (!a.map.empty()) {
        open_map(a.map);
        if (a.level > 0) set_level(a.level);
    } else {
        do_open();
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (acc != nullptr && TranslateAcceleratorW(g_hwnd, acc, &msg) != 0) {
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (acc != nullptr) DestroyAcceleratorTable(acc);
    return static_cast<int>(msg.wParam);
}






