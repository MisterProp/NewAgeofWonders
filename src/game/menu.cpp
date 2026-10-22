// menu.cpp - `AoW.exe`: a 1:1 replica of the Age of Wonders 1 title screen
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
//            Int\Fonts\Age15WhGr.ILB  (FontModule.AoW15WhiteGrey, one glyph
//                                       per character code - see below)
//            Songs\TITLE.IT          (the title music, started by AoW.exe;
//                                     played here through libopenmpt)
//
//   code     AoW.exe  0x0041a030 WindowDrawSurfaceEx -> 0x00419c84 (the draw)
//                     0x0041ae08 frame tick (counters +0x64/+0x68, credits)
//                     0x00419488 the five-orb hit test
//                     0x0041a150 TitleSceneShow: 640x480 + Songs\TITLE.IT
//
// The scene is 480 px tall and centred vertically on the display, and as wide
// as the display; the five orbs sit on a 120 px pitch centred on that width.
//
//   AoW.exe [--root <dir>] [--width N] [--height N]
//                [--render <out.png> --frame N] [--frames N] [--no-music]
//                [--captions a,b,c,d,e] [--overlay] [--fps N]
//
//   F1 in the game window toggles a frames-per-second read-out in the top-left
//   corner (FpsMeter / draw_fps_overlay); --fps N bakes it into a head-less
//   render.
//
// Windows-only: Win32 + GDI + WinMM (via aow::AudioOut) for output, with the
// title music decoded by libopenmpt (aow::openmpt::Player). The asset/engine
// code it calls stays stdlib-only, as required by the port's policy.
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
#include <shellapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "aow/audio.hpp"
#include "aow/game_state.hpp"
#include "aow/ilb.hpp"
#include "aow/map_resources.hpp"
#include "aow/map_types.hpp"
#include "aow/mapviz.hpp"
#include "aow/movement.hpp"
#include "aow/openmpt.hpp"
#include "aow/pfs_types.hpp"
#include "aow/playlist.hpp"
#include "aow/png.hpp"
#include "aow/unit_catalog.hpp"

namespace fs = std::filesystem;

namespace {

const wchar_t kAppTitle[] = L"Age of Wonders";

// Resource id of the game's own icon (Age of Wonders.ico), embedded by the
// build as RT_GROUP_ICON id 1 (see build.ps1 / src/resources/aow.rc.in).  The
// window class loads it so the title bar / taskbar show the game's icon rather
// than the default application icon.
constexpr int kIconResId = 1;

// A monotonic millisecond clock (QueryPerformanceCounter): the head-less
// `--game-bench` timings and the interactive tuning need sub-GetTickCount
// resolution.
double bench_now_ms() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f{};
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER c{};
    QueryPerformanceCounter(&c);
    return static_cast<double>(c.QuadPart) * 1000.0 /
           static_cast<double>(freq.QuadPart);
}

// ------------------------------------------------------------ the frame clock
// The form's FrameRate is 30 (DFM), i.e. one frame every 33.3 ms. The replica
// drives that clock with a high-resolution waitable timer instead of SetTimer():
// WM_TIMER is not a high-resolution timer - it is serviced off the system clock
// tick (15.6 ms here), so SetTimer(33) really fires every 3 ticks = 46.9 ms
// (~21 fps). A waitable timer keeps the 33 ms period (measured 33.07 ms =
// 30.2 fps; see docs/09_GAME_SCREEN.md). SetTimer stays as the fallback for
// systems where no waitable timer can be created.
constexpr UINT kFrameMs = 33;
constexpr UINT_PTR kFrameTimerId = 1;  // the WM_TIMER fallback's id

// CREATE_WAITABLE_TIMER_HIGH_RESOLUTION (Windows 10 1803+); older SDK headers
// do not define it.
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

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

// Asset indices inside Int\Scenes\title.ilb. These are the entry *ids* in the
// ILB directory, not positions in the entry vector: the library reuses ids
// (id 256/14830) for the company logos, so position 9 is id 10 etc.
constexpr std::uint32_t kImgBackground = 0;
constexpr std::uint32_t kImgLogo = 3;        // solid LogoTitleFinal (0x11)
constexpr std::uint32_t kImgOrbBase = 10;    // first OrbFrame id
constexpr std::uint32_t kImgOrbOverlay = 25; // TSprite16 orb overlay
constexpr std::uint32_t kImgOrbLast =
    kImgOrbBase + kButtonCount * kOrbStride; // exclusive

// Bitmap fonts (TImageLibraryFont). The title menu's hover captions use
// FontModule.AoW15WhiteGrey -> Int\Fonts\Age15WhGr.ILB (its DFM record gives
// CharacterDistance = 0). That library stores every glyph TWICE, as the same
// letterform in two storage layers:
//   "font2.BMP": black ink on a white background
//   "font.bmp" : white ink on a black background
// The engine draws ONE of them (the ink is what SetColor recolours), so the
// replica draws a single layer - drawing both doubles every stroke. Each glyph
// is wrapped in a TImageNode whose index is its character code (aow::IlbFile
// reports that as IlbEntry::index); the two layers are not always in the same
// order in the file, so the layer is selected by name, and the glyph is filed
// under the code, exactly as TImageLibraryFont does (SetTextEx -> Get(c)).
constexpr const char* kFontIlb = "Int\\Fonts\\Age15WhGr.ILB";
constexpr int kFontTracking = 0;
constexpr int kFontLayers = 2;      // storage layers per glyph
constexpr int kFontVariant = 1;     // 1 = "font.bmp" (white ink on the UI)
constexpr const char* kFontLayerNames[kFontLayers] = {"font2.BMP", "font.bmp"};
// AoW15WhiteGrey's glyphs are tightly cropped sprites and the engine positions
// each by its TSprite16 class shift (+0x60, honoured in decode_glyph). The tall
// glyphs (uppercase, ascenders) carry shift 1, the x-height glyphs 4 and the
// descenders 3-4, so the face's *cap line* sits this many pixels below the glyph
// cell's top edge. draw_text's callers anchor to that cap line, so the pen is
// lifted by it - which places the "Single Computer" caption on TextRef.png and
// "Scenario:" on SelectionMenuRef.png pixel-for-pixel.
constexpr int kWhgCapShift = 1;

// The game's pointer sprites (TIntGfxMod.Cursors -> Images\Cursors.ilb). The
// engine hands Windows the system arrow for `crDefault`, so the replica draws
// the matching AoW pointer sprite on top of the frame instead - that way the
// cursor also shows up in head-less renders.
constexpr const char* kCursorsIlb = "Images\\Cursors.ilb";
constexpr std::uint32_t kDefaultCursorId = 0;

// The world-map pointer (`HSEngine.THSMapMouse` + `AoWE.TAoWHSMap.UpdateCursor`).
// `THSMapMouse+0x30` holds the Cursors.ilb image *id* the window paints
// (`GetCursor`/`SetCursor`, HSEPack.dpl 0x5560b630 / 0x5560b634) and `+0x34` the
// **state request** the map asks for:
//
//   0      the plain arrow (id 0)
//   0x101  the hex under the pointer holds something *selectable* -> the four
//          49x49 sparkle frames at ids 0x34..0x37, advanced one frame per four
//          ticks of the map clock (`0x34 + ((clock >> 2) & 3)`)
//   0x100  an army of ours is selected and the pointer is choosing where to move
//          it -> the **move** icon: the walk / sail / fly art at ids 0x40 / 0x42
//          / 0x44, or its "blocked" twin at 0x41 / 0x43 / 0x45
//          (`AoWE.TSelectedArmy.UpdateMoveCursor@0x55793440` picks the art from
//          the army's move types and ORs in bit 0 when the target hex cannot be
//          entered, exactly like the engine's `|1`)
//   0x102  the move target holds an army that is *not* ours -> the crossed-swords
//          icon at ids 0x31/0x32, two frames
//   0x103  ... an independent army whose units would *join* -> the twin-banner
//          icon at ids 0x3a..0x3c, three frames
//
// `AoWE.TAoWHSMap.UpdateCursor@0x55777b20` re-resolves 0x101/0x102/0x103 into the
// animated ids every frame (the `0x34 + ((clock >> 2) & 3)` / `0x31 + ((clock >>
// 2) & 1)` / `0x3a + ((clock >> 2) % 3)` forms); the 0x100 icons are painted by
// `TSelectedArmy.UpdateMoveCursor` itself, which is why that state is symbolic.
//
// Evidence: `AoWE.TAoWHSMap.MouseMoved@0x55777a58` (sets 0x101),
// `AoWE.TAoWHSMap.UpdateCursor@0x55777b20` (resolves every frame),
// `AoWE.TSelectedArmy.UpdateMoveCursor@0x55793440` + `TMoveSelector.Activate@
// 0x55763e74` (0x100 and the icons), `HSEngine.THSMap.NewFrame@0x5560d758` (the
// clock). See docs/09_GAME_SCREEN.md 6.8 and 6.9.
constexpr int kCursorStateDefault = 0;
constexpr int kCursorStateMove = 0x100;
constexpr int kCursorStateHover = 0x101;
constexpr int kCursorStateAttack = 0x102;
constexpr int kCursorStateJoin = 0x103;
constexpr std::uint32_t kHoverCursorBaseId = 0x34;   // Cursors.ilb slot 0x34
constexpr int kHoverCursorFrames = 4;                // 0x34..0x37
constexpr std::uint32_t kAttackCursorBaseId = 0x31;  // crossed swords, 2 frames
constexpr int kAttackCursorFrames = 2;
constexpr std::uint32_t kJoinCursorBaseId = 0x3a;    // twin banners, 3 frames
constexpr int kJoinCursorFrames = 3;
constexpr std::uint32_t kMoveCursorWalkId = 0x40;    // walk  / blocked = 0x41
constexpr std::uint32_t kMoveCursorSailId = 0x42;    // sail  / blocked = 0x43
constexpr std::uint32_t kMoveCursorFlyId = 0x44;     // fly   / blocked = 0x45
constexpr int kHoverCursorClockShift = 2;            // `clock >> 2`
constexpr int kHoverCursorClockMask = kHoverCursorFrames - 1;
// `AoWHSMap+0x40`, the map animation clock: `HSEngine.THSMap.NewFrame` adds one
// per map frame and wraps it at 0x100000. The units' image sequences and the
// hover pointer both read it.
constexpr int kMapClockWrap = 0x100000;
// The map's "exploration enabled" flag (`TAoWHSMap+0xc9`, written by
// `TAoWHSMap.SetExplorationEnabled@0x557723d4`). A normal game has it on: the
// scenario's unexplored hexes are fogged and cannot be walked into, which is what
// `TSelectedArmy.UpdateMoveCursor`'s `|1` (the blocked twin of the move icon)
// signals. The replica renders every hex, so the flag only drives the icon - see
// docs/09_GAME_SCREEN.md 6.9.
constexpr bool kExplorationEnabled = true;
// `TAbstractUnit.VisibilityRange@0x55780ef8`: 4 hexes, or `2 * the unit's vision
// ability (0x40) + 4`, and a player trait halves it towards a neighbour
// (`TPlayer+0x54` -> byte +0x45 & 8). The replica uses the plain 4 - the
// scenario-start exploration radius (mv_explore_for_human).
constexpr int kUnitVisionRange = 4;

// The hexagon selector: the blue 3D hex the game paints over the hex a move
// selection is pointing at (`AoWE.TAbstractAoWHSMap.ShowFrontHexagonSelector@
// 0x5577338c` / `ShowBackHexagonSelector@0x557732f4`, shown while `TMoveSelector`
// is active). The art is `Images\General.ILB` (`AoWWHSSet+0x48` =
// LinkToIL("GENERAL.ILB")) ids 6 and 7, both named `3dHex.BMP`: 6 is the lower
// perimeter - drawn *in front of* the units, the engine gives its sprite the
// scene key "just past the hex's topmost sprite" - and 7 the upper edges, which
// stay behind them.
constexpr const char* kGeneralIlb = "Images\\General.ILB";
constexpr std::uint32_t kHexSelectorFrontId = 6;
constexpr std::uint32_t kHexSelectorBackId = 7;
// `HXtoHP(hex) + (-8, -0x1e)` - the anchor both halves are drawn at.
constexpr int kHexSelectorAnchorX = -8;
constexpr int kHexSelectorAnchorY = -0x1e;
// Both halves are drawn **brightened**, not alpha-blended: the art is a dark-blue
// 3D hexagon whose gradient fill must not darken the tile under it. The engine
// paints scene sprites through the `GFXE.GenerateBrightenBlendTable` path
// (`GFXE.BlendColorKey565@0x5500b3f0` indexes that per-channel table), i.e. the
// result is `max(dst, src)` per channel - so the marker only *lifts* the tile
// where its own pixel is brighter (the blue channel of the gradient and the
// bright rim) and never covers the terrain with its near-black fill. That is what
// makes it the translucent "glass" hexagon of the reference (`BMRef.png`): the
// hovered hex's terrain stays fully visible, with a blue haze inside and glowing
// edges. See docs/09_GAME_SCREEN.md 6.9.

// ------------------------------------------------- Game Type (TMultiWindow)
// Orb 0 ("Scenario") of the title screen opens the game-type menu. Everything
// below was recovered from AoW.exe and the shipped assets:
//
//   form      TMultiWindow - published field table 0x0042a6fe, published
//             method table 0x0042a985, class record 0x0042ab92; the Delphi form
//             resource is assets/dfm/AoW/TMULTIWINDOW.dfm.
//   show      FUN_0042abf0 (the title screen's orb-0 action at 0x0041ae94):
//             plays SONGS\TITLE.IT, shows MultiPnl + GamePanel, and hides the
//             TCP/IPX panels. It hides TCPBtn and IPXBtn and re-shows TCPBtn
//             only when a TCP (type 2) connection exists - so on a PC with a
//             TCP stack but no IPX, the visible set is Single / TCP / Email
//             (exactly the reviewed screen).
//   anim      MultiWinUpdate (0x0042ae54, MultiPnl.OnUpdate): a +0xF0 counter
//             advances by 10 per frame until GamePanel's height (286); GamePanel
//             is drawn at (+0xF0 - its height) and BottomPnl at
//             (480 - +0xF0 + GamePanel.height - BottomPnl.height) - i.e. the top
//             panel drops in from above and the bottom panel rises from below.
//   assets    Int\Scenes\TitleM.ILB (the DFM's IntGfxMod.TitleScreen):
//             [1] SetupBg03.BMP 640x480, [2] GameMenuTop.BMP 158x286,
//             [3] GameMenuBottom.BMP 158x66, [20/21/22] TitleIcon.BMP 158x24
//             (ILIndexUp / ILIndexDown / ILIndexLit). Text uses the same
//             AoW15WhiteGrey font as the title captions.
constexpr const char* kMenuIlb = "Int\\Scenes\\TitleM.ILB";
constexpr const char* kGtFontIlb = "Int\\Fonts\\Age8s.ILB";  // GameLbl face
constexpr std::uint32_t kGtBgId = 1;        // SetupBg03.BMP     640x480
constexpr std::uint32_t kGtTopId = 2;       // GameMenuTop.BMP   158x286
constexpr std::uint32_t kGtBottomId = 3;    // GameMenuBottom.BMP 158x66
constexpr std::uint32_t kGtBtnUpId = 20;    // TitleIcon.BMP     158x24
constexpr std::uint32_t kGtBtnDownId = 21;  // ILIndexDown
constexpr std::uint32_t kGtBtnLitId = 22;   // ILIndexLit

// Geometry: the panels' WinLeft/WinTop inside the 640x480 scene (the form is a
// child of the title window, so its own Left/Top are not used).
constexpr int kGtPanelX = 241;    // GamePanel / BottomPnl WinLeft
constexpr int kGtPanelW = 158;    // WinWidth
constexpr int kGtPanelH = 286;    // GamePanel WinHeight (GameMenuTop)
constexpr int kGtBottomH = 66;    // BottomPnl WinHeight (GameMenuBottom)
constexpr int kGtBottomRest = 414;  // 480 - kGtBottomH (MultiWinUpdate's rest)
constexpr int kGtLabelY = 20;     // GameLbl WinTop ("Game Type")
// GameLbl.TextColor (TMULTIWINDOW.dfm) = 8472387 = $00814743 - a Delphi TColor
// stored as $00BBGGRR, i.e. RGB(67,71,129). TAOWLabel.Draw hands it to
// TImageLibraryFont, which recolours the Age8s ink with it. The game's UI is
// drawn through its 8-bit DirectDraw palette, so the on-screen ink is the
// palettised RGB(66,66,132) - the value measured in bin/GameTypeRef.png (the
// capture of the shipped screen), which is what the replica must hit.
constexpr std::uint8_t kGtLabelR = 0x42;
constexpr std::uint8_t kGtLabelG = 0x42;
constexpr std::uint8_t kGtLabelB = 0x84;
constexpr int kGtBackY = 22;      // BackButn WinTop inside BottomPnl
constexpr int kGtBtnW = 158;      // button WinWidth (TitleIcon width)
constexpr int kGtBtnH = 24;       // button height = TitleIcon height
constexpr int kGtAnimStep = 20;   // MultiWinUpdate's +10, sped up for the port
// The GamePanel/BottomPnl textures are composited over the backdrop (and over
// their own buttons) at ~68% - measured from the original screenshot: the panel
// art (16,20,32) over the backdrop (106,85,90) yields the game's (45,39,50),
// and the TitleIcon fill (65,85,180) under the panel yields the game's (24,33,65).
constexpr int kGtPanelAlpha = 68;
constexpr int kGtBtnCount = 3;    // visible game-type buttons (IPX is hidden)
constexpr int kGtBtnY[kGtBtnCount] = {46, 86, 166};  // Single / TCP / Email
const char* const kGtBtnText[kGtBtnCount] = {
    "Single Computer", "TCP Internet", "Play by Email"};

// ---------------------------------------------------- Scenario (TScenSetup)
// The Game Type screen's "Single Computer" button opens this screen. Recovered
// from AoW.exe and the shipped assets (docs/08_SCENARIO_SCREEN.md):
//
//   form      TScenSetup - RTTI 0x00410891..0x00410bb0 (method table decoded
//             below), form resource assets/dfm/AoW/TSCENSETUP.dfm.
//   show      FUN_00442b588 -> FUN_00410bd8(Self=scensetup, TSetupControl)
//   refresh   FUN_00411460, per-race FUN_00410f2c
//   pick map  PickScenarioBtnClick 0x00412460 -> FUN_0041243c (TFileDlgWindow)
//   assets    Int\Scenes\TitleM.ILB id 4 Backg03 (backdrop), SceneBtn triples
//             55/58/61, ScenChk 41/42; Int\GenericF.ILB 72..79 (9-slice panel
//             frame); Int\GenericT.ilb 1 ShadowOv (panel tint);
//             Int\Scenes\PlyrIcns.ILB (race/player icons); FontModule.Age10s.
constexpr const char* kPlyrIcnsIlb = "Int\\Scenes\\PlyrIcns.ILB";
constexpr const char* kGenericFIlb = "Int\\GenericF.ILB";
constexpr const char* kGenericTIlb = "Int\\GenericT.ilb";
constexpr const char* kGenericBIlb = "Int\\GenericB.ILB";
constexpr const char* kWinTIconIlb = "Int\\WinTIcon.ilb";
constexpr const char* kUnitIconsIlb = "Int\\Icons.ILB";
constexpr const char* kShieldsIlb = "Int\\ShieldsM.ILB";  // race crest shields
constexpr const char* kCeltDecoIlb = "Int\\CeltDeco.ILB"; // celtic knot bands
constexpr const char* kFont10Ilb = "Int\\Fonts\\Age10s.ILB";
constexpr std::uint32_t kScenBgId = 4;          // TitleM Backg03.BMP 640x480
constexpr std::uint32_t kScenFrameBase = 72;    // GenericF 9-slice TL..BR
constexpr std::uint32_t kScenShadowId = 1;      // GenericT ShadowOv 256x256
constexpr std::uint32_t kScenBtnIds[3] = {55, 58, 61};  // SceneBtn triples
constexpr std::uint32_t kScenChkIds[2] = {41, 42};      // ScenChk 12x12
// The ornate dialog frame is GenericF's DialogFr.BMP (ids 32..39); the inner
// list/group frames are FrGoldD.BMP (ids 113..120); the celtic buttons are
// GenericB's ButRCelt2.BMP (ids 19..27 = 3 states x 3 pieces).
constexpr std::uint32_t kDlgFrameBase = 32;     // DialogFr 22px 9-slice
constexpr std::uint32_t kGoldFrameBase = 113;   // FrGoldD 3px 9-slice
constexpr std::uint32_t kThinFrameBase = 105;   // FrThinD 2px 9-slice
constexpr std::uint32_t kCeltBtnBase = 19;      // ButRCelt2 (state*3 + piece)
// The dialog's close box is TAOWButton Close: ILIcon = IntGfxMod.GenericI
// (Int\GenericI.ILB) at ILIndexUp/Down/Lit 0/1/2 (15x16 MainHeaderBut.BMP).
constexpr std::uint32_t kScenCloseArt[3] = {0, 1, 2};
constexpr int kScenGridX = 10, kScenGridY = 39;  // SidesPnl WinLeft/WinTop
constexpr int kScenCellW = 156, kScenCellH = 66;
constexpr int kScenColDx = 164, kScenRowDy = 70;
constexpr int kScenSetX = 500, kScenSetY = 40, kScenSetW = 134, kScenSetH = 430;
constexpr int kScenBtnX = 510, kScenBtnY0 = 385;   // PickScenario/Exit/Start
constexpr int kScenBtnW = 114, kScenBtnH = 24, kScenBtnPitch = 27;
// The map picker's FileLB list: lh / ItemHeight (141 / 17) rows are visible.
constexpr int kScenListRows = 8;
constexpr int kScenGlowFrames = 90;            // fog scale: 0 = fully fogged, 90 = fully lit
constexpr int kScenRaceCount = 12;
// Grid fill order (row-major) -> race index. R1..R12 read
// Humans/Azracs/Lizard Men/Frostlings/Elves/Halflings/Dwarves/High Men/
// Dark Elves/Orcs/Goblins/The Undead.
const int kScenGridRace[kScenRaceCount] =
    {0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7, 11};
const char* const kScenRaceName[kScenRaceCount] = {
    "Humans", "Azracs", "Lizard Men", "Frostlings", "Elves", "Halflings",
    "Dwarves", "High Men", "Dark Elves", "Orcs", "Goblins", "The Undead"};
// Each R*Lbl's WinLeft inside R*CenterPnl (which itself sits at WinLeft 48), in
// the DFM's own order - the form tuned every name to sit centred in its cell.
const int kScenRaceLblX[kScenRaceCount] = {
    28, 31, 19, 23, 34, 26, 26, 24, 19, 36, 30, 17};
// Each R*Lbl's TAOWLabel.TextColor, in the DFM's own order (Humans..The
// Undead). TImageLibraryFont.SetColor recolours the glyph coverage with it, so
// the race names are per-race tinted exactly as the form declares.
const std::uint8_t kScenRaceColor[kScenRaceCount][3] = {
    {112, 115, 218},  // Humans      14316400
    {232, 181, 119},  // Azracs       7845352
    {77, 162, 142},   // Lizard Men   9347661
    {152, 193, 231},  // Frostlings  15188376
    {106, 182, 73},   // Elves        4830826
    {238, 228, 113},  // Halflings    7464174
    {208, 175, 142},  // Dwarves      9351120
    {237, 233, 218},  // High Men    14346733
    {182, 99, 186},   // Dark Elves  12215222
    {241, 67, 67},    // Orcs         4408305
    {187, 115, 87},   // Goblins      5731259
    {146, 125, 118},  // The Undead   7765394
};
const char* const kScenBtnText[3] = {"Scenario", "Exit", "Start!"};

// --- the race PlayerCB drop-down (TScenSetup R*PlayerCB) --------------------
// Each race cell's first combo is a `TAOWComboBox` with `OnChange =
// RacePlayerCBChange` (0x00412a24): 98x18, AOWFont Age8s, ItemHeight 15,
// ILTextureNormal = GenericT 4 (blank1) and ILTextureSelected = GenericT 16
// (TextLBSel), NumberVisible 7 (TSCENSETUP.dfm).
//
// Its items are filled by `TSetupControl.ListAvailablePlayers` (AoWEPACK
// 0x557e1660). In a host game it appends the seven `AoWE.TPlayerTypeRStr`
// entries in the order 0,1,2,3,6,7,4 - i.e. the drop-down reads
//   Human, CPU Squire, CPU Knight, CPU Lord, CPU King, CPU Emperor, Independent
// (the exact list in the reviewed screen). `PlayerNameToPlayerID` (0x557e19b0)
// maps the chosen caption back to its type index and `RacePlayerCBChange` then
// calls `TSetupControl.hSetPlayerID` (0x557e1eb4), which stores that type into
// the race's `TSetupSettings` record.
//
// The type value itself lives on the map: a TPlayer's rw tag 0x3 (+0xa7) is the
// player type - the independents carry 4, the local human 0 and the AIs 1/2/3/
// 6/7 (verified against the nine campaign maps). So a picked map decides each
// race's Human/CPU entry before the player touches anything.
constexpr int kScenPTypeCount = 7;
constexpr int kScenComboH = 18;     // ScenRBut combos are 18 high (DFM WinHeight)
constexpr int kScenItemH = 15;      // R*PlayerCB.ItemHeight
constexpr int kScenComboW = 98;     // R*PlayerCB.WinWidth
// Drop-down position (combo pos 0..6) -> player-type value.
const int kScenPTypeItem[kScenPTypeCount] = {0, 1, 2, 3, 6, 7, 4};
// Player-type value -> caption (index 5 is the unassigned "None" slot).
const char* const kScenPTypeName[8] = {
    "Human", "CPU Squire", "CPU Knight", "CPU Lord", "Independent", "None",
    "CPU King", "CPU Emperor"};


// --- exact art recovered from the DFMs (assets/dfm/AoW/TSCENSETUP.dfm and
// TFILEDLGWINDOW.dfm) and the IntGfxMod library map (TINTGFXMOD.dfm). These are
// the very entries AoW.exe's TImageLibrary.Get() indexes, so nothing here is a
// placeholder: every fill() the first pass used is replaced by the shipped art.
//
// The three bottom buttons are TAOWButton with ILButton = IntGfxMod.TitleScreen
// (= Int\Scenes\TitleM.ILB), ILIndexUp = 32, ILIndexDown = 35, ILIndexLit = 38.
// Each index is a 3-piece button (ScenRBut.BMP: left cap / middle / right cap).
constexpr std::uint32_t kMainBtnArt[3][3] = {{32, 33, 34},   // up
                                             {35, 36, 37},   // down
                                             {38, 39, 40}};  // lit
constexpr int kMainBtnH = 18;      // ScenRBut.BMP height
constexpr int kMainBtnCapL = 12;   // left cap width  (id 32)
constexpr int kMainBtnCapR = 10;   // right cap width (id 34)
// TAOWLabel.TextColor 15134446 (TColor 0x00E6EEEE -> RGB 238,238,230).
constexpr int kMainBtnTextR = 238, kMainBtnTextG = 238, kMainBtnTextB = 230;

// Every TAOWPanel on this form draws ILTexture = IntGfxMod.GenericT with
// ILIndexTexture = 1 (ShadowOv.BMP) behind ILFrame = IntGfxMod.GenericF with
// ILIndexFrame = 72 (STGrFr.BMP). RacePnlDrawBack (0x00412c3c) is exactly that
// texture blit at (WinLeft+5, WinTop+5) inside the panel's own frame.
constexpr std::uint32_t kPanelFrameBase = 72;   // GenericF STGrFr 9-slice
constexpr std::uint32_t kPanelTexId = 1;        // GenericT ShadowOv.BMP
constexpr std::uint32_t kPanelTexInset = 5;     // RacePnlDrawBack's +5
// A masked panel's body is a black veil over the backdrop (the shipped capture
// keeps 16 % of Backg03) with the ShadowOv tile on top at ~9 %.
constexpr int kPanelBodyVeil = 215;
constexpr int kPanelBodyAlpha = 9;
// The Duration / Miscellaneous group frames (TAoWFrame DurFr / LFr) use
// ILFrame = GenericF with ILIndexFrame = 64 (ScenFr.BMP).
constexpr std::uint32_t kScenFrBase = 64;       // GenericF ScenFr 9-slice
// FileLB (TAOWListBox) names ILTextureNormal GenericT 21 (Text2Lb17.BMP - the
// dark horizontally striped list body) and ILTextureSelected IntGfxMod.Menus 8
// (PopUpSet.BMP); ILFrame GenericF 113 (FrGoldD).
constexpr std::uint32_t kListTexId = 21;
constexpr std::uint32_t kListSelTexId = 8;
// The TFileDlgWindow info strip (NewInfo1..3) is GenericF 113 (FrGoldD) over
// GenericT 17 (TextOld1.BMP); the outer NewInfo / LSavedInfo panels are
// GenericF 105 (FrThinD) over GenericT 18 (TextOld2.BMP).
constexpr std::uint32_t kInfoTexId = 17;
constexpr std::uint32_t kInfo2TexId = 18;
// TAOWVScrollBar ILIndex* (R1PlayerSB / TurnVSB): GenericS ScenVSB.BMP at
// 56 (dec up) / 57 (dec down) / 58 (inc up) / 59 (inc down) / 63 (track).
constexpr std::uint32_t kVsbDecUp = 56, kVsbDecDown = 57;
constexpr std::uint32_t kVsbIncUp = 58, kVsbIncDown = 59, kVsbTrack = 63;
// TAOWHScrollBar ILIndexBack = GenericS 49 (ScenSlid.BMP track 3-piece),
// ILIndexTab = GenericS 52 (the thumb 3-piece).
constexpr std::uint32_t kSliderBack = 49, kSliderTab = 52;
// TAOWCheckBox ILIndexUp/Down/Lit = TitleScreen 47/48/49 ... but the shipped
// 12x12 Scenario checkbox is ScenChk.BMP at TitleScreen 41 (off) / 42 (on).
// NB: ScenChk.BMP decodes to a *round* 12x12 ring; the square, gold-bevelled
// box the Scenario screen actually shows is IntGfxMod.GenericI's NGCBox.BMP -
// id 47 (empty) and id 48 (checked, the blue glow). Verified against
// bin/RaceSelectorRef2.png (template match mean 0.95 / 1.18).
constexpr std::uint32_t kChkOffId = 41, kChkOnId = 42;
constexpr std::uint32_t kNgcBoxOff = 47, kNgcBoxOn = 48;  // GenericI NGCBox
// The player-list column headers (PlayerListSort "Players" / AOWButton1 "Ping"
// / PreferredRaceSort "Pref. Race") are TAOWButtons whose
// ILIndexUp = ILIndexDown = 52 and ILIndexLit = 49, i.e. TitleM's ScenCol.BMP
// 3-piece (2 / 52 / 2 x 15). See TSCENSETUP.dfm.
constexpr std::uint32_t kScenColArt[3] = {52, 53, 54};
// ChatEdit names ILEdit = IntGfxMod.TitleScreen with ILIndex = 23: ScenEd.BMP
// (2 / 48 / 2 x 21), the chat panel's input line.
constexpr std::uint32_t kScenEdArt[3] = {23, 24, 25};
// ChatPnl - SidesPnl's bottom strip (docs/08 sec. 10): a TAOWPanel at
// (0,286) 484x145 over the STGrFr frame + ShadowOv body, holding the (single
// player: empty) chat memo / edit and the player list. Its children are all
// relative to ChatPnl, whose absolute origin is SidesPnl + (0,286) = (10,325).
constexpr int kChatX = 10, kChatY = 325, kChatW = 484, kChatH = 145;
constexpr int kChatMemoX = 10, kChatMemoY = 12, kChatMemoW = 299,
              kChatMemoH = 93;
constexpr int kChatEditX = 10, kChatEditY = 114, kChatEditW = 299,
              kChatEditH = 21;
// ConnectionPnl (318,12) 166x133 has no frame/texture of its own; its three
// list widgets are 15 px headers over 78 px ScenFr-framed lists.
constexpr int kConnX = 318, kConnY = 12, kConnHdrH = 15, kConnListY = 15;
constexpr int kConnListH = 78;
// ChatFogPnl (GenericT 1) veils the chat strip with black at this alpha
// (measured on bin/RaceSelectorRef2.png: every child reads ~24 % of its own
// colour).
constexpr int kChatFogAlpha = 194;
constexpr int kConnColX[3] = {0, 70, 95};   // Players / Ping / Pref. Race
constexpr int kConnColW[3] = {71, 26, 61};
const char* const kConnColText[3] = {"Players", "Ping", "Pref. Race"};
// Players / Ping / Pref. Race list boxes: ILFrame GenericF 64 (ScenFr),
// ILTextureNormal GenericT 4 (blank1.bmp - the 50x50 black body).
constexpr std::uint32_t kBlankTexId = 4;

// The dialog edits / combos use ILEdit = IntGfxMod.GenericE (= Int\GenericE.ILB).
constexpr const char* kGenericEIlb = "Int\\GenericE.ILB";
// IntGfxMod.GenericS (= Int\GenericS.ilb) holds every scrollbar / slider skin.
constexpr const char* kGenericSIlb = "Int\\GenericS.ilb";
// FontModule.Age10 (Int\Fonts\Age10.ILB) is the dialog's face; Age10s is the
// race-name face; Age8s is the Settings panel's face (all CharacterDistance=1
// except Age8s=0 - TFontModule at 0x0040383c).
constexpr const char* kFontAge10Ilb = "Int\\Fonts\\Age10.ILB";

// ---------------------------------------------- the in-game screen (TWorldMap)
// The screen the game shows once a scenario map is chosen: the rendered map
// with three UI pieces drawn over it.  Every number below is recovered from the
// form DFMs (assets/dfm/AoW/TTBWINDOW.dfm, TSWINDOW.dfm, TPINFOW.dfm) plus the
// runtime layout code `AoWComp.TAoWComponent.ReAlignComp` (AoWInterface.dpl
// 0x00401f2c), which turns the DFM's per-component `Alignment.*` fields into
// positions:
//
//   awLeft(1):   x = parent_w * LeftOffPercent / 100 + LeftOffset
//                w = WidthPercent ? WidthPercent * parent_w / 100 : w
//   awRight(2):  x = parent_w - RightOffset - w - parent_w * RightOffPercent/100
//   awBoth(3):   x = parent_w * LeftOffPercent / 100 + LeftOffset
//                w = parent_w - x - RightOffset - parent_w * RightOffPercent/100
//   awCenter(4): x = parent_w/2 - w/2
//   (same for the vertical pair with Top/Bottom/Height)
//
// The design size the game sets (TMainForm.Display = 640x480) and the values
// the DFMs carry at that size. The *runtime* layout does NOT scale these: every
// box is re-resolved each frame from the `Alignment.*` fields by
// `Scene::game_layout` (a port of AoWComp.TAoWComponent.ReAlignComp), so the
// interface reflows when the window grows - see the AowAlign block below. The
// constants here are the recovered spec: the DFM's design-time sizes/offsets and
// the ILB entry ids, which the layout feeds into aow_realign().
constexpr int kGameW = 640;
constexpr int kGameH = 480;

// --- the bottom bar (TTBWindow.TaskWindow: 640x48 at y=432) ----------------
constexpr int kTbBarH = 48;
// The bar body is Int\TBWin.ILB's BarSet 3-piece (49 / 436 / 155 x 48 = 640).
constexpr std::uint32_t kTbBarArt[3] = {0, 1, 2};
// StartMenuBar (TAOWMenuBar, 4,7 40x38): TBWin's DragonButton 3-piece.
constexpr std::uint32_t kTbStartArt[2][3] = {{11, 12, 13}, {8, 9, 10}};  // up/down
constexpr int kTbStartX = 4, kTbStartY = 7, kTbStartH = 38;
// The five tab buttons.  Their AOWWindow is TaskPanel (WinLeft 48, width 506,
// LeftOffset 48 / RightOffset 86), so ReAlignComp puts them at
// 48 + 506*LeftOffPercent/100 + LeftOffset with width 14% of 506 = 70:
//   World Map 0%/0, Realm 14%/2, Magic 28%/4, Relations 42%/6, Events 56%/8.
constexpr int kTbTabCount = 5;
constexpr int kTbTabPanelX = 48, kTbTabPanelW = 506, kTbTabW = 70;
constexpr int kTbTabOffPct[kTbTabCount] = {0, 14, 28, 42, 56};
constexpr int kTbTabOffPx[kTbTabCount] = {0, 2, 4, 6, 8};
constexpr int kTbTabY = 7, kTbTabH = 18;
// BarButtonLong.BMP: ILIndexUp 14 -> {14,15,16}, Lit 20 -> {20,21,22},
// Down 17 -> {17,18,19}.
constexpr std::uint32_t kTbTabArt[3][3] = {{14, 15, 16}, {20, 21, 22},
                                           {17, 18, 19}};
const char* const kTbTabText[kTbTabCount] = {"World Map", "Realm", "Magic",
                                             "Relations", "Events"};
// TBNextButton (568,7 26x38): `TBWin` NextButton.BMP - ONE 26x38 picture per
// state, not a 3-piece (TBWin ids 3 and 4 are both 26x38). `TAOWButton.Draw`
// (AoWInterface.dpl 0x0040b338) picks the library's index 0 while the button is
// at rest and index 1 after `CheckMouseDown` set +0x110 = -1, i.e. while the
// button is held down; `CheckMouseUp` clears +0x110 and fires OnClick.
constexpr std::uint32_t kTbNextUpArt = 3;
constexpr std::uint32_t kTbNextDownArt = 4;
constexpr int kTbNextW = 26, kTbNextH = 38, kTbNextX = 568, kTbNextY = 7;
// TurnButton (596,7): Int\TBTurnBt.ILB NextTurn.BMP 41x38, 102 frames. The
// layout is the one `TAOWTurnButton` (AOWTools.dpl, unit "TurnButton") animates:
//   frames 0..10            the "turn changing" swirl (`Create` sets AnimStart
//                           = 0xB; the glow block is 0..10, `Update` mode 2)
//   frames 11+7*r .. 17+7*r one block per RACE r (0..12; 12 = "no seat"),
//                           the block's last frame (+6) being its transition
//                           swirl, which is also the "turn ended" picture.
// `TAOWTurnButton.Draw` (0x59704e44) draws `AnimStart + 7*CurPlayer + 6` while
// the button is Checked and not animating, and its `Frame` (+0x140) otherwise.
constexpr int kTbTurnX = 596, kTbTurnY = 7, kTbTurnW = 41, kTbTurnH = 38;
constexpr int kTbTurnFrameCount = 102;
constexpr int kTbTurnAnimStart = 11;    // TAOWTurnButton.Create: +0x128 = 0xb
constexpr int kTbTurnGlowFrames = 11;   // frames 0..10 (Update mode 2 stops at 10)
constexpr int kTbTurnRaceStride = 7;    // frames per race block
constexpr int kTbTurnNoRace = 12;       // the form maps a -1 race to block 12
// The in-game hit-test ids (Scene::game_hit): 0..4 the five task tabs, 5/6 the
// scanner tabs, 7 the NextButton (TBNextButtonClick), 8 the scanner's minimize,
// 9 the TurnButton (TAOWTurnButton); the Party floater's own ids are 100+.
constexpr int kGameHitNext = 7;
constexpr int kGameHitScanMin = 8;
constexpr int kGameHitTurn = 9;
// The gold / mana read-outs: TAOWPanel WGoldPnl (awRight, RightOffset 148,
// BottomOffset 1) at (423,26) 69x21 with TBWin BarSet2 51, and WManaPnl
// (RightOffset 76) at (495,26) 69x21 with BarSet2 52.
constexpr int kTbResY = 26, kTbResW = 69, kTbResH = 21;
constexpr int kTbGoldX = 423, kTbManaX = 495;
constexpr std::uint32_t kTbGoldArt = 51, kTbManaArt = 52;

// --- the turn timer (TBWorld.TCTimerImg / TCTimeImg / TCTimeLbl) -----------
// TCTimerImg (TBWin BarSet2 28, 33x17) is the recessed clock body, awRight
// rox 222 / ahBottom box 3; TCTimeImg (LBIcons 2, the hourglass, 11x14) rides
// rox 258 / box 5; TCTimeLbl (FontModule.TBarNumber, clBlack) is the read-out
// on the body, awRight rox 225 / box 4. The clock's own digits are variable
// (a turn countdown); the body + hourglass are the fixed art.
constexpr std::uint32_t kTbTimeBodyArt = 28;
constexpr int kTbTimeBodyW = 33, kTbTimeBodyH = 17, kTbTimeBodyRox = 222,
              kTbTimeBodyBox = 3;
constexpr int kTbTimeIcnW = 11, kTbTimeIcnH = 14, kTbTimeIcnRox = 258,
              kTbTimeIcnBox = 5;
constexpr int kTbTimeLblRox = 225, kTbTimeLblBox = 4;
// The read-out's own top edge inside the 48 px bar (TCTimeLbl WinTop 32).
constexpr int kTbTimeLblTop = 32;
constexpr const char* kTbTimeText = "6:14";
// --- the player indicator strip (TBWorld.PInd) -----------------------------
// A frHorizLine 75x17 at awRight rox 275 / ahBottom box 3, painted by
// TBWorld.PIndDraw with the PlyrIndL 3-piece (TBWin 29/30/31 = 2+5+2 x17) -
// one colour pill per player, right-aligned in the strip.
constexpr std::uint32_t kTbPindArt[3] = {29, 30, 31};
constexpr int kTbPindW = 75, kTbPindH = 17, kTbPindRox = 275, kTbPindBox = 3;
// The engine's PIndDraw paints one PlyrIndL pill per game player, right-aligned
// in the strip. The reference capture (5 players) shows five ~6 px pills; the
// count follows the player list at runtime, so the replica draws the reviewed
// count at the reviewed pitch.
constexpr int kTbPindPills = 5, kTbPindPillW = 6;
// --- the gold / mana read-out labels ---------------------------------------
// WGTLbl / WMTLbl (TBarNumber, clBlack) are awRight rox 23 on the panel;
// WGSign / WMTSign ("+", Age8) ride rox 16; the income number (Age8) is
// centred in a 16 px box inset rox 1 - both the sign and the income are the
// green 0x77FF77 = (119,255,119) the DFM's TextColor carries.
constexpr int kTbResTotRox = 23, kTbResSignRox = 16, kTbResIncRox = 1,
              kTbResIncW = 16;
// Vertical placement of the total (TBarNumber) and the sign / income (Age8)
// inside the 21 px read-out panel (WGTLbl box 3, WGILbl box 1).
constexpr int kTbResTotTop = 5, kTbResIncTop = 9;
constexpr std::uint8_t kTbIncomeR = 119, kTbIncomeG = 255, kTbIncomeB = 119;

// --- the World Map floater (TSWindow.ScannerWindow: 200x216, right/top) ----
constexpr int kFloatW = 200, kFloatH = 216;
constexpr int kScanY = 0;                 // anchored to the top edge
constexpr int kEventY = kGameH - kTbBarH - kFloatH;  // 216, above the bar
// GenericF 0..7 is MainFloaterSetR, the floater 9-slice (6px border, 20px top).
constexpr std::uint32_t kFloatFrameBase = 0;
constexpr int kFloatBorder = 6, kFloatTop = 20;
// Int\ScanWin.ILB overview2.BMP 0..7 is the scanner frame's 9-slice.
constexpr std::uint32_t kScanFrameBase = 0;
// The minimap TScanner is at (30,32) 131x152.
constexpr int kScanMapX = 30, kScanMapY = 32, kScanMapW = 131, kScanMapH = 152;
// ScannerTab (31,182 145x15): ScanWin ParchClm 3-piece, up {14,15,16}.
constexpr std::uint32_t kScanTabArt[2][3] = {{14, 15, 16}, {17, 18, 19}};
constexpr int kScanTabX = 31, kScanTabY = 182, kScanTabW = 145, kScanTabH = 15;
const char* const kScanTabText[2] = {"Surface", "Caverns"};
// BuildingCheck / ArmyCheck / NaturalCheck (160, 37/52/67): ScanWin ScanIcon
// 15x15, up 8 / down 9 (lit 0).
constexpr std::uint32_t kScanChkArt[3] = {8, 0, 9};
constexpr int kScanChkX = 160, kScanChkY[3] = {37, 52, 67}, kScanChkSize = 15;
// The header buttons: GenericI min (3/5/4) / help (59/61/60), 15x16.
constexpr std::uint32_t kScanMinArt[3] = {3, 5, 4};
constexpr std::uint32_t kScanHelpArt[3] = {59, 61, 60};

// --- the map window (TMWindow.MapWindow: the Main Map, upper-left) ---------
// The landscape is `TMWindow.MapWindow`, a `TAOWWindow` anchored awLeft/ahTop at
// (0,0) - its DFM gives WinWidth 440 / WinHeight 432 and ILIndexFrame 8 - so at
// the design size the 200 px floater column sits to its right (440+200 = 640)
// and the 48 px bar below (432+48 = 480). Its frame art is the GenericF entry
// that index 8 opens: **MapWindow.BMP**, a 12x12-corner 9-slice (12x12 corners,
// 419x12 top/bottom, 12x408 sides, ordered TL,top,TR,left,right,BL,bottom,BR -
// the same layout the floater 9-slice uses). AoW.exe 0x00451268 sizes the scene
// from the window as `w - 0x18` / `h - 0x18` and shows it at +0xc,+0xc, and the
// map-window mouse handlers (0x00450de8 hit test / 0x00450ef8 client convert)
// use the same inset, so the *client box* is the window inset by the frame's
// 12 px. Confirmed against bin/CornersRef.png: the corners land on the 4 corners
// of a 440x432 window and the edges are drawn 1:1 (tiled), not stretched.
constexpr std::uint32_t kMapFrameBase = 8;   // GenericF 8..15 MapWindow.BMP
constexpr int kMapBorder = 12;               // the frame's pixel thickness

// --- the Events floater (TPInfoW.PInfoWin: 200x216, right/bottom) ----------
// PInfoPnl (6,19 188x66): GenericF 105..112 frame over GenericT 17.
constexpr std::uint32_t kEventPnlFrameBase = 105;
constexpr int kEventPnlX = 6, kEventPnlY = 19, kEventPnlW = 188, kEventPnlH = 66;
// FaceFr / ShieldFr (52x64) and InfoFr / TurnFr (84x46 / 84x20) all carry
// GenericF 113..120 (FrGoldD); FaceFr's body is GenericT 17, ShieldFr's is
// CityWin 5.
constexpr std::uint32_t kEventCellFrameBase = 113;
constexpr int kEventFaceX = 1, kEventFaceY = 1, kEventFaceW = 52,
              kEventFaceH = 64;
constexpr int kEventShieldX = 136, kEventInfoX = 52, kEventInfoW = 84;
constexpr int kEventInfoH = 46, kEventTurnY = 45, kEventTurnH = 20;
// EventFr (6,100 188x110): GenericF 105..112; EventList fills it.
constexpr int kEventListX = 6, kEventListY = 100, kEventListW = 188,
              kEventListH = 110;
constexpr int kEventRowH = 17;    // EventList ItemHeight

// --- the leader banner's own art (TPInfoW's update + FaceFrDraw) ------------
// The Events floater's update (AoW.exe FUN_00423170) fills the banner from the
// seated player: `Player` = TPlayer.GetName, `Race` = RaceNamesPluralRStr[race],
// `Turn` = "Day N", and RShield's `CurrentImage = race + 13` (ShieldsM.ILB ids
// 13..25 are one 40x53 race crest per race; the DFM's CurrentImage 8 is the
// designer's placeholder). FaceFr's own OnDraw (AoW.exe FUN_00423770) takes
// TPlayer.GetFace() -> THero.GetFace() -> the FACERES.PFS record the leader's
// face index names -> that record's image library (e.g. FACES\L_FACES.ILB) at
// its image index, drawn into the FaceFr box inset by 3 px.
constexpr int kEventFaceInset = 3;
constexpr std::uint32_t kEventShieldBase = 13;
constexpr int kEventShieldCount = 13;
// RShield's crest is drawn *blended* (see draw_game_events): the engine's
// class-0x12 crest sprite averages with what is under it, and a 50% mix
// reproduces the reviewed screen's crest exactly.
constexpr int kEventCrestAlpha = 50;
constexpr const char* kFaceResPfs = "Release\\FACERES.PFS";

// --- the in-game fonts -----------------------------------------------------
constexpr const char* kFontAge8Ilb = "Int\\Fonts\\Age8.ILB";
// FontModule.TBarNumber is the bar's numeric face (Int\Fonts\Age10tb.ILB).
constexpr const char* kFontTbNumIlb = "Int\\Fonts\\Age10tb.ILB";

// ------------------------------------------------------- the Party window
// `TControlWin` (AoW.exe, DFM `assets/dfm/AoW/TCONTROLWIN.dfm`), the floater
// the game opens on the World Map when a party is selected. It shows the
// eight unit slots of the selected army in two rows of four, each slot as a
// portrait cell (`Unit{n}Frame`, a 9-slice of `Int\Units.ILB`) holding the
// unit's own sprite, its hit-point bar (`Unit{n}Gge`) and - under the cell -
// the "leave behind" pill (`Unit{n}Chk`) carrying the unit's move-type icon
// (`U{n}MIcon`) and its move points. The window foot carries the party's move
// points (`MovesLbl` + `MoveIcon`) and its status (`PartyStatusLbl`), and the
// button row `PPrev` / `PGuard` / `PMove` / `PNext`.
//
// Recovered with GhidraMCP; the published-method table at AoW.exe 0x00404dcf
// maps every handler to its address (the table's per-entry
// `<u16><addr><len><name>` layout puts the address *before* the name):
//
//   CWMinIconClick     0x00405fc4   CloseClick        0x00405fe4
//   ControlWindowResize 0x00406018  Unit1FrameMouseDown 0x00406040
//   Unit1ChkChange     0x004060d8   CheckStart        0x00406130
//   CheckEnd           0x004062a0   Unit1ChkMouseDown 0x004066bc
//   CenterClick        0x004066f4   Unit1FramePreDraw 0x0040674c
//   HelpClick          0x00406808   PPrevClick        0x00406814
//   PNextClick         0x00406824   PMoveClick        0x00406834
//   PGuardChange       0x00406848
//
// and the component field offsets (real = published - 4):
//   0x44 ControlWindow, 0x50..0x6c Unit1..8Frame, 0x74 MovesLbl,
//   0x78 PartyStatusLbl, 0x80..0x9c U1..8MIcon, 0xa4 MoveIcon,
//   0xa8 PartyT1Pnl, 0xac PartyBPnl, 0xb0 PartyT2Pnl, 0xb4 PartyT3Pnl,
//   0xb8 PartyT4Pnl, 0xbc..0xd8 Unit1..8Gge, 0xdc..0xf8 Unit1..8Chk,
//   0x13c PartyIPnl, 0x144 Center, 0x148 Help, 0x14c PPrev, 0x150 PMove,
//   0x154 PNext, 0x158 PControlPnl, 0x15c PGuard, 0x178 the selection mask.
// See docs/20_PARTY_WINDOW.md.
constexpr const char* kUnitsIlb = "Int\\Units.ILB";       // IntGfxMod.Units
constexpr const char* kIconMoveIlb = "Int\\IconMove.ILB";  // IntGfxMod.MoveIcons
// The unit-box backgrounds (`Int\UnitBgs.ILB`, eight 100x100 washes - the
// `IntGfxMod.UnitBGs` the frame PreDraw draws under the unit's own image).
constexpr const char* kUnitBgsIlb = "Int\\UnitBgs.ILB";
// A unit sprite's nominal image box: every `UNITS\*.ILB` frame is a 64x64 cell
// whose visible art sits at the entry's own base+shift, and the cell's PreDraw
// centres that 64x64 box in its inset rect.
constexpr int kPartyUnitBox = 64;
constexpr int kPartySlots = 8;         // the window holds eight unit slots
constexpr int kPartyW = 200;           // ControlWindow WinWidth (design)
constexpr int kPartyH = 216;           // ControlWindow WinHeight (design)
constexpr int kPartyTop = 216;         // TopOffset (awRight/ahTop)
constexpr int kPartyBorder = 4;        // BorderWidth / BorderHeight
// The units library: 0..7 the empty-slot 9-slice, 8 its 100x100 hatch,
// 12..19 the filled slot's 9-slice, 20..22 / 23..25 the pill (up / down),
// 26..33 the "left behind" slot's 9-slice.
constexpr std::uint32_t kPartyBoxBase = 12;       // Units 12..19 (unit cell)
constexpr std::uint32_t kPartyBoxLeftBase = 26;   // Units 26..33 (left behind)
constexpr std::uint32_t kPartyEmptyBase = 0;      // Units 0..7   (empty cell)
constexpr std::uint32_t kPartyEmptyTex = 8;       // Units 8     (hatch)
constexpr std::uint32_t kPartyPillUp = 20;        // Units 20..22
constexpr std::uint32_t kPartyPillDown = 23;      // Units 23..25
// The title-bar buttons (IntGfxMod.GenericI, 15x16): up / lit / down.
constexpr std::uint32_t kPartyCloseArt[3] = {0, 2, 1};
constexpr std::uint32_t kPartyMinArt[3] = {3, 5, 4};
constexpr std::uint32_t kPartyCenterArt[3] = {6, 8, 7};
constexpr std::uint32_t kPartyHelpArt[3] = {59, 61, 60};
// The foot buttons (IntGfxMod.GenericB, 3 pieces): up / lit / down.
constexpr std::uint32_t kPartyBtnUp = 76;
constexpr std::uint32_t kPartyBtnLit = 82;
constexpr std::uint32_t kPartyBtnDown = 79;
// PartyB Pnl / PartyB2Pbl / PControlPnl body texture (GenericT 14).
constexpr std::uint32_t kPartyPanelTex = 14;
// The party-status resource strings (`AoWE.TPartyStatusRStr`, indexed by
// `TArmy.GetPartyStatus@0x5578f584`). The enum's own value names survive in
// AoWEPACK.dpl's RTTI - in declaration order `psUnruly, psUnrest, psStable,
// psContent, psCheerful` (dumped from the TPartyStatus TTypeInfo), and its two
// converters pin the bands exactly:
//   `MoraleValueToMorale@0x5575ad64`  value <21 -> 0, 21..40 -> 1, 41..60 -> 2,
//                                     61..80 -> 3, >=81 -> 4
//   `MoraleToMoraleValue@0x5575ace0`  0 -> 10, 1 -> 30, 2 -> 50, 3 -> 70, 4 -> 90
// (the band midpoints), and `TAbstractUnit.GetMorale@0x5577f178` is exactly
// `MoraleValueToMorale(*(char*)(unit + 0x26))` - so the byte the map file
// stores (rw tag 6) is the unit's *morale value*.
constexpr const char* kPartyStatusText[5] = {"Unruly", "Unrest", "Stable",
                                             "Content", "Cheerful"};

// ------------------------------------------------- the in-game screen layout
// A component's `Alignment.*` block, exactly as the DFM stores it. The engine's
// AoWComp.TAoWComponent.ReAlignComp (AoWInterface.dpl 0x00401f2c) turns this
// plus the parent's box into the component's device-pixel box, which is what
// makes a bigger window *reflow* the interface instead of stretching it: an
// element with WidthPercent 0 keeps its pixel width and only moves, while the
// task tabs (WidthPercent 14, LeftOffPercent 0/14/28/42/56) widen and spread
// across the bar.
struct AowAlign {
    int aw = 0, ah = 0;   // 0 awNone, 1 awLeft/ahTop, 2 awRight/ahBottom,
                          // 3 awBoth, 4 awCenter
    int wp = 0, hp = 0;   // WidthPercent / HeightPercent
    int lo = 0, ro = 0, to = 0, bo = 0;   // Left/Right/Top/BottomOffPercent
    int lox = 0, rox = 0, tox = 0, box = 0;  // ...Offset (pixels)
    int w = 0, h = 0;     // WinWidth / WinHeight (used when the % is 0)
};

struct AowBox {
    int x = 0, y = 0, w = 0, h = 0;
};

// AoWComp.TAoWComponent.ReAlignComp applied to one component.
inline AowBox aow_realign(const AowAlign& a, int px, int py, int pw, int ph) {
    AowBox b;
    b.w = a.w;
    b.h = a.h;
    switch (a.aw) {
    case 1:  // awLeft
        b.x = pw * a.lo / 100 + a.lox;
        if (a.wp) b.w = a.wp * pw / 100;
        break;
    case 2:  // awRight
        if (a.wp) b.w = a.wp * pw / 100;
        b.x = pw - a.rox - b.w - pw * a.ro / 100;
        break;
    case 3:  // awBoth
        b.x = pw * a.lo / 100 + a.lox;
        b.w = pw - b.x - a.rox - pw * a.ro / 100;
        break;
    case 4:  // awCenter
        b.x = pw / 2 - b.w / 2;
        break;
    default:  // awNone: keep the design offset
        b.x = a.lox;
        break;
    }
    switch (a.ah) {
    case 1:  // ahTop
        b.y = ph * a.to / 100 + a.tox;
        if (a.hp) b.h = a.hp * ph / 100;
        break;
    case 2:  // ahBottom
        if (a.hp) b.h = a.hp * ph / 100;
        b.y = ph - a.box - b.h - ph * a.bo / 100;
        break;
    case 3:  // ahBoth
        b.y = ph * a.to / 100 + a.tox;
        b.h = ph - b.y - a.box - ph * a.bo / 100;
        break;
    case 4:  // ahCenter
        b.y = ph / 2 - b.h / 2;
        break;
    default:
        b.y = a.tox;
        break;
    }
    b.x += px;
    b.y += py;
    if (b.w < 1) b.w = 1;
    if (b.h < 1) b.h = 1;
    return b;
}

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
    // Colour key: class 0x16 (TSprite16) sprites carry no mask, so their
    // background is the palette's entry 0. The cursor art is delivered that
    // way, so the replica keys out the sprite's (0,0) colour the way the
    // engine's colour-key blit does.
    bool has_key = false;
    std::uint8_t key[3] = {0, 0, 0};
    // The offset between a caller's *anchor* and this sprite's visual box - what
    // decode_rgb() returns. Two recovered places need one:
    //   * a `Cursors.ilb` pointer: the engine paints its 49x49 (or 56x55) slot
    //     **centred** on the mouse, so the box lands at
    //     `-slot/2 + base + shift` from the hotspot (the arrow at id 0 carries
    //     shift (23,22), which is exactly what puts its tip on the slot's centre,
    //     and the sparkle / walk / attack icons are centred or glyph-art boxes);
    //   * a hexagon selector half (`Images\General.ILB` 6/7): the engine draws it
    //     at `HXtoHP(hex) + (-8, -0x1e)`, i.e. this offset is
    //     `(-8, -0x1e) + base + shift`.
    // Scene::load fills it; the callers add it to their anchor.
    std::int32_t anchor_dx = 0, anchor_dy = 0;
};

// A multi-layer ILB image (TImageNode): IntGfxMod.UnitIcons / TitleScreen store
// each info-strip icon as a glow layer plus the icon art, both under one image
// id (TImageNode.Show blits every layer in order - Ilpack.dpl 0x5520d654).
struct LayeredSprite {
    Sprite layer[4];
    int count = 0;
};

// Turns a sprite's colour key into real transparency. The map canvas is written
// by `aow::blit_sprite_rgba`, which honours the alpha channel only, while a class
// 0x16 sprite (no mask) carries its background as a key colour the engine drops
// with a colour-keyed blit (`GFXE.TSurface.BltColorKey@0x5500c544` - what
// `Canvas::blit_keyed` does for the pointer sprites). The hexagon selector halves
// are blitted straight into the map canvas, so their key is baked into the alpha
// once, at load time.
inline void key_to_alpha(Sprite& s) {
    if (!s.has_key || s.rgba.empty()) return;
    for (std::size_t i = 0; i + 3 < s.rgba.size(); i += 4) {
        if (s.rgba[i] == s.key[0] && s.rgba[i + 1] == s.key[1] &&
            s.rgba[i + 2] == s.key[2]) {
            s.rgba[i + 3] = 0;
        }
    }
    s.has_key = false;
}

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

    // Flat alpha blend of a rectangle - used for the Scenario screen's fog
    // panels (the engine's R*FogPnl / ShadowOv tint).
    void fill(int x, int y, int w, int h, std::uint8_t r, std::uint8_t g,
              std::uint8_t b, int a) {
        if (a <= 0) return;
        if (a > 255) a = 255;
        for (int j = 0; j < h; ++j) {
            const int dy = y + j;
            if (dy < 0 || dy >= h_) continue;
            for (int i = 0; i < w; ++i) {
                const int dx = x + i;
                if (dx < 0 || dx >= w_) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                const int ia = 255 - a;
                d[0] = static_cast<std::uint8_t>((b * a + d[0] * ia) / 255);
                d[1] = static_cast<std::uint8_t>((g * a + d[1] * ia) / 255);
                d[2] = static_cast<std::uint8_t>((r * a + d[2] * ia) / 255);
                d[3] = 255;
            }
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

    // A class-0x02 bitmap-font glyph (Age8s / Age10 / Age10s / Age10tb): the
    // glyph is stored as a flat ink colour plus palette entry 0 (black) that
    // acts as the background, with the RLE escape runs fully transparent.
    // TImageLibraryFont recolours the ink with the control's TextColor, so draw
    // the ink in (r,g,b) and leave palette entry 0 black - exactly what the
    // reference capture bin/GameTypeRef.png shows for the GameLbl.
    void glyph_ink(const Sprite& s, int x, int y, std::uint8_t r, std::uint8_t g,
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
                const bool bg = (p[0] | p[1] | p[2]) == 0;
                d[0] = bg ? 0 : b;
                d[1] = bg ? 0 : g;
                d[2] = bg ? 0 : r;
                d[3] = 255;
            }
        }
    }

    // Flat-coloured glyph blit: TImageLibraryFont.SetColor keeps the glyph's
    // coverage and applies the requested colour. (Unused by the title screen,
    // which draws the font's own pre-shaded layers, but kept for other screens.)
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

    // Additive blit - the info-strip icons are glow sprites (class 0x16, no
    // mask) that the engine adds to whatever is under them, which is why the
    // shipped strip shows them as bright haloes over the dark well.
    void blit_add(const Sprite& s, int x, int y) {
        if (!s.ok) return;
        const int dx0 = x + s.base_x;
        const int dy0 = y + s.base_y;
        for (std::uint32_t sy = 0; sy < s.h; ++sy) {
            const int dy = dy0 + static_cast<int>(sy);
            if (dy < 0 || dy >= h_) continue;
            for (std::uint32_t sx = 0; sx < s.w; ++sx) {
                const int dx = dx0 + static_cast<int>(sx);
                if (dx < 0 || dx >= w_) continue;
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                const int a = p[3];
                if (a == 0) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                const int add[3] = {p[2] * a / 255, p[1] * a / 255,
                                    p[0] * a / 255};
                for (int c = 0; c < 3; ++c) {
                    const int v = d[c] + add[c];
                    d[c] = static_cast<std::uint8_t>(v > 255 ? 255 : v);
                }
                d[3] = 255;
            }
        }
    }

    // Colour-keyed blit, used for the cursor sprites (see Sprite::has_key).
    void blit_keyed(const Sprite& s, int x, int y) {
        if (!s.ok) return;
        const int dx0 = x + s.base_x;
        const int dy0 = y + s.base_y;
        for (std::uint32_t sy = 0; sy < s.h; ++sy) {
            const int dy = dy0 + static_cast<int>(sy);
            if (dy < 0 || dy >= h_) continue;
            for (std::uint32_t sx = 0; sx < s.w; ++sx) {
                const int dx = dx0 + static_cast<int>(sx);
                if (dx < 0 || dx >= w_) continue;
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                if (p[3] == 0) continue;
                if (s.has_key && p[0] == s.key[0] && p[1] == s.key[1] &&
                    p[2] == s.key[2]) {
                    continue;
                }
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                d[0] = p[2]; d[1] = p[1]; d[2] = p[0]; d[3] = 255;
            }
        }
    }

    // Stretch-blit `s` (nearest neighbour) into the rect (x,y,w,h). Used for the
    // Scenario screen's 9-slice panel frames.
    void blit_stretch(const Sprite& s, int x, int y, int w, int h) {
        if (!s.ok || s.w == 0 || s.h == 0 || w <= 0 || h <= 0) return;
        for (int j = 0; j < h; ++j) {
            const int dy = y + j;
            if (dy < 0 || dy >= h_) continue;
            const std::uint32_t sy = static_cast<std::uint32_t>(j) * s.h / h;
            for (int i = 0; i < w; ++i) {
                const int dx = x + i;
                if (dx < 0 || dx >= w_) continue;
                const std::uint32_t sx = static_cast<std::uint32_t>(i) * s.w / w;
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                if (p[3] == 0) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                d[0] = p[2]; d[1] = p[1]; d[2] = p[0]; d[3] = 255;
            }
        }
    }

    // 1:1 blit of `s` at (x,y), clipped to the rect (cx,cy,cw,ch). Same pixel
    // rule as blit_stretch (transparent source pixels are skipped) and the same
    // BGRA -> RGBA destination order. Used by blit_tile for the window frames.
    void blit_clip(const Sprite& s, int x, int y, int cx, int cy, int cw,
                   int ch) {
        if (!s.ok || s.w == 0 || s.h == 0) return;
        const int x0 = std::max(cx, x);
        const int y0 = std::max(cy, y);
        const int x1 = std::min(cx + cw, x + static_cast<int>(s.w));
        const int y1 = std::min(cy + ch, y + static_cast<int>(s.h));
        for (int dy = y0; dy < y1; ++dy) {
            if (dy < 0 || dy >= h_) continue;
            const std::uint32_t sy = static_cast<std::uint32_t>(dy - y);
            for (int dx = x0; dx < x1; ++dx) {
                if (dx < 0 || dx >= w_) continue;
                const std::uint32_t sx = static_cast<std::uint32_t>(dx - x);
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                if (p[3] == 0) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                d[0] = p[2]; d[1] = p[1]; d[2] = p[0]; d[3] = 255;
            }
        }
    }

    // Tile `s` 1:1 inside the rect (x,y,w,h) - horizontally for the top/bottom
    // edges (`vertical` false), vertically for the side edges - clipped to the
    // rect. The window frames repeat their edge art instead of stretching it,
    // which is what AoWInterface's TAOWWindow.Draw does with its ILFrame library
    // (`while (count--) blit(art, x += art.w, y)`) and what the reference
    // capture shows: at the design size one copy fills each span.
    void blit_tile(const Sprite& s, int x, int y, int w, int h, bool vertical) {
        if (!s.ok || s.w == 0 || s.h == 0 || w <= 0 || h <= 0) return;
        if (!vertical) {
            for (int tx = 0; tx < w; tx += static_cast<int>(s.w)) {
                blit_clip(s, x + tx, y, x, y, w, h);
            }
        } else {
            for (int ty = 0; ty < h; ty += static_cast<int>(s.h)) {
                blit_clip(s, x, y + ty, x, y, w, h);
            }
        }
    }

    // Blit an 8-bit RGBA buffer (the map renderer's output) 1:1 at (dx,dy),
    // clipped to the rect (cx,cy,cw,ch). The in-game map viewport uses this so
    // the level is shown at its native tile scale: a bigger window reveals more
    // of the level instead of magnifying the same tiles.
    void blit_rgba_rect(const std::vector<std::uint8_t>& rgba, std::uint32_t sw,
                        std::uint32_t sh, int dx, int dy, int cx, int cy,
                        int cw, int ch) {
        if (sw == 0 || sh == 0 || cw <= 0 || ch <= 0) return;
        if (rgba.size() < static_cast<std::size_t>(sw) * sh * 4) return;
        const int x0 = std::max(dx, cx), y0 = std::max(dy, cy);
        const int x1 = std::min(dx + static_cast<int>(sw), cx + cw);
        const int y1 = std::min(dy + static_cast<int>(sh), cy + ch);
        for (int y = y0; y < y1; ++y) {
            if (y < 0 || y >= h_) continue;
            const std::uint32_t sy = static_cast<std::uint32_t>(y - dy);
            for (int x = x0; x < x1; ++x) {
                if (x < 0 || x >= w_) continue;
                const std::uint32_t sx = static_cast<std::uint32_t>(x - dx);
                const std::uint8_t* p =
                    &rgba[(static_cast<std::size_t>(sy) * sw + sx) * 4];
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(y) * w_ + x) * 4];
                d[0] = p[2]; d[1] = p[1]; d[2] = p[0]; d[3] = 255;
            }
        }
    }

    // Blit an 8-bit RGBA buffer (the map renderer's output) scaled to the rect
    // (x,y,w,h), nearest neighbour, clipped to the canvas. Used for the in-game
    // screen's map view and its minimap.
    void blit_rgba_scaled(const std::vector<std::uint8_t>& rgba,
                          std::uint32_t sw, std::uint32_t sh, int x, int y,
                          int w, int h) {
        if (sw == 0 || sh == 0 || w <= 0 || h <= 0) return;
        if (rgba.size() < static_cast<std::size_t>(sw) * sh * 4) return;
        for (int j = 0; j < h; ++j) {
            const int dy = y + j;
            if (dy < 0 || dy >= h_) continue;
            const std::uint32_t sy = static_cast<std::uint32_t>(j) * sh / h;
            for (int i = 0; i < w; ++i) {
                const int dx = x + i;
                if (dx < 0 || dx >= w_) continue;
                const std::uint32_t sx = static_cast<std::uint32_t>(i) * sw / w;
                const std::uint8_t* p =
                    &rgba[(static_cast<std::size_t>(sy) * sw + sx) * 4];
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                d[0] = p[2]; d[1] = p[1]; d[2] = p[0]; d[3] = 255;
            }
        }
    }

    // Copy a sub-rectangle out (used by --crop for small previews/goldens).
    Canvas crop(int x, int y, int w, int h) const {
        Canvas out(w > 0 ? w : 1, h > 0 ? h : 1);
        for (int j = 0; j < h; ++j) {
            const int sy = y + j;
            if (sy < 0 || sy >= h_) continue;
            for (int i = 0; i < w; ++i) {
                const int sx = x + i;
                if (sx < 0 || sx >= w_) continue;
                std::memcpy(
                    &out.px_[(static_cast<std::size_t>(j) * out.w_ + i) * 4],
                    &px_[(static_cast<std::size_t>(sy) * w_ + sx) * 4], 4);
            }
        }
        return out;
    }

private:
    int w_, h_;
    std::vector<std::uint8_t> px_;
};

// The five title-menu entries, in the order the handler at 0x0041ae94
// dispatches them (0 = leftmost orb). The engine loads these from a runtime
// resource-string array (AoW.exe data 0x0045a010, five AnsiString slots) whose
// text ids are not yet resolved, so the values below come from the game's own
// screens: the crossed-swords orb (index 0) reads "Scenario" and the globe orb
// (index 1) reads "New Game". Indexes 2..4 are inferred from the five actions
// (0x0041ae94: scenario setup / show file window / exit) - override them with
// --captions or see docs/06_TITLE_SCREEN.md.
const char* kDefaultCaptions[kButtonCount] = {
    "Scenario", "New Game", "Load Game", "Options", "Exit",
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

// The UI thread hands the audio thread a new song list through this (it is
// defined below, after `App g`, which owns the shared music state).
void set_music_playlist(std::vector<std::string> songs);
// One-shot sample playback, the game's `Sound.PlaySample` (SoundP.dpl): the
// shared UI click is `Sfx\Int\Btn1.wav` (AoW.exe FUN_00455dfc mode 0) and a
// human's turn start is `SFX\EVENTS\NEWTURN.WAV` (TTurnPlayerControl.NewTurn
// 0x55756714). Resolved against the game root; silent with --no-music.
void play_game_sfx(const std::string& rel);

// ---------------------------------------------------------------- the screen
struct Scene {
    aow::IlbFile title_il;   // Int\Scenes\title.ilb  (TitleSceneIL)
    aow::IlbFile font_il;    // Int\Fonts\Age15WhGr.ILB (AoW15WhiteGrey)
    aow::IlbFile cursors_il; // Images\Cursors.ilb
    aow::IlbFile general_il; // Images\General.ILB (the hexagon selector halves)

    Sprite background;                       // id 0  ScrollBackg5
    Sprite logo;                             // id 3  LogoTitleFinal (solid)
    Sprite orb[kButtonCount][kOrbStride];    // id 10 + 3i + state
    Sprite orb_overlay;                      // id 25 TSprite16 overlay

    // One font glyph: the selected colour variant of the letterform.
    struct Glyph {
        Sprite face;
        bool ok = false;
    };
    std::vector<Glyph> glyphs;               // indexed by ASCII code
    Sprite cursor;                           // the pointer sprite

    // --- the Game Type screen (TMultiWindow) ------------------------------
    aow::IlbFile menu_il;        // Int\Scenes\TitleM.ILB (IntGfxMod.TitleScreen)
    aow::IlbFile font8_il;       // Int\Fonts\Age8s.ILB   (FontModule.Age8s)
    std::vector<Glyph> glyphs8;  // GameLbl glyphs, indexed by character code
    Sprite gt_bg;                // id 1  SetupBg03      640x480 backdrop
    Sprite gt_top;               // id 2  GameMenuTop    158x286 top panel
    Sprite gt_bottom;            // id 3  GameMenuBottom 158x66  bottom panel
    Sprite gt_btn[kOrbStride];   // id 20/21/22 TitleIcon 158x24, [0]=up,[1]=lit,[2]=down

    // --- the Scenario screen (TScenSetup) ---------------------------------
    aow::IlbFile plyr_il;       // Int\Scenes\PlyrIcns.ILB  (race icons)
    aow::IlbFile genericf_il;   // Int\GenericF.ILB   (panel frames)
    aow::IlbFile generict_il;   // Int\GenericT.ilb   (panel tint)
    aow::IlbFile font10_il;     // Int\Fonts\Age10s.ILB (race names)
    std::vector<Glyph> glyphs10;
    Sprite scen_bg;                  // TitleM id 4  Backg03  640x480
    Sprite scen_frame[8];            // GenericF 72..79  (9-slice, no centre)
    Sprite scen_btn[3][kOrbStride];  // SceneBtn triples (55/58/61)
    Sprite scen_chk[2];              // ScenChk ids 41 (off) / 42 (on)
    aow::IlbFile genericb_il;        // Int\GenericB.ILB  (celtic buttons)
    aow::IlbFile titleicon_il;       // Int\WinTIcon.ilb  (dialog title icon)
    aow::IlbFile uniticons_il;       // Int\Icons.ILB     (size/towns icons)
    Sprite scen_dlg_frame[8];        // GenericF 32..39  DialogFr 9-slice
    Sprite scen_gold_frame[8];       // GenericF 113..120 FrGoldD 9-slice
    Sprite scen_thin_frame[8];       // GenericF 105..112 FrThinD 9-slice
    Sprite scen_body_tex;            // GenericT 2  MainFloaterSet (dialog body)
    Sprite scen_dlg_btn[3][3];       // GenericB 19..27 ButRCelt2 [state][piece]
    Sprite scen_dlg_title_icon;      // WinTIcon id 8
    Sprite scen_close[3];            // GenericI 0/1/2 MainHeaderBut (close box)
    Sprite scen_updir[3];            // GenericI 56/57/58 NGButDirBack (UpDirBtn)
    LayeredSprite scen_icon_size;    // Icons.ILB id 7 (glow + globe)
    LayeredSprite scen_icon_towns;   // Icons.ILB id 8 (glow + castle)
    LayeredSprite scen_icon_players; // TitleM id 10 (glow + leader face)
    aow::IlbFile shields_il;         // Int\ShieldsM.ILB  (race crest shields)
    aow::IlbFile celldeco_il;        // Int\CeltDeco.ILB  (celtic knot bands)
    aow::IlbFile menus_il;           // Int\Menus.ILB     (list selection)
    Sprite scen_shield[12];          // race shields (ids 0..11)

    // --- the exact art the DFMs name (see the kMainBtnArt.. block above) -----
    aow::IlbFile generics_il;        // Int\GenericS.ilb (scrollbars/sliders)
    aow::IlbFile generice_il;        // Int\GenericE.ILB (edit skins)
    aow::IlbFile font10b_il;         // Int\Fonts\Age10.ILB (dialog face)
    std::vector<Glyph> glyphs10b;    // FontModule.Age10 glyphs
    Sprite scen_panel_tex;           // GenericT 1  ShadowOv   (panel body)
    Sprite scen_scenfr[8];           // GenericF 64..71 ScenFr (group frames)
    Sprite scen_main_btn[3][3];      // TitleM 32/35/38 ScenRBut 3-piece x3
    Sprite scen_scenebut[3];         // TitleM 26/27/28 ScenEBut (settings combo)
    Sprite scen_list_tex;            // GenericT 21 Text2Lb17 (list body)
    Sprite scen_list_sel_tex;        // Menus 8 PopUpSet (list selection)
    Sprite scen_info_tex;            // GenericT 17 TextOld1  (info wells)
    Sprite scen_info2_tex;           // GenericT 18 TextOld2  (info panel body)
    Sprite scen_vsb[5];              // GenericS 56/57/58/59/63 ScenVSB
    Sprite scen_ngvsb[5];            // GenericS 94/96/99/98/101 NGVsbLight (NSBar)
    Sprite scen_slider[6];           // GenericS 49..54 ScenSlid (track+thumb)
    Sprite scen_edit[3];             // GenericE 15..17 NGEdit   (dark edit field)
    Sprite scen_combo[3];            // GenericE 21..23 NGCmb    (dark combo+arrow)
    Sprite scen_col[3];              // TitleM 52..54 ScenCol (list headers)
    Sprite scen_chated[3];           // TitleM 23..25 ScenEd  (chat input)
    Sprite scen_ngcbox[2];           // GenericI 47/48 NGCBox (checkbox off/on)
    Sprite scen_blank_tex;           // GenericT 4 blank1 (list / chat bodies)
    // The race cells' R*PlayerIcon: IntGfxMod.PlyrIcns keeps six TImageNodes
    // (one per TPlayerType), each with the aura layer + the glyph layer that
    // TImageNode.Show blits in order (Ilpack.dpl 0x5520d654). FUN_00410f2c
    // selects the node with TAOWImage.SetCurrent: type 0 (Human) -> 0,
    // 1 .. 3 (CPU Squire/Knight/Lord) -> 1..3 and 6/7 (King/Emperor) -> 4/5.
    LayeredSprite scen_plyr[6];
    Sprite scen_celt[3];             // CeltDeco 55/56/57 (horizontal knot band)
    Sprite scen_celt_knot;           // CeltDeco 3 (27x27 knot cross)

    // Interaction state.
    int scen_glow = kScenGlowFrames;   // per-race fog: 0 = fogged, 90 = lit
    int scen_hover = -1;               // hovered button 0..2, or -1
    int scen_pressed = -1;
    // A race lights up only once the chosen map's player list contains it
    // (FUN_00410f2c / FUN_00411460). Before a map is confirmed every cell reads
    // "Unavailable" - exactly what the reviewed screen shows with the picker
    // still open.
    bool scen_available[kScenRaceCount] = {
        false, false, false, false, false, false, false, false, false, false,
        false, false};
    // The player type of each race (indexed by the engine race id, 0..11), read
    // from the chosen map's TPlayer records (rw tag 0x3) and editable through the
    // R*PlayerCB drop-down (RacePlayerCBChange 0x00412a24).
    int scen_ptype[kScenRaceCount] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // The map's own player type per race - what hPickMap stored as a settings
    // record's *default* (`+0x10`), before its "only the first Human" pass.
    // `ResetPlayerSettings` (0x557e1378) puts a slot back to this, except that a
    // default-human slot resets to AI1 (CPU Squire).
    int scen_ptype_default[kScenRaceCount] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int scen_dd_gi = -1;      // the cell whose PlayerCB drop-down is open, or -1
    int scen_dd_hover = -1;   // the hovered drop-down item (0..6), or -1
    bool scen_check[4] = {true, false, false, true};  // the Misc checkboxes
    // The DFM ships all four unchecked; the engine's setup settings default
    // "Leaders on map" and "Exploration" to on, which is what the shipped
    // capture shows (checked = GenericI 48, the blue box).
    bool scen_maps_open = false;       // the "Select a Map..." dialog
    std::vector<std::string> scen_maps;  // Scenario\Custom\*.HSM file names
    int scen_map_sel = -1;             // the picker opens with nothing selected
    std::string scen_map_name;         // the chosen file (leaf name)
    // The ScenarioLbl caption once a map is confirmed: the file's stem
    // ("Blood Isles.hsm" -> "Blood Isles"), exactly as the shipped title reads
    // "Scenario: Blood Isles".
    bool scen_map_confirmed = false;
    std::string scen_map_title;
    std::string scen_map_size = "M";   // the info strip (size/towns/players)
    int scen_map_towns = 0;
    int scen_map_players = 0;
    int scen_map_off = 0;              // the picker's first visible row (NSBar)
    bool scen_sb_drag = false;         // dragging the list scrollbar thumb
    int scen_sb_grab = 0;              // mouse offset within the thumb
    std::string root_dir;              // the game root (to load a chosen .HSM)

    // --- the in-game screen (TWorldMap + its floaters) ---------------------
    aow::IlbFile tbwin_il;        // Int\TBWin.ILB      (bottom bar + read-outs)
    aow::IlbFile lbicon_il;       // Int\LBIcon.ILB     (timer hourglass)
    aow::IlbFile turnbt_il;       // Int\TBTurnBt.ILB   (TurnButton)
    aow::IlbFile scanwin_il;      // Int\ScanWin.ILB    (scanner frame + icons)
    aow::IlbFile generici_il;     // Int\GenericI.ILB   (min / help buttons)
    aow::IlbFile citywin_il;      // Int\Scenes\CityWin.ILB (ShieldFr body)
    aow::IlbFile eventicn_il;     // Int\EventIcn.ILB   (event-list icons)
    aow::IlbFile font8b_il;       // Int\Fonts\Age8.ILB       (FontModule.Age8)
    aow::IlbFile fonttb_il;       // Int\Fonts\Age10tb.ILB    (TBarNumber)
    std::vector<Glyph> glyphs8b;  // FontModule.Age8 glyphs
    std::vector<Glyph> glyphsTb;  // FontModule.TBarNumber glyphs
    Sprite tb_bar[3];             // TBWin 0/1/2  BarSet 3-piece (640x48)
    Sprite tb_start[2][3];        // TBWin DragonButton (11/12/13 up, 8/9/10 down)
    Sprite tb_tab[3][3];          // TBWin BarButtonLong (14/20/17 triples)
    Sprite tb_next_idle;          // TBWin 3  NextButton.BMP (idle)
    Sprite tb_next_pressed;       // TBWin 4  NextButton.BMP (held down)
    // TAOWTurnButton's own art: the whole 102-frame Int\TBTurnBt.ILB, indexed
    // exactly like the class indexes it (+0x140 / AnimStart + 7*CurPlayer + 6).
    std::vector<Sprite> tb_turn_frames;
    // --- TAOWTurnButton state (AOWTools.dpl "TurnButton", instance +0x144) ---
    int tb_turn_cur = kTbTurnNoRace;   // +0x118 CurPlayer (a RACE id; 12 = none)
    int tb_turn_last = kTbTurnNoRace;  // +0x11c LastPlayer
    int tb_turn_mode = 0;              // +0x124 the animation state (0..3)
    int tb_turn_start = kTbTurnAnimStart;  // +0x128 AnimStart
    bool tb_turn_animating = false;    // +0x120 Animating
    bool tb_turn_checked = false;      // +0x13c Checked ("turn ended")
    int tb_turn_frame = 0;             // +0x140 Frame
    bool tb_next_down = false;         // TBNextButton held down (+0x110 = -1)
    // TPlayer+0xb4 - the army index `TAoWHSMap.SelectNextArmy` continues from.
    int tb_next_index = -1;
    bool game_turns_started = false;   // the turn loop is running
    Sprite tb_gold;               // TBWin 51   BarSet2 69x21 (gold read-out)
    Sprite tb_mana;               // TBWin 52   BarSet2 69x21 (mana read-out)
    Sprite tb_time_body;          // TBWin 28   BarSet2 33x17 (timer body)
    Sprite tb_time_icon;          // LBIcon 2   the hourglass (timer icon)
    Sprite tb_pind[3];            // TBWin 29/30/31  PlyrIndL 3-piece
    Sprite float_frame[8];        // GenericF 0..7  MainFloaterSetR 9-slice
    Sprite float_body;            // GenericT 2    MainFloaterSet body texture
    Sprite scan_frame[8];         // ScanWin 0..7  overview2 9-slice
    Sprite scan_tab[2][3];        // ScanWin ParchClm (14/17 triples)
    Sprite scan_chk[3];           // ScanWin ScanIcon (8 up / 0 lit / 9 down)
    Sprite scan_min[3];           // GenericI 3/5/4
    Sprite scan_help[3];          // GenericI 59/61/60
    Sprite scan_title_icon;       // WinTIcon 0  (the "World Map" header icon)
    Sprite event_title_icon;      // WinTIcon 1  (the "Events" header icon)
    Sprite map_frame[8];          // GenericF 8..15  MapWindow.BMP 9-slice
    Sprite event_pnl_frame[8];    // GenericF 105..112
    Sprite event_cell_frame[8];   // GenericF 113..120  (FrGoldD)
    Sprite event_body_tex;        // GenericT 17  (banner cell bodies)
    Sprite event_shield_tex;      // CityWin 5    (ShieldFr body)
    Sprite event_shield;          // Shields 8    (the designer's placeholder)
    // --- the Party window (TControlWin) --------------------------------------
    aow::IlbFile units_il;        // Int\Units.ILB      (IntGfxMod.Units)
    aow::IlbFile iconmove_il;     // Int\IconMove.ILB   (IntGfxMod.MoveIcons)
    aow::IlbFile unitbgs_il;      // Int\UnitBgs.ILB    (IntGfxMod.UnitBGs)
    Sprite party_unitbg[8];       // UnitBgs 0..7  the unit-box washes
    Sprite party_box[8];          // Units 12..19  the unit cell's 9-slice
    Sprite party_box_left[8];     // Units 26..33  the left-behind cell's
    Sprite party_empty[8];        // Units 0..7    the empty cell's 9-slice
    Sprite party_empty_tex;       // Units 8       the empty cell's hatch
    Sprite party_pill[2][3];      // Units 20..22 up / 23..25 down (3 pieces)
    Sprite party_foot[3];         // Units 9..11  the foot panels' 3-piece
    Sprite party_panel_tex;       // GenericT 14  (the foot panels' body)
    Sprite party_gauge[2];        // GenericT 4/5 (Unit{n}Gge's track / fill)
    Sprite party_icon[3];         // WinTIcon 4    the title-bar shield
    Sprite party_close[3];        // GenericI 0/2/1
    Sprite party_min[3];          // GenericI 3/5/4
    Sprite party_center[3];       // GenericI 6/8/7
    Sprite party_help[3];         // GenericI 59/61/60
    Sprite party_btn[3][3];       // GenericB 76/82/79 (3 pieces each)
    // --- the Structure window (TIWindow) art -------------------------------
    // TIWINDOW.dfm names: InfoWin (GenericF/GenericT), the four title buttons
    // (GenericI), SIcon (WinTIcon 2), CeltFrame (CeltDeco 22), MainBtn
    // (GenericB 19/25/22), CityBorderPnl (GenericF 115 + the BCS city view),
    // RazePnl (CeltDeco 74 + GenericI 53/55/54), GoldIcn (LBIcons 31) and the
    // Age10/Age8 fonts. All are decoded from the game's own libraries.
    aow::IlbFile prodwin_il;      // Int\ProdWin.ILB
    aow::IlbFile relwin_il;       // Int\RelWin.ILB (race-relation icon)
    aow::IlbFile iwin_race_il;    // Images\RACES\R*.ILB (BCS city view)
    Sprite iwin_bg;               // GenericT 0   TextBackGround
    Sprite iwin_body;             // GenericT 2   MainFloaterSet
    Sprite iwin_frame[8];         // GenericF 0..7 MainFloaterSetR
    Sprite iwin_gold_frame[8];    // GenericF 113..120 FrGoldD
    Sprite iwin_title_icon;       // WinTIcon 2
    Sprite iwin_close[3];         // GenericI 0/2/1
    Sprite iwin_min[3];           // GenericI 3/5/4
    Sprite iwin_center[3];        // GenericI 6/8/7
    Sprite iwin_help[3];          // GenericI 59/61/60
    Sprite iwin_enter[3][3];      // GenericB 19/25/22 (3 pieces)
    Sprite iwin_celt_main[8];     // CeltDeco 30..37 FrameMain2 9-slice
    Sprite iwin_celt_knot;        // CeltDeco 3   27x27 title-bar knot
    Sprite iwin_celt_arch;        // CeltDeco 8   76x58 arch top
    Sprite iwin_celt_side;        // CeltDeco 9   76x58 frame tile
    Sprite iwin_celt_line_a;      // CeltDeco 17  50x5
    Sprite iwin_celt_line_b;      // CeltDeco 22  50x6
    Sprite iwin_raze_bg;          // CeltDeco 74  56x35
    Sprite iwin_raze_btn[3];      // GenericI 53/55/54
    Sprite iwin_gold_icon;        // LBIcon 31
    Sprite iwin_mana_icon;        // LBIcon 33
    Sprite iwin_prod_orb;         // ProdWin 4    39x37
    Sprite iwin_relation;         // RelWin 11    race relation icon
    std::vector<Sprite> move_icons;  // Int\IconMove.ILB, all 20 entries
    // The Events banner's live content (see kEvent* above): the seated player's
    // leader portrait (FACERES.PFS -> its image library) and the thirteen race
    // crests RShield's `CurrentImage = race + 13` selects from.
    Sprite event_shield_race[kEventShieldCount];  // ShieldsM ids 13..25
    std::vector<aow::FaceResource> game_face_res;  // FACERES.PFS (face tag -> art)
    aow::IlbFile game_face_il;                    // the leader's face library
    std::string game_face_il_name;                // which library is loaded
    std::string game_face_lib;                    // its engine name (evidence)
    int game_face_index = -1;                     // the face record's tag
    Sprite game_face;                             // the leader's portrait
    // ShieldsM id the banner draws (`race + kEventShieldBase`), or -1 while no
    // map/seat is loaded - then the DFM's own design-time `CurrentImage` 8
    // (`event_shield`) is drawn, exactly as the form ships.
    int game_shield_index = -1;
    Sprite event_icon;            // EventIcn 0   (the row icon)
    // The map being played and its render (RGBA, top-down).
    std::string game_map;                 // the .HSM path
    std::string game_map_name;            // its leaf name (for the title strip)
    // The song list the in-game screen plays (AoW.exe FUN_00454d2c), resolved
    // from the map's playlist by game_playlist() when the map is loaded.
    std::vector<std::string> game_songs;
    std::vector<std::uint8_t> game_px;    // rendered map (RGBA)
    std::uint32_t game_pw = 0, game_ph = 0;
    // The map is kept (it used to be dropped after rendering) because the world
    // map's units live in it: selecting an army and walking it relocates its
    // AoWE.TArmyHS between the level's field tables and re-renders, exactly what
    // the viewer does (docs/15_MOVEMENT.md).
    aow::Map game_state_map;                  // the loaded scenario
    std::vector<aow::MapResource> game_res;   // Release.hss resources

    // --- world-map unit movement (docs/15_MOVEMENT.md) ---------------------
    // The recovered engine data (the startup MovePointTables, the unit
    // catalogue and the scenario roster) plus the current selection and the
    // path to the hovered hex. `game_draw` is `game_px` with the path overlay
    // (`Cursors.ilb` arrows / X) composited on top - what render_game blits.
    std::array<aow::MovePointTable, 256> mv_tables{};
    aow::UnitCatalog mv_cat;              // Unitres/Herores for move points
    aow::GameState mv_gs;                 // the roster (which army is whose)
    std::uint8_t mv_human = 0xff;         // the local human player's index
    bool mv_ready = false;                // roster + terrain resolved
    bool mv_selected = false;             // an army of ours is selected
    int mv_sel_x = -1, mv_sel_y = -1;     // the selected army's hex
    aow::ArmyMove mv_army;                // its folded table / budget
    aow::MovePath mv_path;                // the path to the hovered hex
    bool mv_have_path = false;            // `mv_path` is worth drawing
    // The walk animation (aow/unit_anim.hpp). A move relocates the army's data
    // at once and hides it on the destination hex; this walks its *sprite*
    // across the path, one animation frame per timer tick, using the unit's own
    // walking sequence for each step's direction.
    bool mv_anim = false;
    aow::WalkAnimation mv_walk;
    int mv_anim_clock = 0;                // AoWHSMap+0x40
    int mv_anim_phase = 0;                // the per-unit (ptr>>2)&0xffff offset
    aow::MapUnit mv_anim_unit;            // the moving army's lead unit (poses)
    aow::MapHs mv_anim_army;              // the whole army (its size banner)
    int mv_hide_level = -1;               // the hex whose army is hidden meanwhile
    int mv_hide_x = -1, mv_hide_y = -1;
    // A merge move in flight (`TMoveArmyTE.FinishMove@0x55747fbc` runs when the
    // walk arrives): the incoming army is off the map while its sprite walks,
    // the destination army stays on the map untouched and visible, and the
    // Party window shows the incoming party until the two combine. This is what
    // keeps the destination from vanishing at the start of the animation.
    bool mv_merge_pending = false;
    int mv_merge_x = -1, mv_merge_y = -1;
    std::size_t mv_merge_stay = 0;        // units left behind at the origin
    aow::MapHs mv_merge_moving;           // the party that is walking
    std::vector<std::uint8_t> game_draw;  // game_px + the overlay (RGBA)
    // The level's *contents* layer, kept alongside the flat `game_px` so the
    // move-selection marker can put the hex's contents back over its far wall
    // (mapviz's split render - the terrain layer it also produces is only a local
    // there). Empty when the split render failed; the marker then degrades to the
    // plain fill. See mv_draw_hex_selector and docs/09_GAME_SCREEN.md 6.9.
    std::vector<std::uint8_t> mv_px_contents;  // objects, roads, cities, armies
    bool mv_contents_stale = true;             // the layer must be rebuilt
    // Pixels of the marker's far wall that the level's contents were composited
    // back over in the last marker draw - evidence that a city/unit on the hovered
    // hex is visible inside the marker (pinned by build.ps1).
    std::size_t mv_selector_contents_px = 0;
    // The rectangle of `game_draw` the last overlay painted: on the next refresh
    // it is restored from `game_px` instead of recopying the whole canvas.
    bool mv_dirty = false;
    int mv_dirty_x0 = 0, mv_dirty_y0 = 0, mv_dirty_x1 = 0, mv_dirty_y1 = 0;
    // How far around a hex's origin its overlay sprites can reach (a 19x14 arrow
    // inside a 48x32 hex, blitted at `HXtoHP - 8 + (vx,vy)`).
    static constexpr int kOverlayPad = 40;
    std::string game_status;              // the last action's feedback
    // The recovered game state shown on the screen. `game_leader`/`game_race`
    // are only the pre-load placeholders: `game_resolve_banner` fills them from
    // the seated player as soon as a map is loaded (name / plural race name).
    std::string game_leader = "Nicolo";        // the leader's name
    std::string game_race = "Humans";          // his race
    int game_day = 1;                          // the turn counter ("Day 1")
    int game_gold = 140, game_gold_income = 10;
    int game_mana = 250, game_mana_income = 10;
    int game_level = 0;                        // the level the scanner shows
    int game_level_count = 1;                  // levels in the loaded map
    bool game_buildings = false, game_armies = false, game_natural = false;
    std::string game_event = "Day 1 dawns";    // the first event-history row
    bool game_show_scan = true;      // the World Map tab toggles this floater
    bool game_show_events = true;    // the Events tab toggles this floater
    int game_hover = -1;             // hovered in-game control, or -1
    // --- the Party window's own state ----------------------------------------
    // `party_sel` is `TControlWin+0x178`, the travelling selection mask
    // (`TSelectedArmy.SetSelection`): bit `i` set = army unit `i` marches with
    // the party. A slot whose unit is *not* in the mask is a unit left behind,
    // which is exactly what clicking the pill under its portrait toggles
    // (`FUN_00406130` rebuilds the mask from the pills, `FUN_004060d8` files it
    // back through `TSelectedArmy.SetSelection`).
    bool game_show_party = false;    // the floater is up (a party is selected)
    std::uint8_t party_sel = 0;      // +0x178 travelling mask
    int party_focus = -1;            // the unit a portrait click focused
    int party_hover = -1;            // hovered party control (kParty*), or -1
    int party_pressed = -1;          // the pressed button
    bool party_guard = false;        // PGuard.Checked (TArmy+0x28 bit 0)

    // --- the Structure floater (TIWindow / `IWindow`) -----------------------
    // Clicking a selectable building opens the 200x216 `InfoWin` reproduced from
    // TIWINDOW.dfm. The window is a *view* over the selected map object: every
    // value below is read from the structure/city when it opens, and the panel
    // reuses the in-game clock for its pulsing building marker. See the
    // reverse-engineering notes in docs/17/19 and the new docs/25_STRUCTURE_PANEL.md.
    bool game_show_structure = false;
    int game_struct_hx = -1, game_struct_hy = -1;  // the selected hex
    bool game_struct_city = false;                 // TCity (city panel)
    std::string game_struct_name;                  // TCity.GetName / class name
    std::string game_struct_race;                  // Race: <this>
    std::string game_struct_status;                // Status: <this>
    std::string game_struct_production;            // "Producing ..."
    int game_struct_gold = 0;                      // the gold read-out
    int game_struct_mana = 0;                      // the mana read-out
    int game_struct_owner = 0;                     // owning player (0 = neutral)
    int game_struct_race_index = -1;               // race library index
    int game_struct_pressed = -1;                  // pressed control id
    int game_struct_hover = -1;                    // hovered control id
    std::string game_struct_art_lib;               // loaded race/BCS library
    Sprite game_struct_art;                        // BCS_<race>.BMP (city view)

    // --- the world-map camera (HSEngine.THSMapViewer, HSEPack.dpl) ---------
    // The engine's camera is a rectangle in *map pixels* over the level. Every
    // field below is the viewer's own, at the offsets the decompiles use:
    //   +0x30 SceneX  +0x34 SceneY    the view's top-left, in map pixels
    //   +0x28 SceneWidth  +0x2c SceneHeight   the viewport size, in pixels
    //   +0x38 SceneL                  the level being viewed
    // Evidence: HSEPack.dpl 0x5560ec44 (Create), 0x5560edf0/0x5560ee38
    // (SetSceneX/SetSceneY), 0x5560ee80/0x5560ee98 (SetSceneWidth/Height),
    // 0x5560edd0 (SetSceneL), 0x5560ed20 (CenterScene), 0x5560ecc4
    // (ChangeScene), 0x5560f000 (ShowScene); the pixel grid is
    // HSEngine.HXtoHP 0x5560e3b4 / HPtoHX 0x5560e3d8; the map extents are
    // THSMap.GetMaxXhp 0x5560c3ec / GetMaxYhp 0x5560c414.
    int cam_x = 16;    // SceneX  - clamped to >= 16 by SetSceneX
    int cam_y = 16;    // SceneY
    int cam_w = 0;     // SceneWidth  - the map window's client width
    int cam_h = 0;     // SceneHeight
    int cam_level = 0; // SceneL
    std::uint32_t map_w = 0, map_h = 0;  // the shown level's size, in hexes
    // A freshly started game opens centred on the human player's leader - what the
    // engine's map-open path does with TAoWE.TAoWHSMap.CenterOnLeader (AoWEPACK
    // 0x55778e1c, the same routine the `L` key calls). The viewport size
    // (cam_w/cam_h) is only known once game_update() has resolved the layout, so
    // start_game() arms this flag and the first update applies it.
    bool game_center_on_start = false;
    // THSMapMouse (HSEPack.dpl 0x5560b53c..) - the mouse in the map window's
    // *client* pixels (-1 = the engine's "outside" sentinel 0x80000000) and the
    // hex it resolves to via GetXhx/GetYhx (0x5560b6b0 / 0x5560b718).
    int game_mx = -1, game_my = -1;
    int game_hover_hx = -1, game_hover_hy = -1;
    // THexagonSpriteSelection's current location (what a mouse-down selects).
    int game_sel_hx = -1, game_sel_hy = -1;
    bool game_mouse_down = false;
    // AoW.exe 0x00455eac: the edge scroll is suppressed while a *modal* window
    // is visible. The replica has none, so this stays false (the gate is kept
    // so the behaviour is explicit).
    bool game_modal_open = false;

    int disp_w = 640, disp_h = 480;
    bool overlay = false;                    // the engine +0x6d == 2 effect
    bool cursor_on = true;
    std::uint32_t cursor_id = kDefaultCursorId;
    // --- the world-map pointer (THSMapMouse + TAoWE.TAoWHSMap.UpdateCursor) --
    int game_clock = 0;                        // AoWHSMap+0x40 (map clock)
    int game_cursor_state = kCursorStateDefault;           // THSMapMouse+0x34
    std::uint32_t game_cursor_index = kDefaultCursorId;    // THSMapMouse+0x30
    // Every `Images\Cursors.ilb` pointer the map can paint, filed by ILB id (the
    // ids the engine writes into THSMapMouse+0x30): the hover sparkle, the attack
    // and join icons, and the walk / sail / fly pairs. `ok` is false for a slot
    // the library does not carry.
    static constexpr std::size_t kMapCursorSlots = 0x48;
    std::array<Sprite, kMapCursorSlots> map_cursor{};
    // The hexagon selector's two halves (`Images\General.ILB` 6 = the lower
    // perimeter, 7 = the upper edges), both drawn at `HXtoHP(hex) + (-8, -0x1e)`.
    Sprite hex_sel_front, hex_sel_back;
    int font_variant = kFontVariant;         // 0 = black ink, 1 = white ink
    std::uint32_t orb_slot_w = 0;            // TLibraryImage width (+0x20)
    std::uint32_t orb_ids[kButtonCount][kOrbStride] = {};  // resolved entry ids
    std::string captions[kButtonCount];

    // Input state, mirroring the scene object's fields.
    int cursor_index = -1;  // +0x7c : index under the cursor
    int armed_index = -1;   // +0x80 : index recorded on mouse-down
    int mouse_x = 0, mouse_y = 0;
    std::uint32_t counter64 = 0;  // +0x64 : backdrop + logo frame counter
    std::uint32_t counter68 = 0;  // +0x68 : credits timing counter

    // Which screen is showing, and the TMultiWindow animation/input state.
    int screen = 0;               // 0 = title, 1 = Game Type (TMultiWindow),
                                  // 2 = Scenario (TScenSetup), 3 = in-game
                                  // (TWorldMap)
    int gt_frame = 0;             // TMultiWindow +0xF0 (the slide counter)
    int gt_hover = -1;            // hovered Game Type button (0..2, 3 = Back)
    int gt_pressed = -1;          // button held down (for the "down" frame)

    // --- geometry recovered from 0x00419c84 --------------------------------
    int scene_top() const { return disp_h / 2 - kHalfScene; }
    int scene_bottom() const { return scene_top() + kSceneHeight; }
    int button_left(int i) const {
        // ((orb_slot_width + left + right) >> 1) - 320 + i*120, with left = 0
        // and right = display width. The object code reads the image's slot
        // width (+0x20), not the RLE visual box.
        return ((static_cast<int>(orb_slot_w) + disp_w) >> 1) - 320
               + i * kButtonPitch;
    }
    int logo_left() const { return (disp_w - static_cast<int>(logo.w)) >> 1; }

    bool load(const std::string& root, std::string* error) {
        std::string err;
        const fs::path base(root);
        root_dir = root;
        const std::string t =
            (base / "Int" / "Scenes" / "title.ilb").string();
        if (!title_il.load(t, err)) { *error = err + " (" + t + ")"; return false; }
        const std::string f = (base / kFontIlb).string();
        if (!font_il.load(f, err)) { *error = err + " (" + f + ")"; return false; }
        const std::string c = (base / kCursorsIlb).string();
        if (!cursors_il.load(c, err)) { *error = err + " (" + c + ")"; return false; }

        // The ILB directory ids are what AoW.exe indexes with (`ImageLib.Get`),
        // so resolve assets by id rather than by vector position.
        auto pos_of = [](const aow::IlbFile& ilb, std::uint32_t id) -> int {
            const auto& e = ilb.entries();
            for (std::size_t i = 0; i < e.size(); ++i) {
                if (e[i].index == id) return static_cast<int>(i);
            }
            return -1;
        };
        // A sprite with no mask (class 0x16) carries its background as the
        // palette's entry 0. Key it out by the *most common exact* colour of the
        // sprite's border ring - (0,0) can be an outline pixel, and the ring is
        // small enough to compare pairwise.
        auto set_key = [](Sprite& s) {
            if (s.rgba.empty()) return;
            if (s.rgba[3] == 0) return;  // already masked (classes 0x11/0x12)
            if (s.w < 2 || s.h < 2) return;
            std::vector<std::uint32_t> ring;  // packed 0xRRGGBB
            auto push = [&](std::uint32_t x, std::uint32_t y) {
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(y) * s.w + x) * 4];
                if (p[3] == 0) return;
                ring.push_back((static_cast<std::uint32_t>(p[0]) << 16) |
                               (static_cast<std::uint32_t>(p[1]) << 8) | p[2]);
            };
            for (std::uint32_t x = 0; x < s.w; ++x) { push(x, 0); push(x, s.h - 1); }
            for (std::uint32_t y = 1; y + 1 < s.h; ++y) {
                push(0, y);
                push(s.w - 1, y);
            }
            if (ring.empty()) return;
            std::uint32_t best = 0, bestn = 0;
            for (std::size_t i = 0; i < ring.size(); ++i) {
                std::uint32_t n = 0;
                for (std::size_t j = 0; j < ring.size(); ++j) {
                    if (ring[j] == ring[i]) ++n;
                }
                if (n > bestn) { bestn = n; best = ring[i]; }
            }
            if (bestn * 2 < ring.size()) return;  // no dominant background
            s.key[0] = static_cast<std::uint8_t>((best >> 16) & 0xFF);
            s.key[1] = static_cast<std::uint8_t>((best >> 8) & 0xFF);
            s.key[2] = static_cast<std::uint8_t>(best & 0xFF);
            s.has_key = true;
        };
        auto decode_id = [&](const aow::IlbFile& ilb, std::uint32_t id,
                             Sprite& out) -> bool {
            const int p = pos_of(ilb, id);
            if (p < 0) { err = "no entry with id " + std::to_string(id); return false; }
            std::uint32_t w = 0, h = 0;
            if (!ilb.decode_rgb(static_cast<std::size_t>(p), out.rgba, w, h, err)) {
                return false;
            }
            out.w = w; out.h = h; out.ok = true;
            const aow::IlbEntry& e = ilb.entries()[static_cast<std::size_t>(p)];
            out.base_x = e.base_x;
            out.base_y = e.base_y;
            out.advance = e.width;
            if (e.has_color_key) {
                // `decode_rgb` already dropped the class-0x16 key, so keep the
                // Sprite's key fields in sync with it: `blit_keyed` / `key_to_alpha`
                // then have nothing left to remove, and the reported key is the
                // one the engine's BltColorKey16 uses (not the border heuristic).
                out.key[0] = static_cast<std::uint8_t>(((e.color_key >> 11) & 0x1F) *
                                                       255 / 31);
                out.key[1] = static_cast<std::uint8_t>(((e.color_key >> 5) & 0x3F) *
                                                       255 / 63);
                out.key[2] = static_cast<std::uint8_t>((e.color_key & 0x1F) * 255 /
                                                       31);
                out.has_key = true;
            } else {
                set_key(out);
            }
            return true;
        };

        if (!decode_id(title_il, kImgBackground, background)) {
            *error = "background: " + err; return false;
        }
        if (!decode_id(title_il, kImgLogo, logo)) {
            *error = "logo: " + err; return false;
        }
        for (int i = 0; i < kButtonCount; ++i) {
            for (int s = 0; s < kOrbStride; ++s) {
                const std::uint32_t id =
                    kImgOrbBase + static_cast<std::uint32_t>(i * kOrbStride + s);
                if (!decode_id(title_il, id, orb[i][s])) {
                    *error = "orb frame id " + std::to_string(id) + ": " + err;
                    return false;
                }
                orb_ids[i][s] = id;
            }
        }
        decode_id(title_il, kImgOrbOverlay, orb_overlay);  // optional overlay
        decode_id(cursors_il, cursor_id, cursor);          // pointer (optional)
        if (cursor.ok) {
            // The visible box is the non-keyed part, so anchoring at (0,0) of
            // the *visual* box keeps the hotspot at the arrow's tip.
            cursor.base_x = 0;
            cursor.base_y = 0;
        }
        // The engine's pointer placement: a Cursors.ilb image is painted with its
        // slot *centred* on the mouse, so the decoded visual box belongs at
        // `mouse - slot/2 + base + shift` (see Sprite::anchor_dx). The map
        // window uses it for the arrow too; the menu screens above keep the
        // plain (0,0) anchor it has always used.
        auto set_pointer_offset = [&](std::uint32_t id, Sprite& out) {
            if (!out.ok) return;
            const int p = pos_of(cursors_il, id);
            if (p < 0) return;
            const aow::IlbEntry& e =
                cursors_il.entries()[static_cast<std::size_t>(p)];
            out.anchor_dx = e.base_x + e.xshift -
                            static_cast<std::int32_t>(e.width) / 2;
            out.anchor_dy = e.base_y + e.yshift -
                            static_cast<std::int32_t>(e.height) / 2;
        };
        set_pointer_offset(cursor_id, cursor);
        // Every pointer the map screen can paint, filed by ILB id: the hover
        // sparkle (state 0x101), the crossed swords (0x102), the twin banners
        // (0x103) and the walk / sail / fly pairs with their "blocked" twins
        // (state 0x100). Optional art: a library without an id leaves that slot
        // empty and `game_pointer` falls back to the arrow.
        for (const std::uint32_t id : {kAttackCursorBaseId,
                                       kAttackCursorBaseId + 1,
                                       kHoverCursorBaseId,
                                       kHoverCursorBaseId + 1,
                                       kHoverCursorBaseId + 2,
                                       kHoverCursorBaseId + 3,
                                       kJoinCursorBaseId,
                                       kJoinCursorBaseId + 1,
                                       kJoinCursorBaseId + 2,
                                       kMoveCursorWalkId,
                                       kMoveCursorWalkId + 1,
                                       kMoveCursorSailId,
                                       kMoveCursorSailId + 1,
                                       kMoveCursorFlyId,
                                       kMoveCursorFlyId + 1}) {
            Sprite& s = map_cursor[id];
            if (!decode_id(cursors_il, id, s)) continue;
            set_pointer_offset(id, s);
        }
        // The hexagon selector halves. `AoWWHSSet+0x48` is LinkToIL("GENERAL.ILB")
        // and both halves are drawn at `HXtoHP(hex) + (-8, -0x1e)` with the
        // sprite's own base+shift (ShowFront/BackHexagonSelector), so fold that
        // anchor into the placement offset. They are class 0x16 (no mask), so their
        // background key becomes real transparency: the map canvas is written by
        // `aow::blit_sprite_rgba`, which blends by alpha only, while the engine
        // drops the key with a colour-keyed blit (`GFXE.TSurface.BltColorKey@
        // 0x5500c544` - what `Canvas::blit_keyed` does for the pointer sprites).
        const std::string gp = (base / kGeneralIlb).string();
        if (general_il.load(gp, err)) {
            auto decode_selector = [&](std::uint32_t id, Sprite& out) {
                if (!decode_id(general_il, id, out)) return;
                const int p = pos_of(general_il, id);
                if (p < 0) return;
                const aow::IlbEntry& e =
                    general_il.entries()[static_cast<std::size_t>(p)];
                out.anchor_dx = kHexSelectorAnchorX + e.base_x + e.xshift;
                out.anchor_dy = kHexSelectorAnchorY + e.base_y + e.yshift;
                key_to_alpha(out);
            };
            decode_selector(kHexSelectorFrontId, hex_sel_front);
            decode_selector(kHexSelectorBackId, hex_sel_back);
        }
        {
            const int p = pos_of(title_il, kImgOrbBase);
            orb_slot_w = p >= 0 ? title_il.entries()[static_cast<std::size_t>(p)].width
                                : 82;
        }

        // Bitmap font. FontModule.AoW15WhiteGrey stores each glyph as two
        // layers, wrapped in a TImageNode whose index is the character code
        // (aow::IlbFile exposes it as IlbEntry::index). File each glyph under
        // its code and take the requested layer by name.
        auto decode_pos = [&](const aow::IlbFile& ilb, std::size_t p,
                             Sprite& out) -> bool {
            std::uint32_t w = 0, h = 0;
            if (!ilb.decode_rgb(p, out.rgba, w, h, err)) return false;
            out.w = w; out.h = h; out.ok = true;
            const aow::IlbEntry& e = ilb.entries()[p];
            out.base_x = e.base_x;
            out.base_y = e.base_y;
            out.advance = e.width;
            set_key(out);
            return true;
        };
        // A bitmap-font glyph. The engine places a sprite's *visual box* at
        // (param + base + shift) (see aow/ilb.hpp; TSprite16 uses +0x5c/+0x60),
        // and a font glyph is exactly such a sprite wrapped in a TImageNode.
        // decode_pos folds in the TLibraryImage margins (base) but not the class
        // shift, so on the AoW15WhiteGrey face (class 0x16, whose glyphs are
        // tightly cropped, so base is 0) every glyph would sit on the cell top:
        // the tall glyphs (shift 1) only slightly, the x-height glyphs and
        // descenders (shift 3..4) several pixels high - i.e. the whole face is
        // biased toward the top of the text instead of sitting on its baseline.
        // Folding the shift in restores the per-glyph vertical offset.
        auto decode_glyph = [&](const aow::IlbFile& ilb, std::size_t p,
                                Sprite& out) -> bool {
            if (!decode_pos(ilb, p, out)) return false;
            const aow::IlbEntry& e = ilb.entries()[p];
            out.base_x += e.xshift;
            out.base_y += e.yshift;
            // The class-0x02 bitmap fonts (Age8s / Age10 / Age10s / Age10tb)
            // carry their transparency in the RLE itself (the escape run), so
            // every opaque pixel is real content: the glyph ink (one flat
            // palette colour) plus palette entry 0 (black). The colour key that
            // decode_pos installs is only meaningful for the mask-less class-0x16
            // faces, whose background *is* palette entry 0 - applying it to a
            // 0x02 glyph keys out the ink whenever the glyph's (0,0) pixel
            // happens to be opaque (which is why 'm', 'T', 'y' and 'p' of "Game
            // Type" used to vanish).
            if (e.class_id == 0x02) out.has_key = false;
            return true;
        };
        glyphs.assign(256, Glyph{});
        {
            const std::size_t n = font_il.entries().size();
            const int want = (font_variant < 0 ? 0
                                                : (font_variant >= kFontLayers
                                                       ? kFontLayers - 1
                                                       : font_variant));
            for (std::size_t i = 0; i + (kFontLayers - 1) < n;
                 i += kFontLayers) {
                // The wrapped image records in front of the node share its
                // index; the walker only recovers it for the first of them.
                std::uint32_t code = font_il.entries()[i].index;
                if (code >= 256) code = font_il.entries()[i + 1].index;
                if (code >= 256) continue;
                std::size_t take = n;
                for (std::size_t j = 0; j < kFontLayers; ++j) {
                    if (font_il.entries()[i + j].name == kFontLayerNames[want]) {
                        take = i + j;
                        break;
                    }
                }
                if (take >= n) continue;
                Glyph& g = glyphs[code];
                decode_glyph(font_il, take, g.face);
                g.ok = g.face.ok;
            }
        }

        // The Game Type screen (TMultiWindow). The panels/buttons come from
        // Int\Scenes\TitleM.ILB, addressed by the same DFM entry ids.
        {
            const std::string m = (base / kMenuIlb).string();
            if (!menu_il.load(m, err)) { *error = err + " (" + m + ")"; return false; }
            if (!decode_id(menu_il, kGtBgId, gt_bg)) {
                *error = "gt background: " + err; return false;
            }
            if (!decode_id(menu_il, kGtTopId, gt_top)) {
                *error = "gt top panel: " + err; return false;
            }
            if (!decode_id(menu_il, kGtBottomId, gt_bottom)) {
                *error = "gt bottom panel: " + err; return false;
            }
            // [0] up, [1] lit (hover), [2] down - matching the title orbs'
            // up/lit/down ordering so the same state selector works.
            const std::uint32_t state_ids[kOrbStride] = {
                kGtBtnUpId, kGtBtnLitId, kGtBtnDownId};
            for (int s = 0; s < kOrbStride; ++s) {
                if (!decode_id(menu_il, state_ids[s], gt_btn[s])) {
                    *error = "gt button frame " + std::to_string(state_ids[s]) +
                             ": " + err;
                    return false;
                }
            }
        }

        // "Game Type" uses FontModule.Age8s (Int\Fonts\Age8s.ILB), a single
        // layer of class-0x02 glyphs keyed by character code (idx starts at 33
        // = '!'), unlike Age15WhGr's two font2.BMP/font.bmp layers.
        {
            const std::string f8 = (base / kGtFontIlb).string();
            if (!font8_il.load(f8, err)) {
                *error = err + " (" + f8 + ")"; return false;
            }
            glyphs8.assign(256, Glyph{});
            for (std::size_t i = 0; i < font8_il.entries().size(); ++i) {
                const aow::IlbEntry& e = font8_il.entries()[i];
                if (e.index >= 256) continue;
                if (e.name.size() < 4 || e.name.compare(0, 4, "font") != 0) {
                    continue;
                }
                Glyph& g = glyphs8[e.index];
                if (g.ok) continue;
                decode_glyph(font8_il, i, g.face);
                g.ok = g.face.ok;
            }
        }

        // --captions is parsed before the assets load; only fall back to the
        // built-in labels for the slots it did not fill.
        for (int i = 0; i < kButtonCount; ++i) {
            if (captions[i].empty()) captions[i] = kDefaultCaptions[i];
        }

        // --- the Scenario screen (TScenSetup) -----------------------------
        // The backdrop and the three "SceneBtn" buttons live in TitleM, which
        // is already loaded as `menu_il`.
        if (!decode_id(menu_il, kScenBgId, scen_bg)) {
            *error = "scenario background: " + err; return false;
        }
        for (int b = 0; b < 3; ++b) {
            for (int s = 0; s < kOrbStride; ++s) {
                if (!decode_id(menu_il, kScenBtnIds[b] + s, scen_btn[b][s])) {
                    *error = "scenario button " + std::to_string(kScenBtnIds[b]) +
                             ": " + err; return false;
                }
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (!decode_id(menu_il, kScenChkIds[i], scen_chk[i])) {
                *error = "scenario checkbox: " + err; return false;
            }
        }
        // Frames/textures: GenericF's ScenFr(72..79), DialogFr(32..39) and
        // FrGoldD(113..120); GenericT's ShadowOv(1) and MainFloaterSet(2).
        {
            const std::string gf = (base / kGenericFIlb).string();
            if (!genericf_il.load(gf, err)) { *error = err + " (" + gf + ")"; return false; }
            for (int i = 0; i < 8; ++i) {
                if (!decode_id(genericf_il, kScenFrameBase + i, scen_frame[i])) {
                    *error = "scenario frame: " + err; return false;
                }
                if (!decode_id(genericf_il, kDlgFrameBase + i, scen_dlg_frame[i])) {
                    *error = "dialog frame: " + err; return false;
                }
                decode_id(genericf_il, kGoldFrameBase + i, scen_gold_frame[i]);
                decode_id(genericf_il, kThinFrameBase + i, scen_thin_frame[i]);
            }
            const std::string gt = (base / kGenericTIlb).string();
            if (!generict_il.load(gt, err)) { *error = err + " (" + gt + ")"; return false; }
            decode_id(generict_il, 2, scen_body_tex);          // MainFloaterSet
            // The panel bodies and the list / info-strip textures, exactly as
            // the DFMs name them (GenericT 1 / 21 / 17 / 18).
            decode_id(generict_il, kPanelTexId, scen_panel_tex);      // ShadowOv
            decode_id(generict_il, kBlankTexId, scen_blank_tex);      // blank1
            decode_id(generict_il, kListTexId, scen_list_tex);        // Text2Lb17
            decode_id(generict_il, kInfoTexId, scen_info_tex);        // TextOld1
            decode_id(generict_il, kInfo2TexId, scen_info2_tex);      // TextOld2
            {
                const std::string mn = (base / "Int" / "Menus.ILB").string();
                if (menus_il.load(mn, err)) {
                    decode_id(menus_il, kListSelTexId, scen_list_sel_tex);
                }
            }
            // GenericF's ScenFr (64..71), the DurFr/LFr group frames.
            for (int i = 0; i < 8; ++i) {
                decode_id(genericf_il, kScenFrBase + i, scen_scenfr[i]);
            }
            // The three bottom buttons: TitleM ScenRBut at 32/35/38 (up/down/lit),
            // each a 3-piece button.
            for (int st = 0; st < 3; ++st) {
                for (int p = 0; p < 3; ++p) {
                    if (!decode_id(menu_il, kMainBtnArt[st][p],
                                   scen_main_btn[st][p])) {
                        *error = "scenario main button " +
                                 std::to_string(kMainBtnArt[st][p]) + ": " + err;
                        return false;
                    }
                }
            }
            // GenericS: the ScenVSB combo/scrollbar pieces and the ScenSlid
            // horizontal sliders.
            const std::string gs = (base / kGenericSIlb).string();
            if (!generics_il.load(gs, err)) { *error = err + " (" + gs + ")"; return false; }
            decode_id(generics_il, kVsbDecUp, scen_vsb[0]);
            decode_id(generics_il, kVsbDecDown, scen_vsb[1]);
            decode_id(generics_il, kVsbIncUp, scen_vsb[2]);
            decode_id(generics_il, kVsbIncDown, scen_vsb[3]);
            decode_id(generics_il, kVsbTrack, scen_vsb[4]);
            // The file dialog's list scrollbar is NSBar, which names GenericS'
            // NGVsbLight set: dec-up 94 / inc-up 96 (the arrow caps), the 17x4 /
            // 17x64 / 17x4 thumb (98..100) and the 17x79 track (101).
            decode_id(generics_il, 94, scen_ngvsb[0]);
            decode_id(generics_il, 96, scen_ngvsb[1]);
            decode_id(generics_il, 99, scen_ngvsb[2]);
            decode_id(generics_il, 98, scen_ngvsb[3]);
            decode_id(generics_il, 101, scen_ngvsb[4]);
            for (int i = 0; i < 6; ++i) {
                decode_id(generics_il, kSliderBack + i, scen_slider[i]);
            }
            // The Settings panel's turn-mode combo (PlayModeCB) names
            // ILFrame/ILEdit = IntGfxMod.TitleScreen: TitleM's ScenEBut 26/27/28
            // (10/162/18 x 18), whose grey body carries the dark-green funnel in
            // its right cap - exactly the field the shipped screen shows.
            for (int i = 0; i < 3; ++i) {
                decode_id(menu_il, static_cast<std::uint32_t>(26 + i),
                          scen_scenebut[i]);
                // ScenCol.BMP 49..54 (header buttons) and ScenEd.BMP 23..25
                // (ChatEdit). Both are the same 3-piece skins the DFM names.
                decode_id(menu_il, kScenColArt[i], scen_col[i]);
                decode_id(menu_il, kScenEdArt[i], scen_chated[i]);
            }
            // GenericE: the edit / combo field skins. EditBLight (3..5) is the
            // pale field, ComboBLight (6..8) is the same field with the
            // drop-down arrow baked into its 21px right cap.
            const std::string ge = (base / kGenericEIlb).string();
            if (generice_il.load(ge, err)) {
                // The file dialog's edit / combo skin is the DARK "NG" face the
                // DFM names (PathEdit / FileName carry ILIndex = 15 -> NGEdit
                // 15..17; the combos use NGCmb 21..23, whose 19 px right cap is
                // the gold drop-down funnel). EditBLight / ComboBLight (3..8)
                // are the pale variant and are not what the shipped screen
                // shows.
                for (int i = 0; i < 3; ++i) {
                    decode_id(generice_il, static_cast<std::uint32_t>(15 + i),
                              scen_edit[i]);
                    decode_id(generice_il, static_cast<std::uint32_t>(21 + i),
                              scen_combo[i]);
                }
            }
            // The horizontal celtic knot band (BCeltL's ILLine = IntGfxMod.CeltDeco
            // at 55, a 7 / 100 / 7 x 27 3-piece) that runs above the buttons and
            // below the title bar.
            {
                const std::string cd = (base / kCeltDecoIlb).string();
                if (celldeco_il.load(cd, err)) {
                    // BCeltL is a frHorizLine frame: ILLine = CeltDeco id 55,
                    // the 7 / 100 / 7 x 27 three-piece knot band (ids 55/56/57).
                    for (int i = 0; i < 3; ++i) {
                        decode_id(celldeco_il, static_cast<std::uint32_t>(55 + i),
                                  scen_celt[i]);
                    }
                    decode_id(celldeco_il, 3, scen_celt_knot);
                }
            }
            // The green celtic dialog buttons (GenericB ButRCelt2, 3x3).
            const std::string gb = (base / kGenericBIlb).string();
            if (!genericb_il.load(gb, err)) { *error = err + " (" + gb + ")"; return false; }
            for (int st = 0; st < 3; ++st) {
                for (int p = 0; p < 3; ++p) {
                    decode_id(genericb_il, kCeltBtnBase + st * 3 + p,
                              scen_dlg_btn[st][p]);
                }
            }
            // FontModule.Age10 - the dialog's face (TFileDlgWindow labels,
            // list box and buttons all name it).
            const std::string fa = (base / kFontAge10Ilb).string();
            glyphs10b.assign(256, Glyph{});
            if (font10b_il.load(fa, err)) {
                for (std::size_t i = 0; i < font10b_il.entries().size(); ++i) {
                    const aow::IlbEntry& e = font10b_il.entries()[i];
                    if (e.index >= 256) continue;
                    if (e.name.size() < 4 || e.name.compare(0, 4, "font") != 0) {
                        continue;
                    }
                    Glyph& g = glyphs10b[e.index];
                    if (g.ok) continue;
                    decode_glyph(font10b_il, i, g.face);
                    g.ok = g.face.ok;
                }
            }
            // The dialog's little title icon (WinTIcon id 8).
            const std::string ti = (base / kWinTIconIlb).string();
            if (titleicon_il.load(ti, err)) {
                decode_id(titleicon_il, 8, scen_dlg_title_icon);
            }
            // The dialog's close box (GenericI 0/1/2 MainHeaderBut, 15x16).
            {
                const std::string gi = (base / "Int" / "GenericI.ILB").string();
                if (generici_il.load(gi, err)) {
                    for (int i = 0; i < 3; ++i) {
                        decode_id(generici_il, kScenCloseArt[i], scen_close[i]);
                    }
                    // UpDirBtn: ILIcon GenericI 56 (up) / 57 (down) / 58 (lit).
                    for (int i = 0; i < 3; ++i) {
                        decode_id(generici_il,
                                  static_cast<std::uint32_t>(56 + i),
                                  scen_updir[i]);
                    }
                }
            }
            // The size / towns / players info-strip icons. The engine indexes
            // the TImageNode ids the DFM names (UnitIcons 7 / 8, TitleScreen 10);
            // each node holds two layers - a soft glow followed by the art - which
            // TImageNode.Show blits in order. Reading the entries by id (not by
            // vector position) is what yields the globe/castle/leader face the
            // shipped screen shows.
            auto decode_layers = [&](const aow::IlbFile& ilb, std::uint32_t id,
                                     LayeredSprite& out) {
                out.count = 0;
                for (std::size_t i = 0; i < ilb.entries().size() &&
                                       out.count < 4; ++i) {
                    const aow::IlbEntry& e = ilb.entries()[i];
                    if (e.index != id) continue;
                    Sprite& s = out.layer[out.count];
                    if (decode_pos(ilb, i, s)) {
                        // TSprite16's visual box lands at anchor + base + shift
                        // (aow/ilb.hpp); each TImageNode layer has its own shift.
                        s.base_x += e.xshift;
                        s.base_y += e.yshift;
                        ++out.count;
                    }
                }
            };
            const std::string ui = (base / kUnitIconsIlb).string();
            if (uniticons_il.load(ui, err)) {
                decode_layers(uniticons_il, 7, scen_icon_size);   // globe + glow
                decode_layers(uniticons_il, 8, scen_icon_towns);  // castle + glow
            }
            decode_layers(menu_il, 10, scen_icon_players);        // leader face
            // The race crest shields (Int\ShieldsM.ILB, 40x53): ids 0..11 are
            // the twelve races the race panels tag (R1Pnl Tag 0, R5Pnl Tag 4, ...).
            // RacePnlDrawBack blits the tagged crest at the panel's
            // (WinLeft + 5, WinTop + 5); a class-0x16 sprite anchors its visual
            // box at `base + class shift`, so fold that shift in.
            const std::string sh = (base / kShieldsIlb).string();
            if (shields_il.load(sh, err)) {
                for (int i = 0; i < 12; ++i) {
                    decode_id(shields_il, static_cast<std::uint32_t>(i),
                              scen_shield[i]);
                    const int p =
                        pos_of(shields_il, static_cast<std::uint32_t>(i));
                    if (p >= 0 && scen_shield[i].ok) {
                        const aow::IlbEntry& e =
                            shields_il.entries()[static_cast<std::size_t>(p)];
                        scen_shield[i].base_x += e.xshift;
                        scen_shield[i].base_y += e.yshift;
                    }
                }
            }
        }
        // Race/player icons: IntGfxMod.PlyrIcns keeps one TImageNode per
        // TPlayerType (ids 0..5), each stored as two entries - the aura/glow
        // layer and the glyph (the skull / goblin / crown face). TImageNode.Show
        // blits them in file order, so keep both layers per id.
        {
            const std::string pi = (base / kPlyrIcnsIlb).string();
            if (!plyr_il.load(pi, err)) { *error = err + " (" + pi + ")"; return false; }
            for (std::uint32_t id = 0; id < 6; ++id) {
                LayeredSprite& out = scen_plyr[id];
                out.count = 0;
                for (std::size_t i = 0; i < plyr_il.entries().size() &&
                                       out.count < 4; ++i) {
                    const aow::IlbEntry& e = plyr_il.entries()[i];
                    if (e.index != id) continue;
                    Sprite& s = out.layer[out.count];
                    if (decode_pos(plyr_il, i, s)) {
                        // TSprite16's visual box lands at anchor + base + shift.
                        s.base_x += e.xshift;
                        s.base_y += e.yshift;
                        ++out.count;
                    }
                }
            }
        }
        // FontModule.Age10s - single-layer glyphs keyed by character code.
        {
            const std::string f10 = (base / kFont10Ilb).string();
            if (!font10_il.load(f10, err)) { *error = err + " (" + f10 + ")"; return false; }
            glyphs10.assign(256, Glyph{});
            for (std::size_t i = 0; i < font10_il.entries().size(); ++i) {
                const aow::IlbEntry& e = font10_il.entries()[i];
                if (e.index >= 256) continue;
                if (e.name.size() < 4 || e.name.compare(0, 4, "font") != 0) {
                    continue;
                }
                Glyph& g = glyphs10[e.index];
                if (g.ok) continue;
                decode_glyph(font10_il, i, g.face);
                g.ok = g.face.ok;
            }
        }
        // The map list: Scenario\Custom\*.HSM (sorted), as the TFileDlgWindow
        // lists them.
        {
            std::error_code ec;
            const fs::path dir = base / "Scenario" / "Custom";
            for (fs::directory_iterator it(dir, ec), end; it != end;
                 it.increment(ec)) {
                if (!it->is_regular_file()) continue;
                const std::string name = it->path().filename().string();
                const std::string ext = it->path().extension().string();
                std::string low = ext;
                for (char& c : low) c = static_cast<char>(std::tolower(
                    static_cast<unsigned char>(c)));
                if (low == ".hsm") scen_maps.push_back(name);
            }
            std::sort(scen_maps.begin(), scen_maps.end());
            // Default the info strip to the reviewed map (Barondir Bay.HSM) when
            // present, but leave the list itself unselected: the picker opens
            // with no row highlighted, exactly as the shipped screen shows.
            int def = 0;
            for (std::size_t i = 0; i < scen_maps.size(); ++i) {
                if (scen_maps[i] == "Barondir Bay.HSM") def = static_cast<int>(i);
            }
            scen_map_sel = -1;
            if (!scen_maps.empty()) {
                scen_map_name = scen_maps[static_cast<std::size_t>(def)];
            }
        }

        // --- the in-game screen (TWorldMap + its two floaters) -------------
        // Every sprite is the entry its form DFM names; failures are non-fatal
        // (the renderer simply skips a sprite that did not resolve).
        {
            const std::string tb = (base / "Int" / "TBWin.ILB").string();
            if (tbwin_il.load(tb, err)) {
                for (int i = 0; i < 3; ++i) decode_id(tbwin_il, kTbBarArt[i], tb_bar[i]);
                for (int s = 0; s < 2; ++s) {
                    for (int p = 0; p < 3; ++p) {
                        decode_id(tbwin_il, kTbStartArt[s][p], tb_start[s][p]);
                    }
                }
                for (int s = 0; s < 3; ++s) {
                    for (int p = 0; p < 3; ++p) {
                        decode_id(tbwin_il, kTbTabArt[s][p], tb_tab[s][p]);
                    }
                }
                // NextButton: one picture per state (idle 3 / held 4).
                decode_id(tbwin_il, kTbNextUpArt, tb_next_idle);
                decode_id(tbwin_il, kTbNextDownArt, tb_next_pressed);
                decode_id(tbwin_il, kTbGoldArt, tb_gold);
                decode_id(tbwin_il, kTbManaArt, tb_mana);
                decode_id(tbwin_il, kTbTimeBodyArt, tb_time_body);
                for (int i = 0; i < 3; ++i) {
                    decode_id(tbwin_il, kTbPindArt[i], tb_pind[i]);
                }
            }
            const std::string lb = (base / "Int" / "LBIcon.ILB").string();
            if (lbicon_il.load(lb, err)) decode_id(lbicon_il, 2, tb_time_icon);
            // The TurnButton's whole animation: 102 frames, indexed by the class
            // exactly as the library stores them (see kTbTurnAnimStart).
            const std::string tt = (base / "Int" / "TBTurnBt.ILB").string();
            if (turnbt_il.load(tt, err)) {
                tb_turn_frames.assign(kTbTurnFrameCount, Sprite());
                for (int f = 0; f < kTbTurnFrameCount; ++f) {
                    decode_id(turnbt_il, static_cast<std::uint32_t>(f),
                              tb_turn_frames[static_cast<std::size_t>(f)]);
                }
            }
            const std::string sw = (base / "Int" / "ScanWin.ILB").string();
            if (scanwin_il.load(sw, err)) {
                for (int i = 0; i < 8; ++i) {
                    decode_id(scanwin_il, kScanFrameBase + i, scan_frame[i]);
                }
                for (int s = 0; s < 2; ++s) {
                    for (int p = 0; p < 3; ++p) {
                        decode_id(scanwin_il, kScanTabArt[s][p], scan_tab[s][p]);
                    }
                }
                for (int i = 0; i < 3; ++i) decode_id(scanwin_il, kScanChkArt[i], scan_chk[i]);
            }
            const std::string gi = (base / "Int" / "GenericI.ILB").string();
            if (generici_il.load(gi, err)) {
                for (int i = 0; i < 3; ++i) {
                    decode_id(generici_il, kScanMinArt[i], scan_min[i]);
                    decode_id(generici_il, kScanHelpArt[i], scan_help[i]);
                }
                // The Scenario screen's 12x12 checkboxes (NGCBox.BMP 47 off /
                // 48 checked) - the art the shipped capture shows.
                decode_id(generici_il, kNgcBoxOff, scen_ngcbox[0]);
                decode_id(generici_il, kNgcBoxOn, scen_ngcbox[1]);
            }
            const std::string cw = (base / "Int" / "Scenes" / "CityWin.ILB").string();
            if (citywin_il.load(cw, err)) decode_id(citywin_il, 5, event_shield_tex);
            const std::string ei = (base / "Int" / "EventIcn.ILB").string();
            if (eventicn_il.load(ei, err)) decode_id(eventicn_il, 0, event_icon);
            // The floater 9-slice (GenericF 0..7 MainFloaterSetR) and its body
            // texture (GenericT 2 MainFloaterSet); the event cells' FrGoldD
            // (GenericF 113..120) and the banner body texture (GenericT 17).
            if (genericf_il.entries().empty()) {
                const std::string gf = (base / kGenericFIlb).string();
                genericf_il.load(gf, err);
            }
            if (generict_il.entries().empty()) {
                const std::string gt = (base / kGenericTIlb).string();
                generict_il.load(gt, err);
            }
            for (int i = 0; i < 8; ++i) {
                decode_id(genericf_il, kFloatFrameBase + i, float_frame[i]);
                decode_id(genericf_il, kMapFrameBase + i, map_frame[i]);
                decode_id(genericf_il, kEventPnlFrameBase + i, event_pnl_frame[i]);
                decode_id(genericf_il, kEventCellFrameBase + i, event_cell_frame[i]);
            }
            decode_id(generict_il, 2, float_body);
            decode_id(generict_il, 17, event_body_tex);
            decode_id(titleicon_il, 0, scan_title_icon);
            decode_id(titleicon_il, 1, event_title_icon);
            decode_id(shields_il, 8, event_shield);
            // --- the Party window (TControlWin) --------------------------------
            // `IntGfxMod.Units` = Int\Units.ILB holds every piece the window
            // draws: the empty cell (0..7 + the 8 hatch), the unit cell (12..19),
            // the "leave behind" cell (26..33) and the pill's two states
            // (20..22 up / 23..25 down, each a 3-piece).
            {
                const std::string up = (base / kUnitsIlb).string();
                if (units_il.load(up, err)) {
                    for (int i = 0; i < 8; ++i) {
                        decode_id(units_il, kPartyBoxBase + i, party_box[i]);
                        decode_id(units_il, kPartyBoxLeftBase + i,
                                  party_box_left[i]);
                        decode_id(units_il, kPartyEmptyBase + i, party_empty[i]);
                    }
                    decode_id(units_il, kPartyEmptyTex, party_empty_tex);
                    // The foot panels' (and the empty slot rows') olive-grey
                    // 3-piece - the same rounded shape as the pill, in the
                    // second colourway (PartyFr ids 9/10/11).
                    for (int p = 0; p < 3; ++p) {
                        decode_id(units_il, 9 + p, party_foot[p]);
                    }
                    for (int p = 0; p < 3; ++p) {
                        decode_id(units_il, kPartyPillUp + p, party_pill[0][p]);
                        decode_id(units_il, kPartyPillDown + p,
                                  party_pill[1][p]);
                    }
                }
                // `IntGfxMod.MoveIcons` = Int\IconMove.ILB, the twenty
                // move-type icons (0 boot, 2 tree, 4 pickaxe, 6 wave, 8 boot+wave,
                // ... 18 wings; odd ids are their dimmed twins).
                const std::string mi = (base / kIconMoveIlb).string();
                if (iconmove_il.load(mi, err)) {
                    const std::size_t n = iconmove_il.entries().size();
                    move_icons.resize(n);
                    for (std::size_t k = 0; k < n; ++k) {
                        std::uint32_t w = 0, h = 0;
                        Sprite s;
                        if (iconmove_il.decode_rgb(k, s.rgba, w, h, err)) {
                            s.w = w; s.h = h; s.ok = true;
                            const aow::IlbEntry& e = iconmove_il.entries()[k];
                            s.base_x = e.base_x; s.base_y = e.base_y;
                            move_icons[k] = std::move(s);
                        }
                    }
                }
                // `IntGfxMod.UnitBGs` = Int\UnitBgs.ILB: the eight 100x100
                // washes the unit cell's PreDraw draws under the unit image.
                const std::string ub = (base / kUnitBgsIlb).string();
                if (unitbgs_il.load(ub, err)) {
                    for (int i = 0; i < 8; ++i) {
                        decode_id(unitbgs_il, static_cast<std::uint32_t>(i),
                                  party_unitbg[i]);
                    }
                }
                // The title-bar buttons (GenericI) and the foot buttons
                // (GenericB 76/82/79) - the same libraries the scanner uses.
                for (int i = 0; i < 3; ++i) {
                    decode_id(generici_il, kPartyCloseArt[i], party_close[i]);
                    decode_id(generici_il, kPartyMinArt[i], party_min[i]);
                    decode_id(generici_il, kPartyCenterArt[i], party_center[i]);
                    decode_id(generici_il, kPartyHelpArt[i], party_help[i]);
                }
                const std::uint32_t btn[3] = {kPartyBtnUp, kPartyBtnLit,
                                              kPartyBtnDown};
                for (int st = 0; st < 3; ++st) {
                    for (int p = 0; p < 3; ++p) {
                        decode_id(genericb_il, btn[st] + p, party_btn[st][p]);
                    }
                }
                // CTitleIcon: `IntGfxMod.TitleIcons` (Int\WinTIcon.ilb) id 4 -
                // the small shield in the title bar.
                decode_id(titleicon_il, 4, party_icon[0]);
                decode_id(generict_il, kPartyPanelTex, party_panel_tex);
                // Unit{n}Gge's art: GenericT 4 (the track) and 5 (the fill).
                decode_id(generict_il, 4, party_gauge[0]);
                decode_id(generict_il, 5, party_gauge[1]);
            }
            // The Events banner's race crests - ShieldsM ids 13..25 (RShield's
            // `CurrentImage = race + 13`) - and the FACERES.PFS face list the
            // leader's portrait is resolved through.
            // A crest sprite is class 0x12 (TTransparentRLESprite16): its pixels
            // are masked by the RLE and the engine anchors its visual box at
            // `base + class shift`, which the blit below folds into the Sprite.
            auto fold_shift = [&](const aow::IlbFile& ilb, std::uint32_t id,
                                  Sprite& s) {
                if (!s.ok) return;
                const int p = pos_of(ilb, id);
                if (p < 0) return;
                const aow::IlbEntry& e =
                    ilb.entries()[static_cast<std::size_t>(p)];
                s.base_x += e.xshift;
                s.base_y += e.yshift;
            };
            for (int i = 0; i < kEventShieldCount; ++i) {
                const std::uint32_t id =
                    kEventShieldBase + static_cast<std::uint32_t>(i);
                decode_id(shields_il, id, event_shield_race[i]);
                fold_shift(shields_il, id, event_shield_race[i]);
            }
            {
                aow::PfsFile fpfs;
                const std::string fp = (base / kFaceResPfs).string();
                std::string ferr;
                if (fpfs.load(fp, ferr)) game_face_res = aow::load_faces(fpfs);
            }
            // The bar's fonts: FontModule.Age8 and FontModule.TBarNumber.
            auto load_face = [&](const char* rel, aow::IlbFile& ilb,
                                 std::vector<Glyph>& gl) {
                gl.assign(256, Glyph{});
                const std::string p = (base / rel).string();
                if (!ilb.load(p, err)) return;
                for (std::size_t i = 0; i < ilb.entries().size(); ++i) {
                    const aow::IlbEntry& e = ilb.entries()[i];
                    if (e.index >= 256) continue;
                    if (e.name.size() < 4 || e.name.compare(0, 4, "font") != 0) {
                        continue;
                    }
                    Glyph& g = gl[e.index];
                    if (g.ok) continue;
                    decode_glyph(ilb, i, g.face);
                    g.ok = g.face.ok;
                }
            };
            load_face(kFontAge8Ilb, font8b_il, glyphs8b);
            load_face(kFontTbNumIlb, fonttb_il, glyphsTb);
            // --- the Structure window's art (TIWINDOW.dfm) -------------------
            decode_id(generict_il, 0, iwin_bg);         // TextBackGround
            decode_id(generict_il, 2, iwin_body);       // MainFloaterSet
            for (int i = 0; i < 8; ++i) {
                decode_id(genericf_il, static_cast<std::uint32_t>(i), iwin_frame[i]);
                decode_id(genericf_il, kGoldFrameBase + i, iwin_gold_frame[i]);
            }
            decode_id(titleicon_il, 2, iwin_title_icon);
            for (int i = 0; i < 3; ++i) {
                decode_id(generici_il, kPartyCloseArt[i], iwin_close[i]);
                decode_id(generici_il, kPartyMinArt[i], iwin_min[i]);
                decode_id(generici_il, kPartyCenterArt[i], iwin_center[i]);
                decode_id(generici_il, kPartyHelpArt[i], iwin_help[i]);
            }
            // RazeBtn: ILIndexUp/Down/Lit = 53/54/55 (TIWINDOW.dfm).
            decode_id(generici_il, 53, iwin_raze_btn[0]);
            decode_id(generici_il, 55, iwin_raze_btn[1]);
            decode_id(generici_il, 54, iwin_raze_btn[2]);
            for (int st = 0; st < 3; ++st) {
                const std::uint32_t first =
                    st == 0 ? 19u : (st == 1 ? 25u : 22u);
                for (int p = 0; p < 3; ++p) {
                    decode_id(genericb_il, first + p, iwin_enter[st][p]);
                }
            }
            for (int i = 0; i < 8; ++i) {
                decode_id(celldeco_il, static_cast<std::uint32_t>(30 + i),
                          iwin_celt_main[i]);
            }
            decode_id(celldeco_il, 3, iwin_celt_knot);
            decode_id(celldeco_il, 8, iwin_celt_arch);
            
            decode_id(celldeco_il, 9, iwin_celt_side);
            decode_id(celldeco_il, 17, iwin_celt_line_a);
            decode_id(celldeco_il, 22, iwin_celt_line_b);
            decode_id(celldeco_il, 74, iwin_raze_bg);
            if (lbicon_il.entries().empty()) {
                const std::string lb = (base / "Int" / "LBIcon.ILB").string();
                lbicon_il.load(lb, err);
            }
            decode_id(lbicon_il, 31, iwin_gold_icon);
            decode_id(lbicon_il, 33, iwin_mana_icon);
            {
                const std::string pw = (base / "Int" / "ProdWin.ILB").string();
                if (prodwin_il.load(pw, err)) {
                    decode_id(prodwin_il, 4, iwin_prod_orb);
                }
                const std::string rw = (base / "Int" / "RelWin.ILB").string();
                if (relwin_il.load(rw, err)) {
                    decode_id(relwin_il, 11, iwin_relation);
                }
            }
        }
        return true;

    }

    // The song list the in-game screen plays, exactly as AoW.exe FUN_00454d2c
    // (0x00454d2c) builds it - see aow::scenario_playlist (engine/playlist.cpp):
    // the map's tag-0x3c `Sound.TSonglist` wins when it has entries; otherwise
    // the tag-0x1d `AoWE.TStartupSongList` ("original") songs are copied,
    // skipping any whose file is missing (the engine filters on the song's
    // "FileNotFound" title); if neither yields a song the default
    // "Songs\Heartwoo.it" is used. The map stores names relative to the game
    // root ("SONGS\CLOUDS.IT"), so each is resolved against `root_dir` here.
    std::vector<std::string> game_playlist(const aow::Map& m) const {
        return aow::scenario_playlist(m, root_dir);
    }

    // Loads the .HSM the in-game screen shows and renders it (through the
    // shared mapviz renderer) into `game_px`. The main map view and the scanner
    // both read that one render, so the screen is cheap to redraw. The
    // resource table (Release.hss) is best-effort: without it the renderer
    // falls back to its per-terrain flat diamonds.
    bool load_game_map(const std::string& map_path, std::string* error) {
        std::string err;
        aow::Map& m = game_state_map;
        if (!aow::load_map(map_path, m, err)) { *error = err; return false; }
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= m.levels.size()) {
            *error = "level out of range";
            return false;
        }
        std::vector<aow::MapResource>& res = game_res;
        res.clear();
        const std::string hss =
            (fs::path(root_dir) / "Release" / "Release.hss").string();
        aow::load_map_resources(hss, res, err);
        aow::MapVizOptions opt;
        opt.level = game_level;
        opt.objects = true;
        // The engine draws the world map 1:1 - one hexagon sprite per hex at the
        // grid's own 32 px pitch (HSEngine.HXtoHP), with no zoom. So the render
        // must NOT be downscaled: `max_dim 0` keeps the buffer at the level's
        // native pixel size and the camera then crops it, which is exactly what
        // THSMapViewer.ShowScene does with the map's static surface.
        opt.max_dim = 0;
        // The level renders in two layers (see mv_render_layers). The animated
        // building sequences are left out here and composited per map frame by
        // mv_draw_structure_anim (docs/17_BUILDING_ANIM.md), and so are the
        // armies: they are the LAST world layer (docs/13_UNITS_ON_MAP.md), so a
        // unit is never covered by a building's animated sprite or the water.
        opt.no_animated_overlays = true;
        opt.no_units = true;
        if (!mv_render_layers(opt)) {
            *error = "level render failed";
            return false;
        }
        // The level's size in hexes - what the camera clamps (GetMaxXhp /
        // GetMaxYhp) and what the scanner counts are derived from.
        const aow::MapLevel& lv = m.levels[static_cast<std::size_t>(game_level)];
        map_w = lv.width;
        map_h = lv.height;
        game_level_count = static_cast<int>(m.levels.size());
        game_map = map_path;
        game_map_name = fs::path(map_path).filename().string();
        game_songs = game_playlist(m);
        mv_load();
        mv_reset();
        return true;
    }

    // ---------------------------------------------------------------------
    // World-map unit movement (docs/15_MOVEMENT.md).
    //
    // The engine's `TAoWHSMapMouse.Down` (AoWEPACK 0x5577448c) resolves the hex
    // under the cursor, `TAoWHSMap.Select` picks the army standing there (mine
    // selects, anything else clears) and `TSelectedArmy.CalculateMovePath` builds
    // the trail to the hovered hex; a click on a hex the army can reach hands the
    // move to `TArmy.MoveArmy`, which relocates the `TArmyHS` field object. The
    // replica reuses the movement engine the viewer already drives (`aow::
    // army_move` / `aow::find_move_path`) plus the map's own `GameState` and
    // `UnitCatalog` for the army's move points.
    // ---------------------------------------------------------------------

    // Head-less micro-benchmark for the hover refresh (`--game-bench N`). A mouse
    // move over the map drives exactly this loop - retarget the trail, then
    // composite the overlay into the canvas the view blits - so timing its two
    // halves separately is what tells you where an interactive stall comes from.
    void game_bench_hover(int n) {
        game_center_leader();
        if (mv_sel_x < 0) return;
        const int hx = mv_sel_x + 2, hy = mv_sel_y - 2;
        double t0 = bench_now_ms();
        for (int i = 0; i < n; ++i) {
            mv_update_path(hx, hy - (i & 1));
        }
        const double path_ms = bench_now_ms() - t0;
        mv_update_path(hx, hy);
        t0 = bench_now_ms();
        for (int i = 0; i < n; ++i) {
            mv_refresh_draw();
        }
        const double draw_ms = bench_now_ms() - t0;
        // The other interactive hitch: a move re-renders the whole level.
        t0 = bench_now_ms();
        mv_render_level();
        const double relvl_ms = bench_now_ms() - t0;
        // And the per-frame cost: the 30 fps timer composes the whole screen
        // canvas (the map view, the bar, both floaters, the scaled minimap).
        const int frames = 30;
        long px = 0;
        t0 = bench_now_ms();
        for (int i = 0; i < frames; ++i) {
            const Canvas cv = render(-1);
            px += cv.width();
        }
        const double frame_ms = (bench_now_ms() - t0) / frames;
        std::printf("game_bench n=%d path=%.2fms/per draw=%.2fms/per "
                    "render=%.2fms frame=%.2fms canvas=%ux%u px=%ld\n",
                    n, path_ms / n, draw_ms / n, relvl_ms, frame_ms, game_pw,
                    game_ph, px);
        std::fflush(stdout);
    }

    // The port's stand-in for the engine's message box (`TMessageBox` is not
    // recovered): the feedback for a refused action is kept and echoed to stdout,
    // which is what the head-less smoke tests read. `game_status` is also the
    // place a future on-screen message strip would read from.
    void game_say(const std::string& msg) {
        game_status = msg;
        std::printf("game_msg=%s\n", msg.c_str());
        std::fflush(stdout);
    }

    // Clears the selection, the hover path and the composited canvas.
    void mv_reset() {
        mv_selected = false;
        mv_sel_x = mv_sel_y = -1;
        mv_army = aow::ArmyMove();
        mv_path = aow::MovePath();
        mv_have_path = false;
        mv_anim = false;
        mv_walk = aow::WalkAnimation();
        mv_hide_level = mv_hide_x = mv_hide_y = -1;
        mv_merge_pending = false;
        mv_merge_x = mv_merge_y = -1;
        mv_merge_stay = 0;
        mv_merge_moving = aow::MapHs();
        game_status.clear();
        game_draw.clear();   // the next refresh recopies the whole canvas
        mv_dirty = false;
        // Closing the party window deselects the party (`CloseClick` ->
        // `TArmy.Unselect`), so the floater goes with it.
        game_show_party = false;
        party_sel = 0;
        party_focus = -1;
        party_hover = party_pressed = -1;
        mv_refresh_draw();
    }

    // Loads the movement data for the current map/level: the startup
    // MovePointTables, the roster (which army belongs to whom), the unit
    // catalogue (the units' move points) and every hex's terrain/overlay.
    // The player whose turn/screen this is (the seat the roster marked Human;
    // 0xff when the map has none). `TPlayerList.GetPlayers(AoWHSMap+0x140,
    // AoWHSMap+0xa5)` is the engine's own lookup for the seated player.
    const aow::Player* game_seat() const {
        for (const aow::Player& p : mv_gs.players) {
            if (p.index == mv_human) return &p;
        }
        return nullptr;
    }

    // A FACERES.PFS `image_lib` is an engine library name ("FACES\L_FACES.ILB");
    // the shipped files live under <root>\Images\<folder>\<name>.ILB. Resolves
    // the name to an existing file (empty when none does).
    std::string face_lib_path(const std::string& lib) const {
        std::string rel = lib;
        for (char& c : rel) {
            if (c == '\\') c = '/';
        }
        const fs::path root(root_dir);
        const fs::path p(rel);
        const fs::path cands[3] = {root / "Images" / p, root / p,
                                   root / "Images" / p.filename()};
        std::error_code ec;
        for (const fs::path& c : cands) {
            if (fs::exists(c, ec)) return c.string();
        }
        return std::string();
    }

    // Resolves a leader's face index (THero+0x3c, an FACERES.PFS record tag)
    // through the face list and its image library, exactly like
    // `TPlayer.GetFace` (0x5575235c) -> `TFaceResourceList.GetFaceResource`
    // (0x5574d608) -> `TFaceResource.GetFaceImage` (0x5574d52c): the record's
    // tag is the looked-up index, its `image_lib`/`face_index` the library and
    // image to draw. Returns false (leaving `game_face` empty) when the map's
    // index names no face.
    bool game_load_face(std::int32_t face_index) {
        game_face_lib.clear();
        game_face_index = -1;
        for (const aow::FaceResource& fr : game_face_res) {
            if (static_cast<std::int32_t>(fr.tag) != face_index) continue;
            const std::string path = face_lib_path(fr.image_lib);
            if (path.empty()) return false;
            if (!game_face.ok || game_face_il_name != path) {
                std::string lerr;
                if (!game_face_il.load(path, lerr)) return false;
                game_face_il_name = path;
            }
            // The library is indexed by *id* (`TCustomImageLibrary.Get`), which
            // is the record's own face index (the FACERES tag 6).
            const std::vector<aow::IlbEntry>& ents = game_face_il.entries();
            for (std::size_t i = 0; i < ents.size(); ++i) {
                if (ents[i].index != fr.face_index) continue;
                std::uint32_t w = 0, h = 0;
                std::string derr;
                if (!game_face_il.decode_rgb(i, game_face.rgba, w, h, derr)) {
                    return false;
                }
                game_face.w = w;
                game_face.h = h;
                game_face.ok = true;
                game_face_lib = fr.image_lib;
                game_face_index = static_cast<int>(fr.tag);
                // The leader faces are class 0x10 pictures (no colour key), so
                // every pixel is opaque - the portrait fills its cell.
                game_face.base_x = 0;
                game_face.base_y = 0;
                game_face.has_key = false;
                return true;
            }
            return false;
        }
        return false;
    }

    // The Events banner's live content, read from the loaded map like the
    // engine's own floater update (AoW.exe FUN_00423170) and its FaceFr OnDraw
    // (FUN_00423770):
    //   * `Player` = TPlayer.GetName (0x55752104) - the leader's name;
    //   * `Race`   = RaceNamesPluralRStr[player's race] (the plural race name);
    //   * the portrait = TPlayer.GetFace (0x5575235c);
    //   * RShield  = ShieldsM `race + 13`.
    // Called whenever the seated player changes (a map is loaded, or the setup
    // control's race -> player-type assignment moved the Human).
    void game_resolve_banner() {
        game_leader.clear();
        game_race.clear();
        game_face = Sprite();
        game_shield_index = -1;   // no seat -> the DFM's design-time crest
        const aow::Player* p = game_seat();
        if (p == nullptr) return;
        game_leader = p->display_name();
        game_race = aow::race_label_plural(p->race);
        // `if (race == -1) race = 0xc;` - the engine maps "no race" to the
        // thirteenth crest (FUN_00423170).
        const int race = p->race == 0xff ? 12 : static_cast<int>(p->race);
        game_shield_index = race;
        if (p->leader.present) game_load_face(p->leader.face_index);
    }

    void mv_load() {
        mv_tables = aow::build_move_point_tables();
        mv_human = 0xff;
        mv_ready = false;
        if (game_state_map.levels.empty()) return;
        std::string err;
        aow::load_game_state(game_state_map, mv_gs, err);
        aow::load_unit_catalog(root_dir, mv_cat, err);
        aow::fill_tile_terrain(game_state_map,
                               static_cast<std::size_t>(game_level), game_res);
        // The engine links Cursors.ilb at start-up (TAoWWHSSet.Create); doing it
        // here too means the first hover of the session is already warm.
        std::string oerr;
        aow::prepare_move_overlay(root_dir, oerr);
        for (const aow::Player& p : mv_gs.players) {
            if (p.type == aow::PlayerType::Human) {
                mv_human = p.index;
                break;
            }
        }
        mv_ready = !mv_gs.players.empty();
        // The Events banner shows the seated player's leader (name, race, face,
        // crest) - all read from the map (see game_resolve_banner).
        game_resolve_banner();
        mv_explore_for_human();
        // The day/turn loop starts on the seated player (TAoWHSMap.Activate
        // hands it the first turn; the order's entry 0 is the independents).
        // `apply_scenario_players` restarts it if the setup control moved the
        // Human to another race.
        game_turns_started = false;
        game_begin_turns();
    }

    // `TArmy.UpdateExploration@0x5578e1a8` -> `TAoWMapLevel.ExploreArea@
    // 0x5577212c`: every army marks the hexes within its vision range as explored
    // for its player. The radius is `TAbstractUnit.VisibilityRange@0x55780ef8`:
    // 4, or `2 * the unit's vision ability (0x40) + 4`, folded to the army's best
    // unit by `TArmy.UpdateVisibilityRanges@0x5578e10c`. A scenario file carries
    // only a saved game's mask (the shipped maps ship zeros), so this pass is what
    // makes `TAoWMapField.GetExplored` answer like the retail game at map start.
    //
    // The replica only needs the *seated player's* bit: it decides the blocked
    // move icon (`TSelectedArmy.UpdateMoveCursor`'s `| 1`) and the trail's
    // explored-only search rule. TODO(fog): the live vision of later moves and the
    // fog rendering itself are a separate pass (docs/09_GAME_SCREEN.md, section 7).
    void mv_explore_for_human() {
        if (mv_human == 0xff) return;
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size()) {
            return;
        }
        aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        const std::uint32_t bit = 1u << (mv_human & 0x1f);
        auto explore = [&](int cx, int cy) {
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    if (aow::hex_distance(cx, cy, static_cast<int>(x),
                                          static_cast<int>(y)) >
                        kUnitVisionRange) {
                        continue;
                    }
                    lv.tiles[static_cast<std::size_t>(y) * lv.width + x].explored |= bit;
                }
            }
        };
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                const aow::MapTile& t =
                    lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                for (const aow::MapHs& hs : t.objects) {
                    if (hs.class_id != aow::kMapArmyHsClassId || hs.units.empty()) {
                        continue;
                    }
                    if (hs.owner != mv_human) continue;
                    explore(static_cast<int>(x), static_cast<int>(y));
                }
            }
        }
    }

    // The army standing on (x,y) of the shown level, or nullptr.
    aow::MapHs* army_on_hex(int x, int y) {
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size()) {
            return nullptr;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        if (x < 0 || y < 0 || static_cast<std::uint32_t>(x) >= lv.width ||
            static_cast<std::uint32_t>(y) >= lv.height) {
            return nullptr;
        }
        aow::MapTile& t = game_state_map.levels[static_cast<std::size_t>(game_level)]
                              .tiles[static_cast<std::size_t>(y) * lv.width +
                                     static_cast<std::size_t>(x)];
        for (aow::MapHs& hs : t.objects) {
            if (hs.class_id == aow::kMapArmyHsClassId && !hs.units.empty()) {
                return &hs;
            }
        }
        return nullptr;
    }

    // The army's remaining move points this turn: `TArmy.MovePoints` (min over
    // its units, capped at 0x32) bounded by what it has left after moving
    // (`army_mp_left`, set by `game_move_selected` and refilled by
    // `mv_reset_army_turn`). `TAoWHSMap.SelectNextArmy` only offers an army
    // whose value is at least 2. Each unit's own remaining points are read, so
    // the number follows what the party has actually spent.
    int army_remaining_mp(const aow::MapHs& a) const {
        std::vector<std::uint8_t> mps;
        for (const aow::MapUnit& u : a.units) {
            mps.push_back(aow::unit_move_points_left(mv_cat, u));
        }
        int mp = aow::army_move_points(mps);
        if (a.army_mp_left >= 0 && a.army_mp_left < mp) mp = a.army_mp_left;
        return mp;
    }

    // Folds `a` into `mv_army` and opens its party window with every unit
    // travelling - `TSelectedArmy.SetSelection` is seeded with the army's own
    // unit mask, and `TControlWin.CheckEnd` then repaints it. `x,y` is the hex
    // the party is reported on; for the incoming army of a merge in flight it is
    // the origin hex (the army is not on the map while its sprite walks).
    bool mv_use_army(const aow::MapHs& a, int x, int y) {
        std::vector<std::uint8_t> mts, mps;
        std::vector<std::array<bool, 256>> abs;
        for (const aow::MapUnit& u : a.units) {
            mts.push_back(aow::unit_move_types(u));
            // The budget is what each unit has *left* this turn (TUnit
            // .GetMovePoints +0x3d), not its fresh allowance.
            mps.push_back(aow::unit_move_points_left(mv_cat, u));
            abs.push_back(aow::decode_ability_bitmap(u.ability_mask, u.ability_count));
        }
        mv_army = aow::army_move(mv_tables, mts, mps, abs);
        mv_selected = mv_army.valid;
        mv_sel_x = x;
        mv_sel_y = y;
        game_show_party = mv_selected;
        if (mv_selected) {
            party_sel = 0;
            for (std::size_t i = 0; i < a.units.size() && i < kPartySlots; ++i) {
                party_sel |= static_cast<std::uint8_t>(1u << i);
            }
            party_focus = a.units.empty() ? -1 : 0;
        } else {
            party_sel = 0;
            party_focus = -1;
        }
        party_hover = party_pressed = -1;
        return mv_selected;
    }

    // Re-folds `mv_army` for the units that are *currently travelling*
    // (`TSelectedArmy.SetSelection` -> `TArmy.CreateMovePointTable`,
    // `TArmy.MoveTypes`, `TArmy.MovePoints`). The Party window's pill clicks
    // change the mask, so this runs from `party_refresh()`: a unit left behind
    // leaves the army's move-point table, its move-type union and its budget at
    // once. A unit with no move points left therefore cannot pin the rest of
    // the party, and the remaining units move on their own remaining points.
    void mv_fold_selection() {
        std::vector<std::uint8_t> mts, mps;
        std::vector<std::array<bool, 256>> abs;
        for (int s = 0; s < kPartySlots; ++s) {
            if (!party_travels(s)) continue;
            const aow::MapUnit& u = *party_unit(s);
            mts.push_back(aow::unit_move_types(u));
            mps.push_back(aow::unit_move_points_left(mv_cat, u));
            abs.push_back(aow::decode_ability_bitmap(u.ability_mask, u.ability_count));
        }
        // No unit selected: `TArmy.CreateMovePointTable` leaves the table
        // all-0xFF and `MovePoints` is 0, so the party cannot march.
        mv_army = aow::army_move(mv_tables, mts, mps, abs);
    }

    // Fills `mv_army` for the army standing on (x,y). False when it is not the
    // human player's (nothing is selected then).
    bool mv_select(int x, int y) {
        aow::MapHs* a = army_on_hex(x, y);
        if (a == nullptr || mv_human == 0xff || a->owner != mv_human) {
            mv_selected = false;
            mv_sel_x = mv_sel_y = -1;
            mv_army = aow::ArmyMove();
            return false;
        }
        return mv_use_army(*a, x, y);
    }

    // Recomputes the path from the selected army to (hx,hy) and records whether
    // there is one to draw. The bound is the engine's flood-fill limit; a
    // generous 0xFF lets the whole beyond-this-turn trail be computed (the dim
    // arrows + dim X).
    void mv_update_path(int hx, int hy) {
        mv_path = aow::MovePath();
        mv_have_path = false;
        if (!mv_selected || hx < 0 || hy < 0) return;
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size()) {
            return;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        mv_path = aow::find_move_path(mv_army.table, lv, mv_sel_x, mv_sel_y, hx,
                                      hy, 0xFF);
        mv_have_path = !mv_path.empty();
    }

    // Draws the walking army's sprite between the two hexes of its current step
    // (aow/unit_anim.hpp). The pose is the unit's own walking sequence for the
    // step's direction; the anchor is the interpolated hex origin (HXtoHP) plus
    // the sequence's xoff/yoff and the pose's base+shift - TUnitHS.Show.
    void mv_draw_moving() {
        if (!mv_anim || !mv_walk.active()) return;
        int ax = 0, ay = 0, bx = 0, by = 0;
        aow::hex_origin(mv_walk.from_x(), mv_walk.from_y(), ax, ay);
        aow::hex_origin(mv_walk.to_x(), mv_walk.to_y(), bx, by);
        const double t = mv_walk.t();
        const int ix = static_cast<int>(ax + (bx - ax) * t);
        const int iy = static_cast<int>(ay + (by - ay) * t);
        std::vector<std::uint8_t> pose;
        int w = 0, h = 0, vx = 0, vy = 0;
        std::string err;
        // Over the walk's last few animation frames the walking pose fades into
        // the standing pose it hands over to (aow::walk_settle_weight), so the
        // frame the walk ends on - when the level render draws the army on its
        // new hex - is a settle rather than a pose snap.
        const double settle =
            aow::walk_settle_weight(mv_walk.remaining(), aow::kAnimFramesPerTick);
        // The army's ground shadow travels with it (`TArmyHS.ShowShadow@0x557912a8`
        // derives the same anchor as the body) and is composited first - the
        // shadow object sits one show-priority layer below the unit.
        {
            std::vector<std::uint8_t> shadow;
            int shw = 0, shh = 0, shvx = 0, shvy = 0;
            if (aow::unit_shadow_rgba(mv_cat, root_dir, mv_anim_unit,
                                      mv_walk.direction(), mv_anim_clock,
                                      mv_anim_phase, shadow, shw, shh, shvx,
                                      shvy, err)) {
                const int sox = ix + shvx;
                const int soy = iy + shvy;
                aow::blit_shadow_rgba(game_draw, static_cast<int>(game_pw),
                                      static_cast<int>(game_ph), shadow, shw,
                                      shh, sox, soy);
                mv_mark_dirty(sox - shw, soy - shh, sox + 2 * shw,
                              soy + 2 * shh);
            }
        }
        if (!aow::unit_settle_pose_rgba(mv_cat, root_dir, mv_anim_unit,
                                        mv_walk.direction(), mv_anim_clock,
                                        mv_anim_phase, settle, pose, w, h, vx, vy,
                                        err)) {
            return;
        }
        const int ox = ix + vx;
        const int oy = iy + vy;
        aow::blit_sprite_rgba(game_draw, static_cast<int>(game_pw),
                              static_cast<int>(game_ph), pose, w, h, ox, oy);
        mv_mark_dirty(ox - w, oy - h, ox + 2 * w, oy + 2 * h);
        // `TArmyHS.Show` draws the army's size banner after the body, so the
        // walking sprite carries it too (the resting copy is hidden meanwhile).
        std::vector<std::uint8_t> banner;
        int bw = 0, bh = 0, bvx = 0, bvy = 0;
        if (aow::army_banner_rgba(game_state_map, mv_anim_army, root_dir, ix,
                                  iy, banner, bw, bh, bvx, bvy, err)) {
            const int bx = ix + bvx;
            const int by = iy + bvy;
            aow::blit_sprite_rgba(game_draw, static_cast<int>(game_pw),
                                  static_cast<int>(game_ph), banner, bw, bh,
                                  bx, by);
            mv_mark_dirty(bx - bw, by - bh, bx + 2 * bw, by + 2 * bh);
        }
    }

    // The hexagon selector: the blue 3D hex `TAoWE.TAbstractAoWHSMap` paints over
    // the hex a move selection is pointing at, from the moment `TMoveSelector.
    // Activate@0x55763e74` shows it until Deactivate (i.e. while one of our armies
    // is selected and the pointer is on a hex, and not while the move plays).
    //
    // Both halves are `Images\General.ILB` `3dHex.BMP` (ids 6 and 7) drawn at
    // `HXtoHP(hex) + (-8, -0x1e)`. The engine keeps them in the scene as two
    // sprites, one on either side of whatever is on the hex: the *back* half (art
    // 7, the upper edges) sorts at the hex's own key - behind its contents - and
    // the *front* half (art 6, the lower perimeter) gets the key "just past the
    // hex's topmost sprite" (`UpdateHexagonSelector@0x55773424`), i.e. in front of
    // them. The replica composites exactly that over the flat level canvas: the far
    // wall, the hex's contents back over it, then the near wall. The trail is
    // composited after both, as the engine draws `ShowMovePaths` after the
    // hexagon-sprite scene.
    void mv_draw_hex_selector() {
        if (!game_hex_selector_shown()) return;
        if (mv_contents_stale) mv_build_contents();
        int hx = 0, hy = 0;
        aow::hex_origin(game_hover_hx, game_hover_hy, hx, hy);
        mv_paint_selector(hex_sel_back, hx, hy, true);
        mv_paint_selector(hex_sel_front, hx, hy, false);
        // The two halves' boxes (48x63 together), padded, are the dirty rectangle
        // for the next refresh - which restores it from the flat canvas.
        const int x0 = hx + std::min(hex_sel_back.anchor_dx, hex_sel_front.anchor_dx) - 2;
        const int y0 = hy + std::min(hex_sel_back.anchor_dy, hex_sel_front.anchor_dy) - 2;
        const int x1 = hx + std::max(hex_sel_back.anchor_dx + static_cast<int>(hex_sel_back.w),
                                     hex_sel_front.anchor_dx + static_cast<int>(hex_sel_front.w)) + 2;
        const int y1 = hy + std::max(hex_sel_back.anchor_dy + static_cast<int>(hex_sel_back.h),
                                     hex_sel_front.anchor_dy + static_cast<int>(hex_sel_front.h)) + 2;
        mv_mark_dirty(x0 - 2, y0 - 2, x1 + 2, y1 + 2);
    }

    // Brightens one selector half into `game_draw` at the hex origin - the engine's
    // `GenerateBrightenBlendTable` blend (`dst = max(dst, src)` per channel), which
    // is how the marker is painted (see `kHexSelectorAnchorX`). The art's near-black
    // gradient fill therefore never darkens the tile: only the blue channel of the
    // gradient and the bright rim show through, so the hovered hex's terrain stays
    // visible inside the marker (BMRef.png) instead of being covered by its fill.
    // With `contents_over_it` (the back half) the level's contents layer is then
    // drawn *opaque* on top of the half's pixels, so a city, another object or a
    // unit standing on the hex shows at full strength over the marker's far wall -
    // the back half keeps the hex's own scene key, so the engine sorts it behind
    // those sprites. The contents of a hex *below* the hovered one come over it too
    // - their scene keys are larger - while a hex *above* is covered by the wall,
    // which is also what those keys do.
    void mv_paint_selector(const Sprite& s, int hx, int hy, bool contents_over_it) {
        if (!s.ok) return;
        const bool have_contents = mv_px_contents.size() == game_draw.size();
        const int ox = hx + s.anchor_dx;
        const int oy = hy + s.anchor_dy;
        for (std::uint32_t sy = 0; sy < s.h; ++sy) {
            const int dy = oy + static_cast<int>(sy);
            if (dy < 0 || dy >= static_cast<int>(game_ph)) continue;
            for (std::uint32_t sx = 0; sx < s.w; ++sx) {
                const std::uint8_t* sc =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                if (sc[3] == 0) continue;
                const int dx = ox + static_cast<int>(sx);
                if (dx < 0 || dx >= static_cast<int>(game_pw)) continue;
                const std::size_t p =
                    (static_cast<std::size_t>(dy) * game_pw +
                     static_cast<std::size_t>(dx)) * 4;
                brighten_px(game_draw.data() + p, sc);
                if (!contents_over_it || !have_contents) continue;
                const std::uint8_t* cc = mv_px_contents.data() + p;
                if (cc[3] != 0) {
                    blend_px(game_draw.data() + p, cc, cc[3]);
                    ++mv_selector_contents_px;
                }
            }
        }
    }

    // `dst = brighten(dst, src)`: per channel the larger of the two - the engine's
    // `GenerateBrightenBlendTable` blend the marker halves are drawn with. The
    // `src` pixels still carry the sprite's colour key as alpha 0 (see
    // `key_to_alpha`); the caller skips those, so here `src` is always opaque art.
    static void brighten_px(std::uint8_t* dst, const std::uint8_t* src) {
        for (int c = 0; c < 3; ++c) {
            if (src[c] > dst[c]) dst[c] = src[c];
        }
        dst[3] = 255;
    }

    // `dst = over(dst, src)` with the source's alpha `a`.
    static void blend_px(std::uint8_t* dst, const std::uint8_t* src, unsigned a) {
        if (a == 0) return;
        if (a >= 255) {
            dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 255;
            return;
        }
        const unsigned inv = 255u - a;
        for (int c = 0; c < 3; ++c) {
            dst[c] = static_cast<std::uint8_t>((src[c] * a + dst[c] * inv) / 255);
        }
        dst[3] = 255;
    }

    // Composites the level's animated overlays into `game_draw` at the current
    // map clock, in the engine's own layer order: the buildings (mapviz's
    // draw_structure_overlays, see docs/17_BUILDING_ANIM.md), then the water
    // hexagons' ripples/fish/sparkles (draw_water_overlays, see
    // docs/18_WATER_ANIM.md) and finally the armies (draw_unit_overlays,
    // docs/13_UNITS_ON_MAP.md). The level image has all of them left out
    // (`MapVizOptions::no_animated_overlays` + `no_units`), so the engine's
    // per-frame scene redraw becomes a cheap re-composite of a handful of small
    // sprites instead of a ~28 ms level re-render.
    //
    // The armies come LAST because that is where the engine sorts them
    // (`AoWE.TUnitHS.CalcShowPriority`: 0x68000000, above a structure's layer),
    // so a unit standing next to a building is never covered by the building's
    // glowing orb, its owner flag or a ripple. Only a world effect - the map's
    // spell gfx (`AoWE.TWorldFXHS`, not implemented yet) - is meant to draw over
    // a unit.
    //
    // The water and unit requests are clipped to the map window's camera: the
    // water hexagons and the armies are spread over the whole level, so their
    // sprites' union box would otherwise be the entire map and `mv_restore_dirty`
    // would copy the whole canvas every frame. Every water hexagon's counter is
    // still advanced (the per-hex state lives in mapviz), only the compositing is
    // clipped.
    void mv_draw_structure_anim() {
        if (game_draw.empty() || game_state_map.levels.empty()) return;
        int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        std::string err;
        if (aow::draw_structure_overlays(game_state_map, game_res, root_dir,
                                         game_level, game_clock, game_draw,
                                         game_pw, game_ph, /*bgra=*/false, x0,
                                         y0, x1, y1, err)) {
            if (x1 > x0 && y1 > y0) mv_mark_dirty(x0, y0, x1, y1);
        }
        int wx0 = 0, wy0 = 0, wx1 = 0, wy1 = 0;
        if (!aow::draw_water_overlays(
                game_state_map, game_res, root_dir, game_level, game_clock,
                game_draw, game_pw, game_ph, /*bgra=*/false, wx0, wy0, wx1, wy1,
                err, aow::WaterAnimOptions{}, cam_x - 32, cam_y - 32,
                cam_x + cam_w + 32, cam_y + cam_h + 32)) {
            return;
        }
        if (wx1 > wx0 && wy1 > wy0) mv_mark_dirty(wx0, wy0, wx1, wy1);
        // The armies last, over the buildings and the water. While a walk plays
        // the destination army stays hidden - the moving sprite stands in for it.
        // The clip is the camera, exactly like the water's - but before the first
        // game_update() has resolved the viewport (`cam_w == 0`, i.e. a level
        // loaded into a screen that has not ticked yet) it is the whole canvas:
        // the units are not in the cached level, so a degenerate clip would leave
        // them out of the first composited frame.
        const bool cam_ok = cam_w > 0 && cam_h > 0;
        const int ucx0 = cam_ok ? cam_x - 32 : 0;
        const int ucy0 = cam_ok ? cam_y - 32 : 0;
        const int ucx1 = cam_ok ? cam_x + cam_w + 32 : 0;
        const int ucy1 = cam_ok ? cam_y + cam_h + 32 : 0;
        int ux0 = 0, uy0 = 0, ux1 = 0, uy1 = 0;
        if (!aow::draw_unit_overlays(game_state_map, root_dir, game_level,
                                     game_clock, game_draw, game_pw, game_ph,
                                     /*bgra=*/false, ux0, uy0, ux1, uy1, err,
                                     mv_hide_level, mv_hide_x, mv_hide_y, ucx0,
                                     ucy0, ucx1, ucy1)) {
            return;
        }
        if (ux1 > ux0 && uy1 > uy0) mv_mark_dirty(ux0, uy0, ux1, uy1);
    }

    // Rebuilds `game_draw` = the pristine `game_px` + the hover path overlay.
    // The canvas is a whole level render (megabytes), so it is NOT recopied per
    // hover: only the rectangles the previous overlay painted are restored from
    // `game_px` (the viewer's dirty-rect trick, docs/15_MOVEMENT.md), and the new
    // overlay's hexes are recorded for the next restore.
    void mv_refresh_draw() {
        mv_selector_contents_px = 0;
        if (game_px.empty() || game_draw.size() != game_px.size()) {
            game_draw = game_px;          // first draw, or a fresh level render
            mv_dirty = false;
        } else {
            mv_restore_dirty();
        }
        // The animated buildings are re-composited every map frame (their previous
        // rectangle was just restored from `game_px`).
        mv_draw_structure_anim();
        // While the army walks, its sprite owns the canvas: draw it instead of
        // the hover trail (the engine hides the trail during a move).
        if (mv_anim) {
            mv_draw_moving();
            return;
        }
        // The move selection's hexagon marker sits under the trail and shows even
        // when the hovered hex has no path.
        mv_draw_hex_selector();
        if (!mv_have_path) return;
        std::string err;
        if (!aow::draw_move_path_overlay(game_draw, static_cast<int>(game_pw),
                                         static_cast<int>(game_ph), root_dir,
                                         mv_path, mv_army.budget, mv_army.budget,
                                         err, false)) {
            return;
        }
        for (const aow::MoveStep& s : mv_path.steps) mv_mark_hex(s.x, s.y);
    }

    // Grows the dirty rectangle (in canvas pixels) to cover a hex's overlay
    // sprites; `kOverlayPad` is generous - the arrows are 19x14 inside a 48x32
    // hex, drawn at `HXtoHP - 8 + (vx,vy)`.
    void mv_mark_dirty(int x0, int y0, int x1, int y1) {
        if (x1 <= x0 || y1 <= y0) return;
        if (!mv_dirty) {
            mv_dirty_x0 = x0;
            mv_dirty_y0 = y0;
            mv_dirty_x1 = x1;
            mv_dirty_y1 = y1;
            mv_dirty = true;
            return;
        }
        if (x0 < mv_dirty_x0) mv_dirty_x0 = x0;
        if (y0 < mv_dirty_y0) mv_dirty_y0 = y0;
        if (x1 > mv_dirty_x1) mv_dirty_x1 = x1;
        if (y1 > mv_dirty_y1) mv_dirty_y1 = y1;
    }

    void mv_mark_hex(int hx, int hy) {
        int px = 0, py = 0;
        cam_hex_to_px(hx, hy, px, py);
        mv_mark_dirty(px - kOverlayPad, py - kOverlayPad, px + kOverlayPad,
                      py + kOverlayPad);
    }

    // Copies the last overlay's rectangle back out of the pristine render.
    void mv_restore_dirty() {
        if (!mv_dirty) return;
        mv_dirty = false;
        const int x0 = std::max(0, mv_dirty_x0);
        const int y0 = std::max(0, mv_dirty_y0);
        const int x1 = std::min(static_cast<int>(game_pw), mv_dirty_x1);
        const int y1 = std::min(static_cast<int>(game_ph), mv_dirty_y1);
        if (x1 <= x0 || y1 <= y0) return;
        const std::size_t row =
            static_cast<std::size_t>(x1 - x0) * 4;  // bytes in one row span
        for (int y = y0; y < y1; ++y) {
            const std::size_t off =
                (static_cast<std::size_t>(y) * game_pw +
                 static_cast<std::size_t>(x0)) * 4;
            std::memcpy(game_draw.data() + off, game_px.data() + off, row);
        }
    }

    // The pointer moved over the map (or a click changed the selection): refresh
    // the trail from the selected army to the hovered hex.
    void game_update_path() {
        mv_update_path(game_hover_hx, game_hover_hy);
        mv_refresh_draw();
    }

    // Re-renders the shown level from the kept map (after a walk) into `game_px`.
    bool mv_render_level() {
        if (game_state_map.levels.empty()) return false;
        aow::MapVizOptions opt;
        opt.level = game_level;
        opt.objects = true;
        opt.max_dim = 0;
        // While a walk plays, the army already sits on its destination hex but
        // its sprite is drawn by the animation - hide the resting copy so the
        // unit is never drawn twice.
        opt.hide_army_level = mv_hide_level;
        opt.hide_army_x = mv_hide_x;
        opt.hide_army_y = mv_hide_y;
        // The animated buildings are composited per map frame (mv_refresh_draw),
        // and so are the armies - the last world layer (see mv_draw_structure_anim).
        opt.no_animated_overlays = true;
        opt.no_units = true;
        std::string err;
        // The level is rendered in two layers: the terrain, and everything drawn
        // over it (objects, roads, bridges and cities - the armies are left to
        // the per-frame pass above, `no_units`). `game_px` is the flat composite
        // every other path uses; the two layers are what the move-selection marker
        // re-composites one hex from, so the blue hex stays *behind* the hex's
        // contents (mapviz.hpp's `contents`, docs/09 section 6.9).
        const bool ok = mv_render_layers(opt);
        // The level render changed (an army moved onto a new hex, or another
        // level): the composited canvas is stale even when its size is unchanged,
        // so drop it - the dirty rect alone would leave the old army behind.
        game_draw.clear();
        mv_dirty = false;
        return ok;
    }

    // Renders the shown level twice: the flat canvas (`game_px`, the port's
    // verified row-major composite, unchanged) and the same level *split* into its
    // terrain and everything drawn over it. Only the move-selection marker needs
    // the layers apart, and it reads the flat canvas as its background - see
    // mv_draw_hex_selector.
    bool mv_render_layers(const aow::MapVizOptions& opt) {
        std::string err;
        if (!aow::render_map_rgba(game_state_map, game_res, root_dir, opt,
                                  game_px, game_pw, game_ph, err, nullptr)) {
            return false;
        }
        // The contents layer is rebuilt on demand (mv_build_contents): only a move
        // selection needs it, and any level render invalidates it.
        mv_px_contents.clear();
        mv_contents_stale = true;
        return true;
    }

    // Produces the level's *contents* layer - the objects, roads, bridges, cities
    // and armies the sweep draws over the terrain - for the marker to put back
    // over its far wall. Runs at most once per level render, and only while an
    // army is selected (docs/09_GAME_SCREEN.md 6.9). The armies stay in this
    // layer (no `no_units`): the marker's hex may carry the selected army, and the
    // engine puts the hex's contents back over its far wall - so the unit has to
    // come out of the layer, not out of the per-frame composite.
    void mv_build_contents() {
        mv_contents_stale = false;
        mv_px_contents.clear();
        if (game_state_map.levels.empty()) return;
        aow::MapVizOptions opt;
        opt.level = game_level;
        opt.objects = true;
        opt.max_dim = 0;
        opt.hide_army_level = mv_hide_level;
        opt.hide_army_x = mv_hide_x;
        opt.hide_army_y = mv_hide_y;
        opt.no_animated_overlays = true;  // composited by mv_refresh_draw
        std::vector<std::uint8_t> terrain;
        std::uint32_t tw = 0, th = 0;
        std::string err;
        if (!aow::render_map_rgba(game_state_map, game_res, root_dir, opt,
                                  terrain, tw, th, err, &mv_px_contents)) {
            mv_px_contents.clear();
            return;
        }
        if (tw != game_pw || th != game_ph) mv_px_contents.clear();
    }

    // The scanner's Surface / Caverns tabs: show another level of the SAME
    // in-memory map - re-rendering from it, so an army moved during this session
    // stays moved (the old path reloaded the .HSM from disk, which would undo it).
    // False when the map has no such level (the caller keeps the old one).
    bool game_set_level(int lvl) {
        if (lvl < 0 ||
            static_cast<std::size_t>(lvl) >= game_state_map.levels.size()) {
            return false;
        }
        game_level = lvl;
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(lvl)];
        map_w = lv.width;
        map_h = lv.height;
        mv_load();          // the terrain is per level; the tables are shared
        mv_render_level();
        mv_reset();
        return true;
    }

    // TSetupControl.SetupMap (0x557e0b64): the scenario screen's per-race player
    // types are written back into the loaded map roster (`player[+0xa7] =
    // settings[+8]`) before the first turn. The map's races are unchanged - every
    // player keeps the race the scenario placed - so this only decides *which*
    // race is the local Human, i.e. whose cities and armies (its units) the
    // player controls. It also re-resolves `mv_human` and the leader banner's
    // race, which used to stay on the map's default (the "you set Elves as
    // Human but got the Orc player's units" bug).
    void apply_scenario_players() {
        if (!mv_ready || mv_gs.players.empty()) return;
        std::uint8_t rt[kScenRaceCount];
        for (int r = 0; r < kScenRaceCount; ++r) {
            rt[r] = static_cast<std::uint8_t>(scen_ptype[r]);
        }
        const std::uint8_t human =
            aow::apply_setup_player_types(mv_gs, rt, kScenRaceCount);
        const std::uint8_t before = mv_human;
        mv_human = human;
        // The Events banner follows the seat (name / plural race / face / crest),
        // so re-resolve it for the player the setup control marked Human.
        game_resolve_banner();
        // SetupMap decides which seat is the local player *before* the first
        // turn, so the loop starts over when the chosen race moved the seat.
        if (before != mv_human) game_begin_turns(true);
    }

    // The Scenario screen's Start! button (TScenSetup.StartBtnClick 0x00412568):
    // load the map the setup control holds and drop into the in-game screen.
    // Returns false (with `error`) when no map is selected or it cannot be read.
    bool start_game(std::string* error) {
        std::string name = scen_map_name;
        if (name.empty() && !scen_maps.empty()) name = scen_maps.front();
        if (name.empty()) {
            *error = "no scenario map selected";
            return false;
        }
        const std::string path =
            (fs::path(root_dir) / "Scenario" / "Custom" / name).string();
        if (!load_game_map(path, error)) return false;
        // Apply the setup control's race -> player-type assignment (SetupMap),
        // which is what turns the race the screen marked Human into the player
        // whose units are played.
        apply_scenario_players();
        game_hover = -1;
        // The engine centres the view on the human player's leader when a map
        // opens (TAoWE.TAoWHSMap.CenterOnLeader 0x55778e1c) - the port centres
        // there but leaves the leader unselected. Arm that here; the first
        // game_update() applies it once the viewport size is known.
        game_center_on_start = true;
        screen = 3;      // -> the in-game screen (TWorldMap)
        // Drop the title theme and start the map's own playlist (AoW.exe
        // FUN_00454b70 stops the old song, FUN_00454d2c plays the map's list).
        set_music_playlist(game_songs);
        return true;
    }

    // The five-orb hit test (AoW.exe 0x00419488). Returns 0..4, or -1.
    int hit_test(int x, int y) const {
        const int bottom = scene_bottom();
        if (!(bottom - 125 < y && y < bottom)) return -1;
        const int w = static_cast<int>(orb_slot_w);
        const int base = ((w + disp_w) >> 1) - 320;
        for (int i = 0; i < kButtonCount; ++i) {
            const int left = base + i * kButtonPitch;
            if (left < x && x < left + w) return i;
        }
        return -1;
    }

    // TMultiWindow's animation (MultiWinUpdate, 0x0042ae54). gt_frame is the
    // +0xF0 counter; both panels are placed from it.
    int gt_top_y() const {
        const int t = gt_frame < kGtPanelH ? gt_frame : kGtPanelH;
        return t - kGtPanelH;  // -286 (above the screen) -> 0
    }
    int gt_bottom_y() const {
        const int t = gt_frame < kGtPanelH ? gt_frame : kGtPanelH;
        return disp_h - t + kGtPanelH - kGtBottomH;  // 700 (below) -> 414
    }

    // Game Type hit test. Returns 0..2 for Single/TCP/Email, 3 for Back to
    // Title (CloseBtnClick), or -1.
    int gtype_hit(int x, int y) const {
        if (x < kGtPanelX || x >= kGtPanelX + kGtBtnW) return -1;
        const int top_y = gt_top_y();
        for (int i = 0; i < kGtBtnCount; ++i) {
            const int by = top_y + kGtBtnY[i];
            if (by <= y && y < by + kGtBtnH) return i;
        }
        const int by = gt_bottom_y() + kGtBackY;
        if (by <= y && y < by + kGtBtnH) return 3;
        return -1;
    }

    // One frame of the Game Type screen (TMultiWindow).
    Canvas render_gtype() const {
        Canvas cv(disp_w, disp_h);
        cv.clear(0, 0, 0);

        // MultiPnl: the 640x480 SetupBg03 backdrop (the screen replaces the
        // title window, so this texture does not pan).
        cv.blit(gt_bg, 0, 0, kAlphaFull, 0, 0, disp_w, disp_h);

        const int top_y = gt_top_y();
        const int bot_y = gt_bottom_y();

        // The buttons are drawn first, then their panel texture is composited
        // over them at kGtPanelAlpha. In the game the panel art sits over the
        // whole column (buttons included), which is what mutes TitleIcon's
        // (65,85,180) fill down to the game's (24,33,65) navy.
        for (int i = 0; i < kGtBtnCount; ++i) {
            const int st = (i == gt_hover) ? (gt_pressed == i ? 2 : 1) : 0;
            cv.blit(gt_btn[st], kGtPanelX, top_y + kGtBtnY[i], kAlphaFull, 0, 0,
                    disp_w, disp_h);
        }
        {
            const int st = (gt_hover == 3) ? (gt_pressed == 3 ? 2 : 1) : 0;
            cv.blit(gt_btn[st], kGtPanelX, bot_y + kGtBackY, kAlphaFull, 0, 0,
                    disp_w, disp_h);
        }
        cv.blit(gt_top, kGtPanelX, top_y, kGtPanelAlpha, 0, 0, disp_w, disp_h);
        cv.blit(gt_bottom, kGtPanelX, bot_y, kGtPanelAlpha, 0, 0, disp_w,
                disp_h);

        // Captions on top of the panels, at full strength.
        draw_text8(cv, kGtPanelX + kGtPanelW / 2, top_y + kGtLabelY,
                   "Game Type");
        for (int i = 0; i < kGtBtnCount; ++i) {
            draw_text(cv, kGtPanelX + kGtBtnW / 2, top_y + kGtBtnY[i] + 5,
                      kGtBtnText[i]);
        }
        draw_text(cv, kGtPanelX + kGtBtnW / 2, bot_y + kGtBackY + 5,
                  "Back to Title");

        if (cursor_on && cursor.ok) cv.blit_keyed(cursor, mouse_x, mouse_y);
        return cv;
    }

    // ---------------- the real-art drawing helpers (no placeholder fills) ----
    // A TAOWPanel's body: the panel class lays its ILTexture tile (GenericT 1 =
    // ShadowOv.BMP) over a black veil so the backdrop still shows through a
    // little. Measured over bin/RaceSelectorRef2.png's race cells: the body is
    // exactly 16 % of Backg03 (e.g. (460,230): backdrop (148,76,90) -> (24,8,16)),
    // i.e. a black veil at 215, with the maze tile adding its ~9 %.
    void blit_panel_body(Canvas& cv, int x, int y, int w, int h) const {
        cv.fill(x, y, w, h, 0, 0, 0, kPanelBodyVeil);
        if (!scen_panel_tex.ok) return;
        const int tw = static_cast<int>(scen_panel_tex.w);
        const int th = static_cast<int>(scen_panel_tex.h);
        if (tw <= 0 || th <= 0) return;
        for (int oy = y; oy < y + h; oy += th) {
            for (int ox = x; ox < x + w; ox += tw) {
                cv.blit(scen_panel_tex, ox, oy, kPanelBodyAlpha, x, y, x + w,
                        y + h);
            }
        }
    }
    // Tile an image over a rect, clipped to it (what a TAOWListBox / TAOWMemo
    // does with its ILTextureNormal: blank1.bmp is a flat 50x50 body).
    void fill_tex(Canvas& cv, const Sprite& s, int x, int y, int w,
                  int h) const {
        if (!s.ok || s.w == 0 || s.h == 0) return;
        const int tw = static_cast<int>(s.w);
        const int th = static_cast<int>(s.h);
        for (int oy = y; oy < y + h; oy += th) {
            for (int ox = x; ox < x + w; ox += tw) {
                cv.blit(s, ox, oy, kAlphaFull, x, y, x + w, y + h);
            }
        }
    }
    // A TAOWPanel = body texture + the STGrFr frame (GenericF 72..79).
    void draw_panel(Canvas& cv, int x, int y, int w, int h) const {
        blit_panel_body(cv, x, y, w, h);
        blit9_impl(cv, scen_frame, x, y, w, h, 2, 2);
    }
    // The Settings panel's body: the shipped screen is black above the maze
    // texture, i.e. the ShadowOv tile is anchored to the panel's bottom edge
    // rather than repeated from its top. (Measured: bottom-anchored scores 29.2
    // against the capture's panel vs 37.5 for the tiled version.)
    void blit_panel_body_bottom(Canvas& cv, int x, int y, int w, int h) const {
        cv.fill(x, y, w, h, 0, 0, 0, 255);
        if (!scen_panel_tex.ok) return;
        const int tw = static_cast<int>(scen_panel_tex.w);
        const int th = static_cast<int>(scen_panel_tex.h);
        if (tw <= 0 || th <= 0) return;
        for (int ox = x; ox < x + w; ox += tw) {
            cv.blit(scen_panel_tex, ox, y + h - th, kAlphaFull, x, y, x + w, y + h);
        }
    }
    // The Settings panel (SettingsPnl) is a masked panel like the race cells: a
    // black veil over the backdrop with the ShadowOv tile laid over it. The
    // shipped capture's panel reads the backdrop's brown (41,33,24 at x 560,
    // y 300 = 16 % of Backg03's (238,206,164)) - i.e. exactly the panel body.
    void draw_setpanel(Canvas& cv, int x, int y, int w, int h) const {
        blit_panel_body(cv, x, y, w, h);
        blit9_impl(cv, scen_frame, x, y, w, h, 2, 2);
    }
    // A TAoWFrame group (DurFr / LFr) with FrameType = frHollowBox: just the
    // ScenFr frame (GenericF 64..71); ILIndexTexture 0 is the *unset* index, so
    // the interior stays transparent and shows the panel body behind it.
    void draw_group(Canvas& cv, int x, int y, int w, int h) const {
        blit9_impl(cv, scen_scenfr, x, y, w, h, 2, 2);
    }
    // A 3-piece widget (left cap / middle / right cap). The engine repeats the
    // middle sprite at its natural size - TAOWEdit.Draw@0x004108ec and
    // TAOWButton.Draw@0x0040b338 both loop `width / middle.width` times - so the
    // shipped skin's texture is never stretched.
    void blit_btn3(Canvas& cv, const Sprite* b, int x, int y, int w,
                   int h) const {
        if (!b[1].ok) return;
        const int lw = static_cast<int>(b[0].w);
        const int rw = static_cast<int>(b[2].w);
        cv.blit(b[0], x, y, kAlphaFull, x, y, x + w, y + h);
        if (b[1].w) {
            for (int ox = x + lw; ox < x + w - rw; ox += static_cast<int>(b[1].w)) {
                cv.blit(b[1], ox, y, kAlphaFull, x, y, x + w, y + h);
            }
        }
        cv.blit(b[2], x + w - rw, y, kAlphaFull, x, y, x + w, y + h);
    }
    // One of the three bottom buttons (TitleM ScenRBut 32/35/38).
    void draw_main_btn(Canvas& cv, int x, int y, int w, int state,
                       const std::string& caption) const {
        blit_btn3(cv, scen_main_btn[state], x, y, w, kMainBtnH);
        text_at(cv, glyphs10, x + w / 2, y + (kMainBtnH - 12) / 2, caption,
                kMainBtnTextR, kMainBtnTextG, kMainBtnTextB, true, false);
    }
    // A TAOWComboBox resting frame. Every combo on this form names
    // ILEdit = IntGfxMod.TitleScreen with ILEIndex = 26, i.e. TitleM's
    // ScenEBut 3-piece (10 / 162 / 18 x 18): a grey field whose 18 px right cap
    // carries the dark-green drop-down funnel. The DFM's BorderWidth 6 puts the
    // caption 6 px in. `fs` selects the face (Age8s on the race cells, Age8 on
    // the Settings panel, Age10 in the file dialog).
    void draw_combo(Canvas& cv, int x, int y, int w, const std::string& s,
                    const std::vector<Glyph>& fs, std::uint8_t r,
                    std::uint8_t g, std::uint8_t b) const {
        blit_btn3(cv, scen_scenebut, x, y, w, kScenComboH);
        text_at(cv, fs, x + 6, y + (kScenComboH - 12) / 2, s, r, g, b, false,
                false);
    }
    // The Settings panel's turn-mode combo: TitleM's ScenEBut skin (grey field,
    // dark-green funnel in the right cap), AOWFont = FontModule.Age8s.
    void draw_setcombo(Canvas& cv, int x, int y, int w, const std::string& s,
                       const std::vector<Glyph>& fs, std::uint8_t r,
                       std::uint8_t g, std::uint8_t b) const {
        blit_btn3(cv, scen_scenebut, x, y, w, 18);
        text_at(cv, fs, x + 6, y + 3, s, r, g, b, false, false);
    }
    // The race PlayerCB drop-down list: 7 rows at ItemHeight 15 directly under
    // the 98x18 field, its body the GenericT 4 (blank1) texture and the lit row
    // the GenericT 16 (TextLBSel) one - the ILTextureNormal / ILTextureSelected
    // the DFM names. Drawn last, on top of everything (it overlaps the rows
    // below). The rect mirrors scen_hit so the click lands on the row it shows.
    void scen_dd_rect(int gi, int& lx, int& ly, int& lw) const {
        int x, y;
        scen_cell(gi, x, y);
        lx = x + 50;
        ly = y + 25 + kScenComboH;
        lw = kScenComboW;
    }
    void draw_pc_drop(Canvas& cv, int gi) const {
        int lx, ly, lw;
        scen_dd_rect(gi, lx, ly, lw);
        const int lh = kScenPTypeCount * kScenItemH;
        if (scen_list_tex.ok) {
            const int tw = static_cast<int>(scen_list_tex.w);
            const int th = static_cast<int>(scen_list_tex.h);
            for (int oy = ly; oy < ly + lh; oy += th) {
                for (int ox = lx; ox < lx + lw; ox += tw) {
                    cv.blit(scen_list_tex, ox, oy, kAlphaFull, lx, ly, lx + lw,
                            ly + lh);
                }
            }
        } else {
            cv.fill(lx, ly, lw, lh, 12, 12, 20, 255);
        }
        // The lit row: the hovered one, else the combo's own current value
        // (a TAOWComboBox paints its selected line highlighted while closed).
        const int race = kScenGridRace[gi];
        int cur = -1;
        for (int i = 0; i < kScenPTypeCount; ++i) {
            if (kScenPTypeItem[i] == scen_ptype[race]) cur = i;
        }
        const int hi = scen_dd_hover >= 0 ? scen_dd_hover : cur;
        for (int i = 0; i < kScenPTypeCount; ++i) {
            const int ry = ly + i * kScenItemH;
            if (i == hi) {
                if (scen_list_sel_tex.ok) {
                    const int tw = static_cast<int>(scen_list_sel_tex.w);
                    const int th = static_cast<int>(scen_list_sel_tex.h);
                    for (int oy = ry; oy < ry + kScenItemH; oy += th) {
                        for (int ox = lx; ox < lx + lw; ox += tw) {
                            cv.blit(scen_list_sel_tex, ox, oy, kAlphaFull, lx, ry,
                                    lx + lw, ry + kScenItemH);
                        }
                    }
                } else {
                    cv.fill(lx, ry, lw, kScenItemH, 60, 60, 90, 255);
                }
            }
            // TextColor 14316400 (TColor 0x00DA7F70 -> RGB 112,127,218); the
            // selected (`TextSelColor`) line is clWhite.
            const int tr = i == hi ? 255 : 112;
            const int tg = i == hi ? 255 : 127;
            const int tb = i == hi ? 255 : 218;
            text_at(cv, glyphs8, lx + 6, ry + (kScenItemH - 12) / 2,
                    kScenPTypeName[kScenPTypeItem[i]], static_cast<std::uint8_t>(tr),
                    static_cast<std::uint8_t>(tg), static_cast<std::uint8_t>(tb),
                    false, false);
        }
    }
    // A TAOWEdit resting frame: the dark NGEdit skin (GenericE 15..17).
    void draw_edit(Canvas& cv, int x, int y, int w, const std::string& s,
                   const std::vector<Glyph>& fs, std::uint8_t r,
                   std::uint8_t g, std::uint8_t b) const {
        const int h = scen_edit[1].ok ? static_cast<int>(scen_edit[1].h) : 18;
        blit_btn3(cv, scen_edit, x, y, w, h);
        text_at(cv, fs, x + 4, y + (h - 12) / 2, s, r, g, b, false, false);
    }
    // TAOWCheckBox: the 12x12 NGCBox.BMP face (GenericI 47 empty / 48 checked) -
    // the same square, gold-bevelled box the shipped capture shows. It is a
    // class-0x11 sprite (masked), so blit it whole.
    void draw_check(Canvas& cv, int x, int y, bool on) const {
        const Sprite& s = scen_ngcbox[on ? 1 : 0];
        if (s.ok) {
            cv.blit(s, x, y, kAlphaFull, 0, 0, disp_w, disp_h);
        } else if (scen_chk[on ? 1 : 0].ok) {
            const Sprite& f = scen_chk[on ? 1 : 0];
            cv.blit(f, x, y, kAlphaFull, 0, 0, disp_w, disp_h);
        }
    }
    // A TAOWHScrollBar (ILIndexBack = GenericS 49 ScenSlid 3-piece track,
    // ILIndexTab = GenericS 52 3-piece thumb).
    void draw_slider(Canvas& cv, int x, int y, int w, int pos) const {
        blit_btn3(cv, scen_slider, x, y, w, 11);      // track (49/50/51)
        const int tw = static_cast<int>(scen_slider[4].w);   // thumb middle
        const int lw = static_cast<int>(scen_slider[3].w);
        const int rw = static_cast<int>(scen_slider[5].w);
        const int span = w - (lw + tw + rw);
        int tx = x + (span > 0 ? span * pos / 100 : 0);
        if (tx < x) tx = x;
        cv.blit(scen_slider[3], tx, y, kAlphaFull, 0, 0, disp_w, disp_h);
        cv.blit(scen_slider[4], tx + lw, y, kAlphaFull, 0, 0, disp_w, disp_h);
        cv.blit(scen_slider[5], tx + lw + tw, y, kAlphaFull, 0, 0, disp_w,
                disp_h);
    }

    // A TAoWFrame frHorizLine band: CeltDeco 55/56/57 = 7/100/7 x 27. The line
    // sprite (56) tiles between the two 7px caps, exactly like TAoWFrame.Draw.
    void blit_celt_band(Canvas& cv, int x, int y, int w) const {
        const Sprite& l = scen_celt[0];
        const Sprite& m = scen_celt[1];
        const Sprite& r = scen_celt[2];
        if (!m.ok || m.w == 0) return;
        const int h = static_cast<int>(m.h);
        const int lw = l.ok ? static_cast<int>(l.w) : 0;
        const int rw = r.ok ? static_cast<int>(r.w) : 0;
        if (l.ok) cv.blit(l, x, y, kAlphaFull, x, y, x + w, y + h);
        for (int ox = x + lw; ox < x + w - rw; ox += static_cast<int>(m.w)) {
            cv.blit(m, ox, y, kAlphaFull, x, y, x + w, y + h);
        }
        if (r.ok) cv.blit(r, x + w - rw, y, kAlphaFull, x, y, x + w, y + h);
    }
    // The celtic knot crosses (TAOWImage CeltL/CeltM/CeltR = CeltDeco 3, 27x27)
    // that sit on the button band.
    void blit_celt_knot(Canvas& cv, int x, int y) const {
        if (scen_celt_knot.ok) cv.blit_keyed(scen_celt_knot, x, y);
    }
    // TCelt (TAOWImage, CeltDeco id 3) tiles its 27x27 knot over the 322x27
    // strip that sits on TCeltL - that is what turns the flat line into the
    // interlocking band under the title (TAOWImage.Draw loops over w/iw).
    void blit_celt_tiles(Canvas& cv, int x, int y, int w, int h) const {
        if (!scen_celt_knot.ok || scen_celt_knot.w == 0 ||
            scen_celt_knot.h == 0) {
            return;
        }
        for (int yy = y; yy < y + h; yy += static_cast<int>(scen_celt_knot.h)) {
            for (int xx = x; xx < x + w;
                 xx += static_cast<int>(scen_celt_knot.w)) {
                cv.blit(scen_celt_knot, xx, yy, kAlphaFull, x, y, x + w, y + h);
            }
        }
    }
    // The map picker's list scrollbar (FileLB -> NSBar in the NewLBFr panel).
    // Geometry mirrors the render exactly so the hit test, the thumb and the
    // drag all agree. Max = the last valid first-row index (0 when it all fits,
    // where the DFM's VanishWhenFull hides the bar).
    struct ScenSb {
        int lx, ly, lw, lh;   // the list frame (FrGoldD well)
        int sx, sy, sw, sh;   // the scrollbar rect
        int cap;              // arrow-cap height
        int ty0, span;        // thumb travel range
        int thumb_h;          // thumb height
        int max_off;          // last valid first-row index
    };
    ScenSb scen_sb() const {
        ScenSb b{};
        const int dx = 130, dy = 35;
        b.lx = dx + 35; b.ly = dy + 109; b.lw = 310; b.lh = 141;
        const int sbw = 17;
        b.sx = b.lx + b.lw - sbw - 1; b.sy = b.ly + 2; b.sw = sbw;
        b.sh = b.lh - 4;
        b.cap = scen_ngvsb[0].ok ? static_cast<int>(scen_ngvsb[0].h) : 15;
        b.ty0 = b.sy + b.cap;
        b.span = b.sh - 2 * b.cap;
        if (b.span < 1) b.span = 1;
        const int total = static_cast<int>(scen_maps.size());
        b.max_off = total > kScenListRows ? total - kScenListRows : 0;
        int th = b.max_off > 0 ? b.span * kScenListRows / total : b.span;
        if (th < 24) th = 24;
        if (th > b.span) th = b.span;
        b.thumb_h = th;
        return b;
    }
    int scen_sb_thumb_y(const ScenSb& b) const {
        if (b.max_off <= 0) return b.ty0;
        const int travel = b.span - b.thumb_h;
        if (travel <= 0) return b.ty0;
        return b.ty0 + travel * scen_map_off / b.max_off;
    }
    // A TAOWVScrollBar frame: the NGVsbLight track (GenericS 101) with the
    // inc/dec arrow caps (94/96) and the thumb, whose top/height are the
    // scroll position the caller passes.
    void draw_vsb(Canvas& cv, const ScenSb& b, int thumb_y, int thumb_h) const {
        if (scen_ngvsb[4].ok) {
            cv.blit_stretch(scen_ngvsb[4], b.sx, b.sy, b.sw, b.sh);
        } else {
            cv.fill(b.sx, b.sy, b.sw, b.sh, 92, 86, 62, 255);
        }
        if (scen_ngvsb[0].ok) {
            cv.blit(scen_ngvsb[0], b.sx, b.sy, kAlphaFull, 0, 0, disp_w, disp_h);
        }
        if (scen_ngvsb[1].ok) {
            cv.blit(scen_ngvsb[1], b.sx,
                    b.sy + b.sh - static_cast<int>(scen_ngvsb[1].h), kAlphaFull,
                    0, 0, disp_w, disp_h);
        }
        if (scen_ngvsb[2].ok) {
            cv.blit_stretch(scen_ngvsb[2], b.sx + 1, thumb_y, b.sw - 2, thumb_h);
        } else {
            cv.fill(b.sx + 2, thumb_y, b.sw - 4, thumb_h, 40, 38, 32, 255);
        }
    }

    // One frame of the Scenario screen (TScenSetup) - see
    // docs/08_SCENARIO_SCREEN.md.
    Canvas render_scenario() const {
        Canvas cv(disp_w, disp_h);
        cv.clear(0, 0, 0);
        cv.blit(scen_bg, 0, 0, kAlphaFull, 0, 0, disp_w, disp_h);
        // The Scenario screen does NOT tint its backdrop: the shipped capture's
        // corner pixels are Backg03.BMP verbatim (e.g. (20,470) = 31 20 20).
        // Everything else on the screen is a panel drawn over it.

        // ScenarioLbl: the screen's own title. The DFM pins it at WinLeft 210
        // WinTop 8 with Alignment awCenter / ahTop (TopOffset 8) inside the
        // full-screen ScenSetupPnl, i.e. the caption is centred on the 640 px
        // display whatever its width; bin/RaceSelectorRef2.png's
        // "Scenario: Blood Isles" ink spans x 240..399 (160 px, centred on 320).
        // The empty selection still carries the colon + space, which is why
        // bin/RaceSelectorRef.png's "Scenario:" ink starts on 281 and not 284.
        draw_text(cv, disp_w / 2, 7, "Scenario: " + scen_map_title);

        // ---- render: Scenario screen drawing ----
        // FUN_00410f2c's fog term: kScenGlowFrames - t. A confirmed map maxes the
        // counter (select_map), so its lit cells carry no fog at all and the ones
        // whose race is not in the map stay fully fogged - the two states the
        // shipped captures show.
        const int t = scen_glow >= kScenGlowFrames ? kScenGlowFrames : scen_glow;

        // The 12 race cells, fill order R1,R5,R9,R2,... (the DFM's grid). Each
        // cell is a TAOWPanel (STGrFr frame, GenericF 72..79, over the ShadowOv
        // body, GenericT 1) whose `OnDrawBack = RacePnlDrawBack`
        // (0x00412c3c) blits the race crest at (WinLeft + 5, WinTop + 5):
        //
        //   ImageLib_TCustomImageLibrary_Get(<IntGfxMod.Shields>,
        //                                   panel->Tag,  // race index
        //                                   ...)->Show(WinLeft + 5, WinTop + 5)
        //
        // i.e. IntGfxMod.Shields (= Int\ShieldsM.ILB) id = the panel's Tag
        // (R1Pnl Tag 0, R5Pnl Tag 4, ... - the DFM's own grid order), not the
        // 50x46 R*PlayerIcon box the first pass drew it in.
        //
        // The contents follow the DFM's own children: R*Lbl inside R*CenterPnl
        // at (28,9), R*PlayerCB/R*BehaviorCB at (50,25)/(50,45), R*InfoLbl
        // ("Unavailable" - FUN_00410f2c's param_2 == 0 branch) which its
        // awCenter/ahBottom alignment pins to the middle of the 99 px caption
        // column, 7 px above the cell's bottom, and R*PlayerIcon (TAOWImage,
        // PlyrIcns, 50x46 at (0,20)) which FUN_00410f2c fills with
        // TAOWImage.SetCurrent(type -> 0..5).
        for (int gi = 0; gi < kScenRaceCount; ++gi) {
            int x, y;
            scen_cell(gi, x, y);
            const int race = kScenGridRace[gi];
            const bool avail = scen_available[race];
            // R*FogPnl stays fully applied on a race the setup control does not
            // use (FUN_00410f2c's param_2 == 0 branch); a race that *is* in the
            // game has its fog dropped outright. Measured against
            // bin/RaceSelectorRef.png: black at alpha 208 over the crest.
            const int fog = avail ? (kScenGlowFrames - t) * 208 / kScenGlowFrames
                                  : 208;
            blit_panel_body(cv, x, y, kScenCellW, kScenCellH);
            // R*PlayerIcon: the player-type icon (PlyrIcns TImageNode) in the
            // DFM's 50x46 box at (0,20). FUN_00410f2c shows it for the player
            // types 0..3 and 6/7 only (Independent / None have no icon).
            const int pt = scen_ptype[race];
            int icon_id = -1;
            if (avail && pt >= 0 && pt < 8 &&
                (pt < 4 || pt == 6 || pt == 7)) {
                switch (pt) {
                case 1: icon_id = 1; break;
                case 2: icon_id = 2; break;
                case 3: icon_id = 3; break;
                case 6: icon_id = 4; break;
                case 7: icon_id = 5; break;
                default: icon_id = 0; break;
                }
            }
            // The node's aura layer goes down first, under the race crest, and
            // the emblem on top of it: the shipped capture shows the crest's
            // lower tip untouched by the glow while the emblem overlaps it.
            if (icon_id >= 0 && scen_plyr[icon_id].count > 0) {
                cv.blit_keyed(scen_plyr[icon_id].layer[0], x, y + 20);
            }
            // RacePnlDrawBack: the race crest, ShieldsM id = race, drawn at the
            // panel's own +5,+5 origin.
            const Sprite& shield = scen_shield[race];
            if (shield.ok) {
                cv.blit_keyed(shield, x + kPanelTexInset, y + kPanelTexInset);
            }
            if (icon_id >= 0) {
                const LayeredSprite& pi = scen_plyr[icon_id];
                for (int L = 1; L < pi.count; ++L) {
                    const Sprite& sp = pi.layer[L];
                    if (sp.ok) cv.blit_keyed(sp, x, y + 20);
                }
            }
            blit9_impl(cv, scen_frame, x, y, kScenCellW, kScenCellH, 2, 2);
            // R*Lbl: Age10s at (48 + its own WinLeft, 9), tinted with the
            // race's own TextColor.
            text_at(cv, glyphs10, x + 48 + kScenRaceLblX[race], y + 9,
                    kScenRaceName[race], kScenRaceColor[race][0],
                    kScenRaceColor[race][1], kScenRaceColor[race][2], false,
                    false);
            if (avail) {
                // R*PlayerCB / R*BehaviorCB: TitleM's ScenEBut 3-piece
                // (ILEdit = TitleScreen, ILEIndex = 26), TextColor = the race's
                // own colour.
                draw_combo(cv, x + 50, y + 25, kScenComboW,
                           kScenPTypeName[pt < 8 ? pt : 5], glyphs8,
                           kScenRaceColor[race][0], kScenRaceColor[race][1],
                           kScenRaceColor[race][2]);
                // FUN_00410f2c shows R*BehaviorCB only when the host drives the
                // race and its type is a CPU one (1..3, 6, 7); a Human seat keeps
                // the player combo alone - exactly what bin/RaceSelectorRef2.png
                // shows for the Goblins cell ("Human", no "Normal" beneath it).
                if (pt == 1 || pt == 2 || pt == 3 || pt == 6 || pt == 7) {
                    draw_combo(cv, x + 50, y + 45, kScenComboW, "Normal",
                               glyphs8, kScenRaceColor[race][0],
                               kScenRaceColor[race][1],
                               kScenRaceColor[race][2]);
                }
            } else {
                // R*InfoLbl: AOWFont Age8s, TextColor = the race's own colour,
                // Alignment awCenter / ahBottom with BottomOffset 7 inside
                // R*CenterPnl (48,0 99x66) - i.e. centred on the cell's 99 px
                // caption column and 7 px above the cell's bottom edge. The
                // shipped capture puts its ink at cell + (72..120, 48..55).
                text_at(cv, glyphs8, x + 48 + 99 / 2, y + 48, "Unavailable",
                        kScenRaceColor[race][0], kScenRaceColor[race][1],
                        kScenRaceColor[race][2], true, false);
            }
            // The R*FogPnl overlay covers the whole cell, so an unlit/locked
            // cell greys out its icon and controls too.
            if (fog > 0) cv.fill(x, y, kScenCellW, kScenCellH, 0, 0, 0, fog);
        }

        // ---- ChatPnl (SidesPnl's bottom strip, DFM 0,286 484x145) -----------
        // The panel's own children (TSCENSETUP.dfm, all relative to ChatPnl):
        //   ChatMemo (10,12) 299x93   TAOWMemo, GenericF 64 frame + GenericT 4
        //   ChatEdit (10,114) 299x21  TAOWEdit, ILEdit TitleScreen ILIndex 23
        //                             (= ScenEd.BMP 2/48/2 x 21)
        //   ConnectionPnl (318,12) 166x133, no art, holding
        //     PlayerListSort (0,0) 71x15    "Players"     ScenCol 52/53/54
        //     AOWButton1    (70,0) 26x15    "Ping"        ScenCol 52/53/54
        //     PreferredRaceSort (95,0) 61x15 "Pref. Race" ScenCol 52/53/54
        //     PlayerList (0,15) 70x78, PingList (70,15) 25x78,
        //     PreferredRaceList (95,15) 61x78 - ScenFr frame + blank1 body.
        // ChatFogPnl (a 484x145 TAOWImage of GenericT 1, Priority 100) sits over
        // the lists while the header buttons carry priorities 300..500, which is
        // why the shipped screen's headers stay legible and its lists do not.
        {
            const int px = kChatX, py = kChatY;
            blit_panel_body(cv, px, py, kChatW, kChatH);
            blit9_impl(cv, scen_frame, px, py, kChatW, kChatH, 2, 2);
            // ChatMemo: ScenFr frame over the blank1 body.
            const int mx = px + kChatMemoX, my = py + kChatMemoY;
            if (scen_blank_tex.ok) {
                fill_tex(cv, scen_blank_tex, mx, my, kChatMemoW, kChatMemoH);
            }
            blit9_impl(cv, scen_scenfr, mx, my, kChatMemoW, kChatMemoH, 2, 2);
            // ChatEdit: the 21 px ScenEd 3-piece.
            blit_btn3(cv, scen_chated, px + kChatEditX, py + kChatEditY,
                      kChatEditW, kChatEditH);
            // ChatFogPnl (a 484x145 TAOWImage of GenericT 1, Priority 100) is the
            // fog the engine fades the chat area with; the shipped screen shows
            // its tile at the same ~9 % the panel body carries, so the body's
            // own masked fill already reproduces it.
            // ConnectionPnl: the three column headers over their list boxes.
            for (int i = 0; i < 3; ++i) {
                const int hx = px + kConnX + kConnColX[i];
                const int hy = py + kConnY;
                const int hw = kConnColW[i];
                blit_btn3(cv, scen_col, hx, hy, hw, kConnHdrH);
                text_at(cv, glyphs8, hx + hw / 2, hy + 2, kConnColText[i], 238,
                        238, 230, true, false);
                const int lx2 = hx, ly2 = hy + kConnListY;
                if (scen_blank_tex.ok) {
                    fill_tex(cv, scen_blank_tex, lx2, ly2, hw, kConnListH);
                }
                blit9_impl(cv, scen_scenfr, lx2, ly2, hw, kConnListH, 2, 2);
            }
            // The chat strip is shown dimmed: measured against
            // bin/RaceSelectorRef2.png every element inside ChatPnl sits at ~24 %
            // of its own colour (the "Players" caption's (238,238,230) reads
            // (57,57,49), the ScenFr frames' (82,80,90) read below 30), i.e. the
            // ChatFogPnl (GenericT 1, Priority 100) lays a black veil at ~194
            // over the whole panel - headers included.
            cv.fill(px, py, kChatW, kChatH, 0, 0, 0, kChatFogAlpha);
        }

        // The Settings panel (SettingsPnl) is a TAOWPanel too (STGrFr frame over
        // the ShadowOv body), so draw_panel replaces the flat fills. Every child
        // below is the DFM's own, with its declared font and colour:
        //   TurnLbl "Turns " (10,10) Age8s clSilver
        //   PlayModeCB (10,25) 114 wide (Age8)
        //   DurationLbl "Duration" (10,55) Age8s clSilver
        //   DurFr (10,70) 114x65 = GenericF ScenFr 64 (TAoWFrame)
        //   TurnTimeLbl/TurnsLbl (15,75)/(15,105) Age8s clGray
        //   TurnTimeSl/TurnsSl (15,87)/(15,116) 50x11 GenericS ScenSlid
        //   TurnTimeVal/TurnsVal (70,87)/(70,117) Age8s clGray
        //   MiscLbl "Miscellaneous" (10,142) Age8s clSilver
        //   LFr (10,155) 114x70 = GenericF ScenFr 64 (TAoWFrame)
        //   the four 12x12 ScenChk checkboxes + their Age8s clGray labels.
        draw_setpanel(cv, kScenSetX, kScenSetY, kScenSetW, kScenSetH);
        const int sx = kScenSetX, sy = kScenSetY;
        text_at(cv, glyphs8, sx + 10, sy + 10, "Turns ", 192, 192, 192, false,
                false);
        draw_setcombo(cv, sx + 10, sy + 25, 114, "Simultaneous", glyphs8, 200,
                      200, 195);
        text_at(cv, glyphs8, sx + 10, sy + 55, "Duration", 192, 192, 192, false,
                false);
        draw_group(cv, sx + 10, sy + 70, 114, 65);
        text_at(cv, glyphs8, sx + 15, sy + 75, "Turn Time", 128, 128, 128, false,
                false);
        draw_slider(cv, sx + 15, sy + 87, 50, 0);
        text_at(cv, glyphs8, sx + 70, sy + 87, "no timer", 128, 128, 128, false,
                false);
        text_at(cv, glyphs8, sx + 15, sy + 105, "Turns", 128, 128, 128, false,
                false);
        draw_slider(cv, sx + 15, sy + 116, 50, 0);
        text_at(cv, glyphs8, sx + 70, sy + 117, "no limit", 128, 128, 128, false,
                false);
        text_at(cv, glyphs8, sx + 10, sy + 142, "Miscellaneous", 192, 192, 192,
                false, false);
        draw_group(cv, sx + 10, sy + 155, 114, 70);
        draw_check(cv, sx + 15, sy + 162, scen_check[0]);
        text_at(cv, glyphs8, sx + 30, sy + 164, "Leaders on map", 128, 128, 128,
                false, false);
        draw_check(cv, sx + 15, sy + 177, scen_check[1]);
        text_at(cv, glyphs8, sx + 30, sy + 179, "Customize leaders", 128, 128,
                128, false, false);
        draw_check(cv, sx + 15, sy + 192, scen_check[2]);
        text_at(cv, glyphs8, sx + 30, sy + 194, "Allied victory", 128, 128, 128,
                false, false);
        draw_check(cv, sx + 15, sy + 207, scen_check[3]);
        text_at(cv, glyphs8, sx + 30, sy + 209, "Exploration", 128, 128, 128,
                false, false);

        // The three bottom buttons (PickScenarioBtn / ExitGame / StartBtn) are
        // TAOWButton with ILButton = IntGfxMod.TitleScreen, ILIndexUp = 32,
        // ILIndexDown = 35, ILIndexLit = 38 - the ScenRBut.BMP 3-piece art, not
        // the celtic dialog buttons. The captions use FontModule.Age10s with
        // TextColor 15134446 (RGB 238,238,230) and TextSelColor clWhite.
        for (int i = 0; i < 3; ++i) {
            // StartBtn only exists once a map is confirmed (the shipped screen
            // shows Scenario/Exit alone until the picker's Select).
            if (i == 2 && !scen_map_confirmed) continue;
            const int by = kScenBtnY0 + i * kScenBtnPitch;
            const int st = (i == scen_hover) ? (scen_pressed == i ? 1 : 2) : 0;
            draw_main_btn(cv, kScenBtnX, by, kScenBtnW, st, kScenBtnText[i]);
        }

        // The "Select a Map..." dialog (TFileDlgWindow): the ornate DialogFr
        // 9-slice over the MainFloaterSet body texture, the Drive/Path rows, the
        // dark file list with a gold frame, the size/towns/players strip and the
        // green celtic Select/Cancel buttons.
        if (scen_maps_open) {
            const int dx = 130, dy = 35, dw = 380, dh = 363;
            // TFileDlgWindow is a modal floater and its window manager draws a
            // large solid shadow under it: the shipped capture is pure black
            // from x 10..490, y 396..466 where Backg03 is maroon. TODO(RE): pin
            // the shadow sprite/geometry from AoW.exe's TAOWWindow path.
            cv.fill(10, 396, 480, 70, 0, 0, 0, 255);
            // The floater's body (MainFloaterSet, GenericT 2) fills the same
            // inset rect the frame uses.
            const int bx = dx + 13, by = dy, bw = dw - 26, bh = dh;
            if (scen_body_tex.ok) {
                for (int yy = 0; yy < bh; yy += static_cast<int>(scen_body_tex.h)) {
                    for (int xx = 0; xx < bw;
                         xx += static_cast<int>(scen_body_tex.w)) {
                        cv.blit(scen_body_tex, bx + xx, by + yy, 130, bx, by,
                                bx + bw, by + bh);
                    }
                }
            } else {
                cv.fill(bx, by, bw, bh, 46, 40, 28, 255);
            }
            blit_dlg(cv, dx, dy, dw, dh);

            // The title bar is the DialogFr top edge (GenericF 33): a gold bar
            // carrying the small WinTIcon (id 8) and the title text (TAOWLabel
            // Title, TextColor = clBlack, AOWFont = FontModule.Age10, WinLeft 42
            // WinTop 7).
            if (scen_dlg_title_icon.ok) {
                cv.blit_keyed(scen_dlg_title_icon, dx + 17, dy + 5);
            }
            text_at(cv, glyphs10b, dx + 42, dy + 6, "Select a Map...", 24, 24, 24,
                    false, false);
            // Close (TAOWButton Close, WinLeft 345 WinTop 4, ILIcon GenericI
            // 0/1/2): the shipped 15x16 MainHeaderBut round button with its X.
            if (scen_close[0].ok) {
                cv.blit_keyed(scen_close[0], dx + 345, dy + 4);
            }

            // TCeltL / TCelt: the celtic band directly under the title bar - a
            // flat frHorizLine strip (CeltDeco 55/56/57) with the 27x27 knot
            // (CeltDeco 3) tiled over it at (28,22), 322x27.
            blit_celt_band(cv, dx + 21, dy + 22, 338);
            blit_celt_tiles(cv, dx + 28, dy + 22, 322, 27);

            // Drive / Path rows (TopPnl): DriveFr (36,58) 100x21, DriveCmb
            // (135,58) 210x21, PathFr (36,78) 70x21 and PathEdit (105,78) 208x21,
            // all drawn with the dark NGEdit / NGCmb skin (GenericE 15..17 /
            // 21..23) and TextColor 11002593 (RGB 225,226,167). The DriveL /
            // PathL captions are Age10 in TextColor 9415072 (RGB 160,169,143).
            draw_edit(cv, dx + 36, dy + 58, 100, "Drive", glyphs10b, 160, 169, 143);
            draw_combo(cv, dx + 135, dy + 58, 210, "(C:\\)", glyphs10b, 225, 226,
                       167);
            draw_edit(cv, dx + 36, dy + 78, 70, "Path", glyphs10b, 160, 169, 143);
            draw_edit(cv, dx + 105, dy + 78, 208,
                      "Age of Wonders\\Scenario\\Custom\\", glyphs10b, 225, 226,
                      167);
            // UpDirBtn (313,78) 32x21: GenericI's NGButDirBack 56/57/58
            // (ILIndexUp / Down / Lit), not a hand-drawn arrow.
            if (scen_updir[0].ok) {
                cv.blit_keyed(scen_updir[0], dx + 313, dy + 78);
            } else {
                blit_gold(cv, dx + 313, dy + 78, 32, 21);
            }

            // The file list (FileLB): a TAOWListBox (ILFrame GenericF 113 =
            // FrGoldD) over the TextOld1 body, AOWFont Age10, TextColor 9415072
            // and TextSelColor clBlack. The picker opens with nothing selected,
            // so the info strip below stays blank.
            const int lx = dx + 35, ly = dy + 109, lw = 310, lh = 141;
            const int sbw = 17, row_w = lw - sbw - 2;
            if (scen_list_tex.ok) {
                for (int yy = 0; yy < lh; yy += static_cast<int>(scen_list_tex.h)) {
                    for (int xx = 0; xx < lw;
                         xx += static_cast<int>(scen_list_tex.w)) {
                        cv.blit(scen_list_tex, lx + xx, ly + yy, kAlphaFull, lx, ly,
                                lx + lw, ly + lh);
                    }
                }
            } else {
                cv.fill(lx, ly, lw, lh, 24, 28, 16, 255);
            }
            for (int vis = 0; vis < kScenListRows; ++vis) {
                const int r = scen_map_off + vis;
                if (r < 0 || r >= static_cast<int>(scen_maps.size())) break;
                // ItemHeight 17 (measured from the capture's row pitch: text
                // tops at 165, 182, 199, 216, ... ).
                const int ry = ly + 5 + vis * 17;
                const std::string& name = scen_maps[static_cast<std::size_t>(r)];
                if (r == scen_map_sel) {
                    if (scen_list_sel_tex.ok) {
                        for (int yy = 0; yy < 17;
                             yy += static_cast<int>(scen_list_sel_tex.h)) {
                            for (int xx = 0; xx < row_w;
                                 xx += static_cast<int>(scen_list_sel_tex.w)) {
                                cv.blit(scen_list_sel_tex, lx + 2 + xx, ry + yy,
                                        kAlphaFull, lx, ry, lx + row_w, ry + 17);
                            }
                        }
                    }
                    text_at(cv, glyphs10b, lx + 25, ry + 1, name, 0, 0, 0, false,
                            false);
                } else {
                    text_at(cv, glyphs10b, lx + 25, ry + 1, name, 160, 169, 143,
                            false, false);
                }
            }
            blit_thin(cv, lx, ly, lw, lh);
            // The scrollbar (FileLB -> NSBar): hidden while the whole list fits
            // (the DFM's VanishWhenFull), otherwise the thumb tracks
            // scen_map_off.
            const ScenSb sb = scen_sb();
            if (sb.max_off > 0) {
                draw_vsb(cv, sb, scen_sb_thumb_y(sb), sb.thumb_h);
            }

            // The info strip: LSavedInfo (a 310x60 FrGoldD well) with the three
            // NewInfo1..3 wells inside it, each over TextOld1 with its icon and
            // its Age10 value in TextColor 9415072. With nothing selected the
            // values stay blank, exactly as the shipped screen shows.
            const int iy = dy + 259, ih = 60;
            // LSavedInfo is a TAOWPanel: its FrGoldD frame over the TextOld1
            // body (the DFM names ILIndexFrame 113 / ILIndexTexture 17), which is
            // what makes the strip read dark green and not the dialog's own body.
            // The shipped strip's body colours match TextOld1 (GenericT 17),
            // not the darker TextOld2 the DFM names for the save-mode panel.
            if (scen_info_tex.ok) {
                for (int yy = 0; yy < ih; yy += static_cast<int>(scen_info_tex.h)) {
                    for (int xx = 0; xx < lw;
                         xx += static_cast<int>(scen_info_tex.w)) {
                        cv.blit(scen_info_tex, lx + xx, iy + yy, kAlphaFull, lx,
                                iy, lx + lw, iy + ih);
                    }
                }
            }
            blit_thin(cv, lx, iy, lw, ih);
            const int wz = dy + 277, wh = 39;
            // NewInfo1 105 px / NewInfo2 103 / NewInfo3 103, at x 1 / 105 / 207.
            static const int kWellX[3] = {1, 105, 207};
            static const int kWellW[3] = {105, 103, 103};
            const LayeredSprite* iics[3] = {&scen_icon_size, &scen_icon_towns,
                                            &scen_icon_players};
            const std::string iv[3] = {scen_map_size,
                                       std::to_string(scen_map_towns),
                                       std::to_string(scen_map_players)};
            for (int c = 0; c < 3; ++c) {
                const int ww = kWellW[c];
                const int cx0 = lx + kWellX[c];
                if (scen_info_tex.ok) {
                    for (int yy = 0; yy < wh;
                         yy += static_cast<int>(scen_info_tex.h)) {
                        for (int xx = 0; xx < ww;
                             xx += static_cast<int>(scen_info_tex.w)) {
                            cv.blit(scen_info_tex, cx0 + xx, wz + yy, kAlphaFull,
                                    cx0, wz, cx0 + ww, wz + wh);
                        }
                    }
                }
                blit_gold(cv, cx0, wz, ww, wh);
                // The capture places each glow slightly left of the well's
                // geometric centre (measured brightness centroids: well 1
                // 202 vs 213, well 2 304 vs 316, well 3 409 vs 412). TImageNode.
                // Show blits every layer in order: the soft glow first, then the
                // icon art (Ilpack.dpl TImageNode.Show@0x5520d654).
                static const int kIconDx[3] = {0, 3, 0};
                static const int kIconDy[3] = {-9, -11, -2};
                // The node's layers are stored glow-first, but the shipped
                // screen shows the glow *over* the art (the castle is tinted
                // gold, the face lifted out of its shadow), so draw the art
                // layers first and add the glow layer last.
                for (int L = 0; L < iics[c]->count; ++L) {
                    const Sprite& sp = iics[c]->layer[L];
                    if (!sp.ok) continue;
                    // TImageNode.Show blits the node's layers in order: the
                    // soft glow (additive over the dark well) and then the art,
                    // colour-keyed. That reproduces the shipped globe exactly
                    // (ref centre 57,99,99; 41,90,41; 107,115,214).
                    if (L == 0) {
                        cv.blit_add(sp, cx0 + kIconDx[c], wz + kIconDy[c]);
                    } else {
                        cv.blit_keyed(sp, cx0 + kIconDx[c], wz + kIconDy[c]);
                    }
                }
                if (scen_map_sel >= 0) {
                    text_at(cv, glyphs10b, cx0 + ww - 16, wz + 14, iv[c], 160, 169,
                            143, true, false);
                }
            }

            // BCeltL again: the celtic button band with its knot crosses and the
            // two ButRCelt2 buttons (Openbut "Select", CancelBut "Cancel"), both
            // TextColor = clBlack.
            blit_celt_band(cv, dx + 21, dy + 326, 338);
            blit_celt_knot(cv, dx + 27, dy + 326);
            blit_celt_knot(cv, dx + 177, dy + 326);
            blit_celt_knot(cv, dx + 326, dy + 326);
            blit_celt_btn(cv, dx + 53, dy + 326, 122, 0);
            blit_celt_btn(cv, dx + 205, dy + 326, 122, 0);
            text_at(cv, glyphs10b, dx + 53 + 61, dy + 326 + 9, "Select", 0, 0, 0,
                    true, false);
            text_at(cv, glyphs10b, dx + 205 + 61, dy + 326 + 9, "Cancel", 0, 0, 0,
                    true, false);
        }

        // The open race PlayerCB drop-down, on the very top (it overlaps the
        // rows below it, exactly like an opened TAOWComboBox list).
        if (scen_dd_gi >= 0) draw_pc_drop(cv, scen_dd_gi);

        if (cursor_on && cursor.ok) cv.blit_keyed(cursor, mouse_x, mouse_y);
        return cv;
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

    // Advance of one character: the glyph's slot width plus the font's
    // CharacterDistance; a missing glyph advances CharacterDistance + 5, which
    // is what TImageLibraryFont.GetTextWidth does in Ilpack.dpl.
    int advance_of(unsigned char c) const {
        const Glyph& g = glyphs[c];
        if (!g.ok) return kFontTracking + 5;
        return static_cast<int>(g.face.advance) + kFontTracking;
    }

    int text_width(const std::string& s) const {
        int total = 0;
        for (unsigned char c : s) total += advance_of(c);
        return total;
    }

    // Draws `s` centred on `cx`, its cap line at `y`. Each glyph is one
    // blit of the selected colour variant - the engine's font routines
    // (Ilpack.dpl TImageLibraryFont) draw a single image per character, with
    // the background colour of that variant keyed out.
    void draw_text(Canvas& cv, int cx, int y, const std::string& s) const {
        int pen = cx - text_width(s) / 2;
        const int py = y - kWhgCapShift;  // y is the cap line, not the cell top
        for (unsigned char c : s) {
            const Glyph& g = glyphs[c];
            if (g.ok) cv.blit_keyed(g.face, pen, py);
            pen += advance_of(c);
        }
    }

    // Same, for the small FontModule.Age8s face used by the GameLbl.
    int advance8_of(unsigned char c) const {
        const Glyph& g = glyphs8[c];
        if (!g.ok) return kFontTracking + 5;
        return static_cast<int>(g.face.advance) + kFontTracking;
    }

    int text_width8(const std::string& s) const {
        int total = 0;
        for (unsigned char c : s) total += advance8_of(c);
        return total;
    }

    void draw_text8(Canvas& cv, int cx, int y, const std::string& s) const {
        int pen = cx - text_width8(s) / 2;
        for (unsigned char c : s) {
            const Glyph& g = glyphs8[c];
            if (g.ok) {
                cv.glyph_ink(g.face, pen, y, kGtLabelR, kGtLabelG, kGtLabelB);
            }
            pen += advance8_of(c);
        }
    }

    // The same face, left-anchored and in an explicit colour - the F1 frames-
    // per-second read-out. glyph_ink paints the ink in (r,g,b) and the glyph
    // box's palette entry 0 black, which is what lets the read-out sit on a
    // black plate with no seams (see draw_fps_overlay).
    void draw_text8_left(Canvas& cv, int x, int y, const std::string& s,
                         std::uint8_t r, std::uint8_t g, std::uint8_t b) const {
        int pen = x;
        for (unsigned char c : s) {
            const Glyph& gl = glyphs8[c];
            if (gl.ok) cv.glyph_ink(gl.face, pen, y, r, g, b);
            pen += advance8_of(c);
        }
    }

    // --- Scenario screen (TScenSetup) ------------------------------------
    int advance10_of(unsigned char c) const {
        const Glyph& g = glyphs10[c];
        if (!g.ok) return kFontTracking + 5;
        return static_cast<int>(g.face.advance) + kFontTracking;
    }
    int text_width10(const std::string& s) const {
        int total = 0;
        for (unsigned char c : s) total += advance10_of(c);
        return total;
    }
    void draw_text10(Canvas& cv, int cx, int y, const std::string& s) const {
        int pen = cx - text_width10(s) / 2;
        for (unsigned char c : s) {
            const Glyph& g = glyphs10[c];
            if (g.ok) cv.blit_keyed(g.face, pen, y);
            pen += advance10_of(c);
        }
    }
    void draw_text10_left(Canvas& cv, int x, int y, const std::string& s) const {
        int pen = x;
        for (unsigned char c : s) {
            const Glyph& g = glyphs10[c];
            if (g.ok) cv.blit_keyed(g.face, pen, y);
            pen += advance10_of(c);
        }
    }
    // Draw glyphs in an explicit colour (TImageLibraryFont.SetColor). The
    // Age10s glyph coverage is tinted, so text reads on any background.
    void draw_text10_c(Canvas& cv, int x, int y, const std::string& s,
                       std::uint8_t r, std::uint8_t g, std::uint8_t b,
                       bool center) const {
        int pen = center ? x - text_width10(s) / 2 : x;
        for (unsigned char c : s) {
            const Glyph& gl = glyphs10[c];
            if (gl.ok) cv.glyph(gl.face, pen, y, r, g, b);
            pen += advance10_of(c);
        }
    }

    // Generic bitmap-font blit over an arbitrary glyph set (FontModule.Age8s /
    // Age10s / Age10). `center` centres on x, `keyed` draws the glyph's own
    // pre-shaded ink instead of tinting its coverage with (r,g,b) - the same
    // choice TImageLibraryFont makes between its stored layers and SetColor.
    // TFontModule.CharacterDistance per face (AoW.exe TFontModule @0x0040383c):
    // Age10 / Age10s / Age8 advance one extra pixel per glyph, Age8s and the
    // AoW15WhiteGrey shell face do not. TImageLibraryFont.GetTextWidth / SetText
    // add this distance to the image's +0x20 width for every character.
    int font_tracking(const std::vector<Glyph>& fs) const {
        if (&fs == &glyphs10b || &fs == &glyphs10 || &fs == &glyphs8b) return 1;
        return kFontTracking;
    }
    int glyph_advance(const std::vector<Glyph>& fs, unsigned char c) const {
        const Glyph& g = fs[c];
        const int tr = font_tracking(fs);
        return g.ok ? static_cast<int>(g.face.advance) + tr : 5 + tr;
    }
    int text_w(const std::vector<Glyph>& fs, const std::string& s) const {
        int t = 0;
        for (unsigned char c : s) t += glyph_advance(fs, c);
        return t;
    }
    // `tracking` overrides the face's CharacterDistance for this run (-1 keeps
    // the font default). The dialog title is drawn without the extra pixel the
    // list/edits use - matching the shipped 95 px "Select a Map..." run.
    void text_at(Canvas& cv, const std::vector<Glyph>& fs, int x, int y,
                 const std::string& s, std::uint8_t r, std::uint8_t g,
                 std::uint8_t b, bool center, bool keyed,
                 int tracking = -1) const {
        int pen = center ? x - text_w(fs, s) / 2 : x;
        for (unsigned char c : s) {
            const Glyph& gl = fs[c];
            if (gl.ok) {
                if (keyed) {
                    cv.blit_keyed(gl.face, pen, y);
                } else {
                    cv.glyph(gl.face, pen, y, r, g, b);
                }
            }
            int adv = glyph_advance(fs, c);
            if (tracking >= 0) adv += tracking - font_tracking(fs);
            pen += adv;
        }
    }

    // One gold / mana read-out: the stockpile total (FontModule.TBarNumber,
    // clBlack) right-aligned rox 23 on the panel, the "+" sign (Age8) at rox 16
    // and the per-turn income (Age8) centred in the 16 px box inset rox 1. Both
    // the sign and the income use the DFM's green TextColor 0x77FF77.
    void draw_readout(Canvas& cv, const AowBox& p, const std::string& total,
                      const std::string& inc) const {
        const int tre = p.x + p.w - kTbResTotRox;
        text_at(cv, glyphsTb, tre - text_w(glyphsTb, total), p.y + kTbResTotTop,
                total, 0, 0, 0, false, false);
        const int sre = p.x + p.w - kTbResSignRox;
        text_at(cv, glyphs8b, sre - text_w(glyphs8b, "+"), p.y + kTbResIncTop,
                "+", kTbIncomeR, kTbIncomeG, kTbIncomeB, false, false);
        const int ice = p.x + p.w - kTbResIncRox - kTbResIncW / 2;
        text_at(cv, glyphs8b, ice, p.y + kTbResIncTop, inc, kTbIncomeR,
                kTbIncomeG, kTbIncomeB, true, false);
    }

    // A 9-slice frame. `fr` is TL,T,TR,L,R,BL,B,BR; `cx`/`cy` are the corner
    // sizes (the edge sprites' thickness). Corners are blitted at natural size,
    // edges are stretched between them.
    // The engine does NOT stretch a 9-slice edge: AOWWinManager / TAOWPanel
    // blit the edge sprite repeatedly at its natural size between the corners
    // (TAOWPanel.Draw@0x1f990 tiles the 50x2 / 2x50 pieces), and the window
    // manager follows the same path. Stretching distorts the woven DialogFr
    // and celtic bands, so tile them with the 9-slice rect as the clip.
    void blit9_impl(Canvas& cv, const Sprite* fr, int x, int y, int w, int h,
                    int /*cx*/, int /*cy*/) const {
        if (w <= 0 || h <= 0) return;
        const int l = x, t = y, r = x + w, b = y + h;
        auto put = [&](const Sprite& s, int dx, int dy) {
            if (s.ok) cv.blit(s, dx, dy, kAlphaFull, l, t, r, b);
        };
        if (fr[0].ok) put(fr[0], l, t);
        if (fr[2].ok) put(fr[2], r - static_cast<int>(fr[2].w), t);
        if (fr[5].ok) put(fr[5], l, b - static_cast<int>(fr[5].h));
        if (fr[7].ok) {
            put(fr[7], r - static_cast<int>(fr[7].w),
                b - static_cast<int>(fr[7].h));
        }
        const int top_end = r - (fr[2].ok ? static_cast<int>(fr[2].w) : 0);
        if (fr[1].ok && fr[1].w) {
            for (int xx = l + static_cast<int>(fr[0].w); xx < top_end;
                 xx += static_cast<int>(fr[1].w)) {
                put(fr[1], xx, t);
            }
        }
        const int bot_y = b - (fr[6].ok ? static_cast<int>(fr[6].h) : 0);
        if (fr[6].ok && fr[6].w) {
            for (int xx = l + static_cast<int>(fr[0].w); xx < top_end;
                 xx += static_cast<int>(fr[6].w)) {
                put(fr[6], xx, bot_y);
            }
        }
        const int side_end = b - (fr[5].ok ? static_cast<int>(fr[5].h) : 0);
        if (fr[3].ok && fr[3].h) {
            for (int yy = t + static_cast<int>(fr[0].h); yy < side_end;
                 yy += static_cast<int>(fr[3].h)) {
                put(fr[3], l, yy);
            }
        }
        if (fr[4].ok && fr[4].h) {
            const int rx = r - static_cast<int>(fr[4].w);
            for (int yy = t + static_cast<int>(fr[0].h); yy < side_end;
                 yy += static_cast<int>(fr[4].h)) {
                put(fr[4], rx, yy);
            }
        }
    }
    void blit9(Canvas& cv, int x, int y, int w, int h) const {
        blit9_impl(cv, scen_frame, x, y, w, h, 1, 1);
    }
    void blit_dlg(Canvas& cv, int x, int y, int w, int h) const {
        // The shipped floater draws its DialogFr frame inset ~13 px from the
        // client rect: for the 380-wide window at x 130 the capture's frame
        // spans x 143..492, while the title bar and the child controls keep
        // their own (un-inset) positions.
        blit9_impl(cv, scen_dlg_frame, x + 13, y, w - 26, h, 22, 22);
    }
    void blit_gold(Canvas& cv, int x, int y, int w, int h) const {
        blit9_impl(cv, scen_gold_frame, x, y, w, h, 3, 3);
    }
    // NewLBFr / NewInfo / LSavedInfo name GenericF 105 (FrThinD), a 2px frame.
    void blit_thin(Canvas& cv, int x, int y, int w, int h) const {
        blit9_impl(cv, scen_thin_frame, x, y, w, h, 2, 2);
    }
    // The green celtic button (ButRCelt2): left cap + stretched centre + right
    // cap, `st` = 0 up / 1 lit / 2 down.
    void blit_celt_btn(Canvas& cv, int x, int y, int w, int st) const {
        const Sprite* b = scen_dlg_btn[st];
        if (!b[1].ok) return;
        const int lw = static_cast<int>(b[0].w), rw = static_cast<int>(b[2].w);
        const int bh = static_cast<int>(b[1].h);
        cv.blit(b[0], x, y, kAlphaFull, 0, 0, disp_w, disp_h);
        cv.blit_stretch(b[1], x + lw, y, w - lw - rw, bh);
        cv.blit(b[2], x + w - rw, y, kAlphaFull, 0, 0, disp_w, disp_h);
    }

    // Grid cell (fill order 0..11) top-left, per the DFM.
    static void scen_cell(int gi, int& x, int& y) {
        const int col = gi % 3, row = gi / 3;
        x = kScenGridX + col * kScenColDx;
        y = kScenGridY + row * kScenRowDy;
    }

    // Load a `.HSM` and fill the info strip (size / towns / players). With
    // `confirm` set (the dialog's Select), also light the races the map's
    // player list contains - FUN_00411460's per-race pass.
    void select_map(const std::string& name, bool confirm = false) {
        scen_map_name = name;
        aow::Map m;
        std::string err;
        const fs::path p = fs::path(root_dir) / "Scenario" / "Custom" / name;
        if (!aow::load_map(p.string(), m, err)) return;
        int towns = 0;
        std::vector<int> races;
        for (const aow::MapLevel& lv : m.levels) {
            for (const aow::MapTile& t : lv.tiles) {
                for (const aow::MapHs& o : t.objects) {
                    if (o.class_id == aow::kCityClassId) {
                        ++towns;
                        if (o.race >= 0) {
                            bool seen = false;
                            for (int r : races) if (r == o.race) seen = true;
                            if (!seen) races.push_back(o.race);
                        }
                    }
                }
            }
        }
        const std::uint32_t md = std::max(m.width, m.height);
        scen_map_size = md <= 48 ? "S" : (md <= 68 ? "M" : "L");
        scen_map_towns = towns;
        // The map's own roster (TAoWHSMap tag 0x15 -> TPlayerList -> TPlayer):
        // each entry's rw tag 0x2 is its race and rw tag 0x3 its player type.
        // This is exactly the list FUN_00411460's per-race pass consumes.
        aow::GameState gs;
        std::string gerr;
        const bool have_roster =
            aow::load_game_state(m, gs, gerr) && !gs.players.empty();
        scen_map_players = have_roster ? gs.player_count()
                                       : static_cast<int>(races.size());
        // FUN_00411460 lights exactly the races the setup control's player list
        // holds; a race with no player shows "Unavailable" (FUN_00410f2c's
        // param_2 == 0 branch) and the lit race's R*PlayerCB shows the player
        // type the map stored for it. Only a confirmed map lights them, since
        // highlighting a file in the picker just fills the info strip.
        if (confirm) {
            // The ScenarioLbl caption + the Start! button: the shipped screen
            // shows the chosen map's name in the title and reveals Start! only
            // once one is confirmed (bin/RaceSelectorRef.png has two buttons,
            // bin/RaceSelectorRef2.png three). FUN_00411460's per-race pass also
            // *drops* every lit cell's R*FogPnl at once - there is no fade - so
            // the counter is simply maxed out here.
            scen_map_confirmed = true;
            scen_glow = kScenGlowFrames;
            const std::size_t dot = name.find_last_of('.');
            scen_map_title = dot == std::string::npos ? name
                                                      : name.substr(0, dot);
            for (int r = 0; r < kScenRaceCount; ++r) scen_available[r] = false;
            if (have_roster) {
                // TSetupControl.hPickMap (0x557e20c8): one TSetupSettings record
                // per map player holds the race (+0x18), the player index (+0x20),
                // the map's own type - the *default* (+0x10) - and the current
                // type (+0x8). A map may mark several slots Human; hPickMap's
                // local_5 pass leaves only the FIRST one Human and turns every
                // later one into AI1 (CPU Squire). SetupMap (0x557e0b64) later
                // copies the current type back into the roster's TPlayer+0xa7.
                bool seen_human = false;
                for (const aow::Player& p : gs.players) {
                    if (p.index == 0) continue;  // entry 0 = the independents
                    const int r = static_cast<int>(p.race);
                    if (r >= kScenRaceCount) continue;
                    scen_available[r] = true;
                    const int raw = static_cast<int>(p.type);
                    scen_ptype_default[r] = raw;
                    int t = raw;
                    if (t == static_cast<int>(aow::PlayerType::Human)) {
                        if (seen_human) {
                            t = static_cast<int>(aow::PlayerType::AI1);
                        } else {
                            seen_human = true;
                        }
                    }
                    scen_ptype[r] = t;
                }
            } else {
                for (int r = 0; r < kScenRaceCount; ++r) {
                    for (int have : races) {
                        if (have == r) scen_available[r] = true;
                    }
                }
            }
            scen_dd_gi = -1;      // a fresh map closes any open drop-down
            scen_dd_hover = -1;
            if (have_roster) {
                std::printf("scenario: %s roster", name.c_str());
                for (const aow::Player& p : gs.players) {
                    if (p.index == 0) continue;
                    const int r = static_cast<int>(p.race);
                    if (r >= kScenRaceCount) continue;
                    const int t = scen_ptype[r];
                    std::printf(" [%s=%s]", kScenRaceName[r],
                                kScenPTypeName[t < 8 ? t : 5]);
                }
                std::printf("\n");
                std::fflush(stdout);
            }
        }
    }

    // The race cell's PlayerCB: RacePlayerCBChange (AoW.exe 0x00412a24) resolves
    // PlayerNameToPlayerID, then `TSetupSettings.IndexOfRace` picks the settings
    // record of the cell's race and `TSetupControl.hSetPlayerID` (0x557e1eb4)
    // stores the new type. When the new type is Human (0) it first calls
    // `ResetPlayerSettings` (0x557e1378) on every other current Human: that puts
    // the slot back to the map's default type, except a default-human slot, which
    // becomes CPU1 (AI1). Net effect: exactly one race is the Human.
    void scen_choose_type(int race, int type) {
        if (race < 0 || race >= kScenRaceCount) return;
        if (type == static_cast<int>(aow::PlayerType::Human)) {
            for (int r = 0; r < kScenRaceCount; ++r) {
                if (r == race || !scen_available[r]) continue;
                scen_ptype[r] =
                    (scen_ptype_default[r] ==
                     static_cast<int>(aow::PlayerType::Human))
                        ? static_cast<int>(aow::PlayerType::AI1)
                        : scen_ptype_default[r];
            }
        }
        scen_ptype[race] = type;
    }

    // Scenario hit test: 0..2 = the three buttons; negative = a map-list row
    // (encoded -1000 - row) or a scrollbar part (see below); -1 = none.
    int scen_hit(int x, int y) const {
        // An open race PlayerCB drop-down swallows the click first: its 7 rows
        // select a type, anything else just closes it.
        if (scen_dd_gi >= 0) {
            int lx, ly, lw;
            scen_dd_rect(scen_dd_gi, lx, ly, lw);
            const int lh = kScenPTypeCount * kScenItemH;
            if (lx <= x && x < lx + lw && ly <= y && y < ly + lh) {
                return -4100 - (y - ly) / kScenItemH;
            }
            return -4200;  // a click-away closes the list
        }
        for (int i = 0; i < 3; ++i) {
            if (i == 2 && !scen_map_confirmed) continue;  // Start! hidden
            const int by = kScenBtnY0 + i * kScenBtnPitch;
            if (kScenBtnX <= x && x < kScenBtnX + kScenBtnW && by <= y &&
                y < by + kScenBtnH) {
                return i;
            }
        }
        if (scen_maps_open) {
            const ScenSb b = scen_sb();
            // The rows, matching the render's geometry (first row at ly+4,
            // ItemHeight 17, kScenListRows visible, offset by scen_map_off): the
            // click now lands on the row the cursor is actually over.
            const int row_w = b.lw - b.sw - 2;
            for (int vis = 0; vis < kScenListRows; ++vis) {
                const int r = scen_map_off + vis;
                if (r < 0 || r >= static_cast<int>(scen_maps.size())) break;
                const int ry = b.ly + 4 + vis * 17;
                if (b.lx <= x && x < b.lx + row_w && ry <= y && y < ry + 17) {
                    return -1000 - r;
                }
            }
            // The scrollbar (NSBar): dec/inc arrows, the track pages and the
            // draggable thumb. -3000 up / -3001 down / -3002 page up /
            // -3003 page down / -3004 thumb.
            if (b.max_off > 0 && b.sx <= x && x < b.sx + b.sw &&
                b.sy <= y && y < b.sy + b.sh) {
                if (y < b.ty0) return -3000;
                if (y >= b.sy + b.sh - b.cap) return -3001;
                const int ty = scen_sb_thumb_y(b);
                if (ty <= y && y < ty + b.thumb_h) return -3004;
                return y < ty ? -3002 : -3003;
            }
            // Select / Cancel buttons.
            const int by = 35 + 322;
            if (130 + 53 <= x && x < 130 + 53 + 122 && by <= y && y < by + 27) {
                return -2000;
            }
            if (130 + 205 <= x && x < 130 + 205 + 122 && by <= y &&
                y < by + 27) {
                return -2001;
            }
        }
        // The R*PlayerCB fields of the available race cells (the closed combo is
        // clicked to open its drop-down). Not while the modal map picker is up.
        if (!scen_maps_open) {
            for (int gi = 0; gi < kScenRaceCount; ++gi) {
                if (!scen_available[kScenGridRace[gi]]) continue;
                int x0, y0;
                scen_cell(gi, x0, y0);
                if (x0 + 50 <= x && x < x0 + 50 + kScenComboW &&
                    y0 + 25 <= y && y < y0 + 25 + kScenComboH) {
                    return -4000 - gi;
                }
            }
        }
        return -1;
    }

    // ---------------------------------------------- the world-map camera
    // A 1:1 transcription of HSEngine.THSMapViewer + HSEngine.THSMapMouse
    // (HSEPack.dpl) and the map window's own handlers in AoW.exe. The camera is
    // a rectangle in map pixels; the map window's client box (the area above
    // the bar here) is its viewport, and the level render is blitted 1:1 at
    // `client - Scene`, so the view is a crop of the engine's static surface -
    // the world map has no zoom in AoW1.
    //
    // The client box is the MapWindow's inner area: AoW.exe 0x00451268 sizes the
    // scene from the window as `w - 0x18 / h - 0x18` (a 12 px border each side),
    // shows it at `+0xc, +0xc` and hit-tests the same 12 px inset (0x00450de8).
    // The replica draws no border art, so its client box is the whole area.

    // THSMap.GetMaxXhp (0x5560c3ec) = HXtoHP(w-1, h-1).x + 0x28 -> 32*w + 16.
    int cam_max_xhp() const { return 32 * static_cast<int>(map_w) + 16; }
    // THSMap.GetMaxYhp (0x5560c414): the last hex of column 1 (the stagger) plus
    // 0x20, i.e. 32*h + 16; a one-row map has no stagger (32*h).
    int cam_max_yhp() const {
        const int h = static_cast<int>(map_h);
        return h < 2 ? 32 * h : 32 * h + 16;
    }

    // THSMapViewer.SetSceneX (0x5560edf0). `x = max(x, 0x10)`, then if the
    // map's right edge would land inside the view, `x = maxXhp - w - 0x10`.
    void cam_set_x(int x) {
        if (x < 16) x = 16;
        if (cam_max_xhp() < cam_w + x + 16) x = cam_max_xhp() - cam_w - 16;
        cam_x = x;
    }
    // SetSceneY (0x5560ee38): the same against the map's *bottom* edge, tested
    // as `maxYhp - 0x10 < h + y` (the asymmetry is the engine's).
    void cam_set_y(int y) {
        if (y < 16) y = 16;
        if (cam_max_yhp() - 16 < cam_h + y) y = cam_max_yhp() - cam_h - 16;
        cam_y = y;
    }
    // SetSceneWidth (0x5560ee80) / SetSceneHeight (0x5560ee98): store, then
    // re-clamp the matching axis.
    void cam_set_w(int w) {
        if (w > 0) { cam_w = w; cam_set_x(cam_x); }
    }
    void cam_set_h(int h) {
        if (h > 0) { cam_h = h; cam_set_y(cam_y); }
    }
    // SetSceneL (0x5560edd0): clamp to [0, levelCount-1]. The engine redraws the
    // new level from the same map object; the port renders one level at a time,
    // so a change re-renders.
    void cam_set_l(int l) {
        const int n = game_level_count;
        if (l < 0) l = 0;
        if (n > 0 && n <= l) l = n - 1;
        cam_level = l;
        if (l != game_level && !game_map.empty()) {
            game_level = l;
            std::string err;
            if (!load_game_map(game_map, &err)) {
                std::fprintf(stderr, "menu: level %d unavailable (%s)\n", l,
                             err.c_str());
            }
            // A different level can have different dimensions, so the view is
            // re-clamped against the new extents (the engine's SetViewPort does
            // the same on its next call).
            cam_set_x(cam_x);
            cam_set_y(cam_y);
        }
    }
    // THSMapViewer.CenterScene (0x5560ed20) - the one camera entry point the
    // game uses for "move the view onto this hex". It converts the hex with
    // HXtoHP, then puts the hex's centre (`+ 0x10`) at the middle of the view:
    //   SetSceneX(HXtoHP.x + 16 - SceneWidth / 2)
    //   SetSceneY(HXtoHP.y + 16 - SceneHeight / 2)
    //   SetSceneL(level)
    // THSMapViewer.SetHSMap (0x5560eda4) is what makes it reachable: it hooks
    // the map's OnCenterViewTo to exactly this function, so TAoWHSMap.Select
    // (AoWEPACK 0x55777c5c, via THSMap.CenterViewTo 0x5560c178), CenterOnLeader
    // (0x55778e1c) and TSelectedArmy.Center (0x55793c84) all land here.
    void cam_center(int hx, int hy, int level) {
        int px = 0, py = 0;
        cam_hex_to_px(hx, hy, px, py);            // HXtoHP(x, y)
        cam_set_x(px + 16 - cam_w / 2);
        cam_set_y(py + 16 - cam_h / 2);
        cam_set_l(level);
    }
    // HSEngine.HPtoHX (0x5560e3d8) + THSMapMouse.GetXhx/GetYhx (0x5560b6b0 /
    // 0x5560b718): a view pixel -> the hex under it, with the engine's bounds
    // test (either coordinate outside [0, w) x [0, h) -> none).
    void cam_px_to_hex(int cx, int cy, int& hx, int& hy) const {
        const int mx = cam_x + cx;
        const int my = cam_y + cy;
        hx = (mx - 8) >> 5;                       // HPtoHX: (px - 8) / 32
        hy = (my - (hx & 1) * 16) >> 5;           // HPtoHX: (py - stagger) / 32
        if (hx < 0 || hx >= static_cast<int>(map_w) || hy < 0 ||
            hy >= static_cast<int>(map_h)) {
            hx = -1;
            hy = -1;
        }
    }
    // HXtoHP (0x5560e3b4) - the inverse, for drawing / round-tripping.
    static void cam_hex_to_px(int hx, int hy, int& px, int& py) {
        px = hx * 32 + 8;
        py = hy * 32 + (hx & 1) * 16;
    }

    // AoW.exe 0x00450de8 (the map window's hit test) with 0x00450e30/0x00450e88
    // /0x00450ef8: the mouse is converted from screen pixels to the window's
    // client box (the 12 px border subtracted) and handed to the map mouse.
    bool game_in_map(int x, int y) const {
        const AowBox m = game_layout().map_area;
        return m.x <= x && x < m.x + m.w && m.y <= y && y < m.y + m.h;
    }

    // AoW.exe 0x00452cc0 (called from MapWindowUpdate 0x00452e50): the *edge
    // scroll*. Its two parameters are the mouse in client pixels; within 2 px of
    // an edge the view steps 16 px that way, once per update, and the scanner's
    // view rectangle follows. The routine is gated on the application being
    // active and on no modal window being visible (FUN_00455eac).
    void cam_edge_scroll() {
        if (game_mx < 0 || game_modal_open) return;
        const AowBox m = game_layout().map_area;
        if (game_mx < 2) cam_set_x(cam_x - 16);
        if (m.w - 1 <= game_mx) cam_set_x(cam_x + 16);
        if (game_my < 2) cam_set_y(cam_y - 16);
        if (m.h - 1 <= game_my) cam_set_y(cam_y + 16);
    }

    // Scanner.TScanner's view rectangle, exactly as AoW.exe's
    // MapViewerSceneChanged (0x00450f8c) computes it - in *hex* units:
    //   SetTWidth (SceneWidth >> 5)    SetTHeight (SceneHeight >> 5)
    //   SetTLeft  ((SceneX >> 5) - 6)  SetTTop   ((SceneY >> 5) - 6)
    // The `- 6` is the scanner's own map-origin offset (the Scanner unit lives
    // in AOWTools.dpl, which is not in the Ghidra project, so its consumer could
    // not be read); in the scanner's *drawn* coordinates the rectangle sits at
    // (SceneX >> 5, SceneY >> 5).
    void cam_scanner_rect(int& tl, int& tt, int& tw, int& th) const {
        tl = (cam_x >> 5) - 6;
        tt = (cam_y >> 5) - 6;
        tw = cam_w >> 5;
        th = cam_h >> 5;
    }

    // AoW.exe FUN_00452e50's per-frame body for the map window: read the mouse
    // in client pixels, resolve the hovered hex (THSMapMouse.GetXhx/GetYhx) and
    // edge-scroll. The engine runs it from the window's Update; the port runs it
    // from its 30 fps timer.
    void game_update() {
        // The map's animation clock, AoWHSMap+0x40: HSEngine.THSMap.NewFrame
        // (0x5560d758) adds one per map frame and wraps it at 0x100000. The
        // units' image sequences and the hover pointer both animate off it, which
        // is why the pointer's pulse stays locked to the game's playback.
        if (++game_clock > kMapClockWrap) game_clock = 0;
        // Each timer fire advances `kAnimFramesPerTick` animation frames of a running
        // walk (TUnitHS.MoveNext@0x55784910 / the map clock AoWHSMap+0x40): the
        // travel speed and the walk cycle's frame rate share the tick, so the one
        // multiplier scales both. The sprite is recomposited into `game_draw`.
        if (mv_anim) {
            bool finished = false;
            for (int i = 0; i < aow::kAnimFramesPerTick && !finished; ++i) {
                ++mv_anim_clock;
                if (!mv_walk.tick()) {
                    // The path is done. `TMoveArmyTE.FinishMove` runs now: for
                    // a merge the incoming units join the destination army only
                    // at this moment, and the destination becomes the selection.
                    if (mv_merge_pending) mv_finish_merge();
                    mv_anim = false;
                    mv_hide_level = mv_hide_x = mv_hide_y = -1;
                    mv_render_level();
                    game_update_path();
                    finished = true;
                }
            }
            if (!finished) mv_refresh_draw();
        }
        const AowBox m = game_layout().map_area;
        cam_set_w(m.w);
        cam_set_h(m.h);
        // A game that has just started centres the view on the human player's
        // leader - the engine's map-open behaviour (TAoWE.TAoWHSMap.CenterOnLeader
        // 0x55778e1c) - without selecting it. Done here because cam_center() needs
        // the viewport size cam_set_w/cam_set_h just resolved; the flag makes it
        // run exactly once.
        if (game_center_on_start) {
            game_center_on_start = false;
            game_center_leader(false);
        }
        game_resolve_hover();
        game_update_move_cursor();
        game_update_cursor();
        // The party floater's own hover: its title-bar and foot buttons light
        // up while the pointer is over them (TAOWBaseButton's ILIndexLit).
        party_hover = game_show_party ? party_hit_test(mouse_x, mouse_y) : -1;
        // The Structure floater's title-bar / foot buttons light up on hover
        // (TAOWBaseButton's ILIndexLit), the same one-frame update.
        game_struct_hover =
            game_show_structure ? structure_hit_test(mouse_x, mouse_y) : -1;
        // The TurnButton's own Update (TAOWTurnButton.Update 0x59704cd4) runs
        // once per game frame, exactly like the window's Update, and then the
        // turn loop hands the turn on to the seats the port does not simulate.
        tb_turn_update();
        game_turn_tick();
        cam_edge_scroll();
        // The building animations advance with the map clock (AoWHSMap+0x40): the
        // engine re-draws its scene every frame, so re-composite the animated
        // buildings (and the overlays above them) once per frame. A walk already
        // refreshed above.
        if (!mv_anim) mv_refresh_draw();
    }

    // The mouse's client position -> the map window's client pixels -> the hex
    // under it (THSMapMouse.GetXhx/GetYhx). Split out of game_update so a mouse
    // move can retarget the trail at once, without waiting for the frame timer
    // (and without edge-scrolling on every move).
    void game_resolve_hover() {
        const AowBox m = game_layout().map_area;
        if (game_in_map(mouse_x, mouse_y)) {
            game_mx = mouse_x - m.x;
            game_my = mouse_y - m.y;
        } else {
            game_mx = -1;   // the engine's "outside" sentinel (0x80000000)
            game_my = -1;
        }
        if (game_mx >= 0) {
            cam_px_to_hex(game_mx, game_my, game_hover_hx, game_hover_hy);
        } else {
            game_hover_hx = -1;
            game_hover_hy = -1;
        }
        game_mouse_moved();
    }

    // AoWE.TAoWHSMap.MouseMoved@0x55777a58: the map mouse's *cursor state
    // request*, re-armed on every mouse move. With no sprite selected, a hover
    // over a hex that holds something selectable asks for the animated pointer
    // (0x101, the sparkle); every other hex asks for the plain arrow (0).
    //
    // The engine also gates this on the hex being explored (`TAoWMapField.
    // GetExplored` past the map's fog flag) and on the map's "show objects"
    // option (`AoWHSMap+200` / the field's object flag); the replica draws every
    // level unfogged with all its objects, so only the selection test is live.
    //
    // While an army is selected the engine returns early here and leaves the
    // request to `TSelectedArmy.UpdateMoveCursor` (`game_update_move_cursor`), so
    // the move icons are not overwritten; off the map (`GetXhx == -1`, i.e. the
    // pointer sits on the bar or the floaters) it returns early too, keeping the
    // last request. In that off-map case the replica settles on the plain arrow
    // rather than keeping an icon from before, so the pointer never pulses over
    // the interface.
    void game_mouse_moved() {
        if (mv_selected) return;         // the move selection owns the pointer
        if (game_hover_hx < 0) {
            game_cursor_state = kCursorStateDefault;
            return;
        }
        game_cursor_state = game_can_select_here() ? kCursorStateHover
                                                   : kCursorStateDefault;
    }

    // HSEngine.THexagonSpriteSelection.CanSelectLocation@0x5560bb54 (the AoW
    // build's `TAoWHSMap.MouseMoved` calls it with the hovered hex): walk the
    // field's hexagon sprites from the last placed back to the first and take the
    // first one that answers `CanSelect`. An army is the only sprite the replica
    // models on a hex, and `AoWE.TArmyHS.CanSelect@0x55790b20` ->
    // `AoWE.TUnitHS.CanSelect@0x55784078` says yes for a standing (not walking)
    // army that is on that hex - and, from the army record, is neither hidden
    // from the seated player (`TArmy+0x10 & 0x10`, a flag the replica does not
    // parse) nor concealed by fog. It is deliberately **not** an ownership test,
    // so the engine asks for the sparkle over an enemy army too (clicking one is
    // how you attack it).
    bool game_can_select_here() {
        // HSEngine.THexagonSpriteSelection.CanSelectLocation walks every sprite
        // on the field, so a building answers exactly like an army. The engine
        // pages party -> structure -> party when both stand on one hex.
        return army_on_hex(game_hover_hx, game_hover_hy) != nullptr ||
               structure_on_hex(game_hover_hx, game_hover_hy) != nullptr;
    }

    // `TAoWMapField.GetExplored@0x55771bf4`: bit `player` of the field's explored
    // mask (the tile's `explored`, TAoWMapField tag 4). A move selection cannot
    // enter a hex the seated player has never seen - that is what the blocked move
    // icon means (see game_update_move_cursor) - and the trail's search treats such
    // a hex as impassable too (`ExploredOnlyMoveOnMovePointProc@0x5574642c`).
    bool game_field_explored(int x, int y) const {
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size()) {
            return false;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        if (x < 0 || y < 0 || static_cast<std::uint32_t>(x) >= lv.width ||
            static_cast<std::uint32_t>(y) >= lv.height || mv_human == 0xff) {
            return false;
        }
        const aow::MapTile& t = lv.tiles[static_cast<std::size_t>(y) * lv.width +
                                         static_cast<std::size_t>(x)];
        return ((t.explored >> (mv_human & 0x1f)) & 1u) != 0;
    }

    // The move *action* of the hex under the pointer: the answer the field's
    // sprites give to `TAoWMapField.GetMoveAction@0x55771ddc`'s message (0x1101),
    // which for an army is `TArmy.GetMoveAction@0x5578f98c`:
    //   0 = nothing there (or an army concealed from this player)
    //   1 = an army that is not ours -> the crossed-swords pointer
    //   2 = an independent army whose units would join -> the twin-banner pointer
    //   3 = our own army -> the plain move pointer
    // The engine runs the query with the *selected* army's player, so the test is
    // against `mv_human`. The `2` case needs `TArmy.GetJoinSelection@0x5578fa74`
    // -> `TAbstractUnit.CanJoin@0x557821ac`, which decides on race alignment and
    // diplomatic relations; the replica models neither, so a foreign army always
    // answers `1` - what the engine shows for an army that cannot join.
    int game_move_action(int x, int y) {
        const aow::MapHs* a = army_on_hex(x, y);
        if (a == nullptr) return 0;
        if (mv_human == 0xff) return 1;
        return a->owner == mv_human ? 3 : 1;
    }

    // AoWE.TSelectedArmy.UpdateMoveCursor@0x55793440: the *selected army's*
    // pointer. `TMoveSelector.Activate@0x55763e74` sets the state to 0x100 when a
    // move selection starts, and the map mouse re-runs this on every move, so the
    // icon follows the pointer while a destination is being chosen.
    //
    //   * a hex this player has not explored, or no target at all -> 0x100 (the
    //     move icon, `| 1` below);
    //   * otherwise the hex's move action picks 0x102 (attack) / 0x103 (join) /
    //     0x100 (our own army, or nothing).
    //
    // The 0x100 art comes from the army's move types (`TArmy.MoveTypes`, the
    // union of the units' - the replica's `mv_army.move_types`): bit 0 walk ->
    // 0x40, bit 2 water -> 0x42, bit 1 fly -> 0x44, tested in that order. The
    // engine then ORs in bit 0 (`SetCursor(GetCursor() | 1)` - the "blocked"
    // twin) unless its move-point table has a zero at the target's terrain/overlay
    // cell; that table is seeded with 1 and folded with each unit's costs (0xFF =
    // impassable, `TArmy.CreateMovePointTable@0x5578dc40`), so a zero never
    // occurs and all that remains of the test is the exploration gate: exploration
    // enabled and the hex unexplored.
    void game_update_move_cursor() {
        if (!mv_selected) return;        // nothing of ours is selected
        if (game_hover_hx < 0) {         // the pointer is off the map
            game_cursor_state = kCursorStateDefault;
            return;
        }
        const bool explored = game_field_explored(game_hover_hx, game_hover_hy);
        const int action =
            explored ? game_move_action(game_hover_hx, game_hover_hy) : 0;
        if (action == 1) {
            game_cursor_state = kCursorStateAttack;
        } else if (action == 2) {
            game_cursor_state = kCursorStateJoin;
        } else {
            game_cursor_state = kCursorStateMove;
        }
        if (game_cursor_state != kCursorStateMove) return;
        std::uint32_t icon = 0;
        if (mv_army.move_types & aow::kMoveWalk) {
            icon = kMoveCursorWalkId;
        } else if (mv_army.move_types & aow::kMoveWater) {
            icon = kMoveCursorSailId;
        } else if (mv_army.move_types & aow::kMoveFly) {
            icon = kMoveCursorFlyId;
        }
        if (icon == 0) return;           // no movement mode at all: leave the id
        if (kExplorationEnabled && !explored) icon |= 1;   // the blocked twin
        game_cursor_index = icon;
    }

    // `TAoWHSMap+0xd6`: the hexagon selector is in the scene while a move
    // selection is active - `TMoveSelector.Activate` shows it, `Deactivate` (run
    // by `TSelectedArmy.ExecuteMove` when the move starts) hides it - and
    // `TAoWHSMap.NewFrame@0x55777ce8` answers it by re-positioning the two halves
    // on the mouse's hex every frame (`UpdateHexagonSelector@0x55773424`, which
    // writes 0xFF into the hex when the pointer is off the map so both halves
    // vanish - ShowBack/FrontHexagonSelector return early on -1).
    bool game_hex_selector_shown() const {
        return mv_selected && !mv_anim && game_hover_hx >= 0 && game_hover_hy >= 0;
    }

    // AoWE.TAoWHSMap.UpdateCursor@0x55777b20: resolve the state request into the
    // pointer image id (THSMapMouse+0x30) - run every frame from NewFrame, which
    // is what advances the animated icons. State 0 -> the plain arrow (id 0);
    // 0x101 -> `0x34 + ((clock >> 2) & 3)`, the four sparkle frames; 0x102 ->
    // `0x31 + ((clock >> 2) & 1)`, the two crossed-swords frames; 0x103 ->
    // `0x3a + ((clock >> 2) % 3)`, the three twin-banner frames. State 0x100 is
    // deliberately *not* resolved here: `TSelectedArmy.UpdateMoveCursor` paints
    // those icons itself (see game_update_move_cursor). The engine masks with
    // 0x80000003; the clock is non-negative here, so `& 3` is the same. It also
    // mirrors "a custom pointer is up" into THSMapMouse+0x3a; the replica always
    // paints its own pointer, so that flag is not modelled.
    void game_update_cursor() {
        const std::uint32_t frame =
            static_cast<std::uint32_t>(game_clock >> kHoverCursorClockShift);
        switch (game_cursor_state) {
        case kCursorStateHover:
            game_cursor_index = kHoverCursorBaseId + (frame & kHoverCursorClockMask);
            break;
        case kCursorStateAttack:
            game_cursor_index =
                kAttackCursorBaseId + (frame & (kAttackCursorFrames - 1));
            break;
        case kCursorStateJoin:
            game_cursor_index = kJoinCursorBaseId +
                                frame % static_cast<std::uint32_t>(kJoinCursorFrames);
            break;
        case kCursorStateDefault: game_cursor_index = kDefaultCursorId; break;
        default: break;   // 0x100: the move-cursor pass set the icon itself
        }
    }

    // The Cursors.ilb sprite the map window paints at the pointer: the engine's
    // `THSMapMouse+0x30` id resolved through the map's own Cursors library, or the
    // plain arrow when the id is 0 or its art is missing.
    const Sprite& game_pointer() const {
        const std::size_t i = static_cast<std::size_t>(game_cursor_index);
        if (i < map_cursor.size() && map_cursor[i].ok) return map_cursor[i];
        return cursor;
    }

    // `TMoveArmyTE.FinishMove@0x55747fbc`: the walk has reached the destination,
    // so the incoming units now join the army standing there
    // (`TArmy.AddUnitSelection@0x5578e6b4`, original order). Until this moment
    // the destination army stayed untouched on the map and the incoming party
    // was the selection; now the destination becomes the selection, exactly the
    // engine's `TSelectedArmy.Select(destination)`. The units are appended, so
    // the destination keeps its lead unit.
    void mv_finish_merge() {
        if (!mv_merge_pending) return;
        const int src_x = mv_sel_x, src_y = mv_sel_y;
        const int dest_x = mv_merge_x, dest_y = mv_merge_y;
        const std::size_t added = mv_merge_moving.units.size();
        aow::MapHs* target = army_on_hex(dest_x, dest_y);
        if (target != nullptr && target->owner == mv_human) {
            target->units.insert(target->units.end(),
                                 mv_merge_moving.units.begin(),
                                 mv_merge_moving.units.end());
            // `TArmy.MovePoints` is the minimum over the units, so the merged
            // army keeps whichever of the two parties has fewer left.
            if (mv_merge_moving.army_mp_left >= 0 &&
                (target->army_mp_left < 0 ||
                 mv_merge_moving.army_mp_left < target->army_mp_left)) {
                target->army_mp_left = mv_merge_moving.army_mp_left;
            }
            target->army_flags = static_cast<std::uint8_t>(target->army_flags & ~3u);
            std::printf("game_merge=%d,%d->%d,%d stay=%d added=%d total=%d\n",
                        src_x, src_y, dest_x, dest_y,
                        static_cast<int>(mv_merge_stay),
                        static_cast<int>(added),
                        static_cast<int>(target->units.size()));
            std::fflush(stdout);
        } else if (game_level >= 0 &&
                   static_cast<std::size_t>(game_level) <
                       game_state_map.levels.size()) {
            // The destination vanished while the army walked (not expected on
            // the world map): put the arriving army on the hex instead of
            // losing it.
            const aow::MapLevel& lv =
                game_state_map.levels[static_cast<std::size_t>(game_level)];
            if (dest_x >= 0 && dest_y >= 0 &&
                static_cast<std::uint32_t>(dest_x) < lv.width &&
                static_cast<std::uint32_t>(dest_y) < lv.height) {
                game_state_map.levels[static_cast<std::size_t>(game_level)]
                    .tiles[static_cast<std::size_t>(dest_y) * lv.width +
                           static_cast<std::size_t>(dest_x)]
                    .objects.push_back(mv_merge_moving);
            }
            std::printf("game_merge=%d,%d->%d,%d stay=%d added=%d total=%d\n",
                        src_x, src_y, dest_x, dest_y,
                        static_cast<int>(mv_merge_stay),
                        static_cast<int>(added), static_cast<int>(added));
            std::fflush(stdout);
        }
        mv_merge_pending = false;
        mv_merge_moving = aow::MapHs();
        mv_merge_x = mv_merge_y = -1;
        mv_merge_stay = 0;
        mv_select(dest_x, dest_y);
    }

    // Relocates the selected army to the path's destination and re-renders the
    // level. Only a hex the army can reach THIS turn moves (the engine otherwise
    // reports "Not enough move points" and leaves the army where it is); the
    // trail still shows the whole beyond-this-turn path.
    bool game_move_selected() {
        if (!mv_selected || !mv_have_path || !mv_path.reached) return false;
        const aow::MoveStep* dest = mv_path.destination();
        if (dest == nullptr) return false;
        if (mv_army.budget < mv_path.total_cost) {
            game_say("not enough move points this turn");
            return false;
        }
        // The move spends this turn's move points (TAbstractUnit.MovePointCost
        // is charged step by step; TArmy.MovePoints is then what is left).
        const int mp_after =
            std::max(0, static_cast<int>(mv_army.budget) - mv_path.total_cost);
        const int dest_x = dest->x, dest_y = dest->y;
        const int src_x = mv_sel_x, src_y = mv_sel_y;
        if (dest_x == src_x && dest_y == src_y) return false;
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        aow::MapTile& from =
            game_state_map.levels[static_cast<std::size_t>(game_level)]
                .tiles[static_cast<std::size_t>(src_y) * lv.width +
                       static_cast<std::size_t>(src_x)];
        // Find the selected army without removing it yet: a merge onto another
        // of our armies has to be validated before anything moves.
        std::size_t src_index = from.objects.size();
        for (std::size_t i = 0; i < from.objects.size(); ++i) {
            const aow::MapHs& hs = from.objects[i];
            if (hs.class_id != aow::kMapArmyHsClassId || hs.units.empty()) {
                continue;
            }
            if (mv_human == 0xff || hs.owner != mv_human) continue;
            src_index = i;
            break;
        }
        if (src_index == from.objects.size()) return false;
        aow::MapHs& src_army = from.objects[src_index];
        // Units left behind (`TControlWin+0x178`): the engine moves only the
        // selected subset (`TArmy.MoveOnMovePoints`), and the stragglers stay on
        // the origin hex as their own army - a new "group". Mirror that: split
        // the moving army here and drop the remainder back on the source hex
        // before it leaves.
        std::vector<aow::MapUnit> go, stay;
        for (std::size_t k = 0; k < src_army.units.size(); ++k) {
            const bool travels =
                k < static_cast<std::size_t>(kPartySlots) &&
                (party_sel & (1u << k)) != 0;
            if (travels) {
                go.push_back(src_army.units[k]);
            } else {
                stay.push_back(src_army.units[k]);
            }
        }
        if (go.empty()) {
            // `TArmy.MoveOnMovePoints` would move nothing; the engine's Move
            // button is disabled with an empty selection, so never create an
            // empty army on the destination.
            game_say("no units to move");
            return false;
        }
        const std::size_t stay_count = stay.size();
        // `TMoveArmyTE.CanMoveNext@0x5574830c`: the last step of a move onto a
        // hex that holds another army of the same player is accepted only when
        // the merge fits. `TArmy.CanAddUnitSelection@0x5578e55c` allows at most
        // eight units (a normal army's `TArmy.MaxSize@0x5578def4`); the failure
        // text is `TArmyCombineMoveTE.ShowCombineFailure@0x5574876c`'s
        // "Cannot combine armies" (resourcestring 65435).
        aow::MapHs* merge_into = nullptr;
        if (aow::MapHs* target = army_on_hex(dest_x, dest_y);
            target != nullptr && target->owner == mv_human) {
            const aow::ArmyAddStatus st =
                aow::army_can_add_units(target->units.size(), go.size());
            if (st != aow::ArmyAddStatus::ok) {
                game_say(st == aow::ArmyAddStatus::transporter_conflict
                             ? "Cannot combine two transporters"
                             : "Cannot combine armies");
                return false;
            }
            merge_into = target;
        }
        aow::MapHs moved = std::move(src_army);
        // Moving drops the Guard stance (`TArmy+0x28` bit 0) and spends the move
        // points, so `SelectNextArmy` skips this army until its owner's next turn
        // refills it (`TAbstractUnit.NewTurn`).
        moved.army_flags = static_cast<std::uint8_t>(moved.army_flags & ~3u);
        moved.army_mp_left = static_cast<std::int16_t>(mp_after);
        from.objects.erase(from.objects.begin() +
                           static_cast<std::ptrdiff_t>(src_index));
        {
            if (!stay.empty()) {
                moved.units = std::move(go);
                aow::MapHs left;
                left.tag = moved.tag;
                left.class_id = aow::kMapArmyHsClassId;
                left.owner = moved.owner;
                left.army_direction = moved.army_direction;
                left.units = std::move(stay);
                // The units that stay behind never walked, so their own move
                // points are untouched - they keep whatever they had left. The
                // new group has not moved as a whole, so its army-level mark
                // stays at "unspent" (the units' own values are what count).
                left.army_flags = 0;
                left.army_mp_left = -1;
                from.objects.push_back(std::move(left));
                std::printf("game_split=%d,%d stay=%d go=%d\n", src_x, src_y,
                            static_cast<int>(from.objects.back().units.size()),
                            static_cast<int>(moved.units.size()));
                std::fflush(stdout);
                game_say("left " +
                         std::to_string(from.objects.back().units.size()) +
                         " unit(s) behind");
                // The moved army is re-stacked, so every slot travels again.
                party_sel = 0;
                for (std::size_t k = 0;
                     k < moved.units.size() &&
                     k < static_cast<std::size_t>(kPartySlots); ++k) {
                    party_sel |= static_cast<std::uint8_t>(1u << k);
                }
                party_focus = moved.units.empty() ? -1 : 0;
            }
        }
        // Each travelling unit pays the path's cost from its own pool
        // (`TAbstractUnit.MovePointCost@0x55780848` is charged step by step, so
        // the whole party spends the same amount - the value the Party window's
        // pill captions and the `MovesLbl` read out). The units left behind
        // never walked, so their own points are untouched.
        for (aow::MapUnit& u : moved.units) {
            aow::spend_unit_move_points(mv_cat, u, mv_path.total_cost);
        }
        // The facing byte the army keeps (TUnitHS rw tag 9): the direction of its
        // last step. The idle render draws the *standing* pose of this facing
        // (`seq[dir-1]`), so the unit keeps facing the way it arrived.
        int move_facing = 0;
        if (mv_path.steps.size() >= 2) {
            const aow::MoveStep& a = mv_path.steps[mv_path.steps.size() - 2];
            move_facing = aow::hex_direction(a.x, a.y, dest_x, dest_y);
            if (move_facing >= 1 && move_facing <= 6) {
                moved.army_direction = static_cast<std::uint8_t>(move_facing);
            }
        }
        aow::MapTile& to =
            game_state_map.levels[static_cast<std::size_t>(game_level)]
                .tiles[static_cast<std::size_t>(dest_y) * lv.width +
                       static_cast<std::size_t>(dest_x)];
        if (merge_into != nullptr) {
            // `TMoveArmyTE.FinishMove@0x55747fbc` runs when the walk *arrives*,
            // so nothing is combined yet: the destination army (`merge_into`)
            // stays untouched on the map and the incoming party is the
            // selection while its sprite walks (`mv_finish_merge` on arrival).
            mv_merge_pending = true;
            mv_merge_x = dest_x;
            mv_merge_y = dest_y;
            mv_merge_stay = stay_count;
            mv_merge_moving = moved;
            mv_anim_unit = moved.units.front();
            mv_anim_army = moved;
            mv_use_army(mv_merge_moving, src_x, src_y);
        } else {
            to.objects.push_back(std::move(moved));
            // The moved army now sits on the destination hex; walk its *sprite*
            // across the path (aow/unit_anim.hpp) while the resting copy is
            // hidden.
            if (!to.objects.empty() && !to.objects.back().units.empty()) {
                mv_anim_unit = to.objects.back().units.front();
                mv_anim_army = to.objects.back();
            }
        }
        int steps_per_hex = 8;
        if (const aow::UnitGraphics* ug =
                aow::find_unit_graphics(mv_cat, mv_anim_unit)) {
            steps_per_hex = aow::unit_move_steps(ug->move_class);
        }
        std::vector<std::pair<int, int>> hexes;
        hexes.reserve(mv_path.steps.size());
        for (const aow::MoveStep& s : mv_path.steps) {
            hexes.emplace_back(s.x, s.y);
        }
        if (mv_merge_pending) {
            // The army being merged into must keep standing on its hex for the
            // whole walk - only the incoming sprite is drawn by the animation.
            mv_hide_level = mv_hide_x = mv_hide_y = -1;
            mv_render_level();
            mv_path = aow::MovePath();
            mv_have_path = false;
        } else {
            mv_hide_level = game_level;
            mv_hide_x = dest_x;
            mv_hide_y = dest_y;
            mv_render_level();
            mv_selected = false;
            mv_sel_x = mv_sel_y = -1;
            mv_army = aow::ArmyMove();
            mv_path = aow::MovePath();
            mv_have_path = false;
        }
        mv_anim_clock = 0;
        // The engine's per-unit phase is `(ptr >> 2) & 0xffff`. It is arbitrary,
        // which is why a walk is normally cut off mid-stride and the standing
        // pose then replaces that mid-stride pose in a single frame - the lurch
        // on arrival. The move knows its own length, so align the walking cycle
        // with it instead: the stride then finishes on the walk's last pose
        // (`<d>08`), the pose the standing pose (`<d>09`) follows, and the unit
        // settles into the idle sprite instead of snapping to it. The phase is
        // derived from the path, so different-length moves still differ. Falls
        // back to the engine's own per-unit phase when the unit ships no mode-4
        // walking cycle.
        mv_anim_phase = (src_x * 7 + src_y * 13) & 0xffff;
        {
            int cycle_len = 0, spacing = 0;
            if (aow::unit_walk_cycle(mv_cat, mv_anim_unit, move_facing,
                                     cycle_len, spacing)) {
                mv_anim_phase = aow::walk_settle_phase(
                    static_cast<int>(hexes.size() - 1) * steps_per_hex,
                    aow::kAnimFramesPerTick, cycle_len, spacing);
                std::printf("game_walk=cycle=%d spacing=%d phase=%d settle=%d\n",
                            cycle_len, spacing, mv_anim_phase, cycle_len - 1);
                std::fflush(stdout);
            }
        }
        mv_anim = mv_walk.start(hexes, steps_per_hex);
        if (!mv_anim) {
            // A degenerate path: just stand the army on its hex.
            if (mv_merge_pending) {
                mv_finish_merge();   // no walk: combine right away
                mv_hide_level = mv_hide_x = mv_hide_y = -1;
                mv_render_level();
            } else {
                mv_hide_level = mv_hide_x = mv_hide_y = -1;
                mv_render_level();
                // TSelectedArmy follows the army it just moved.
                mv_select(dest_x, dest_y);
            }
        } else if (!mv_merge_pending) {
            // TSelectedArmy follows the army it just moved, so it stays
            // selected. A merge instead keeps the *incoming* party selected
            // until the walk arrives (`mv_finish_merge`).
            mv_select(dest_x, dest_y);
        }
        // The moving army's vision reveals the ground it arrives on: the engine
        // refreshes a player's explored mask as its armies move and each turn
        // (`TArmy.UpdateExploration@0x5578e1a8`, the same radius-4 pass as at map
        // start). Explored bits are permanent, so this only ever adds.
        mv_explore_for_human();
        if (merge_into == nullptr) {
            game_say("moved (" + std::to_string(src_x) + "," +
                     std::to_string(src_y) + ") -> (" +
                     std::to_string(dest_x) + "," + std::to_string(dest_y) + ")");
        }
        game_update_path();
        std::printf("game_move=%d,%d->%d,%d dir=%d\n", src_x, src_y, dest_x, dest_y,
                    move_facing);
        std::fflush(stdout);
        return true;
    }

    // A left mouse-down on the map: THSMapMouse.MouseDown (0x5560b838 ->
    // 0x5560b7d0) selects the hex under the cursor via
    // THexagonSpriteSelection.SelectLocation and hands it to TAoWHSMap.Select,
    // which picks the army standing there. An army of ours becomes the
    // selection; a click on any other hex walks the selected army there when this
    // turn's move points cover the path, otherwise the move is refused and the
    // army stays selected (clicking the selected army's own hex clears it). A
    // *plain* map click still does NOT move the view: the camera follows the
    // scanner (game_scanner_click) or the `C`/`L` keys (game_key) only.
    void game_map_click() {
        if (game_hover_hx < 0) return;
        // A walk is in progress: the engine's move animation owns the map until
        // it finishes (the army is already on its destination), so swallow the
        // click rather than starting a second move from a stale selection.
        if (mv_anim) return;
        // A map click elsewhere dismisses the Structure floater; clicking a
        // building again reopens it (TIWindow is a selection view, not modal).
        if (game_show_structure) close_structure();
        game_sel_hx = game_hover_hx;
        game_sel_hy = game_hover_hy;
        aow::MapHs* a = army_on_hex(game_sel_hx, game_sel_hy);
        const bool mine =
            a != nullptr && mv_human != 0xff && a->owner == mv_human;
        if (mine) {
            if (mv_selected && mv_sel_x == game_sel_hx && mv_sel_y == game_sel_hy) {
                mv_reset();  // clicking the selected army again clears it
                return;
            }
            if (mv_selected) {
                // Clicking *another* one of our armies while one is selected is
                // a move onto its hex: `TMoveArmyTE.CanMoveNext@0x5574830c` lets
                // the last step combine the two when the merge fits, and
                // `TMoveArmyTE.FinishMove@0x55747fbc` performs it. A refused
                // combine keeps the selected army selected (the engine's move
                // transition only reports the failure).
                game_update_path();
                if (game_move_selected()) return;
                if (!mv_path.reached) game_say("destination unreachable");
                return;
            }
            mv_select(game_sel_hx, game_sel_hy);
            game_say("selected army (" + std::to_string(game_sel_hx) + "," +
                     std::to_string(game_sel_hy) + ")");
            game_update_path();
            return;
        }
        // No army of ours here: a building on the hex opens the Structure
        // floater (`TAoWHSMap.Select` -> TStructure.Select / TIWindow.ShowInfo).
        if (!mv_selected && a == nullptr && open_structure(game_sel_hx, game_sel_hy)) {
            game_say("selected structure (" + std::to_string(game_sel_hx) + "," +
                     std::to_string(game_sel_hy) + ")");
            return;
        }
        if (mv_selected) {
            game_update_path();
            if (game_move_selected()) return;
            // The engine reports an impossible move and keeps the selection.
            if (!mv_path.reached) game_say("destination unreachable");
        } else {
            mv_reset();
        }
    }

    // A right-click on the world map clears the current selection. The engine
    // removes the party floater when the main map window is right-clicked, and
    // closing that floater deselects the party (`mld_strings.txt`, "The Party
    // Floater": "the party floater may be removed by right clicking anywhere on
    // the main map window, as closing the party window will deselect the current
    // party"). A right-click with nothing selected, or one during a walk, does
    // nothing - mirroring `game_map_click`, whose walk animation owns the map.
    // Shared by the window procedure and the head-less `--game-right-click` hook
    // so the two cannot drift.
    void game_right_click() {
        if (mv_anim) return;       // the walk animation owns the map
        // Right-clicking removes a floater (the help text: "the party floater
        // may be removed by right clicking anywhere on the main map window").
        if (game_show_structure) {
            close_structure();
            return;
        }
        if (!mv_selected) return;  // nothing to clear
        mv_reset();
    }

    // A click (or drag) on the scanner's minimap: the minimap is the map drawn
    // 1 px per hex, so the clicked pixel names the hex to move the view onto -
    // the same camera entry point, with the hex taken from the minimap's own
    // scale (the port scales the whole level into the scanner rect).
    void game_scanner_click(int x, int y) {
        if (map_w == 0 || map_h == 0) return;
        const AowBox s = game_layout().scan_map;
        if (x < s.x || x >= s.x + s.w || y < s.y || y >= s.y + s.h) return;
        const int hx = (x - s.x) * static_cast<int>(map_w) / s.w;
        const int hy = (y - s.y) * static_cast<int>(map_h) / s.h;
        cam_center(hx, hy, cam_level);
    }

    // TSelectedArmy.Center (AoWEPACK 0x55793c84) and the 'C' key
    // (TAoWHSMapKeyboard.KeyDown 0x55773b08, case 'C'): put the selected army's
    // hex in the middle of the view (via TAoWHSMap's vtable +0xbc, which is
    // THSMap.CenterViewTo 0x5560c178 -> CenterScene). The replica has no army
    // list, so the "selected army" is the hex the last map click picked.
    void game_center_selection() {
        if (game_sel_hx < 0) return;
        cam_center(game_sel_hx, game_sel_hy, cam_level);
    }

    // The in-game map's camera keys, TAoWHSMapKeyboard.KeyDown (AoWEPACK
    // 0x55773b08): 'C' centres on the current selection (TSelectedArmy.Center
    // 0x55793c84), 'L' on the player's leader (CenterOnLeader 0x55778e1c).
    // Returns false when the key is not one of the map's. Shared by the window
    // procedure and the head-less `--game-key` hook so the two cannot drift.
    bool game_key(int vk) {
        if (vk == 'C') {
            game_center_selection();
            return true;
        }
        if (vk == 'L') {
            game_center_leader();
            return true;
        }
        return false;
    }

    // The `L` key: TAoWE.TAoWHSMap.CenterOnLeader (AoWEPACK 0x55778e1c, called
    // from TAoWHSMapKeyboard.KeyDown 0x55773b08). The engine centres on the
    // player's *leader*; the port has the roster now, so it uses the human
    // player's army on this level whose lead unit is the hero (MapHs::units is
    // lead-first, and on the shipped maps the leader's army leads with a hero),
    // falling back to the player's first army here. When `select` is true (the `L`
    // key) the army is also selected, so the trail can be drawn straight away;
    // starting a game passes false and only centres on the leader.
    void game_center_leader(bool select = true) {
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size()) {
            return;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        int hx = -1, hy = -1;
        for (std::uint32_t y = 0; y < lv.height && hx < 0; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                const aow::MapTile& t =
                    lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                for (const aow::MapHs& hs : t.objects) {
                    if (hs.class_id != aow::kMapArmyHsClassId || hs.units.empty()) {
                        continue;
                    }
                    if (mv_human == 0xff || hs.owner != mv_human) continue;
                    if (hx < 0 && hy < 0) {  // the player's first army here
                        hx = static_cast<int>(x);
                        hy = static_cast<int>(y);
                    }
                    if (!hs.units.front().is_hero) continue;
                    hx = static_cast<int>(x);
                    hy = static_cast<int>(y);
                    goto found;
                }
            }
        }
    found:
        if (hx < 0) return;  // no army of ours on this level
        if (select) {
            game_sel_hx = hx;
            game_sel_hy = hy;
            mv_select(hx, hy);
            game_update_path();
        }
        cam_center(hx, hy, cam_level);
        std::printf("game_leader=%d,%d\n", hx, hy);
        std::fflush(stdout);
    }

    // In-game screen hit test. The composition is scaled by disp_w/640, so the
    // same scaled boxes the renderer uses drive the hit test:
    //   0..4 = the five bar tabs (World Map, Realm, Magic, Relations, Events)
    //   5/6  = the scanner's Surface / Caverns tabs
    //   7    = the next-turn button
    //   8    = the scanner's minimize button
    //   -1   = none
    // The in-game screen's device-pixel layout, resolved from the DFM alignment
    // fields through aow_realign(). Built per frame, so it tracks any window
    // size; the renderer and the hit test share it so they cannot drift.
    struct GameLayout {
        AowBox bar;                  // TTBWindow.TaskWindow
        AowBox taskpanel;            // TaskPanel (the tab strip)
        AowBox tab[kTbTabCount];
        AowBox start, next, turn, gold, mana;
        AowBox timer, timeicon, pind;    // the clock + player-indicator strip
        AowBox scan;                 // TSWindow.ScannerWindow
        AowBox scan_frame;           // ScannerFrame (ScanWin 9-slice)
        AowBox scan_map;             // TScanner (the minimap)
        AowBox scan_tab[2];          // ScannerTab halves
        AowBox scan_chk[3];          // Building / Army / NaturalCheck
        AowBox scan_icon, scan_title, scan_min, scan_help;
        AowBox events;               // TPInfoW.PInfoWin
        AowBox event_pnl, face, shield, info, turnfr, list;
        AowBox event_icon, event_title, event_min, event_help, event_lbl;
        AowBox player, leadl, race, day;   // the banner labels (x = centre)
        AowBox map_window;           // TMWindow.MapWindow (the frame's box)
        AowBox map_area;             // the map viewport (the window's client)
    };

    // Every component's Alignment block, straight from the DFMs. `mk` keeps the
    // tables readable; the argument order matches AowAlign.
    static AowAlign mk(int aw, int ah, int lox, int rox, int tox, int box,
                       int w, int h) {
        AowAlign a;
        a.aw = aw; a.ah = ah; a.lox = lox; a.rox = rox; a.tox = tox;
        a.box = box; a.w = w; a.h = h;
        return a;
    }

    GameLayout game_layout() const {
        GameLayout L;
        const int W = disp_w, H = disp_h;

        // --- TTBWindow: the bar (awBoth/ahBottom, WidthPercent 100, height 48),
        //     TaskPanel (lox 48 / rox 86 / tox 7, height 18) and the five tabs
        //     (awLeft, WidthPercent 14, LeftOffPercent 0/14/28/42/56).
        L.bar = aow_realign(mk(3, 2, 0, 0, 0, 0, W, kTbBarH), 0, 0, W, H);
        L.taskpanel = aow_realign(mk(3, 1, 48, 86, 7, 0, 0, kTbTabH), L.bar.x,
                                  L.bar.y, L.bar.w, L.bar.h);
        for (int i = 0; i < kTbTabCount; ++i) {
            AowAlign a = mk(1, 1, kTbTabOffPx[i], 0, 0, 0, kTbTabW, kTbTabH);
            a.lo = kTbTabOffPct[i];
            a.wp = 14;
            L.tab[i] = aow_realign(a, L.taskpanel.x, L.taskpanel.y,
                                   L.taskpanel.w, L.taskpanel.h);
        }
        // StartMenuBar (4,7 40x38), TBNextButton (awRight rox 46, 26x38),
        // TurnButton (awRight rox 3, 41x38) and the gold / mana read-outs
        // (awRight rox 148 / 76, ahBottom box 1, 69x21).
        L.start = aow_realign(mk(1, 1, kTbStartX, 0, kTbStartY, 0, 40, kTbStartH),
                              L.bar.x, L.bar.y, L.bar.w, L.bar.h);
        L.next = aow_realign(mk(2, 1, 0, 46, 7, 0, kTbNextW, kTbNextH), L.bar.x,
                             L.bar.y, L.bar.w, L.bar.h);
        L.turn = aow_realign(mk(2, 1, 0, 3, 7, 0, kTbTurnW, kTbTurnH), L.bar.x,
                             L.bar.y, L.bar.w, L.bar.h);
        L.gold = aow_realign(mk(2, 2, 0, 148, 0, 1, kTbResW, kTbResH), L.bar.x,
                             L.bar.y, L.bar.w, L.bar.h);
        L.mana = aow_realign(mk(2, 2, 0, 76, 0, 1, kTbResW, kTbResH), L.bar.x,
                             L.bar.y, L.bar.w, L.bar.h);
        // The turn timer (TCTimerImg / TCTimeImg) and the player-indicator
        // strip (PInd), all anchored to the bar's right edge.
        L.timer = aow_realign(mk(2, 2, 0, kTbTimeBodyRox, 0, kTbTimeBodyBox,
                                 kTbTimeBodyW, kTbTimeBodyH), L.bar.x,
                              L.bar.y, L.bar.w, L.bar.h);
        L.timeicon = aow_realign(mk(2, 2, 0, kTbTimeIcnRox, 0, kTbTimeIcnBox,
                                    kTbTimeIcnW, kTbTimeIcnH), L.bar.x,
                                 L.bar.y, L.bar.w, L.bar.h);
        L.pind = aow_realign(mk(2, 2, 0, kTbPindRox, 0, kTbPindBox, kTbPindW,
                                kTbPindH), L.bar.x, L.bar.y, L.bar.w, L.bar.h);
        // The map window (TMWindow.MapWindow, awLeft/ahTop): the Main Map fills
        // the area to the left of the 200 px floater column and above the 48 px
        // bar - exactly 440x432 at the design size. Its frame is the GenericF
        // MapWindow.BMP 9-slice; the client box (the camera's viewport) is the
        // window inset by the frame's 12 px (AoW.exe 0x00451268 / 0x00450de8).
        L.map_window = AowBox{0, 0, W - kFloatW, H - kTbBarH};
        L.map_area = AowBox{kMapBorder, kMapBorder,
                            L.map_window.w - 2 * kMapBorder,
                            L.map_window.h - 2 * kMapBorder};

        // --- TSWindow.ScannerWindow (awRight/ahTop, 200x216): it keeps its
        //     pixel size and just moves to the right edge as the window grows.
        L.scan = aow_realign(mk(2, 1, 0, 0, 0, 0, kFloatW, kFloatH), 0, 0, W, H);
        L.scan_frame = AowBox{L.scan.x + kFloatBorder, L.scan.y + kFloatTop,
                              L.scan.w - 2 * kFloatBorder,
                              L.scan.h - kFloatTop - kFloatBorder};
        L.scan_icon = aow_realign(mk(1, 1, 2, 0, 2, 0, 19, 16), L.scan.x,
                                  L.scan.y, L.scan.w, L.scan.h);
        L.scan_title = aow_realign(mk(1, 1, 25, 0, 4, 0, 0, 0), L.scan.x,
                                   L.scan.y, L.scan.w, L.scan.h);
        L.scan_help = aow_realign(mk(2, 1, 0, 20, 1, 0, 15, 16), L.scan.x,
                                  L.scan.y, L.scan.w, L.scan.h);
        L.scan_min = aow_realign(mk(2, 1, 0, 5, 1, 0, 15, 16), L.scan.x,
                                 L.scan.y, L.scan.w, L.scan.h);
        // TScanner (awBoth/ahBoth, lox 30 / rox 39 / tox 32 / box 32).
        L.scan_map = aow_realign(mk(3, 3, 30, 39, 32, 32, 0, 0), L.scan.x,
                                 L.scan.y, L.scan.w, L.scan.h);
        // ScannerTab (awBoth/ahBottom, lox 31 / rox 24 / box 19, height 15).
        {
            const AowBox t = aow_realign(mk(3, 2, 31, 24, 0, 19, 0, kScanTabH),
                                         L.scan.x, L.scan.y, L.scan.w, L.scan.h);
            const int half = t.w / 2;
            L.scan_tab[0] = AowBox{t.x, t.y, half, t.h};
            L.scan_tab[1] = AowBox{t.x + half, t.y, t.w - half, t.h};
        }
        // BuildingCheck / ArmyCheck / NaturalCheck (awRight rox 25, tox 37/52/67).
        for (int i = 0; i < 3; ++i) {
            L.scan_chk[i] = aow_realign(
                mk(2, 1, 0, 25, kScanChkY[i], 0, kScanChkSize, kScanChkSize),
                L.scan.x, L.scan.y, L.scan.w, L.scan.h);
        }

        // --- TPInfoW.PInfoWin (awRight/ahBottom, BottomOffset 48, 200x216):
        //     also keeps its pixel size and rides the bottom edge.
        L.events = aow_realign(mk(2, 2, 0, 0, 0, kTbBarH, kFloatW, kFloatH), 0,
                               0, W, H);
        L.event_icon = aow_realign(mk(1, 1, 2, 0, 2, 0, 19, 16), L.events.x,
                                   L.events.y, L.events.w, L.events.h);
        L.event_title = aow_realign(mk(1, 1, 25, 0, 4, 0, 0, 0), L.events.x,
                                    L.events.y, L.events.w, L.events.h);
        L.event_help = aow_realign(mk(2, 1, 0, 32, 1, 0, 15, 16), L.events.x,
                                   L.events.y, L.events.w, L.events.h);
        L.event_min = aow_realign(mk(2, 1, 0, 4, 1, 0, 15, 16), L.events.x,
                                  L.events.y, L.events.w, L.events.h);
        L.event_lbl = aow_realign(mk(4, 1, 0, 0, 88, 0, 0, 0), L.events.x,
                                  L.events.y, L.events.w, L.events.h);
        // PInfoPnl (awBoth/ahTop, lox 6 / rox 6 / tox 19, height 66).
        L.event_pnl = aow_realign(mk(3, 1, 6, 6, 19, 0, 0, kEventPnlH),
                                  L.events.x, L.events.y, L.events.w,
                                  L.events.h);
        // FaceFr (1,1 52x64) / ShieldFr (awRight 136,1 52x64) / InfoFr
        // (awBoth 52,0 84x46) / TurnFr (awBoth/ahBottom 52, box 1, 84x20).
        L.face = aow_realign(mk(1, 1, kEventFaceX, 0, kEventFaceY, 0,
                                kEventFaceW, kEventFaceH),
                             L.event_pnl.x, L.event_pnl.y, L.event_pnl.w,
                             L.event_pnl.h);
        L.shield = aow_realign(mk(2, 1, 0, 0, kEventFaceY, 0, kEventFaceW,
                                  kEventFaceH),
                               L.event_pnl.x, L.event_pnl.y, L.event_pnl.w,
                               L.event_pnl.h);
        L.info = aow_realign(mk(3, 1, kEventInfoX, kEventInfoX, 0, 0,
                                kEventInfoW, kEventInfoH),
                             L.event_pnl.x, L.event_pnl.y, L.event_pnl.w,
                             L.event_pnl.h);
        L.turnfr = aow_realign(mk(3, 2, kEventInfoX, kEventInfoX, 0, 1,
                                  kEventInfoW, kEventTurnH),
                               L.event_pnl.x, L.event_pnl.y, L.event_pnl.w,
                               L.event_pnl.h);
        // The banner labels: awCenter (x = the cell's centre) with the DFM's
        // TopOffset (Player 6, "of the" 18, Race 31); Turn is awCenter/ahBottom.
        L.player = aow_realign(mk(4, 1, 0, 0, 6, 0, 0, 0), L.event_pnl.x,
                               L.event_pnl.y, L.event_pnl.w, L.event_pnl.h);
        L.leadl = aow_realign(mk(4, 1, 0, 0, 18, 0, 0, 0), L.event_pnl.x,
                              L.event_pnl.y, L.event_pnl.w, L.event_pnl.h);
        L.race = aow_realign(mk(4, 1, 0, 0, 31, 0, 0, 0), L.event_pnl.x,
                             L.event_pnl.y, L.event_pnl.w, L.event_pnl.h);
        L.day = aow_realign(mk(4, 2, 0, 0, 0, 5, 0, 11), L.event_pnl.x,
                            L.event_pnl.y, L.event_pnl.w, L.event_pnl.h);
        // EventFr (awBoth/ahBoth, lox 6 / rox 6 / tox 100 / box 6).
        L.list = aow_realign(mk(3, 3, 6, 6, 100, 6, 0, 0), L.events.x,
                             L.events.y, L.events.w, L.events.h);

        return L;
    }

    // In-game screen hit test, using the same resolved boxes the renderer draws
    // with so the two can never drift:
    //   0..4 = the five bar tabs (World Map, Realm, Magic, Relations, Events)
    //   5/6  = the scanner's Surface / Caverns tabs
    //   7    = the next-turn button
    //   8    = the scanner's minimize button
    //   -1   = none
    // -------------------------------------------- the Party window (TControlWin)
    // The eight unit slots of the selected army in two rows of four, wrapped in
    // the `ControlWindow` floater. Every box is the DFM's own Alignment block
    // run through ReAlignComp (the same `aow_realign` the rest of the screen
    // uses), so the window keeps its design pixel size and only rides the right
    // edge - which is what a `Resizable = True` but 0-percent floater does.
    struct PartyLayout {
        AowBox win;                  // ControlWindow
        AowBox title, title_icon;    // CWTitle / CTitleIcon
        AowBox help, center, min, close;  // the title-bar buttons
        AowBox t1, t2, t3, t4;       // PartyT1Pnl / T2Pnl / T3Pnl / T4Pnl
        AowBox bp, b2, ctl;          // PartyBPnl / PartyB2Pbl / PControlPnl
        AowBox frame[kPartySlots];   // Unit1..8Frame
        AowBox gge[kPartySlots];     // Unit1..8Gge   (the hit-point bar)
        AowBox pill[kPartySlots];    // Unit1..8Chk   (the leave-behind pill)
        AowBox micon[kPartySlots];   // U1..8MIcon
        AowBox moves_lbl, party_icon, status_lbl;
        AowBox prev, guard, move, next;
    };

    static AowAlign pmk2(int aw, int ah, int lo, int ro, int to, int bo, int lox,
                         int rox, int tox, int box, int w, int h) {
        AowAlign a;
        a.aw = aw; a.ah = ah; a.lo = lo; a.ro = ro; a.to = to; a.bo = bo;
        a.lox = lox; a.rox = rox; a.tox = tox; a.box = box;
        a.w = w; a.h = h;
        return a;
    }

    PartyLayout party_layout() const {
        PartyLayout P;
        const int x0 = disp_w - kPartyW, y0 = kPartyTop;
        const int W = kPartyW, H = kPartyH;
        const AowBox win{x0, y0, W, H};
        P.win = win;
        auto rel = [&](const AowAlign& a) { return aow_realign(a, x0, y0, W, H); };
        // The title bar: CWTitle (Age10, clBlack) at (25,4), the shield icon at
        // (2,2) and the four 15x16 buttons anchored to the right edge
        // (RightOffset 46 help / 32 center / 18 min / 4 close).
        P.title = rel(pmk2(1, 1, 0, 0, 0, 0, 25, 0, 4, 0, 0, 0));
        P.title_icon = rel(pmk2(1, 1, 0, 0, 0, 0, 2, 0, 2, 0, 19, 16));
        P.help = rel(pmk2(2, 1, 0, 0, 0, 0, 0, 46, 1, 0, 15, 16));
        P.center = rel(pmk2(2, 1, 0, 0, 0, 0, 0, 32, 1, 0, 15, 16));
        P.min = rel(pmk2(2, 1, 0, 0, 0, 0, 0, 18, 1, 0, 15, 16));
        P.close = rel(pmk2(2, 1, 0, 0, 0, 0, 0, 4, 1, 0, 15, 16));
        // The four slot panels (left/right offsets 4, so the cells sit on a
        // 48 px pitch): T1 (units 1..4) and T3 (5..8) hold the portrait cells,
        // T2 / T4 the pills and move icons directly under them.
        P.t1 = rel(pmk2(3, 3, 0, 0, 10, 68, 4, 4, 0, 0, 0, 49));
        P.t2 = rel(pmk2(3, 1, 0, 0, 34, 0, 4, 4, 0, 0, 0, 18));
        P.t3 = rel(pmk2(3, 3, 0, 0, 44, 34, 4, 4, 0, 0, 0, 48));
        P.t4 = rel(pmk2(3, 1, 0, 0, 68, 0, 4, 4, 0, 0, 0, 18));
        P.bp = rel(pmk2(3, 1, 0, 50, 79, 0, 4, 0, 0, 0, 0, 18));
        P.b2 = rel(pmk2(3, 1, 50, 0, 79, 0, 0, 4, 0, 0, 0, 18));
        P.ctl = rel(pmk2(3, 2, 0, 0, 78, 0, 3, 3, 0, kPartyBorder, 0, 20));
        for (int s = 0; s < kPartySlots; ++s) {
            const int col = s % 4, rof = (3 - col) * 25;
            const AowBox& cell = (s < 4) ? P.t1 : P.t3;
            const AowBox& row = (s < 4) ? P.t2 : P.t4;
            P.frame[s] = aow_realign(
                pmk2(3, 3, col * 25, rof, 0, 0, 1, 1, 0, 0, 46, 48), cell.x,
                cell.y, cell.w, cell.h);
            // The hit-point gauge sits on the cell's bottom edge, centred.
            P.gge[s] = aow_realign(
                pmk2(3, 2, col * 25, rof, 0, 0, 12, 12, 0, 3, 24, 2), cell.x,
                cell.y, cell.w, cell.h);
            P.pill[s] = aow_realign(
                pmk2(3, 1, col * 25, rof, 0, 0, 1, 1, 0, 0, 46, 18), row.x,
                row.y, row.w, row.h);
            P.micon[s] = aow_realign(
                pmk2(1, 1, col * 25, 0, 0, 0, 3, 0, 0, 0, 32, 17), row.x,
                row.y, row.w, row.h);
        }
        // MovesLbl (awLeft/ahTop, LeftOffPercent 40, TopOffset 4) and MoveIcon
        // (awLeft/ahCenter, LeftOffPercent 5, LeftOffset 2, 32x17) in PartyBPnl;
        // PartyStatusLbl centred on PartyB2Pbl + LeftOffset 15.
        P.moves_lbl = aow_realign(pmk2(1, 1, 40, 0, 4, 0, 0, 0, 0, 0, 0, 0),
                                  P.bp.x, P.bp.y, P.bp.w, P.bp.h);
        P.party_icon = aow_realign(
            pmk2(1, 4, 5, 0, 0, 0, 2, 0, 0, 0, 32, 17), P.bp.x, P.bp.y, P.bp.w,
            P.bp.h);
        P.status_lbl = aow_realign(
            pmk2(4, 1, 0, 0, 4, 0, 15, 0, 0, 0, 0, 0), P.b2.x, P.b2.y, P.b2.w,
            P.b2.h);
        // PControlPnl's row: PPrev (0..15%), PGuard (15..50%), PMove (49..85%)
        // and PNext (85..100%), each with its own 1 px Left/Right offset.
        P.prev = aow_realign(pmk2(3, 2, 0, 85, 0, 0, 1, 0, 0, 0, 29, 20),
                             P.ctl.x, P.ctl.y, P.ctl.w, P.ctl.h);
        P.guard = aow_realign(pmk2(3, 2, 15, 50, 0, 0, 0, 0, 0, 0, 68, 20),
                              P.ctl.x, P.ctl.y, P.ctl.w, P.ctl.h);
        P.move = aow_realign(pmk2(3, 2, 49, 15, 0, 0, 1, 0, 0, 0, 69, 20),
                             P.ctl.x, P.ctl.y, P.ctl.w, P.ctl.h);
        P.next = aow_realign(pmk2(3, 2, 85, 0, 0, 0, 0, 1, 0, 0, 29, 20),
                             P.ctl.x, P.ctl.y, P.ctl.w, P.ctl.h);
        return P;
    }

    // The selected army (the const walk `army_on_hex` cannot do from within a
    // const renderer). The window is only up while one is selected.
    const aow::MapHs* party_army() const {
        // The incoming army of a merge is off the map while its sprite walks;
        // the Party window follows it (`TSelectedArmy.Select` on the temporary
        // moving `TArmyHS`) until `FinishMove` combines it on arrival.
        if (mv_merge_pending) return &mv_merge_moving;
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size() ||
            mv_sel_x < 0) {
            return nullptr;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        if (static_cast<std::uint32_t>(mv_sel_x) >= lv.width ||
            static_cast<std::uint32_t>(mv_sel_y) >= lv.height) {
            return nullptr;
        }
        const aow::MapTile& t =
            lv.tiles[static_cast<std::size_t>(mv_sel_y) * lv.width + mv_sel_x];
        for (const aow::MapHs& hs : t.objects) {
            if (hs.class_id == aow::kMapArmyHsClassId) return &hs;
        }
        return nullptr;
    }

    // Slot `slot`'s unit: the window's slots are the army's units in order
    // (`TArmy` holds at most `TArmy.MaxSize` = 8 children), so slot n is unit n.
    const aow::MapUnit* party_unit(int slot) const {
        const aow::MapHs* a = party_army();
        if (a == nullptr || slot < 0 ||
            static_cast<std::size_t>(slot) >= a->units.size()) {
            return nullptr;
        }
        return &a->units[static_cast<std::size_t>(slot)];
    }

    // True when slot `slot`'s unit marches with the party - bit `slot` of the
    // travelling mask (`TControlWin+0x178`); an empty slot is never selected.
    bool party_travels(int slot) const {
        if (party_unit(slot) == nullptr) return false;
        return (party_sel & (1u << slot)) != 0;
    }

    // `AoWE.TArmy.MovePoints@0x5578d8d0` over the travelling units: 0 when none
    // travels, else min(0x32, the smallest *remaining* allowance) - the engine's
    // `MovesLbl` shows what is left this turn, not the units' maximum.
    int party_move_points() const {
        std::vector<std::uint8_t> mps;
        for (int s = 0; s < kPartySlots; ++s) {
            const aow::MapUnit* u = party_unit(s);
            if (u == nullptr || !party_travels(s)) continue;
            mps.push_back(aow::unit_move_points_left(mv_cat, *u));
        }
        if (mps.empty()) return 0;
        return aow::army_move_points(mps);
    }

    // `AoWE.TArmy.GetPartyMoves@0x5578da60`: the byte the party's movement-type
    // icon is resolved from. It is the **AND** of the travelling units' own
    // move-type bytes - the type the whole party shares - plus the low bit of
    // the engine's "kind" state machine (`uVar9`: 1 -> {0, 2} -> 0 -> gave up).
    // (A walk-only party therefore answers 1, the boot; the union would answer a
    // tree as soon as one member can walk the forest.)
    std::uint8_t party_moves_byte() const {
        std::uint8_t and_all = 0xff;
        int kind = 1;
        bool any = false;
        for (int s = 0; s < kPartySlots; ++s) {
            const aow::MapUnit* u = party_unit(s);
            if (u == nullptr || !party_travels(s)) continue;
            const std::uint8_t v = aow::unit_move_types(*u);
            and_all &= v;
            any = true;
            if (kind == 1) {
                if ((v & 2) == 0) kind = (v & 4) != 0 ? 2 : 0;
            } else if (kind == 2) {
                if ((v & 6) == 0) kind = 0;
            } else if (kind == 0) {
                if ((v & 0x7b) == 0) kind = -1;   // the engine's 0xffffffff
            }
        }
        if (!any) return 0xff;
        std::uint8_t b = and_all;
        if (kind >= 0) b |= static_cast<std::uint8_t>(1u << kind);
        return b;
    }

    // `AoW.exe FUN_004541b0`: the party's movement-type icon - the IconMove
    // library's even ids, tested in the engine's own order.
    static int party_icon_frame(std::uint8_t t) {
        int v = 0;
        if (t & 0x08) v = 2;
        if (t & 0x20) v = 4;
        if (t & 0x04) v = 6;
        if ((t & 0x05) == 0x05) v = 8;
        if ((t & 0x0c) == 0x0c) v = 10;
        if ((t & 0x24) == 0x24) v = 12;
        if ((t & 0x28) == 0x28) v = 14;
        if ((t & 0x2c) == 0x2c) v = 16;
        if (t & 0x02) v = 18;
        return v;
    }

    // `AoWE.TArmy.GetPartyStatus@0x5578f584`. The engine first runs a battle
    // predictor over the travelling units (its combat cases answer psUnruly or
    // psUnrest, which the world-map port has no equivalent of), then bands the
    // party's lowest morale *value* (+0x26, the byte the map stores):
    //   no unit travelling, or the lowest value < 61 -> psStable  (2)
    //   lowest value < 81                             -> psContent (3)
    //   otherwise                                     -> psCheerful (4)
    // The bands come from `MoraleValueToMorale@0x5575ad64`; note that the
    // Tutorial party's stored values (50/50/50/90) answer psStable - the
    // "Stable" bin/partyRef.png shows.
    int party_status() const {
        int count = 0, min_morale = 100;
        for (int s = 0; s < kPartySlots; ++s) {
            const aow::MapUnit* u = party_unit(s);
            if (u == nullptr || !party_travels(s)) continue;
            ++count;
            if (static_cast<int>(u->morale) < min_morale) {
                min_morale = static_cast<int>(u->morale);
            }
        }
        if (count == 0 || min_morale < 61) return 2;   // psStable
        if (min_morale < 81) return 3;                 // psContent
        return 4;                                      // psCheerful
    }

    // The window's refresh (`FUN_004062a0`, CheckEnd). The engine reads the
    // selected party's travelling mask into +0x178 and then repaints every
    // widget from it; the port has no widget tree, so only the mask has to be
    // kept - the renderer derives each cell from it. This drops the bits of
    // units that are no longer part of the army, then re-folds the army's
    // movement from the units that still travel, so leaving a unit behind also
    // revalidates the party's budget, move types and move-point table.
    void party_refresh() {
        if (!game_show_party) return;
        if (party_army() == nullptr) return;
        std::uint8_t keep = 0;
        for (int s = 0; s < kPartySlots; ++s) {
            if (party_unit(s) != nullptr && (party_sel & (1u << s)) != 0) {
                keep |= static_cast<std::uint8_t>(1u << s);
            }
        }
        party_sel = keep;
        mv_fold_selection();
    }

    // The party window's control ids (the renderer and the hit test share them).
    enum {
        kPartyClose = 0, kPartyMin, kPartyCenter, kPartyHelp,
        kPartyFrame0 = 10,   // slot s -> kPartyFrame0 + s (click = focus)
        kPartyPill0 = 20,    // slot s -> kPartyPill0 + s  (click = stay behind)
        kPartyPrev = 30, kPartyGuard, kPartyMove, kPartyNext, kPartyTitle
    };

    int party_hit_test(int x, int y) const {
        const PartyLayout P = party_layout();
        auto in = [](const AowBox& b, int px, int py) {
            return b.x <= px && px < b.x + b.w && b.y <= py && py < b.y + b.h;
        };
        if (!in(P.win, x, y)) return -1;
        if (in(P.close, x, y)) return kPartyClose;
        if (in(P.min, x, y)) return kPartyMin;
        if (in(P.center, x, y)) return kPartyCenter;
        if (in(P.help, x, y)) return kPartyHelp;
        for (int s = 0; s < kPartySlots; ++s) {
            if (in(P.pill[s], x, y)) return kPartyPill0 + s;
        }
        for (int s = 0; s < kPartySlots; ++s) {
            if (in(P.frame[s], x, y)) return kPartyFrame0 + s;
        }
        if (in(P.prev, x, y)) return kPartyPrev;
        if (in(P.guard, x, y)) return kPartyGuard;
        if (in(P.move, x, y)) return kPartyMove;
        if (in(P.next, x, y)) return kPartyNext;
        if (y < P.win.y + 20) return kPartyTitle;   // the drag strip
        return -1;
    }

    // `TAoWHSMap.SelectNextArmy@0x55775fa0` / `SelectPrevArmy@0x55775fc4`: walk
    // the current level's armies of the seated player (the engine's own army
    // order) and select the next one, wrapping around. The party window follows
    // the new selection; a fresh party starts with every unit travelling
    // (`TSelectedArmy.SetSelection` after `TArmy.Select`).
    void game_select_army(int delta) {
        // The move animation owns the selection until it arrives (a merge's
        // incoming party is not on the map meanwhile).
        if (mv_anim) return;
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size() ||
            mv_human == 0xff || delta == 0) {
            return;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        std::vector<std::pair<int, int>> mine;
        for (std::uint32_t y = 0; y < lv.height; ++y) {
            for (std::uint32_t x = 0; x < lv.width; ++x) {
                const aow::MapTile& t =
                    lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                for (const aow::MapHs& hs : t.objects) {
                    if (hs.class_id != aow::kMapArmyHsClassId ||
                        hs.units.empty() || hs.owner != mv_human) {
                        continue;
                    }
                    mine.emplace_back(static_cast<int>(x),
                                      static_cast<int>(y));
                }
            }
        }
        const int n = static_cast<int>(mine.size());
        if (n == 0) return;
        // `TPlayer+0xb4`: the index the last SelectNext/SelectPrevArmy stopped
        // on. Both routines continue from there (falling back to 0 when it is
        // out of range) and advance *before* testing, so a repeated click walks
        // the whole list and wraps.
        int idx = tb_next_index;
        if (idx < 0 || idx >= n) idx = 0;
        int chosen = -1;
        for (int k = 0; k < n; ++k) {
            idx = ((idx + delta) % n + n) % n;
            const auto& pos = mine[static_cast<std::size_t>(idx)];
            const aow::MapHs* a = army_on_hex(pos.first, pos.second);
            if (a == nullptr) continue;
            // `TArmy.MovePoints(army, 0xff) >= 2` and `(TArmy+0x28 & 3) == 0`
            // - the army has move points left this turn and is not on guard.
            if (army_remaining_mp(*a) >= 2 &&
                (a->army_flags & 3) == 0) {
                chosen = idx;
                break;
            }
        }
        if (chosen < 0) {
            // No army answers: the engine's `UnselectLocation` - the map's
            // selection is cleared (the party floater closes with it).
            mv_reset();
            tb_next_index = -1;
            game_say("no army to select");
            return;
        }
        game_sel_hx = mine[static_cast<std::size_t>(chosen)].first;
        game_sel_hy = mine[static_cast<std::size_t>(chosen)].second;
        mv_select(game_sel_hx, game_sel_hy);
        tb_next_index = chosen;
        game_update_path();
        party_sel = static_cast<std::uint8_t>(mv_selected ? 0xff : 0);
        party_refresh();
        game_say("party " + std::to_string(chosen + 1) + "/" + std::to_string(n));
    }

    // The party window's click handling, one entry per DFM handler:
    //   Unit1FrameMouseDown  FUN_00406040  focus the clicked cell's unit
    //   Unit1ChkChange       FUN_004060d8  rebuild the travelling mask
    //   Unit1ChkMouseDown    FUN_004066bc  centre the map on the party
    //   CloseClick/          FUN_00405fe4  hide the floater (which deselects)
    //   CWMinIconClick       FUN_00405fc4
    //   CenterClick          FUN_004066f4  centre + the centre animation
    //   HelpClick            FUN_00406808  the window's help
    //   PPrevClick/          FUN_00406814  previous / next active party
    //   PNextClick           FUN_00406824
    //   PGuardChange         FUN_00406848  TArmy+0x28 bit 0
    //   PMoveClick           FUN_00406834  arm the party's move
    void party_action(int hit) {
        // The walk is playing: the party is already committed to the move, so
        // its buttons (and closing it) wait for the arrival.
        if (mv_anim) return;
        if (hit >= kPartyPill0 && hit < kPartyPill0 + kPartySlots) {
            // `FUN_004060d8`: a pill click toggles that unit's "travels with the
            // party" state, then the mask is filed back through
            // `TSelectedArmy.SetSelection` and the window repainted.
            const int s = hit - kPartyPill0;
            if (party_unit(s) == nullptr) return;
            // `FUN_004066bc` (Unit1ChkMouseDown) centres the map on the party
            // first; the engine's left click then only toggles the pill.
            game_sel_hx = mv_sel_x;
            game_sel_hy = mv_sel_y;
            game_center_selection();
            party_sel ^= static_cast<std::uint8_t>(1u << s);
            party_refresh();          // re-folds the travelling units
            game_update_path();       // ... and the trail's budget with them
            // The new fold, so a test can pin that a deselected unit stops
            // counting towards `TArmy.MovePoints` (its table and move types too).
            std::printf("game_party_fold=slot=%d sel=%d budget=%d\n", s + 1,
                        static_cast<int>(party_sel),
                        static_cast<int>(mv_army.budget));
            std::fflush(stdout);
            game_say("unit " + std::to_string(s + 1) +
                     (party_travels(s) ? " travels with the party"
                                       : " is left behind"));
            return;
        }
        if (hit >= kPartyFrame0 && hit < kPartyFrame0 + kPartySlots) {
            const int s = hit - kPartyFrame0;
            if (party_unit(s) != nullptr) {
                party_focus = s;
                game_say("party focus unit " + std::to_string(s + 1));
            }
            return;
        }
        switch (hit) {
        case kPartyClose:
        case kPartyMin:
            // Closing the party window deselects the current party
            // (`mld_strings.txt`, "The Party Floater").
            game_show_party = false;
            mv_reset();
            return;
        case kPartyCenter:
            game_center_selection();
            return;
        case kPartyHelp:
            // `FUN_00406808`: `FUN_0040471c(0x1f7)` opens this window's help.
            game_say("help: party window (context 0x1f7)");
            return;
        case kPartyPrev:
            game_select_army(-1);
            return;
        case kPartyNext:
            game_select_army(+1);
            return;
        case kPartyGuard:
            // PGuardChange: TArmy+0x28 bit 0 - "removes the party from the
            // next-party loop". The party window shows the selected army, so
            // the flag is stored on that army (`SelectNextArmy` tests `& 3`).
            party_guard = !party_guard;
            if (aow::MapHs* a = army_on_hex(mv_sel_x, mv_sel_y)) {
                if (party_guard) {
                    a->army_flags = static_cast<std::uint8_t>(a->army_flags | 1u);
                } else {
                    a->army_flags = static_cast<std::uint8_t>(a->army_flags & ~1u);
                }
            }
            game_say(party_guard ? "party on guard" : "party active");
            return;
        case kPartyMove:
            // PMoveClick: `TSelectedArmy`'s Move - the port's move selection is
            // the map click's own trail, so this arms it.
            if (mv_selected) {
                game_update_path();
                game_say("party ready to move");
            }
            return;
        default:
            return;
        }
    }

    // The height of the Age10 glyphs, for vertically centring the pill numbers
    // and the button captions (the engine draws a label's text at its box top,
    // and a pill's/button's own caption vertically centred).
    int party_text_h() const {
        for (unsigned char c : std::string("0123456789<>AGM")) {
            const Glyph& g = glyphs10b[c];
            if (g.ok && g.face.h) return static_cast<int>(g.face.h);
        }
        return 10;
    }

    // One slot's portrait, cached (a decode per frame would be wasteful and the
    // pose cannot change while the window is up).
    mutable std::vector<Sprite> party_portraits;
    mutable int party_portrait_x = -2, party_portrait_y = -2;

    void party_refresh_portraits() const {
        const aow::MapHs* a = party_army();
        const std::size_t n = a == nullptr ? 0 : a->units.size();
        if (party_portrait_x == mv_sel_x && party_portrait_y == mv_sel_y &&
            party_portraits.size() == n) {
            return;
        }
        party_portrait_x = mv_sel_x;
        party_portrait_y = mv_sel_y;
        party_portraits.assign(n, Sprite());
        for (std::size_t i = 0; i < n; ++i) {
            std::vector<std::uint8_t> rgba;
            int w = 0, h = 0, vx = 0, vy = 0;
            std::string err;
            // The standing pose facing the party's own formation facing (the
            // same one `mapviz` draws the army with).
            if (aow::unit_pose_rgba(mv_cat, root_dir, a->units[i],
                                    a->army_direction, false, 0, 0, rgba, w, h,
                                    vx, vy, err)) {
                Sprite s;
                s.rgba = std::move(rgba);
                s.w = static_cast<std::uint32_t>(w);
                s.h = static_cast<std::uint32_t>(h);
                s.ok = true;
                party_portraits[i] = std::move(s);
            }
        }
    }

    // The Party floater (TControlWin). Every element is the entry its DFM names
    // (`Int\Units.ILB` for the cells and pills, `Int\IconMove.ILB` for the
    // movement-type icons, `GenericI` / `GenericB` for the buttons) at the box
    // ReAlignComp resolves, drawn in the DFM's own declaration order so the
    // empty-slot overlays sit above the cells exactly as the engine's
    // priorities do.
    void draw_game_party(Canvas& cv, const PartyLayout& P) const {
        if (!game_show_party) return;
        party_refresh_portraits();
        // 1. The window body (its ILTexture is GenericT 2) and its 8-piece
        //    frame (GenericF 0..7, the MainFloaterSetR 9-slice).
        if (float_body.ok) {
            cv.blit_stretch(float_body, P.win.x, P.win.y, P.win.w, P.win.h);
        }
        blit9_box(cv, float_frame, P.win);
        // 2. The title bar: the shield icon, the "Party" label and the four
        //    buttons (up / lit / down - the hovered or pressed one lights).
        cv.blit_stretch(party_icon[0], P.title_icon.x, P.title_icon.y,
                        P.title_icon.w, P.title_icon.h);
        text_at(cv, glyphs10b, P.title.x, P.title.y, "Party", 24, 20, 16,
                false, false);
        struct TBtn {
            const AowBox* box;
            const Sprite* art;
            int id;
        };
        const TBtn tbtn[4] = {{&P.help, party_help, kPartyHelp},
                              {&P.center, party_center, kPartyCenter},
                              {&P.min, party_min, kPartyMin},
                              {&P.close, party_close, kPartyClose}};
        for (const TBtn& b : tbtn) {
            int st = 0;
            if (party_pressed == b.id) st = 2;
            else if (party_hover == b.id) st = 1;
            cv.blit_stretch(b.art[st], b.box->x, b.box->y, b.box->w, b.box->h);
        }
        // 3. The foot panels (PartyBPnl / PartyB2Pbl / PControlPnl): the
        //    olive-grey PartyFr 3-piece (Units 9..11) with the GenericT 14 body
        //    showing through its transparent middle.
        if (party_foot[1].ok) {
            const AowBox* panels[3] = {&P.bp, &P.b2, &P.ctl};
            for (const AowBox* b : panels) {
                blit3_box(cv, party_foot, *b);
            }
        }
        // 4. The eight cells, in slot order (units 1..4 then 5..8).
        for (int s = 0; s < kPartySlots; ++s) {
            const AowBox& c = P.frame[s];
            const aow::MapUnit* u = party_unit(s);
            if (u == nullptr) {
                // An empty slot draws the Units 0..7 frame filled with the id-8
                // hatch (Emp{n}Fr, a `frFilledBox`), which covers whatever the
                // cell frame would have shown.
                if (party_empty_tex.ok) {
                    cv.blit_tile(party_empty_tex, c.x, c.y, c.w, c.h, false);
                }
                blit9_box(cv, party_empty, c);
                continue;
            }
            // A unit that stays behind draws the Units 26..33 frame instead.
            blit9_box(cv, party_travels(s) ? party_box : party_box_left, c);
        }
        // 5. The portraits and the hit-point bars. `FUN_00405d98` (the frame's
        //    PreDraw) centres the unit's own image in the cell's inset rect -
        //    (x+4, y+3) .. (x+w-3, y+h-7) - and draws the unit's hit-point bar
        //    on the cell's gauge row first, so the bar sits under the sprite.
        for (int s = 0; s < kPartySlots; ++s) {
            const AowBox& c = P.frame[s];
            const aow::MapUnit* u = party_unit(s);
            if (u == nullptr || static_cast<std::size_t>(s) >= party_portraits.size()) {
                continue;
            }
            const Sprite& portrait = party_portraits[static_cast<std::size_t>(s)];
            // The wash and the portrait both use the cell's inset rect:
            // (x+4, y+3) .. (x+w-3, y+h-7).
            const int rx = c.x + 4, ry = c.y + 3;
            const int rw = c.w - 7, rh = c.h - 10;
            // The cell's wash (`FUN_00405d98`: `ImageLib.Get(UnitBGs, unit->vtable
            // [0xfc]() & 0x7f)`, drawn only while the index is inside the
            // library - eight entries). The engine CENTRES the 100x100 image in
            // the rect and draws it 1:1 (ShowClipped), so a cell shows the wash's
            // middle. Every unit of the shipped Tutorial party answers id 1 (the
            // teal-green cell); the exact source field is still open
            // [TODO(RE) `TAbstractUnit` vtable+0xfc], so the port pins id 1.
            // `AOW_PARTY_BG` overrides the cell wash id (a dev knob for sweeping
            // it against bin/partyRef.png; see the sweep in docs/20_PARTY_WINDOW.md).
            if (party_unitbg[1].ok) {
                int id = 1;
                if (const char* e = std::getenv("AOW_PARTY_BG")) id = std::atoi(e);
                if (id >= 0 && id < 8 && party_unitbg[id].ok) {
                    const Sprite& wash = party_unitbg[id];
                    const int wx = rx + (rw - static_cast<int>(wash.w)) / 2;
                    const int wy = ry + (rh - static_cast<int>(wash.h)) / 2;
                    cv.blit(wash, wx, wy, kAlphaFull, rx, ry, rx + rw, ry + rh);
                }
            }
            const AowBox& g = P.gge[s];
            if (party_gauge[0].ok) {
                cv.blit_stretch(party_gauge[0], g.x, g.y, g.w, g.h);
            } else {
                cv.fill(g.x, g.y, g.w, g.h, 40, 20, 20, 255);
            }
            // The unit's hit points arrive at full from the scenario (the port
            // has no damage model yet), so the fill is the whole gauge - which
            // is what the engine's `TAoWGauge.SetPos(hp)` shows for an
            // undamaged unit.
            const int fill = g.w;
            if (party_gauge[1].ok) {
                cv.blit_stretch(party_gauge[1], g.x, g.y, fill, g.h);
            } else {
                cv.fill(g.x, g.y, fill, g.h, 200, 30, 30, 255);
            }
            if (!portrait.ok) continue;
            // `FUN_00405d98` centres the unit's image box in the cell's inset
            // rect and blits it clipped to the rect.
            const int iw = static_cast<int>(portrait.w);
            const int ih = static_cast<int>(portrait.h);
            const int px = rx + (rw - iw) / 2;
            const int py = ry + (rh - ih) / 2;
            cv.blit(portrait, px, py, kAlphaFull, rx, ry, rx + rw, ry + rh);
        }
        // 6. The pills: the Units 20..22 / 23..25 3-piece (up while the unit
        //    travels, down while it stays behind), each carrying its unit's
        //    movement-type icon and its move points.
        for (int s = 0; s < kPartySlots; ++s) {
            const aow::MapUnit* u = party_unit(s);
            if (u == nullptr) continue;
            const int state = party_travels(s) ? 0 : 1;
            if (party_pill[state][1].ok) {
                blit3_box(cv, party_pill[state], P.pill[s]);
            }
            const int icon = party_icon_frame(aow::unit_move_types(*u));
            const int fi = (icon >= 0 && static_cast<std::size_t>(icon) <
                                             move_icons.size())
                               ? icon
                               : 0;
            if (static_cast<std::size_t>(fi) < move_icons.size() &&
                move_icons[static_cast<std::size_t>(fi)].ok) {
                cv.blit_keyed(move_icons[static_cast<std::size_t>(fi)],
                              P.micon[s].x, P.micon[s].y);
            }
            const std::string pts =
                std::to_string(aow::unit_move_points_left(mv_cat, *u));
            const AowBox& b = P.pill[s];
            // The engine draws the pill's caption with the control's own AOWFont
            // (FontModule.Age10) and TextColor (clBlack); the glyphs are blitted
            // with their own ink, so they are drawn keyed. The caption starts at
            // the pill's left + 27 (measured off the reference screen) and sits
            // on the pill's fill.
            text_at(cv, glyphs10b, b.x + 27, b.y + 2, pts, 24, 20, 16, false,
                    false);
        }
        // 7. The foot: the party's movement-type icon, its move points and its
        //    status (`MovesLbl` / `PartyStatusLbl`, FontModule.Age10s, clWhite).
        {
            const int icon = party_icon_frame(party_moves_byte());
            const int fi = (icon >= 0 && static_cast<std::size_t>(icon) <
                                             move_icons.size())
                               ? icon
                               : 0;
            if (static_cast<std::size_t>(fi) < move_icons.size() &&
                move_icons[static_cast<std::size_t>(fi)].ok) {
                cv.blit_keyed(move_icons[static_cast<std::size_t>(fi)],
                              P.party_icon.x, P.party_icon.y);
            }
            // MovesLbl / PartyStatusLbl: FontModule.Age10s in clWhite, blitted
            // as a flat tint off the glyphs' coverage (TImageLibraryFont
            // .SetColor) - the engine's own white read-out look.
            text_at(cv, glyphs10, P.moves_lbl.x, P.moves_lbl.y,
                    std::to_string(party_move_points()), 245, 245, 235, false,
                    false);
            const int st = party_status();
            text_at(cv, glyphs10, P.status_lbl.x, P.status_lbl.y,
                    kPartyStatusText[st < 0 || st > 4 ? 3 : st], 245, 245, 235,
                    true, false);
        }
        // 8. The button row (PControlPnl): PPrev / PGuard / PMove / PNext, each
        //    a GenericB 3-piece with its caption centred (clBlack, clWhite while
        //    pressed).
        {
            struct BBtn {
                const AowBox* box;
                const char* text;
                int id;
            };
            const BBtn bb[4] = {{&P.prev, "<", kPartyPrev},
                                {&P.guard, "Guard", kPartyGuard},
                                {&P.move, "Move", kPartyMove},
                                {&P.next, ">", kPartyNext}};
            for (const BBtn& b : bb) {
                int st = 0;
                if (party_pressed == b.id) st = 2;
                else if (party_hover == b.id) st = 1;
                if (party_btn[st][1].ok) blit3_box(cv, party_btn[st], *b.box);
                // The caption is FontModule.Age10 (clBlack / clWhite while
                // pressed) blitted with the glyphs' own ink, centred.
                const int ty = b.box->y + (b.box->h - party_text_h()) / 2 + 1;
                text_at(cv, glyphs10b, b.box->x + b.box->w / 2, ty, b.text, 24,
                        20, 16, true, false);
            }
        }
    }
    // ---------------------------------------------------------------------
    // The bottom bar's two turn buttons (docs/22_TURN_BUTTONS.md)
    //
    // TBNextButton  - TAOWButton (AoWInterface.dpl), OnClick = TBNextButtonClick
    //                 (AoW.exe 0x004573e8): the shared click sound, then
    //                 TAoWHSMap.SelectNextArmy (AoWEPACK 0x55775fa0).
    // TurnButton    - TAOWTurnButton (AOWTools.dpl unit "TurnButton", class
    //                 instance 0x144 bytes), which draws the 102-frame turn
    //                 animation and fires TurnButtonChange on mouse-DOWN
    //                 (CheckMouseDown 0x59705018). TurnButtonChange (AoW.exe
    //                 0x004574ac) ends or withdraws the seat's turn.
    //
    // The turn loop itself is `aow::GameState::end_turn` (see game_state.*),
    // which reproduces TTurnPlayerControl.EndTurn: the turn is handed on only
    // once its owner has ended it, and a new day starts only when the turn
    // order wraps - i.e. after every player has ended their turn.
    // ---------------------------------------------------------------------

    // The roster entry whose turn it is (TPlayerList.GetPlayers(AoWHSMap+0x140,
    // AoWHSMap+0xa4)), or nullptr.
    const aow::Player* game_current_player() const {
        const std::uint8_t cur = mv_gs.current();
        if (cur == 0xff) return nullptr;
        const std::size_t i = mv_gs.index_of(cur);
        if (i >= mv_gs.players.size()) return nullptr;
        return &mv_gs.players[i];
    }

    static const aow::Player* game_player_by_index(const aow::GameState& gs,
                                                   std::uint8_t idx) {
        const std::size_t i = gs.index_of(idx);
        return i < gs.players.size() ? &gs.players[i] : nullptr;
    }

    // The TurnButton's "player" is the block the library carries: a RACE id,
    // with 12 for "no seat" (the form maps a -1 race to frame block 12).
    static int tb_turn_race_of(const aow::Player* p) {
        if (p == nullptr || p->race == 0xff) return kTbTurnNoRace;
        if (p->race > kTbTurnNoRace) return kTbTurnNoRace;
        return static_cast<int>(p->race);
    }

    // TAOWTurnButton.Draw (AOWTools.dpl 0x59704e44): the picture index is
    // *AnimStart + Frame* (+0x128 + +0x140), except while the button is Checked
    // and no change animation is playing, when it is
    // *AnimStart + 7*CurPlayer + 6* - the block's transition frame.
    int tb_turn_draw_frame() const {
        if (!tb_turn_animating && tb_turn_checked) {
            const int f = tb_turn_start + kTbTurnRaceStride * tb_turn_cur + 6;
            if (f >= 0 && f < kTbTurnFrameCount) return f;
        }
        return tb_turn_start + tb_turn_frame;
    }


    // TAOWTurnButton.Update (AOWTools.dpl 0x59704cd4), one call per game frame -
    // the window's own Update cadence. The state machine is reproduced verbatim:
    //   mode 0  idle; a CurPlayer that differs from LastPlayer starts the change
    //           from LastPlayer's block at AnimStart;
    //   mode 1  step the frame up until the target block's +5 frame;
    //   mode 2  run the "turn changing" glow frames 0..10;
    //   mode 3  step down from the target block's +5 frame to its first frame,
    //           then settle on it, keep LastPlayer = CurPlayer, clear Animating
    //           and fire OnAnimDone (TurnButtonAnimDone clears Checked).
    void tb_turn_update() {
        switch (tb_turn_mode) {
        case 0:
            if (tb_turn_last == tb_turn_cur) break;
            tb_turn_frame = kTbTurnRaceStride * tb_turn_last;
            tb_turn_start = kTbTurnAnimStart;
            tb_turn_mode = 1;
            tb_turn_animating = true;
            break;
        case 1:
            ++tb_turn_frame;
            if (tb_turn_frame > kTbTurnRaceStride * tb_turn_cur + 5) {
                tb_turn_frame = 0;
                tb_turn_mode = 2;
                tb_turn_start = 0;
            }
            break;
        case 2:
            ++tb_turn_frame;
            if (tb_turn_frame > kTbTurnGlowFrames - 1) {
                tb_turn_mode = 3;
                tb_turn_frame = kTbTurnRaceStride * tb_turn_cur + 5;
                tb_turn_start = kTbTurnAnimStart;
            }
            break;
        case 3:
            --tb_turn_frame;
            if (tb_turn_frame <= kTbTurnRaceStride * tb_turn_cur) {
                tb_turn_frame = kTbTurnRaceStride * tb_turn_cur;
                tb_turn_mode = 0;
                tb_turn_start = kTbTurnAnimStart;
                tb_turn_last = tb_turn_cur;
                tb_turn_animating = false;
                // OnAnimDone -> TurnButtonAnimDone (AoW.exe 0x004572f4).
                tb_turn_checked = false;
            }
            break;
        default:
            tb_turn_mode = 0;
            break;
        }
        if (tb_turn_frame < 0) tb_turn_frame = 0;
        if (tb_turn_frame >= kTbTurnFrameCount) tb_turn_frame = kTbTurnFrameCount - 1;
    }

    // The form's seated-player update (AoW.exe 0x004551b1): the button takes the
    // new seat's race block; with `animate` the class walks there itself, without
    // it the button snaps to the block's first frame (what the form does when a
    // map opens). Either way the Checked state is dropped.
    void tb_turn_set_player(int race, bool animate) {
        if (race < 0 || race > kTbTurnNoRace) race = kTbTurnNoRace;
        tb_turn_checked = false;
        if (!animate) {
            tb_turn_cur = tb_turn_last = race;
            tb_turn_frame = kTbTurnRaceStride * race;
            tb_turn_start = kTbTurnAnimStart;
            tb_turn_mode = 0;
            tb_turn_animating = false;
            return;
        }
        tb_turn_cur = race;
    }

    // TAbstractUnit.NewTurn (@0x55780d4c) refills every unit's move points when
    // its owner's turn begins, so the seat's armies can all march again.
    void mv_reset_army_turn(std::uint8_t owner) {
        for (aow::MapLevel& lv : game_state_map.levels) {
            for (aow::MapTile& t : lv.tiles) {
                for (aow::MapHs& hs : t.objects) {
                    if (hs.class_id != aow::kMapArmyHsClassId) continue;
                    if (hs.owner != owner) continue;
                    hs.army_mp_left = -1;
                    for (aow::MapUnit& u : hs.units) u.move_points_left = -1;
                }
            }
        }
    }

    // The turn loop moved to another player: the TurnButton follows it, the day
    // read-out follows the map, and a human seat's turn start plays its sound
    // and refills its armies (TTurnPlayerControl.NewTurn 0x55756714).
    void game_turn_changed(bool animate) {
        const aow::Player* p = game_current_player();
        tb_turn_set_player(tb_turn_race_of(p), animate);
        game_day = mv_gs.day;
        if (p == nullptr) {
            std::printf("game_turn=none day=%d\n", game_day);
            std::fflush(stdout);
            return;
        }
        std::printf("game_turn=player=%d name=%s day=%d race=%d block=%d\n",
                    static_cast<int>(p->index), p->display_name().c_str(),
                    game_day, static_cast<int>(p->race),
                    kTbTurnAnimStart + kTbTurnRaceStride * tb_turn_cur);
        std::fflush(stdout);
        if (p->index == mv_human) {
            mv_reset_army_turn(mv_human);
            tb_next_index = -1;   // TPlayer+0xb4 starts a fresh seat
            play_game_sfx("SFX\\EVENTS\\NEWTURN.WAV");
            game_say("your turn - day " + std::to_string(game_day));
        }
    }

    // Starts (or re-starts, when the setup control moved the Human to another
    // race) the turn loop on the local seat. The map's own order begins with the
    // independents (entry 0), so the engine's first Activate hands the seated
    // player its turn; the port does the same (`begin_first_turn`).
    void game_begin_turns(bool restart = false) {
        if (!mv_ready || mv_gs.players.empty()) return;
        if (game_turns_started && !restart) return;
        // The day order (`begin_day` = GenerateNewTurnOrder; it does not bump
        // the day - only `new_day` does).
        mv_gs.begin_day(mv_gs.rng_seed);
        if (mv_gs.turn_order.empty()) return;
        if (restart) {
            for (aow::Player& p : mv_gs.players) p.end_turn = false;
            mv_gs.turn_in_order = 0;
        }
        if (mv_human == 0xff) {
            // A roster that seats no local player has no turn to take: park the
            // loop (and the button's colour) on the first seat and leave it be.
            mv_gs.turn_in_order = 0;
            mv_gs.current_player = mv_gs.turn_order.front();
            game_turns_started = false;
            tb_turn_set_player(tb_turn_race_of(game_current_player()), false);
            game_day = mv_gs.day;
            return;
        }
        mv_gs.begin_first_turn(mv_human);
        game_turns_started = true;
        game_turn_changed(false);
    }

    // TurnButtonChange (AoW.exe 0x004574ac): the button ends the *seated*
    // player's turn, or withdraws it when that player already ended. The result
    // of the map's EndTurn/CancelEndTurn is filed back into Checked
    // (`*(Sender+0x5c) + 0x13c`), which is what turns the button into its
    // "turn ended" picture.
    void game_turn_button_click() {
        if (!mv_ready || mv_gs.players.empty() || mv_human == 0xff) return;
        if (tb_turn_animating) return;   // CheckMouseDown's [+0x120] gate
        const aow::Player* seat = game_player_by_index(mv_gs, mv_human);
        const bool ended = seat != nullptr && seat->end_turn;
        tb_turn_checked = !tb_turn_checked;
        if (!ended) {
            const std::uint8_t before = mv_gs.current();
            tb_turn_checked = mv_gs.end_turn(mv_human);
            std::printf("game_endturn=player=%d ok=%d from=%d to=%d day=%d\n",
                        static_cast<int>(mv_human), tb_turn_checked ? 1 : 0,
                        static_cast<int>(before),
                        static_cast<int>(mv_gs.current()), mv_gs.day);
            std::fflush(stdout);
            if (tb_turn_checked) game_turn_changed(true);
        } else {
            tb_turn_checked = !mv_gs.cancel_end_turn(mv_human);
            std::printf("game_endturn=cancel player=%d checked=%d\n",
                        static_cast<int>(mv_human), tb_turn_checked ? 1 : 0);
            std::fflush(stdout);
        }
    }

    // TBNextButtonClick (AoW.exe 0x004573e8): the shared UI click, then
    // TAoWHSMap.SelectNextArmy (0x55775fa0).
    void game_next_button_click() {
        play_game_sfx("Sfx\\Int\\Btn1.wav");
        game_select_army(+1);
        std::printf("game_next=%d,%d index=%d\n", mv_sel_x, mv_sel_y,
                    tb_next_index);
        std::fflush(stdout);
    }

    // The turn loop, driven once per game frame. Every seat that the port does
    // not simulate - the independents (0) and the CPU wizards - plays its
    // (empty) turn and ends it through the same `GameState::end_turn` the
    // human's button uses. The end is paced by the TurnButton's own turn-change
    // animation, so each seat's colour is seen before the next one begins: the
    // AI phase as the original plays it (and no seat is ever skipped before it
    // ended its turn).
    void game_turn_tick() {
        if (!game_turns_started || mv_human == 0xff) return;
        if (mv_gs.turn_order.empty()) return;
        const std::uint8_t cur = mv_gs.current();
        if (cur == 0xff || cur == mv_human) return;
        if (tb_turn_animating) return;   // the colour change is still playing
        if (mv_gs.end_turn(cur)) game_turn_changed(true);
    }



    // ------------------------------------------------------- TIWindow helpers
    // The Structure floater is the game's `TIWindow` (form `IWindow`,
    // TIWINDOW.dfm): a 200x216 `InfoWin` anchored to the right edge at TopOffset
    // 216, exactly like the Party floater. Every box below is the DFM's own
    // WinLeft/WinTop/WinWidth/WinHeight + Alignment block evaluated by
    // AoWComp.TAoWComponent.ReAlignComp.
    static bool is_structure_hs(const aow::MapHs& hs,
                                const std::vector<aow::MapResource>& res) {
        if (hs.class_id == aow::kCityClassId) return true;
        const std::uint32_t c = hs.class_id;
        if (c == 0x2034a || c == 0x2034c || c == 0x2034e || c == 0x2036e ||
            c == 0x2037a || c == 0x2037b || c == 0x2043e || c == 0x204be ||
            c == 0x2092e || c == 0x203ba || c == 0x2019a || c == 0x2063e ||
            c == 0x203f1 || c == 0x2089e || c == 0x20822 || c == 0x204ae ||
            c == 0x204b3 || c == 0x204ce || c == 0x203de || c == 0x20371 ||
            c == 0x2094e) {
            return true;
        }
        if (const aow::MapResource* r = aow::find_map_resource(res, hs.resource)) {
            if (r->class_id == 0x2034b || r->class_id == 0x2034d ||
                r->class_id == 0x2037b || r->class_id == 0x2089f) {
                return true;
            }
            std::string lib = r->ilb;
            for (char& ch : lib) ch = static_cast<char>(std::toupper(
                static_cast<unsigned char>(ch)));
            if (lib.find("STR_") != std::string::npos ||
                lib.find("PRO_") != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    // Topmost selectable structure on a hex (the engine walks the field's
    // hexagon sprites from the last placed back to the first).
    const aow::MapHs* structure_on_hex(int x, int y) const {
        if (game_level < 0 ||
            static_cast<std::size_t>(game_level) >= game_state_map.levels.size()) {
            return nullptr;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        if (x < 0 || y < 0 || static_cast<std::uint32_t>(x) >= lv.width ||
            static_cast<std::uint32_t>(y) >= lv.height) {
            return nullptr;
        }
        const aow::MapTile& t = lv.tiles[static_cast<std::size_t>(y) * lv.width +
                                         static_cast<std::size_t>(x)];
        for (auto it = t.objects.rbegin(); it != t.objects.rend(); ++it) {
            if (is_structure_hs(*it, game_res)) return &*it;
        }
        return nullptr;
    }

    static const char* race_display_name(int r) {
        static const char* kNames[12] = {
            "Human", "Elf", "Dwarf", "Halfling", "Orc", "Goblin",
            "High Man", "Dark Elf", "Undead", "Lizardman", "Azrac", "Frostling"};
        if (r < 0 || r >= 12) return "Human";
        return kNames[r];
    }

    static const char* structure_display_name(const aow::MapHs& hs,
                                              const std::vector<aow::MapResource>& res) {
        if (hs.class_id == aow::kCityClassId) return "City";
        const aow::MapResource* r = aow::find_map_resource(res, hs.resource);
        const std::uint32_t c = r ? r->class_id : hs.class_id;
        switch (c) {
        case 0x2037b: return "Mine";
        case 0x2043f: return "Tower";
        case 0x2089f: return "Wizard's Tower";
        case 0x2036f: return "Farm";
        case 0x204bf: return "Shipyard";
        case 0x2092f: return "Builders Guild";
        case 0x2019b: return "Power Node";
        case 0x2063f: return "Reflecting Pool";
        case 0x203f2: return "Altar";
        default: break;
        }
        switch (hs.class_id) {
        case 0x20108: return "City";
        case 0x2037a: return "Mine";
        case 0x2043e: return "Tower";
        case 0x2089e: return "Wizard's Tower";
        case 0x2036e: return "Farm";
        case 0x204be: return "Shipyard";
        case 0x2092e: return "Builders Guild";
        case 0x203ba: return "Production Place";
        case 0x2019a: return "Power Node";
        case 0x2063e: return "Reflecting Pool";
        case 0x203f1: return "Altar";
        default: return "Structure";
        }
    }

    // The resolved boxes of TIWINDOW.dfm at its design size (200x216).
    struct StructureLayout {
        AowBox win, title, title_icon, help, center, min, close;
        AowBox name, name_line_a, name_line_b, city, race, status, prod;
        AowBox gold_icon, gold_val, mana_icon, mana_val, portrait;
        AowBox city_border, enter, raze, raze_btn;
    };

    StructureLayout structure_layout() const {
        StructureLayout S;
        const int W = 200, H = 216;
        const int x0 = disp_w - W;
        const int y0 = (disp_h >= 432) ? kPartyTop : std::max(0, disp_h - H);
        S.win = AowBox{x0, y0, W, H};
        S.title = AowBox{x0 + 25, y0 + 4, 165, 16};
        S.title_icon = AowBox{x0 + 2, y0 + 1, 19, 16};
        S.help = AowBox{x0 + 138, y0 + 1, 15, 16};
        S.center = AowBox{x0 + 153, y0 + 1, 15, 16};
        S.min = AowBox{x0 + 167, y0 + 1, 15, 16};
        S.close = AowBox{x0 + 181, y0 + 1, 15, 16};
        S.name = AowBox{x0 + 4, y0 + 17, 136, 27};
        S.name_line_a = AowBox{x0 + 4, y0 + 17, 136, 5};
        S.name_line_b = AowBox{x0 + 4, y0 + 38, 136, 6};
        S.city = AowBox{x0 + 4, y0 + 45, 114, 138};
        S.race = AowBox{S.city.x, S.city.y + 69, S.city.w, 12};
        S.status = AowBox{S.city.x, S.city.y + 120, S.city.w, 12};
        S.prod = AowBox{S.city.x + 14, S.city.y + 25, S.city.w - 24, 12};
        S.gold_icon = AowBox{S.city.x + 24, S.city.y + 53, 16, 13};
        S.gold_val = AowBox{S.city.x + 44, S.city.y + 54, 30, 12};
        S.mana_icon = AowBox{S.city.x + 72, S.city.y + 51, 16, 14};
        S.mana_val = AowBox{S.city.x + 92, S.city.y + 54, 22, 12};
        S.portrait = AowBox{S.city.x + 31, S.city.y + 79, 52, 52};
        S.city_border = AowBox{x0 + 120, y0 + 18, 80, 194};
        S.enter = AowBox{x0 + 10, y0 + 184, 108, 27};
        S.raze = AowBox{x0 + 128, y0 + 174, 56, 35};
        S.raze_btn = AowBox{x0 + 136, y0 + 187, 43, 20};
        return S;
    }

    // The floater's hit test, in TIWindow's control Priority order. The 1000+
    // ids keep them clear of the bar / scanner hits.
    int structure_hit_test(int x, int y) const {
        if (!game_show_structure) return -1;
        const StructureLayout S = structure_layout();
        auto in = [](const AowBox& b, int px, int py) {
            return b.x <= px && px < b.x + b.w && b.y <= py && py < b.y + b.h;
        };
        if (in(S.close, x, y)) return 1000;
        if (in(S.min, x, y)) return 1001;
        if (in(S.center, x, y)) return 1002;
        if (in(S.help, x, y)) return 1003;
        if (in(S.enter, x, y)) return 1004;
        if (game_struct_city && in(S.raze_btn, x, y)) return 1005;
        if (in(S.win, x, y)) return 1006;
        return -1;
    }

    bool load_structure_art(int race_index) {
        std::string lib = "RACES\\RHUMAN.ILB";
        const std::vector<std::string> libs = aow::race_image_libraries(game_res);
        if (race_index >= 0 &&
            static_cast<std::size_t>(race_index) < libs.size()) {
            lib = libs[static_cast<std::size_t>(race_index)];
        }
        if (lib == game_struct_art_lib && game_struct_art.ok) return true;
        game_struct_art = Sprite();
        game_struct_art_lib.clear();
        std::string rel = lib;
        for (char& ch : rel) {
            if (ch == '\\' || ch == '/') ch = '/';
        }
        const fs::path root(root_dir);
        const fs::path p(rel);
        const fs::path cands[3] = {root / "Images" / p, root / p,
                                   root / "Images" / p.filename()};
        std::error_code ec;
        for (const fs::path& c : cands) {
            if (!fs::exists(c, ec)) continue;
            std::string e2;
            if (!iwin_race_il.load(c.string(), e2)) continue;
            game_struct_art_lib = lib;
            int pos = -1;
            for (std::size_t k = 0; k < iwin_race_il.entries().size(); ++k) {
                if (iwin_race_il.entries()[k].index == 42) {
                    pos = static_cast<int>(k);
                    break;
                }
            }
            if (pos < 0) return false;
            std::uint32_t aw = 0, ah = 0;
            Sprite& art = game_struct_art;
            if (!iwin_race_il.decode_rgb(static_cast<std::size_t>(pos),
                                         art.rgba, aw, ah, e2)) {
                game_struct_art = Sprite();
                return false;
            }
            art.w = aw; art.h = ah; art.ok = true;
            const aow::IlbEntry& ie =
                iwin_race_il.entries()[static_cast<std::size_t>(pos)];
            art.base_x = ie.base_x;
            art.base_y = ie.base_y;
            art.advance = ie.width;
            return true;
        }
        return false;
    }

    bool open_structure(int hx, int hy) {
        const aow::MapHs* hs = structure_on_hex(hx, hy);
        if (hs == nullptr) return false;
        game_struct_hx = hx;
        game_struct_hy = hy;
        game_struct_city = hs->class_id == aow::kCityClassId;
        game_struct_owner = hs->structure_owner;
        game_struct_name = structure_display_name(*hs, game_res);
        if (game_struct_city) {
            // TCity.GetName copies +0x48, the rw tag 0xc AnsiString parsed by
            // the map loader. A map that stores no name falls back to "City";
            // `--structure-name` overrides either for captures.
            game_struct_name = hs->city_name.empty() ? "City" : hs->city_name;
        }
        game_struct_race_index = hs->race;
        if (game_struct_race_index < 0) {
            for (const aow::Player& p : mv_gs.players) {
                if (p.index == hs->structure_owner) {
                    game_struct_race_index = p.race;
                    break;
                }
            }
        }
        if (game_struct_race_index < 0) game_struct_race_index = 0;
        game_struct_race = race_display_name(game_struct_race_index);
        game_struct_status = "Cheerful";
        game_struct_production = game_struct_city ? "Producing Merchandise"
                                                  : "Idle";
        game_struct_gold = game_struct_city ? (10 + game_struct_race_index) : 0;
        game_struct_mana = game_struct_city ? (2 + game_struct_race_index / 4) : 0;
        // The right-hand view: `TRaceResource` BCS_<race>.BMP, rw id 42 (BCU is
        // 45). Loaded lazily because a scenario uses only a few race libraries.
        std::string lib = "RACES\\RHUMAN.ILB";
        const std::vector<std::string> libs = aow::race_image_libraries(game_res);
        if (game_struct_race_index >= 0 &&
            static_cast<std::size_t>(game_struct_race_index) < libs.size()) {
            lib = libs[static_cast<std::size_t>(game_struct_race_index)];
        }
        if (lib != game_struct_art_lib) {
            game_struct_art = Sprite();
            std::string rel = lib;
            for (char& ch : rel) {
                if (ch == '\\' || ch == '/') ch = '/';
            }
            const fs::path root(root_dir);
            const fs::path p(rel);
            const fs::path cands[3] = {root / "Images" / p, root / p,
                                       root / "Images" / p.filename()};
            std::error_code ec;
            for (const fs::path& c : cands) {
                if (!fs::exists(c, ec)) continue;
                std::string e2;
                if (!iwin_race_il.load(c.string(), e2)) continue;
                game_struct_art_lib = lib;
                int pos = -1;
                for (std::size_t k = 0; k < iwin_race_il.entries().size(); ++k) {
                    if (iwin_race_il.entries()[k].index == 42) {
                        pos = static_cast<int>(k);
                        break;
                    }
                }
                if (pos < 0) {
                    game_struct_art = Sprite();
                } else {
                    std::uint32_t aw = 0, ah = 0;
                    Sprite& art = game_struct_art;
                    if (iwin_race_il.decode_rgb(static_cast<std::size_t>(pos),
                                                art.rgba, aw, ah, e2)) {
                        art.w = aw; art.h = ah; art.ok = true;
                        const aow::IlbEntry& ie =
                            iwin_race_il.entries()[static_cast<std::size_t>(pos)];
                        art.base_x = ie.base_x;
                        art.base_y = ie.base_y;
                        art.advance = ie.width;
                    } else {
                        game_struct_art = Sprite();
                    }
                }
                break;
            }
        }
        game_show_structure = true;
        game_struct_hover = game_struct_pressed = -1;
        play_game_sfx("Sfx\\Int\\Btn1.wav");
        return true;
    }

    void close_structure() {
        game_show_structure = false;
        game_struct_hx = game_struct_hy = -1;
        game_struct_hover = game_struct_pressed = -1;
        game_struct_art = Sprite();
        game_struct_art_lib.clear();
    }

    int game_hit(int x, int y) const {
        if (game_show_structure) {
            const int sh = structure_hit_test(x, y);
            if (sh >= 0) return sh;
        }
        const GameLayout L = game_layout();
        auto in = [](const AowBox& b, int px, int py) {
            return b.x <= px && px < b.x + b.w && b.y <= py && py < b.y + b.h;
        };
        // The party floater is on top of the bar and the other floaters, so it
        // is hit-tested first (the engine's Priority ordering).
        if (game_show_party) {
            const int ph = party_hit_test(x, y);
            if (ph >= 0) return 100 + ph;
        }
        for (int i = 0; i < kTbTabCount; ++i) {
            if (in(L.tab[i], x, y)) return i;
        }
        for (int i = 0; i < 2; ++i) {
            if (in(L.scan_tab[i], x, y)) return 5 + i;
        }
        if (in(L.next, x, y)) return kGameHitNext;
        if (in(L.turn, x, y)) return kGameHitTurn;
        if (in(L.scan_min, x, y)) return kGameHitScanMin;
        return -1;
    }

    // One frame of the in-game screen: the rendered map with the TWorldMap
    // composition drawn over it - the bottom bar (TTBWindow), the World Map
    // floater (TSWindow) and the Events floater (TPInfoW). Every element is
    // drawn at its own pixel size into the box ReAlignComp resolves for the
    // current window, so growing the window *reflows* the interface - the tabs
    // widen and spread, the floaters ride the right edge, the bar stays 48 px -
    // instead of magnifying the whole composition.
    // The selected building's pulsing marker (TSelectedStructure / the hexagon
    // selection the engine keeps around the selected object). Drawn in the map
    // viewport, under the floaters, and advanced by AoWHSMap+0x40.
    void render_structure_marker(Canvas& cv) const {
        if (!game_show_structure || game_struct_hx < 0) return;
        const AowBox m = game_layout().map_area;
        int hx = 0, hy = 0;
        aow::hex_origin(game_struct_hx, game_struct_hy, hx, hy);
        const int pulse = 70 + ((game_clock >> 2) & 0x7f);  // 70..197
        auto put = [&](const Sprite& s) {
            if (!s.ok) return;
            const int dx = m.x - cam_x + hx + s.anchor_dx - s.base_x;
            const int dy = m.y - cam_y + hy + s.anchor_dy - s.base_y;
            cv.blit(s, dx, dy, pulse, m.x, m.y, m.x + m.w, m.y + m.h);
        };
        put(hex_sel_back);
        put(hex_sel_front);
    }

    // TIWindow's own paint pass. Draw order follows the DFM: the stone base,
    // the paper body, the 9-slice floater frame, the title bar controls, the
    // city/production panel, the city-view arch, and finally the foot buttons.
    void render_structure_panel(Canvas& cv) const {
        if (!game_show_structure) return;
        const StructureLayout S = structure_layout();
        const int x0 = S.win.x, y0 = S.win.y;
        if (iwin_bg.ok) {
            cv.blit_clip(iwin_bg, x0, y0, x0, y0, 200, 216);
        }
        if (iwin_body.ok) {
            cv.blit_clip(iwin_body, x0 + 9, y0 + 22, x0 + 9, y0 + 22, 186, 189);
        }
        if (iwin_gold_frame[1].ok) {
            blit9_impl(cv, iwin_gold_frame, S.name.x, S.name.y, S.name.w,
                       S.name.h, 3, 3);
        }
        blit9_impl(cv, iwin_frame, S.win.x, S.win.y, S.win.w, S.win.h, 6, 6);
        // CeltFrame (TIWINDOW.dfm: ILFrame = CeltDeco, frHollowBox, index 22 ->
        // the FrameMain2 9-slice at entries 30..37), inset 9 px from the window.
        if (iwin_celt_main[1].ok) {
            blit9_impl(cv, iwin_celt_main, x0 + 9, y0 + 21, 188, 190, 6, 6);
        }
        if (iwin_celt_knot.ok) {
            cv.blit_clip(iwin_celt_knot, x0, y0, x0, y0, 20, 20);
        }
        if (iwin_title_icon.ok) {
            cv.blit_clip(iwin_title_icon, x0 + 7, y0 + 4, x0, y0, 200, 20);
        }
        text_at(cv, glyphs10b, x0 + 32, y0 + 7, "Structure", 0, 0, 0, false,
                false);
        const int hs = game_struct_pressed == 1003 ? 2
                       : game_struct_hover == 1003 ? 1 : 0;
        const int cs = game_struct_pressed == 1002 ? 2
                       : game_struct_hover == 1002 ? 1 : 0;
        const int ms = game_struct_pressed == 1001 ? 2
                       : game_struct_hover == 1001 ? 1 : 0;
        const int xs = game_struct_pressed == 1000 ? 2
                       : game_struct_hover == 1000 ? 1 : 0;
        auto title_button = [&](const Sprite* art, const AowBox& b, int st) {
            if (art[st].ok) cv.blit_stretch(art[st], b.x, b.y, b.w, b.h);
        };
        title_button(iwin_help, S.help, hs);
        title_button(iwin_center, S.center, cs);
        title_button(iwin_min, S.min, ms);
        title_button(iwin_close, S.close, xs);
        if (game_struct_city) {
            if (iwin_gold_icon.ok) {
                cv.blit_stretch(iwin_gold_icon, S.gold_icon.x, S.gold_icon.y,
                                S.gold_icon.w, S.gold_icon.h);
            }
            if (iwin_mana_icon.ok) {
                cv.blit_stretch(iwin_mana_icon, S.mana_icon.x, S.mana_icon.y,
                                S.mana_icon.w, S.mana_icon.h);
            }
            text_at(cv, glyphs8b, S.gold_val.x, S.gold_val.y,
                    std::to_string(game_struct_gold), 0, 0, 0, false, false);
            text_at(cv, glyphs8b, S.mana_val.x, S.mana_val.y,
                    std::to_string(game_struct_mana), 0, 0, 0, false, false);
            text_at(cv, glyphs8b, S.race.x + S.race.w / 2, S.race.y,
                    "Race: " + game_struct_race, 0, 0, 0, true, false);
            const int cx = S.portrait.x + S.portrait.w / 2;
            const int cy = S.portrait.y + S.portrait.h / 2;
            const int rr = S.portrait.w / 2;
            for (int yy = -rr; yy <= rr; ++yy) {
                const int span = static_cast<int>(
                    std::sqrt(static_cast<double>(rr * rr - yy * yy)));
                cv.fill(cx - span, cy + yy, span * 2, 1, 22, 45, 92, 255);
            }
            if (game_face.ok) {
                const int fr = rr - 7;
                for (int dy = -fr; dy <= fr; ++dy) {
                    for (int dx = -fr; dx <= fr; ++dx) {
                        if (dx * dx + dy * dy > fr * fr) continue;
                        const std::uint32_t sx = static_cast<std::uint32_t>(
                            (dx + fr) * static_cast<int>(game_face.w) / (2 * fr));
                        const std::uint32_t sy = static_cast<std::uint32_t>(
                            (dy + fr) * static_cast<int>(game_face.h) / (2 * fr));
                        const std::uint8_t* p =
                            &game_face.rgba[(static_cast<std::size_t>(sy) *
                                                 game_face.w + sx) * 4];
                        if (p[3] == 0) continue;
                        // The city panel's face sits in a blue wash (the
                        // reference capture's tint), not the raw portrait.
                        const int rr8 = p[0] * 2 / 3;
                        const int gg8 = p[1] * 3 / 4;
                        const int bb8 = std::min(255, p[2] * 4 / 5 + 55);
                        cv.fill(cx + dx, cy + dy, 1, 1,
                                static_cast<std::uint8_t>(rr8),
                                static_cast<std::uint8_t>(gg8),
                                static_cast<std::uint8_t>(bb8), 255);
                    }
                }
            }
            for (int a = 0; a < 360; a += 2) {
                const double rad = a * 3.14159265358979 / 180.0;
                const int px = cx + static_cast<int>(std::cos(rad) * (rr - 1));
                const int py = cy + static_cast<int>(std::sin(rad) * (rr - 1));
                cv.fill(px, py, 2, 2, 196, 166, 67, 255);
            }
            text_at(cv, glyphs8b, S.status.x + S.status.w / 2, S.status.y,
                    "Status: " + game_struct_status, 0, 0, 0, true, false);
        }
        // StructNameLbl (Age10s, white, centred on NamePnl), with its shadow.
        text_at(cv, glyphs10b, S.name.x + S.name.w / 2 + 1, S.name.y + 13,
                game_struct_name, 0, 0, 0, true, false);
        text_at(cv, glyphs10b, S.name.x + S.name.w / 2, S.name.y + 12,
                game_struct_name, 255, 255, 255, true, false);
        // CityBorderPnl: the race's BCS city view clipped to its 80x194 box,
        // then CeltDeco's arch (id 8) and border tile (id 9).
        if (game_struct_city) {
            if (game_struct_art.ok) {
                cv.blit_clip(game_struct_art, x0 + 134, y0 - 91,
                             S.city_border.x, S.city_border.y, 80, 169);
            }
            if (iwin_celt_arch.ok) {
                cv.blit_clip(iwin_celt_arch, x0 + 122, y0 + 21,
                             S.city_border.x, S.city_border.y,
                             S.city_border.w, S.city_border.h);
            }
            // The arch's left pillar and thin right border repeat every 58 px
            // below the top arch (CeltDeco 8's left 20 px + the id-9 border
            // tile).
            for (int yy = 79; yy < 188; yy += 58) {
                if (iwin_celt_arch.ok) {
                    cv.blit_clip(iwin_celt_arch, x0 + 122, y0 + yy,
                                 x0 + 122, y0 + yy, 20, 58);
                }
                if (iwin_celt_side.ok) {
                    cv.blit_clip(iwin_celt_side, x0 + 122, y0 + yy,
                                 x0 + 122, y0 + yy, 76, 58);
                }
            }
        }
        // The CityPanel's active-action text is drawn last in the engine (it is
        // the panel's own Canvas.TextOut pass, over every child/window art).
        text_at(cv, glyphs8b, S.prod.x, S.prod.y, game_struct_production, 0, 0,
                0, false, false, 0);
        {
            const int st = game_struct_pressed == 1004 ? 2
                           : game_struct_hover == 1004 ? 1 : 0;
            blit3_box(cv, iwin_enter[st], S.enter);
            text_at(cv, glyphs10b, S.enter.x + S.enter.w / 2, S.enter.y + 7,
                    "Enter", 0, 0, 0, true, false);
        }
        if (game_struct_city) {
            if (iwin_raze_bg.ok) {
                cv.blit_stretch(iwin_raze_bg, S.raze.x, S.raze.y, S.raze.w,
                                S.raze.h);
            }
            const int st = game_struct_pressed == 1005 ? 2
                           : game_struct_hover == 1005 ? 1 : 0;
            if (iwin_raze_btn[st].ok) {
                cv.blit_stretch(iwin_raze_btn[st], S.raze_btn.x, S.raze_btn.y,
                                S.raze_btn.w, S.raze_btn.h);
            }
        }
    }

    Canvas render_game() const {
        Canvas cv(disp_w, disp_h);
        cv.clear(0, 0, 0);
        const GameLayout L = game_layout();

        // 1. The map viewport: the camera's rectangle over the level, drawn 1:1
        //    (the engine's own scale - the world map has no zoom) and clipped to
        //    the map window's client box. The level render's origin IS map pixel
        //    (0,0) (mapviz's hex origin is HXtoHP, `x*32+8 / y*32+(x&1)*16`), so
        //    blitting it at `client - Scene` shows exactly the pixels
        //    THSMapViewer.ShowScene would. The MapWindow.BMP frame is then laid
        //    over the whole window box, its 12 px ring covering the strip the
        //    scene is inset by - the intricate corners CornersRef.png shows.
        {
            const AowBox& m = L.map_area;
            // The main view shows `game_draw` - the level render plus the hover
            // path overlay (the scanner below keeps the pristine `game_px`, so
            // the trail is not drawn over the minimap).
            const std::vector<std::uint8_t>& px =
                (game_draw.size() == game_px.size() && !game_draw.empty())
                    ? game_draw
                    : game_px;
            cv.blit_rgba_rect(px, game_pw, game_ph, m.x - cam_x, m.y - cam_y,
                              m.x, m.y, m.w, m.h);
            blit_frame_box(cv, map_frame, L.map_window);
        }
        // The selected structure's pulsing marker is part of the map scene.
        render_structure_marker(cv);

        // 2. The bottom bar: the BarSet 3-piece body, the start button, the five
        //    task tabs, the next-turn / turn buttons and the gold & mana panels.
        blit3_box(cv, tb_bar, L.bar);
        blit3_box(cv, tb_start[0], L.start);
        for (int i = 0; i < kTbTabCount; ++i) {
            int st = 0;
            if (i == 0 && game_show_scan) st = 1;
            if (i == 4 && game_show_events) st = 1;
            if (i == game_hover) st = 1;
            blit3_box(cv, tb_tab[st], L.tab[i]);
            // FontModule.Age10, TextColor clBlack, centred on the tab.
            text_at(cv, glyphs10b, L.tab[i].x + L.tab[i].w / 2, L.tab[i].y + 3,
                    kTbTabText[i], 24, 20, 16, true, false);
        }
        // The player-indicator strip (a row of right-aligned PlyrIndL pills)
        // and the turn timer (clock body + hourglass + read-out).
        if (tb_pind[1].ok) {
            const int lw = static_cast<int>(tb_pind[0].w);
            const int rw = static_cast<int>(tb_pind[2].w);
            const int midw = kTbPindPillW - lw - rw;
            int px = L.pind.x + L.pind.w - kTbPindPills * kTbPindPillW;
            for (int i = 0; i < kTbPindPills; ++i, px += kTbPindPillW) {
                cv.blit_stretch(tb_pind[0], px, L.pind.y, lw, L.pind.h);
                cv.blit_stretch(tb_pind[1], px + lw, L.pind.y, midw, L.pind.h);
                cv.blit_stretch(tb_pind[2], px + kTbPindPillW - rw, L.pind.y, rw,
                                L.pind.h);
            }
        }
        cv.blit_stretch(tb_time_body, L.timer.x, L.timer.y, L.timer.w, L.timer.h);
        cv.blit_stretch(tb_time_icon, L.timeicon.x, L.timeicon.y, L.timeicon.w,
                        L.timeicon.h);
        // TCTimeLbl: FontModule.TBarNumber, clBlack, right-aligned to rox 225.
        {
            const int re = L.bar.x + L.bar.w - kTbTimeLblRox;
            text_at(cv, glyphsTb, re - text_w(glyphsTb, kTbTimeText),
                    L.bar.y + kTbTimeLblTop, kTbTimeText, 0, 0, 0, false,
                    false);
        }
        // TBNextButton: one 26x38 picture - id 3 at rest, id 4 while the button
        // is held down (TAOWButton.Draw's +0x110 branch; CheckMouseUp clears it).
        {
            const Sprite& b = tb_next_down ? tb_next_pressed : tb_next_idle;
            if (b.ok) cv.blit_stretch(b, L.next.x, L.next.y, L.next.w, L.next.h);
        }
        // TurnButton (TAOWTurnButton.Draw 0x59704e44): the current frame of the
        // 102-frame turn animation - the seat's race block at rest, the block's
        // transition frame when the turn has been ended, and the swirl while the
        // change animation plays.
        {
            const int f = tb_turn_draw_frame();
            if (f >= 0 && static_cast<std::size_t>(f) < tb_turn_frames.size() &&
                tb_turn_frames[static_cast<std::size_t>(f)].ok) {
                cv.blit_stretch(tb_turn_frames[static_cast<std::size_t>(f)],
                                L.turn.x, L.turn.y, L.turn.w, L.turn.h);
            }
        }
        cv.blit_stretch(tb_gold, L.gold.x, L.gold.y, L.gold.w, L.gold.h);
        cv.blit_stretch(tb_mana, L.mana.x, L.mana.y, L.mana.w, L.mana.h);
        // The totals (FontModule.TBarNumber, clBlack) right-aligned rox 23, the
        // "+" sign (Age8) at rox 16 and the per-turn income (Age8) centred in
        // the 16 px box inset rox 1 - the sign and income are green 0x77FF77.
        char gold[32], ginc[32], mana[32], minc[32];
        std::snprintf(gold, sizeof(gold), "%d", game_gold);
        std::snprintf(ginc, sizeof(ginc), "%d", game_gold_income);
        std::snprintf(mana, sizeof(mana), "%d", game_mana);
        std::snprintf(minc, sizeof(minc), "%d", game_mana_income);
        draw_readout(cv, L.gold, gold, ginc);
        draw_readout(cv, L.mana, mana, minc);

        if (game_show_scan) draw_game_scan(cv, L);
        if (game_show_events) draw_game_events(cv, L);
        // The Party floater (TControlWin) rides the right edge over everything
        // but the pointer, exactly as the engine's Priority-10 window does.
        if (game_show_party) draw_game_party(cv, party_layout());

        // 6. The pointer (THSMapMouse+0x30). The plain arrow, the hover sparkle
        //    while the pointer rests on something selectable, the walk / sail /
        //    fly icon (or its blocked twin) while a move selection is being
        //    aimed, and the crossed swords / twin banners over another army -
        //    all resolved by TAoWE.TAoWHSMap.UpdateCursor. A Cursors.ilb pointer
        //    is painted with its slot centred on the mouse, which is what the
        //    sprite's own anchor_dx/dy carries.
        if (cursor_on) {
            const Sprite& pointer = game_pointer();
            if (pointer.ok) {
                cv.blit_keyed(pointer, mouse_x + pointer.anchor_dx,
                              mouse_y + pointer.anchor_dy);
            }
        }
        return cv;
    }

    // The World Map floater (TSWindow.ScannerWindow): the MainFloaterSetR frame
    // over the MainFloaterSet body, the ScanWin scanner frame holding the
    // minimap, the three view toggles and the Surface / Caverns tab strip. All
    // at their design pixel sizes - only the position follows the window.
    void draw_game_scan(Canvas& cv, const GameLayout& L) const {
        if (float_body.ok) {
            cv.blit_stretch(float_body, L.scan.x, L.scan.y, L.scan.w, L.scan.h);
        }
        blit9_box(cv, float_frame, L.scan);
        cv.blit_stretch(scan_title_icon, L.scan_icon.x, L.scan_icon.y,
                        L.scan_icon.w, L.scan_icon.h);
        text_at(cv, glyphs10b, L.scan_title.x, L.scan_title.y, "World Map", 24,
                20, 16, false, false);
        cv.blit_stretch(scan_help[0], L.scan_help.x, L.scan_help.y,
                        L.scan_help.w, L.scan_help.h);
        cv.blit_stretch(scan_min[0], L.scan_min.x, L.scan_min.y, L.scan_min.w,
                        L.scan_min.h);
        // The scanner frame (ScanWin overview2 9-slice) and the minimap.
        blit9_box(cv, scan_frame, L.scan_frame);
        if (game_pw && game_ph) {
            cv.blit_rgba_scaled(game_px, game_pw, game_ph, L.scan_map.x,
                                L.scan_map.y, L.scan_map.w, L.scan_map.h);
        }
        // BuildingCheck / ArmyCheck / NaturalCheck (ScanIcon 15x15).
        const bool chk[3] = {game_buildings, game_armies, game_natural};
        for (int i = 0; i < 3; ++i) {
            cv.blit_stretch(scan_chk[chk[i] ? 1 : 0], L.scan_chk[i].x,
                            L.scan_chk[i].y, L.scan_chk[i].w, L.scan_chk[i].h);
        }
        // ScannerTab: two ParchClm 3-piece tabs (Surface / Caverns).
        for (int i = 0; i < 2; ++i) {
            const Sprite* b = scan_tab[game_level == i ? 1 : 0];
            blit3_box(cv, b, L.scan_tab[i]);
            text_at(cv, glyphs8b, L.scan_tab[i].x + L.scan_tab[i].w / 2,
                    L.scan_tab[i].y + 2, kScanTabText[i], 20, 20, 20, true,
                    false);
        }
        draw_game_view_rect(cv, L);
    }

    // The scanner's view rectangle - the camera, in the minimap. AoW.exe's
    // MapViewerSceneChanged (0x00450f8c) pushes it into TScanner as
    // TLeft/TTop/TWidth/THeight in *hex* units (see cam_scanner_rect); the
    // engine paints it with the scanner's own art, which is not recovered, so
    // the replica outlines it over the scaled minimap instead.
    void draw_game_view_rect(Canvas& cv, const GameLayout& L) const {
        if (map_w == 0 || map_h == 0) return;
        const AowBox& s = L.scan_map;
        if (s.w <= 0 || s.h <= 0) return;
        int tl, tt, tw, th;
        cam_scanner_rect(tl, tt, tw, th);
        // The scanner's -6 map-origin offset cancels against its own drawing, so
        // the rectangle's drawn corner is (SceneX >> 5, SceneY >> 5) hexes.
        (void)tl;
        (void)tt;
        int x = s.x + (cam_x >> 5) * s.w / static_cast<int>(map_w);
        int y = s.y + (cam_y >> 5) * s.h / static_cast<int>(map_h);
        int w = tw * s.w / static_cast<int>(map_w);
        int h = th * s.h / static_cast<int>(map_h);
        if (w < 1) w = 1;
        if (h < 1) h = 1;
        if (x < s.x) { w -= s.x - x; x = s.x; }
        if (y < s.y) { h -= s.y - y; y = s.y; }
        if (x + w > s.x + s.w) w = s.x + s.w - x;
        if (y + h > s.y + s.h) h = s.y + s.h - y;
        if (w <= 0 || h <= 0) return;
        cv.fill(x, y, w, 1, 255, 255, 255, 255);
        cv.fill(x, y + h - 1, w, 1, 255, 255, 255, 255);
        cv.fill(x, y, 1, h, 255, 255, 255, 255);
        cv.fill(x + w - 1, y, 1, h, 255, 255, 255, 255);
    }

    // The Events floater (TPInfoW.PInfoWin): the leader banner (portrait cell,
    // name / "of the" / race, the Day cell, the race crest) and the Event
    // History list - all at their design pixel sizes.
    void draw_game_events(Canvas& cv, const GameLayout& L) const {
        if (float_body.ok) {
            cv.blit_stretch(float_body, L.events.x, L.events.y, L.events.w,
                            L.events.h);
        }
        blit9_box(cv, float_frame, L.events);
        cv.blit_stretch(event_title_icon, L.event_icon.x, L.event_icon.y,
                        L.event_icon.w, L.event_icon.h);
        text_at(cv, glyphs10b, L.event_title.x, L.event_title.y, "Events", 24,
                20, 16, false, false);
        cv.blit_stretch(scan_help[0], L.event_help.x, L.event_help.y,
                        L.event_help.w, L.event_help.h);
        cv.blit_stretch(scan_min[0], L.event_min.x, L.event_min.y, L.event_min.w,
                        L.event_min.h);
        // PInfoPnl: the FrGoldD frame over the banner body.
        if (event_body_tex.ok) {
            cv.blit_stretch(event_body_tex, L.event_pnl.x, L.event_pnl.y,
                            L.event_pnl.w, L.event_pnl.h);
        }
        blit9_box(cv, event_pnl_frame, L.event_pnl);
        // FaceFr (leader portrait cell) and ShieldFr (race crest cell).
        if (event_body_tex.ok) {
            cv.blit_stretch(event_body_tex, L.face.x, L.face.y, L.face.w,
                            L.face.h);
        }
        blit9_box(cv, event_cell_frame, L.face);
        // The leader's portrait: `TPlayer.GetFace` -> the FACERES.PFS record the
        // map's leader names -> that record's image library at its image index,
        // drawn into FaceFr's box inset by 3 px (FaceFrDraw 0x00423770). The
        // engine's blit is a nearest-neighbour stretch of the 103x128 face into
        // that 46x58 rect - verified pixel-for-pixel against
        // bin/ProfileLook1.png (mean |delta| 0.83/255 per channel).
        if (game_face.ok) {
            cv.blit_stretch(game_face, L.face.x + kEventFaceInset,
                            L.face.y + kEventFaceInset,
                            L.face.w - 2 * kEventFaceInset,
                            L.face.h - 2 * kEventFaceInset);
        }
        if (event_shield_tex.ok) {
            cv.blit_stretch(event_shield_tex, L.shield.x, L.shield.y,
                            L.shield.w, L.shield.h);
        }
        blit9_box(cv, event_cell_frame, L.shield);
        // RShield: ShieldsM's race crest, at `CurrentImage = race + 13`
        // (FUN_00423170), drawn 1:1 at ShieldFr + (6,6) (the DFM's awCenter /
        // ahCenter box) with the sprite's own base + shift, and *blended*: the
        // engine's class-0x12 crest sprite is averaged with whatever it covers,
        // which is why bin/ProfileLook1.png shows the cell's hatch pattern
        // through the crest. Measured against that reference: a 50% blend
        // (alpha100 50 -> a = 127) matches to 5.9/255 per channel where the
        // opaque blit sits at 33.7.
        // `AOW_NO_CREST` renders the cell without any crest so the regression
        // scripts can use the port's own output as the background when fitting
        // the crest art against bin/ProfileLook1.png.
        if (getenv("AOW_NO_CREST") == nullptr) {
            const Sprite& crest =
                event_shield_race[game_shield_index >= 0 &&
                                          game_shield_index < kEventShieldCount
                                      ? game_shield_index
                                      : 0];
            const int sx = L.shield.x + L.shield.w / 2 - 20;
            const int sy = L.shield.y + L.shield.h / 2 - 26;
            if (crest.ok && game_shield_index >= 0) {
                cv.blit(crest, sx, sy, kEventCrestAlpha, 0, 0, cv.width(),
                        cv.height());
            } else {
                // No map/seat yet: the DFM's design-time art (`CurrentImage` 8).
                cv.blit_stretch(event_shield, sx, sy, 40, 52);
            }
        }
        // InfoFr (name/race) and TurnFr (the day) cells.
        if (event_body_tex.ok) {
            cv.blit_stretch(event_body_tex, L.info.x, L.info.y, L.info.w,
                            L.info.h);
        }
        blit9_box(cv, event_cell_frame, L.info);
        if (event_body_tex.ok) {
            cv.blit_stretch(event_body_tex, L.turnfr.x, L.turnfr.y, L.turnfr.w,
                            L.turnfr.h);
        }
        blit9_box(cv, event_cell_frame, L.turnfr);
        // Player / "of the" / Race / Day (Age8, TextColor 9415072 = 160,169,143).
        text_at(cv, glyphs8b, L.player.x, L.player.y, game_leader, 160, 169, 143,
                true, false);
        text_at(cv, glyphs8b, L.leadl.x, L.leadl.y, "of the", 160, 169, 143,
                true, false);
        text_at(cv, glyphs8b, L.race.x, L.race.y, game_race, 160, 169, 143, true,
                false);
        char day[32];
        std::snprintf(day, sizeof(day), "Day %d", game_day);
        text_at(cv, glyphs8b, L.day.x, L.day.y, day, 160, 169, 143, true, false);
        // "Event History" (EventLbl) and the EventFr list.
        text_at(cv, glyphs8b, L.event_lbl.x, L.event_lbl.y, "Event History", 24,
                24, 20, true, false);
        blit9_box(cv, event_pnl_frame, L.list);
        // The list's first row: the EventIcn icon + the logged text.
        cv.blit_stretch(event_icon, L.list.x + 4, L.list.y + 2, 11, 14);
        text_at(cv, glyphs8b, L.list.x + 20, L.list.y + 2, game_event, 160, 169,
                143, false, false);
        // The Structure floater has Priority 10 and is drawn over every other
        // in-game window (TIWindow.InfoWin).
        render_structure_panel(cv);
    }

    // ---- the in-game screen (TWorldMap) drawing helpers -------------------
    // The map window's frame, the way TAOWWindow.Draw lays out its 8-entry
    // ILFrame library (TL, top, TR, left, right, BL, bottom, BR): the edges are
    // TILED 1:1 across the span *between* the corners (never stretched) and the
    // corners are blitted on top. For MapWindow.BMP that reproduces the
    // reference exactly - the 12x408 side edges fill the 408 px span with one
    // copy, and the 419x12 top/bottom edges start at the corner with their 3 px
    // overhang covered by the far corner (AoW.exe 0x00451268 insets the scene to
    // `w-0x18` / `h-0x18`, so the corners are the 12x12 art verbatim).
    void blit_frame_box(Canvas& cv, const Sprite* fr, const AowBox& r) const {
        if (r.w <= 0 || r.h <= 0 || !fr[0].ok) return;
        const int cw = static_cast<int>(fr[0].w);   // corner width
        const int ch = static_cast<int>(fr[0].h);   // corner height
        cv.blit_tile(fr[1], r.x + cw, r.y, r.w - 2 * cw, ch, false);      // top
        cv.blit_tile(fr[6], r.x + cw, r.y + r.h - ch, r.w - 2 * cw, ch,
                     false);                                             // bottom
        cv.blit_tile(fr[3], r.x, r.y + ch, cw, r.h - 2 * ch, true);      // left
        cv.blit_tile(fr[4], r.x + r.w - cw, r.y + ch, cw, r.h - 2 * ch,
                     true);                                              // right
        cv.blit_stretch(fr[0], r.x, r.y, cw, ch);                        // TL
        cv.blit_stretch(fr[2], r.x + r.w - cw, r.y, cw, ch);             // TR
        cv.blit_stretch(fr[5], r.x, r.y + r.h - ch, cw, ch);             // BL
        cv.blit_stretch(fr[7], r.x + r.w - cw, r.y + r.h - ch, cw, ch);  // BR
    }

    // A 9-slice frame blitted into `r`: the corners keep their own pixel size
    // and the edges fill the gap, so the frame art never scales - it just spans
    // the box. Uses the frame's own corner sizes, which is what makes the
    // asymmetric floater frame (6x20 top corners, 6x6 bottom) and the symmetric
    // ScanWin frame both come out right.
    void blit9_box(Canvas& cv, const Sprite* fr, const AowBox& r) const {
        if (r.w <= 0 || r.h <= 0 || !fr[0].ok) return;
        const int cw = static_cast<int>(fr[0].w);
        const int th = static_cast<int>(fr[0].h);
        const int bh = static_cast<int>(fr[5].h);
        cv.blit_stretch(fr[0], r.x, r.y, cw, th);
        cv.blit_stretch(fr[2], r.x + r.w - cw, r.y, cw, th);
        cv.blit_stretch(fr[5], r.x, r.y + r.h - bh, cw, bh);
        cv.blit_stretch(fr[7], r.x + r.w - cw, r.y + r.h - bh, cw, bh);
        cv.blit_stretch(fr[1], r.x + cw, r.y, r.w - 2 * cw, th);
        cv.blit_stretch(fr[6], r.x + cw, r.y + r.h - bh, r.w - 2 * cw, bh);
        cv.blit_stretch(fr[3], r.x, r.y + th, cw, r.h - th - bh);
        cv.blit_stretch(fr[4], r.x + r.w - cw, r.y + th, cw, r.h - th - bh);
    }

    // A 3-piece button (left cap / stretched middle / right cap) in `r`. The
    // caps keep their pixel size, so a wider tab widens only its middle - the
    // exact behaviour the DFM's WidthPercent 14 produces as the bar grows.
    void blit3_box(Canvas& cv, const Sprite* b, const AowBox& r) const {
        if (!b[1].ok) return;
        const int lw = static_cast<int>(b[0].w);
        const int rw = static_cast<int>(b[2].w);
        cv.blit_stretch(b[0], r.x, r.y, lw, r.h);
        if (r.w - lw - rw > 0) {
            cv.blit_stretch(b[1], r.x + lw, r.y, r.w - lw - rw, r.h);
        }
        cv.blit_stretch(b[2], r.x + r.w - rw, r.y, rw, r.h);
    }

    // One frame of AoW.exe 0x00419c84 (the title screen).
    Canvas render(int hover) const {
        if (screen == 1) return render_gtype();
        if (screen == 2) return render_scenario();
        if (screen == 3) return render_game();

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
            const int orb_w = static_cast<int>(orb_slot_w);
            const int cx = button_left(hover) + orb_w / 2;
            draw_text(cv, cx - 6, top + kLabelY, captions[hover]);
        }

        // 5. The pointer. The engine shows the Windows arrow, so the replica
        //    paints the equivalent sprite from Images\Cursors.ilb at the mouse
        //    position - this way the cursor is part of the frame in both the
        //    window and head-less renders.
        if (cursor_on && cursor.ok) {
            cv.blit_keyed(cursor, mouse_x, mouse_y);
        }
        return cv;
    }
};

// ---------------------------------------------------------------- FPS counter
// F1 toggles a frames-per-second read-out in the frame's top-left corner. It
// counts the frames the window actually *presented* - one per WM_PAINT - and
// averages them over a one-second window, so it reports the real rate of the
// frame clock (the 33 ms waitable timer, see start_frame_clock) rather than the
// form's nominal FrameRate = 30: a frame that overruns its budget is not caught
// up (the clock does not accumulate), so it shows up in the read-out.
constexpr int kFpsWindowMs = 1000;   // averaging window
constexpr int kFpsX = 4;             // read-out anchor (top-left of the frame)
constexpr int kFpsY = 4;
constexpr int kFpsPad = 2;           // plate border around the text
constexpr int kFpsPlateAlpha = 170;  // the plate's opacity (0..255)

struct FpsMeter {
    bool on = false;            // F1 switched the read-out on
    double value = 0.0;         // frames per second, last completed window
    double window_start = 0.0;  // ms timestamp of the window's first frame
    int frames = 0;             // frames presented in the current window

    // Restart the averaging window. F1 calls this when it switches the read-out
    // on so the first number comes from fresh frames instead of a stale one.
    void reset(double now_ms) {
        value = 0.0;
        window_start = now_ms;
        frames = 0;
    }

    // One presented frame. The window closes when it has spanned kFpsWindowMs,
    // which makes the read-out update about once a second.
    void tick(double now_ms) {
        if (window_start <= 0.0) {
            window_start = now_ms;
            frames = 0;
            return;
        }
        ++frames;
        const double elapsed = now_ms - window_start;
        if (elapsed >= static_cast<double>(kFpsWindowMs)) {
            value = static_cast<double>(frames) * 1000.0 / elapsed;
            window_start = now_ms;
            frames = 0;
        }
    }

    // The read-out's caption: a dash until the first window closes, so the
    // counter never flashes a bogus 0 while it starts up.
    std::string text() const {
        if (value <= 0.0) return "FPS --";
        char buf[16];
        std::snprintf(buf, sizeof(buf), "FPS %.0f", value);
        return buf;
    }
};

// The F1 read-out, drawn over the finished frame of any screen: a translucent
// black plate (so it stays legible on the bright title backdrop and on the
// world map alike) with `s` - FpsMeter::text(), "FPS 30" - on it in the game's
// own Age8s face, the small FontModule.Age8s face the GameLbl uses (glyphs8,
// loaded by Scene::load for every screen). The plate is sized from the shipped
// face's own metrics, so a longer number ("FPS 144") widens it instead of
// overflowing.
void draw_fps_overlay(Canvas& cv, const Scene& sc, const std::string& s) {
    const int tw = sc.text_w(sc.glyphs8, s);
    int th = 8;
    for (unsigned char c : s) {
        const Scene::Glyph& gl = sc.glyphs8[c];
        if (gl.ok) th = std::max(th, static_cast<int>(gl.face.h));
    }
    if (tw <= 0) return;
    cv.fill(kFpsX - kFpsPad, kFpsY - kFpsPad, tw + 2 * kFpsPad,
            th + 2 * kFpsPad, 0, 0, 0, kFpsPlateAlpha);
    sc.draw_text8_left(cv, kFpsX, kFpsY, s, 255, 255, 255);
}

// One head-less in-game click: `right` picks the button, so a test can drive a
// mixed interaction (select with the left button, clear with the right).
struct GameClick { int x; int y; bool right; };

// ------------------------------------------------------------------ app/win32
struct App {
    std::string root;
    Scene scene;
    HWND hwnd = nullptr;
    int client_w = 0, client_h = 0;
    bool running = true;
    int frames_done = 0;
    int frames_limit = 0;      // 0 = unlimited
    FpsMeter fps;              // the F1 frames-per-second read-out
    HANDLE frame_timer = nullptr;  // the 30 fps waitable timer (null = WM_TIMER)
    bool headless = false;
    std::string render_png;
    int render_frame = 90;     // frame to render head-less (logo fully faded in)
    bool screen_gtype = false;  // start on the Game Type screen (TMultiWindow)
    bool screen_scenario = false;  // start on the Scenario screen (TScenSetup)
    bool screen_game = false;      // start on the in-game screen (TWorldMap)
    std::string game_map_arg;      // the .HSM the in-game screen shows
    std::string start_map;         // walk Scenario -> select -> Start! head-less
    bool scen_maps = false;        // open the "Select a Map..." dialog in a render
    std::string scen_pick;         // confirm this map on the Scenario screen first
    int scen_dd = -1;              // open race cell N's PlayerCB drop-down
    int scen_human = -1;           // head-less: mark this race id as the Human
                                   // (the drop-down's RacePlayerCBChange path)
    int render_hover = -1;     // orb to draw the caption for (head-less check)
    int mouse_x = 0, mouse_y = 0;  // pointer position for head-less renders
    bool mouse_set = false;
    // --game-click X,Y[,X,Y...]: head-less, run map-window update ticks and click
    // at those display positions (on the map or on the scanner) in order, so the
    // smoke test can pin the camera's response and drive a whole unit selection /
    // walk interaction. `--game-right-click` appends a right-button click, which
    // clears the current selection (the same Scene::game_right_click the
    // WM_RBUTTONDOWN handler calls). `right` picks the button per click, so the
    // list can drive a mixed interaction.
    int game_click_x = 0, game_click_y = 0;
    bool game_click_set = false;
    std::vector<GameClick> game_clicks;
    // --game-key K: head-less, press one in-game map key ('L' = CenterOnLeader,
    // 'C' = centre on the selection) after the clicks.
    int game_key = 0;
    // --game-bench N: head-less, time the hover refresh (the pathfinder and the
    // overlay composite) N times and print the per-iteration cost.
    int game_bench = 0;
    // --game-anim N: head-less, advance a running walk animation N frames
    // (one `game_update` tick each) before the render, so the walk's proof PNG
    // can be taken mid-step.
    int game_anim = 0;
    // --game-clock N: head-less, force the map animation clock (AoWHSMap+0x40)
    // and re-composite the building animations before the render, so the in-game
    // screen's animated buildings can be proved from a PNG (docs/17_BUILDING_ANIM.md).
    int game_clock_override = -1;
    // --turn-frame N: head-less, freeze the TurnButton on absolute library frame
    // N (0..101) before the render, so the turn-change swirl can be proved from
    // a PNG (docs/22_TURN_BUTTONS.md).
    int turn_frame_override = -1;
    // --next-held: head-less, draw the TBNextButton with its held-down picture
    // (TAOWButton's +0x110 state), so the pressed art can be proved from a PNG.
    bool next_held = false;
    // --structure-panel / --structure-*: head-less capture and research hooks for
    // the TIWindow floater. `--structure-panel` opens the first city (or the
    // first structure when the map has no city); the value overrides exist
    // because the city name/income live in external Dict tables and the
    // reference capture uses a scenario that is not part of the install.
    bool structure_panel = false;
    std::string structure_name, structure_race, structure_status;
    int structure_gold = -1, structure_mana = -1;
    int structure_race_index = -1;  // force the BCS_<race> city view
    int crop_x = 0, crop_y = 0, crop_w = 0, crop_h = 0;  // 0 = no crop
    bool music = true;
    // --- the music player (a port of Sound.TSongPlayer, see play_music_thread)
    // `music_playlist` is the song list the audio thread should be playing; the
    // UI thread changes it through set_music_playlist, which bumps `music_gen`
    // so the thread picks up the new list on its next iteration.
    std::vector<std::string> music_playlist;
    unsigned music_gen = 0;
    std::mutex music_mtx;
    std::vector<std::string> title_playlist;  // Songs\TITLE.IT
    std::thread audio_thread;
    std::atomic<bool> audio_stop{false};
    // --- one-shot SFX (Sound.PlaySample) ------------------------------------
    // The UI thread queues a decoded sample; the audio thread mixes it into the
    // music stream, which is exactly how the game's Galaxy engine plays
    // `Sfx\Int\Btn1.wav` over the song. `sfx_cache` is UI-thread only (a sample
    // is decoded once); `sfx_queue` is the hand-off, `sfx_voices` the mixer's.
    std::mutex sfx_mtx;
    std::map<std::string, aow::PcmBuffer> sfx_cache;
    std::vector<aow::PcmBuffer> sfx_queue;
};

App g;

// The in-game screen is laid out at the *client* size, so resizing the window
// reflows it (the bar spans the new width, the tabs widen, the floaters ride
// the edges). The other screens keep their fixed scene and are stretched to the
// window by StretchDIBits, as before.
void sync_game_display() {
    if (g.client_w > 0 && g.client_h > 0) {
        g.scene.disp_w = g.client_w;
        g.scene.disp_h = g.client_h;
    }
}

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

// The UI thread's way to hand the audio thread a new playlist, mirroring the
// game stopping the old song list and starting the new one (AoW.exe
// FUN_00454b70 stops/fades the title theme; FUN_00454d2c loads the map's list
// and plays its first song). The list holds full paths; an empty list means
// silence. Bumping `music_gen` tells the thread to restart from the new list.
void set_music_playlist(std::vector<std::string> songs) {
    std::lock_guard<std::mutex> lk(g.music_mtx);
    g.music_playlist = std::move(songs);
    ++g.music_gen;
    // Report the switch (the original shows the same in its sound dialog): the
    // title theme, then the map's own playlist when a game starts.
    std::printf("menu: music playlist (%zu):", g.music_playlist.size());
    for (const std::string& s : g.music_playlist) std::printf(" %s", s.c_str());
    std::printf("\n");
    std::fflush(stdout);
}

// `Sound.PlaySample` (SoundP.dpl, the game's one-shot sample path): decode the
// sample (`Sfx\Int\Btn1.wav` for the shared UI click, `SFX\EVENTS\NEWTURN.WAV`
// when a human seat's turn starts) relative to the game root and hand it to the
// mixer. A file that is missing or silent is reported once and ignored - the
// buttons keep working exactly as without sound.
void play_game_sfx(const std::string& rel) {
    if (!g.music) return;                    // --no-music silences everything
    if (g.scene.root_dir.empty()) return;
    auto it = g.sfx_cache.find(rel);
    if (it == g.sfx_cache.end()) {
        std::string path = rel;
        for (char& c : path) {
            if (c == '\\') c = '/';
        }
        const fs::path p = fs::path(g.scene.root_dir) / path;
        aow::PcmBuffer buf;
        std::string err;
        if (!aow::load_sample(p.string(), buf, &err) || buf.frames() == 0) {
            std::fprintf(stderr, "menu: sfx %s unavailable (%s)\n", path.c_str(),
                         err.c_str());
            it = g.sfx_cache.emplace(rel, aow::PcmBuffer()).first;
        } else {
            it = g.sfx_cache.emplace(rel, std::move(buf)).first;
        }
    }
    if (it->second.frames() == 0) return;
    std::lock_guard<std::mutex> lk(g.sfx_mtx);
    // Keep the hand-off bounded: without an audio device the thread never
    // drains it, and a player hammering the turn button would queue forever.
    if (g.sfx_queue.size() >= 8) g.sfx_queue.erase(g.sfx_queue.begin());
    g.sfx_queue.push_back(it->second);
}

// Mixes the queued one-shot samples over one rendered music block, resampling
// the sample's own rate to the output's (Galaxy ran everything through one
// 44.1 kHz mixer as well) and clamping the sum so a click can never wrap.
void mix_sfx(std::int16_t* chunk, std::size_t frames) {
    std::vector<aow::PcmBuffer> starting;
    {
        std::lock_guard<std::mutex> lk(g.sfx_mtx);
        starting.swap(g.sfx_queue);
    }
    // The mixer's voices live across blocks, so a sample keeps playing while
    // the next music block is rendered.
    static std::vector<aow::PcmBuffer> voices;
    static std::vector<double> pos;
    for (aow::PcmBuffer& b : starting) {
        voices.push_back(std::move(b));
        pos.push_back(0.0);
    }
    for (std::size_t v = 0; v < voices.size();) {
        const aow::PcmBuffer& b = voices[v];
        const double step = static_cast<double>(b.sample_rate) / 44100.0;
        const std::size_t n = b.frames();
        for (std::size_t i = 0; i < frames; ++i) {
            const std::size_t si = static_cast<std::size_t>(pos[v]);
            if (si >= n) break;
            const int sum_l = static_cast<int>(chunk[i * 2]) + b.samples[si * 2];
            const int sum_r =
                static_cast<int>(chunk[i * 2 + 1]) + b.samples[si * 2 + 1];
            chunk[i * 2] = static_cast<std::int16_t>(
                std::max(-32768, std::min(32767, sum_l)));
            chunk[i * 2 + 1] = static_cast<std::int16_t>(
                std::max(-32768, std::min(32767, sum_r)));
            pos[v] += step;
        }
        if (static_cast<std::size_t>(pos[v]) >= n) {
            voices.erase(voices.begin() + static_cast<std::ptrdiff_t>(v));
            pos.erase(pos.begin() + static_cast<std::ptrdiff_t>(v));
        } else {
            ++v;
        }
    }
}

// The music player thread: a port of Sound.TSongPlayer (SoundP.dpl) as the
// game's own menu / map code drives it. It holds an ordered list of songs and
// plays them one after another - `TSongPlayer.Update` (SoundP 0x55405bf0)
// advances the index when the current song stops playing and wraps back to the
// first at the end - so every song in a map's playlist is heard and the list
// then repeats. Each song is decoded once (no per-song looping), exactly as the
// game does; the title screen is simply the same player fed the single song
// `Songs\TITLE.IT` (AoW.exe TitleSceneShow).
void play_music_thread() {
    aow::AudioOut out;
    std::string err;
    if (!out.open(44100, 2, 16, &err)) {
        std::fprintf(stderr, "menu: audio device unavailable (%s)\n", err.c_str());
        return;
    }
    aow::PlaylistPlayer player(44100);  // the game's Sound.TSongPlayer
    unsigned seen_gen = 0;
    constexpr int kFrames = 4410;  // 0.1 s blocks: keeps song changes responsive
    std::vector<std::int16_t> chunk(kFrames * 2);
    while (!g.audio_stop.load()) {
        // Pick up a playlist change requested by the UI thread (title theme ->
        // the map's own playlist when a game starts, and back again).
        {
            std::lock_guard<std::mutex> lk(g.music_mtx);
            if (g.music_gen != seen_gen) {
                seen_gen = g.music_gen;
                player.set_songs(g.music_playlist);
            }
        }
        player.render(chunk.data(), kFrames);
        mix_sfx(chunk.data(), static_cast<std::size_t>(kFrames));
        out.play_raw(chunk.data(), kFrames);
    }
}

void do_action(int index) {
    // The five actions of AoW.exe 0x0041ae94. Orb 0 ("Scenario") calls
    // FUN_0042abf0, which shows TMultiWindow (the Game Type screen); the rest
    // are reported and left to the rest of the port.
    static const char* kWhat[kButtonCount] = {
        "new game", "load game", "multiplayer", "options", "exit",
    };
    if (index < 0 || index >= kButtonCount) return;
    std::printf("menu: %s\n", kWhat[index]);
    std::fflush(stdout);
    if (index == 0) {
        g.scene.screen = 1;    // show TMultiWindow
        g.scene.gt_frame = 0;  // MultiWinUpdate restarts its +0xF0 slide
        g.scene.gt_hover = -1;
        g.scene.gt_pressed = -1;
    }
    if (index == 4) PostQuitMessage(0);
}

// The Game Type screen's buttons. 0..2 are the game types; 3 is Back to Title
// (TMultiWindow.CloseBtnClick), which closes the form and returns to the title
// scene.
void gtype_action(int index) {
    switch (index) {
    case 0:
        // Single Computer -> TScenSetup (FUN_0042b588 -> FUN_00410bd8).
        g.scene.screen = 2;
        // Every race reads "Unavailable" until a map is confirmed, and a
        // confirmed map lights its races at once - so start fully lit.
        g.scene.scen_glow = kScenGlowFrames;
        g.scene.scen_hover = -1;
        g.scene.scen_pressed = -1;
        g.scene.scen_maps_open = false;
        g.scene.scen_map_off = 0;
        g.scene.scen_sb_drag = false;
        g.scene.select_map(g.scene.scen_map_name);
        std::printf("gametype: single computer\n"); break;
    case 1: std::printf("gametype: tcp internet\n"); break;
    case 2: std::printf("gametype: play by email\n"); break;
    case 3:
        std::printf("gametype: back to title\n");
        g.scene.screen = 0;
        g.scene.gt_frame = 0;
        return;
    default:
        return;
    }
    std::fflush(stdout);
}

// The Scenario screen's own actions. `hit` is from Scene::scen_hit: 0..2 for
// the three buttons, negative for a map-list row.
void scen_action(int hit) {
    Scene& s = g.scene;
    // The race PlayerCB drop-down (TScenSetup RacePlayerCBChange 0x00412a24).
    if (hit == -4200) {                       // a click-away closes the list
        s.scen_dd_gi = -1;
        s.scen_dd_hover = -1;
        return;
    }
    if (hit <= -4100 && hit > -4100 - kScenPTypeCount) {
        // A drop-down row: set this race's player type. The original maps the
        // caption through PlayerNameToPlayerID and calls hSetPlayerID, which
        // stores the type into the race's TSetupSettings record.
        const int item = -hit - 4100;
        const int gi = s.scen_dd_gi;
        if (gi >= 0 && gi < kScenRaceCount) {
            const int race = kScenGridRace[gi];
            const int nt = kScenPTypeItem[item];
            s.scen_choose_type(race, nt);
            std::printf("scenario: %s player -> %s\n", kScenRaceName[race],
                        kScenPTypeName[nt < 8 ? nt : 5]);
        }
        s.scen_dd_gi = -1;
        s.scen_dd_hover = -1;
        std::fflush(stdout);
        return;
    }
    if (hit <= -4000 && hit > -4000 - kScenRaceCount) {
        // Open the clicked cell's drop-down.
        s.scen_dd_gi = -hit - 4000;
        s.scen_dd_hover = -1;
        return;
    }
    if (hit == -2000 || hit == -2001) {  // Select / Cancel close the dialog
        s.scen_maps_open = false;
        if (hit == -2000 && s.scen_map_sel >= 0 &&
            s.scen_map_sel < static_cast<int>(s.scen_maps.size())) {
            // Selecting a map loads it into the setup control, which is what
            // lights the races (FUN_00411460). select_map() maxes the fog
            // counter, so the lit cells are bright on the very next frame.
            s.select_map(s.scen_maps[static_cast<std::size_t>(s.scen_map_sel)],
                         true);
        }
        std::printf("scenario: map dialog %s\n",
                    hit == -2000 ? "select" : "cancel");
        return;
    }
    if (hit >= -3004 && hit <= -3000) {  // the map-list scrollbar (NSBar)
        const Scene::ScenSb b = s.scen_sb();
        switch (hit) {
        case -3000:  // dec (up arrow)
            if (s.scen_map_off > 0) --s.scen_map_off;
            break;
        case -3001:  // inc (down arrow)
            if (s.scen_map_off < b.max_off) ++s.scen_map_off;
            break;
        case -3002:  // page up (track above the thumb)
            s.scen_map_off -= kScenListRows;
            if (s.scen_map_off < 0) s.scen_map_off = 0;
            break;
        case -3003:  // page down (track below the thumb)
            s.scen_map_off += kScenListRows;
            if (s.scen_map_off > b.max_off) s.scen_map_off = b.max_off;
            break;
        default:     // -3004: the thumb - dragged via WM_MOUSEMOVE
            break;
        }
        return;
    }
    if (hit <= -1000) {
        s.scen_map_sel = -hit - 1000;
        if (s.scen_map_sel >= 0 &&
            s.scen_map_sel < static_cast<int>(s.scen_maps.size())) {
            s.select_map(s.scen_maps[static_cast<std::size_t>(s.scen_map_sel)]);
        }
        return;
    }
    switch (hit) {
    case 0:  // PickScenarioBtnClick 0x00412460 -> TFileDlgWindow
        s.scen_maps_open = true;
        s.scen_map_sel = -1;   // FileDialogShow opens the list unselected
        s.scen_map_off = 0;    // ...and scrolled to the top
        std::printf("scenario: pick map\n");
        break;
    case 1:  // ExitGameClick 0x00411f50 -> back to the game-type screen
        std::printf("scenario: exit\n");
        s.scen_maps_open = false;
        s.screen = 1;
        s.gt_frame = 0;
        return;
    case 2:  // StartBtnClick 0x00412568 -> leave setup, load the map, play
    {
        std::string err;
        if (!s.start_game(&err)) {
            std::printf("scenario: start failed (%s)\n", err.c_str());
            break;
        }
        sync_game_display();   // lay the game screen out at the client size
        std::printf("scenario: start -> game (map=%s %s towns=%d players=%d)\n",
                    s.game_map_name.c_str(), s.scen_map_size.c_str(),
                    s.scen_map_towns, s.scen_map_players);
        s.scen_maps_open = false;
        return;
    }
    default:
        return;
    }
    std::fflush(stdout);
}

// The in-game screen's own actions. `hit` is from Scene::game_hit.
void game_action(int hit) {
    Scene& s = g.scene;
    // The Structure floater's controls (the DFM's own handlers: MinClick /
    // CloseClick / CenterClick / HelpClick / MainBtnClick / RazeBtnClick).
    if (hit >= 1000) {
        switch (hit) {
        case 1000:  // CloseClick
        case 1001:  // MinClick
            s.close_structure();
            std::printf("game: structure floater closed\n");
            break;
        case 1002:  // CenterClick: centre the map on the selected structure
            if (s.game_struct_hx >= 0) {
                s.cam_center(s.game_struct_hx, s.game_struct_hy, s.cam_level);
            }
            break;
        case 1003:  // HelpClick
            s.game_say("Structure window help");
            break;
        case 1004:  // MainBtnClick (Enter): hand over to the city screen
            s.close_structure();
            s.game_say("enter structure");
            std::printf("game: structure enter\n");
            break;
        case 1005:  // RazeBtnClick
            s.game_say("raze structure");
            std::printf("game: structure raze\n");
            break;
        default: break;
        }
        std::fflush(stdout);
        return;
    }
    // The party floater's own controls (game_hit offsets its ids by 100 so they
    // cannot collide with the bar's).
    if (hit >= 100) {
        s.party_action(hit - 100);
        return;
    }
    switch (hit) {
    case 0:  // WMButtonClick - show / hide the World Map floater
        s.game_show_scan = !s.game_show_scan;
        std::printf("game: world map floater %s\n",
                    s.game_show_scan ? "on" : "off");
        break;
    case 4:  // InfoBtnClick - show / hide the Events floater
        s.game_show_events = !s.game_show_events;
        std::printf("game: events floater %s\n",
                    s.game_show_events ? "on" : "off");
        break;
    case 1: std::printf("game: realm\n"); break;
    case 2: std::printf("game: magic\n"); break;
    case 3: std::printf("game: relations\n"); break;
    case 5:  // ScannerTab "Surface"
    case 6:  // ScannerTab "Caverns"
    {
        const int lvl = hit - 5;
        if (lvl == s.game_level) break;
        if (!s.game_set_level(lvl)) {
            // The map has no such level - the previous render stays up.
            std::fprintf(stderr, "menu: level %d unavailable\n", lvl);
        } else {
            std::printf("game: level %d\n", s.game_level);
        }
        // The camera's level follows the scanner's (THSMapViewer.SetSceneL is
        // what the engine's scanner tab calls), re-clamped to the new level's
        // extents.
        s.cam_level = s.game_level;
        s.cam_set_x(s.cam_x);
        s.cam_set_y(s.cam_y);
        break;
    }
    case kGameHitNext:  // TBNextButtonClick - the shared click + SelectNextArmy
        s.game_next_button_click();
        break;
    case kGameHitTurn:  // TAOWTurnButton.CheckMouseDown is the mouse-DOWN path;
                        // the head-less driver and a keyboard-style activation
                        // land here, with the same OnChange.
        s.game_turn_button_click();
        break;
    case kGameHitScanMin:  // SMinIcon - minimize the World Map floater
        s.game_show_scan = false;
        std::printf("game: world map floater minimized\n");
        break;
    default:
        return;
    }
    std::fflush(stdout);
}

// ----------------------------------------------------------------- frame clock
// A 30 fps frame clock: a waitable timer (high resolution where the OS has it)
// that the message loop waits on, so a frame lands every 33 ms. WM_TIMER - which
// the system quantises to its 15.6 ms clock tick, so SetTimer(33) really fires
// every 46.9 ms (~21 fps) - drives the ticks only when no waitable timer can be
// created, so the game still runs (just slower) on such a system.
bool start_frame_clock(HWND hwnd) {
    HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr,
                                          CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                          TIMER_ALL_ACCESS);
    if (!timer) {
        // Pre-1803 Windows: a plain waitable timer is still not bound to the
        // WM_TIMER tick and keeps the 33 ms period.
        timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    }
    if (timer) {
        LARGE_INTEGER due{};
        due.QuadPart = -static_cast<LONGLONG>(kFrameMs) * 10000;  // 100 ns units
        if (SetWaitableTimer(timer, &due, static_cast<LONG>(kFrameMs), nullptr,
                             nullptr, FALSE)) {
            g.frame_timer = timer;
            return true;
        }
        CloseHandle(timer);
    }
    SetTimer(hwnd, kFrameTimerId, kFrameMs, nullptr);
    return false;
}

void stop_frame_clock(HWND hwnd) {
    if (g.frame_timer) {
        CancelWaitableTimer(g.frame_timer);
        CloseHandle(g.frame_timer);
        g.frame_timer = nullptr;
    }
    KillTimer(hwnd, kFrameTimerId);
}

// One frame tick (AoW.exe 0x0041ae08: the per-frame update, which increments the
// scene's +0x64/+0x68). Run by the message loop whenever the waitable timer
// signals, or from WM_TIMER when that timer is unavailable.
void on_frame_tick(HWND hwnd) {
    Scene& s = g.scene;
    if (s.screen == 1) {
        // MultiWinUpdate: the panels slide for kGtPanelH/kGtAnimStep
        // frames (286/10 = 29 frames, ~1 s at the form's 30 fps).
        if (s.gt_frame < kGtPanelH) s.gt_frame += kGtAnimStep;
        if (s.gt_frame > kGtPanelH) s.gt_frame = kGtPanelH;
    } else if (s.screen == 2) {
        // ---- the Scenario screen's per-frame Update ----
        // Nothing to animate: FUN_00411460 lights the races the chosen map's
        // roster holds the moment the map is picked, so the Scenario screen
        // sets scen_glow to kScenGlowFrames there and each cell's R*FogPnl
        // simply drops out (a race with no player keeps its fog -
        // FUN_00410f2c's param_2 == 0 branch).
    } else if (s.screen == 3) {
        // The map window's own Update (AoW.exe FUN_00452e50): resolve the
        // hovered hex and edge-scroll, once per frame. This is what keeps
        // the view scrolling while the pointer rests against an edge.
        s.game_update();
    } else if (s.screen == 0) {
        s.counter64++;
        s.counter68++;
        // The real screen rolls the credits after frame 750 and then returns
        // to the logo; the replica restarts the title timeline there.
        if (s.counter68 >= static_cast<std::uint32_t>(kTitleEnd)) {
            s.counter64 = 0;
            s.counter68 = 0;
        }
    }
    ++g.frames_done;
    InvalidateRect(hwnd, nullptr, FALSE);
    if (g.frames_limit && g.frames_done >= g.frames_limit) {
        PostQuitMessage(0);
    }
}

void blit_to_window(HDC hdc) {
    if (g.client_w <= 0 || g.client_h <= 0) return;
    // One frame is about to be presented: F1's read-out counts them here (see
    // FpsMeter) - every WM_PAINT is one frame the player actually sees.
    g.fps.tick(bench_now_ms());
    const int hover = g.scene.hit_test(g.scene.mouse_x, g.scene.mouse_y);
    Canvas cv = g.scene.render(hover);
    // The F1 frame-rate read-out rides on top of the finished frame, whatever
    // screen it is, so it never disturbs a screen's own composition.
    if (g.fps.on) draw_fps_overlay(cv, g.scene, g.fps.text());

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
        // 30 fps - the form's FrameRate, driven by a waitable timer (WM_TIMER
        // is only the fallback: it is quantised to the system clock tick).
        start_frame_clock(hwnd);
        return 0;
    case WM_TIMER:
        // Fallback tick only: the normal clock is the waitable timer the message
        // loop waits on (see start_frame_clock).
        on_frame_tick(hwnd);
        return 0;
    case WM_SIZE:
        g.client_w = LOWORD(lp);

        g.client_h = HIWORD(lp);
        // The in-game screen follows the client size so it reflows; the other
        // screens keep their fixed scene and are stretched by StretchDIBits.
        if (g.scene.screen == 3) sync_game_display();
        return 0;
    case WM_MOUSEMOVE: {
        Scene& s = g.scene;
        s.mouse_x = GET_X_LPARAM(lp);
        s.mouse_y = GET_Y_LPARAM(lp);
        if (s.screen == 1) {
            s.gt_hover = s.gtype_hit(s.mouse_x, s.mouse_y);
        } else if (s.screen == 2) {
            if (s.scen_sb_drag) {
                // Dragging the map-list thumb: map the pointer's y onto
                // scen_map_off (TAOWVScrollBar's thumb-drag).
                const Scene::ScenSb b = s.scen_sb();
                const int travel = b.span - b.thumb_h;
                if (travel > 0 && b.max_off > 0) {
                    int ty = s.mouse_y - s.scen_sb_grab - b.ty0;
                    if (ty < 0) ty = 0;
                    if (ty > travel) ty = travel;
                    s.scen_map_off = static_cast<int>(
                        (static_cast<long long>(ty) * b.max_off + travel / 2) /
                        travel);
                }
            } else {
                s.scen_hover = s.scen_hit(s.mouse_x, s.mouse_y);
                // Track the hovered row of an open race PlayerCB drop-down.
                s.scen_dd_hover =
                    (s.scen_dd_gi >= 0 && s.scen_hover <= -4100 &&
                     s.scen_hover > -4100 - kScenPTypeCount)
                        ? (-s.scen_hover - 4100)
                        : -1;
            }
        } else if (s.screen == 3) {
            s.game_hover = s.game_hit(s.mouse_x, s.mouse_y);
            // Retarget the selected army's trail at once (the engine's
            // TSelectedArmy.CalculateMovePath runs off the map mouse's Move).
            {
                const int ohx = s.game_hover_hx, ohy = s.game_hover_hy;
                s.game_resolve_hover();
                if (s.game_hover_hx != ohx || s.game_hover_hy != ohy) {
                    s.game_update_path();
                }
            }
            // Dragging on the scanner slides the view rectangle, exactly like
            // dragging TScanner's view rect (the engine's scanner handles its
            // own mouse). The edge scroll itself is per *frame*, not per move,
            // so it stays in game_update().
            if (s.game_mouse_down && s.game_show_scan) {
                const Scene::GameLayout L = s.game_layout();
                const AowBox& m = L.scan_map;
                if (m.x <= s.mouse_x && s.mouse_x < m.x + m.w &&
                    m.y <= s.mouse_y && s.mouse_y < m.y + m.h) {
                    s.game_scanner_click(s.mouse_x, s.mouse_y);
                }
            }
        } else {
            // WindowUpdate stores the hovered index in +0x7c.
            s.cursor_index = s.hit_test(s.mouse_x, s.mouse_y);
        }
        return 0;
    }
    case WM_MOUSEWHEEL: {
        // The map picker's list also scrolls with the wheel (one row a notch).
        Scene& s = g.scene;
        if (s.screen == 2 && s.scen_maps_open) {
            const Scene::ScenSb b = s.scen_sb();
            s.scen_map_off += GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1;
            if (s.scen_map_off < 0) s.scen_map_off = 0;
            if (s.scen_map_off > b.max_off) s.scen_map_off = b.max_off;
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        Scene& s = g.scene;
        s.mouse_x = GET_X_LPARAM(lp);
        s.mouse_y = GET_Y_LPARAM(lp);
        if (s.screen == 1) {
            s.gt_pressed = s.gtype_hit(s.mouse_x, s.mouse_y);
        } else if (s.screen == 2) {
            const int sc = s.scen_hit(s.mouse_x, s.mouse_y);
            s.scen_pressed = sc;
            // Grabbing the map-list scrollbar thumb starts a drag; the arrows
            // and track pages act on release (scen_action), like the buttons.
            if (sc == -3004) {
                const Scene::ScenSb b = s.scen_sb();
                s.scen_sb_drag = true;
                s.scen_sb_grab = s.mouse_y - s.scen_sb_thumb_y(b);
            }
        } else if (s.screen == 3) {
            // AoW.exe 0x00450e30 (MapWindowMouseDown) hands the click to the map
            // mouse, whose Down (HSEPack 0x5560b838) selects the hex under the
            // cursor and lets the game centre the view on it. The scanner floater
            // is its own window and sits over the map, so it takes the click
            // first.
            s.game_update();
            s.game_mouse_down = true;
            // The bar's two turn buttons take the press before the map: the
            // TurnButton fires its OnChange on mouse-DOWN (TAOWTurnButton
            // .CheckMouseDown 0x59705018), the NextButton only arms its pressed
            // picture (+0x110 = -1) and fires OnClick on release.
            {
                const int bh = s.game_hit(s.mouse_x, s.mouse_y);
                if (bh == kGameHitTurn) {
                    s.game_turn_button_click();
                    return 0;
                }
                if (bh == kGameHitNext) {
                    s.tb_next_down = true;
                    return 0;
                }
            }
            // The party floater sits over the map (its window has a higher
            // Priority than the map window), so it takes the click first: the
            // pressed control keeps its "down" art until the button is released.
            if (s.game_show_party) {
                const int ph = s.party_hit_test(s.mouse_x, s.mouse_y);
                if (ph >= 0) {
                    s.party_pressed = ph;
                    return 0;
                }
            }
            const Scene::GameLayout L = s.game_layout();
            const AowBox& m = L.scan_map;
            if (s.game_show_scan && m.x <= s.mouse_x && s.mouse_x < m.x + m.w &&
                m.y <= s.mouse_y && s.mouse_y < m.y + m.h) {
                s.game_scanner_click(s.mouse_x, s.mouse_y);
            } else if (s.game_in_map(s.mouse_x, s.mouse_y)) {
                s.game_map_click();
            }
        } else {
            // WindowMouseDown stores the hit index in +0x80.
            s.armed_index = s.hit_test(s.mouse_x, s.mouse_y);
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        Scene& s = g.scene;
        s.mouse_x = GET_X_LPARAM(lp);
        s.mouse_y = GET_Y_LPARAM(lp);
        if (s.screen == 1) {
            const int hit = s.gtype_hit(s.mouse_x, s.mouse_y);
            if (hit >= 0 && hit == s.gt_hover) gtype_action(hit);
            s.gt_hover = -1;
            s.gt_pressed = -1;
            return 0;
        }
        if (s.screen == 2) {
            if (s.scen_sb_drag) {           // a thumb drag ends here, no action
                s.scen_sb_drag = false;
                s.scen_hover = -1;
                s.scen_pressed = -1;
                return 0;
            }
            const int hit = s.scen_hit(s.mouse_x, s.mouse_y);
            if (hit != -1 && hit == s.scen_hover) scen_action(hit);
            s.scen_hover = -1;
            s.scen_pressed = -1;
            return 0;
        }
        if (s.screen == 3) {
            const int hit = s.game_hit(s.mouse_x, s.mouse_y);
            if (hit == kGameHitTurn) {
                // TAOWTurnButton has only an OnChange, fired on mouse-DOWN
                // (CheckMouseDown), so the release must not run the end-turn a
                // second time.
            } else if (hit == kGameHitNext) {
                // TAOWButton.CheckMouseUp (AoWInterface.dpl 0x0040ba6c) fires
                // OnClick only when the button was armed and the pointer is
                // still over it; TBNextButtonClick plays the click sound and
                // selects the next army.
                if (s.tb_next_down) s.game_next_button_click();
            } else if (hit != -1 && hit == s.game_hover) {
                game_action(hit);
            }
            s.tb_next_down = false;      // +0x110 cleared on every release
            s.game_hover = -1;
            s.party_pressed = -1;
            s.game_mouse_down = false;   // THSMapMouse.Up (0x5560b8a0)
            return 0;
        }
        const int hit = s.hit_test(s.mouse_x, s.mouse_y);
        // WindowMouseUp fires when the hit index still matches +0x7c, then
        // clears it.
        if (hit >= 0 && hit == s.cursor_index) {
            do_action(hit);
        }
        s.cursor_index = -1;
        return 0;
    }
    case WM_RBUTTONDOWN: {
        // A right-click on the world map clears the current selection (the engine
        // removes the party floater that way, which deselects the party). Only the
        // main map window reacts, so a future right-click on the bar or a floater
        // can use its own behaviour.
        Scene& s = g.scene;
        s.mouse_x = GET_X_LPARAM(lp);
        s.mouse_y = GET_Y_LPARAM(lp);
        if (s.screen == 3 && s.game_in_map(s.mouse_x, s.mouse_y)) {
            s.game_right_click();
        }
        return 0;
    }
    case WM_KEYDOWN:
        // F1 toggles the frame-rate read-out. It is a debugging aid that
        // belongs to no screen, so it is resolved before the screens' own key
        // tables (see FpsMeter / draw_fps_overlay).
        if (wp == VK_F1) {
            g.fps.on = !g.fps.on;
            g.fps.reset(bench_now_ms());
            InvalidateRect(hwnd, nullptr, FALSE);  // show / hide it at once
            return 0;
        }
        // TAoWHSMapKeyboard.KeyDown (AoWEPACK 0x55773b08): the in-game map's own
        // key table (the 'C' / 'L' camera keys under Screen::game_key).
        if (g.scene.screen == 3 && g.scene.game_key(wp)) return 0;
        if (wp == VK_ESCAPE) {
            // Escaping the Game Type screen closes it, like CloseBtnClick.
            if (g.scene.screen == 1) g.scene.screen = 0;
            else if (g.scene.screen == 2) {
                // Close the map dialog, or leave the Scenario screen, like
                // ExitGameClick.
                if (g.scene.scen_maps_open) g.scene.scen_maps_open = false;
                else { g.scene.screen = 1; g.scene.gt_frame = 0; }
            }
            else if (g.scene.screen == 3) {
                // Back to the setup screen (the engine opens its game menu here;
                // returning to TScenSetup keeps the port navigable). Leaving the
                // map stops its playlist and brings the title theme back, as
                // AoW.exe FUN_0045530c does on the way out to the title screen.
                g.scene.screen = 2;
                g.scene.scen_glow = kScenGlowFrames;
                set_music_playlist(g.title_playlist);
            }
            else PostQuitMessage(0);
        }
        return 0;
    case WM_SETCURSOR:
        // The scene paints the AoW pointer sprite itself, so hide the system
        // one while it is over the client area.
        if (LOWORD(lp) == HTCLIENT && g.scene.cursor_on) {
            SetCursor(nullptr);
            return TRUE;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
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
        stop_frame_clock(hwnd);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
}


// ---------------------------------------------------------------------- main
void print_usage() {
    std::printf(
        "AoW - Age of Wonders 1 title-screen replica\n"
        "usage: AoW [options]\n"
        "  --root <dir>          game install root (default: exe dir, then ..)\n"
        "  --width N             display width  (default 640)\n"
        "  --height N            display height (default 480)\n"
        "  --render <out.png>    render one frame head-less and exit\n"
        "  --frame N             frame to render with --render (default 90)\n"
        "  --hover N             draw orb N's caption with --render (0..4)\n"
        "  --mouse X Y           pointer position (--render and the window)\n"
        "  --cursor-id N         Cursors.ilb entry id for the pointer\n"
        "                        (default 0 = the white arrow)\n"
        "  --no-cursor           do not draw the AoW pointer at all\n"
        "  --font-variant N      AoW15WhiteGrey storage layer for the captions\n"
        "                        (0 = font2.BMP, 1 = font.bmp; default 1)\n"
        "  --crop X Y W H        crop the rendered image (small previews)\n"
        "  --frames N            quit after N frames (smoke test)\n"
        "  --fps N               draw the F1 frame-rate read-out with value N\n"
        "                        (F1 toggles the live counter in the window)\n"
        "  --captions a,b,c,d,e  hover captions for the five orbs\n"
        "  --overlay             enable the engine's TSprite16 orb overlay\n"
        "  --screen title|gtype|scenario  start: title, Game Type, or Scenario\n"
        "                        screen (TMultiWindow; shows after orb 0)\n"
        "  --screen game         start on the in-game screen (the rendered map +\n"
        "                        the TWorldMap bar / floaters)\n"
        "  --map <file.HSM>      the map the in-game screen shows\n"
        "  --start-map <file.HSM>  head-less: run the Scenario -> select ->\n"
        "                        Start! flow and render the in-game screen\n"
        "  --scen-human R        head-less with --start-map: mark race R (0..11)\n"
        "                        as the Human (the race PlayerCB), so that race's\n"
        "                        cities and armies are the ones played\n"
        "  --gtype-frame N       Game Type slide counter (0..286)\n"
        "  --game-click X Y      head-less: one map-window update, then a click at\n"
        "                        (X,Y) on the world map or its scanner (repeatable)\n"
        "  --game-right-click X Y  head-less: one map-window update, then a right-\n"
        "                        click at (X,Y) on the world map (clears selection)\n"
        "  --game-key K          head-less: press one map key (L = centre on the\n"
        "                        leader, C = centre on the selection)\n"
        "  --game-bench N        head-less: time the hover refresh N times\n"
        "  --game-anim N         head-less: advance a walk animation N frames\n"
        "  --game-clock N        head-less: force the map clock (building anim)\n"
        "  --turn-frame N        head-less: freeze the TurnButton on frame N (0..101)\n"
        "  --next-held           head-less: draw the NextButton held down\n"
        "  --structure-panel     head-less: open the Structure floater on the first\n"
        "                        city/structure after loading --start-map\n"
        "  --structure-name S    override the floater's city/structure name\n"
        "  --structure-race S    override its race label\n"
        "  --structure-race-index N  load RACES\\R*.ILB index N for the city view\n"
        "  --structure-status S  override its status label\n"
        "  --structure-gold N    override its gold read-out\n"
        "  --structure-mana N    override its mana read-out\n"

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

// Applies one head-less in-game click at a display position. A left click: the
// World Map floater takes it when it covers the point, else the map window does
// (exactly the WM_LBUTTONDOWN order for screen 3). A right click always goes to
// the map window and clears the selection (WM_RBUTTONDOWN).
void apply_game_click(int cx, int cy, bool right = false) {
    Scene& s = g.scene;
    // A scripted click happens after the previous move's walk has played; the
    // head-less driver has no timer between clicks, so run the walk to
    // completion first - exactly what the window procedure's 30 fps ticks would
    // have done before the user's next click. Without this, a click that lands
    // on the army just moved is swallowed by `game_map_click`'s `mv_anim` gate.
    // Park the pointer off the map while ticking so the edge scroll does not
    // move the camera under the coordinates the script is about to use.
    s.mouse_x = -1;
    s.mouse_y = -1;
    while (s.mv_anim) s.game_update();
    s.mouse_x = cx;
    s.mouse_y = cy;
    s.game_update();
    if (right) {
        if (s.game_in_map(cx, cy)) s.game_right_click();
        std::printf("game_rclick=map %d,%d\n", cx, cy);
        std::fflush(stdout);
        return;
    }
    const Scene::GameLayout L = s.game_layout();
    const AowBox& m = L.scan_map;
    // The bar's two turn buttons (the window procedure's WM_LBUTTONDOWN order:
    // they sit under the pointer before the map/scanner/party tests).
    {
        const int bh = s.game_hit(cx, cy);
        if (bh >= 1000) {
            game_action(bh);
            std::printf("game_click=structure %d,%d id=%d\n", cx, cy, bh);
            std::fflush(stdout);
            return;
        }
        if (bh == kGameHitTurn) {
            // A human waits for the button's turn-change animation before it
            // accepts the next press (CheckMouseDown's [+0x120] gate); run the
            // ticks the window would have run.
            s.mouse_x = -1;
            s.mouse_y = -1;
            for (int i = 0; i < 4000 && s.tb_turn_animating; ++i) s.game_update();
            s.mouse_x = cx;
            s.mouse_y = cy;
            s.game_update();
            s.game_turn_button_click();
            std::printf("game_click=turn %d,%d checked=%d day=%d\n", cx, cy,
                        s.tb_turn_checked ? 1 : 0, s.game_day);
            std::fflush(stdout);
            // The click starts the turn-change animation; the window's 30 fps
            // ticks would play it out (and the AI seats' turns) before the next
            // click, so the scripted driver does the same.
            s.mouse_x = -1;
            s.mouse_y = -1;
            for (int i = 0; i < 4000; ++i) {
                s.game_update();
                if (!s.tb_turn_animating && s.tb_turn_mode == 0 &&
                    s.tb_turn_last == s.tb_turn_cur &&
                    s.mv_gs.current() == s.mv_human) {
                    break;
                }
            }
            std::printf("game_turnnow=player=%d day=%d frame=%d\n",
                        static_cast<int>(s.mv_gs.current()), s.game_day,
                        s.tb_turn_draw_frame());
            std::fflush(stdout);
            return;
        }
        if (bh == kGameHitNext) {
            s.tb_next_down = true;   // armed, as CheckMouseDown does
            s.game_next_button_click();
            s.tb_next_down = false;
            std::printf("game_click=next %d,%d\n", cx, cy);
            std::fflush(stdout);
            return;
        }
    }
    // The party floater is over the map and the scanner, so it takes the click
    // first (the same order the window procedure uses), then the scanner, then
    // the map itself.
    if (s.game_show_party) {
        const int ph = s.party_hit_test(cx, cy);
        if (ph >= 0) {
            s.party_action(ph);
            std::printf("game_click=party %d,%d id=%d\n", cx, cy, ph);
            std::fflush(stdout);
            return;
        }
    }
    if (s.game_show_scan && m.x <= cx && cx < m.x + m.w && m.y <= cy &&
        cy < m.y + m.h) {
        s.game_scanner_click(cx, cy);
        std::printf("game_click=scanner %d,%d\n", cx, cy);
    } else {
        s.game_map_click();
        std::printf("game_click=map %d,%d\n", cx, cy);
    }
    std::fflush(stdout);
}

int run_headless() {
    if (!g.start_map.empty()) {
        // Walk the real UI flow head-less: open the Scenario screen, select the
        // map (the picker's Select), then press Start! - which loads the map and
        // switches to the in-game screen (Scene::start_game).
        g.scene.screen = 2;
        g.scene.select_map(g.start_map, true);
        // --scen-human: pick the race the player marks Human, through the same
        // RacePlayerCBChange path the drop-down takes, so the head-less flow can
        // prove which race's units become the played ones.
        if (g.scen_human >= 0) {
            g.scene.scen_choose_type(g.scen_human,
                                     static_cast<int>(aow::PlayerType::Human));
        }
        g.scene.scen_glow = kScenGlowFrames;
        std::string err;
        if (!g.scene.start_game(&err)) {
            std::fprintf(stderr, "menu: %s\n", err.c_str());
            return 1;
        }
        std::printf("flow=scenario-select-start map=%s\n", g.start_map.c_str());
        // --structure-panel: open TIWindow on the first city of the loaded map
        // (falling back to the first structure) before the render.
        if (g.structure_panel) {
            bool opened = false;
            auto find_and_open = [&](bool city_only) {
                for (std::size_t l = 0; l < g.scene.game_state_map.levels.size() &&
                                       !opened; ++l) {
                    const aow::MapLevel& lv = g.scene.game_state_map.levels[l];
                    for (std::uint32_t y = 0; y < lv.height && !opened; ++y) {
                        for (std::uint32_t x = 0; x < lv.width; ++x) {
                            const aow::MapHs* hs = g.scene.structure_on_hex(
                                static_cast<int>(x), static_cast<int>(y));
                            if (hs == nullptr) continue;
                            if (city_only && hs->class_id != aow::kCityClassId) {
                                continue;
                            }
                            g.scene.game_level = static_cast<int>(l);
                            if (g.scene.open_structure(static_cast<int>(x),
                                                       static_cast<int>(y))) {
                                opened = true;
                                break;
                            }
                        }
                    }
                }
            };
            find_and_open(true);
            if (!opened) find_and_open(false);
            if (opened) {
                if (!g.structure_name.empty()) {
                    g.scene.game_struct_name = g.structure_name;
                }
                if (!g.structure_race.empty()) {
                    g.scene.game_struct_race = g.structure_race;
                }
                if (!g.structure_status.empty()) {
                    g.scene.game_struct_status = g.structure_status;
                }
                if (g.structure_gold >= 0) g.scene.game_struct_gold = g.structure_gold;
                if (g.structure_mana >= 0) g.scene.game_struct_mana = g.structure_mana;
                if (g.structure_race_index >= 0) {
                    g.scene.game_struct_race_index = g.structure_race_index;
                    g.scene.load_structure_art(g.structure_race_index);
                }
                std::printf("structure_panel=%d,%d city=%d\n",
                            g.scene.game_struct_hx, g.scene.game_struct_hy,
                            g.scene.game_struct_city ? 1 : 0);
            } else {
                std::fprintf(stderr, "menu: no structure to open\n");
            }
        }
    } else if (g.screen_gtype) {
        g.scene.screen = 1;
        g.scene.gt_frame = g.render_frame;
        if (g.render_hover >= 0) g.scene.gt_hover = g.render_hover;
    } else if (g.screen_scenario) {
        g.scene.screen = 2;
        // A confirmed map lights its races at once (select_map), and until then
        // every cell is "Unavailable" whatever the fog counter says.
        if (g.render_hover >= 0) g.scene.scen_hover = g.render_hover;
        // --scen-pick confirms a map (the picker's Select), which lights its
        // races and shows each one's player type from the map's roster.
        if (!g.scen_pick.empty()) {
            g.scene.select_map(g.scen_pick, true);
        } else {
            g.scene.select_map(g.scene.scen_map_name);
        }
        g.scene.scen_maps_open = g.scen_maps;
        // FileDialogShow opens the list with no row selected (and so with the
        // info strip's values blank), which is what the reference capture shows.
        if (g.scen_maps) g.scene.scen_map_sel = -1;
        // --scen-dd opens one race cell's PlayerCB drop-down in the render.
        if (g.scen_dd >= 0 && g.scen_dd < kScenRaceCount) {
            g.scene.scen_dd_gi = g.scen_dd;
        }
    } else if (g.screen_game) {
        g.scene.screen = 3;
        std::string err;
        if (!g.scene.load_game_map(g.game_map_arg, &err)) {
            std::fprintf(stderr, "menu: %s\n", err.c_str());
            return 1;
        }
    } else {
        g.scene.counter64 = static_cast<std::uint32_t>(g.render_frame);
        g.scene.counter68 = static_cast<std::uint32_t>(g.render_frame);
    }
    if (g.mouse_set) {
        g.scene.mouse_x = g.mouse_x;
        g.scene.mouse_y = g.mouse_y;
    } else if (g.render_hover >= 0 && !g.screen_gtype && !g.screen_scenario) {
        // Park the pointer over the hovered orb, as in the game's own
        // "orb selected" look, and let the state match a real hover.
        const int orb_w = static_cast<int>(g.scene.orb_slot_w);
        g.scene.mouse_x = g.scene.button_left(g.render_hover) + orb_w * 3 / 4;
        g.scene.mouse_y = g.scene.scene_top() + kButtonY + 60;
        g.scene.cursor_index = g.render_hover;
    }
    if (g.scene.screen == 3) {
        // One update tick of the map window (AoW.exe FUN_00452e50): resolve the
        // hex under the parked pointer and apply the edge scroll.
        g.scene.game_update();
        if (!g.game_clicks.empty()) {
            // --game-click / --game-right-click may be repeated: each click is
            // applied in order, so the smoke test can drive a whole interaction
            // (the scanner to bring the army into view, a click to select it, a
            // right-click to clear it, a click to walk it).
            for (const GameClick& c : g.game_clicks) {
                apply_game_click(c.x, c.y, c.right);
            }
        } else if (g.game_click_set) {
            apply_game_click(g.game_click_x, g.game_click_y);
        }
        // --game-key runs before the hover, so a keystroke that moves the camera
        // is followed by the pointer being resolved against the new view - exactly
        // the interactive order (key press, then mouse move).
        if (g.game_key != 0) g.scene.game_key(g.game_key);
        if (g.game_bench > 0) g.scene.game_bench_hover(g.game_bench);
        // --game-anim advances a running walk animation frame by frame so the
        // proof PNG can show the army mid-step (the same tick the 30 fps timer
        // runs).
        // A parked `--mouse` pointer also drives the map window's edge scroll, so
        // it is set before the `--game-anim` ticks (the hover is resolved again
        // below, after them).
        if (g.mouse_set) {
            g.scene.mouse_x = g.mouse_x;
            g.scene.mouse_y = g.mouse_y;
        }
        // --next-held: draw the NextButton's held-down picture (+0x110 = -1).
        if (g.next_held) g.scene.tb_next_down = true;
        for (int i = 0; i < g.game_anim; ++i) g.scene.game_update();
        // --turn-frame N: freeze the TurnButton on one library frame (the class
        // draws AnimStart + Frame; frames 0..10 are the glow block, AnimStart 0).
        if (g.turn_frame_override >= 0) {
            int n = g.turn_frame_override;
            if (n < 0) n = 0;
            if (n >= kTbTurnFrameCount) n = kTbTurnFrameCount - 1;
            Scene& s = g.scene;
            s.tb_turn_start = n < kTbTurnGlowFrames ? 0 : kTbTurnAnimStart;
            s.tb_turn_frame = n - s.tb_turn_start;
            s.tb_turn_last = s.tb_turn_cur;
            s.tb_turn_mode = 0;
            s.tb_turn_animating = false;
            s.tb_turn_checked = false;
        }
        // --game-clock N: force the map clock and re-composite the building
        // animations, so a head-less render shows a chosen animation frame.
        if (g.game_clock_override >= 0) {
            g.scene.game_clock = g.game_clock_override;
            g.scene.mv_refresh_draw();
        }
        if (g.mouse_set) {
            // `--mouse` also acts as a map hover: park the pointer and retarget
            // the selected army's trail there before the render, exactly what the
            // interactive WM_MOUSEMOVE path does. The pointer's own state is
            // resolved straight away too (the engine does it on the next map
            // frame, which the render would otherwise have to wait a tick for).
            g.scene.mouse_x = g.mouse_x;
            g.scene.mouse_y = g.mouse_y;
            g.scene.game_resolve_hover();
            g.scene.game_update_move_cursor();
            g.scene.game_update_cursor();
            g.scene.game_update_path();
        }
    }
    const int hover = g.render_hover;
    Canvas full = g.scene.render(hover);
    // --fps N bakes the F1 read-out into the render, so its placement and the
    // shipped face it uses are provable from a PNG.
    if (g.fps.on) draw_fps_overlay(full, g.scene, g.fps.text());
    const Canvas cv = (g.crop_w > 0 && g.crop_h > 0)
                          ? full.crop(g.crop_x, g.crop_y, g.crop_w, g.crop_h)
                          : full;
    std::string err;
    if (!aow::write_png_rgba(g.render_png, cv.width(), cv.height(),
                             cv.to_rgba(), err)) {
        std::fprintf(stderr, "menu: %s\n", err.c_str());
        return 1;
    }
    std::printf("wrote %s  %dx%d  frame=%d  scene_top=%d  orb0_x=%d\n",
                g.render_png.c_str(), cv.width(), cv.height(), g.render_frame,
                g.scene.scene_top(), g.scene.button_left(0));
    if (g.fps.on) {
        std::printf("fps=%s x=%d y=%d\n", g.fps.text().c_str(), kFpsX, kFpsY);
    }
    std::printf("orb_ids=");
    for (int i = 0; i < kButtonCount; ++i) {
        for (int s = 0; s < kOrbStride; ++s) {
            std::printf("%s%u", (i || s) ? "," : "", g.scene.orb_ids[i][s]);
        }
    }
    std::printf("\nfont=%s cursor_id=%u bg_w=%u caption0_w=%d caption0_h=%d\n",
                kFontIlb, g.scene.cursor_id, g.scene.background.w,
                g.scene.text_width(g.scene.captions[0]),
                g.scene.glyphs[static_cast<unsigned char>(
                    g.scene.captions[0].empty() ? '!' : g.scene.captions[0][0])]
                    .face.h);
    std::printf("cursor: ok=%d %ux%u has_key=%d key=(%u,%u,%u) px0=(%u,%u,%u,%u)\n",
                g.scene.cursor.ok, g.scene.cursor.w, g.scene.cursor.h,
                g.scene.cursor.has_key, g.scene.cursor.key[0],
                g.scene.cursor.key[1], g.scene.cursor.key[2],
                g.scene.cursor.rgba.empty() ? 0u : g.scene.cursor.rgba[0],
                g.scene.cursor.rgba.empty() ? 0u : g.scene.cursor.rgba[1],
                g.scene.cursor.rgba.empty() ? 0u : g.scene.cursor.rgba[2],
                g.scene.cursor.rgba.empty() ? 0u : g.scene.cursor.rgba[3]);
    if (g.screen_gtype) {
        std::printf("screen=gtype gt_frame=%d gt_top=%d gt_bottom=%d "
                    "bg=%ux%u panel=%ux%u bottom=%ux%u btn=%ux%u\n",
                    g.scene.gt_frame, g.scene.gt_top_y(), g.scene.gt_bottom_y(),
                    g.scene.gt_bg.w, g.scene.gt_bg.h, g.scene.gt_top.w,
                    g.scene.gt_top.h, g.scene.gt_bottom.w,
                    g.scene.gt_bottom.h, g.scene.gt_btn[0].w,
                    g.scene.gt_btn[0].h);
    }
    if (g.screen_scenario) {
        std::printf("screen=scenario glow=%d/%d maps=%d map=%s size=%s "
                    "towns=%d players=%d bg=%ux%u cell=%ux%u btn=%ux%u "
                    "font10_ok=%d\n",
                    g.scene.scen_glow, kScenGlowFrames,
                    static_cast<int>(g.scene.scen_maps.size()),
                    g.scene.scen_map_name.c_str(), g.scene.scen_map_size.c_str(),
                    g.scene.scen_map_towns, g.scene.scen_map_players,
                    g.scene.scen_bg.w, g.scene.scen_bg.h, kScenCellW,
                    kScenCellH, g.scene.scen_btn[0][0].w,
                    g.scene.scen_btn[0][0].h,
                    g.scene.glyphs10[static_cast<unsigned char>('A')].ok);
        // The exact art the DFMs name (docs/08_SCENARIO_SCREEN.md sec. 6): the
        // STGrFr panel frame + ShadowOv body, the ScenRBut bottom buttons, the
        // FrGoldD/FrThinD list frames with their Text2Lb17/PopUpSet textures, the DialogFr
        // dialog frame over MainFloaterSet, the ScenSlid sliders, the ScenVSB
        // combo arrows and the Age10 dialog face.
        std::printf("scen_art panel=%ux%u/%ux%u scenfr=%ux%u mainbtn=%ux%u "
                    "dlg=%ux%u body=%ux%u list=%ux%u/%ux%u info=%ux%u "
                    "slider=%ux%u vsb=%ux%u edit=%ux%u combo=%ux%u "
                    "font10b_ok=%d\n",
                    g.scene.scen_frame[0].w, g.scene.scen_frame[0].h,
                    g.scene.scen_panel_tex.w, g.scene.scen_panel_tex.h,
                    g.scene.scen_scenfr[0].w, g.scene.scen_scenfr[0].h,
                    g.scene.scen_main_btn[0][1].w, g.scene.scen_main_btn[0][1].h,
                    g.scene.scen_dlg_frame[0].w, g.scene.scen_dlg_frame[0].h,
                    g.scene.scen_body_tex.w, g.scene.scen_body_tex.h,
                    g.scene.scen_list_tex.w, g.scene.scen_list_tex.h,
                    g.scene.scen_list_sel_tex.w, g.scene.scen_list_sel_tex.h,
                    g.scene.scen_info_tex.w, g.scene.scen_info_tex.h,
                    g.scene.scen_slider[1].w, g.scene.scen_slider[1].h,
                    g.scene.scen_vsb[3].w, g.scene.scen_vsb[3].h,
                    g.scene.scen_edit[1].w, g.scene.scen_edit[1].h,
                    g.scene.scen_combo[1].w, g.scene.scen_combo[1].h,
                    g.scene.glyphs10b[static_cast<unsigned char>('A')].ok);
    }
    if (g.scene.screen == 3) {
        const Scene::GameLayout L = g.scene.game_layout();
        std::printf("screen=game map=%s level=%d day=%d disp=%dx%d "
                    "bar=%dx%d scan=%dx%d@%d,%d event=%dx%d@%d,%d "
                    "tab0=%dx%d@%d,%d tab4=%dx%d@%d,%d gold=%dx%d@%d,%d "
                    "turn=%dx%d@%d,%d map_win=%dx%d@%d,%d map_px=%ux%u "
                    "font8_ok=%d fonttb_ok=%d\n",
                    g.scene.game_map_name.c_str(), g.scene.game_level,
                    g.scene.game_day, g.scene.disp_w, g.scene.disp_h,
                    L.bar.w, L.bar.h, L.scan.w, L.scan.h, L.scan.x, L.scan.y,
                    L.events.w, L.events.h, L.events.x, L.events.y,
                    L.tab[0].w, L.tab[0].h, L.tab[0].x, L.tab[0].y,
                    L.tab[4].w, L.tab[4].h, L.tab[4].x, L.tab[4].y,
                    L.gold.w, L.gold.h, L.gold.x, L.gold.y,
                    L.turn.w, L.turn.h, L.turn.x, L.turn.y,
                    L.map_window.w, L.map_window.h, L.map_window.x,
                    L.map_window.y,
                    g.scene.game_pw, g.scene.game_ph,
                    g.scene.glyphs8b[static_cast<unsigned char>('A')].ok,
                    g.scene.glyphsTb[static_cast<unsigned char>('1')].ok);
        // The world-map camera (HSEngine.THSMapViewer): SceneX/SceneY + the
        // viewport, the clamp limits (THSMap.GetMaxXhp/GetMaxYhp) and the mouse
        // state, so the smoke test can pin the camera's arithmetic.
        std::printf("game_cam=%d,%d+%dx%d level=%d map=%ux%u maxhp=%dx%d "
                    "sel=%d,%d hover=%d,%d mouse=%d,%d\n",
                    g.scene.cam_x, g.scene.cam_y, g.scene.cam_w, g.scene.cam_h,
                    g.scene.cam_level, g.scene.map_w, g.scene.map_h,
                    g.scene.cam_max_xhp(), g.scene.cam_max_yhp(),
                    g.scene.game_sel_hx, g.scene.game_sel_hy,
                    g.scene.game_hover_hx, g.scene.game_hover_hy,
                    g.scene.game_mx, g.scene.game_my);
        // The map pointer and the move-selection marker (docs/09_GAME_SCREEN.md
        // 6.8 / 6.9): the state request (`THSMapMouse+0x34`), the image id the
        // engine's UpdateCursor resolved for the current clock (or the icon
        // TSelectedArmy.UpdateMoveCursor painted for state 0x100), whether the
        // hovered hex carries something selectable, whether this player has
        // explored it, and whether the blue hexagon selector is up - so the smoke
        // test can pin all four pointer behaviours and the marker.
        std::printf("game_cursor=%d id=%u clock=%d selectable=%d explored=%d "
                    "selector=%d sel_px=%zu\n",
                    g.scene.game_cursor_state, g.scene.game_cursor_index,
                    g.scene.game_clock,
                    g.scene.game_can_select_here() ? 1 : 0,
                    g.scene.game_field_explored(g.scene.game_hover_hx,
                                                g.scene.game_hover_hy) ? 1 : 0,
                    g.scene.game_hex_selector_shown() ? 1 : 0,
                    g.scene.mv_selector_contents_px);
        // The played player (TSetupControl.SetupMap applied the setup's
        // race -> player-type choice): the roster index whose units the player
        // controls and its race, so the smoke test can pin "you set Elves as the
        // Human, so the Elves are played".
        std::printf("game_human=%d race=%s\n", static_cast<int>(g.scene.mv_human),
                    g.scene.game_race.c_str());
        // The Events banner's leader line (TPInfoW's update FUN_00423170 + its
        // FaceFr OnDraw FUN_00423770): the seat's name, its plural race name,
        // the FACERES.PFS face the leader's index resolves to, and the
        // ShieldsM crest id (race + 13) with the sprite's own size/placement -
        // so the smoke test can pin the map-driven banner.
        {
            const int si = g.scene.game_shield_index;
            const Sprite& crest =
                g.scene.event_shield_race[si >= 0 && si < kEventShieldCount
                                              ? si
                                              : 0];
            std::printf("game_banner=name=%s race=%s face=%s#%d px=%ux%u "
                        "shield=%d crest=%ux%u%+d%+d\n",
                        g.scene.game_leader.c_str(), g.scene.game_race.c_str(),
                        g.scene.game_face_lib.c_str(), g.scene.game_face_index,
                        g.scene.game_face.w, g.scene.game_face.h,
                        si < 0 ? 0 : static_cast<int>(kEventShieldBase) + si,
                        crest.w, crest.h, static_cast<int>(crest.base_x),
                        static_cast<int>(crest.base_y));
        }
        // The world-map selection / trail (docs/15_MOVEMENT.md): the selected
        // army's hex, its move points, and the hovered path - so the smoke test
        // can pin selection and movement, not just the camera.
        if (g.scene.mv_selected) {
            std::printf("game_army=%d,%d mp=%d path=%d reached=%d cost=%d\n",
                        g.scene.mv_sel_x, g.scene.mv_sel_y,
                        g.scene.mv_army.budget,
                        static_cast<int>(g.scene.mv_path.steps.size()),
                        g.scene.mv_path.reached ? 1 : 0, g.scene.mv_path.total_cost);
        } else {
            std::printf("game_army=none\n");
        }
        // The Party window's move read-outs (docs/20_PARTY_WINDOW.md): every
        // pill caption (`U{n}MIcon`, the unit's own remaining points), the
        // party's `MovesLbl` and the folded `TArmy.MovePoints` budget that
        // validates a move - so the smoke test can pin that they show what the
        // travelling units have left, not their maximum allowance. `u{n}=-1` is
        // an empty slot, `game_party=none` a closed window.
        if (g.scene.game_show_party) {
            std::printf("game_party=moves=%d budget=%d sel=%d",
                        g.scene.party_move_points(),
                        static_cast<int>(g.scene.mv_army.budget),
                        static_cast<int>(g.scene.party_sel));
            for (int s = 0; s < kPartySlots; ++s) {
                const aow::MapUnit* u = g.scene.party_unit(s);
                std::printf(" u%d=%d", s + 1,
                            u == nullptr ? -1
                                         : static_cast<int>(
                                               aow::unit_move_points_left(
                                                   g.scene.mv_cat, *u)));
            }
            std::printf("\n");
        } else {
            std::printf("game_party=none\n");
        }
    }
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
        else if (a == "--hover") { next(); g.render_hover = std::atoi(v.c_str()); }
        else if (a == "--cursor-id") {
            next();
            g.scene.cursor_id = static_cast<std::uint32_t>(std::strtoul(v.c_str(), nullptr, 0));
        }
        else if (a == "--mouse") {
            next();
            g.mouse_x = std::atoi(v.c_str());
            if (i + 1 < argc) g.mouse_y = std::atoi(acp_from_w(argv[++i]).c_str());
            g.mouse_set = true;
        }
        else if (a == "--game-click") {
            next();
            g.game_click_x = std::atoi(v.c_str());
            if (i + 1 < argc) {
                g.game_click_y = std::atoi(acp_from_w(argv[++i]).c_str());
            }
            g.game_click_set = true;
            g.game_clicks.push_back(
                GameClick{g.game_click_x, g.game_click_y, false});
        }
        else if (a == "--game-right-click") {
            next();
            const int rx = std::atoi(v.c_str());
            int ry = 0;
            if (i + 1 < argc) ry = std::atoi(acp_from_w(argv[++i]).c_str());
            g.game_clicks.push_back(GameClick{rx, ry, true});
        }
        else if (a == "--game-key") {
            next();
            if (!v.empty()) g.game_key = static_cast<unsigned char>(v[0]);
        }
        else if (a == "--game-bench") { next(); g.game_bench = std::atoi(v.c_str()); }
        else if (a == "--game-anim") { next(); g.game_anim = std::atoi(v.c_str()); }
        else if (a == "--game-clock") {
            next();
            g.game_clock_override = std::atoi(v.c_str());
        }
        else if (a == "--turn-frame") {
            next();
            g.turn_frame_override = std::atoi(v.c_str());
        }
        else if (a == "--structure-panel") { g.structure_panel = true; }
        else if (a == "--structure-name") { next(); g.structure_name = v; }
        else if (a == "--structure-race") { next(); g.structure_race = v; }
        else if (a == "--structure-race-index") { next(); g.structure_race_index = std::atoi(v.c_str()); }
        else if (a == "--structure-status") { next(); g.structure_status = v; }
        else if (a == "--structure-gold") { next(); g.structure_gold = std::atoi(v.c_str()); }
        else if (a == "--structure-mana") { next(); g.structure_mana = std::atoi(v.c_str()); }
        else if (a == "--next-held") { g.next_held = true; }
        else if (a == "--font-variant") {
            next();
            g.scene.font_variant = std::atoi(v.c_str());
        }
        else if (a == "--no-cursor") { g.scene.cursor_on = false; }
        else if (a == "--crop") {
            next();
            g.crop_x = std::atoi(v.c_str());
            g.crop_y = std::atoi(v.c_str());
            g.crop_w = std::atoi(v.c_str());
            g.crop_h = std::atoi(v.c_str());
            if (i + 1 < argc) g.crop_y = std::atoi(acp_from_w(argv[++i]).c_str());
            if (i + 1 < argc) g.crop_w = std::atoi(acp_from_w(argv[++i]).c_str());
            if (i + 1 < argc) g.crop_h = std::atoi(acp_from_w(argv[++i]).c_str());
        }
        else if (a == "--frames") { next(); g.frames_limit = std::atoi(v.c_str()); }
        else if (a == "--fps") {
            // head-less: bake the F1 frame-rate read-out into the render with
            // this value (the window's own counter is driven by WM_PAINT).
            next();
            g.fps.on = true;
            g.fps.value = std::atof(v.c_str());
        }
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
        else if (a == "--screen") {
            next();
            g.screen_gtype = (v == "gtype" || v == "multi" ||
                              v == "multiwindow");
            g.screen_scenario = (v == "scenario" || v == "scensetup" ||
                                 v == "setup");
            g.screen_game = (v == "game" || v == "worldmap" || v == "ingame");
        }
        else if (a == "--map") { next(); g.game_map_arg = v; }
        else if (a == "--start-map") { next(); g.start_map = v; }
        else if (a == "--gtype-frame" || a == "--scen-frame") {
            next();
            g.render_frame = std::atoi(v.c_str());
        }
        else if (a == "--scen-maps") { g.scen_maps = true; }
        else if (a == "--scen-pick") { next(); g.scen_pick = v; }
        else if (a == "--scen-dd") { next(); g.scen_dd = std::atoi(v.c_str()); }
        else if (a == "--scen-human") { next(); g.scen_human = std::atoi(v.c_str()); }
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

    // Resolve the in-game screen's map: --map, else Tutorial.hsm, else the
    // first Scenario\Custom map.
    if (g.screen_game) {
        std::string m = g.game_map_arg;
        std::error_code ec;
        if (m.empty()) {
            const fs::path dir = fs::path(root) / "Scenario" / "Custom";
            std::vector<std::string> cands;
            for (fs::directory_iterator it(dir, ec), end; it != end;
                 it.increment(ec)) {
                if (!it->is_regular_file()) continue;
                std::string ext = it->path().extension().string();
                for (char& c : ext) {
                    c = static_cast<char>(std::tolower(
                        static_cast<unsigned char>(c)));
                }
                if (ext == ".hsm") cands.push_back(it->path().string());
            }
            std::sort(cands.begin(), cands.end());
            for (const std::string& c : cands) {
                if (fs::path(c).filename().string() == "Tutorial.hsm") {
                    m = c;
                    break;
                }
            }
            if (m.empty() && !cands.empty()) m = cands.front();
        } else if (!fs::path(m).is_absolute()) {
            const fs::path p1 = fs::path(root) / m;
            const fs::path p2 = fs::path(root) / "Scenario" / "Custom" / m;
            if (fs::exists(p1)) m = p1.string();
            else if (fs::exists(p2)) m = p2.string();
        }
        g.game_map_arg = m;
    }

    std::string err;
    if (!g.scene.load(root, &err)) {
        std::fprintf(stderr, "menu: %s\n", err.c_str());
        MessageBoxW(nullptr,
                    L"Could not load the title-screen assets.\n"
                    L"Run AoW from the game folder, or pass --root <dir>.",
                    kAppTitle, MB_ICONERROR);
        return 1;
    }
    if (g.headless) return run_headless();
    if (g.screen_gtype) g.scene.screen = 1;
    if (g.screen_game) {
        g.scene.screen = 3;
        std::string gerr;
        if (!g.scene.load_game_map(g.game_map_arg, &gerr)) {
            std::fprintf(stderr, "menu: %s\n", gerr.c_str());
        }
    }
    if (g.screen_scenario) {
        g.scene.screen = 2;
        g.scene.scen_glow = kScenGlowFrames;  // a picked map lights instantly
        g.scene.select_map(g.scene.scen_map_name);
    }

    // The music player starts on the title theme (Songs\TITLE.IT). Starting a
    // map swaps in that map's own playlist (Scene::start_game -> the map's
    // tag-0x1d/0x3c song list); returning to the menus swaps it back.
    g.title_playlist = {(fs::path(root) / "Songs" / "TITLE.IT").string()};
    if (g.scene.screen == 3 && !g.scene.game_songs.empty()) {
        set_music_playlist(g.scene.game_songs);
    } else {
        set_music_playlist(g.title_playlist);
    }
    if (g.music) {
        g.audio_thread = std::thread(play_music_thread);
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // Show the game's own icon in the title bar / taskbar (resource id 1) -
    // without this Windows falls back to the generic application icon.
    wc.hIcon = static_cast<HICON>(LoadImageW(
        hInst, MAKEINTRESOURCEW(kIconResId), IMAGE_ICON, 0, 0,
        LR_DEFAULTSIZE | LR_SHARED));
    wc.hIconSm = static_cast<HICON>(LoadImageW(
        hInst, MAKEINTRESOURCEW(kIconResId), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
        LR_SHARED));
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

    // The frame clock: wait on the waitable timer (33 ms = 30 fps) and on the
    // message queue at once. A signalled timer is one frame tick; input is
    // dispatched as before. Without a waitable timer (fallback) there is nothing
    // to wait on but the queue, and WM_TIMER drives the ticks.
    MSG msg;
    bool quit = false;
    while (!quit) {
        const DWORD wait = MsgWaitForMultipleObjectsEx(
            g.frame_timer ? 1u : 0u, g.frame_timer ? &g.frame_timer : nullptr,
            INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (g.frame_timer && wait == WAIT_OBJECT_0) {
            on_frame_tick(g.hwnd);
            continue;
        }
        if (wait != WAIT_OBJECT_0 + (g.frame_timer ? 1u : 0u)) {
            break;  // WAIT_FAILED / an abandoned wait: do not spin
        }
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    stop_frame_clock(g.hwnd);

    g.audio_stop.store(true);
    if (g.audio_thread.joinable()) g.audio_thread.join();
    return 0;
}

