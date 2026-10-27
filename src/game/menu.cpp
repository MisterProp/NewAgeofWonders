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
#include "aow/structure_capture.hpp"
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
// The selected structure's blink (`TStructure.Select` -> its body sprite fading
// against the world). `bin/Screen Selection.gif` is the ground truth: 13 map
// frames at 30 fps - frames 1-5 and 8-13 are fully opaque, and only frames 6-7
// (offsets 5 and 6) drop the building to 50%. The owner flag, the city wall and
// the armies on the hex are separate scene objects and stay opaque. The cycle is
// phased on the map clock at the moment the Structure floater opens, so the first
// five frames after a click are always opaque - exactly the order of the reference
// capture.
constexpr int kStructurePulsePeriod = 13;   // map frames (30 fps -> ~0.43 s)
constexpr int kStructurePulseFirst = 5;     // first dimmed frame (0-based)
constexpr int kStructurePulseFrames = 2;    // frames 6 and 7 of the capture
constexpr int kStructurePulseAlpha = 128;   // 50% body opacity
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
// TIWindow's city panel (docs/25_STRUCTURE_PANEL.md). The BCS_<race>.BMP city
// banner is the RACES library id 42 (57x278; ids 40/41 are its top strip and
// upper frame, 45 is the underground BCU banner). The RaceRelationIcon is
// RelWin's 51x36 face, id 11 onwards, and the city-level icon beside the income
// is LBIcon 1 (the gold "upgrade" tree - LBIcon 33 is the unrelated mana
// diamond).
constexpr std::uint32_t kStructureCityArtId = 42;
constexpr std::uint32_t kStructureRelationBase = 11;
constexpr std::uint32_t kStructureRelationCount = 8;
// The reference capture's city is owned by a player at relation 4 (the blue
// laughing face, RelFr 15). The port has no diplomacy model yet, so the city
// panel uses this band unless `--structure-relation` overrides it.
constexpr int kStructureRelationDefault = 4;
constexpr std::uint32_t kThinFrameBase = 105;   // FrThinD 2px 9-slice
constexpr std::uint32_t kCeltBtnBase = 19;      // ButRCelt2 (state*3 + piece)
// The dialog's close box is TAOWButton Close: ILIcon = IntGfxMod.GenericI
// (Int\GenericI.ILB) at ILIndexUp/Down/Lit 0/1/2 (15x16 MainHeaderBut.BMP).
constexpr std::uint32_t kScenCloseArt[3] = {0, 1, 2};
constexpr int kScenGridX = 10, kScenGridY = 39;  // SidesPnl WinLeft/WinTop
constexpr int kScenCellW = 156, kScenCellH = 66;
constexpr int kScenColDx = 164, kScenRowDy = 70;
constexpr int kScenSetX = 500, kScenSetY = 40, kScenSetW = 134, kScenSetH = 430;
// --- SettingsPnl's own children (assets/dfm/AoW/TSCENSETUP.dfm, all panel
// relative). The panel is the TAOWPanel at (500,40) 134x430; every control
// below is a real component of the form with its declared geometry:
//
//   PlayModeCB  (10,25) 114 wide  TAOWComboBox, Age8, NumberVisible 2,
//                                ItemHeight 13, items = TObject(1) "Classic" /
//                                TObject(2) "Simultaneous" (hSetPlayMode takes
//                                that value: 1 = classic, 2 = simultaneous)
//   TurnTimeSl  (15,87)  50x11    TAOWHScrollBar Min 0 Max 20 (minutes)
//   TurnsSl     (15,116) 50x11    TAOWHScrollBar Min 0 Max 40 (x10 turns)
//   TurnTimeVal (70,87)  TurnsVal (70,117)  Age8s clGray value labels
//   the four TAOWCheckBox 12x12 faces at (15,162/177/192/207) + Age8s labels
//   at (30,164/179/194/209).
constexpr int kSetModeX = 10, kSetModeY = 25, kSetModeW = 114;
constexpr int kSetModeItemH = 13;    // PlayModeCB.ItemHeight
constexpr int kSetModeRows = 2;      // PlayModeCB.NumberVisible
constexpr int kSetSlX = 15, kSetSlW = 50, kSetSlH = 11;
constexpr int kSetTurnTimeSlY = 87, kSetTurnsSlY = 116;
constexpr int kSetTurnTimeMax = 20;  // TurnTimeSl.Max: minutes 0..20
constexpr int kSetTurnsMax = 40;     // TurnsSl.Max: 0 == no limit, else x10
constexpr int kSetValX = 70, kSetTurnTimeValY = 87, kSetTurnsValY = 117;
constexpr int kSetChkX = 15, kSetChkSize = 12;
constexpr int kSetChkY[4] = {162, 177, 192, 207};   // the four checkboxes
constexpr int kSetChkLblX = 30;
constexpr int kSetChkLblY[4] = {164, 179, 194, 209};  // their labels
// The settings handlers themselves (published-method table entries): every
// checkbox calls TSetupControl.hSetX(Checked) and then FUN_00455dfc 1 (on, i.e.
// Sfx\Int\Btn5.wav) or 2 (off, Sfx\Int\Btn6.wav) - AoW.exe 0x00412b04..
// 0x00412bd0. The two sliders are 0x0041285c (TurnTimeSlChange) and
// 0x004129a8 (TurnsSlChange); the combo is 0x004129e8 (PlayModeCBChange).
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
constexpr std::uint32_t kNgcBoxLit = 49;   // NGCBox's ILIndexLit (hover)
// PlayModeCB's list textures (TSCENSETUP.dfm): ILTextureNormal GenericT 4
// (blank1, kBlankTexId) and ILTextureSelected GenericT 16 (TextLBSel.BMP).
constexpr std::uint32_t kLbSelTexId = 16;
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
// The single-player day handler (AoW.exe 0x0044ece8) stores LastPlayer = -1
// when the day rolls over. Update then starts at Frame = 7*(-1) = -7, i.e.
// library frame 11-7 = 4, and walks: the 6-frame glow tail (4..9), the 11-frame
// glow block (0..10) and the 6-frame landing on the seat's block (5..0).
// That is 23 window Updates at the game's 30 fps (~0.77 s) - one rotation.
constexpr int kTbTurnDayAnimFrames = 23;
// The port has no strategic AI, so an AI seat ends instantly where the original
// spends its turn. Hold each seat for the button's own change span while the
// button keeps showing its Checked glow, so the "turn ended" still is visible
// until every seat ended and the one rotation at the new day begins.
constexpr int kGameAiTurnHoldFrames = kTbTurnDayAnimFrames;
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
// A net loss uses Delphi's clRed (TColor 0x0000FF -> RGB 255,0,0) so the
// income box reads as a warning, matching the engine's "only costs" state.
constexpr std::uint8_t kTbCostR = 255, kTbCostG = 0, kTbCostB = 0;

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

// ------------------------------------------------- the Unit info window
// `TUnitWindow` (`AoW.exe`, DFM `assets/dfm/AoW/TUNITWINDOW.dfm`) - the modal
// floater the Party window opens when a unit's portrait is clicked (the party
// frame's `Unit1FrameMouseDown@0x00406040` files the clicked unit into the
// world-map selection and calls `FUN_00407db8`, the window's whole refresh).
// The window is the DFM's 420x417 `UnitWin` at the display's centre
// (awCenter/ahCenter), `Masked = True`, `Priority = 300`, with the Title/tabs/
// body/stat panel/right frame and the Disband/Close footer.
//
// Every handler was recovered from the form's published-method table in
// `AoW.exe.orig` (pooled with the DFM strings around 0x00406b00); it uses the
// same `<u16><u32 addr><u8 len><name>` records as TControlWin:
//
//   UnitWindowCreate 0x00409c60   UnitWinUpdate   0x00409c68
//   UnitWinShow      0x0040a10c   UnitTabChange   0x0040a0b4
//   UnitWinMouseDown 0x0040a0c4   UnitWinKeyDown  0x0040abd4
//   CloseClick       0x0040a144   DisbandBtnClick 0x0040aaa8
//   FrLShPreDraw     0x0040a214   FrRShPreDraw    0x0040a2a0
//   PreDrawAbilityItem 0x00407ba8 T1AbilityLBMouseDown 0x0040ab2c
//
// and the refresh writes the component fields the component table at
// 0x00406430 publishes (real offset = published - 4):
//   0x170 UnitName, 0x174 UnitFrRight, 0x178/0x17c/0x180/0x184/0x188/0x18c
//   stat icons, 0x190..0x1a4 stat labels, 0x1b8 Level, 0x1cc RightFramePnl,
//   0x1d0 MoraleLbl, 0x1d4/0x1d8 upkeep, 0x1ec RelationIcn, 0x1f0 RelationV,
//   0xe0/0xe4 TypeIcn/TypeV, 0xd0/0xd4 AAlignment/AAlignmentLbl,
//   0x20c XP, 0x210/0x214 PNext/PPrev, 0x74/0x94/0x98 ability lists.
//
// The data behind each read-out (FUN_00407db8):
//   Level  = "Level " + GetLevel()                 (resourcestring 65507)
//   Exp    = "Exp (" + GetExperience() + "/" + GetNextLevelExperience() + ")"
//   Type   = Unit / Creature / Machine / Hero / Leader   (65533/65531/65532/
//            65535/65534), icon UnitWin 21/22/23
//   Align  = TAlignmentRStr[GetAlignment & 0x7f], icon RelWin 31..35
//   Rel    = TStatusRStr[GetPlayerRelation], icon RelWin 11..14
//   Morale = "Morale: " + TMoraleRStr[GetMorale] (Terrible..High)
//   Stats  = GetAttack/Defense/Damage/Resistance and Hits/HP, Moves/MovePoints
//   Abilities = TAbstractUnit.ListAbilityInfo@0x5577fa50 -> name/type/source,
//               sorted by name.
constexpr const char* kUnitWinIlb = "Int\\UnitWin.ILB";  // IntGfxMod.UnitWin
constexpr const char* kRelWinIlb = "Int\\Scenes\\RelFr.ILB";  // IntGfxMod.RelWin
constexpr int kUnitWinW = 420;   // UnitWin.WinWidth  (design)
constexpr int kUnitWinH = 417;   // UnitWin.WinHeight (design)
// UnitWin's own GenericF frame is DialogFr.BMP (ids 32..39).
constexpr std::uint32_t kUnitWinFrameBase = 32;
// UnitFrameLeft / UnitFrRight / FrLSh / FrRSh in Int\UnitWin.ILB.
constexpr std::uint32_t kUnitWinLeftFrame = 44;
constexpr std::uint32_t kUnitWinRightFrame = 45;
constexpr std::uint32_t kUnitWinShade = 46;
// TypeIcn choices: DFM default 21; the refresh uses 21 Creature / 22 Machine /
// 23 Unit-Hero-Leader.
constexpr std::uint32_t kUnitWinTypeCreature = 21;
constexpr std::uint32_t kUnitWinTypeMachine = 22;
constexpr std::uint32_t kUnitWinTypeUnit = 23;
// AAlignment (RelWin 31..35) and RelationIcn (RelWin 11..18; the four status
// faces are 11..14 - Hostile / Wary / Polite / Friendly).
constexpr std::uint32_t kUnitWinAlignBase = 31;
constexpr std::uint32_t kUnitWinRelationBase = 11;
// UnitTab's TAOWTabPanel skin: CeltDeco 67/70, three pieces per state.
constexpr std::uint32_t kUnitTabDown = 67;
constexpr std::uint32_t kUnitTabUp = 70;
// The three column headers are GenericB 85/91/88 (up/lit/down).
constexpr std::uint32_t kUnitHeaderUp = 85;
constexpr std::uint32_t kUnitHeaderLit = 91;
constexpr std::uint32_t kUnitHeaderDown = 88;
// The foot buttons are GenericB 19/25/22 (CeltDeco button skin).
constexpr std::uint32_t kUnitButtonUp = 19;
constexpr std::uint32_t kUnitButtonLit = 25;
constexpr std::uint32_t kUnitButtonDown = 22;
// TAlignmentRStr - index is `GetAlignment & 0x7f`. Values from AoWEPACK's
// RT_STRING: 65295 Pure Good, 65264 Good, 65265 Pure Neutral, 65268 Neutral,
// 65266 Evil, 65267 Pure Evil; 6 is the "None" state (hidden).
constexpr const char* kUnitAlignmentText[7] = {
    "Pure Good", "Good", "Pure Neutral", "Neutral",
    "Evil", "Pure Evil", "None"};
// TMoraleRStr (65269 High, 65270 Terrible, 65271 Poor, 65272 Okay): the array
// is indexed by `MoraleValueToMorale` (0 = worst .. 4 = best).
constexpr const char* kUnitMoraleText[5] = {"Terrible", "Poor", "Okay", "Good",
                                            "High"};
// TStatusRStr - the four race-relation captions.
constexpr const char* kUnitRelationText[4] = {"Hostile", "Wary", "Polite",
                                              "Friendly"};
// The ability type captions (TAbilityTypeRStr: 65317 Movement, 65318 Combat,
// 65319 Enchantment, 65320 Secret).
constexpr const char* kAbilityTypeMovement = "Movement";
constexpr const char* kAbilityTypeCombat = "Combat";
constexpr const char* kAbilityTypeMagic = "Magic";
constexpr const char* kAbilityTypeSecret = "Secret";
constexpr const char* kAbilitySourceInherent = "Inherent";

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
    std::uint32_t w = 0, h = 0;   // visual (RLE-cropped) size
    // Descriptor slot/cell size. The engine lays a 9-slice out by these, then
    // places the cropped visual inside the cell with base+shift; e.g. DialogFr
    // 32 is a 22x22 slot whose 18x22 art sits at x+4, and the left edge 35 is
    // a 22x184 slot holding a 9x184 strip at x+13.
    std::uint32_t slot_w = 0, slot_h = 0;
    std::vector<std::uint8_t> rgba;
    // The engine blits a sprite so that its visual box lands at
    // (x + base_x + shift_x, y + base_y + shift_y); see aow/ilb.hpp.
    std::int32_t base_x = 0, base_y = 0;
    std::int32_t shift_x = 0, shift_y = 0;
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

// Reads the port's own city-screen backdrop (`tools/city_screen_bg.py` writes
// an uncompressed 24-bit BMP): the 640x432 `TCityScreen.Dlg` skin the reference
// capture shows.  The port has no general image reader (all game art arrives
// through `aow::IlbFile`), so this deliberately handles only the single BMP
// shape the tool emits.
bool load_bmp24(const std::string& path, Sprite& out) {
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (f == nullptr) return false;
    std::uint8_t hdr[54];
    const bool header_ok = std::fread(hdr, 1, sizeof hdr, f) == sizeof hdr &&
                           hdr[0] == 'B' && hdr[1] == 'M';
    std::uint32_t off = 0, w = 0, h = 0, bpp = 0, comp = 0;
    if (header_ok) {
        auto u32 = [&](int p) {
            return static_cast<std::uint32_t>(hdr[p]) |
                   (static_cast<std::uint32_t>(hdr[p + 1]) << 8) |
                   (static_cast<std::uint32_t>(hdr[p + 2]) << 16) |
                   (static_cast<std::uint32_t>(hdr[p + 3]) << 24);
        };
        off = u32(10);
        w = u32(18);
        h = u32(22);
        bpp = static_cast<std::uint32_t>(hdr[28]) |
              (static_cast<std::uint32_t>(hdr[29]) << 8);
        comp = u32(30);
    }
    if (!header_ok || off < 54 || w == 0 || h == 0 || bpp != 24 || comp != 0 ||
        w > 4096 || h > 4096) {
        std::fclose(f);
        return false;
    }
    const std::uint32_t row = (w * 3 + 3) & ~3u;
    std::vector<std::uint8_t> raw(static_cast<std::size_t>(row) * h);
    const bool data_ok = std::fseek(f, static_cast<long>(off), SEEK_SET) == 0 &&
                         std::fread(raw.data(), 1, raw.size(), f) == raw.size();
    std::fclose(f);
    if (!data_ok) return false;
    out.w = w;
    out.h = h;
    out.rgba.assign(static_cast<std::size_t>(w) * h * 4, 255);
    for (std::uint32_t y = 0; y < h; ++y) {
        const std::uint8_t* src = &raw[static_cast<std::size_t>(h - 1 - y) * row];
        std::uint8_t* dst = &out.rgba[static_cast<std::size_t>(y) * w * 4];
        for (std::uint32_t x = 0; x < w; ++x) {
            dst[x * 4 + 0] = src[x * 3 + 2];  // BMP is B,G,R
            dst[x * 4 + 1] = src[x * 3 + 1];
            dst[x * 4 + 2] = src[x * 3 + 0];
        }
    }
    out.ok = true;
    return true;
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
               std::uint8_t b, bool key_black = false) {
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
                const bool bg = (p[0] | p[1] | p[2]) == 0;
                // Age8s (the city Race/Status labels) is a thin 1 px face whose
                // black pixels are the font's colour key. Age8/Age10/Age10s
                // carry a real black shadow, so those pixels stay black - which
                // is what gives the white "38"/"2" and "Venice" runs their dark
                // lower-right edge in the capture (glyph() used to recolour the
                // whole cell, erasing that shadow).
                if (bg && key_black) continue;
                const int dx = dx0 + static_cast<int>(sx);
                if (dx < 0 || dx >= w_) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                d[0] = bg ? 0 : b;
                d[1] = bg ? 0 : g;
                d[2] = bg ? 0 : r;
                d[3] = 255;
            }
        }
    }

    // The engine renders into a 16-bit RGB555 back surface (every reference
    // capture - StructurePReference.png, partyRef.png, CounterRef.png, ... - is
    // 100 % RGB555-expanded: all channel values are in the 32-step set). The
    // port's canvases are 8-bit, so screenshots are quantised here, exactly as
    // GFXE's 16-bit surface does. The live window keeps the 8-bit buffer.
    static std::uint8_t rgb555(std::uint8_t v) {
        const unsigned q = v >> 3;
        return static_cast<std::uint8_t>((q << 3) | (q >> 2));
    }

    std::vector<std::uint8_t> to_rgba(bool rgb555_surface = false) const {
        std::vector<std::uint8_t> out(px_.size());
        for (std::size_t i = 0; i < px_.size(); i += 4) {
            out[i] = rgb555_surface ? rgb555(px_[i + 2]) : px_[i + 2];
            out[i + 1] = rgb555_surface ? rgb555(px_[i + 1]) : px_[i + 1];
            out[i + 2] = rgb555_surface ? rgb555(px_[i]) : px_[i];
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

    // TShadow / ShadowBlendTable blit: every non-transparent source pixel
    // darkens the destination to 75 % (`GFXE.GenerateShadowBlendTable` with
    // full strength, the same 0.75 factor as the 16-bit TShadow routine:
    // `(dst >> 1 & 0x7bef) + ((dst & 0xe79c) >> 2)`). UnitWin image 46 is the
    // FrLSh/FrRSh frame shadow: a dithered light mask whose transparent centre
    // leaves the face alone and whose ring is the portrait's vignette.
    void blit_shadow(const Sprite& s, int x, int y) {
        if (!s.ok) return;
        const int dx0 = x + s.base_x + s.shift_x;
        const int dy0 = y + s.base_y + s.shift_y;
        for (std::uint32_t sy = 0; sy < s.h; ++sy) {
            const int dy = dy0 + static_cast<int>(sy);
            if (dy < 0 || dy >= h_) continue;
            for (std::uint32_t sx = 0; sx < s.w; ++sx) {
                const int dx = dx0 + static_cast<int>(sx);
                if (dx < 0 || dx >= w_) continue;
                const std::uint8_t* p =
                    &s.rgba[(static_cast<std::size_t>(sy) * s.w + sx) * 4];
                if (p[3] == 0) continue;
                std::uint8_t* d =
                    &px_[(static_cast<std::size_t>(dy) * w_ + dx) * 4];
                // ShadowBlendTable[s][dst] = round(dst * s / 31): the mask's
                // intensity (a light-grey dither, 222..255) scales the
                // destination, so the vignette is a 0..13 % darkening.
                const int f = p[2];   // BGRA: red is the grey level
                d[0] = static_cast<std::uint8_t>(d[0] * f / 255);
                d[1] = static_cast<std::uint8_t>(d[1] * f / 255);
                d[2] = static_cast<std::uint8_t>(d[2] * f / 255);
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

// --- TCityScreen's unit detail page (`FUN_0043c43c` cases 7/8) -------------
// Selecting one of the twelve P1CB..P12CB cards shows `UnitPnl` instead of the
// activity page (`FUN_0043c43c` hides UpgradePnl/MigratePnl and shows UnitPnl).
// The page is drawn live from the same DFM controls the original uses:
//
//   control          game rect       frame / body
//   UnitPnl          23,227 594x130  (fills the page above the queue band)
//   UnitLbl          (centred 320, 233)
//   UPicturePnl      33,237 90x110   GenericF 105 over GenericT 18
//   UPictureFramePnl 35,238 87x107   GenericF 113 over GenericT 19
//   UFace            37,240 83x103   TAbstractUnit.ShowFace (stretched)
//   UStatPnl        143,256 166x91   GenericF 105 over GenericT 17
//   UStatFr1        145,257 81x57    GenericF 113 (left stat column)
//   UStatFr2        226,257 82x57    GenericF 113 (right stat column)
//   UInfoFr         145,314 163x31   GenericF 113 (alignment / level)
//   AbilPnl         331,256 166x91   GenericF 105 over GenericT 17
//   UAbilitiesLB    333,257 163x88   GenericF 113, ItemHeight 16
//   UCostPnl        517,237 90x110   GenericF 105 over GenericT 17
//   CostFr          519,238 87x54    GenericF 113
//   TurnsFr         519,292 87x53    GenericF 113
//
// These are the DFM's ReAlignComp results with ActivityPnl at (11,162): the
// shipped 640x480 screen draws the mode-1 band two pixels above the DFM's
// WinTop (the page texture's phase, measured on assets/cityprod1.bmp, is
// (11,162)). The frame lines of bin/UnitsSelectionRef.png land exactly on the
// rects above (game = ref + (3,-1), the capture's crop origin), which is the
// benchmark this page is matched against.
constexpr int kUnitPageX = 23, kUnitPageY = 227;
constexpr int kUnitPageW = 594, kUnitPageH = 130;
// TImageLibraryFont.SetColor uses the DFM's TextColor 9415072 = 0x8FA9A0
// (TColor is BGR): every Age8/Age10s caption on the page is this warm grey.
constexpr std::uint8_t kUnitTextR = 160;
constexpr std::uint8_t kUnitTextG = 169;
constexpr std::uint8_t kUnitTextB = 143;
// UAbilitiesLB: text starts at BorderWidth 10 + the 3 px FrGoldD frame, rows
// are ItemHeight 16 (BorderHeight 4 + the frame).
constexpr int kUnitAbilX = 346, kUnitAbilY = 264, kUnitAbilDy = 16;
// UAbilitiesLB is a TAOWListBox with VScrollBar T2SBar: 88 px tall, rows of 16,
// so five rows fit and the bar (VanishWhenFull) appears for longer lists. The
// bar is the DFM's (AbilPnl + 148,1) 17x88 rect and names GenericS 86..93
// (NGVsb2.BMP: dec up/down, inc up/down, thumb top/middle/bottom, track).
constexpr int kUnitAbilRows = 5;
constexpr int kUnitAbilSbX = 481, kUnitAbilSbY = 258;
constexpr int kUnitAbilSbW = 17, kUnitAbilSbH = 88;
// The six TAOWImage cells, in control order: attack, defense, moves, damage,
// resistance, hits. UAttackIcon/UDefenseIcon are at UStatPnl+(12,3)/(12,20),
// UMovesIcon at (8,37), UDamageIcon/UResistanceIcon/UHitsIcon at
// (91,3)/(91,20)/(91,37). These are the DFM's ReAlignComp positions; the
// sprite's own class shift (`base`) then lands its visual box exactly where
// bin/UnitsSelectionRef.png draws it.
constexpr int kUnitIconX[6] = {155, 155, 151, 234, 234, 234};
constexpr int kUnitIconY[6] = {259, 276, 293, 259, 276, 293};
// Labels are awCenter in UStatP1 (42 px at +32) / UStatP2 (36 px at +120).
constexpr int kUnitStatLblX[2] = {196, 281};
constexpr int kUnitStatLblY[3] = {264, 281, 298};

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
    Sprite scen_ngcbox[3];           // GenericI 47/48/49 NGCBox (off/on/lit)
    Sprite scen_lbsel_tex;           // GenericT 16 TextLBSel (combo list line)
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
    int scen_chk_hover = -1;  // the checkbox under the pointer (lit face 49)
    int scen_chk_press = -1;  // the checkbox held down right now
    // --- SettingsPnl's live state (TScenSetup + TSetupSettings) -------------
    // TSetupSettings+0x24 holds the play mode hSetPlayMode writes (1 = classic,
    // 2 = simultaneous); the scenario screen opens in simultaneous turns, which
    // is what the shipped capture's PlayModeCB reads.
    int scen_play_mode = 2;
    bool scen_mode_dd = false;   // PlayModeCB's list is dropped down
    int scen_mode_dd_hover = -1; // the hovered row (0 Classic / 1 Simultaneous)
    // TSetupSettings+0x34 / +0x30: TurnTimeSl.Position (minutes) and TurnsSl's
    // Position (the turn limit is x10, so 40 -> 400 turns).
    int scen_turn_time = 0;
    int scen_turns = 0;
    int scen_drag_slider = -1;   // 0 = TurnTimeSl, 1 = TurnsSl (thumb drag)
    int scen_drag_grab = 0;      // mouse offset inside the thumb
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
    int tb_turn_cur = kTbTurnNoRace;   // +0x118 CurPlayer - the SEATED seat's race
    int tb_turn_last = kTbTurnNoRace;  // +0x11c LastPlayer; -1 forces one rotation
    int tb_turn_mode = 0;              // +0x124 the animation state (0..3)
    int tb_turn_start = kTbTurnAnimStart;  // +0x128 AnimStart
    bool tb_turn_animating = false;    // +0x120 Animating
    bool tb_turn_checked = false;      // +0x13c Checked ("turn ended")
    int tb_turn_frame = 0;             // +0x140 Frame
    int tb_turn_ai_hold = 0;           // port-only pacing for the un-simulated AI
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
    // --- the Unit info window (TUnitWindow) ---------------------------------
    // `IntGfxMod.UnitWin` = Int\UnitWin.ILB: the two 118x143 portrait frames
    // (44 left face frame, 45 right unit frame), the transparent shade (46) and
    // the TypeIcn art (21/22/23).
    aow::IlbFile unitwin_il;
    Sprite unitwin_frame[8];       // GenericF 32..39 DialogFr
    Sprite unitwin_body;           // GenericT 2  UnitWin body texture
    Sprite unitwin_tex17;          // GenericT 17 TextOld1 (T2 cards / T2Text)
    Sprite unitwin_tex18;          // GenericT 18 TextOld2 (T2Memo body)
    Sprite unitwin_tex7;           // GenericT 7  RightFramePnl body
    Sprite unitwin_tex21;          // GenericT 21 the ability list body
    Sprite unitwin_gold_frame[8];  // GenericF 113..120 FrGoldD
    Sprite unitwin_thin_frame[8];  // GenericF 105..112 FrThinD
    Sprite unitwin_left_frame;     // UnitWin 44
    Sprite unitwin_right_frame;    // UnitWin 45
    Sprite unitwin_shade;          // UnitWin 46
    Sprite unitwin_type_icon[3];   // UnitWin 21/22/23
    Sprite unitwin_align_icon[5];  // RelWin 31..35
    Sprite unitwin_relation[8];    // RelWin 11..18
    Sprite unitwin_move_icon;      // IconMove 0 (UMovesIcon, 32x17)
    Sprite unitwin_stat_icon[5];   // Icons 0..4 (attack/defense/damage/res/hits)
    Sprite unitwin_upkeep_gold;    // TBWin 51
    Sprite unitwin_upkeep_mana;    // TBWin 52
    Sprite unitwin_title_icon;     // WinTIcon 3
    Sprite unitwin_close[3];       // GenericI 0/2/1
    Sprite unitwin_tab[2][3];      // CeltDeco 67..69 down / 70..72 up
    Sprite unitwin_header[3][3];   // GenericB 85/91/88 (3 pieces)
    Sprite unitwin_button[3][3];   // GenericB 19/25/22 Disband/Close
    Sprite unitwin_list_sel;       // Menus 8   selected ability row
    Sprite unitwin_celt_knot;      // CeltDeco 3 27x27 knot
    Sprite unitwin_celt_line;      // CeltDeco 55 7x27 left cap of the band
    Sprite unitwin_celt_mid;       // CeltDeco 56 100x27 braid tile
    Sprite unitwin_celt_right;     // CeltDeco 57 7x27 right cap of the band
    Sprite unitwin_line;           // CeltDeco 1 100x27 UnitLine1/2
    // `Int\Scenes\CTrans.ILB` = IntGfxMod.CampaignTrans: the 12x9 PNext/PPrev
    // arrows (23/24 up/down previous, 25/26 up/down next).
    aow::IlbFile ctrans_il;
    Sprite unitwin_pprev[2];
    Sprite unitwin_pnext[2];
    // T2SBar's NGVsb2 pieces (GenericS 94..101): dec up/down, inc up/down,
    // track and thumb. The memo's scroll bar (visible in the reference).
    Sprite unitwin_sb[8];
    // The selected unit's own art. `unitwin_face` is the 103x128 portrait from
    // the unit-gfx record's image library; `unitwin_sprite` is the 64x64
    // standing pose drawn in the right frame (`TAbstractUnit.ShowUnit`).
    aow::IlbFile unitwin_face_il;
    std::string unitwin_face_path;
    Sprite unitwin_face;
    int unitwin_face_key = -2;     // UnitEntry id the face was decoded for
    Sprite unitwin_sprite;
    int unitwin_sprite_key = -2;   // MapUnit id/slot the sprite was decoded for
    int unitwin_sprite_vx = 0;     // frame offsets relative to the ShowUnit anchor
    int unitwin_sprite_vy = 0;
    // --- the Structure window (TIWindow) art -------------------------------
    // TIWINDOW.dfm names: InfoWin (GenericF/GenericT), the four title buttons
    // (GenericI), SIcon (WinTIcon 2), CeltFrame (CeltDeco 22), MainBtn
    // (GenericB 19/25/22), CityBorderPnl (GenericF 115 + the BCS city view),
    // RazePnl (CeltDeco 74 + GenericI 53/55/54), GoldIcn (LBIcons 31) and the
    // Age10/Age8 fonts. All are decoded from the game's own libraries.
    aow::IlbFile prodwin_il;      // Int\ProdWin.ILB
    aow::IlbFile relwin_il;       // Int\Scenes\RelFr.ILB (race-relation faces)
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
    Sprite iwin_celt_edge;        // CeltDeco 0   7x27 ILIndexLine
    Sprite iwin_celt_line_a;      // CeltDeco 17  50x5
    Sprite iwin_celt_line_b;      // CeltDeco 22  50x6
    Sprite iwin_raze_bg;          // CeltDeco 74  56x35
    Sprite iwin_raze_btn[3];      // GenericI 53/55/54
    Sprite iwin_gold_icon;        // LBIcon 31
    Sprite iwin_upgrade_icon;     // LBIcon 1     city-level icon
    Sprite iwin_prod_orb;         // ProdWin 4    39x37
    Sprite iwin_relation[kStructureRelationCount];  // RelFr 11..18 faces
    Sprite iwin_plaque_tex;       // GenericT 14  name-plaque parchment
    // --- TCityScreen's production cards (`PreDrawProductions`) --------------
    // `PreDrawProductions` (AoW.exe 0x43d950) fills a small draw-info record
    // ({facing, 0, 0, 0, 0, 0}) and hands it to the selected `TUnitProduction`'s
    // sprite draw together with the anchor `(rect.Left - 4, rect.Top - 4)`. The
    // record's first byte is the facing the unit-art sequence resolves: **3**
    // for a "produce" item (`TUnitProduction+0xc <> 0`) and **5** for an
    // "install" item - the same 3 `AoWE.TUnitHS.Create@0x55783da4` leaves in
    // `TUnitHS+0x14`, which is AoW's towards-the-viewer pose. The port used to
    // pass direction 1 (the away-facing pose), so every card showed the unit's
    // back. See docs/27_PRODUCTION_SCREEN.md §3.3.1.
    static constexpr int kCityCardFaceProduce = 3;  // 0x43d9f5 record byte 0
    static constexpr int kCityCardFaceInstall = 5;  // 0x43da4f record byte 0
    // The P1CB..P12CB control rects: ActivityPnl (WinLeft 11, WinTop 164) plus
    // the checkbox's own WinLeft/WinTop (P1CB 2/0, P2CB 53/0 -> a 51 px pitch).
    static constexpr int kCityCardX = 13;
    static constexpr int kCityCardY = 164;
    static constexpr int kCityCardPitch = 51;
    // `PreDrawProductions` builds its rect 2 px above that control: the
    // reference capture places the 52x64 RelFr 42 checkbox art at
    // (cell.x, cell.y - 2) and the 48x60 CityWin 4/5 face at (cell.x + 2,
    // cell.y) - the handler's own +2,+2 - and the art is not clipped to the
    // 52x52 control (its frame reaches cell.y + 62).
    static constexpr int kCityCardRaise = 2;
    // The sprite anchor inside the cell: `Show(Left - 4, Top - 4)`; the frame's
    // own base+shift rides on top (`CityCardArt::vx/vy`). The reference capture
    // lands `HMNAR209` (facing 3, shift (25,25)) on `cell + (21,19)` - exactly
    // `cell + (-4,-6) + (25,25)` - and HMNAR209's own sequence offsets
    // (-16,-24) are *not* applied there.
    static constexpr int kCityCardUnitDx = 4;
    static constexpr int kCityCardUnitDy = 4;
    // The "install" branch's second pass: `TSprite16.ShowBlended` with
    // `GFXE.TLookupBlendTableCollection100.GetLookupTables(AlphaBlendTable,
    // 0x2d)` - the 45 % alpha-blend table (`GenerateLookupTables` maps index i
    // to table `i * count / 100`; `GenerateAlphaBlendTable` fills each entry
    // with `round(src * p + dst * (1 - p))` per 5-bit channel).
    // `PreDrawProductions` re-draws the CityWin frame-5 face over the card with
    // it: the pale wash over a unit that is not installed yet.
    static constexpr int kCityCardInstallBlend = 45;  // 0x2d = 45 %

// --- the City screen (TCityScreen) art ---------------------------------
    // `Int\Scenes\CityWin.ILB` supplies the dialog skin (CityBg), the celtic
    // frame, the city shadows, the separator bar, the three picture cards and
    // the current-action card; `Images\Faces\R_Faces.ILB` holds one 103x128
    // race portrait per race (RacePortrait's OnDraw -> TRaceResource.GetFace).
    // The static 640x432 backdrop comes from `assets/cityscreen.bmp`
    // (tools/city_screen_bg.py); every live control is drawn over it.
    Sprite city_bg;               // the 640x480 TCityScreen.Dlg backdrop
    Sprite city_bg_clean;         // same, with the live labels erased
    bool city_bg_exact = false;   // city_bg is the captured reference skin
    Sprite city_glass;            // CityWin 1  GlassFr 618x162 celtic oval
    Sprite city_shadow_l;         // CityWin 2  CityShadowL 231x151
    Sprite city_shadow_r;         // CityWin 3  CityShadowR 231x151
    Sprite city_sepbar;           // CityWin 9  CityFr 618x4 separator
    Sprite city_income_pic;       // CityWin 60 Is-Income 48x60
    Sprite city_ench_pic;         // CityWin 61 Is-Ench 48x60
    Sprite city_action_pic;       // CityWin 40 Is-Merch 48x60
    // The production cards (`P1CB..P12CB`): the base TAOWCheckBox draws
    // RelFr 42 (up) / 43 (lit+down) and `PreDrawProductions` overlays the
    // CityWin card face - frame 4 for a `TUnitProduction+0xc` entry, frame 5
    // for the other kind, frame 62 (Is-Empty) when GetItem() returns 0.
    Sprite city_activity_pic[6];  // CityWin 40..46 (B1CB..B6CB cards)
    Sprite city_card_frame[2];    // CityWin 4 / 5 (the two card faces)
    Sprite city_card_empty;       // CityWin 62 Is-Empty
    Sprite city_card_up;          // RelFr 42 (unchecked)
    Sprite city_card_sel;         // RelFr 43 (checked)
    Sprite city_queue_clean;      // assets/cityqueue.bmp (QueueLB w/o <empty>)
    Sprite city_queue_sel_tex;    // Menus 8 (selected row texture)
    aow::IlbFile relfr_il;        // Int\Scenes\RelFr.ILB (RelWin)
    Sprite city_body_tex;         // GenericT 17 TextOld1 (tile)
    Sprite city_body_tex2;        // GenericT 18 TextOld2 (tile)
    Sprite city_check[3];         // GenericI 47/49/48 unchecked/lit/down
    // The production/change page's six activity backdrops
    // (assets/cityprod1..6.bmp; tools/city_prod_bg.py). The original draws
    // this page from CityWin.ILB over the generated city map, so - like the
    // overview backdrop - the port carries the six reference captures.
    Sprite city_prod_frame[6];    // B1CB..B6CB pages, 640x480
    // The unit detail page (`FUN_0043c43c` cases 7/8). Its controls are drawn
    // live: the green page body (GenericT 2), TextOld1/2 panel bodies
    // (GenericT 17/18/19), the FrThinD/FrGoldD frames (GenericF 105..120), the
    // six stat icons (Int\Icons.ILB 0..4), the move-type icon (IconMove.ILB)
    // and the unit's Unitgfx face. See the kUnit* block above.
    Sprite city_page_tex;         // GenericT 2  MainFloaterSet (page body)
    Sprite city_panel_tex3;       // GenericT 19 (UPictureFramePnl body)
    Sprite city_unit_frame[8];    // GenericF 105..112 FrThinD
    Sprite city_unit_gold_frame[8];    // GenericF 113..120 FrGoldD
    Sprite city_unit_icon[5];     // Int\Icons.ILB 0..4 (atk/def/dmg/res/hits)
    Sprite city_unit_abil_sb[8];  // GenericS 86..93 NGVsb2 (UAbilitiesLB bar)
    int city_unit_abil_top = 0;   // first visible row of UAbilitiesLB
    aow::IlbFile city_unit_face_il;  // the selected unit's FACES library
    std::string city_unit_face_path;
    Sprite city_unit_face;        // the selected unit's face (103x128)
    // The unit id `city_unit_face` was decoded for, -1 when none. The cache
    // cannot key on `city_unit_page.unit_id`: by the time the face is loaded
    // that field already names the *newly* checked card, which made every
    // selection after the first keep the previous unit's portrait.
    int city_unit_face_id = -1;
    // Everything `FUN_0043c43c` writes into the UnitPnl controls, resolved for
    // the checked card (`city_prod_sel`). `city_unit_page_refresh` rebuilds it.
    struct CityUnitPage {
        bool valid = false;
        int unit_id = -1;
        std::string title;        // "Produce "/"Install " + UnitRes.GetUnitName
        int attack = 0, defense = 0, damage = 0;
        int resistance = 0, hits = 0, moves = 0;
        int move_icon = 0;        // FUN_004541b0(GetMoveTypes)
        int cost = 0, turns = 0;  // UnitProductionGold / UnitProductionTime
        std::string alignment;    // TAlignmentRStr[alignment & 0x7f]
        int level = 0, upkeep = 0;  // Level X / Upkeep Y
        std::vector<std::string> abilities;  // ListAbilities -> UAbilitiesLB
    };
    CityUnitPage city_unit_page;
    aow::IlbFile rfaces_il;       // Images\Faces\R_Faces.ILB
    Sprite city_race_face[12];    // R_Faces 0..11 (103x128)
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
    // The arriving army's `TArmyHS.PlaceOnMap` (map field message 0x1102) is
    // delivered when the walk reaches its destination, not when the move starts:
    // `TMoveArmyTE.FinishMove@0x55747fbc` runs at the end of the transition, so
    // a building under the army keeps its owner and its flag for the whole walk
    // and is claimed only once the unit has arrived (`mv_finish_arrival`).
    bool mv_arrive_pending = false;
    int mv_arrive_x = -1, mv_arrive_y = -1;
    std::vector<std::uint8_t> game_draw;  // game_px + the overlay (RGBA)
    // The level's *contents* layer, kept alongside the flat `game_px` so the
    // move-selection marker can put the hex's contents back over its far wall
    // (mapviz's split render - the terrain layer it also produces is only a local
    // there). Empty when the split render failed; the marker then degrades to the
    // plain fill. See mv_draw_hex_selector and docs/09_GAME_SCREEN.md 6.9.
    std::vector<std::uint8_t> mv_px_contents;  // objects, roads, cities, armies
    bool mv_contents_stale = true;             // the layer must be rebuilt
    // The selected structure's opacity pulse (TStructure.Select / TIWindow):
    // `mv_px_struct_bg` is the same level rendered with only that map object
    // hidden. Every frame the object's pixels are re-blended over this
    // background at a clock-driven alpha, exactly like the engine re-drawing
    // the scene with the selected building's opacity oscillating.
    std::vector<std::uint8_t> mv_px_struct_bg;  // level minus the selected object
    AowBox mv_struct_fade_box;                  // its pixel bounding box
    bool mv_struct_fade = false;                // the background is valid
    int mv_struct_fade_level = -1;
    std::size_t mv_struct_fade_offset = static_cast<std::size_t>(-1);
    int mv_struct_fade_clock0 = 0;              // map clock when the floater opened
    int mv_struct_fade_alpha = 255;             // last applied 0..255
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
    // The day NEWTURN.WAV was last played for. The sound belongs to the start of
    // the new-day rotation (the day handler), so the later hand-over to the
    // human's seat on the same day must not play it a second time.
    int game_newturn_sound_day = 0;
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
    // --- the Unit info window (TUnitWindow) ---------------------------------
    // `FUN_00407db8` fills every control from the selected unit; this is the
    // resolved copy the renderer draws (the port has no widget tree). One row
    // is what `TAbstractUnit.ListAbilityInfo` appends to the three listboxes.
    struct UnitWindowAbility {
        std::string name;
        std::string type;
        std::string source;
    };
    struct UnitWindowData {
        bool valid = false;
        int slot = -1;
        std::string name;          // UnitName
        std::string description;   // T2Memo
        std::string type;          // TypeV
        std::string alignment;     // AAlignmentLbl
        std::string relation;      // RelationV
        std::string morale;        // MoraleLbl (empty on a Machine)
        int type_icon = 0;         // 0..2 -> UnitWin 21/22/23
        int align_icon = 0;        // 0..4 -> RelWin 31..35
        int relation_icon = 0;     // 0..7 -> RelWin 11..18
        int attack = 0, defense = 0, damage = 0, resistance = 0;
        int hits = 0, max_hits = 0, moves = 0, max_moves = 0;
        int level = 0, experience = 0, next_level_experience = 0;
        int upkeep = 0;
        bool upkeep_mana = false;  // icon 52 instead of 51
        int move_icon = 0;         // IconMove id
        bool show_type = true;
        bool show_relation = true;
        bool show_morale = true;
        bool is_hero = false;
        std::vector<UnitWindowAbility> abilities;
    };
    bool game_show_unit = false;     // UnitWin.Visible
    int game_unit_tab = 0;           // UnitTab.Index (0 Abilities, 1 Info)
    int game_unit_slot = -1;         // the party slot the window was opened on
    UnitWindowData unit_win;         // the resolved unit page
    int unitwin_hover = -1;          // hovered control id, or -1
    int unitwin_pressed = -1;        // pressed control id
    int unitwin_abil_sel = -1;       // T1AbilityLB.Index
    int unitwin_abil_top = 0;        // first visible ability row
    mutable int unitwin_memo_top = 0;  // T2Memo first visible line (T2SBar)
    mutable std::vector<std::string> unitwin_memo_lines;  // wrapped T2Memo rows
    mutable int unitwin_memo_total = 0;                   // T2SBar.Max
    bool unitwin_disband_armed = false;  // the Disband confirmation is up

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
    int game_struct_gold = 0;                      // the income read-out
    // The second InfoPnl value is TIWindow's CityLevelLbl (the city's size /
    // level byte), not mana; the field name is kept for the capture override.
    int game_struct_mana = 0;
    int game_struct_relation = kStructureRelationDefault;  // RelFr face band
    int game_struct_owner = 0;                     // owning player (0 = neutral)
    int game_struct_race_index = -1;               // race library index
    int game_struct_pressed = -1;                  // pressed control id
    int game_struct_hover = -1;                    // hovered control id
    std::string game_struct_art_lib;               // loaded race/BCS library
    Sprite game_struct_art;                        // BCS_<race>.BMP (city view)

    // --- the City screen (TCityScreen, modal `Dlg`) -------------------------
    // The Structure floater's MainBtnClick (`TIWindow.MainBtnClick`, the Enter
    // button) hands a city over to TCityScreen: `TCityScreen.ShowCity`
    // (AoW.exe 0x0043af24) fills the activity panels (`FUN_0043b208`), builds
    // the production row (`FUN_0043bb14`) and generates the tactical city view
    // through `AoWTC.TRndTacHsm.RandomMap` (AoWTCPCK.dpl 0x004245ec), then
    // shows the 640x432 `Dlg` over the world map. Every displayed city value
    // is snapshotted here when the screen opens, exactly like the form's own
    // controls, so the renderer is a pure function of this state.
    bool game_show_city = false;
    int game_city_hx = -1, game_city_hy = -1;
    std::string city_name;                 // StructName (AoW15WhiteGrey)
    std::string city_population;           // PopulationHdr line 2 ("Humans")
    std::string city_status;               // PopulationHdr line 4 ("Cheerful")
    std::string city_race_lines;           // PopulationMemo ("Race Relation: ...")
    std::string city_income_header;        // IncomeHdr ("Income" / "38 gold")
    std::string city_income_lines;         // IncomeMemo ("Base Income", ...)
    std::string city_income_gold;           // IncomeGoldMemo (right-aligned values)
    std::string city_enh_header;           // EnhancementsHdr
    std::string city_enh_lines;            // EnhancementsMemo
    std::string city_production;           // ActiveAction ("Producing ...")
    std::string city_production_turns;     // ActiveActionTurns ("(default)")
    int city_gold = 38;                    // income read-out
    int city_base_income = 20;
    int city_crop_fields = 5;
    int city_crop_income = 10;
    int city_prod_income = 6;
    int city_upgrade_level = 2;
    std::string city_wall = "Stone Wall";
    int city_queue_sel = -1;               // QueueLB.Selected (255 = none)
    bool city_repeat = false;              // LoopQueueBox.Checked
    // TCityScreen's +0x2cc display mode: 0 = overview, 1 = production picker,
    // 2 = queue editor. `city_change` keeps the picker up for both 1 and 2;
    // `city_queue_edit` is the original's `+0x2cc == 2`.
    bool city_change = false;
    bool city_queue_edit = false;
    int city_activity = 0;                 // B1CB..B6CB Tag (0..5)
    // The selected production card (`TCityScreen+0x2c4`) and the city's own
    // production model (City.TCity rw tag 0x12, parsed by the map loader).
    int city_prod_sel = -1;
    std::vector<aow::CityProductionUnit> city_prod_units;
    std::vector<aow::CityQueueItem> city_queue;
    std::vector<std::string> city_prod_names;
    std::vector<std::string> city_queue_names;
    bool city_loop = false;
    // The TCity object the open city screen was shown for (its MapHs in
    // game_state_map). Production edits are written back here so closing and
    // re-opening the screen - and the turn loop - see the same queue.
    aow::MapHs* city_map_hs = nullptr;
    // The live `TProductionControl+0x20` state and the `+0x24` progress fields
    // (see aow::MapHs::production_turns_*). 0 = idle, 1 = waiting for the
    // player's gold (ExecuteProduction declined), 2 = executing.
    std::uint8_t city_prod_state = 0;
    std::int32_t city_prod_turns_left = 0;
    std::int32_t city_prod_turns_total = 0;
    std::int32_t city_prod_gold_paid = 0;
    std::int32_t city_prod_mana_paid = 0;
    // A card's decoded standing pose, drawn exactly the way
    // `PreDrawProductions` draws it: at the engine's own sprite anchor
    // (`Show(Left - kCityCardUnitDx, Top)` plus the sequence's xoff/yoff and the
    // frame's base+shift), clipped to the 52x52 control. Built once when the
    // city opens so the page render never decodes a unit sprite.
    struct CityCardArt {
        Sprite s;
        int vx = 0;   // anchor offset from (cell.x - kCityCardUnitDx, cell.y)
        int vy = 0;
    };
    std::vector<CityCardArt> city_card_art;
    int city_pressed = -1;                 // pressed control id
    int city_hover = -1;                   // hovered control id

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
            out.slot_w = e.width;
            out.slot_h = e.height;
            out.shift_x = e.xshift;
            out.shift_y = e.yshift;
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
            out.slot_w = e.width;
            out.slot_h = e.height;
            out.shift_x = e.xshift;
            out.shift_y = e.yshift;
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
            //
            // The pen step stays the descriptor slot width (`e.width`, the
            // TLibraryImage advance that TImageLibraryFont.GetTextWidth adds
            // CharacterDistance to); the RLE rectangle (`e.rle_w`) is the ink
            // box, not the advance. Advancing by `e.rle_w` here changes every
            // 0x02 face at once and visibly squeezed the Scenario screen's
            // Age8s labels ("CPU Squire", "Simultaneous") by a pixel per glyph.
            // Faces that need a different step pin their CharacterDistance at
            // the draw site instead (see draw_game_unit_tabs).
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
            // PlayModeCB's ILTextureSelected: GenericT 16 = TextLBSel.BMP, the
            // light row the combo paints over its dropped-down list.
            decode_id(generict_il, kLbSelTexId, scen_lbsel_tex);
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
            // TCityScreen's UAbilitiesLB VScrollBar (T2SBar) names GenericS 86..93.
            for (int i = 0; i < 8; ++i) {
                decode_id(generics_il, static_cast<std::uint32_t>(86 + i),
                          city_unit_abil_sb[i]);
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
                // `IntGfxMod.UnitIcons` ids 0..4 are the six unit-stat icons
                // TCityScreen's UAttackIcon..UHitsIcon name: sword, shield,
                // star (damage), ankh (resistance), heart (hits). They are
                // class-0x11 RLE sprites whose visual box sits at anchor +
                // base + class shift, so fold the shift in like the shields.
                for (int i = 0; i < 5; ++i) {
                    decode_id(uniticons_il, static_cast<std::uint32_t>(i),
                              city_unit_icon[i]);
                    const int p =
                        pos_of(uniticons_il, static_cast<std::uint32_t>(i));
                    if (p >= 0 && city_unit_icon[i].ok) {
                        const aow::IlbEntry& e =
                            uniticons_il.entries()[static_cast<std::size_t>(p)];
                        city_unit_icon[i].base_x += e.xshift;
                        city_unit_icon[i].base_y += e.yshift;
                    }
                }
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
                // 48 checked / 49 lit) - the DFM's ILIndexUp/Down/Lit, and the
                // art the shipped capture shows.
                decode_id(generici_il, kNgcBoxOff, scen_ngcbox[0]);
                decode_id(generici_il, kNgcBoxOn, scen_ngcbox[1]);
                decode_id(generici_il, kNgcBoxLit, scen_ngcbox[2]);
            }
            const std::string cw = (base / "Int" / "Scenes" / "CityWin.ILB").string();
            if (citywin_il.load(cw, err)) {
                decode_id(citywin_il, 5, event_shield_tex);
                // TCityScreen's own art (TCITYSCREEN.dfm). The MapView city
                // panorama itself is generated by TRndTacHsm.RandomMap; the
                // port's static backdrop carries the reference's recovered
                // city view (see assets/cityscreen.bmp below).
                decode_id(citywin_il, 0, city_bg);       // CityBg (fallback)
                decode_id(citywin_il, 1, city_glass);    // GlassFr
                decode_id(citywin_il, 2, city_shadow_l); // CityShadowL
                decode_id(citywin_il, 3, city_shadow_r); // CityShadowR
                decode_id(citywin_il, 9, city_sepbar);   // CityFr separator
                decode_id(citywin_il, 40, city_action_pic);  // Is-Merch
                decode_id(citywin_il, 60, city_income_pic);  // Is-Income
                decode_id(citywin_il, 61, city_ench_pic);    // Is-Ench
                // B1CB..B6CB activity cards (TCityScreen's ActivityPnl2).
                for (std::uint32_t i = 0; i < 6; ++i) {
                    decode_id(citywin_il, 40 + i, city_activity_pic[i]);
                }
                // P1CB..P12CB card faces: 4 (kind 4) / 5 (other kind) and
                // 62 Is-Empty when TUnitProductionList.GetItem() is null.
                decode_id(citywin_il, 4, city_card_frame[0]);
                decode_id(citywin_il, 5, city_card_frame[1]);
                decode_id(citywin_il, 62, city_card_empty);
            }
            // RelWin (`Int\Scenes\RelFr.ILB`) frames 42/43 are the
            // TAOWCheckBox art behind each production card.
            {
                const std::string rf =
                    (base / "Int" / "Scenes" / "RelFr.ILB").string();
                if (relfr_il.load(rf, err)) {
                    decode_id(relfr_il, 42, city_card_up);
                    decode_id(relfr_il, 43, city_card_sel);
                }
            }
            // QueueLB's selected-row texture (`ILTextureSelected = Menus 8`).
            if (menus_il.entries().empty()) {
                const std::string mn = (base / "Int" / "Menus.ILB").string();
                menus_il.load(mn, err);
            }
            decode_id(menus_il, 8, city_queue_sel_tex);
            if (generict_il.entries().empty()) {
                const std::string gt = (base / kGenericTIlb).string();
                generict_il.load(gt, err);
            }
            decode_id(generict_il, 17, city_body_tex);   // TextOld1
            decode_id(generict_il, 18, city_body_tex2);  // TextOld2
            // The unit detail page (`FUN_0043c43c` cases 7/8): the green page
            // body is the same MainFloaterSet tile, UPictureFramePnl uses
            // TextLb17, and every panel is TextOld1.
            decode_id(generict_il, 2, city_page_tex);    // MainFloaterSet
            decode_id(generict_il, 19, city_panel_tex3); // TextLb17
            decode_id(generici_il, 47, city_check[0]);   // NGCBox off
            decode_id(generici_il, 49, city_check[1]);   // NGCBox lit
            decode_id(generici_il, 48, city_check[2]);   // NGCBox down
            // RacePortraitDraw -> TRaceResource.GetFace: one 103x128 portrait
            // per race from Images\Faces\R_Faces.ILB (r_human.BMP .. r_undead).
            const std::string rf =
                (base / "Images" / "Faces" / "R_Faces.ILB").string();
            if (rfaces_il.load(rf, err)) {
                for (int r = 0; r < 12; ++r) {
                    decode_id(rfaces_il, static_cast<std::uint32_t>(r),
                              city_race_face[r]);
                }
            }
            // The 640x432 dialog backdrop (tools/city_screen_bg.py). The game
            // install's CityBg is the shipped skin; the capture's UI parchment
            // differs, so the port prefers its own extracted bitmap and falls
            // back to CityWin's CityBg when the asset is absent.
            {
                char exe[MAX_PATH] = {0};
                GetModuleFileNameA(nullptr, exe, MAX_PATH);
                const fs::path exe_dir = fs::path(exe).parent_path();
                const fs::path cands[] = {
                    base / "assets" / "cityscreen.bmp",
                    base / "port" / "assets" / "cityscreen.bmp",
                    exe_dir / "assets" / "cityscreen.bmp",
                    exe_dir / ".." / "assets" / "cityscreen.bmp",
                    exe_dir / ".." / "port" / "assets" / "cityscreen.bmp",
                };
                for (const fs::path& p : cands) {
                    if (load_bmp24(p.string(), city_bg)) {
                        city_bg_exact = true;
                        fs::path clean = p;
                        clean.replace_filename("cityscreen_clean.bmp");
                        load_bmp24(clean.string(), city_bg_clean);
                        break;
                    }
                }
            }
            // The six production/change pages (tools/city_prod_bg.py). They are
            // the B1CB..B6CB activity pages of TCityScreen mode 1; the reference
            // captures are 1:1 but cropped/shifted, so the tool registers them
            // onto the 01 frame and writes six full 640x480 BMPs.
            {
                char exe[MAX_PATH] = {0};
                GetModuleFileNameA(nullptr, exe, MAX_PATH);
                const fs::path exe_dir = fs::path(exe).parent_path();
                for (int i = 0; i < 6; ++i) {
                    const std::string name =
                        "cityprod" + std::to_string(i + 1) + ".bmp";
                    const fs::path cands[] = {
                        base / "assets" / name,
                        base / "port" / "assets" / name,
                        exe_dir / "assets" / name,
                        exe_dir / ".." / "assets" / name,
                        exe_dir / ".." / "port" / "assets" / name,
                    };
                    for (const fs::path& p : cands) {
                        if (load_bmp24(p.string(), city_prod_frame[i])) break;
                    }
                }
                // The QueueLB box without the capture's five `<empty>` rows
                // (tools/city_prod_bg.py's clean_queue patch).
                const fs::path qcands[] = {
                    base / "assets" / "cityqueue.bmp",
                    base / "port" / "assets" / "cityqueue.bmp",
                    exe_dir / "assets" / "cityqueue.bmp",
                    exe_dir / ".." / "assets" / "cityqueue.bmp",
                    exe_dir / ".." / "port" / "assets" / "cityqueue.bmp",
                };
                for (const fs::path& p : qcands) {
                    if (load_bmp24(p.string(), city_queue_clean)) break;
                }
            }
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
                // TCityScreen's unit detail page: UStatPnl/AbilPnl/UCostPnl
                // name GenericF 105 (FrThinD) and the inset stat/picture/cost
                // boxes name GenericF 113 (FrGoldD).
                decode_id(genericf_il, kThinFrameBase + i, city_unit_frame[i]);
                decode_id(genericf_il, kGoldFrameBase + i, city_unit_gold_frame[i]);
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
            // --- the Unit info window (TUnitWindow) ----------------------------
            // Its own library (Int\UnitWin.ILB 44/45/46 and 21..23), the
            // DialogFr frame (GenericF 32..39), the FrGoldD/FrThinD panels and
            // the GenericT bodies 2/7/17/21 - the same art the party and city
            // screens already load. The tabs/headers/buttons use CeltDeco and
            // GenericB three-piece skins; RelWin supplies the alignment rings
            // and the relation faces.
            {
                if (generict_il.entries().empty()) {
                    generict_il.load((base / kGenericTIlb).string(), err);
                }
                if (genericb_il.entries().empty()) {
                    genericb_il.load((base / kGenericBIlb).string(), err);
                }
                if (generici_il.entries().empty()) {
                    generici_il.load((base / "Int" / "GenericI.ILB").string(),
                                     err);
                }
                if (celldeco_il.entries().empty()) {
                    celldeco_il.load((base / kCeltDecoIlb).string(), err);
                }
                if (menus_il.entries().empty()) {
                    menus_il.load((base / "Int" / "Menus.ILB").string(), err);
                }
                if (uniticons_il.entries().empty()) {
                    uniticons_il.load((base / kUnitIconsIlb).string(), err);
                }
                if (tbwin_il.entries().empty()) {
                    tbwin_il.load((base / "Int" / "TBWin.ILB").string(), err);
                }
                if (iconmove_il.entries().empty()) {
                    iconmove_il.load((base / kIconMoveIlb).string(), err);
                }
                if (titleicon_il.entries().empty()) {
                    titleicon_il.load((base / kWinTIconIlb).string(), err);
                }
                const std::string uw = (base / kUnitWinIlb).string();
                if (unitwin_il.load(uw, err)) {
                    decode_id(unitwin_il, kUnitWinLeftFrame,
                              unitwin_left_frame);
                    decode_id(unitwin_il, kUnitWinRightFrame,
                              unitwin_right_frame);
                    decode_id(unitwin_il, kUnitWinShade, unitwin_shade);
                    decode_id(unitwin_il, kUnitWinTypeCreature,
                              unitwin_type_icon[0]);
                    decode_id(unitwin_il, kUnitWinTypeMachine,
                              unitwin_type_icon[1]);
                    decode_id(unitwin_il, kUnitWinTypeUnit,
                              unitwin_type_icon[2]);
                }
                for (int i = 0; i < 8; ++i) {
                    decode_id(genericf_il, kUnitWinFrameBase + i,
                              unitwin_frame[i]);
                    decode_id(genericf_il, kGoldFrameBase + i,
                              unitwin_gold_frame[i]);
                    decode_id(genericf_il, kThinFrameBase + i,
                              unitwin_thin_frame[i]);
                }
                decode_id(generict_il, 2, unitwin_body);
                decode_id(generict_il, 7, unitwin_tex7);
                decode_id(generict_il, 17, unitwin_tex17);
                decode_id(generict_il, 18, unitwin_tex18);
                decode_id(generict_il, 21, unitwin_tex21);
                decode_id(titleicon_il, 3, unitwin_title_icon);
                for (int i = 0; i < 3; ++i) {
                    decode_id(generici_il, kPartyCloseArt[i], unitwin_close[i]);
                }
                for (int p = 0; p < 3; ++p) {
                    decode_id(celldeco_il, kUnitTabDown + p,
                              unitwin_tab[0][p]);
                    decode_id(celldeco_il, kUnitTabUp + p, unitwin_tab[1][p]);
                }
                const std::uint32_t hdr[3] = {kUnitHeaderUp, kUnitHeaderLit,
                                              kUnitHeaderDown};
                for (int st = 0; st < 3; ++st) {
                    for (int p = 0; p < 3; ++p) {
                        decode_id(genericb_il, hdr[st] + p,
                                  unitwin_header[st][p]);
                    }
                }
                const std::uint32_t btn[3] = {kUnitButtonUp, kUnitButtonLit,
                                              kUnitButtonDown};
                for (int st = 0; st < 3; ++st) {
                    for (int p = 0; p < 3; ++p) {
                        decode_id(genericb_il, btn[st] + p,
                                  unitwin_button[st][p]);
                    }
                }
                decode_id(menus_il, 8, unitwin_list_sel);
                decode_id(celldeco_il, 3, unitwin_celt_knot);
                decode_id(celldeco_il, 55, unitwin_celt_line);
                decode_id(celldeco_il, 1, unitwin_line);
                for (int i = 0; i < 5; ++i) {
                    decode_id(uniticons_il, static_cast<std::uint32_t>(i),
                              unitwin_stat_icon[i]);
                }
                decode_id(iconmove_il, 0, unitwin_move_icon);
                decode_id(tbwin_il, 51, unitwin_upkeep_gold);
                decode_id(tbwin_il, 52, unitwin_upkeep_mana);
                // BCeltL is a frHorizLine: CeltDeco 55 (left cap) + tiled 56
                // (100x27 braid) + 57 (right cap), exactly DrawILLh's three
                // images (ILIndexLine 55, +1, +2).
                decode_id(celldeco_il, 56, unitwin_celt_mid);
                decode_id(celldeco_il, 57, unitwin_celt_right);
                // T2SBar: GenericS 94..101 (ILIndexDecUp..ILIndexTab).
                for (std::uint32_t i = 0; i < 8; ++i) {
                    if (generics_il.entries().empty()) {
                        generics_il.load((base / "Int" / "GenericS.ILB").string(),
                                         err);
                    }
                    decode_id(generics_il, 94 + i, unitwin_sb[i]);
                }
                // PNext/PPrev: CampaignTrans 23/25 (up) and 24/26 (down).
                if (ctrans_il.load((base / "Int" / "Scenes" / "CTrans.ILB").string(),
                                   err)) {
                    decode_id(ctrans_il, 23, unitwin_pprev[0]);
                    decode_id(ctrans_il, 24, unitwin_pprev[1]);
                    decode_id(ctrans_il, 25, unitwin_pnext[0]);
                    decode_id(ctrans_il, 26, unitwin_pnext[1]);
                }
                if (relfr_il.entries().empty()) {
                    relfr_il.load((base / kRelWinIlb).string(), err);
                }
                for (int i = 0; i < 5; ++i) {
                    decode_id(relfr_il, kUnitWinAlignBase + i,
                              unitwin_align_icon[i]);
                }
                for (int i = 0; i < 8; ++i) {
                    decode_id(relfr_il, kUnitWinRelationBase + i,
                              unitwin_relation[i]);
                }
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
            // TAoWFrame's ILIndexLine = 0: the 7x27 knot segment it tiles along
            // the frame's edges (CeltFrame.ILIndexLine in TIWINDOW.dfm).
            decode_id(celldeco_il, 0, iwin_celt_edge);
            decode_id(celldeco_il, 17, iwin_celt_line_a);
            decode_id(celldeco_il, 22, iwin_celt_line_b);
            decode_id(celldeco_il, 74, iwin_raze_bg);
            if (lbicon_il.entries().empty()) {
                const std::string lb = (base / "Int" / "LBIcon.ILB").string();
                lbicon_il.load(lb, err);
            }
            decode_id(lbicon_il, 31, iwin_gold_icon);
            decode_id(lbicon_il, 1, iwin_upgrade_icon);
            {
                const std::string pw = (base / "Int" / "ProdWin.ILB").string();
                if (prodwin_il.load(pw, err)) {
                    decode_id(prodwin_il, 4, iwin_prod_orb);
                }
                // RaceRelationIcon: IntGfxMod.RelWin is Int\Scenes\RelFr.ILB.
                // Its ids 11..18 are the eight 51x36 relation faces (11 angry
                // .. 18 friendly); `ShowInfo` sets CurrentImage = relation +
                // 0xb from `TRaceResource.GetFace` + GetPlayerRelation.
                const std::string rw =
                    (base / "Int" / "Scenes" / "RelFr.ILB").string();
                if (relwin_il.load(rw, err)) {
                    for (std::uint32_t i = 0; i < kStructureRelationCount; ++i) {
                        decode_id(relwin_il, kStructureRelationBase + i,
                                  iwin_relation[i]);
                    }
                }
                // The name plaque's parchment is GenericT's RedText tile (id
                // 14, 100x100); TAOWPanel tiles the inherited window texture.
                {
                    const std::string gt = (base / "Int" / "GenericT.ilb").string();
                    aow::IlbFile texture_il;
                    if (texture_il.load(gt, err)) {
                        decode_id(texture_il, 14, iwin_plaque_tex);
                    }
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
        mv_arrive_pending = false;
        mv_arrive_x = mv_arrive_y = -1;
        game_status.clear();
        game_draw.clear();   // the next refresh recopies the whole canvas
        mv_dirty = false;
        // Closing the party window deselects the party (`CloseClick` ->
        // `TArmy.Unselect`), so the floater goes with it.
        game_show_party = false;
        party_sel = 0;
        party_focus = -1;
        party_hover = party_pressed = -1;
        // The Unit info window follows the selection.
        game_show_unit = false;
        game_unit_slot = -1;
        unitwin_hover = unitwin_pressed = -1;
        unitwin_disband_armed = false;
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

    // The four bar read-outs: WGoldPnl's total/income and WManaPnl's total/
    // income. They are the seated player's live `Player::gold`, `Player::income`,
    // `Player::magic.mana` and `Player::magic.mana_income` (the values the turn
    // loop and compute_economy maintain). The `game_*` members remain the
    // pre-load reference seeds used when no roster is seated yet.
    void game_live_stats(int& gold, int& gold_income, int& mana,
                         int& mana_income) const {
        gold = game_gold;
        gold_income = game_gold_income;
        mana = game_mana;
        mana_income = game_mana_income;
        const aow::Player* p = game_seat();
        if (p == nullptr) return;
        gold = p->gold;
        gold_income = p->income;
        mana = p->magic.mana;
        mana_income = p->magic.mana_income;
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
        // The catalog is needed for TUnit upkeep (the Unitres level is not in
        // the map); load it before the economy pass. game_res is the map's
        // Release.hss resource list, which gives a city its TCityResource size
        // and the crop-owner surrounding-hex tables.
        aow::load_unit_catalog(root_dir, mv_cat, err);
        // Resolve the tile terrain first: a farm's `CropsEnabled`/income pass
        // reads the hex terrain (terrain 5 disables its crops), exactly like
        // TILTerrainMO.Activate@5570358c.
        aow::fill_tile_terrain(game_state_map,
                               static_cast<std::size_t>(game_level), game_res);
        aow::load_game_state(game_state_map, game_res, &mv_cat, mv_gs, err);
        // The first `TPlayerControl.NewDay` (day becomes 1) sends 0x20020001 to
        // the map; `Gems.TGems.MsgProc` answers it with GenerateGemsAmount, so
        // every gold pile gets its value from its stored size class now, using
        // the map's own RNG seed (docs/28_ECONOMY.md). Run it after the game
        // state exists (the amount is map data; the treasury is not touched).
        aow::generate_gems_amounts(game_state_map);
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
        // The selected structure's own opacity pulse, over the background render
        // that omits only that object (TStructure.Select).
        mv_apply_structure_fade();
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

    // Builds the selected structure's opacity-pulse background: the exact same
    // level render with only the selected map object omitted. The difference
    // between `game_px` and this canvas is the structure's own pixels (body,
    // walls, pad, owner flag), whose bounding box is small enough to re-blend
    // every map frame.
    void mv_clear_structure_fade() {
        mv_px_struct_bg.clear();
        mv_struct_fade = false;
        mv_struct_fade_level = -1;
        mv_struct_fade_offset = static_cast<std::size_t>(-1);
        mv_struct_fade_box = AowBox{};
        mv_struct_fade_clock0 = 0;
        mv_struct_fade_alpha = 255;
    }

    bool mv_prepare_structure_fade(int level, std::size_t offset) {
        mv_clear_structure_fade();
        if (game_px.empty() || game_state_map.levels.empty()) return false;
        aow::MapVizOptions opt;
        opt.level = level;
        opt.objects = true;
        opt.max_dim = 0;
        opt.hide_army_level = mv_hide_level;
        opt.hide_army_x = mv_hide_x;
        opt.hide_army_y = mv_hide_y;
        opt.no_animated_overlays = true;
        opt.no_units = true;
        opt.hide_object_level = level;
        opt.hide_object_offset = offset;
        std::uint32_t bw = 0, bh = 0;
        std::string err;
        if (!aow::render_map_rgba(game_state_map, game_res, root_dir, opt,
                                  mv_px_struct_bg, bw, bh, err, nullptr)) {
            mv_clear_structure_fade();
            return false;
        }
        if (bw != game_pw || bh != game_ph ||
            mv_px_struct_bg.size() != game_px.size()) {
            mv_clear_structure_fade();
            return false;
        }
        int x0 = static_cast<int>(bw), y0 = static_cast<int>(bh);
        int x1 = -1, y1 = -1;
        for (std::uint32_t y = 0; y < bh; ++y) {
            const std::size_t row = static_cast<std::size_t>(y) * bw * 4;
            for (std::uint32_t x = 0; x < bw; ++x) {
                const std::size_t p = row + static_cast<std::size_t>(x) * 4;
                if (game_px[p] == mv_px_struct_bg[p] &&
                    game_px[p + 1] == mv_px_struct_bg[p + 1] &&
                    game_px[p + 2] == mv_px_struct_bg[p + 2] &&
                    game_px[p + 3] == mv_px_struct_bg[p + 3]) {
                    continue;
                }
                if (static_cast<int>(x) < x0) x0 = static_cast<int>(x);
                if (static_cast<int>(y) < y0) y0 = static_cast<int>(y);
                if (static_cast<int>(x) > x1) x1 = static_cast<int>(x);
                if (static_cast<int>(y) > y1) y1 = static_cast<int>(y);
            }
        }
        if (x1 < x0 || y1 < y0) return false;
        mv_struct_fade_box = AowBox{x0, y0, x1 - x0 + 1, y1 - y0 + 1};
        mv_struct_fade = true;
        mv_struct_fade_level = level;
        mv_struct_fade_offset = offset;
        // The blink's phase 0 is the click that opened the floater, so the first
        // five map frames show the building fully opaque (the GIF's frames 1-5).
        mv_struct_fade_clock0 = game_clock;
        return true;
    }

    // Re-blends the selected structure over its background at the clock-driven
    // opacity of `bin/Screen Selection.gif` (see kStructurePulse*). This is the
    // visual `TStructure.Select` leaves behind: the building body itself blinks
    // against the terrain - a short 50% dip, not a smooth sinusoidal fade and not
    // a hex marker drawn over it. The flag/wall/armies are separate objects and
    // are re-composited opaque after this (mv_draw_structure_anim).
    void mv_apply_structure_fade() {
        if (!mv_struct_fade || !game_show_structure ||
            mv_struct_fade_level != game_level ||
            game_draw.size() != game_px.size() ||
            mv_px_struct_bg.size() != game_px.size()) {
            return;
        }
        // A square pulse: opaque for eleven of the thirteen map frames, 50% for
        // the two consecutive frames 6-7 of the capture. Wrapped by hand because
        // the map clock keeps counting (and `--game-clock` may step it backwards).
        int phase = (game_clock - mv_struct_fade_clock0) % kStructurePulsePeriod;
        if (phase < 0) phase += kStructurePulsePeriod;
        const bool dim = phase >= kStructurePulseFirst &&
                         phase < kStructurePulseFirst + kStructurePulseFrames;
        if (!dim) {
            // The prepare/refresh path already restored `game_px` (the opaque
            // building), so an on-frame needs no pixels written - only the box
            // recorded by the previous dimmed frame is left untouched.
            mv_struct_fade_alpha = 255;
            return;
        }
        const int a = kStructurePulseAlpha;
        const AowBox& b = mv_struct_fade_box;
        for (int y = b.y; y < b.y + b.h; ++y) {
            if (y < 0 || y >= static_cast<int>(game_ph)) continue;
            const std::size_t row = static_cast<std::size_t>(y) * game_pw * 4;
            for (int x = b.x; x < b.x + b.w; ++x) {
                if (x < 0 || x >= static_cast<int>(game_pw)) continue;
                const std::size_t p = row + static_cast<std::size_t>(x) * 4;
                if (game_px[p] == mv_px_struct_bg[p] &&
                    game_px[p + 1] == mv_px_struct_bg[p + 1] &&
                    game_px[p + 2] == mv_px_struct_bg[p + 2] &&
                    game_px[p + 3] == mv_px_struct_bg[p + 3]) {
                    continue;
                }
                for (int c = 0; c < 3; ++c) {
                    game_draw[p + c] = static_cast<std::uint8_t>(
                        (mv_px_struct_bg[p + c] * (255 - a) +
                         game_px[p + c] * a) / 255);
                }
                game_draw[p + 3] = 255;
            }
        }
        mv_struct_fade_alpha = a;
        mv_mark_dirty(b.x, b.y, b.x + b.w, b.y + b.h);
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
    // --- the Settings panel's controls ---------------------------------------
    // PlayModeCB's two items. hSetPlayMode (AoWEPACK 0x557e2028) stores the
    // value 1 for classic turns and 2 for simultaneous ones, and the refresh
    // (AoW.exe 0x00411460) picks the row with `Items.IndexOfObject(mode)`, so
    // the list is Classic (TObject(1)) then Simultaneous (TObject(2)). The
    // captions are the AoWE ClassicRStr / SimultaneousRStr resourcestrings.
    static int set_mode_value(int row) { return row == 0 ? 1 : 2; }
    static const char* set_mode_name(int mode) {
        return mode == 1 ? "Classic" : "Simultaneous";
    }
    // TurnTimeVal: FUN_00412778 - Position 0 reads the lowercase NoTimerRStr
    // ("no timer"), otherwise Format(XMinutesRStr = "%s minutes", Position).
    static std::string set_turn_time_text(int pos) {
        if (pos <= 0) return "no timer";
        return std::to_string(pos) + " minutes";
    }
    // TurnsVal: FUN_004128b4 - Position 0 reads the lowercase NoLimitRStr
    // ("no limit"), otherwise Format(lowercase(XTurnsRStr = "%s turns"),
    // Position * 10) - TurnsSlChange hands hSetTurnLimitation `Position * 10`.
    static std::string set_turns_text(int pos) {
        if (pos <= 0) return "no limit";
        return std::to_string(pos * 10) + " turns";
    }
    // PlayModeCB's dropped-down list (TAOWComboBox): NumberVisible 2 rows of
    // ItemHeight 13 directly under the 18 px field, the body the DFM's
    // ILTextureNormal GenericT 4 (blank1) and the hovered/selected line its
    // ILTextureSelected GenericT 16 (TextLBSel). Geometry mirrors scen_hit.
    AowBox set_mode_list_rect() const {
        return AowBox{kScenSetX + kSetModeX, kScenSetY + kSetModeY + kScenComboH,
                      kSetModeW, kSetModeRows * kSetModeItemH};
    }
    void draw_mode_list(Canvas& cv) const {
        const AowBox lr = set_mode_list_rect();
        if (scen_blank_tex.ok) {
            fill_tex(cv, scen_blank_tex, lr.x, lr.y, lr.w, lr.h);
        } else {
            cv.fill(lr.x, lr.y, lr.w, lr.h, 12, 12, 20, 255);
        }
        // The lit row: the hovered one, else the combo's own value (a
        // TAOWComboBox paints its selected line highlighted).
        const int hi = scen_mode_dd_hover >= 0
                           ? scen_mode_dd_hover
                           : (scen_play_mode == 1 ? 0 : 1);
        for (int i = 0; i < kSetModeRows; ++i) {
            const int ry = lr.y + i * kSetModeItemH;
            const bool hot = hi == i;
            if (hot && scen_lbsel_tex.ok) {
                const int tw = static_cast<int>(scen_lbsel_tex.w);
                const int th = static_cast<int>(scen_lbsel_tex.h);
                for (int oy = ry; oy < ry + kSetModeItemH; oy += th) {
                    for (int ox = lr.x; ox < lr.x + lr.w; ox += tw) {
                        cv.blit(scen_lbsel_tex, ox, oy, kAlphaFull, lr.x, ry,
                                lr.x + lr.w, ry + kSetModeItemH);
                    }
                }
            }
            // TextColor 15134446 (238,238,230); TextSelColor clWhite.
            const std::uint8_t tr = hot ? 255 : 238;
            const std::uint8_t tg = hot ? 255 : 238;
            const std::uint8_t tb = hot ? 255 : 230;
            text_at(cv, glyphs8, lr.x + 6, ry + (kSetModeItemH - 12) / 2,
                    set_mode_name(set_mode_value(i)), tr, tg, tb, false, false);
        }
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
    // `lit` paints the ILIndexLit face (49) - TAOWCheckBox's hover state.
    void draw_check(Canvas& cv, int x, int y, bool on, bool lit) const {
        const int which = lit ? 2 : (on ? 1 : 0);
        const Sprite& s = scen_ngcbox[which];
        if (s.ok) {
            cv.blit(s, x, y, kAlphaFull, 0, 0, disp_w, disp_h);
        } else if (scen_chk[on ? 1 : 0].ok) {
            const Sprite& f = scen_chk[on ? 1 : 0];
            cv.blit(f, x, y, kAlphaFull, 0, 0, disp_w, disp_h);
        }
    }
    // The thumb width (its three pieces) and the travel the engine allows it:
    // TAOWHScrollBar.SetSize makes +0x130 the track's middle width and Draw
    // slides the thumb over `middle - thumbMiddle`, i.e. the whole widget minus
    // the thumb - so at Position 0 the thumb sits on the left edge and at Max
    // its right edge is on the widget's right edge.
    int slider_thumb_w() const {
        return static_cast<int>(scen_slider[3].w) +
               static_cast<int>(scen_slider[4].w) +
               static_cast<int>(scen_slider[5].w);
    }
    int slider_thumb_x(int x, int w, int pos, int max) const {
        const int span = w - slider_thumb_w();
        if (span <= 0 || max <= 0) return x;
        if (pos < 0) pos = 0;
        if (pos > max) pos = max;
        return x + (span * pos + max / 2) / max;
    }
    // A TAOWHScrollBar (ILIndexBack = GenericS 49 ScenSlid 3-piece track,
    // ILIndexTab = GenericS 52 3-piece thumb), thumb placed from `pos / max`.
    void draw_slider(Canvas& cv, int x, int y, int w, int pos, int max) const {
        blit_btn3(cv, scen_slider, x, y, w, kSetSlH);      // track (49/50/51)
        const int tw = static_cast<int>(scen_slider[4].w);   // thumb middle
        const int lw = static_cast<int>(scen_slider[3].w);
        const int tx = slider_thumb_x(x, w, pos, max);
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
        // PlayModeCB shows the row its TSetupSettings+0x24 value maps to
        // (hSetPlayMode 1 = classic / 2 = simultaneous). Its caption is the
        // combo's TextColor 15134446 rendered over the grey ScenEBut field.
        draw_setcombo(cv, sx + kSetModeX, sy + kSetModeY, kSetModeW,
                      set_mode_name(scen_play_mode), glyphs8, 200, 200, 195);
        text_at(cv, glyphs8, sx + 10, sy + 55, "Duration", 192, 192, 192, false,
                false);
        draw_group(cv, sx + 10, sy + 70, 114, 65);
        text_at(cv, glyphs8, sx + 15, sy + 75, "Turn Time", 128, 128, 128, false,
                false);
        draw_slider(cv, sx + kSetSlX, sy + kSetTurnTimeSlY, kSetSlW,
                    scen_turn_time, kSetTurnTimeMax);
        text_at(cv, glyphs8, sx + kSetValX, sy + kSetTurnTimeValY,
                set_turn_time_text(scen_turn_time), 128, 128, 128, false, false);
        text_at(cv, glyphs8, sx + 15, sy + 105, "Turns", 128, 128, 128, false,
                false);
        draw_slider(cv, sx + kSetSlX, sy + kSetTurnsSlY, kSetSlW, scen_turns,
                    kSetTurnsMax);
        text_at(cv, glyphs8, sx + kSetValX, sy + kSetTurnsValY,
                set_turns_text(scen_turns), 128, 128, 128, false, false);
        text_at(cv, glyphs8, sx + 10, sy + 142, "Miscellaneous", 192, 192, 192,
                false, false);
        draw_group(cv, sx + 10, sy + 155, 114, 70);
        // The four checkboxes: NGCBox 47 (up) / 48 (down, i.e. checked) / 49
        // (lit) - TAOWCheckBox draws Lit while the pointer is over the box.
        static const char* const kSetChkText[4] = {
            "Leaders on map", "Customize leaders", "Allied victory",
            "Exploration"};
        for (int i = 0; i < 4; ++i) {
            const bool lit = scen_chk_hover == i || scen_chk_press == i;
            draw_check(cv, sx + kSetChkX, sy + kSetChkY[i], scen_check[i], lit);
            text_at(cv, glyphs8, sx + kSetChkLblX, sy + kSetChkLblY[i],
                    kSetChkText[i], 128, 128, 128, false, false);
        }

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
        // PlayModeCB's drop-down is an opened TAOWComboBox: it floats over the
        // controls below the field (PriorityOpened), so it is painted last.
        if (scen_mode_dd && !scen_maps_open) draw_mode_list(cv);

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
    // TFontModule's CharacterDistance (re/dfm_all/TFONTMODULE.txt): Age10 and
    // Age10s carry 1, Age8 / Age8s / AoW15WhiteGrey carry 0. TImageLibraryFont.
    // GetTextWidth/SetText add this distance to the image's +0x20 width for
    // every character. (Age8 must not be in the +1 group: its glyph slot is
    // already one pixel wider than the ink, and the Unit window's captions -
    // measured against bin/InfoMenuRef*.png - need the slot width unchanged.)
    int font_tracking(const std::vector<Glyph>& fs) const {
        if (&fs == &glyphs10b || &fs == &glyphs10) return 1;
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
                 int tracking = -1, bool key_black = false) const {
        // Honour an explicit CharacterDistance override when centring too: the
        // Unit window's Age8 captions pass 0 (see draw_game_unit_tabs), and a
        // centre that used the face's default advance would shift them left by
        // half the per-glyph difference.
        const int total = tracking < 0 ? text_w(fs, s)
                                       : text_w_track(fs, s, tracking);
        int pen = center ? x - total / 2 : x;
        for (unsigned char c : s) {
            const Glyph& gl = fs[c];
            if (gl.ok) {
                if (keyed) {
                    cv.blit_keyed(gl.face, pen, y);
                } else {
                    cv.glyph(gl.face, pen, y, r, g, b, key_black);
                }
            }
            int adv = glyph_advance(fs, c);
            if (tracking >= 0) adv += tracking - font_tracking(fs);
            pen += adv;
        }
    }

    // One gold / mana read-out: the stockpile total (FontModule.TBarNumber,
    // clBlack) right-aligned rox 23 on the panel, the sign (Age8) at rox 16 and
    // the **net per-turn value** (income - upkeep) centred in the 16 px box
    // inset rox 1. A non-negative value is the DFM's green TextColor 0x77FF77
    // with a "+"; a negative net (only costs, no gains) is red with a "-".
    void draw_readout(Canvas& cv, const AowBox& p, const std::string& total,
                      int net) const {
        const int tre = p.x + p.w - kTbResTotRox;
        text_at(cv, glyphsTb, tre - text_w(glyphsTb, total), p.y + kTbResTotTop,
                total, 0, 0, 0, false, false);
        const bool negative = net < 0;
        const char* sign = negative ? "-" : "+";
        const int r = negative ? kTbCostR : kTbIncomeR;
        const int g = negative ? kTbCostG : kTbIncomeG;
        const int b = negative ? kTbCostB : kTbIncomeB;
        char value[32];
        std::snprintf(value, sizeof(value), "%d", negative ? -net : net);
        const int sre = p.x + p.w - kTbResSignRox;
        text_at(cv, glyphs8b, sre - text_w(glyphs8b, sign), p.y + kTbResIncTop,
                sign, r, g, b, false, false);
        const int ice = p.x + p.w - kTbResIncRox - kTbResIncW / 2;
        text_at(cv, glyphs8b, ice, p.y + kTbResIncTop, value, r, g, b, true,
                false);
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
            scen_mode_dd = false; // ...and PlayModeCB's list (s.refresh)
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

    // hSetPlayMode (AoWEPACK 0x557e2028): store the mode for TSetupSettings
    // +0x24. Switching to simultaneous turns then walks the player list and
    // resets every slot after the first one that is not AI-controlled - in this
    // port's per-race model that leaves exactly the first Human race Human and
    // puts the others back to the map's default type (a default-human slot
    // becomes CPU1, the same rule scen_choose_type applies).
    void scen_set_play_mode(int mode) {
        if (mode != 1 && mode != 2) mode = 2;
        if (mode == scen_play_mode) return;
        scen_play_mode = mode;
        if (mode != 2) return;
        bool seen_human = false;
        for (int gi = 0; gi < kScenRaceCount; ++gi) {
            const int r = kScenGridRace[gi];
            if (!scen_available[r]) continue;
            if (scen_ptype[r] != static_cast<int>(aow::PlayerType::Human)) {
                continue;
            }
            if (!seen_human) {
                seen_human = true;
                continue;
            }
            scen_ptype[r] =
                (scen_ptype_default[r] ==
                 static_cast<int>(aow::PlayerType::Human))
                    ? static_cast<int>(aow::PlayerType::AI1)
                    : scen_ptype_default[r];
        }
    }

    // --- the two Duration sliders -------------------------------------------
    // TurnTimeSlChange (AoW.exe 0x0041285c) calls hSetTurnTimeLimitation with
    // TurnTimeSl.Position (0 = no timer, else minutes); TurnsSlChange
    // (0x004129a8) calls hSetTurnLimitation with Position * 10 (0 = no limit).
    void scen_slider_set(int which, int pos) {
        if (which == 0) {
            if (pos < 0) pos = 0;
            if (pos > kSetTurnTimeMax) pos = kSetTurnTimeMax;
            if (pos == scen_turn_time) return;
            scen_turn_time = pos;
            std::printf("scenario: turn time -> %s\n",
                        set_turn_time_text(pos).c_str());
        } else {
            if (pos < 0) pos = 0;
            if (pos > kSetTurnsMax) pos = kSetTurnsMax;
            if (pos == scen_turns) return;
            scen_turns = pos;
            std::printf("scenario: turn limit -> %s\n",
                        set_turns_text(pos).c_str());
        }
        std::fflush(stdout);
    }
    int scen_slider_value(int which) const {
        return which == 0 ? scen_turn_time : scen_turns;
    }
    int scen_slider_max(int which) const {
        return which == 0 ? kSetTurnTimeMax : kSetTurnsMax;
    }
    // TAOWHScrollBar.CheckMouseDown (aowInt.dpl 0x5981f4d0): a press left of
    // the thumb steps Position down by SmallChange, right of it up by
    // SmallChange (the DFM sets both Small/LargeChange to 1), and a press on
    // the thumb arms the drag (+0x154 = the offset inside it).
    void scen_slider_down(int which, int mx) {
        const int pos = scen_slider_value(which);
        const int x0 = kScenSetX + kSetSlX;
        const int tx = slider_thumb_x(x0, kSetSlW, pos, scen_slider_max(which));
        if (mx > tx + slider_thumb_w()) {
            scen_slider_set(which, pos + 1);
        } else if (mx < tx) {
            scen_slider_set(which, pos - 1);
        } else {
            scen_drag_slider = which;
            scen_drag_grab = mx - tx;
        }
    }
    // CheckMouseMove (0x5981fa5c): Position = ((x - grab) * Range) /
    // trackMiddle + Min, clamped to Min..Max+1-NumberVisible. The port maps the
    // pointer over the same travel the thumb is drawn on, so the thumb follows
    // the pointer exactly.
    void scen_slider_move(int mx) {
        if (scen_drag_slider < 0) return;
        const int which = scen_drag_slider;
        const int max = scen_slider_max(which);
        const int span = kSetSlW - slider_thumb_w();
        if (span <= 0) return;
        // `span * pos / max` is how slider_thumb_x() places the thumb, so this
        // is its exact inverse: the thumb stays under the pointer.
        int pos = ((mx - scen_drag_grab - (kScenSetX + kSetSlX)) * max +
                   span / 2) /
                  span;
        scen_slider_set(which, pos);
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
            // --- the Settings panel's live controls --------------------------
            // A dropped-down PlayModeCB list swallows the click first, exactly
            // like the race combos: a row selects the mode, anything else only
            // closes the list.
            if (scen_mode_dd) {
                const AowBox lr = set_mode_list_rect();
                if (lr.x <= x && x < lr.x + lr.w && lr.y <= y &&
                    y < lr.y + lr.h) {
                    return -5100 - (y - lr.y) / kSetModeItemH;
                }
                return -5200;
            }
            // PlayModeCB's closed field (10,25) 114x18.
            if (kScenSetX + kSetModeX <= x &&
                x < kScenSetX + kSetModeX + kSetModeW &&
                kScenSetY + kSetModeY <= y &&
                y < kScenSetY + kSetModeY + kScenComboH) {
                return -5000;
            }
            // The two TAOWHScrollBars: -5010 TurnTimeSl, -5011 TurnsSl.
            const int slx = kScenSetX + kSetSlX;
            const int slw = kSetSlW, slh = kSetSlH;
            if (slx <= x && x < slx + slw) {
                const int tty = kScenSetY + kSetTurnTimeSlY;
                if (tty <= y && y < tty + slh) return -5010;
                const int tsy = kScenSetY + kSetTurnsSlY;
                if (tsy <= y && y < tsy + slh) return -5011;
            }
            // The four TAOWCheckBoxes (12x12 - their labels are separate
            // TAOWLabels and do not toggle the box): -5020..-5023.
            for (int i = 0; i < 4; ++i) {
                const int cx = kScenSetX + kSetChkX;
                const int cy = kScenSetY + kSetChkY[i];
                if (cx <= x && x < cx + kSetChkSize && cy <= y &&
                    y < cy + kSetChkSize) {
                    return -5020 - i;
                }
            }
        }
        return -1;
    }
    // The checkbox index under (x, y), or -1.
    int scen_chk_at(int x, int y) const {
        for (int i = 0; i < 4; ++i) {
            const int cx = kScenSetX + kSetChkX;
            const int cy = kScenSetY + kSetChkY[i];
            if (cx <= x && x < cx + kSetChkSize && cy <= y &&
                y < cy + kSetChkSize) {
                return i;
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
                    // A plain move's `PlaceOnMap` (the 0x1102 ArmyPlaced event,
                    // which claims an undefended building) runs now too, so the
                    // structure kept its old owner for the whole transition.
                    if (mv_merge_pending) {
                        mv_finish_merge();
                    } else {
                        mv_finish_arrival();
                    }
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
        // The Party floater's own hover: its title-bar and foot buttons light
        // up while the pointer is over them (TAOWBaseButton's ILIndexLit).
        // TUnitWindow is Masked/Priority 300 and modal: while it is up it owns
        // the pointer and the party below takes no hover.
        unitwin_hover =
            game_show_unit ? unit_window_hit_test(mouse_x, mouse_y) : -1;
        party_hover = (game_show_party && !game_show_unit)
                          ? party_hit_test(mouse_x, mouse_y)
                          : -1;
        game_modal_open = game_show_unit || game_show_city;
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
        if (a != nullptr) {
            if (mv_human == 0xff) return 1;
            return a->owner == mv_human ? 3 : 1;
        }
        // `TAoWMapField.GetMoveAction@55771ddc` sends message 0x1101 to every
        // sprite on the field, so a structure answers too. The port used to
        // check armies only, which left the crossed-swords pointer off an
        // enemy/independent city. `TPlayerStructure.GetMoveAction@55761620` /
        // `TCity.GetMoveAction@557ad0d4` decide (a neutral non-city answers 0:
        // walking in claims it).
        if (const aow::MapHs* s = structure_on_hex(x, y)) {
            if (mv_human != 0xff &&
                aow::is_player_structure_class(s->class_id) &&
                aow::structure_move_action(*s, mv_human) == 1) {
                return 1;
            }
        }
        return 0;
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
            // The arriving party is placed on the hex too (TArmyHS.PlaceOnMap
            // runs for the merged selection), so an unowned/contested structure
            // under the merged army updates its owner and a gold pile there is
            // collected (`Gems.TGems.MapFieldMsgProc` answers the same 0x1102).
            game_gems_army_update(dest_x, dest_y);
            game_structure_army_update(dest_x, dest_y, true);
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

    // `TMoveArmyTE.FinishMove@0x55747fbc` on a plain (non-merge) walk: the
    // sprite has reached the destination, so the army is *placed* now and the
    // field's 0x1102 message (`TStructure.MapFieldMsgProc@5575f730` ->
    // `TPlayerStructure.ArmyPlaced` -> `UpdatePlayer@55760980`) runs - an
    // undefended, unowned (or independent) structure under the army changes
    // hands at this moment, not when the move was ordered. Runs from
    // `game_update`'s walk tick when the animation finishes, or straight away
    // when the path is degenerate (no walk at all).
    void mv_finish_arrival() {
        if (!mv_arrive_pending) return;
        mv_arrive_pending = false;
        const int x = mv_arrive_x, y = mv_arrive_y;
        mv_arrive_x = mv_arrive_y = -1;
        // The field's 0x1102 reaches every object on the hex: a gold pile is
        // destroyed and paid out (`Gems.TGems.ArmyEntered`), then the ownable
        // structures re-evaluate their owner (`TPlayerStructure.UpdatePlayer`).
        game_gems_army_update(x, y);
        game_structure_army_update(x, y, true);
    }

    // `Gems.TGems.MapFieldMsgProc@557c52a0` answering the field's 0x1102
    // (ArmyPlaced) message: every gold pile on the hex the placed army stands on
    // is collected (`Gems.TGems.ArmyEntered@557c50f8`). The original adds the
    // pile's amount to the army owner's treasury (`TPlayer.SetGems`), writes a
    // `TLocationTextEventLog` with `AoWE.GoldFoundRStr` ("%s gold pieces found")
    // at the hex, plays `SFX\EVENTS\GOLD.WAV` when the seated player is that
    // owner or can see the hex (`TAoWHSMap.Watching`, radius 0 here), and then
    // destroys the object. Returns true when a pile was collected.
    bool game_gems_army_update(int x, int y) {
        if (!mv_ready || game_level < 0 || game_state_map.levels.empty() ||
            mv_human == 0xff) {
            return false;
        }
        const std::size_t level = static_cast<std::size_t>(game_level);
        const aow::GoldPickup pick = aow::gems_army_entered(
            game_state_map, mv_gs, level, x, y, static_cast<int>(mv_human));
        if (!pick.picked()) return false;
        // `TAoWHSMap.Watching@55775b88` at radius 0: the hex is watched when the
        // army belongs to the seated player, or the seated player's explored bit
        // is set on it (`TAoWMapField.GetExplored@55771bf4`).
        bool watching = pick.player == static_cast<int>(mv_human);
        if (!watching) {
            const aow::MapLevel& lv = game_state_map.levels[level];
            if (x >= 0 && y >= 0 &&
                static_cast<std::uint32_t>(x) < lv.width &&
                static_cast<std::uint32_t>(y) < lv.height) {
                const aow::MapTile& tile =
                    lv.tiles[static_cast<std::size_t>(y) * lv.width +
                             static_cast<std::size_t>(x)];
                watching = (tile.explored & (1u << (mv_human & 0x1f))) != 0;
            }
        }
        if (watching) play_game_sfx("SFX\\EVENTS\\GOLD.WAV");
        const aow::Player* p = game_player_by_index(
            mv_gs, static_cast<std::uint8_t>(pick.player));
        for (int amount : pick.amounts) {
            std::printf("game_gold=%d,%d player=%d amount=%d total=%d gold=%d\n",
                        x, y, pick.player, amount, pick.total(),
                        p ? p->gold : 0);
            // The event logbook entry is shown to the player it belongs to.
            if (pick.player == static_cast<int>(mv_human)) {
                game_say(std::to_string(amount) + " gold pieces found");
            }
        }
        std::fflush(stdout);
        // The pile's object is gone; the caller re-renders the level (the walk
        // tick / the degenerate move path both call mv_render_level right after
        // the finished move, exactly where the original destroys the object).
        return true;
    }

    // `TStructure.MapFieldMsgProc@5575f730` message 0x1102 -> the structure's
    // `ArmyPlaced` (+0x174) -> `TPlayerStructure.UpdatePlayer@55760980`: after a
    // party is placed on (or lifted from) a hex, every ownable structure covering
    // that hex looks at the armies standing on its own hexes and takes the sole
    // owner (owner -1 when two players' armies share a multi-hex city). This is
    // how walking onto an unowned mine claims it, without a battle.
    bool game_structure_army_update(int x, int y, bool arriving = false) {
        if (!mv_ready || game_level < 0 || game_state_map.levels.empty()) {
            return false;
        }
        const std::vector<aow::StructureRef> refs = aow::structures_at(
            game_state_map, static_cast<std::size_t>(game_level), x, y, game_res);
        if (refs.empty()) return false;
        const std::vector<std::uint8_t> races =
            aow::map_player_races(game_state_map);
        bool changed = false;
        std::string text;
        for (const aow::StructureRef& r : refs) {
            // With no combat model the port must not run the ArmyPlaced pass
            // while a defender still stands on the building: the original
            // resolves the battle (and removes the defender) before the winner
            // is placed, so only the winner ever reaches UpdatePlayer.
            if (arriving &&
                aow::structure_has_foreign_army(game_state_map, r, mv_human,
                                                game_res)) {
                continue;
            }
            const aow::StructureChange ch = aow::structure_update_player(
                game_state_map, r, races, game_res);
            if (!ch.changed) continue;
            changed = true;
            if (!ch.text.empty()) text = ch.text;
            const aow::MapHs* hs = aow::structure_object(game_state_map, r);
            std::printf("game_structure_owner=%d,%d owner=%d old=%d flag=0x%x\n",
                        x, y,
                        hs ? static_cast<int>(
                                 static_cast<std::int8_t>(hs->structure_owner))
                           : -1,
                        ch.old_owner, hs ? static_cast<int>(hs->flag_id) : 0);
        }
        if (!changed) return false;
        // SetPlayer re-registers income/power sources; the port recomputes the
        // whole economy from the map (docs/28_ECONOMY.md).
        aow::compute_economy(game_state_map, game_res, &mv_cat, mv_gs);
        if (game_show_structure) open_structure(x, y);
        if (!text.empty()) game_say(text);
        std::printf("game_capture=%d,%d player=%d\n", x, y,
                    static_cast<int>(mv_human));
        std::fflush(stdout);
        return true;
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
        // `TArmyHS.PlaceOnMap`/the army removal path tell the field's structures
        // that the party on the source hex changed (TCity.ArmyRemoved ->
        // UpdatePlayer); a city two players now share becomes contested.
        game_structure_army_update(src_x, src_y);
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
            // The army's *data* reaches the hex now (the resting copy is hidden
            // while its sprite walks), but the field's 0x1102 (ArmyPlaced) event
            // belongs to `TMoveArmyTE.FinishMove`, which runs when the walk
            // arrives: an undefended/unowned structure under the army therefore
            // keeps its old owner and flag for the whole transition and is only
            // claimed on arrival (`mv_finish_arrival`, from game_update's walk
            // tick). The level rendered below is drawn without the arriving
            // army and with the building still on its previous owner.
            mv_arrive_pending = true;
            mv_arrive_x = dest_x;
            mv_arrive_y = dest_y;
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
                mv_finish_arrival();  // no walk: the army has arrived at once
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
            // `open_structure` fills game_struct_name from the map (a city's
            // TCity rw tag 0xc name), so the head-less click log shows exactly
            // what the floater's StructNameLbl will read.
            game_say("selected structure (" + std::to_string(game_sel_hx) + "," +
                     std::to_string(game_sel_hy) + ") name=[" + game_struct_name +
                     "]");
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
        // `UnitWinKeyDown@0x40abd4`: VK_ESCAPE (0x1b) hides the Unit window.
        if (game_show_unit && vk == 0x1b) {
            unit_window_close();
            return true;
        }
        if (game_show_unit) return true;  // modal: no map shortcuts
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

    // -------------------------------------------- the Unit info window data
    // `TAbstractUnit.GetAbilityType` (the subclass override; the base returns
    // 0) picks the TAbilityTypeRStr caption. The shipped captions are Movement
    // / Combat / Magic / Enchantment / Secret. The two abilities the party
    // reference shows are Walking (0 -> Movement) and Fire Musket (45 ->
    // Combat), and the groups below reproduce the concrete ability classes'
    // overrides (movement abilities, combat strikes/shots, enchantments and
    // status effects).
    static const char* unit_ability_type(int id) {
        if (id >= 0 && id <= 5) return kAbilityTypeMovement;      // walk/fly/swim
        if (id == 23 || id == 59 || id == 66) return kAbilityTypeMovement;
        if ((id >= 14 && id <= 45) || (id >= 86 && id <= 90) ||
            (id >= 111 && id <= 122) || id == 57 || id == 58 || id == 61 ||
            id == 62 || id == 65) {
            return kAbilityTypeCombat;
        }
        if ((id >= 46 && id <= 56) || (id >= 68 && id <= 77) || id == 99 ||
            id == 100 || id == 101 || id == 109) {
            return kAbilityTypeMagic;
        }
        return kAbilityTypeSecret;
    }

    // The Unitres record behind a party unit. Heroes use their own runtime
    // fields instead (the map stores a THeroResource index, not a Unitres tag).
    const aow::UnitEntry* unit_window_entry(const aow::MapUnit& u) const {
        if (u.is_hero) return nullptr;
        return city_unit_entry(u.resource);
    }

    // `MoraleValueToMorale@0x5575ad64`: <21 -> 0, 21..40 -> 1, 41..60 -> 2,
    // 61..80 -> 3, >=81 -> 4 (Terrible .. High).
    static int unit_morale_index(std::uint8_t value) {
        if (value < 21) return 0;
        if (value < 41) return 1;
        if (value < 61) return 2;
        if (value < 81) return 3;
        return 4;
    }

    // `FUN_00407db8`: resolve the selected unit into every control's content.
    // For a TUnit the name/stats/description/abilities come from the Unitres
    // record joined to the runtime ability mask; a hero falls back to the
    // map object's own name/level/experience.
    void unit_window_refresh() {
        UnitWindowData d;
        d.slot = game_unit_slot;
        const aow::MapUnit* mu = party_unit(game_unit_slot);
        if (mu == nullptr) {
            unit_win = UnitWindowData();
            game_show_unit = false;
            return;
        }
        const aow::UnitEntry* e = unit_window_entry(*mu);
        d.valid = true;
        d.is_hero = mu->is_hero;
        if (e != nullptr) {
            d.name = e->display_name.empty()
                         ? ("Unit " + std::to_string(e->id))
                         : e->display_name;
            d.description = e->description;
            if (e->unit_type == 1) {
                d.type = "Creature";
                d.type_icon = 0;   // UnitWin 21
            } else if (e->unit_type == 2) {
                d.type = "Machine";
                d.type_icon = 1;   // UnitWin 22
            } else {
                d.type = "Unit";
                d.type_icon = 2;   // UnitWin 23
            }
            const int a = e->alignment & 0x7f;
            d.alignment = (a >= 0 && a < 7) ? kUnitAlignmentText[a] : "Neutral";
            // AAlignment: 0 -> 31, 1 -> 32, 2/3 -> 33, 4 -> 34, 5 -> 35;
            // alignment 6 is "None" and the control is hidden.
            static const int kAlignIcon[7] = {0, 1, 2, 2, 3, 4, 0};
            d.align_icon = (a >= 0 && a < 7) ? kAlignIcon[a] : 2;
            d.level = e->level;
            d.attack = static_cast<int>(e->attack);
            const int morale = unit_morale_index(mu->morale);
            // TAbstractUnit.GetDefense/GetResistance morale modifiers
            // (MoraleDefenseModifier / MoraleResistanceModifier, docs/10_UNIT.md).
            static const int kMoraleDef[5] = {-2, -1, 0, 0, 0};
            static const int kMoraleRes[5] = {-2, -1, 0, 0, 0};
            const int def = static_cast<int>(e->defense) + kMoraleDef[morale];
            const int res = static_cast<int>(e->resistance) + kMoraleRes[morale];
            d.defense = std::max(1, std::min(10, def));
            d.damage = static_cast<int>(e->damage);
            d.resistance = std::max(1, std::min(10, res));
            d.max_hits = e->hit_points;
            d.hits = e->hit_points;
            d.max_moves = e->moves;
            d.moves = std::max(
                0, static_cast<int>(aow::unit_move_points_left(mv_cat, *mu)));
            const std::array<bool, 256> has =
                aow::decode_ability_bitmap(mu->ability_mask, mu->ability_count);
            d.upkeep = has[0x9b] ? 0 : (e->level + 1) * 2;
        }
        if (e != nullptr) {
            // The ability list comes from the runtime ability mask (the map
            // stores it per unit); a map that omitted it falls back to the
            // resource list.
            const std::vector<std::uint8_t>& mask =
                mu->ability_mask.empty() ? e->ability_mask : mu->ability_mask;
            const std::uint32_t count =
                mu->ability_mask.empty() ? e->ability_count : mu->ability_count;
            const std::array<bool, 256> bits =
                aow::decode_ability_bitmap(mask, count);
            for (int i = 0; i < static_cast<int>(count) && i < 256; ++i) {
                if (!bits[static_cast<std::size_t>(i)]) continue;
                const char* name = city_ability_name(i);
                if (name == nullptr) continue;
                const int lvl = static_cast<std::size_t>(i) <
                                        e->ability_levels.size()
                                    ? e->ability_levels[static_cast<std::size_t>(i)]
                                    : 0;
                UnitWindowAbility row;
                if (i == 50 && lvl > 0) {
                    row.name = "Transport (" + std::to_string(lvl) + " units)";
                } else {
                    row.name = std::string(name) +
                               city_ability_level_suffix(i, lvl);
                }
                row.type = unit_ability_type(i);
                row.source = kAbilitySourceInherent;
                d.abilities.push_back(std::move(row));
            }
        } else {
            // A hero: `THero.GetName` / `GetLevel` / `GetExperience`; no
            // Unitres stats are available here, so show the leader-style name
            // and the map object's own level.
            d.name = mu->name.empty() ? "Hero" : mu->name;
            if (!mu->nickname.empty()) d.name += " " + mu->nickname;
            d.type = mu->class_id == aow::kMapLeaderClassId ? "Leader" : "Hero";
            d.type_icon = 2;
            d.alignment = "Neutral";
            d.align_icon = 2;
            d.level = mu->unit_level;
            d.experience = static_cast<int>(mu->experience);
            // THeroResource's skill block (Herores.pfs tags 15..20). A hero's
            // map object carries the hero-resource index in `resource`.
            const aow::HeroStats* hs =
                mu->resource >= 0
                    ? mv_cat.find_hero_stats(static_cast<std::uint32_t>(mu->resource))
                    : nullptr;
            if (hs != nullptr) {
                d.attack = hs->attack;
                d.defense = hs->defense;
                d.damage = hs->damage;
                d.hits = hs->hits;
                d.max_hits = hs->hits;
                d.moves = hs->moves;
                d.max_moves = hs->moves;
                d.resistance = hs->resistance;
            } else {
                d.attack = d.defense = d.damage = d.resistance = 0;
                d.hits = d.max_hits = 1;
                d.moves = d.max_moves = 0;
            }
            d.upkeep = (d.level + 1) * 2;
            const std::array<bool, 256> bits =
                aow::decode_ability_bitmap(mu->ability_mask,
                                           mu->ability_count);
            for (int i = 0; i < static_cast<int>(mu->ability_count) && i < 256;
                 ++i) {
                if (!bits[static_cast<std::size_t>(i)]) continue;
                const char* name = city_ability_name(i);
                if (name == nullptr) continue;
                UnitWindowAbility row;
                row.name = name;
                row.type = unit_ability_type(i);
                row.source = kAbilitySourceInherent;
                d.abilities.push_back(std::move(row));
            }
        }
        if (d.max_moves == 0) d.max_moves = d.moves;
        d.next_level_experience = std::max(1, d.level * 2);
        // `GetMorale`: hidden for Machines (GetUnitType == 2).
        d.show_morale = (e == nullptr || e->unit_type != 2);
        const int mi = unit_morale_index(mu->morale);
        d.morale = std::string("Morale: ") +
                   kUnitMoraleText[std::max(0, std::min(4, mi))];
        // `TRace.GetPlayerRelation` for the party's own race is Friendly; the
        // port has no diplomatic graph yet, so a party unit always answers the
        // 4th TStatusRStr band (the reference's blue Friendly face, RelWin 14).
        d.relation = kUnitRelationText[3];
        d.relation_icon = 3;
        d.show_relation = true;
        d.show_type = true;
        // ListAbilityInfo's `Engine.SortStringsEx` sorts the three columns by
        // the ability name; keep the type/source columns aligned.
        std::stable_sort(d.abilities.begin(), d.abilities.end(),
                         [](const UnitWindowAbility& a,
                            const UnitWindowAbility& b) {
                             return a.name < b.name;
                         });
        d.move_icon = party_icon_frame(aow::unit_move_types(*mu));
        unit_win = std::move(d);
        unitwin_abil_sel = -1;
        unitwin_abil_top = 0;
        unit_face_refresh();
        unit_sprite_refresh();
    }

    // The left portrait (`TAbstractUnit.ShowFace`: the unit-gfx record's face
    // library/index, stretched into the left frame).
    void unit_face_refresh() {
        const aow::MapUnit* mu = party_unit(game_unit_slot);
        const aow::UnitEntry* e = mu != nullptr ? unit_window_entry(*mu) : nullptr;
        const int key = e != nullptr ? static_cast<int>(e->id) : -1;
        if (unitwin_face_key == key && (key < 0 || unitwin_face.ok)) return;
        unitwin_face_key = key;
        unitwin_face = Sprite();
        if (e == nullptr) {
            // A hero/leader portrait is a FACERES.PFS record (THero+0x3c),
            // resolved through TPlayer.GetFace -> TFaceResource.GetFaceImage,
            // the same portrait the Events banner loads.
            if (mu != nullptr && mu->is_hero && game_load_face(mu->face_index) &&
                game_face.ok) {
                unitwin_face = game_face;
            }
            return;
        }
        if (e->gfx == nullptr) return;
        const std::string path = face_lib_path(e->gfx->image_lib);
        if (path.empty()) return;
        if (unitwin_face_path != path) {
            std::string lerr;
            if (!unitwin_face_il.load(path, lerr)) return;
            unitwin_face_path = path;
        }
        const std::vector<aow::IlbEntry>& ents = unitwin_face_il.entries();
        for (std::size_t i = 0; i < ents.size(); ++i) {
            if (ents[i].index != e->gfx->face_index) continue;
            std::uint32_t w = 0, h = 0;
            std::string derr;
            if (!unitwin_face_il.decode_rgb(i, unitwin_face.rgba, w, h, derr)) {
                return;
            }
            unitwin_face.w = w;
            unitwin_face.h = h;
            unitwin_face.ok = true;
            unitwin_face.base_x = 0;
            unitwin_face.base_y = 0;
            unitwin_face.has_key = false;
            return;
        }
    }

    // The right frame (`TAbstractUnit.ShowUnit`): the unit's standing pose,
    // the same 64x64 frame the party cell shows.
    void unit_sprite_refresh() {
        const aow::MapUnit* mu = party_unit(game_unit_slot);
        const aow::MapHs* a = party_army();
        const int key = mu != nullptr
                            ? static_cast<int>(game_unit_slot) * 100000 +
                                  (mu->id + 1)
                            : -1;
        if (unitwin_sprite_key == key && (key < 0 || unitwin_sprite.ok)) return;
        unitwin_sprite_key = key;
        unitwin_sprite = Sprite();
        unitwin_sprite_vx = unitwin_sprite_vy = 0;
        if (mu == nullptr || a == nullptr) return;
        std::vector<std::uint8_t> rgba;
        int w = 0, h = 0, vx = 0, vy = 0;
        std::string err;
        // `FrRShPreDraw` (0x0040a2a0) calls the unit's `ShowUnit` at
        // (FrRSh.Left + 0x31, FrRSh.Top + 0x4e). ShowUnit is a plain
        // TImageLibrary show of the pose - the frame's own base+shift only,
        // *without* the map sequence's xoff/yoff - which is exactly
        // `unit_pose_sprite_rgba` (the helper the city cards use).
        if (aow::unit_pose_sprite_rgba(mv_cat, root_dir, *mu,
                                       a->army_direction, 0, 0, rgba, w, h, vx,
                                       vy, err)) {
            unitwin_sprite.rgba = std::move(rgba);
            unitwin_sprite.w = static_cast<std::uint32_t>(w);
            unitwin_sprite.h = static_cast<std::uint32_t>(h);
            unitwin_sprite.ok = true;
            unitwin_sprite_vx = vx;
            unitwin_sprite_vy = vy;
        }
    }

    // `Unit1FrameMouseDown@0x406040`: a portrait click files that slot's unit
    // and brings the Unit window up.
    void unit_window_open(int slot) {
        if (slot < 0 || slot >= kPartySlots) return;
        game_unit_slot = slot;
        game_unit_tab = 0;
        unitwin_hover = unitwin_pressed = -1;
        unitwin_memo_top = 0;
        unitwin_disband_armed = false;
        unit_window_refresh();
        if (!unit_win.valid) return;
        game_show_unit = true;
        std::printf(
            "game_unit=open slot=%d name=%s lvl=%d atk=%d def=%d dmg=%d "
            "res=%d hits=%d/%d moves=%d/%d upkeep=%d abil=%d morale=%s\n",
            slot + 1, unit_win.name.c_str(), unit_win.level, unit_win.attack,
            unit_win.defense, unit_win.damage, unit_win.resistance,
            unit_win.hits, unit_win.max_hits, unit_win.moves,
            unit_win.max_moves, unit_win.upkeep,
            static_cast<int>(unit_win.abilities.size()), unit_win.morale.c_str());
        std::fflush(stdout);
    }

    void unit_window_close() {
        if (game_show_unit) {
            std::printf("game_unit=close\n");
            std::fflush(stdout);
        }
        game_show_unit = false;
        unitwin_disband_armed = false;
        unitwin_hover = unitwin_pressed = -1;
    }

    // `PPrevClick` / `PNextClick`: step to the previous/next occupied slot of
    // the party (DFM tooltip "Selects the previous/next unit in the party").
    void unit_window_step(int delta) {
        if (party_army() == nullptr || delta == 0) return;
        for (int step = 1; step <= kPartySlots; ++step) {
            int s = (game_unit_slot + delta * step) % kPartySlots;
            if (s < 0) s += kPartySlots;
            if (party_unit(s) != nullptr) {
                game_unit_slot = s;
                unitwin_abil_sel = -1;
                unitwin_abil_top = 0;
                unitwin_memo_top = 0;
                unit_window_refresh();
                return;
            }
        }
    }

    // `DisbandBtnClick@0x40aaa8`: the original asks "Disband this unit?" and
    // on confirmation removes the unit from the army. The port performs the
    // removal on the (already requested) click; the message dialog is not
    // reproduced yet.
    void unit_window_disband() {
        // The army object lives in game_state_map; party_army() is the read-only
        // accessor the renderer uses, so this mutating handler takes it back.
        aow::MapHs* a = const_cast<aow::MapHs*>(party_army());
        if (a == nullptr || game_unit_slot < 0 ||
            static_cast<std::size_t>(game_unit_slot) >= a->units.size()) {
            return;
        }
        const std::string name = unit_win.name;
        const int erased = game_unit_slot;
        const std::uint8_t old_mask = party_sel;
        a->units.erase(a->units.begin() + erased);
        // The slots after the erased one shift down; keep each remaining
        // unit's own travelling/left-behind state.
        std::uint8_t mask = 0;
        for (int i = 0;
             i < static_cast<int>(a->units.size()) && i < kPartySlots; ++i) {
            const int old = i < erased ? i : i + 1;
            if (old < kPartySlots && (old_mask & (1u << old)) != 0) {
                mask |= static_cast<std::uint8_t>(1u << i);
            }
        }
        party_sel = mask;
        party_refresh();
        mv_contents_stale = true;
        game_update_path();
        unit_window_close();
        if (a->units.empty()) {
            game_show_party = false;
            mv_reset();
        } else {
            game_unit_slot = std::min<int>(
                game_unit_slot, static_cast<int>(a->units.size()) - 1);
            unit_window_refresh();
            game_show_unit = true;
        }
        std::printf("game_unit=disband name=%s left=%d\n", name.c_str(),
                    static_cast<int>(a->units.size()));
        std::fflush(stdout);
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
                // `Unit1FrameMouseDown@0x406040`: file the clicked unit into
                // the world-map selection and call FUN_00407db8, which shows
                // the Unit info window for it.
                game_sel_hx = mv_sel_x;
                game_sel_hy = mv_sel_y;
                unit_window_open(s);
                game_say("party focus unit " + std::to_string(s + 1));
            }
            return;
        }
        switch (hit) {
        case kPartyClose:
        case kPartyMin:
            // Closing the party window deselects the current party
            // (`mld_strings.txt`, "The Party Floater").
            unit_window_close();
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

    // -------------------------------------------- the Unit window layout
    // The DFM's design boxes, translated by the floater's origin. UnitWin is a
    // fixed 420x417 window (Width/HeightPercent 0), so every child keeps its
    // WinLeft/WinTop/WinWidth/WinHeight; the only reflow is `UnitMainPnl`'s
    // awBoth width (420 - 21 - 21 = 378) - exactly the design size.
    struct UnitWindowLayout {
        AowBox win, title_icon, close, main, left_frame, left_shade;
        AowBox stat_pnl, stat_icon[7], stat_val[7];
        AowBox right_body, right_frame, right_shade;
        AowBox name, morale, level, xp, line1, line2;
        AowBox tab[2];
        AowBox t1, t2, list_pnl, list_name, list_type, list_src;
        AowBox header[3];
        AowBox memo, memo_body;
        AowBox type_pnl, type_icon, type_lbl, type_val;
        AowBox align_pnl, align_icon, align_lbl, align_val;
        AowBox rel_pnl, rel_icon, rel_lbl, rel_val;
        AowBox memo_sb;              // T2SBar (T2Text + 349,0) 17x86
        AowBox pprev, pnext;         // CampaignTrans arrows, topmost
        AowBox disband, bclose, celt_l, celt_r, celt_line;
        int tab_h = 18;
        int header_h = 14;
        int button_h = 27;
    };

    UnitWindowLayout unit_window_layout() const {
        UnitWindowLayout L;
        const int x0 = (disp_w - kUnitWinW) / 2;
        // awCenter/ahCenter rounds the half-pixel up: at 640x480 the 420x417
        // floater's top is 32. The reference captures include the one row of
        // backdrop above it, so their frame starts at image row 1.
        const int y0 = (disp_h - kUnitWinH + 1) / 2;
        L.win = AowBox{x0, y0, kUnitWinW, kUnitWinH};
        L.title_icon = AowBox{x0 + 20, y0 + 5, 19, 16};
        L.close = AowBox{x0 + 385, y0 + 4, 15, 16};
        L.main = AowBox{x0 + 21, y0 + 22, 378, 143};
        L.left_frame = AowBox{L.main.x, L.main.y, 118, 143};
        L.left_shade = AowBox{L.main.x + 1, L.main.y, 118, 143};
        L.right_body = AowBox{L.main.x + 260, L.main.y, 118, 143};
        L.right_frame = L.right_body;
        L.right_shade = AowBox{L.main.x + 260, L.main.y, 118, 143};
        L.name = AowBox{L.main.x + 189, L.main.y + 8, 0, 0};
        L.morale = AowBox{L.main.x + 189, L.main.y + 136, 0, 0};
        L.line1 = AowBox{L.main.x + 118, L.main.y, 142, 27};
        L.line2 = AowBox{L.main.x + 118, L.main.y + 116, 142, 27};
        L.stat_pnl = AowBox{L.main.x + 117, L.main.y + 28, 144, 86};
        const int sx = L.stat_pnl.x, sy = L.stat_pnl.y;
        // UAttackIcon / UDefenseIcon sit two pixels right of the DFM box in the
        // shipped window (their sprite shift is not folded into the control
        // origin); the defence one is another two rows down.
        L.stat_icon[0] = AowBox{sx + 7, sy + 3, 15, 17};   // attack
        L.stat_icon[1] = AowBox{sx + 7, sy + 26, 15, 17};  // defense
        L.stat_icon[2] = AowBox{sx + 0, sy + 45, 32, 17};  // moves
        L.stat_icon[3] = AowBox{sx + 74, sy + 1, 15, 17};  // damage
        L.stat_icon[4] = AowBox{sx + 74, sy + 22, 15, 17}; // resistance
        L.stat_icon[5] = AowBox{sx + 74, sy + 45, 15, 17}; // hits
        L.stat_icon[6] = AowBox{sx + 5, sy + 65, 19, 21};  // upkeep
        // UAttack/UDefense/UMoves/UpKeep are UStatP1 children at x=22 (P1
        // starts at +28); UDamage/UResistance/UHits are UStatP2 children at
        // x=25 (P2 starts at +93).
        L.stat_val[0] = AowBox{sx + 50, sy + 3, 0, 0};
        L.stat_val[1] = AowBox{sx + 50, sy + 25, 0, 0};
        L.stat_val[2] = AowBox{sx + 50, sy + 47, 0, 0};
        L.stat_val[3] = AowBox{sx + 118, sy + 3, 0, 0};
        L.stat_val[4] = AowBox{sx + 118, sy + 25, 0, 0};
        L.stat_val[5] = AowBox{sx + 118, sy + 47, 0, 0};
        L.stat_val[6] = AowBox{sx + 50, sy + 68, 0, 0};
        // Level/XP are awCenter labels; the capture centres them 13 px right
        // of the first replica's centre (right-frame + 63).
        L.level = AowBox{L.right_body.x + 63, L.right_body.y + 18, 0, 0};
        L.xp = AowBox{L.right_body.x + 63, L.right_body.y + 30, 0, 0};
        // UnitTab is 379 wide with two tabs: SetSize makes the pitch
        // (379/2 = 189) px, so tab 1 starts at 21 + 189 = 210, not at 190.
        L.tab[0] = AowBox{x0 + 21, y0 + 163, 189, 21};
        L.tab[1] = AowBox{x0 + 210, y0 + 163, 189, 21};
        L.t1 = AowBox{x0 + 22, y0 + 178, 376, 201};
        L.t2 = L.t1;
        L.list_pnl = AowBox{L.t1.x + 5, L.t1.y + 15, 366, 156};
        L.header[0] = AowBox{L.list_pnl.x + 1, L.list_pnl.y, 168, 14};
        L.header[1] = AowBox{L.list_pnl.x + 164, L.list_pnl.y, 85, 14};
        L.header[2] = AowBox{L.list_pnl.x + 248, L.list_pnl.y, 118, 14};
        L.list_name = AowBox{L.list_pnl.x + 1, L.list_pnl.y + 14, 164, 141};
        L.list_type = AowBox{L.list_pnl.x + 164, L.list_pnl.y + 14, 85, 141};
        L.list_src = AowBox{L.list_pnl.x + 248, L.list_pnl.y + 14, 118, 141};
        L.memo_body = AowBox{L.t2.x + 5, L.t2.y + 15, 366, 87};
        L.memo = AowBox{L.memo_body.x + 1, L.memo_body.y, 365, 86};
        L.type_pnl = AowBox{L.t2.x + 5, L.t2.y + 109, 116, 87};
        L.align_pnl = AowBox{L.t2.x + 126, L.t2.y + 109, 125, 87};
        L.rel_pnl = AowBox{L.t2.x + 256, L.t2.y + 109, 115, 87};
        L.type_icon = AowBox{L.type_pnl.x + 39, L.type_pnl.y + 10, 39, 36};
        // Card captions at the DFM's WinTop 56; the values one item-height
        // below at 69. Both were confirmed against the capture's bright rows.
        L.type_lbl = AowBox{L.type_pnl.x + 58, L.type_pnl.y + 56, 0, 0};
        L.type_val = AowBox{L.type_pnl.x + 58, L.type_pnl.y + 69, 0, 0};
        L.align_icon = AowBox{L.align_pnl.x + 6, L.align_pnl.y + 10, 113, 36};
        L.align_lbl = AowBox{L.align_pnl.x + 62, L.align_pnl.y + 56, 0, 0};
        L.align_val = AowBox{L.align_pnl.x + 62, L.align_pnl.y + 69, 0, 0};
        L.rel_icon = AowBox{L.rel_pnl.x + 32, L.rel_pnl.y + 10, 51, 36};
        L.rel_lbl = AowBox{L.rel_pnl.x + 57, L.rel_pnl.y + 56, 0, 0};
        L.rel_val = AowBox{L.rel_pnl.x + 57, L.rel_pnl.y + 69, 0, 0};
        // T2SBar: T2Text + (349,0), 17x86 (the DFM's own box).
        L.memo_sb = AowBox{L.memo_body.x + 349, L.memo_body.y, 17, 86};
        // PPrev/PNext (Priority 50000): the two 12x9 CampaignTrans arrows at
        // the bottom of the right frame; UnitMainPnl awRight/ahBottom.
        L.pprev = AowBox{L.main.x + 296, L.main.y + 109, 12, 9};
        L.pnext = AowBox{L.main.x + 331, L.main.y + 109, 12, 9};
        L.disband = AowBox{x0 + 89, y0 + 380, 119, 27};
        L.bclose = AowBox{x0 + 212, y0 + 380, 119, 27};
        L.celt_l = AowBox{x0 + 27, y0 + 380, 54, 27};
        L.celt_r = AowBox{x0 + 339, y0 + 380, 54, 27};
        L.celt_line = AowBox{x0 + 21, y0 + 380, 378, 27};
        if (unitwin_tab[0][0].ok) L.tab_h = static_cast<int>(unitwin_tab[0][0].h);
        for (int i = 0; i < 2; ++i) L.tab[i].h = L.tab_h;
        if (unitwin_header[0][0].ok) {
            L.header_h = static_cast<int>(unitwin_header[0][0].h);
        }
        for (int i = 0; i < 3; ++i) L.header[i].h = L.header_h;
        if (unitwin_button[0][0].ok) {
            L.button_h = static_cast<int>(unitwin_button[0][0].h);
        }
        L.disband.h = L.bclose.h = L.button_h;
        return L;
    }

    // The Unit window's control ids. The title bar's Close is the DFM's own
    // 15x16 button; the tabs sit above the two panels.
    enum {
        kUnitClose = 0, kUnitTab0, kUnitTab1, kUnitDisband, kUnitPrev, kUnitNext,
        kUnitList0 = 10, kUnitListRows = 8,  // + row
        kUnitSbUp = 30, kUnitSbDown
    };

    int unit_window_hit_test(int x, int y) const {
        if (!game_show_unit) return -1;
        const UnitWindowLayout L = unit_window_layout();
        auto in = [](const AowBox& b, int px, int py) {
            return b.x <= px && px < b.x + b.w && b.y <= py && py < b.y + b.h;
        };
        if (!in(L.win, x, y)) return -1;
        // PPrev/PNext are Priority 50000 and own the pointer wherever they are.
        if (in(L.pprev, x, y)) return kUnitPrev;
        if (in(L.pnext, x, y)) return kUnitNext;
        if (in(L.close, x, y)) return kUnitClose;
        if (in(L.tab[0], x, y)) return kUnitTab0;
        if (in(L.tab[1], x, y)) return kUnitTab1;
        if (in(L.disband, x, y)) return kUnitDisband;
        if (game_unit_tab == 1) {
            // T2SBar: the 17x16 dec/inc arrows at each end of the track.
            if (in(AowBox{L.memo_sb.x, L.memo_sb.y, L.memo_sb.w, 16}, x, y)) {
                return kUnitSbUp;
            }
            if (in(AowBox{L.memo_sb.x, L.memo_sb.y + L.memo_sb.h - 16,
                          L.memo_sb.w, 16},
                   x, y)) {
                return kUnitSbDown;
            }
        }
        if (game_unit_tab == 0) {
            // The three listboxes share one selection (T1AbilityLBMouseDown).
            const int row_h = 17;
            for (int i = 0; i < kUnitListRows; ++i) {
                const AowBox r{L.list_name.x, L.list_name.y + i * row_h,
                               L.list_name.w + L.list_type.w + L.list_src.w,
                               row_h};
                if (in(r, x, y)) return kUnitList0 + i;
            }
        }
        return -2;  // inside the window but not a control
    }

    // The DFM handlers: CloseClick (hide), UnitTabChange (show the tab's
    // panel), DisbandBtnClick and T1AbilityLBMouseDown (select a row). The
    // shared UI click is FUN_00455dfc(0) -> Sfx\Int\Btn1.wav.
    void unit_window_action(int hit) {
        if (hit < 0) return;
        play_game_sfx("Sfx\\Int\\Btn1.wav");
        if (hit == kUnitClose) {
            unit_window_close();
            return;
        }
        if (hit == kUnitTab0 || hit == kUnitTab1) {
            const int tab = hit == kUnitTab0 ? 0 : 1;
            if (tab != game_unit_tab) {
                game_unit_tab = tab;
                unitwin_abil_sel = -1;
                unitwin_abil_top = 0;
                unitwin_memo_top = 0;
            }
            return;
        }
        if (hit == kUnitPrev) {
            unit_window_step(-1);
            return;
        }
        if (hit == kUnitNext) {
            unit_window_step(1);
            return;
        }
        if (hit == kUnitSbUp) {
            if (unitwin_memo_top > 0) --unitwin_memo_top;
            return;
        }
        if (hit == kUnitSbDown) {
            ++unitwin_memo_top;   // clamped while drawing
            return;
        }
        if (hit == kUnitDisband) {
            unit_window_disband();
            return;
        }
        if (hit >= kUnitList0 && hit < kUnitList0 + kUnitListRows) {
            const int row = unitwin_abil_top + (hit - kUnitList0);
            if (row >= 0 && static_cast<std::size_t>(row) < unit_win.abilities.size()) {
                unitwin_abil_sel = row;
                std::printf("game_unit=ability row=%d name=%s\n", row + 1,
                            unit_win.abilities[static_cast<std::size_t>(row)]
                                .name.c_str());
                std::fflush(stdout);
            }
        }
    }

    // The Unit info floater (`TUnitWindow`). The draw order follows the DFM
    // priorities: window body/frame, the main panel's two framed portraits
    // (face under the 46 shade, unit sprite under the 46 shade), the stat
    // panel, the celtic rules, the UnitTab strip over the T1/T2 panels, then
    // the footer buttons.
    void draw_game_unit_window(Canvas& cv) const {
        if (!game_show_unit || !unit_win.valid) return;
        const UnitWindowLayout L = unit_window_layout();
        // 1. UnitWin: GenericT 2 body, DialogFr 32..39 frame, title bar.
        blit_tex_rect(cv, unitwin_body, L.win.x, L.win.y, L.win.w, L.win.h,
                      L.win.x, L.win.y);
        blit9_tiled(cv, unitwin_frame, L.win);
        cv.blit_clip(unitwin_title_icon,
                     L.title_icon.x + unitwin_title_icon.base_x +
                         unitwin_title_icon.shift_x,
                     L.title_icon.y + unitwin_title_icon.base_y +
                         unitwin_title_icon.shift_y,
                     L.title_icon.x, L.title_icon.y, L.title_icon.w,
                     L.title_icon.h);
        text_at(cv, glyphs10b, L.win.x + 42, L.win.y + 7, "Unit", 0, 0, 0,
                false, false);
        {
            const int st = unitwin_pressed == kUnitClose
                               ? 2
                               : (unitwin_hover == kUnitClose ? 1 : 0);
            cv.blit_stretch(unitwin_close[st], L.close.x, L.close.y, L.close.w,
                            L.close.h);
        }
        // 2. UnitMainPnl (GenericT 2). Its DFM block does not name an ILFrame,
        // so the panel body is unframed; the texture is tiled from its origin.
        blit_tex_rect(cv, unitwin_body, L.main.x, L.main.y, L.main.w, L.main.h,
                      L.main.x, L.main.y);
        // 3. RightFramePnl (Priority 0): GenericT 7 ScrollT parchment through
        // the FrGoldD frame's 3 px inner origin.
        // RightFramePnl's texture starts at the panel origin, not at the
        // FrGoldD corner (the reference's ScrollT tiles exactly from 281,23).
        blit_tex_rect(cv, unitwin_tex7, L.right_body.x, L.right_body.y,
                      L.right_body.w, L.right_body.h, L.right_body.x,
                      L.right_body.y);
        // 4. The stat panel (a Priority-0 child of UnitMainPnl). UStatPnl has
        // no ILTexture, so the UnitMainPnl body shows through; the reference's
        // border is the 2 px raised/inset bevel the shipped dialog draws around
        // it (outer dark/light on the top+bottom, light/dark on the sides).
        {
            const AowBox b{L.stat_pnl.x - 1, L.stat_pnl.y - 3,
                           L.stat_pnl.w + 5, L.stat_pnl.h + 7};
            // Sampled from the reference's 2 px stat-panel bevel.
            const std::uint8_t d[3] = {41, 49, 33};
            const std::uint8_t l[3] = {165, 173, 148};
            cv.fill(b.x, b.y, b.w, 1, d[0], d[1], d[2], 255);
            cv.fill(b.x, b.y + 1, b.w, 1, l[0], l[1], l[2], 255);
            cv.fill(b.x, b.y + b.h - 2, b.w, 1, d[0], d[1], d[2], 255);
            cv.fill(b.x, b.y + b.h - 1, b.w, 1, l[0], l[1], l[2], 255);
            cv.fill(b.x, b.y + 2, 1, b.h - 4, l[0], l[1], l[2], 255);
            cv.fill(b.x + 1, b.y + 2, 1, b.h - 4, d[0], d[1], d[2], 255);
            cv.fill(b.x + b.w - 2, b.y + 2, 1, b.h - 4, l[0], l[1], l[2], 255);
            cv.fill(b.x + b.w - 1, b.y + 2, 1, b.h - 4, d[0], d[1], d[2], 255);
        }
        for (int i = 0; i < 6; ++i) {
            // Icons 0 Attack / 1 Defense / 2 Damage / 3 Resistance / 4 Hits;
            // the moves slot uses IconMove 0 instead (UMovesIcon). TAOWImage
            // draws the library image at its own base+shift: the moves icon is
            // a 10x11 boot, *not* a 32x17 stretch (which turned it into a
            // distorted anvil in the first replica).
            const Sprite& ic =
                i == 2 ? unitwin_move_icon
                       : unitwin_stat_icon[i < 2 ? i : i - 1];
            if (ic.ok) {
                // TAOWImage blits the library image at base+shift and clips it
                // to the 15x17 control box (the UnitIcons art itself is 15x20,
                // so its bottom 3 px are cut exactly as in the reference).
                cv.blit_clip(ic, L.stat_icon[i].x + ic.base_x,
                             L.stat_icon[i].y + ic.base_y, L.stat_icon[i].x,
                             L.stat_icon[i].y, L.stat_icon[i].w,
                             L.stat_icon[i].h);
            }
        }
        if (unit_win.upkeep_mana ? unitwin_upkeep_mana.ok
                                 : unitwin_upkeep_gold.ok) {
            // TBWin 51/52 are 69x18 `BarSet2` images whose *left* 19 px is the
            // gold/mana pile; the 19x21 icon box clips that pile 1:1.
            const Sprite& ic = unit_win.upkeep_mana ? unitwin_upkeep_mana
                                                    : unitwin_upkeep_gold;
            cv.blit_clip(ic, L.stat_icon[6].x + ic.base_x,
                         L.stat_icon[6].y + ic.base_y, L.stat_icon[6].x,
                         L.stat_icon[6].y, L.stat_icon[6].w,
                         L.stat_icon[6].h);
        }
        const std::string vals[7] = {
            std::to_string(unit_win.attack),
            std::to_string(unit_win.defense),
            std::to_string(unit_win.moves) + "/" +
                std::to_string(unit_win.max_moves),
            std::to_string(unit_win.damage),
            std::to_string(unit_win.resistance),
            std::to_string(unit_win.hits) + "/" +
                std::to_string(unit_win.max_hits),
            std::to_string(unit_win.upkeep)};
        for (int i = 0; i < 7; ++i) {
            // AoW15WhiteGrey's glyph layer is white ink with neutral-grey
            // antialias edges. The engine blits the glyph itself (its palette
            // greys stay grey); recolouring every non-black pixel white - what
            // the first replica did - made every stat value one pixel bolder.
            text_at(cv, glyphs, L.stat_val[i].x, L.stat_val[i].y, vals[i], 255,
                    255, 255, true, true);
        }
        // 5. The portrait pair, in the DFM's priority order:
        //    RightFramePnl(0) -> FrLSh/FrRSh(100: the pre-draw face/sprite
        //    then the 46 shadow) -> UnitFrameLeft/UnitFrRight(200, the frames
        //    over the portrait edges).
        //
        // The 46 shadow is a ShowInfo (mode 2, subtype 4) ShadowBlendTable
        // image *with blend level 0* in this form, so its blend is the
        // identity - it contributes nothing and the reference frame is the
        // raw 44/45 art. (The first replica keyed it opaquely, which painted
        // the whole 104x128 light mask over the portrait.)
        if (unitwin_face.ok) {
            // FrLShPreDraw's rect: (Left+29, Top+28)-(Left+132, Top+156) on
            // the 118x143 frame, i.e. the 44 hole at +8,+7, 103x128.
            cv.blit_stretch(unitwin_face, L.left_frame.x + 10,
                            L.left_frame.y + 7, 103, 128);
        }
        if (unitwin_shade.ok) {
            cv.blit_shadow(unitwin_shade, L.left_shade.x, L.left_shade.y);
        }
        if (unitwin_left_frame.ok) {
            cv.blit_keyed(unitwin_left_frame, L.left_frame.x, L.left_frame.y);
        }
        if (unitwin_sprite.ok) {
            // FrRShPreDraw anchors ShowUnit at (FrRSh.Left + 0x31,
            // FrRSh.Top + 0x4e); the pose then carries its own base+shift.
            // `ShowUnit` blits the pose at the anchor the pre-draw passes; the
            // frame's own base+shift is *not* added a second time here. Using
            // the raw anchor reproduces the reference's sprite column (the
            // pose helper's vx/vy belong to the map/HXtoHP anchor path).
            cv.blit_stretch(unitwin_sprite, L.right_body.x + 0x31 + 4,
                            L.right_body.y + 0x4e - 5,
                            static_cast<int>(unitwin_sprite.w),
                            static_cast<int>(unitwin_sprite.h));
        }
        if (unitwin_shade.ok) {
            cv.blit_shadow(unitwin_shade, L.right_shade.x, L.right_shade.y);
        }
        if (unitwin_right_frame.ok) {
            cv.blit_keyed(unitwin_right_frame, L.right_frame.x,
                          L.right_frame.y);
        }
        // The two celtic rules (UnitLine1/2, Priority 1000) are drawn before
        // the Priority-1500 UnitName/Morale labels so the rules never cover
        // the text.
        if (unitwin_line.ok) {
            // TAOWImage.Draw tiles a 100 px image across the 142 px UnitLine
            // control (`width / art.w + 1` copies clipped to the control), so
            // the celtic rule runs continuously from the control's left edge.
            const int lw = static_cast<int>(unitwin_line.w);
            const int lh = static_cast<int>(unitwin_line.h);
            for (int tx = L.line1.x; tx < L.line1.x + L.line1.w; tx += lw) {
                cv.blit_clip(unitwin_line, tx, L.line1.y, L.line1.x,
                             L.line1.y, L.line1.w, lh);
                cv.blit_clip(unitwin_line, tx, L.line2.y, L.line2.x,
                             L.line2.y, L.line2.w, lh);
            }
        }
        // 6. UnitName / Level / Exp / Morale.
        text_at(cv, glyphs10, L.name.x, L.name.y, unit_win.name, 211, 237, 202,
                true, false);
        text_at(cv, glyphs8b, L.level.x, L.level.y,
                "Level " + std::to_string(unit_win.level), 28, 28, 23, true,
                false);
        text_at(cv, glyphs8b, L.xp.x, L.xp.y,
                "Exp (" + std::to_string(unit_win.experience) + "/" +
                    std::to_string(unit_win.next_level_experience) + ")",
                28, 28, 23, true, false);
        if (unit_win.show_morale) {
            text_at(cv, glyphs10, L.morale.x, L.morale.y - 5, unit_win.morale,
                    211, 237, 202, true, false);
        }
        draw_game_unit_tabs(cv, L);
        // PNext/PPrev are Priority 50000, i.e. above the tabs. CampaignTrans
        // 23/25 idle (previous/next) and 24/26 held.
        {
            const Sprite& prev =
                unitwin_pressed == kUnitPrev ? unitwin_pprev[1] : unitwin_pprev[0];
            const Sprite& next =
                unitwin_pressed == kUnitNext ? unitwin_pnext[1] : unitwin_pnext[0];
            if (prev.ok) {
                cv.blit_stretch(prev, L.pprev.x, L.pprev.y, L.pprev.w,
                                L.pprev.h);
            }
            if (next.ok) {
                cv.blit_stretch(next, L.pnext.x, L.pnext.y, L.pnext.w,
                                L.pnext.h);
            }
        }
        draw_game_unit_footer(cv, L);
    }

    // The UnitTab strip (CeltDeco 67..72) and the two tab panels. T1 is the
    // three-column ability list; T2 is the description memo and the Type /
    // Alignment / Race Relation cards. The tab strip is drawn last so its
    // bottom edge sits over the panel top exactly as Priority 3000 does.
    void draw_game_unit_tabs(Canvas& cv, const UnitWindowLayout& L) const {
        if (game_unit_tab == 0) {
            // T1Panel (GenericT 2, no ILFrame) and T1ListPnl (FrThinD 2 px
            // inner origin over GenericT 2).
            blit_tex_rect(cv, unitwin_body, L.t1.x, L.t1.y, L.t1.w, L.t1.h,
                          L.t1.x, L.t1.y);
            blit_tex_rect(cv, unitwin_body, L.list_pnl.x, L.list_pnl.y,
                          L.list_pnl.w, L.list_pnl.h, L.list_pnl.x + 2,
                          L.list_pnl.y + 2);
            blit9_tiled(cv, unitwin_thin_frame, L.list_pnl);
            // The three listbox bodies (GenericT 21) and their FrGoldD frames
            // (3 px inner origin).
            const AowBox* cols[3] = {&L.list_name, &L.list_type, &L.list_src};
            for (int c = 0; c < 3; ++c) {
                const AowBox& b = *cols[c];
                blit_tex_rect(cv, unitwin_tex21, b.x, b.y, b.w, b.h, b.x + 3,
                              b.y + 3);
                blit9_tiled(cv, unitwin_gold_frame, b);
            }
            const int row_h = 17;
            const int rows = static_cast<int>(unit_win.abilities.size());
            // T1AbilityLB.Selected: Menus 8 across all three columns.
            if (unitwin_abil_sel >= unitwin_abil_top) {
                const int i = unitwin_abil_sel - unitwin_abil_top;
                if (i >= 0 && i < kUnitListRows && unitwin_list_sel.ok) {
                    const int y = L.list_name.y + i * row_h;
                    cv.blit_stretch(unitwin_list_sel, L.list_name.x, y,
                                    L.list_name.w, row_h);
                    cv.blit_stretch(unitwin_list_sel, L.list_type.x, y,
                                    L.list_type.w, row_h);
                    cv.blit_stretch(unitwin_list_sel, L.list_src.x, y,
                                    L.list_src.w, row_h);
                }
            }
            for (int i = 0; i < kUnitListRows; ++i) {
                const int row = unitwin_abil_top + i;
                if (row < 0 || row >= rows) break;
                const bool sel = row == unitwin_abil_sel;
                const std::uint8_t r = sel ? 0 : 160;
                const std::uint8_t g = sel ? 0 : 169;
                const std::uint8_t b = sel ? 0 : 143;
                const int y = L.list_name.y + i * row_h + 5;
                const UnitWindowAbility& a =
                    unit_win.abilities[static_cast<std::size_t>(row)];
                // T1AbilityLB.TextColor 14679807 (BGR) = (255,254,223) cream;
                // Type/Source are the warm grey 9415072 = (160,169,143).
                // CAUTION - shared-font sandbox. TUnitWindow's controls name
                // FontModule.Age8, whose CharacterDistance is 0 (TFONTMODULE.txt);
                // pin it at these draw sites. Never give this screen its spacing
                // by changing decode_glyph to advance by the RLE ink width: that
                // is shared with every 0x02 face and it squeezed the Scenario
                // screen's Age8s labels ("CPU Squire", "Simultaneous") by 1 px
                // per glyph.
                text_at(cv, glyphs8b, L.list_name.x + 6, y, a.name,
                        sel ? 0 : 255, sel ? 0 : 254, sel ? 0 : 223, false,
                        false, 0);
                text_at(cv, glyphs8b, L.list_type.x + 6, y, a.type, r, g, b,
                        false, false, 0);
                text_at(cv, glyphs8b, L.list_src.x + 6, y, a.source, r, g, b,
                        false, false, 0);
            }
            // The three sort headers (GenericB 85/91/88, Age8 clBlack).
            const char* captions[3] = {"Ability Name", "Type", "Source"};
            for (int c = 0; c < 3; ++c) {
                const AowBox& h = L.header[c];
                blit3_tile(cv, unitwin_header[0], h);
                text_at(cv, glyphs8b, h.x + 5, h.y + 1, captions[c], 24, 20, 16,
                        false, false, 0);
            }
        } else {
            draw_game_unit_info(cv, L);
        }
        // The tab strip last (Priority 3000): down for the current page, up
        // for the other. Selected text is clBlack, the other band's is the
        // warm grey TextSelColor 9415072.
        const char* labels[2] = {"Abilities", "Info"};
        for (int i = 0; i < 2; ++i) {
            // The current page is the down/lit band (ILIndexDown = 67);
            // the other one keeps the raised ILIndexUp = 70 art. The tab panel
            // draws its left cap, then the 130x22 middle art repeated/clipped
            // across the span, then the right cap; the reference's light face
            // is continuous all the way to the cap.
            const int st = i == game_unit_tab ? 0 : 1;
            const Sprite* b = unitwin_tab[st];
            const int y = L.tab[i].y;
            if (b[1].ok) {
                const int lw = static_cast<int>(b[0].w);
                const int rw = static_cast<int>(b[2].w);
                const int x0 = L.tab[i].x + lw;
                const int x1 = L.tab[i].x + L.tab[i].w - rw;
                cv.blit_stretch(b[0], L.tab[i].x, y, lw, L.tab[i].h);
                for (int tx = x0; tx < x1; tx += static_cast<int>(b[1].w)) {
                    cv.blit_clip(b[1], tx, y, x0, y, x1 - x0, L.tab[i].h);
                }
                cv.blit_stretch(b[2], x1, y, rw, L.tab[i].h);
            }
            const bool sel = i == game_unit_tab;
            text_at(cv, glyphs10b, L.tab[i].x + L.tab[i].w / 2,
                    L.tab[i].y + 5, labels[i], sel ? 0 : 160,
                    sel ? 0 : 169, sel ? 0 : 143, true, false);
        }
    }

    // T2Memo is a TAOWMemo (FontModule.Age10s, 365x86, ItemHeight 15): word
    // wrap the unit's description, then show five rows from `unitwin_memo_top`
    // and let T2SBar move that window.
    void draw_unit_memo(Canvas& cv, int x, int y, int w,
                        const std::string& s) const {
        const int line_h = 15;
        unitwin_memo_lines.clear();
        std::size_t i = 0;
        while (i < s.size()) {
            // An explicit newline always breaks.
            const std::size_t nl = s.find('\n', i);
            const std::size_t end =
                nl == std::string::npos ? s.size() : nl;
            std::string rest = s.substr(i, end - i);
            while (!rest.empty()) {
                std::size_t take = rest.size();
                // Fit as many characters as the line allows, preferring the
                // last space before the limit (the memo's own word wrap).
                if (text_w(glyphs10, rest) > w) {
                    take = 0;
                    for (std::size_t k = 1; k <= rest.size(); ++k) {
                        if (text_w(glyphs10, rest.substr(0, k)) > w) break;
                        take = k;
                    }
                    const std::size_t sp = rest.rfind(' ', take);
                    if (sp != std::string::npos && sp > 0) take = sp;
                }
                if (take == 0) take = 1;
                unitwin_memo_lines.push_back(rest.substr(0, take));
                rest.erase(0, take);
                while (!rest.empty() && rest.front() == ' ') rest.erase(0, 1);
            }
            if (nl == std::string::npos) break;
            i = nl + 1;
        }
        unitwin_memo_total = static_cast<int>(unitwin_memo_lines.size());
        const int max_top = unitwin_memo_total > 5 ? unitwin_memo_total - 5 : 0;
        if (unitwin_memo_top < 0) unitwin_memo_top = 0;
        if (unitwin_memo_top > max_top) unitwin_memo_top = max_top;
        for (int row = 0; row < 5; ++row) {
            const int li = unitwin_memo_top + row;
            if (li >= unitwin_memo_total) break;
            text_at(cv, glyphs10, x, y + row * line_h,
                    unitwin_memo_lines[static_cast<std::size_t>(li)], 160, 169,
                    143, false, false);
        }
    }

    // T2SBar (TAOWVScrollBar, GenericS 94..101): a 17x86 bar at the memo's
    // right edge. The arrows are always drawn in the reference; the thumb is
    // sized by NumberVisible / Max and sits at Position.
    void draw_unit_memo_sb(Canvas& cv, const UnitWindowLayout& L) const {
        if (!unitwin_sb[0].ok) return;
        const AowBox& b = L.memo_sb;
        cv.blit_stretch(unitwin_sb[0], b.x, b.y, b.w, 16);              // 94 dec up
        cv.blit_stretch(unitwin_sb[2], b.x, b.y + b.h - 16, b.w, 16);   // 96 inc down
        const int track_y = b.y + 16;
        const int track_h = b.h - 32;
        if (unitwin_sb[7].ok) {
            cv.blit_tile(unitwin_sb[7], b.x, track_y, b.w, track_h, true);
        }
        // T2SBar.NumberVisible is 1 in the DFM; the reference's thumb covers
        // roughly two text rows, which is TAOWVScrollBar's two-row minimum.
        const int total = unitwin_memo_total > 5 ? unitwin_memo_total : 5;
        int thumb_h = track_h * 2 / total;
        if (thumb_h < 12) thumb_h = 12;
        if (thumb_h > track_h) thumb_h = track_h;
        const int span = track_h - thumb_h;
        const int max_top = unitwin_memo_total > 5 ? unitwin_memo_total - 5 : 1;
        const int ty = track_y + (max_top > 0 ? span * unitwin_memo_top /
                                                max_top
                                              : 0);
        const Sprite& th = unitwin_sb[4].ok ? unitwin_sb[4] : unitwin_sb[7];
        cv.blit_stretch(th, b.x, ty, b.w, thumb_h);
    }

    // T2Panel: the Info tab's memo over the three cards (Type / Alignment /
    // Race Relation). The cards are TAOWPanel GenericF 105 + GenericT 17 with
    // the Value labels in Age8 and the DFM's own 39x36 / 113x36 / 51x36 art.
    void draw_game_unit_info(Canvas& cv, const UnitWindowLayout& L) const {
        // T2Panel has no ILFrame in the DFM (only its GenericT 2 body), while
        // T2Text/T2Leader and the three cards are FrThinD (2 px inner origin).
        blit_tex_rect(cv, unitwin_body, L.t2.x, L.t2.y, L.t2.w, L.t2.h,
                      L.t2.x, L.t2.y);
        blit_tex_rect(cv, unitwin_tex17, L.memo_body.x, L.memo_body.y,
                      L.memo_body.w, L.memo_body.h, L.memo_body.x + 2,
                      L.memo_body.y + 2);
        blit9_tiled(cv, unitwin_thin_frame, L.memo_body);
        // T2Memo (Priority 200, T2Text+1): GenericT 18 TextOld2 inside its own
        // FrGoldD frame. This is the near-black page the reference shows; the
        // T2Text panel behind it is the TextOld1 border.
        blit_tex_rect(cv, unitwin_tex18, L.memo.x, L.memo.y, L.memo.w,
                      L.memo.h, L.memo.x + 3, L.memo.y + 3);
        blit9_tiled(cv, unitwin_gold_frame, L.memo);
        // BorderWidth 5 / BorderHeight 3 plus the 3 px FrGoldD inset; the
        // reference's first line starts at memo + (8,6). 323 px is the exact
        // wrap interval between the three reference line widths (321/316/293)
        // and the next word (327) - it reproduces the four captured lines.
        draw_unit_memo(cv, L.memo.x + 8, L.memo.y + 6, 323,
                       unit_win.description);
        draw_unit_memo_sb(cv, L);
        const AowBox* panels[3] = {&L.type_pnl, &L.align_pnl, &L.rel_pnl};
        for (const AowBox* p : panels) {
            blit_tex_rect(cv, unitwin_tex17, p->x, p->y, p->w, p->h, p->x + 2,
                          p->y + 2);
            blit9_tiled(cv, unitwin_thin_frame, *p);
        }
        // A TAOWImage blits its slot at the control origin and places the
        // cropped visual by base+shift, clipping to the control. The card art
        // carries transparent margins (39x36 TypeIcn slot with a 37x36 disc,
        // 51x36 RelationIcn slot with a 37x36 face), so stretching the visual
        // to the slot makes it visibly larger than the capture.
        auto card_icon = [&](const Sprite& ic, const AowBox& b) {
            cv.blit_clip(ic, b.x + ic.base_x + ic.shift_x,
                         b.y + ic.base_y + ic.shift_y, b.x, b.y, b.w, b.h);
        };
        // The three captions/values are Age8 with CharacterDistance 0 (same
        // shared-font sandbox as draw_game_unit_tabs): pin the step at the draw
        // sites so a change to the shared face table cannot move them.
        if (unit_win.show_type && unit_win.type_icon >= 0 &&
            unit_win.type_icon < 3 && unitwin_type_icon[unit_win.type_icon].ok) {
            card_icon(unitwin_type_icon[unit_win.type_icon], L.type_icon);
            text_at(cv, glyphs8b, L.type_lbl.x, L.type_lbl.y, "Type", 160, 169,
                    143, true, false, 0);
            text_at(cv, glyphs8b, L.type_val.x, L.type_val.y, unit_win.type,
                    160, 169, 143, true, false, 0);
        }
        if (unit_win.align_icon >= 0 && unit_win.align_icon < 5 &&
            unitwin_align_icon[unit_win.align_icon].ok) {
            card_icon(unitwin_align_icon[unit_win.align_icon], L.align_icon);
            text_at(cv, glyphs8b, L.align_lbl.x, L.align_lbl.y, "Alignment",
                    160, 169, 143, true, false, 0);
            text_at(cv, glyphs8b, L.align_val.x, L.align_val.y,
                    unit_win.alignment, 160, 169, 143, true, false, 0);
        }
        if (unit_win.show_relation && unit_win.relation_icon >= 0 &&
            unit_win.relation_icon < 8 &&
            unitwin_relation[unit_win.relation_icon].ok) {
            card_icon(unitwin_relation[unit_win.relation_icon], L.rel_icon);
            text_at(cv, glyphs8b, L.rel_lbl.x, L.rel_lbl.y, "Race Relation",
                    160, 169, 143, true, false, 0);
            text_at(cv, glyphs8b, L.rel_val.x, L.rel_val.y, unit_win.relation,
                    160, 169, 143, true, false, 0);
        }
    }

    // The footer (both tabs): BCeltL is a `frHorizLine` over CeltDeco 55
    // (7x27 left cap), 56 (100x27 braid) and 57 (7x27 right cap). DrawILLh
    // runs `(width - caps) / middle.w + 1` middle tiles (four here), clipped
    // to the box, then pins the right cap; the braid is therefore continuous
    // across the whole 378 px band exactly as the reference shows. The two
    // CeltLineL/R knots are a 27x27 image in a 54x27 box, which TAOWImage
    // tiles twice.
    void draw_game_unit_footer(Canvas& cv, const UnitWindowLayout& L) const {
        if (unitwin_celt_line.ok && unitwin_celt_mid.ok &&
            unitwin_celt_right.ok) {
            const int y = L.celt_line.y;
            const int lw = static_cast<int>(unitwin_celt_line.w);
            const int rw = static_cast<int>(unitwin_celt_right.w);
            const int x1 = L.celt_line.x + L.celt_line.w - rw;
            cv.blit_keyed(unitwin_celt_line, L.celt_line.x, y);
            for (int tx = L.celt_line.x + lw; tx < x1;
                 tx += static_cast<int>(unitwin_celt_mid.w)) {
                cv.blit_clip(unitwin_celt_mid, tx, y, L.celt_line.x + lw, y,
                             x1 - (L.celt_line.x + lw), L.celt_line.h);
            }
            cv.blit_keyed(unitwin_celt_right, x1, y);
        } else if (unitwin_celt_line.ok) {
            cv.blit_tile(unitwin_celt_line, L.celt_line.x, L.celt_line.y,
                         L.celt_line.w, L.celt_line.h, false);
        }
        if (unitwin_celt_knot.ok) {
            const AowBox* knots[2] = {&L.celt_l, &L.celt_r};
            for (const AowBox* k : knots) {
                for (int ty = 0; ty < k->h;
                     ty += static_cast<int>(unitwin_celt_knot.h)) {
                    for (int tx = 0; tx < k->w;
                         tx += static_cast<int>(unitwin_celt_knot.w)) {
                        cv.blit_clip(unitwin_celt_knot, k->x + tx, k->y + ty,
                                     k->x, k->y, k->w, k->h);
                    }
                }
            }
        }
        struct Btn {
            const AowBox* box;
            const char* text;
            int id;
            std::uint8_t r, g, b;
        };
        const Btn btns[2] = {{&L.disband, "Disband", kUnitDisband, 89, 0, 0},
                             {&L.bclose, "Close", kUnitClose, 24, 20, 16}};
        for (const Btn& b : btns) {
            const int st = unitwin_pressed == b.id
                               ? 2
                               : (unitwin_hover == b.id ? 1 : 0);
            blit3_clip(cv, unitwin_button[st], *b.box);
            text_at(cv, glyphs10b, b.box->x + b.box->w / 2, b.box->y + 4,
                    b.text, b.r, b.g, b.b, true, false);
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
        const int f = tb_turn_start + tb_turn_frame;
        return (f >= 0 && f < kTbTurnFrameCount) ? f : -1;
    }


    // TAOWTurnButton.Update (AOWTools.dpl 0x59704cd4), one call per game frame -
    // the window's own Update cadence. The state machine is reproduced verbatim:
    //   mode 0  idle; a CurPlayer that differs from LastPlayer starts the change
    //           from LastPlayer's block at AnimStart;
    //   mode 1  step the frame up until *LastPlayer's* +5 frame (0x59704d2c
    //           compares +0x140 against +0x11c, not +0x118);
    //   mode 2  run the "turn changing" glow frames 0..10;
    //   mode 3  step down from CurPlayer's +5 frame to its first frame, then
    //           settle on it, keep LastPlayer = CurPlayer, clear Animating and
    //           fire OnAnimDone (TurnButtonAnimDone clears Checked).
    // LastPlayer = -1 (the day handler 0x0044ece8) therefore starts a change
    // even when CurPlayer did not move: Frame = -7, drawn as library frame 4,
    // and the walk is the one 23-frame rotation of a new day.
    void tb_turn_update() {
        switch (tb_turn_mode) {
        case 0:
            if (tb_turn_last == tb_turn_cur) break;
            tb_turn_frame = kTbTurnRaceStride * tb_turn_last;
            tb_turn_start = kTbTurnAnimStart;
            tb_turn_mode = 1;
            tb_turn_animating = true;
            std::printf("game_turnanim=start from=%d to=%d\n", tb_turn_last,
                        tb_turn_cur);
            std::fflush(stdout);
            break;
        case 1:
            ++tb_turn_frame;
            if (tb_turn_frame > kTbTurnRaceStride * tb_turn_last + 5) {
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
                tb_turn_ai_hold = 0;
                std::printf("game_turnanim=done frame=%d\n", tb_turn_frame);
                std::fflush(stdout);
            }
            break;
        default:
            tb_turn_mode = 0;
            break;
        }
        // The decompiled Update has no clamp: Frame may be negative while mode 1
        // walks LastPlayer == -1 (the day change). Draw bounds-checks the index.
    }

    // The form's map-open update (AoW.exe 0x00454d2c with 0x004551b1): the
    // button takes the *seated* player's race block - both CurPlayer and
    // LastPlayer - and snaps to the block's first frame. It is not called when
    // the turn simply moves to the next player; in a single-player game the seat
    // never changes, so the one visible change is the new-day rotation below.
    void tb_turn_set_player(int race) {
        if (race < 0 || race > kTbTurnNoRace) race = kTbTurnNoRace;
        tb_turn_checked = false;
        tb_turn_cur = tb_turn_last = race;
        tb_turn_frame = kTbTurnRaceStride * race;
        tb_turn_start = kTbTurnAnimStart;
        tb_turn_mode = 0;
        tb_turn_animating = false;
        tb_turn_ai_hold = 0;
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

    // The turn loop moved to another player: the day read-out follows the map,
    // and a human seat's turn start plays its sound and refills its armies
    // (TTurnPlayerControl.NewTurn 0x55756714). The TurnButton itself follows the
    // *seated* seat only, set once when the map opens (0x00454d2c); the one
    // in-game change is the new day, where the day handler 0x0044ece8 stores
    // LastPlayer = -1 so TAOWTurnButton.Update plays exactly one rotation.
    void game_turn_changed(bool animate) {
        const aow::Player* p = game_current_player();
        const int prev_day = game_day;
        game_day = mv_gs.day;
        // The round wrapped: every seat has ended, so the day label handler runs
        // and invalidates LastPlayer. The button stays on its Checked glow until
        // the 23-frame rotation finishes (TurnButtonAnimDone clears Checked).
        // NEWTURN.WAV belongs to the same instant - the start of the rotation -
        // not to the later hand-over to the human's seat in the new order.
        if (animate && game_day > prev_day) {
            tb_turn_last = -1;
            play_game_sfx("SFX\\EVENTS\\NEWTURN.WAV");
            game_newturn_sound_day = game_day;
            std::printf("game_newturn_sound=day=%d at=turnanim-start\n",
                        game_day);
            std::fflush(stdout);
        }
        // Head-less diagnostic: the four WGold/WMana bar read-outs for the
        // seated player (`game_live_stats` is what render_game draws).
        if (const aow::Player* seat = game_seat()) {
            std::printf("game_stats=player=%d gold=%d income=%d upkeep=%d "
                        "net=%d mana=%d mana_income=%d\n",
                        static_cast<int>(seat->index), seat->gold, seat->income,
                        seat->upkeep(), seat->income - seat->upkeep(),
                        seat->magic.mana, seat->magic.mana_income);
            std::fflush(stdout);
        }
        if (p == nullptr) {
            std::printf("game_turn=none day=%d\n", game_day);
            std::fflush(stdout);
            return;
        }
        // `TCity.NewTurn@557ab814` -> `TProductionControl.NewTurn@5575dd00`:
        // every city this seat owns retries a waiting item, ticks an executing
        // one and completes it. A finished produce places the unit on the map,
        // so re-composite the current level's armies afterwards.
        mv_city_production_new_turn(p->index);
        if (!game_px.empty()) mv_refresh_draw();
        std::printf("game_turn=player=%d name=%s day=%d race=%d block=%d\n",
                    static_cast<int>(p->index), p->display_name().c_str(),
                    game_day, static_cast<int>(p->race),
                    kTbTurnAnimStart + kTbTurnRaceStride * tb_turn_cur);
        std::fflush(stdout);
        if (p->index == mv_human) {
            mv_reset_army_turn(mv_human);
            tb_next_index = -1;   // TPlayer+0xb4 starts a fresh seat
            // The first turn (and a restart) has no day-change rotation, so it
            // plays the sound here; every later round already played it at the
            // rotation start above.
            if (game_newturn_sound_day != game_day) {
                play_game_sfx("SFX\\EVENTS\\NEWTURN.WAV");
                game_newturn_sound_day = game_day;
                std::printf("game_newturn_sound=day=%d at=seat-start\n",
                            game_day);
                std::fflush(stdout);
            }
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
            tb_turn_set_player(tb_turn_race_of(game_current_player()));
            game_day = mv_gs.day;
            return;
        }
        mv_gs.begin_first_turn(mv_human);
        game_turns_started = true;
        // A fresh/restarted loop must play the first hand-over's NEWTURN.WAV.
        game_newturn_sound_day = 0;
        // The map-open snap: CurPlayer and LastPlayer become the seated seat's
        // race and the button sits on its block's first frame. No animation.
        tb_turn_set_player(tb_turn_race_of(game_current_player()));
        game_turn_changed(false);
    }

    // TurnButtonChange (AoW.exe 0x004574ac): the button ends the *seated*
    // player's turn, or withdraws it when that player already ended. The result
    // of the map's EndTurn/CancelEndTurn is filed back into Checked
    // (`*(Sender+0x5c) + 0x13c`), which is what turns the button into its
    // "turn ended" picture. It does not move CurPlayer/Frame: the button stays
    // on the Checked transition frame until the new-day rotation (0x0044ece8).
    void game_turn_button_click() {
        if (!mv_ready || mv_gs.players.empty() || mv_human == 0xff) return;
        if (tb_turn_animating) return;   // CheckMouseDown's [+0x120] gate
        const aow::Player* seat = game_player_by_index(mv_gs, mv_human);
        const bool ended = seat != nullptr && seat->end_turn;
        tb_turn_checked = !tb_turn_checked;
        if (!ended) {
            const std::uint8_t before = mv_gs.current();
            tb_turn_checked = mv_gs.end_turn(mv_human);
            tb_turn_ai_hold = tb_turn_checked ? kGameAiTurnHoldFrames : 0;
            std::printf("game_endturn=player=%d ok=%d from=%d to=%d day=%d\n",
                        static_cast<int>(mv_human), tb_turn_checked ? 1 : 0,
                        static_cast<int>(before),
                        static_cast<int>(mv_gs.current()), mv_gs.day);
            std::fflush(stdout);
            if (tb_turn_checked) game_turn_changed(true);
        } else {
            tb_turn_checked = !mv_gs.cancel_end_turn(mv_human);
            tb_turn_ai_hold = 0;
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
    // human's button uses. The original spends a real turn on each seat; the
    // port holds each one for the button's change span so the Checked glow is
    // seen while everybody ends, then the day rolls and the single rotation
    // plays. The button never follows these seats (CurPlayer is the seated one).
    void game_turn_tick() {
        if (!game_turns_started || mv_human == 0xff) return;
        if (mv_gs.turn_order.empty()) return;
        const std::uint8_t cur = mv_gs.current();
        if (cur == 0xff || cur == mv_human) return;
        if (tb_turn_animating) return;   // the new-day change is still playing
        if (tb_turn_checked && tb_turn_ai_hold > 0) {
            --tb_turn_ai_hold;
            return;
        }
        if (mv_gs.end_turn(cur)) {
            game_turn_changed(true);
            tb_turn_ai_hold = (mv_gs.current() == mv_human)
                                  ? 0 : kGameAiTurnHoldFrames;
        }
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
    // hexagon sprites from the last placed back to the first). `home_x`/`home_y`
    // receive the hex the object is *stored* on, which differs from the clicked
    // hex for a large building: a capital is one `City.TCity` object on its home
    // hex but its sprite/pad occupies several tiles, so `TMultiHexMO` registers
    // it on every hex its `TTerrainList` covers (see `object_covers_hex`).
    const aow::MapHs* structure_on_hex(int x, int y, int* home_x = nullptr,
                                       int* home_y = nullptr) const {
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
        auto found = [&](const aow::MapHs* hs, int hx, int hy) {
            if (home_x != nullptr) *home_x = hx;
            if (home_y != nullptr) *home_y = hy;
            return hs;
        };
        const aow::MapTile& t = lv.tiles[static_cast<std::size_t>(y) * lv.width +
                                         static_cast<std::size_t>(x)];
        for (auto it = t.objects.rbegin(); it != t.objects.rend(); ++it) {
            if (is_structure_hs(*it, game_res)) return found(&*it, x, y);
        }
        // No object is stored on this tile, but a large building may cover it
        // from a neighbouring home hex. The shipped TTerrainLists reach 65
        // entries (ring 5 of the engine's HN spiral), so search that radius and
        // take the closest structure (later placement wins a tie).
        constexpr int kFootprintRadius = 5;
        const int max_y = std::min(static_cast<int>(lv.height) - 1,
                                   y + kFootprintRadius);
        const int max_x = std::min(static_cast<int>(lv.width) - 1,
                                   x + kFootprintRadius);
        const aow::MapHs* best = nullptr;
        int best_hx = -1, best_hy = -1, best_dist = kFootprintRadius + 1;
        for (int oy = std::max(0, y - kFootprintRadius); oy <= max_y; ++oy) {
            for (int ox = std::max(0, x - kFootprintRadius); ox <= max_x; ++ox) {
                const int d = aow::hex_distance(ox, oy, x, y);
                if (d == 0 || d > kFootprintRadius) continue;
                const aow::MapTile& ot =
                    lv.tiles[static_cast<std::size_t>(oy) * lv.width +
                             static_cast<std::size_t>(ox)];
                for (auto it = ot.objects.rbegin(); it != ot.objects.rend(); ++it) {
                    if (!is_structure_hs(*it, game_res)) continue;
                    if (!aow::object_covers_hex(*it, game_res, ox, oy, x, y)) {
                        continue;
                    }
                    if (best == nullptr || d < best_dist ||
                        (d == best_dist && it->offset > best->offset)) {
                        best = &*it;
                        best_hx = ox;
                        best_hy = oy;
                        best_dist = d;
                    }
                }
            }
        }
        return best == nullptr ? nullptr : found(best, best_hx, best_hy);
    }

    static const char* structure_display_name(const aow::MapHs& hs,
                                              const std::vector<aow::MapResource>& res) {
        // Only a fallback for cities: `open_structure` replaces this with
        // `MapHs::city_name`, the TCity rw tag 0xc name read from the map.
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

    // Every visible box of TIWINDOW.dfm, resolved for the design size (200x216).
    // The text boxes carry the *ink* origin (or centre, see `txt_center`) rather
    // than a component rectangle, because that is what the bitmap font is drawn
    // from; the boxes come from the DFM's Alignment block and were cross-checked
    // against StructurePReference.png (their ink starts at the same offsets).
    struct StructureLayout {
        AowBox win, title, title_icon, help, center, min, close;
        AowBox name, name_text, city, race, status, prod;
        AowBox gold_icon, gold_val, upgrade_icon, level_val, portrait;
        AowBox city_border, city_art, arch, enter, raze, raze_btn;
    };

    StructureLayout structure_layout() const {
        StructureLayout S;
        const int W = 200, H = 216;
        const int x0 = disp_w - W;
        const int y0 = (disp_h >= 432) ? kPartyTop : std::max(0, disp_h - H);
        S.win = AowBox{x0, y0, W, H};
        // Title (Age10, clBlack): the Label's box is (25,4); its glyph ink starts
        // there. The icon art has a 2 px visual margin, so its visual box sits at
        // (4,2) even though SIcon's DFM box is (2,1).
        S.title = AowBox{x0 + 25, y0 + 4, 165, 16};
        S.title_icon = AowBox{x0 + 4, y0 + 2, 19, 16};
        S.help = AowBox{x0 + 138, y0 + 1, 15, 16};
        S.center = AowBox{x0 + 153, y0 + 1, 15, 16};
        S.min = AowBox{x0 + 167, y0 + 1, 15, 16};
        S.close = AowBox{x0 + 181, y0 + 1, 15, 16};
        // NamePnl (4,17 136x27), StructNameLbl (Age10s, clWhite) centred with
        // TopOffset 10. The ink box is (52,27)-(84,35).
        S.name = AowBox{x0 + 4, y0 + 17, 136, 27};
        S.name_text = AowBox{x0 + 69, y0 + 27, 0, 0};
        // CityPanel (4,45 114x138).
        S.city = AowBox{x0 + 4, y0 + 45, 114, 138};
        // CityRaceLbl (Age8s, clBlack), centred, TopOffPercent 53.
        S.race = AowBox{x0 + 61, y0 + 118, 0, 0};
        // CivilStatusLbl (Age8s, clBlack), centred, ahBottom BottomOffset 1.
        S.status = AowBox{x0 + 61, y0 + 171, 0, 0};
        // ActiveCityActionMemo (Age8, clBlack, taCenter) fills CityPanel with a
        // 3 px inset; the first item's ink row is 67.
        S.prod = AowBox{x0 + 12, y0 + 68, 0, 0};
        // InfoPnl (CityPanel+6,48 102x20): GoldIcn (LBIcon 31, 16x13) at +10,2
        // with the income (Age10s white) at +32,+4; UgradeI (LBIcon 1, 11x12)
        // at +61,+3 with the level (Age10s white) at +81,+4.
        S.gold_icon = AowBox{x0 + 20, y0 + 96, 16, 13};
        S.gold_val = AowBox{x0 + 42, y0 + 97, 30, 12};
        S.upgrade_icon = AowBox{x0 + 71, y0 + 97, 11, 12};
        S.level_val = AowBox{x0 + 91, y0 + 97, 22, 12};
        // RaceRelationIcon (CityPanel 32,86 51x36): RelFr 11..18 is a 51x36
        // image whose visible face is 37x36 at +6,0.
        S.portrait = AowBox{x0 + 42, y0 + 131, 51, 36};
        // CityBorderPnl (120,18 80x194): the BCS race banner is 57x278 and its
        // visual box is placed at (131,-94) (source (10,180) lands on the
        // capture at (144,89)); the CeltDeco arch (76x58) starts at (119,19) and
        // repeats every 58 px, covering the panel's left and right edges.
        S.city_border = AowBox{x0 + 120, y0 + 18, 80, 194};
        S.city_art = AowBox{x0 + 135, y0 - 88, 57, 278};
        S.arch = AowBox{x0 + 119, y0 + 19, 76, 58};
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
                if (iwin_race_il.entries()[k].index == kStructureCityArtId) {
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
        // A click may land on any tile a large building's sprite occupies; the
        // floater still belongs to the structure's own (home) hex.
        int home_x = hx, home_y = hy;
        const aow::MapHs* hs = structure_on_hex(hx, hy, &home_x, &home_y);
        if (hs == nullptr) return false;
        game_struct_hx = home_x;
        game_struct_hy = home_y;
        game_struct_city = hs->class_id == aow::kCityClassId;
        game_struct_owner = hs->structure_owner;
        game_struct_name = structure_display_name(*hs, game_res);
        if (game_struct_city) {
            // StructNameLbl.Text = TCity.GetName, i.e. the city's +0x48
            // AnsiString, parsed by the map loader from TCity rw tag 0xc
            // (rwString: one length byte, 0xff => u32 length, then the bytes).
            // The original shows exactly that string - so the floater reads
            // "Venice" for Bloody Marauders' Human capital instead of "City" -
            // and a city whose rw tag is absent has an empty name (the reader
            // clears the field), which the original also shows blank.
            // `--structure-name` overrides it for captures.
            game_struct_name = hs->city_name;
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
        // CityRaceLbl.Text = "Race: %s" (AoWE.RaceX, resourcestring 64846)
        // formatted with `AoWE.RaceNamesRStr[city+0x45]` - the *shipped*
        // singular race name indexed by the signed city race byte, i.e.
        // `human, azrac, lizard, frostling, elf, halfling, dwarf, highman,
        // darkelf, orc, goblin, undead` (see `aow::race_label_singular`).
        // Indexing the port's own readable spellings here (or any other
        // order) mixed the races up: Heartwood Forest's Elves read "Orc"
        // (id 4), its Goblins "Azrac" (10) and its Orcs "Lizardman" (9).
        //
        // An unset race (rw tag 0xd absent, or the editor's 0xff = C_random) is
        // resolved to the owner player's race - the same value the city-body
        // art uses (`city_body_race`); the original indexes the raw byte and is
        // out of bounds there. When that also yields no race id (the
        // independents' roster entry carries 255) the label falls back to race
        // 0 as before, while the BCS view keeps using the raw index.
        const int race_label_id =
            (game_struct_race_index >= 0 && game_struct_race_index < 12)
                ? game_struct_race_index
                : 0;
        game_struct_race = aow::race_label_singular(
            static_cast<std::uint8_t>(race_label_id));
        game_struct_status = "Cheerful";
        game_struct_production = game_struct_city ? "Producing Merchandise"
                                                  : "Idle";
        game_struct_gold = game_struct_city ? (10 + game_struct_race_index) : 0;
        game_struct_mana = game_struct_city ? (2 + game_struct_race_index / 4) : 0;
        // The right-hand view: `TRaceResource` BCS_<race>.BMP, rw id 42 (the
        // underground BCU banner is 45). Loaded lazily because a scenario uses
        // only a few race libraries.
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
                    if (iwin_race_il.entries()[k].index == kStructureCityArtId) {
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
        // TStructure.Select leaves the building pulsing: build the background
        // without this object, then re-blend the object over it every frame.
        mv_prepare_structure_fade(game_level, hs->offset);
        mv_refresh_draw();
        play_game_sfx("Sfx\\Int\\Btn1.wav");
        return true;
    }

    // --- the city production model -----------------------------------------
    // `AoWE.TUnitProductionList` entries carry a Unitres record id; resolve it
    // through the already-loaded `UnitCatalog` (the same join TUnitResource.
    // GetUnitName does in the original).
    const aow::UnitEntry* city_unit_entry(int id) const {
        for (const aow::UnitEntry& u : mv_cat.units) {
            if (static_cast<int>(u.id) == id) return &u;
        }
        return nullptr;
    }

    std::string city_unit_name(int id) const {
        const aow::UnitEntry* u = city_unit_entry(id);
        if (u != nullptr && !u->display_name.empty()) return u->display_name;
        return "Unit " + std::to_string(id);
    }

    // The TCity the screen is showing (`game_city_hx/hy` on the current level).
    // The unit page asks it for UnitProductionGold/Time and the size term of
    // the production timer, exactly like `FUN_0043c43c` reads TCity+0x50.
    const aow::MapHs* city_current_hs() const {
        if (game_level < 0 || static_cast<std::size_t>(game_level) >=
                                  game_state_map.levels.size()) {
            return nullptr;
        }
        const aow::MapLevel& lv =
            game_state_map.levels[static_cast<std::size_t>(game_level)];
        if (game_city_hx < 0 || game_city_hy < 0 ||
            static_cast<std::uint32_t>(game_city_hx) >= lv.width ||
            static_cast<std::uint32_t>(game_city_hy) >= lv.height) {
            return nullptr;
        }
        const aow::MapTile& tile =
            lv.tiles[static_cast<std::size_t>(game_city_hy) * lv.width +
                     static_cast<std::size_t>(game_city_hx)];
        for (const aow::MapHs& hs : tile.objects) {
            if (hs.class_id == aow::kCityClassId) return &hs;
        }
        return nullptr;
    }

    // `TAlignmentRStr` (AoWE's resourcestring array) indexed by
    // `GetAlignment & 0x7f`; the unit page writes "Alignment: %s".
    static const char* city_alignment_name(int a) {
        static const char* const kNames[7] = {
            "Pure Good", "Good", "Pure Neutral", "Neutral",
            "Evil", "Pure Evil", "None"};
        const int i = a & 0x7f;
        return (i >= 0 && i < 7) ? kNames[i] : "Neutral";
    }

    // `TAbility.GetName` (AbilityP.dpl's AoWE.<name>Str resourcestrings).
    // The English texts live in AoWEPACK.dpl's RT_STRING table; each concrete
    // `T*Ability.Create` assigns `this+0xc` the ability id and `this+8` the
    // loaded caption (e.g. `TTurnUndeadAbility.Create@5576ad20` -> id 0x26,
    // "Turn Undead"; `THealingAbility.Create@5576be3c` -> id 0x2f, "Healing";
    // `TDispelMagicAbility.Create@5576cd68` -> id 0x3c, "Dispel Magic"). The id
    // is `Ability.pfs tag - 10`, not the record index: tag 43 is absent, so
    // 34 = Turned Undead, 35 = Invoke Death, 36 = Wall Climbing. Unknown ids
    // return nullptr: ListAbilities skips an ability whose GetName is empty.
    static const char* city_ability_name(int id) {
        switch (id) {
        case 0: return "Walking";
        case 1: return "Flying";
        case 2: return "Swimming";
        case 3: return "Forestry";
        case 4: return "Cave Crawling";
        case 5: return "Mountaineering";
        case 6: return "Magic Immunity";
        case 7: return "Fire Immunity";
        case 8: return "Cold Immunity";
        case 9: return "Lightning Immunity";
        case 10: return "Poison Immunity";
        case 11: return "Death Immunity";
        case 12: return "Holy Immunity";
        case 13: return "Physical Immunity";
        case 14: return "Fire Strike";
        case 15: return "Cold Strike";
        case 16: return "Lightning Strike";
        case 17: return "Poison Strike";
        case 18: return "Death Strike";
        case 19: return "Holy Strike";
        case 20: return "Magic Strike";
        case 21: return "Strike";
        case 22: return "Archery";
        case 23: return "Sailing";
        case 24: return "Hurl Boulder";
        case 25: return "Hurl Stones";
        case 26: return "Trail Of Darkness";
        case 27: return "Web";
        case 28: return "Dominate";
        case 29: return "Seduce";
        case 30: return "Flame Throwing";
        case 31: return "Call Flames";
        case 32: return "Marksmanship";
        case 34: return "Turned Undead";
        case 35: return "Invoke Death";
        case 36: return "Wall Climbing";
        case 37: return "Bard's Skills";
        case 38: return "Turn Undead";
        case 39: return "Night Vision";
        case 40: return "Entangle";
        case 41: return "True Seeing";
        case 42: return "Tunneling";
        case 43: return "Ignition";
        case 44: return "Fire Cannon";
        case 45: return "Fire Musket";
        case 46: return "Leadership";
        case 47: return "Healing";
        case 48: return "Seduced";
        case 49: return "Regeneration";
        case 50: return "Transport";
        case 51: return "Cause Fear";
        case 52: return "Spell Casting";
        case 54: return "Invisibility";
        case 55: return "Doom Gaze";
        case 57: return "Shoot Javelin";
        case 58: return "Shoot Black Javelin";
        case 59: return "Floating";
        case 60: return "Dispel Magic";
        case 61: return "Poison Darts";
        case 62: return "Venomous Spit";
        case 63: return "Dragon";
        case 64: return "Vision";
        case 65: return "Self Destruct";
        case 66: return "Pass Wall";
        case 67: return "Fearless";
        case 68: return "Path Of Decay";
        case 69: return "Path Of Life";
        case 70: return "Death Protection";
        case 71: return "Fire Protection";
        case 72: return "Holy Protection";
        case 73: return "Poison Protection";
        case 74: return "Lightning Protection";
        case 75: return "Magic Protection";
        case 76: return "Cold Protection";
        case 77: return "Physical Protection";
        case 86: return "Fire Breath";
        case 87: return "Cold Breath";
        case 88: return "Black Breath";
        case 89: return "Divine Breath";
        case 90: return "Gas Breath";
        case 92: return "Stunned";
        case 93: return "Cursed";
        case 94: return "Entangled";
        case 95: return "Frozen";
        case 96: return "Poisoned";
        case 97: return "Webbed";
        case 98: return "Vertigo";
        case 99: return "Nature's Blessing";
        case 100: return "Blessed";
        case 101: return "Crusader";
        case 106: return "Possess";
        case 107: return "Possessed";
        case 108: return "Panicked";
        case 109: return "Path Of Frost";
        case 111: return "Charge";
        case 112: return "Dragon Slaying";
        case 113: return "Parry";
        case 114: return "Round Attack";
        case 115: return "Extra Strike";
        case 116: return "First Strike";
        case 117: return "Wall Crushing";
        case 118: return "Life Stealing";
        case 119: return "Entangle Strike";
        case 120: return "Black Bolts";
        case 121: return "Magic Bolts";
        case 122: return "Frost Bolts";
        case 123: return "Lightning Bolts";
        case 124: return "Holy Bolts";
        case 125: return "Construct";
        case 127: return "Burning";
        case 128: return "Resurrected";
        case 129: return "Animated";
        case 130: return "Resurrected";
        case 131: return "Decay";
        case 132: return "Slow";
        case 138: return "Steppe Concealment";
        case 139: return "Underground Concealment";
        case 140: return "Grass Concealment";
        case 141: return "Snow Concealment";
        case 142: return "Desert Concealment";
        case 143: return "Wasteland Concealment";
        case 144: return "Water Concealment";
        case 145: return "Concealment";
        case 146: return "Holy Champion";
        case 147: return "Unholy Champion";
        case 148: return "Charm";
        case 149: return "Charmed";
        case 150: return "Dominated";
        case 152: return "Haste";
        case 153: return "Stone Skin";
        case 154: return "Enchanted Weapon";
        case 155: return "Summoned";
        case 156: return "Fury";
        case 157: return "Free Movement";
        case 168: return "Bloodlust";
        default: return nullptr;
        }
    }

    // The level-carrying `T*Ability.GetName` overrides append a fixed suffix on
    // top of the base caption. TTurnUndeadAbility.GetLevelName@5576af3c /
    // TDispelMagicAbility.GetLevelName@5576cf3c / TMarksmanshipAbility and
    // TVisionAbility all use the DAT_5576b068.. constants " I".." IV".
    static const char* city_ability_level_suffix(int id, int level) {
        switch (id) {
        case 32:  // Marksmanship
        case 38:  // Turn Undead
        case 52:  // Spell Casting
        case 60:  // Dispel Magic
        case 64:  // Vision
            break;
        default:
            return "";
        }
        switch (level) {
        case 1: return " I";
        case 2: return " II";
        case 3: return " III";
        case 4: return " IV";
        default: return "";
        }
    }

    // `FUN_004541b0(GetMoveTypes)`: picks the IconMove id TCityScreen puts in
    // UMovesIcon. The unit's move-type bits come from its movement abilities
    // (TAbstractUnit.GetMoveTypes); each test overwrites the previous one, so
    // the order below is the original's.
    static int city_move_icon(std::uint8_t mask) {
        int v = 0;
        if ((mask & 8) == 8) v = 2;
        if ((mask & 0x20) == 0x20) v = 4;
        if ((mask & 4) == 4) v = 6;
        if ((mask & 5) == 5) v = 8;
        if ((mask & 0xc) == 0xc) v = 10;
        if ((mask & 0x24) == 0x24) v = 12;
        if ((mask & 0x28) == 0x28) v = 14;
        if ((mask & 0x2c) == 0x2c) v = 16;
        if ((mask & 2) == 2) v = 18;
        return v;
    }

    // `TUnit.GetFace@5578283c` -> `TUnitGFXResource.GetFaceImage@55783b00`:
    // the unit-gfx record's image library (`FACES\R_FACES.ILB`) at its face
    // index. `UFaceDraw@0043dc0c` then stretches it into UPictureFramePnl. That
    // handler resolves the face of the card that is checked at draw time
    // (`TCityScreen+0x2b0` -> city+0x50 GetItem(+0x2c4)), so the sprite has to
    // follow the selection; `city_unit_face_id` is the cache key.
    bool city_load_unit_face(int unit_id) {
        if (city_unit_face_id == unit_id && city_unit_face.ok) return true;
        city_unit_face = Sprite();
        city_unit_face_id = -1;
        const aow::UnitEntry* u = city_unit_entry(unit_id);
        if (u == nullptr || u->gfx == nullptr) return false;
        const std::string path = face_lib_path(u->gfx->image_lib);
        if (path.empty()) return false;
        if (city_unit_face_path != path) {
            std::string lerr;
            if (!city_unit_face_il.load(path, lerr)) return false;
            city_unit_face_path = path;
        }
        const std::vector<aow::IlbEntry>& ents = city_unit_face_il.entries();
        for (std::size_t i = 0; i < ents.size(); ++i) {
            if (ents[i].index != u->gfx->face_index) continue;
            std::uint32_t w = 0, h = 0;
            std::string derr;
            if (!city_unit_face_il.decode_rgb(i, city_unit_face.rgba, w, h,
                                              derr)) {
                return false;
            }
            city_unit_face.w = w;
            city_unit_face.h = h;
            city_unit_face.ok = true;
            city_unit_face_id = unit_id;
            // The face pictures are class 0x10 (no colour key); UFaceDraw
            // stretches them across the frame's inner rect.
            city_unit_face.base_x = 0;
            city_unit_face.base_y = 0;
            city_unit_face.has_key = false;
            return true;
        }
        return false;
    }

    // Resolve the checked production card into the values `FUN_0043c43c`
    // writes into UnitPnl. Selecting a card, a queue row, or (re)loading the
    // production list calls this; the renderer is then a pure view.
    void city_unit_page_refresh() {
        CityUnitPage p;
        city_unit_abil_top = 0;
        if (city_prod_sel < 0 ||
            static_cast<std::size_t>(city_prod_sel) >= city_prod_units.size()) {
            city_unit_page = p;
            return;
        }
        const aow::CityProductionUnit& card =
            city_prod_units[static_cast<std::size_t>(city_prod_sel)];
        const aow::UnitEntry* u = city_unit_entry(card.unit_id);
        if (u == nullptr) {
            city_unit_page = p;
            return;
        }
        p.valid = true;
        p.unit_id = card.unit_id;
        p.title = (card.flag ? "Produce " : "Install ") +
                  city_unit_name(card.unit_id);
        p.attack = u->attack;
        p.defense = u->defense;
        p.damage = u->damage;
        p.resistance = u->resistance;
        p.hits = u->hit_points;
        p.moves = u->moves;
        const std::array<bool, 256> has =
            aow::decode_ability_bitmap(u->ability_mask, u->ability_count);
        p.move_icon = city_move_icon(aow::move_types_from_abilities(has));
        // UnitProductionGold / UnitProductionTime for the card's kind (1
        // Produce, 2 Install) - the same pair the Queue button charges.
        const int kind = card.flag ? 1 : 2;
        const aow::MapHs* hs = city_current_hs();
        p.cost = hs != nullptr
                     ? city_production_gold(*hs, kind, city_prod_sel)
                     : city_unit_gold(card.unit_id);
        p.turns = hs != nullptr
                      ? city_production_time(*hs, kind, city_prod_sel)
                      : city_unit_level(card.unit_id);
        p.alignment = city_alignment_name(u->alignment);
        p.level = u->level;
        // TAbstractUnit.GetUpkeep@5577fc64: (level+1)*2, 0 with ability 0x9b.
        p.upkeep = has[0x9b] ? 0 : (u->level + 1) * 2;
        for (int i = 0; i < static_cast<int>(u->ability_count) && i < 256; ++i) {
            if (!has[static_cast<std::size_t>(i)]) continue;
            const char* name = city_ability_name(i);
            if (name == nullptr) continue;
            const int ability_level =
                static_cast<std::size_t>(i) < u->ability_levels.size()
                    ? u->ability_levels[static_cast<std::size_t>(i)]
                    : 0;
            if (i == 50 && ability_level > 0) {
                // TTransportAbility.GetLevelName@557660a0 formats the
                // `TransportXUnitsRStr` caption "Transport (%s units)".
                p.abilities.push_back("Transport (" +
                                      std::to_string(ability_level) + " units)");
            } else {
                p.abilities.push_back(std::string(name) +
                                      city_ability_level_suffix(i, ability_level));
            }
        }
        city_unit_page = p;
        (void)city_load_unit_face(card.unit_id);
    }

    // `City.ProductionIDToCityAction@557a7ce4` maps a TProductionItem+8
    // (`TProductionControl.Produce@5575d95c` / `ProduceUnitEx@5575d670` /
    // `ResearchUnit@5575d870` write it) to the CityBtn1Click switch value:
    //
    //   1  produce a unit              -> 8  (TCityScreen case 8, "Produce")
    //   2  install / research a unit    -> 7  (case 7, "Install")
    //   10 produce merchandise (trade)  -> 1
    //   11 upgrade                      -> 2
    //   12 fortify                      -> 3
    //   13 migrate                      -> 4
    //   14 loot                         -> 5
    //
    // `City.CityActionToProductionID@557a7d44` is the inverse used by the
    // city's Trade/Upgrade/Fortify/Migrate/Loot methods when they queue.
    static int city_queue_action(int kind) {
        switch (kind) {
        case 1: return 8;
        case 2: return 7;
        case 10: return 1;
        case 11: return 2;
        case 12: return 3;
        case 13: return 4;
        case 14: return 5;
        default: return 0;
        }
    }

    static int city_action_to_production_id(int action) {
        switch (action) {
        case 1: return 10;
        case 2: return 11;
        case 3: return 12;
        case 4: return 13;
        case 5: return 14;
        default: return 0;
        }
    }

    static bool city_production_is_unit(int kind) {
        return kind == 1 || kind == 2;
    }

    // `TProductionControl.GetProductionName@5575de04` (unit kinds) and
    // `TCityProductionControl.GetProductionName@557a8acc` (city actions).
    std::string city_production_item_name(int kind, int index) const {
        if (city_production_is_unit(kind)) {
            const bool has_unit =
                index >= 0 && static_cast<std::size_t>(index) <
                                  city_prod_units.size();
            std::string name = has_unit
                ? city_unit_name(city_prod_units[
                      static_cast<std::size_t>(index)].unit_id)
                : "Unknown";
            return (kind == 1 ? "Produce " : "Install ") + name;
        }
        switch (kind) {
        case 10: return "Produce Merchandise";
        case 11: return "Upgrade";
        case 12: return "Fortify";
        case 13:
            return "Migrate to " + aow::race_label_plural(
                static_cast<std::uint8_t>(index < 0 ? 0 : index));
        case 14: return "Loot";
        default: return "Unknown";
        }
    }

    // `TProductionControl.GetProductionNameProgressive@5575dfe4`: the
    // "Producing X" / "Installing X" line the overview's IncomeMemo shows.
    std::string city_production_item_progressive(int kind, int index) const {
        if (city_production_is_unit(kind)) {
            const bool has_unit =
                index >= 0 && static_cast<std::size_t>(index) <
                                  city_prod_units.size();
            std::string name = has_unit
                ? city_unit_name(city_prod_units[
                      static_cast<std::size_t>(index)].unit_id)
                : "Unknown";
            return (kind == 1 ? "Producing " : "Installing ") + name;
        }
        switch (kind) {
        case 10: return "Producing Merchandise";
        case 11: return "Upgrading the city";
        case 12: return "Fortifying the city";
        case 13: return "Migrating the city";
        case 14: return "Looting the city";
        default: return "Producing";
        }
    }

    // --- unit production cost/time (the recovered getters) -------------------
    // `TCityProductionControl.UnitProductionTime@557a913c`:
    //   level - City.GetSize()/2 + 1, clamped to >= 1.
    // `TProductionControl.UnitResearchTime@5575d7b8` (install) = level.
    // `UnitProductionGold/UnitResearchGold` both return the Unitres record's
    // +0x4c, i.e. tag 27 (`TUnitResource.GetObtainValue@55784d58`); the port's
    // UnitEntry stores it as `obtain_value`. Mana is 0 for a city: the
    // `TProductionPlaceControl.UnitProductionGold/Mana` pair sends the cost to
    // the gold slot when the production place is a city (GetType == 8).
    int city_unit_level(int unit_id) const {
        const aow::UnitEntry* u = city_unit_entry(unit_id);
        return u != nullptr && u->level > 0 ? static_cast<int>(u->level) : 1;
    }

    int city_unit_gold(int unit_id) const {
        const aow::UnitEntry* u = city_unit_entry(unit_id);
        return u != nullptr ? static_cast<int>(u->obtain_value) : 0;
    }

    // `TCity.GetSize@557aa938` (docs/26_CITY_SCREEN.md): the number of
    // TTerrainList entries whose terrain byte is >= 0 as a signed char, i.e.
    // the city resource's covered hexes (City1..City4 = 1..4).
    int city_size_hexes(const aow::MapHs& hs) const {
        const aow::MapResource* r =
            aow::find_map_resource(game_res, hs.resource);
        if (r == nullptr) return 1;
        int n = 0;
        for (std::uint16_t e : r->terrain_list) {
            if (static_cast<std::int8_t>(e & 0xff) >= 0) ++n;
        }
        return n > 0 ? n : 1;
    }

    int city_production_time_for(
        const std::vector<aow::CityProductionUnit>& units,
        const aow::MapHs& hs, int kind, int index) const {
        if (kind == 1) {
            const int unit_id =
                (index >= 0 &&
                 static_cast<std::size_t>(index) < units.size())
                    ? units[static_cast<std::size_t>(index)].unit_id
                    : 0;
            const int t = city_unit_level(unit_id) - city_size_hexes(hs) / 2 + 1;
            return t < 1 ? 1 : t;
        }
        if (kind == 2) {
            if (index < 0 || static_cast<std::size_t>(index) >= units.size()) {
                return 1;
            }
            const int lvl =
                city_unit_level(units[static_cast<std::size_t>(index)].unit_id);
            return lvl < 1 ? 1 : lvl;
        }
        switch (kind) {
        case 10: return 1;                   // ExecuteProduction: trade
        case 11: return 3;                   // TCity.UpgradeTime@557aabcc
        case 12: return 2;                   // TCity.FortifyTime@557aad78
        case 13: return 2;                   // TCity.MigrationTime
        case 14: return city_size_hexes(hs);  // TCity.LootTime@557ab024
        default: return 1;
        }
    }

    int city_production_time(const aow::MapHs& hs, int kind,
                             int index) const {
        return city_production_time_for(city_prod_units, hs, kind, index);
    }

    // The gold `ExecuteProduction@5575d118` charges when it starts an item
    // (`CanExecuteProduction@5575cf4c` checks it first).
    int city_production_gold_for(
        const std::vector<aow::CityProductionUnit>& units,
        const aow::MapHs& hs, int kind, int index) const {
        if (kind == 1 || kind == 2) {
            if (index < 0 || static_cast<std::size_t>(index) >= units.size()) {
                return 0;
            }
            return city_unit_gold(
                units[static_cast<std::size_t>(index)].unit_id);
        }
        if (kind == 11) return 250;  // TCity.UpgradeCost: flat 250 gold
        if (kind == 12) {
            // TCity.FortifyCost: 20/40/60/80 by size while the wall is below
            // stone (2); a stone wall cannot be fortified further.
            if (hs.wall_type >= 2) return -1;
            return city_size_hexes(hs) * 20;
        }
        return 0;
    }

    int city_production_gold(const aow::MapHs& hs, int kind,
                             int index) const {
        return city_production_gold_for(city_prod_units, hs, kind, index);
    }

    // `TProductionControl.LoopableProduction@5575ddfc` + the city override:
    // only producing a unit or merchandise may repeat; installing a unit and
    // the Upgrade/Fortify/Migrate/Loot actions do not.
    static bool city_production_loopable(int kind) {
        return kind == 1 || kind == 10;
    }

    // `GetQueueLocked@5575cd20`: full, or a queued Migrate/Loot (the city
    // override's LockQueueProduction) disables the Queue button.
    bool city_queue_locked() const {
        if (city_queue.size() >= 5) return true;
        for (const aow::CityQueueItem& q : city_queue) {
            const int a = city_queue_action(q.kind);
            if (a == 4 || a == 5) return true;
        }
        return false;
    }

    // `TProductionControl.ReadWrite@5575dc18` serialises TProductionControl+0x24
    // as rw tag 0x16. That object is a `TProductionSettings`
    // (`TProductionSettings.ReadWrite@5575c8c4`), whose +8/+0xc/+0x10 are the
    // paid gold/mana and the turns left. `NewTurn@5575dd00` ticks +0x10 and
    // `FUN_0043f574` reads it, so the mirror follows the live values every time
    // they change. It is copied even while the state is 0: ProductionDone
    // keeps the finished item's +8/+0xc/+0x1c (only +0x10 is left at 0), while
    // ExecuteCancelProduction clears the settings, and both cases already have
    // the right values in the live fields.
    static void city_sync_settings_mirror(aow::MapHs& hs) {
        hs.production_current.kind = hs.production_gold_paid;   // +8
        hs.production_current.index = hs.production_mana_paid;  // +0xc
        hs.production_current.extra = hs.production_turns_left; // +0x10
        hs.production_current_present = true;
    }

    // Builds the Scene's copy of `City.TCity` rw tag 0x12: the unit list (the
    // 12 cards), the queue rows and LoopQueueBox. The card art is decoded once
    // here (`PreDrawProductions` draws the unit's standing pose over the
    // CityWin frame); a city with no production control keeps the reference
    // page's cards.
    void load_city_production(const aow::MapHs* hs) {
        city_prod_units.clear();
        city_queue.clear();
        city_prod_names.clear();
        city_queue_names.clear();
        city_card_art.clear();
        city_prod_sel = -1;
        city_loop = false;
        city_queue_edit = false;
        city_queue_sel = -1;
        city_map_hs = const_cast<aow::MapHs*>(hs);
        city_prod_state = 0;
        city_prod_turns_left = 0;
        city_prod_turns_total = 0;
        city_prod_gold_paid = 0;
        city_prod_mana_paid = 0;
        if (hs == nullptr) return;
        city_prod_units = hs->production_units;
        city_queue = hs->production_queue;
        city_loop = hs->loop_queue;
        city_repeat = city_loop;
        for (const aow::CityProductionUnit& u : city_prod_units) {
            city_prod_names.push_back(city_unit_name(u.unit_id));
            // The card's kind is `TUnitProduction+0xc`: non-zero draws the
            // CityWin 4 face with the unit at facing 3, zero draws the CityWin
            // 5 face, the unit at facing 5 and the 45 % blend wash over it.
            std::printf("city_production_card=%u unit=%d kind=%d name=[%s]\n",
                        static_cast<unsigned>(city_prod_names.size() - 1),
                        u.unit_id, u.flag,
                        city_prod_names.back().c_str());
        }
        std::fflush(stdout);
        for (const aow::CityQueueItem& q : city_queue) {
            city_queue_names.push_back(
                city_production_item_name(q.kind, q.index));
        }
        // `TProductionControl+0x20` and the `+0x24` settings object as the
        // rw tags 0x17/0x16 load them. An active item stores its paid gold in
        // +8, paid mana in +0xc, the turns left in +0x10 and the total in
        // +0x1c (ExecuteProduction@5575d118 / TProductionSettings). A map that
        // only carries the queue (state 0/1) has no running timer.
        city_prod_state = hs->production_state;
        if (!city_queue.empty()) {
            const aow::CityQueueItem& front = city_queue.front();
            const int total =
                city_production_time(*hs, front.kind, front.index);
            if (city_prod_state == 2) {
                // Take the live timer, not the queue row: the row's +0x10 is
                // a TProductionItem field that is never written for a queue
                // entry, while +0x10 of tag 0x16 is the byte NewTurn ticks.
                city_prod_turns_left = hs->production_turns_left;
                city_prod_turns_total = hs->production_turns_total;
                city_prod_gold_paid = hs->production_gold_paid;
                city_prod_mana_paid = hs->production_mana_paid;
                if (city_prod_turns_total <= 0) {
                    // A scenario map can carry state 2 without a total (only
                    // ExecuteProduction writes +0x1c); recover it like the
                    // start path does.
                    city_prod_turns_total = total;
                }
                if (city_prod_gold_paid < 0 || city_prod_gold_paid > 1000000) {
                    city_prod_gold_paid =
                        city_production_gold(*hs, front.kind, front.index);
                }
                if (city_prod_mana_paid < 0 || city_prod_mana_paid > 1000000) {
                    city_prod_mana_paid = 0;
                }
                // +0x10 is a signed byte: 0 is the "UnitProduced retry" state
                // NewTurn leaves for this turn, and -1 hides the label
                // (`FUN_0043f574`). Anything larger than the total is stale.
                if (city_prod_turns_left < 0 && city_prod_turns_left != -1) {
                    city_prod_turns_left = 0;
                }
                if (city_prod_turns_left > city_prod_turns_total) {
                    city_prod_turns_left = city_prod_turns_total;
                }
                // The cleaned-up values replace the raw bytes (`city_map_hs` is
                // the non-const view of `hs` taken above).
                city_map_hs->production_turns_left = city_prod_turns_left;
                city_map_hs->production_turns_total = city_prod_turns_total;
                city_map_hs->production_gold_paid = city_prod_gold_paid;
                city_map_hs->production_mana_paid = city_prod_mana_paid;
                city_sync_settings_mirror(*city_map_hs);
            } else if (city_prod_state == 1) {
                city_prod_turns_total = total;
            }
        } else {
            city_prod_state = 0;
        }
        if (city_prod_state != 0) {
            std::printf("city_production_load=state=%u turns_left=%d total=%d "
                        "gold_paid=%d mana_paid=%d\n",
                        static_cast<unsigned>(city_prod_state),
                        city_prod_turns_left, city_prod_turns_total,
                        city_prod_gold_paid, city_prod_mana_paid);
            std::fflush(stdout);
        }
        // One standing pose per card. `PreDrawProductions` resolves it with the
        // draw-info record's facing (3 for a produce item, 5 for an install
        // item) and draws it at `(cell.Left - 4, cell.Top - 4)` plus the
        // *frame's* base+shift only (`aow::unit_pose_sprite_rgba`): the
        // sequence's own xoff/yoff belong to the map's hex anchor, not here. No
        // cropping and no re-centring either - those offsets are what the
        // reference capture shows.
        for (const aow::CityProductionUnit& u : city_prod_units) {
            CityCardArt art;
            aow::MapUnit mu;
            mu.class_id = aow::kMapUnitClassId;
            mu.resource = u.unit_id;
            std::vector<std::uint8_t> rgba;
            int aw = 0, ah = 0, vx = 0, vy = 0;
            std::string err;
            const int facing =
                u.flag ? kCityCardFaceProduce : kCityCardFaceInstall;
            if (aow::unit_pose_sprite_rgba(mv_cat, root_dir, mu, facing, 0, 0,
                                           rgba, aw, ah, vx, vy, err) &&
                aw > 0 && ah > 0) {
                art.s.rgba = std::move(rgba);
                art.s.w = static_cast<std::uint32_t>(aw);
                art.s.h = static_cast<std::uint32_t>(ah);
                art.s.ok = true;
                art.vx = vx;
                art.vy = vy;
            }
            city_card_art.push_back(std::move(art));
        }
        // The unit page is a view of the list; it starts empty (no card
        // checked) and is filled by each card/queue selection.
        city_unit_page_refresh();
    }

    // TIWindow.MainBtnClick's city branch: hand the selected TCity to
    // TCityScreen. `ShowCity` (0x0043af24) first fills the activity panels
    // (FUN_0043b208) from the city - name, population/race, civil status,
    // income and enhancements - then the production row and the city map. The
    // port snapshots the same values so the screen is a pure view of this state.
    bool open_city_screen(int hx, int hy) {
        if (!open_structure(hx, hy)) return false;
        if (!game_struct_city) return false;
        game_show_structure = false;
        game_struct_hover = game_struct_pressed = -1;
        mv_clear_structure_fade();
        if (!game_px.empty()) mv_refresh_draw();
        game_show_city = true;
        game_city_hx = game_struct_hx;
        game_city_hy = game_struct_hy;
        city_name = game_struct_name.empty() ? "City" : game_struct_name;
        const int race = (game_struct_race_index >= 0 &&
                          game_struct_race_index < 12)
                             ? game_struct_race_index
                             : 0;
        // PopulationHdr = "Population:" CR "Humans" CR "Status:" CR "Cheerful"
        // (FUN_0043b208 formats AoWE.PopulationRStr with the *plural* race name
        // and AoWE.StatusRStr with TCity.GetCivilStatus).
        city_population = aow::race_label_plural(
            static_cast<std::uint8_t>(race));
        city_status = game_struct_status.empty() ? "Cheerful"
                                                 : game_struct_status;
        // PopulationMemo = TCity.ListCivilStatusInfo: the relation to the race
        // and the occupation force size. The port has no diplomacy/occupation
        // model yet, so the shipped reference city's values are the defaults.
        city_race_lines = "Race Relation: Friendly\rOccupation Forces: Weak";
        city_gold = 38;
        city_base_income = 20;
        city_crop_fields = 5;
        city_crop_income = 10;
        city_prod_income = 6;
        city_income_header = "Income\r" + std::to_string(city_gold) + " gold";
        city_income_lines = "Base Income\r" + std::to_string(city_crop_fields) +
                            " Crop Fields\rProducing Merchandise";
        city_income_gold = std::to_string(city_base_income) + "\r" +
                           std::to_string(city_crop_income) + "\r" +
                           std::to_string(city_prod_income);
        // EnhancementsHdr / EnhancementsMemo: ListEnhancementsInfo.
        aow::MapHs* city_hs = nullptr;
        if (game_level >= 0 &&
            static_cast<std::size_t>(game_level) < game_state_map.levels.size()) {
            aow::MapLevel& lv =
                game_state_map.levels[static_cast<std::size_t>(game_level)];
            if (game_city_hx >= 0 && game_city_hy >= 0 &&
                static_cast<std::uint32_t>(game_city_hx) < lv.width &&
                static_cast<std::uint32_t>(game_city_hy) < lv.height) {
                aow::MapTile& t =
                    lv.tiles[static_cast<std::size_t>(game_city_hy) * lv.width +
                             static_cast<std::size_t>(game_city_hx)];
                for (aow::MapHs& o : t.objects) {
                    if (o.class_id == aow::kCityClassId) city_hs = &o;
                }
            }
        }
        load_city_production(city_hs);
        city_upgrade_level = city_hs != nullptr && city_hs->city_level > 0
                                 ? city_hs->city_level
                                 : 2;
        const int wall =
            city_hs != nullptr ? static_cast<int>(city_hs->wall_type) : -1;
        city_wall = wall >= 2 ? "Stone Wall" : (wall == 1 ? "Wooden Wall"
                                                          : "No Wall");
        city_enh_header = "Enhancements";
        city_enh_lines = "Upgrade Level: " + std::to_string(city_upgrade_level) +
                         "\r" + city_wall;
        // TCityScreen.ShowCity -> TProductionControl.GetProductionName for the
        // queue's first item; a city with no queue produces merchandise.
        city_queue_sync_production();
        city_queue_sel = -1;
        city_change = false;          // +0x2cc = 0: the overview page
        city_activity = 0;            // B1CB Tag
        city_hover = city_pressed = -1;
        std::printf("city_screen=%d,%d city=1 name=[%s] race=[%s] level=%d "
                    "art=%d/%d\n",
                    game_city_hx, game_city_hy, city_name.c_str(),
                    city_population.c_str(), city_upgrade_level,
                    city_bg.ok ? 1 : 0, city_bg_clean.ok ? 1 : 0);
        std::fflush(stdout);
        play_game_sfx("Sfx\\Int\\Btn1.wav");
        return true;
    }

    void close_city_screen() {
        if (!game_show_city) return;
        city_store_to_map();
        city_map_hs = nullptr;
        game_show_city = false;
        city_change = false;
        city_queue_edit = false;
        city_activity = 0;
        city_prod_sel = -1;
        city_unit_page_refresh();
        city_hover = city_pressed = -1;
        game_city_hx = game_city_hy = -1;
        std::printf("city_screen=closed\n");
        std::fflush(stdout);
    }

    void close_structure() {
        game_show_structure = false;
        game_struct_hx = game_struct_hy = -1;
        game_struct_hover = game_struct_pressed = -1;
        game_struct_art = Sprite();
        game_struct_art_lib.clear();
        // Restore the normal (fully opaque) building sprite.
        mv_clear_structure_fade();
        if (!game_px.empty()) mv_refresh_draw();
    }

    // TCityScreen's controls. The form is modal, so a click anywhere else is
    // swallowed (returns -1) instead of reaching the frozen bar/map behind it.
    // Overview (mode 0): 2001 Change, 2002 PPrev, 2003 PNext, 2004 Exit,
    // 2005 LoopQueueBox, 2010+i QueueLB row i.
    // Production picker (mode 1): 2001 MainBtn1 (the action), 2002 Queue,
    // 2004 Done, 2010+i QueueLB row i, 2100+i B1CB..B6CB activity cells,
    // 2200+i P1CB..P12CB production cells.
    int city_hit_test(int x, int y) const {
        auto in = [](int bx, int by, int bw, int bh, int px, int py) {
            return bx <= px && px < bx + bw && by <= py && py < by + bh;
        };
        if (city_change) {
            // B1CB..B6CB: ActivityPnl2 at (167,357); each cell is WinLeft
            // 51*Tag, WinTop 1, 52 wide and 64 tall (the 48x60 Is-* card is
            // blitted at +2,+2). This is ActivityPnl2's actual bottom row -
            // the earlier 51x65 at y=357 mixed the panel and the cell.
            // Page 04's MigratePnl covers this row entirely (the capture shows
            // its picture/lists there), so the cells are not clickable there.
            if (city_activity != 3) {
                for (int i = 0; i < 6; ++i) {
                    if (in(167 + i * 51, 358, 52, 64, x, y)) return 2100 + i;
                }
            }
            // P1CB..P12CB: ActivityPnl2? No - the cards live in ActivityPnl
            // (WinLeft 11, WinTop 164); P1CB is WinLeft 2 + 51*Tag, WinTop 0,
            // 52x52. The visible row is y=164..216, *not* 203..255.
            for (int i = 0; i < 12; ++i) {
                if (in(13 + i * 51, 164, 52, 52, x, y)) return 2200 + i;
            }
            // RightPnl: MainBtn1 (action / Delete), CityBtn2 (Queue /
            // Execute), CityBtn3 (Done).
            const bool unit_sel =
                city_prod_sel >= 0 &&
                static_cast<std::size_t>(city_prod_sel) < city_prod_units.size();
            if (city_queue_edit) {
                if (in(475, 358, 153, 21, x, y)) {
                    return city_queue_sel >= 0 ? 2001 : -1;
                }
                if (in(475, 379, 153, 21, x, y)) {
                    // `FUN_00441020` shows CityBtn2 (Execute) for row 0 while
                    // the control is waiting for its cost (state 1).
                    return (city_queue_sel == 0 && city_prod_state == 1) ? 2002
                                                                        : -1;
                }
            } else {
                if (in(475, 358, 153, 21, x, y)) {
                    return (unit_sel ||
                            city_activity_has_action(city_activity))
                               ? 2001
                               : -1;
                }
                if (in(475, 379, 153, 21, x, y)) {
                    // `FUN_0043c43c` shows Queue when GetQueueLocked() is
                    // false; Migrate has no queue button.
                    return (!city_queue_locked() &&
                            (unit_sel || city_activity <= 2 ||
                             city_activity == 4))
                               ? 2002
                               : -1;
                }
            }
            if (in(475, 399, 153, 21, x, y)) return 2004;
            // QueueLB: 157x63 at (11,358), ItemHeight 11. Only a row that
            // exists in TProductionQueue selects (the original's
            // `count <= Selected` guard); clicking past it does nothing.
            if (in(11, 358, 157, 63, x, y)) {
                int row = (y - 358) / 11;
                if (row < 0) row = 0;
                if (row > 4) row = 4;
                if (row < static_cast<int>(city_queue.size())) {
                    return 2010 + row;
                }
                return -1;
            }
            return -1;
        }
        if (in(475, 358, 153, 21, x, y)) return 2001;
        if (in(475, 379, 77, 21, x, y)) return 2002;
        if (in(551, 379, 77, 21, x, y)) return 2003;
        if (in(475, 399, 153, 20, x, y)) return 2004;
        if (in(185, 380, 14, 13, x, y)) return 2005;
        if (in(11, 358, 157, 63, x, y)) {
            const int row = (y - 358) / 11;
            return 2010 + (row < 0 ? 0 : (row > 4 ? 4 : row));
        }
        return -1;
    }

    // PPrev/PNextClick (AoW.exe 0x43e420 / 0x43e500): walk the human player's
    // TPlayerStructureList for the previous/next TCity and call ShowCity on it.
    void city_step(int dir) {
        std::vector<std::pair<int, int>> cities;
        for (std::size_t l = 0; l < game_state_map.levels.size(); ++l) {
            const aow::MapLevel& lv = game_state_map.levels[l];
            if (static_cast<int>(l) != game_level) continue;
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    for (const aow::MapHs& o :
                         lv.tiles[static_cast<std::size_t>(y) * lv.width + x]
                             .objects) {
                        if (o.class_id != aow::kCityClassId) continue;
                        if (mv_human != 0xff && o.structure_owner != mv_human) {
                            continue;
                        }
                        cities.emplace_back(static_cast<int>(x),
                                            static_cast<int>(y));
                    }
                }
            }
        }
        if (cities.empty()) return;
        int cur = -1;
        for (std::size_t i = 0; i < cities.size(); ++i) {
            if (cities[i].first == game_city_hx &&
                cities[i].second == game_city_hy) {
                cur = static_cast<int>(i);
            }
        }
        const int n = static_cast<int>(cities.size());
        const int next = cur < 0 ? 0 : ((cur + dir) % n + n) % n;
        open_city_screen(cities[static_cast<std::size_t>(next)].first,
                         cities[static_cast<std::size_t>(next)].second);
        std::printf("city_step=%d (%d,%d)\n", dir, game_city_hx, game_city_hy);
        std::fflush(stdout);
    }

    // CityBtn1Click mode 0 (AoW.exe 0x0043d37c): switch TCityScreen to its
    // production picker. `FUN_0043bb14(param,1)` builds the 12 production cells
    // from TCity's TUnitProductionList, `FUN_0043c414` shows UpgradePnl /
    // ActivityPnl2, and `FUN_0043c43c` fills the page for the selected action.
    // B1CB..B6CB are the six activity cells (Tag 0..5): Produce Merchandise,
    // Upgrade, Fortify, Migrate, Loot and Raze.
    void city_change_production() {
        city_change = true;
        city_queue_edit = false;
        city_prod_sel = -1;
        city_activity = 0;
        city_unit_page_refresh();
        std::printf("city_change=1 activity=0\n");
        std::fflush(stdout);
    }

    static const char* city_activity_title(int a) {
        static const char* const kTitles[6] = {
            "Produce Merchandise", "Upgrade", "Fortify",
            "Migrate", "Loot", "Raze",
        };
        return (a >= 0 && a < 6) ? kTitles[a] : kTitles[0];
    }

    // CityBtn1's label is the action ("Produce Merchandise", "Upgrade",
    // "Fortify", "Loot", "Raze"); Migrate has no MainBtn1
    // (`FUN_0043c43c` hides it for action 4). CityBtn2 (Queue) is driven by
    // `GetQueueLocked` plus the Migrate page's hidden panel.
    static bool city_activity_has_action(int a) { return a != 3; }

    void city_activity_select(int a) {
        if (a < 0) a = 0;
        if (a > 5) a = 5;
        city_activity = a;
        city_queue_edit = false;
        city_prod_sel = -1;
        city_unit_page_refresh();
        std::printf("city_activity=%d title=[%s]\n", city_activity,
                    city_activity_title(city_activity));
        std::fflush(stdout);
    }

    // `CityBtn1Click` mode 1 case 6 (Raze) is the only activity that is not a
    // queued production: it calls the city's raze method immediately and hides
    // the dialog. Every other activity (Trade/Upgrade/Fortify/Migrate/Loot) is
    // queued through `city_queue_add` and applied by
    // `city_production_complete` when its timer runs out, exactly like the
    // original's `TCityProductionControl.NewTurn`.
    void city_activity_apply() {
        if (city_activity != 5) {
            // Defensive: MainBtn1 only reaches here for Raze; a stray call
            // keeps the old page state rather than changing the city.
            std::printf("city_apply=[%s]\n",
                        city_activity_title(city_activity));
            std::fflush(stdout);
            return;
        }
        std::printf("city_destroyed=Raze\n");
        city_change = false;
        close_city_screen();
        std::fflush(stdout);
    }

    // --- the queue editor (TCityScreen mode 2) ------------------------------
    // `FUN_0043c43c` uses the item's action kind, `QueueLBChange` shows the
    // Delete button for a selected row and - only for row 0 with
    // LoopQueueBox checked - the Execute button.
    void city_queue_rebuild_names() {
        city_queue_names.clear();
        for (const aow::CityQueueItem& q : city_queue) {
            city_queue_names.push_back(
                city_production_item_name(q.kind, q.index));
        }
    }

    // `AoWE.FmtTurnsLeftStr@5575a5c4`: the overview's ActiveActionTurns.
    // Only the value 1 takes the singular resource ("1 turn left"); every
    // other value - including 0 - goes through IntToStr + " turns left", so a
    // finished-but-not-yet-placed item really reads "0 turns left".
    static std::string city_turns_left_text(int turns) {
        if (turns == 1) return "1 turn left";
        return std::to_string(turns) + " turns left";
    }

    // `FUN_0043f574` does not cache the label: it reads
    // `TProductionSettings+0x10` (the same byte `NewTurn@5575dd00` ticks)
    // whenever it refreshes the overview. Derive the text from the live city
    // so no refresh path can leave the number behind. The original hides the
    // label for the stored signed byte -1 instead of formatting it.
    std::string city_turns_left_label() const {
        if (city_map_hs != nullptr && city_map_hs->production_state == 2) {
            if (city_map_hs->production_turns_left == -1) return std::string();
            return city_turns_left_text(city_map_hs->production_turns_left);
        }
        return city_production_turns;
    }

    // Keep the overview's ActiveAction / ActiveActionTurns / IncomeMemo in step
    // with the queue's first row and the live production state. The original's
    // `FUN_0043bb14`/`FUN_0043f574` show the queue's `GetProductionName` and
    // `FmtTurnsLeftStr(TProductionControl+0x24 + 0x10)` while an item runs; a
    // waiting item (state 1) shows the reason string CanExecuteProduction
    // fills (not implemented yet), so its label stays hidden.
    void city_queue_sync_production() {
        if (city_queue.empty()) {
            city_production = "Producing Merchandise";
            city_production_turns = "(default)";
            city_income_lines = "Base Income\r" +
                                std::to_string(city_crop_fields) +
                                " Crop Fields\rProducing Merchandise";
            return;
        }
        const aow::CityQueueItem& front = city_queue.front();
        city_production = city_production_item_name(front.kind, front.index);
        if (city_prod_state == 2) {
            city_production_turns = city_prod_turns_left == -1
                                        ? std::string()
                                        : city_turns_left_text(
                                              city_prod_turns_left);
        } else if (city_prod_state == 1) {
            city_production_turns.clear();
        } else {
            city_production_turns = "(default)";
        }
        city_income_lines =
            "Base Income\r" + std::to_string(city_crop_fields) +
            " Crop Fields\r" +
            city_production_item_progressive(front.kind, front.index);
    }

    // The roster entry that owns the open city (`TProductionControl.GetPlayer`
    // @557a9134 -> City+0x30 -> TPlayerList.GetPlayers).
    aow::Player* city_owner_player() {
        if (city_map_hs == nullptr) return nullptr;
        const std::size_t i = mv_gs.index_of(city_map_hs->structure_owner);
        return i < mv_gs.players.size() ? &mv_gs.players[i] : nullptr;
    }

    aow::Player* player_by_index(std::uint8_t idx) {
        const std::size_t i = mv_gs.index_of(idx);
        return i < mv_gs.players.size() ? &mv_gs.players[i] : nullptr;
    }

    // Writes the Scene's live production model back into the city's MapHs
    // (`TProductionControl.ReadWrite@5575dc18`: tag 0x14 queue, 0x15 unit
    // list, 0x16 settings, 0x17 state, 0x18 loop). The settings object's
    // +8/+0xc/+0x10 store the paid gold/mana and the turns left while active;
    // its rw tag 0x16 is always streamed, so `production_current_present`
    // stays set once the city had a production control.
    void city_store_to_map() {
        if (city_map_hs == nullptr) return;
        city_map_hs->production_units = city_prod_units;
        city_map_hs->production_queue = city_queue;
        city_map_hs->loop_queue = city_loop;
        city_map_hs->production_state = city_prod_state;
        city_map_hs->production_turns_left = city_prod_turns_left;
        city_map_hs->production_turns_total = city_prod_turns_total;
        city_map_hs->production_gold_paid = city_prod_gold_paid;
        city_map_hs->production_mana_paid = city_prod_mana_paid;
        city_sync_settings_mirror(*city_map_hs);
    }

    // `TProductionControl.ExecuteProduction@5575d118` +
    // `CanExecuteProduction@5575cf4c`: charge the item's gold/mana, then set
    // the state to 2 and the timer to the item's time. When the treasury is
    // short the item stays in state 1 ("Waiting") and is retried next turn -
    // exactly what `NewTurn@5575dd00` does with a state-1 control.
    bool city_production_try_start() {
        if (city_map_hs == nullptr || city_queue.empty()) return false;
        if (city_prod_state == 2) return true;
        const aow::CityQueueItem& q = city_queue.front();
        const int gold =
            city_production_gold(*city_map_hs, q.kind, q.index);
        if (gold < 0) {
            std::printf("city_production_reject=[%s] cost<0\n",
                        city_production_item_name(q.kind, q.index).c_str());
            std::fflush(stdout);
            return false;
        }
        aow::Player* p = city_owner_player();
        const int have = p != nullptr ? p->gold : 0;
        if (have < gold) {
            city_prod_state = 1;
            city_store_to_map();
            std::printf("city_production_wait=[%s] cost=%d gold=%d\n",
                        city_production_item_name(q.kind, q.index).c_str(),
                        gold, have);
            std::fflush(stdout);
            return false;
        }
        if (p != nullptr) p->gold -= gold;
        city_prod_state = 2;
        city_prod_turns_total =
            city_production_time(*city_map_hs, q.kind, q.index);
        city_prod_turns_left = city_prod_turns_total;
        city_prod_gold_paid = gold;
        city_prod_mana_paid = 0;
        city_store_to_map();
        city_queue_sync_production();
        std::printf("city_production_start=[%s] cost=%d time=%d gold=%d\n",
                    city_production_item_name(q.kind, q.index).c_str(), gold,
                    city_prod_turns_total, p != nullptr ? p->gold : 0);
        std::fflush(stdout);
        return true;
    }

    // `TProductionControl.ProduceUnitEx@5575d670` / `ResearchUnit@5575d870`
    // through `ExecuteAddProduction@5575d324`: append the item, then execute it
    // straight away when it is the only row. `from_queue` is the Queue
    // button's flag (0 for MainBtn1): `CanResearchUnit@5575d7e4` only rejects
    // a duplicate queued install when that flag is set.
    bool city_queue_add(int kind, int index, bool from_queue) {
        if (city_queue.size() >= 5) {
            std::printf("city_queue_add=full\n");
            std::fflush(stdout);
            return false;
        }
        if (city_queue_locked()) {
            std::printf("city_queue_add=locked\n");
            std::fflush(stdout);
            return false;
        }
        if (from_queue && kind == 2) {
            for (const aow::CityQueueItem& q : city_queue) {
                if (q.kind == 2 && q.index == index) {
                    std::printf("city_queue_add=already-queued\n");
                    std::fflush(stdout);
                    return false;
                }
            }
        }
        if ((kind == 11 && city_map_hs != nullptr &&
             city_map_hs->city_level >= city_size_hexes(*city_map_hs)) ||
            (kind == 12 && city_map_hs != nullptr &&
             city_map_hs->wall_type >= 2)) {
            std::printf("city_queue_add=action-unavailable\n");
            std::fflush(stdout);
            return false;
        }
        aow::CityQueueItem q;
        q.kind = kind;
        q.index = index;
        // `ProduceUnitEx@5575d670` stores its caller flag in
        // `TProductionItem+0x10`; `ResearchUnit@5575d870` and the city-action
        // methods leave that field at 0.
        q.extra = (kind == 1 && from_queue) ? 1 : 0;
        city_queue.push_back(q);
        city_queue_rebuild_names();
        city_queue_sync_production();
        city_queue_sel = -1;
        std::printf("city_queue_add=[%s] rows=%d\n",
                    city_queue_names.back().c_str(),
                    static_cast<int>(city_queue.size()));
        std::fflush(stdout);
        if (city_prod_state == 0 && city_queue.size() == 1) {
            city_production_try_start();
        }
        city_store_to_map();
        return true;
    }

    // --- producing a unit into the city's army -------------------------------
    // `TAbstractUnit.Place@557810fc`: find the TArmyHS on the hex (creating one
    // when the hex is empty), `TArmy.AddUnit` it, and fail when the army is
    // full (`TArmy.MaxSize@5578def4` = 8, reduced by a transport ability).
    bool city_place_unit_at(aow::MapLevel& lv, int hx, int hy,
                            std::uint8_t owner, int unit_id) {
        if (hx < 0 || hy < 0 || hx >= static_cast<int>(lv.width) ||
            hy >= static_cast<int>(lv.height)) {
            return false;
        }
        aow::MapTile& tile =
            lv.tiles[static_cast<std::size_t>(hy) * lv.width +
                     static_cast<std::size_t>(hx)];
        aow::MapUnit mu;
        mu.class_id = aow::kMapUnitClassId;
        mu.resource = unit_id;
        mu.id = -1;
        mu.move_points_left = -1;
        // `TUnit.SetUnitResource` copies the Unitres ability list into the new
        // unit. Without it `GetMoveTypes` is 0, and the all-0xFF move table of
        // that one unit makes the whole army immobile when folded.
        if (const aow::UnitEntry* e = city_unit_entry(unit_id)) {
            mu.ability_count = e->ability_count;
            mu.ability_mask = e->ability_mask;
        }
        for (aow::MapHs& o : tile.objects) {
            if (o.class_id != aow::kMapArmyHsClassId) continue;
            if (o.owner != owner) return false;  // another owner's stack
            if (o.units.size() >= 8) return false;  // TArmy.AddUnit fails
            o.units.push_back(mu);
            std::printf("city_unit_produced=unit=%d at=%d,%d army=%zu\n",
                        unit_id, hx, hy, o.units.size());
            std::fflush(stdout);
            return true;
        }
        aow::MapHs army;
        army.class_id = aow::kMapArmyHsClassId;
        army.owner = owner;
        army.army_direction = 3;
        army.units.push_back(mu);
        tile.objects.push_back(army);
        std::printf("city_unit_produced=unit=%d at=%d,%d army=new\n", unit_id,
                    hx, hy);
        std::fflush(stdout);
        return true;
    }

    // `TCityProductionControl.UnitProduced@557a8f10`: try the city's own
    // covered hexes (the TTerrainList entries of its city resource), then the
    // stored hex with `Place(..., force)`. The port derives the covered hexes
    // with the same HN spiral the engine uses (`hn_walk_hex`), so a multi-hex
    // city puts the unit on a footprint tile instead of always on its centre.
    bool city_produce_unit_at_city(aow::MapLevel& lv, const aow::MapHs& hs,
                                   int hx, int hy, int unit_id) {
        const std::uint8_t owner = hs.structure_owner;
        const aow::MapResource* r =
            aow::find_map_resource(game_res, hs.resource);
        if (r != nullptr) {
            for (std::size_t i = 0; i < r->terrain_list.size(); ++i) {
                if (static_cast<std::int8_t>(r->terrain_list[i] & 0xff) < 0) {
                    continue;
                }
                int nx = hx, ny = hy;
                aow::hn_walk_hex(hx, hy, static_cast<int>(i), nx, ny);
                if (city_place_unit_at(lv, nx, ny, owner, unit_id)) {
                    return true;
                }
            }
        }
        return city_place_unit_at(lv, hx, hy, owner, unit_id);
    }

    // Applies a finished queue item and logs it. Unit produce/install are the
    // two mechanics the wiki calls "Produce unit"/"Install unit":
    //   * install = UnitResearched@5575d774: set TUnitProduction+0xc, i.e. the
    //     card becomes producible (its button flips from Install to Produce);
    //   * produce = UnitProduced@557a8f10: place a new TUnit at the city.
    // The other production ids mirror TCityProductionControl.NewTurn@557a82a8.
    bool city_production_complete(aow::MapLevel& lv, aow::MapHs& hs, int hx,
                                  int hy, const aow::CityQueueItem& item) {
        switch (item.kind) {
        case 1: {
            if (item.index < 0 ||
                static_cast<std::size_t>(item.index) >=
                    hs.production_units.size()) {
                return false;
            }
            const int unit_id =
                hs.production_units[static_cast<std::size_t>(item.index)]
                    .unit_id;
            if (!city_produce_unit_at_city(lv, hs, hx, hy, unit_id)) {
                // No city hex could take it: NewTurn keeps the item at the
                // head of the queue and retries UnitProduced next turn.
                std::printf("city_unit_produced=unit=%d at=%d,%d failed\n",
                            unit_id, hx, hy);
                std::fflush(stdout);
                return false;
            }
            return true;
        }
        case 2: {
            if (item.index < 0 ||
                static_cast<std::size_t>(item.index) >=
                    hs.production_units.size()) {
                return false;
            }
            hs.production_units[static_cast<std::size_t>(item.index)].flag =
                true;
            std::printf("city_unit_installed=index=%d unit=%d\n", item.index,
                        hs.production_units[static_cast<std::size_t>(item.index)]
                            .unit_id);
            std::fflush(stdout);
            return true;
        }
        case 10:
            std::printf("city_production_done=[Produce Merchandise]\n");
            break;
        case 11: {
            const int size = city_size_hexes(hs);
            hs.city_level = static_cast<std::uint8_t>(
                std::min<int>(size, static_cast<int>(hs.city_level) + 1));
            std::printf("city_upgrade_level=%d\n",
                        static_cast<int>(hs.city_level));
            break;
        }
        case 12: {
            // TCity.SetWallType: a map without tag 0xf leaves +0x4c at 0, so
            // the port's -1 sentinel means "no wall", not "below wooden".
            const int wall = hs.wall_type < 0 ? 0 : hs.wall_type;
            hs.wall_type = static_cast<std::int8_t>(std::min(2, wall + 1));
            std::printf("city_wall=%d\n", static_cast<int>(hs.wall_type));
            break;
        }
        case 13:
            hs.race = static_cast<std::int8_t>(item.index < 0 ? 0 : item.index);
            std::printf("city_migrated=race=%d\n",
                        static_cast<int>(hs.race));
            break;
        case 14:
            std::printf("city_production_done=[Loot]\n");
            break;
        default:
            break;
        }
        std::fflush(stdout);
        return true;
    }

    // The MapHs-only twin of `city_production_try_start` (the turn loop runs
    // for every city, not just the one whose screen is open).
    bool city_try_start_for(aow::MapHs& hs) {
        if (hs.production_queue.empty()) {
            hs.production_state = 0;
            return false;
        }
        const aow::CityQueueItem& q = hs.production_queue.front();
        const int gold = city_production_gold_for(hs.production_units, hs,
                                                   q.kind, q.index);
        if (gold < 0) {
            std::printf("city_production_reject=[%s] cost<0\n",
                        city_production_item_name(q.kind, q.index).c_str());
            std::fflush(stdout);
            return false;
        }
        aow::Player* p = player_by_index(hs.structure_owner);
        const int have = p != nullptr ? p->gold : 0;
        if (have < gold) {
            hs.production_state = 1;
            std::printf("city_production_wait=[%s] cost=%d gold=%d\n",
                        city_production_item_name(q.kind, q.index).c_str(),
                        gold, have);
            std::fflush(stdout);
            return false;
        }
        if (p != nullptr) p->gold -= gold;
        hs.production_state = 2;
        hs.production_turns_total = city_production_time_for(
            hs.production_units, hs, q.kind, q.index);
        hs.production_turns_left = hs.production_turns_total;
        hs.production_gold_paid = gold;
        hs.production_mana_paid = 0;
        city_sync_settings_mirror(hs);
        std::printf("city_production_start=[%s] cost=%d time=%d gold=%d\n",
                    city_production_item_name(q.kind, q.index).c_str(), gold,
                    hs.production_turns_total, p != nullptr ? p->gold : 0);
        std::fflush(stdout);
        return true;
    }

    // `TProductionControl.ProductionDone@5575dc7c`: free (or, with the Repeat
    // box checked, move to the end) the finished queue row, then execute the
    // next one. `LoopableProduction` excludes installs and every city action
    // except Produce Merchandise.
    void city_production_done(aow::MapLevel& lv, aow::MapHs& hs, int hx,
                              int hy) {
        if (hs.production_queue.empty()) {
            hs.production_state = 0;
            city_sync_settings_mirror(hs);
            return;
        }
        const aow::CityQueueItem item = hs.production_queue.front();
        if (!city_production_complete(lv, hs, hx, hy, item)) {
            // `NewTurn` leaves a state-2 item whose UnitProduced failed at the
            // head of the queue (turn counter already 0); the next turn calls
            // UnitProduced again instead of dropping the finished unit.
            hs.production_state = 2;
            hs.production_turns_left = 0;
            city_sync_settings_mirror(hs);
            std::printf("city_production_retry=[%s]\n",
                        city_production_item_name(item.kind, item.index)
                            .c_str());
            std::fflush(stdout);
            return;
        }
        if (hs.loop_queue && city_production_loopable(item.kind)) {
            hs.production_queue.erase(hs.production_queue.begin());
            hs.production_queue.push_back(item);
        } else {
            hs.production_queue.erase(hs.production_queue.begin());
        }
        // ProductionDone does not clear the settings object: +8/+0xc/+0x1c
        // keep the finished item's values until the next ExecuteProduction
        // refills them (the shipped autosave's idle city still carries 250/3).
        // Only the timer is 0 and the state goes idle.
        hs.production_state = 0;
        hs.production_turns_left = 0;
        if (!hs.production_queue.empty()) city_try_start_for(hs);
        city_sync_settings_mirror(hs);
        std::printf("city_production_advance=[%s] rows=%zu state=%u\n",
                    city_production_item_name(item.kind, item.index).c_str(),
                    hs.production_queue.size(),
                    static_cast<unsigned>(hs.production_state));
        std::fflush(stdout);
    }

    // `FUN_0043f574` refreshes the overview from the live TProductionControl
    // (queue + the settings object's +0x10). The port keeps an editable copy
    // for the screen, so mirror the map fields back into it after a turn ticked
    // the city that is currently open. (The GUI city screen is modal, but the
    // head-less `--game-turns` smoke test keeps it open across days, which is
    // exactly how the stale-number bug showed up.)
    void city_refresh_open_from_map(aow::MapHs& hs) {
        if (city_map_hs != &hs) return;
        city_prod_state = hs.production_state;
        city_prod_turns_left = hs.production_turns_left;
        city_prod_turns_total = hs.production_turns_total;
        city_prod_gold_paid = hs.production_gold_paid;
        city_prod_mana_paid = hs.production_mana_paid;
        // An Install completion flips TUnitProduction+0xc, so the cards must
        // follow too (the art stays keyed by index; the row order is stable).
        city_prod_units = hs.production_units;
        city_prod_names.clear();
        for (const aow::CityProductionUnit& u : city_prod_units) {
            city_prod_names.push_back(city_unit_name(u.unit_id));
        }
        city_queue = hs.production_queue;
        city_queue_rebuild_names();
        if (city_prod_state != 0 && city_prod_turns_total <= 0 &&
            !city_queue.empty()) {
            city_prod_turns_total = city_production_time(
                hs, city_queue.front().kind, city_queue.front().index);
        }
        if (city_queue_sel >= static_cast<int>(city_queue.size())) {
            city_queue_sel = static_cast<int>(city_queue.size()) - 1;
        }
        if (city_queue.empty()) {
            city_queue_sel = -1;
            city_queue_edit = false;
        }
        city_queue_sync_production();
        city_unit_page_refresh();
        std::printf("city_production_tick=state=%u turns_left=%d total=%d "
                    "label=[%s]\n",
                    static_cast<unsigned>(city_prod_state),
                    city_prod_turns_left, city_prod_turns_total,
                    city_turns_left_label().c_str());
        std::fflush(stdout);
    }

    // `TCity.NewTurn@557ab814` -> `TProductionControl.NewTurn@5575dd00` for
    // every city the player owns: retry a waiting item (state 1), tick an
    // executing one (state 2) and finish it when the timer reaches 0.
    void mv_city_production_new_turn(std::uint8_t player) {
        // Collect first: finishing an item can push a new TArmyHS into a
        // tile's object vector, which would invalidate a live range-for.
        struct CityRef {
            aow::MapLevel* lv;
            aow::MapHs* hs;
            int hx, hy;
        };
        std::vector<CityRef> cities;
        for (aow::MapLevel& lv : game_state_map.levels) {
            for (std::uint32_t y = 0; y < lv.height; ++y) {
                for (std::uint32_t x = 0; x < lv.width; ++x) {
                    aow::MapTile& tile =
                        lv.tiles[static_cast<std::size_t>(y) * lv.width + x];
                    for (aow::MapHs& hs : tile.objects) {
                        if (hs.class_id != aow::kCityClassId) continue;
                        if (hs.structure_owner != player) continue;
                        if (hs.production_state == 0 &&
                            hs.production_queue.empty()) {
                            continue;
                        }
                        cities.push_back(CityRef{&lv, &hs,
                                                 static_cast<int>(x),
                                                 static_cast<int>(y)});
                    }
                }
            }
        }
        for (const CityRef& c : cities) {
            aow::MapHs& hs = *c.hs;
            if (hs.production_queue.empty()) {
                hs.production_state = 0;
                if (city_map_hs == &hs) city_refresh_open_from_map(hs);
                continue;
            }
            if (hs.production_state == 1) {
                city_try_start_for(hs);
            } else if (hs.production_state == 2) {
                // `NewTurn@5575dd00`: `if (+0x10 > 0) dec(+0x10); if (+0x10 ==
                // 0) finish`. 0 means "retry UnitProduced this turn", so a
                // failed placement stays at 0 instead of restarting the timer.
                if (hs.production_turns_left > 0) --hs.production_turns_left;
                // Keep the serialized settings mirror equal to the live byte
                // the overview reads (TProductionSettings +0x10).
                city_sync_settings_mirror(hs);
                if (hs.production_turns_left == 0) {
                    city_production_done(*c.lv, hs, c.hx, c.hy);
                }
            } else if (hs.production_state == 0 && !hs.production_queue.empty()) {
                city_try_start_for(hs);
            }
            if (city_map_hs == &hs) city_refresh_open_from_map(hs);
        }
    }

    void city_queue_select(int row) {
        if (row < 0 || static_cast<std::size_t>(row) >= city_queue.size()) {
            return;
        }
        city_queue_sel = row;
        city_queue_edit = true;
        const aow::CityQueueItem& q =
            city_queue[static_cast<std::size_t>(row)];
        const int action = city_queue_action(q.kind);
        if (action >= 1 && action <= 6) {
            city_activity = action - 1;
        } else {
            city_activity = 0;
        }
        if (action == 7 || action == 8) {
            city_prod_sel = q.index;
            if (city_prod_sel < 0 ||
                static_cast<std::size_t>(city_prod_sel) >=
                    city_prod_units.size()) {
                city_prod_sel = city_prod_units.empty() ? -1 : 0;
            }
        } else {
            city_prod_sel = -1;
        }
        city_unit_page_refresh();
        std::printf("city_queue_sel=%d action=%d name=[%s] mode=2\n", row,
                    action,
                    row < static_cast<int>(city_queue_names.size())
                        ? city_queue_names[static_cast<std::size_t>(row)].c_str()
                        : "?");
        std::fflush(stdout);
    }

    // CityBtn1Click mode 2: delete the selected TProductionQueue item
    // (`TProductionControl.DeleteProduction@5575d9e0`). Deleting the running
    // row cancels it through `ExecuteCancelProduction@5575cd64`, which refunds
    // `CancelProductionReturnGold/Mana` - the paid cost times the turns left
    // over the total, rounded (`TProductionControl+0x24`).
    void city_queue_delete() {
        if (city_queue_sel < 0 ||
            static_cast<std::size_t>(city_queue_sel) >= city_queue.size()) {
            return;
        }
        const int row = city_queue_sel;
        const aow::CityQueueItem item =
            city_queue[static_cast<std::size_t>(row)];
        int refund = 0;
        if (row == 0 && city_prod_state == 2) {
            const int total =
                city_prod_turns_total > 0 ? city_prod_turns_total : 1;
            // Delphi's Round is banker's rounding; nearbyint uses the default
            // FE_TONEAREST mode, which is the same.
            const double v = static_cast<double>(city_prod_turns_left) *
                             city_prod_gold_paid / static_cast<double>(total);
            refund = static_cast<int>(std::nearbyint(v));
            if (refund < 0) refund = 0;
            aow::Player* p = city_owner_player();
            if (p != nullptr) p->gold += refund;
            city_prod_state = 0;
            city_prod_turns_left = 0;
            city_prod_turns_total = 0;
            city_prod_gold_paid = 0;
            city_prod_mana_paid = 0;
        } else if (row == 0) {
            city_prod_state = 0;
            city_prod_turns_left = 0;
            city_prod_turns_total = 0;
            city_prod_gold_paid = 0;
            city_prod_mana_paid = 0;
        }
        std::printf("city_queue_delete=%d name=[%s] refund=%d\n", row,
                    city_production_item_name(item.kind, item.index).c_str(),
                    refund);
        city_queue.erase(city_queue.begin() + row);
        city_queue_rebuild_names();
        city_queue_sel = city_queue.empty() ? -1 : row;
        if (city_queue_sel >= static_cast<int>(city_queue.size())) {
            city_queue_sel = static_cast<int>(city_queue.size()) - 1;
        }
        if (row == 0 && !city_queue.empty()) city_production_try_start();
        city_store_to_map();
        city_queue_sync_production();
        if (city_queue.empty()) {
            city_queue_edit = false;
            city_prod_sel = -1;
            city_activity = 0;
        } else if (city_queue_sel >= 0) {
            // Re-run QueueLBChange for the row that slid into the selection so
            // the page/helper captions follow the new item.
            city_queue_select(city_queue_sel);
        }
        city_unit_page_refresh();
        std::fflush(stdout);
    }

    // CityBtn2Click mode 2: `TProductionControl.RetryProductionExecution`
    // (@5575dad8) only acts on a *waiting* (state 1) queue whose first row is
    // selected; it runs CanExecuteProduction and, on success, starts the item.
    void city_queue_execute() {
        if (city_queue_sel != 0 || city_queue.empty()) return;
        if (city_prod_state != 1) return;
        const std::string name =
            city_production_item_name(city_queue.front().kind,
                                      city_queue.front().index);
        const bool ok = city_production_try_start();
        std::printf("city_queue_execute=[%s] ok=%d\n", name.c_str(),
                    ok ? 1 : 0);
        std::fflush(stdout);
        if (ok) {
            city_change = false;
            city_queue_edit = false;
            city_queue_sync_production();
        }
    }

    void city_action(int hit) {
        if (city_change) {
            if (hit >= 2100 && hit <= 2105) {   // B1CB..B6CB
                city_activity_select(hit - 2100);
                return;
            }
            if (hit >= 2200 && hit <= 2211) {   // P1CB..P12CB
                const int card = hit - 2200;
                city_prod_sel = card < static_cast<int>(city_prod_units.size())
                                    ? card
                                    : -1;
                city_queue_edit = false;
                // FUN_0043c43c(7/8) for the checked card: fill UnitPnl.
                city_unit_page_refresh();
                if (city_prod_sel >= 0 &&
                    static_cast<std::size_t>(card) < city_prod_names.size()) {
                    std::printf("city_production_sel=%d name=[%s]\n", card,
                                city_prod_names[static_cast<std::size_t>(card)]
                                    .c_str());
                } else {
                    std::printf("city_production_sel=%d\n", card);
                }
                std::fflush(stdout);
                return;
            }
        }
        switch (hit) {
        case 2001:
            if (city_change) {
                if (city_queue_edit) {
                    city_queue_delete();
                } else if (city_prod_sel >= 0 &&
                           static_cast<std::size_t>(city_prod_sel) <
                               city_prod_units.size()) {
                    // `FUN_0043c43c(7/8)` set MainBtn1 to Produce/Install;
                    // CityBtn1Click then calls the city's Produce/Research,
                    // which queues the matching TProductionItem (kind 1/2) and
                    // returns to the overview.
                    const bool produce =
                        city_prod_units[static_cast<std::size_t>(city_prod_sel)]
                            .flag;
                    if (city_queue_add(produce ? 1 : 2, city_prod_sel, false)) {
                        city_change = false;
                        city_queue_edit = false;
                        city_prod_sel = -1;
                        city_unit_page_refresh();
                    }
                } else if (city_activity_has_action(city_activity)) {
                    if (city_activity == 5) {
                        // Raze is not a queued production: CityBtn1Click case 6
                        // calls the city's raze method immediately.
                        city_activity_apply();
                    } else {
                        const int kind =
                            city_action_to_production_id(city_activity + 1);
                        const int index = (kind == 10) ? -1 : 0;
                        if (city_queue_add(kind, index, false)) {
                            city_change = false;
                            city_queue_edit = false;
                            city_prod_sel = -1;
                            city_unit_page_refresh();
                        }
                    }
                }
            } else {
                city_change_production();
            }
            break;
        case 2002:
            if (city_change) {
                if (city_queue_edit) {
                    city_queue_execute();
                } else if (!city_queue_locked()) {
                    if (city_prod_sel >= 0 &&
                        static_cast<std::size_t>(city_prod_sel) <
                            city_prod_units.size()) {
                        // kind 1 = Produce, 2 = Install (TProductionItem+8).
                        (void)city_queue_add(
                            city_prod_units[
                                static_cast<std::size_t>(city_prod_sel)].flag
                                ? 1
                                : 2,
                            city_prod_sel, true);
                    } else if (city_activity <= 2 || city_activity == 4) {
                        const int kind =
                            city_action_to_production_id(city_activity + 1);
                        const int index = (kind == 10) ? 1 : 0;
                        (void)city_queue_add(kind, index, true);
                    }
                }
            } else {
                city_step(-1);
            }
            break;
        case 2003:
            if (!city_change) city_step(1);
            break;
        case 2004:
            if (city_change) {
                // Done / Escape: back to the overview page (mode 0). It does
                // not close the city screen - that is CityBtn3 in mode 0.
                city_change = false;
                city_queue_edit = false;
                city_prod_sel = -1;
                city_unit_page_refresh();
                std::printf("city_change=0\n");
            } else {
                close_city_screen();
            }
            break;
        case 2005:
            if (!city_change) {
                city_repeat = !city_repeat;
                // LoopQueueBoxChange -> TProductionControl.SetLoopQueueTo
                // (+0x20); QueueLBChange's Execute button reads the same bit.
                city_loop = city_repeat;
                city_store_to_map();
                std::printf("city_repeat=%d\n", city_repeat ? 1 : 0);
            }
            break;
        default:
            if (hit >= 2010 && hit <= 2014) {
                if (city_change) {
                    city_queue_select(hit - 2010);
                } else {
                    // `QueueLBChange` restores -1 in mode 0; the overview's
                    // list is a read-only view of the current production.
                    std::printf("city_queue_sel=%d (mode 0, ignored)\n",
                                hit - 2010);
                }
            }
            break;
        }
        std::fflush(stdout);
    }

    int game_hit(int x, int y) const {
        // TCityScreen.Dlg is modal (Modal = True, Priority 55): while it is up
        // it owns every click, including the bar strip it covers.
        if (game_show_city) return city_hit_test(x, y);
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
        // TAOWWindow.Draw tiles ILTexture (GenericT 2, 190x192) from the window
        // origin across the whole surface, then lays the frame over it. The
        // first pass blitted one copy at (9,22), which put the parchment's
        // mottling in the wrong phase everywhere (10-20/255 off in the left
        // panel); the capture matches the tile from (0,0).
        // The window texture starts at the frame's inner origin (9,22) - the
        // best registration of GenericT 2 against the capture is exactly that
        // phase (mean 3.5/255 over the flat paper), not (0,0).
        if (iwin_body.ok) {
            cv.blit_clip(iwin_body, x0 + 9, y0 + 22, x0 + 6, y0 + 20, 188, 190);
        }
        // NamePnl is a TAOWPanel with the window's texture: the capture shows a
        // dark parchment inset framed by a 1 px black + 1 px light bevel (not
        // the FrGoldD 9-slice the first pass assumed). GenericT 14 is the tile
        // whose mottling matches it.
        {
            const AowBox& p = S.name;
            // The panel is drawn as a raised parchment bevel around its
            // component rect: a black edge one pixel outside on the left, a
            // dark + light + dark three-line bevel, and a dark + light pair on
            // the bottom (measured row by row from StructurePReference.png).
            for (int yy = 20; yy <= 39; ++yy) {
                for (int xx = 7; xx <= 137; ++xx) {
                    std::uint8_t r = 99, g = 96, b = 72;
                    if (iwin_plaque_tex.ok) {
                        const int tx = ((xx + p.x - x0 - 12) % 100 + 100) % 100;
                        const int ty = ((yy + p.y - y0 - 57) % 100 + 100) % 100;
                        const std::uint8_t* t = &iwin_plaque_tex.rgba[
                            (static_cast<std::size_t>(ty) * iwin_plaque_tex.w +
                             static_cast<std::size_t>(tx)) * 4];
                        if (t[3]) { r = t[0]; g = t[1]; b = t[2]; }
                    }
                    cv.fill(x0 + xx, y0 + yy, 1, 1, r, g, b, 255);
                }
            }
            cv.fill(x0 + 3, y0 + 18, 1, 23, 8, 8, 8, 255);       // outer black
            cv.fill(x0 + 4, y0 + 18, 1, 23, 66, 82, 66, 255);    // dark
            cv.fill(x0 + 5, y0 + 18, 1, 22, 156, 173, 140, 255); // light
            cv.fill(x0 + 6, y0 + 18, 1, 22, 57, 57, 41, 255);    // dark
            cv.fill(x0 + 7, y0 + 17, 131, 1, 33, 33, 16, 255);   // top dark
            cv.fill(x0 + 7, y0 + 18, 131, 1, 189, 198, 165, 255);
            cv.fill(x0 + 7, y0 + 19, 131, 1, 41, 41, 33, 255);
            cv.fill(x0 + 7, y0 + 40, 131, 1, 66, 57, 49, 255);   // bottom dark
            cv.fill(x0 + 7, y0 + 41, 131, 1, 148, 156, 132, 255);
            cv.fill(x0 + 138, y0 + 18, 1, 23, 57, 57, 41, 255);
            cv.fill(x0 + 139, y0 + 18, 1, 23, 148, 156, 132, 255);
        }
        // The CityPanel's own top bevel, one row below the plaque (the capture
        // shows the dark row at y=44 and its light inner row at y=45).
        cv.fill(x0 + 4, y0 + 44, 114, 1, 41, 49, 33, 255);
        cv.fill(x0 + 4, y0 + 45, 114, 1, 140, 156, 132, 255);
        blit9_impl(cv, iwin_frame, S.win.x, S.win.y, S.win.w, S.win.h, 6, 6);
        // CeltFrame (TIWINDOW.dfm: ILFrame = CeltDeco, frHollowBox, index 22 ->
        // the FrameMain2 9-slice at entries 30..37), inset 9 px from the window.
        // CeltFrame's own DFM box is (5,19) 190x192. The first pass inset it to
        // (9,21) 188x190, which put its corners at the capture's (12,23) instead
        // of (8,21) and left the name plaque's bevel uncovered.
        if (iwin_celt_main[1].ok) {
            blit9_impl(cv, iwin_celt_main, x0 + 5, y0 + 19, 190, 192, 6, 6);
        }
        // The top-left corner is just the window frame; the standalone 27x27
        // CeltDeco knot the first pass blitted at (0,0) is not in the capture
        // (the Structure SIcon itself is the knot-like WinTIcon art).
        // CeltFrame's left edge is ILIndexLine art (CeltDeco 0, 7x27) tiled
        // down the frame - the capture matches it byte-for-byte at (5,19).
        if (iwin_celt_edge.ok) {
            cv.blit_clip(iwin_celt_edge, x0 + 5, y0 + 19, x0 + 5, y0 + 19, 7,
                         27);
        }
        if (iwin_title_icon.ok) {
            cv.blit_clip(iwin_title_icon, S.title_icon.x, S.title_icon.y, x0, y0,
                         200, 20);
        }
        // Title: Age10, clBlack, at the Label's DFM box (25,4).
        text_at(cv, glyphs10b, S.title.x, S.title.y, "Structure", 0, 0, 0, false,
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
            // GoldIcn (LBIcon 31) and UgradeI (LBIcon 1) are drawn 1:1; the
            // read-outs use FontModule.Age10s (clWhite), centred vertical by the
            // label's own TopOffset. The second value is the city *level*, not
            // mana (TIWINDOW.dfm CityLevelLbl).
            if (iwin_gold_icon.ok) {
                cv.blit_clip(iwin_gold_icon, S.gold_icon.x, S.gold_icon.y,
                             S.gold_icon.x, S.gold_icon.y, S.gold_icon.w,
                             S.gold_icon.h);
            }
            if (iwin_upgrade_icon.ok) {
                cv.blit_clip(iwin_upgrade_icon, S.upgrade_icon.x,
                             S.upgrade_icon.y, S.upgrade_icon.x,
                             S.upgrade_icon.y, S.upgrade_icon.w,
                             S.upgrade_icon.h);
            }
            text_at(cv, glyphs10, S.gold_val.x, S.gold_val.y,
                    std::to_string(game_struct_gold), 255, 255, 255, false,
                    false);
            text_at(cv, glyphs10, S.level_val.x, S.level_val.y,
                    std::to_string(game_struct_mana), 255, 255, 255, false,
                    false);
            // CityRaceLbl: FontModule.Age8s (CharacterDistance 0), clBlack,
            // centred on CityPanel.
            text_at(cv, glyphs8, S.race.x, S.race.y,
                    "Race: " + game_struct_race, 0, 0, 0, true, false, -1, true);
            // RaceRelationIcon: relation face (RelFr 11..18) drawn 1:1. The
            // decoded 51x36 face's visible circle starts at its visual box, so
            // the destination is the DFM box (the capture puts the 37x36 face
            // at (45,133) = window (42,131)).
            const int rel = std::max(
                0, std::min(static_cast<int>(kStructureRelationCount) - 1,
                            game_struct_relation));
            const Sprite& face = iwin_relation[rel];
            if (face.ok) {
                cv.blit_clip(face, S.portrait.x, S.portrait.y, S.portrait.x,
                             S.portrait.y, S.portrait.w, S.portrait.h);
            }
            // CivilStatusLbl: FontModule.Age8s, clBlack, centred, bottom 1.
            text_at(cv, glyphs8, S.status.x, S.status.y,
                    "Status: " + game_struct_status, 0, 0, 0, true, false, -1,
                    true);
        }
        // StructNameLbl: FontModule.Age10s (clWhite) centred on NamePnl; the
        // glyphs carry their own black shadow.
        text_at(cv, glyphs10, S.name_text.x, S.name_text.y, game_struct_name,
                255, 255, 255, true, false);
        // CityBorderPnl: the race's BCS_<race>.BMP city view clipped to its
        // 80x194 box, then CeltDeco's arch (id 8) and the id-9 border tile.
        if (game_struct_city) {
            if (game_struct_art.ok) {
                cv.blit_clip(game_struct_art, S.city_art.x, S.city_art.y,
                             S.city_border.x, S.city_border.y,
                             S.city_border.w, S.city_border.h);
            }
            if (iwin_celt_arch.ok) {
                cv.blit_clip(iwin_celt_arch, S.arch.x, S.arch.y,
                             S.city_border.x, S.city_border.y,
                             S.city_border.w, S.city_border.h);
            }
            // CeltDeco 8 tops the arch at (119,19); the id-9 frame tile follows
            // at (119,77) and (119,135). The capture's last ~17 px below that
            // are the window's own celtic frame, so the tile stops at 193.
            for (int yy = S.arch.y + 58; yy <= S.arch.y + 116; yy += 58) {
                if (iwin_celt_side.ok) {
                    cv.blit_clip(iwin_celt_side, S.arch.x, yy,
                                 S.city_border.x, S.city_border.y,
                                 S.city_border.w, S.city_border.h);
                }
            }
        }
        // ActiveCityActionMemo: Age8, clBlack, taCenter. The memo's first item
        // is the "Producing ..." line; its glyph pixels match the capture at
        // (12,68) exactly (the first pass was one row high).
        text_at(cv, glyphs8b, S.prod.x, S.prod.y, game_struct_production, 0, 0,
                0, false, false, 0);
        {
            const int st = game_struct_pressed == 1004 ? 2
                           : game_struct_hover == 1004 ? 1 : 0;
            blit3_box(cv, iwin_enter[st], S.enter);
            text_at(cv, glyphs10b, S.enter.x + S.enter.w / 2, S.enter.y + 8,
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

    // ---- the City screen (TCityScreen.Dlg, 640x432) -----------------------
    // The backdrop is the captured original skin (assets/cityscreen.bmp); the
    // live controls below are drawn over it at the DFM's own coordinates. The
    // captured BMP still contains the original screen's 48 px bar band, but
    // every city blit is clipped to the 432 px dialog so the live world-map bar
    // (drawn by render_game before the modal) is never replaced or covered.
    // True when the live city matches the capture the backdrop was extracted
    // from (Venice, Humans, Cheerful, 38 gold, level 2, Stone Wall). The
    // backdrop already carries that city's text, so the renderer leaves it
    // untouched - the reference comparison then stays pixel-exact. Any other
    // city gets live labels, a live race portrait and a live queue.
    bool city_is_reference() const {
        return city_bg_exact && city_name == "Venice" &&
               city_population == "Humans" &&
               city_status == "Cheerful" && city_gold == 38 &&
               city_base_income == 20 && city_crop_fields == 5 &&
               city_crop_income == 10 && city_prod_income == 6 &&
               city_upgrade_level == 2 && city_wall == "Stone Wall" &&
               city_production == "Producing Merchandise" &&
               game_struct_race_index == 0 &&
               (city_change || (city_queue_sel < 0 && !city_repeat));
    }

    // The four right-hand buttons (Change / < / > / Exit). While a button is
    // hovered or held down the engine swaps its 3-piece GenericB skin; the
    // reference backdrop already carries the resting art.
    void draw_city_buttons(Canvas& cv) const {
        struct CBtn { AowBox box; const char* text; int id; };
        const CBtn btns[4] = {
            {AowBox{475, 358, 153, 21}, "Change", 2001},
            {AowBox{475, 379, 77, 21}, "<", 2002},
            {AowBox{551, 379, 77, 21}, ">", 2003},
            {AowBox{475, 399, 153, 20}, "Exit", 2004},
        };
        for (const CBtn& b : btns) {
            if (city_pressed != b.id && city_hover != b.id) continue;
            const int st = city_pressed == b.id ? 2 : 1;
            if (party_btn[st][1].ok) blit3_box(cv, party_btn[st], b.box);
            const int ty = b.box.y + (b.box.h - party_text_h()) / 2 + 1;
            text_at(cv, glyphs10b, b.box.x + b.box.w / 2, ty, b.text, 24, 20, 16,
                    true, false);
        }
    }
    // Copy an opaque sub-rectangle of `s` to the canvas (used to restore the
    // RightPnl background behind a button the page capture still shows).
    void blit_sprite_rect(Canvas& cv, const Sprite& s, int sx, int sy, int w,
                          int h, int dx, int dy) const {
        if (!s.ok || w <= 0 || h <= 0) return;
        if (sx < 0 || sy < 0 ||
            sx + w > static_cast<int>(s.w) ||
            sy + h > static_cast<int>(s.h)) {
            return;
        }
        std::vector<std::uint8_t> patch(static_cast<std::size_t>(w) * h * 4);
        for (int y = 0; y < h; ++y) {
            const std::size_t src =
                (static_cast<std::size_t>(sy + y) * s.w + sx) * 4;
            const std::size_t dst = static_cast<std::size_t>(y) * w * 4;
            std::copy(s.rgba.begin() + static_cast<std::ptrdiff_t>(src),
                      s.rgba.begin() +
                          static_cast<std::ptrdiff_t>(src + w * 4),
                      patch.begin() + static_cast<std::ptrdiff_t>(dst));
        }
        cv.blit_rgba_rect(patch, static_cast<std::uint32_t>(w),
                          static_cast<std::uint32_t>(h), dx, dy, dx, dy, w,
                          h);
    }

    // Text width for an explicit CharacterDistance. The Unit window's Age8
    // captions pin the DFM's CharacterDistance 0 (re/dfm_all/TFONTMODULE.txt);
    // the shipped "Alignment: Neutral" is 79 px (bin/UnitsSelectionRef.png),
    // i.e. the glyph slot widths with no extra distance.
    int text_w_track(const std::vector<Glyph>& fs, const std::string& s,
                     int tracking) const {
        int t = 0;
        for (unsigned char c : s) {
            const Glyph& g = fs[c];
            t += (g.ok ? static_cast<int>(g.face.advance) : 5) + tracking;
        }
        return t;
    }

    // Tile `tex` 1:1 over (x,y,w,h) with its screen-space phase anchored at
    // (ox,oy). TAOWPanel.Draw repeats ILTexture from the frame's inner origin,
    // so callers pass panel + the frame corner size (2 for FrThinD, 3 for
    // FrGoldD) - the phase that reproduces the captured pages.
    void blit_tex_rect(Canvas& cv, const Sprite& tex, int x, int y, int w,
                       int h, int ox, int oy) const {
        if (!tex.ok || tex.w == 0 || tex.h == 0 || w <= 0 || h <= 0) return;
        const int tw = static_cast<int>(tex.w);
        const int th = static_cast<int>(tex.h);
        // ShowClipped draws the visual at (dest + base_x/base_y); that margin
        // is part of the pattern's phase (ScrollT's is 15,16, which is exactly
        // the offset the right-frame parchment showed against the reference).
        const int ax = ox + tex.base_x;
        const int ay = oy + tex.base_y;
        const int sx0 = x - (((x - ax) % tw + tw) % tw);
        const int sy0 = y - (((y - ay) % th + th) % th);
        for (int ty = sy0; ty < y + h; ty += th) {
            for (int tx = sx0; tx < x + w; tx += tw) {
                cv.blit_clip(tex, tx + tex.base_x, ty + tex.base_y, x, y, w, h);
            }
        }
    }

    // `FUN_0043c43c` cases 7 and 8: the unit detail page. Everything the
    // original fills into UnitPnl is drawn here (see the kUnit* block for the
    // DFM rects): the picture frame + stretched face, the two stat columns
    // with their icons and values, the alignment/level footer, the ability
    // list, and the Cost/Turns boxes. The twelve cards, the queue and the
    // three RightPnl buttons stay live around it.
    void draw_city_unit_page(Canvas& cv) const {
        const CityUnitPage& p = city_unit_page;
        if (!p.valid) return;
        // The page body is ActivityPnl's MainFloaterSet tile (origin 11,162).
        // Rows 227..228 are the separator/panel bevel the capture already
        // carries (the reference's light bevel), so the live body starts at
        // 229 - the same place the shipped page texture does.
        blit_tex_rect(cv, city_page_tex, kUnitPageX, 229, kUnitPageW, 128, 11,
                      162);
        // UPicturePnl (GenericT 18 + FrThinD) and its inset FrGoldD frame;
        // UFaceDraw stretches the face into the frame's inner rect.
        blit_tex_rect(cv, city_body_tex2, 33, 237, 90, 110, 35, 239);
        blit9_impl(cv, city_unit_frame, 33, 237, 90, 110, 2, 2);
        blit_tex_rect(cv, city_panel_tex3, 35, 238, 87, 107, 38, 241);
        blit9_impl(cv, city_unit_gold_frame, 35, 238, 87, 107, 3, 3);
        if (city_unit_face.ok) {
            cv.blit_stretch(city_unit_face, 37, 240, 83, 103);
        }
        // UStatPnl: the outer FrThinD panel, then UStatFr1 / UStatFr2 / UInfoFr
        // (each FrGoldD; UStatFr1/2 were still empty when the page opened).
        blit_tex_rect(cv, city_body_tex, 143, 256, 166, 91, 145, 258);
        blit9_impl(cv, city_unit_frame, 143, 256, 166, 91, 2, 2);
        blit_tex_rect(cv, city_body_tex, 145, 257, 81, 57, 148, 260);
        blit9_impl(cv, city_unit_gold_frame, 145, 257, 81, 57, 3, 3);
        blit_tex_rect(cv, city_body_tex, 226, 257, 82, 57, 229, 260);
        blit9_impl(cv, city_unit_gold_frame, 226, 257, 82, 57, 3, 3);
        blit_tex_rect(cv, city_body_tex, 145, 314, 163, 31, 148, 317);
        blit9_impl(cv, city_unit_gold_frame, 145, 314, 163, 31, 3, 3);
        // AbilPnl (FrThinD) and the UAbilitiesLB cell (FrGoldD). The list body
        // is the panel's TextOld1 showing through: the DFM's ILTextureNormal
        // (GenericT 21) is only used for item cells, and ListAbilities leaves
        // the list background untextured.
        // The list cell's FrGoldD frame starts its own texture phase (the
        // capture matches TextOld1 from (336,260), i.e. the list rect + the
        // 3 px frame), not the outer panel's (333,258).
        blit_tex_rect(cv, city_body_tex, 331, 256, 166, 91, 336, 260);
        blit9_impl(cv, city_unit_frame, 331, 256, 166, 91, 2, 2);
        blit9_impl(cv, city_unit_gold_frame, 333, 257, 163, 88, 3, 3);
        // UCostPnl (FrThinD) with CostFr / TurnsFr (FrGoldD).
        blit_tex_rect(cv, city_body_tex, 517, 237, 90, 110, 519, 239);
        blit9_impl(cv, city_unit_frame, 517, 237, 90, 110, 2, 2);
        blit_tex_rect(cv, city_body_tex, 519, 238, 87, 54, 522, 241);
        blit9_impl(cv, city_unit_gold_frame, 519, 238, 87, 54, 3, 3);
        blit_tex_rect(cv, city_body_tex, 519, 292, 87, 53, 522, 295);
        blit9_impl(cv, city_unit_gold_frame, 519, 292, 87, 53, 3, 3);
        // The six TAOWImage icons: Int\Icons.ILB 0..4 and UMovesIcon's
        // IconMove frame (FUN_004541b0). The sprite's base already carries the
        // class shift, so cv.blit lands the visual box on the control.
        const int clip[4] = {kUnitPageX, kUnitPageY, kUnitPageX + kUnitPageW,
                             kUnitPageY + kUnitPageH};
        // Int\Icons.ILB's five icons are [sword, shield, star, ankh, heart] =
        // [attack, defense, damage, resistance, hits]; the moves icon is a
        // separate IconMove frame (UMovesIcon) in the middle-left cell.
        static const int kIconCell[5] = {0, 1, 3, 4, 5};
        for (int i = 0; i < 5; ++i) {
            if (!city_unit_icon[i].ok) continue;
            const int cell = kIconCell[i];
            cv.blit(city_unit_icon[i], kUnitIconX[cell], kUnitIconY[cell],
                    kAlphaFull, clip[0], clip[1], clip[2], clip[3]);
        }
        {
            const int fi = p.move_icon;
            if (fi >= 0 && static_cast<std::size_t>(fi) < move_icons.size() &&
                move_icons[static_cast<std::size_t>(fi)].ok) {
                int dx = 0, dy = 0;
                for (const aow::IlbEntry& e : iconmove_il.entries()) {
                    if (e.index == static_cast<std::uint32_t>(fi)) {
                        dx = e.xshift;
                        dy = e.yshift;
                        break;
                    }
                }
                cv.blit(move_icons[static_cast<std::size_t>(fi)],
                        kUnitIconX[2] + dx, kUnitIconY[2] + dy, kAlphaFull,
                        clip[0], clip[1], clip[2], clip[3]);
            }
        }
        // Text: UnitLbl (AoW15WhiteGrey, centred on UnitPnl), UAttack..UHits
        // (Age10s, centred in UStatP1/UStatP2), UAlignment/ULevel (Age8),
        // UAbilitiesLB rows (Age8, ItemHeight 16) and Cost/Turns.
        text_at(cv, glyphs, 320, 233, p.title, 0, 0, 0, true, true);
        const std::string stat[6] = {
            std::to_string(p.attack), std::to_string(p.defense),
            std::to_string(p.moves),  std::to_string(p.damage),
            std::to_string(p.resistance), std::to_string(p.hits)};
        for (int i = 0; i < 6; ++i) {
            const int col = i < 3 ? 0 : 1;
            const int row = i < 3 ? i : i - 3;
            text_at(cv, glyphs10, kUnitStatLblX[col], kUnitStatLblY[row],
                    stat[i], kUnitTextR, kUnitTextG, kUnitTextB, true, false);
        }
        const std::string align = "Alignment: " + p.alignment;
        const std::string level = "Level " + std::to_string(p.level) +
                                  " / Upkeep: " + std::to_string(p.upkeep);
        text_at(cv, glyphs8b, 226 - text_w_track(glyphs8b, align, 0) / 2, 319,
                align, kUnitTextR, kUnitTextG, kUnitTextB, false, false, 0);
        text_at(cv, glyphs8b, 226 - text_w_track(glyphs8b, level, 0) / 2, 331,
                level, kUnitTextR, kUnitTextG, kUnitTextB, false, false, 0);
        // UAbilitiesLB is a TAOWListBox: ItemHeight 16 inside an 88 px body, so
        // at most five rows are visible; its T2SBar (VanishWhenFull) scrolls the
        // rest. The list also clips at its right edge (the bar takes the last
        // 17 px while shown).
        const int ability_total = static_cast<int>(p.abilities.size());
        const int ability_max_top = ability_total > kUnitAbilRows
                                        ? ability_total - kUnitAbilRows
                                        : 0;
        int ability_top = city_unit_abil_top;
        if (ability_top < 0) ability_top = 0;
        if (ability_top > ability_max_top) ability_top = ability_max_top;
        const bool show_sb = ability_max_top > 0;
        const int clip_r = show_sb ? kUnitAbilSbX - 1 : 493;
        for (int row_i = 0; row_i < kUnitAbilRows; ++row_i) {
            const int i = ability_top + row_i;
            if (i >= ability_total) break;
            std::string row = p.abilities[static_cast<std::size_t>(i)];
            while (!row.empty() &&
                   kUnitAbilX + text_w_track(glyphs8b, row, 0) > clip_r) {
                row.pop_back();
            }
            text_at(cv, glyphs8b, kUnitAbilX, kUnitAbilY + row_i * kUnitAbilDy,
                    row, kUnitTextR, kUnitTextG, kUnitTextB, false, false, 0);
        }
        if (show_sb) {
            const int sx = kUnitAbilSbX, sy = kUnitAbilSbY;
            const int sw = kUnitAbilSbW, sh = kUnitAbilSbH;
            if (city_unit_abil_sb[7].ok) {
                cv.blit_stretch(city_unit_abil_sb[7], sx, sy, sw, sh);
            } else {
                cv.fill(sx, sy, sw, sh, 92, 86, 62, 255);
            }
            if (city_unit_abil_sb[0].ok) {
                cv.blit(city_unit_abil_sb[0], sx, sy, kAlphaFull, 0, 0, disp_w,
                        disp_h);
            }
            if (city_unit_abil_sb[2].ok) {
                cv.blit(city_unit_abil_sb[2], sx,
                        sy + sh - static_cast<int>(city_unit_abil_sb[2].h),
                        kAlphaFull, 0, 0, disp_w, disp_h);
            }
            const int cap = city_unit_abil_sb[0].ok
                                ? static_cast<int>(city_unit_abil_sb[0].h)
                                : 16;
            const int span = sh - 2 * cap;
            int thumb_h = span * kUnitAbilRows / ability_total;
            if (thumb_h < 24) thumb_h = 24;
            if (thumb_h > span) thumb_h = span;
            const int travel = span - thumb_h;
            const int thumb_y =
                sy + cap + (ability_max_top > 0
                                ? travel * ability_top / ability_max_top
                                : 0);
            if (city_unit_abil_sb[4].ok) {
                cv.blit(city_unit_abil_sb[4], sx, thumb_y, kAlphaFull, 0, 0,
                        disp_w, disp_h);
            }
            if (city_unit_abil_sb[5].ok && thumb_h > 8) {
                cv.blit_stretch(city_unit_abil_sb[5], sx, thumb_y + 4, sw,
                                thumb_h - 8);
            }
            if (city_unit_abil_sb[6].ok) {
                cv.blit(city_unit_abil_sb[6], sx,
                        thumb_y + thumb_h -
                            static_cast<int>(city_unit_abil_sb[6].h),
                        kAlphaFull, 0, 0, disp_w, disp_h);
            }
        }
        // CostLbl/TurnsLbl are left-aligned at CostFr/TurnsFr + their DFM
        // WinLeft (33 / 31), not centred; UCost/UTurns are centred in the box.
        text_at(cv, glyphs8b, 552, 248, "Cost", kUnitTextR, kUnitTextG,
                kUnitTextB, false, false, 0);
        text_at(cv, glyphs, 562, 261, std::to_string(p.cost), 0, 0, 0, true,
                true);
        text_at(cv, glyphs8b, 550, 302, "Turns", kUnitTextR, kUnitTextG,
                kUnitTextB, false, false, 0);
        text_at(cv, glyphs, 562, 315, std::to_string(p.turns), 0, 0, 0, true,
                true);
    }

    // The 12 P1CB..P12CB cards. The base TAOWCheckBox draws RelFr 42/43 (52x64,
    // no own offset) first and `PreDrawProductions` overlays CityWin 4/5 (or 62
    // Is-Empty, both 48x60, no own offset) at the rect +2,+2 plus the unit's
    // standing pose. The rect the handler builds sits `kCityCardRaise` px above
    // the control: the reference capture puts RelFr 42's top edge on
    // `cell.y - 2` and the face on `(cell.x + 2, cell.y)`, and neither art is
    // clipped to the 52x52 cell - the checkbox frame runs down to
    // `cell.y + 62`. Only the helper-driven states are re-drawn here; the page
    // capture underneath already carries the resting row.
    void draw_city_production_cards(Canvas& cv) const {
        const int top = kCityCardY - kCityCardRaise;
        const int clip_b = top + 64;   // RelFr 42/43 is 52x64
        for (int i = 0; i < 12; ++i) {
            const int x = kCityCardX + i * kCityCardPitch;
            const bool selected = city_prod_sel == i;
            if (!selected && city_card_up.ok) {
                cv.blit(city_card_up, x, top, kAlphaFull, x, top, x + 52,
                        clip_b);
            }
            if (i < static_cast<int>(city_prod_units.size())) {
                const bool produce = city_prod_units[
                    static_cast<std::size_t>(i)].flag;
                const Sprite& face = city_card_frame[produce ? 0 : 1];
                if (face.ok) {
                    cv.blit(face, x + 2, top + 2, kAlphaFull, x, top,
                            x + 52, clip_b);
                }
                if (i < static_cast<int>(city_card_art.size()) &&
                    city_card_art[static_cast<std::size_t>(i)].s.ok) {
                    const CityCardArt& a =
                        city_card_art[static_cast<std::size_t>(i)];
                    cv.blit(a.s, x - kCityCardUnitDx + a.vx,
                            top - kCityCardUnitDy + a.vy, kAlphaFull, x, top,
                            x + 52, clip_b);
                }
                // The install branch's second pass: the frame-5 face again,
                // through GFXE's 45 % alpha-blend table, which washes the unit
                // art (and only the unit art: over the face itself src == dst)
                // out - the "not installed yet" look of the reference capture.
                if (!produce && face.ok) {
                    cv.blit(face, x + 2, top + 2, kCityCardInstallBlend, x,
                            top, x + 52, clip_b);
                }
            } else if (city_card_empty.ok) {
                cv.blit(city_card_empty, x + 2, top + 2, kAlphaFull, x, top,
                        x + 52, clip_b);
            }
            if (selected && city_card_sel.ok) {
                cv.blit(city_card_sel, x, top, kAlphaFull, x, top, x + 52,
                        clip_b);
            }
        }
    }

    // QueueLB's five rows. The captured `<empty>` text is removed by first
    // blitting `assets/cityqueue.bmp` (the same TAOWListBox art generated by
    // tools/city_prod_bg.py); the selected row uses Menus 8 + black text,
    // exactly like TAOWListBox.Draw.
    void draw_city_queue_live(Canvas& cv) const {
        if (!city_queue_clean.ok) return;
        cv.blit(city_queue_clean, 11, 358, kAlphaFull, 11, 358, 168, 421);
        for (int i = 0; i < 5; ++i) {
            std::string row =
                std::to_string(i + 1) + ". " +
                (i < static_cast<int>(city_queue_names.size())
                     ? city_queue_names[static_cast<std::size_t>(i)]
                     : "<empty>");
            // QueueLB is 157 px wide with a 3 px border; the engine clips a
            // long production name at the control's right edge.
            while (!row.empty() &&
                   17 + text_w(glyphs8b, row) > 165) {
                row.pop_back();
            }
            const int y = 360 + i * 11;
            const bool selected = city_queue_edit && city_queue_sel == i;
            if (selected) {
                if (city_queue_sel_tex.ok) {
                    for (int xx = 0; xx < 155;
                         xx += static_cast<int>(city_queue_sel_tex.w)) {
                        cv.blit_clip(city_queue_sel_tex, 13 + xx, y, 13, y,
                                     155, 11);
                    }
                }
                text_at(cv, glyphs8b, 17, y, row, 0, 0, 0, false, false);
            } else {
                text_at(cv, glyphs8b, 17, y, row, 118, 131, 103, false,
                        false);
            }
        }
    }

    // The production picker's three right-hand buttons. `FUN_0043c43c` sets
    // MainBtn1's caption to the selected action and hides it for Migrate;
    // `FUN_0043a?` hides Queue for Migrate/Loot/Raze. The resting art is part
    // of the captured page; only the hover/held skin is drawn live.
    void draw_city_change_buttons(Canvas& cv) const {
        struct CBtn {
            AowBox box;
            std::string text;
            int id;
            bool visible;
        };
        const bool unit_sel =
            city_prod_sel >= 0 &&
            static_cast<std::size_t>(city_prod_sel) < city_prod_units.size();
        std::vector<CBtn> btns;
        if (city_queue_edit) {
            // `QueueLBChange` sets MainBtn1 to Delete, shows MainBtn2 as
            // Execute for row 0 while the control is waiting (state 1), and
            // leaves Done.
            btns.push_back(CBtn{AowBox{475, 358, 153, 21}, "Delete", 2001,
                                city_queue_sel >= 0});
            btns.push_back(CBtn{AowBox{475, 379, 153, 21}, "Execute", 2002,
                                city_queue_sel == 0 && city_prod_state == 1});
            btns.push_back(CBtn{AowBox{475, 399, 153, 21}, "Done", 2004,
                                true});
        } else {
            if (city_activity_has_action(city_activity) || unit_sel) {
                std::string title = city_activity_title(city_activity);
                if (unit_sel) {
                    title = city_prod_units[
                                static_cast<std::size_t>(city_prod_sel)].flag
                                ? "Produce"
                                : "Install";
                }
                btns.push_back(
                    CBtn{AowBox{475, 358, 153, 21}, title, 2001, true});
            } else {
                btns.push_back(CBtn{AowBox{475, 358, 153, 21}, "", 2001,
                                    false});
            }
            btns.push_back(CBtn{
                AowBox{475, 379, 153, 21}, "Queue", 2002,
                !city_queue_locked() &&
                    (unit_sel || city_activity <= 2 || city_activity == 4)});
            btns.push_back(
                CBtn{AowBox{475, 399, 153, 21}, "Done", 2004, true});
        }
        for (const CBtn& b : btns) {
            const bool hover = city_hover == b.id;
            const bool pressed = city_pressed == b.id;
            // The page capture already carries MainBtn1/Queue/Done for the
            // resting mode-1 case. Redraw when the helper changes a caption,
            // in the queue editor, and for hover/held.
            const bool force = city_queue_edit ||
                               (unit_sel && b.id == 2001) ||
                               (!b.visible && b.id == 2002 &&
                                city_queue_locked());
            if (!b.visible) {
                // The Migrate capture (page 04) shows a clean RightPnl with
                // only Done, so its top/middle rectangles are the background
                // behind a hidden MainBtn1/CityBtn2.
                if ((force || city_queue_edit) && b.id != 2004 &&
                    city_prod_frame[3].ok) {
                    blit_sprite_rect(cv, city_prod_frame[3], b.box.x,
                                     b.box.y, b.box.w, b.box.h, b.box.x,
                                     b.box.y);
                }
                continue;
            }
            if (!force && !hover && !pressed) continue;
            const int st = pressed ? 2 : (hover ? 1 : 0);
            if (party_btn[st][1].ok) blit3_box(cv, party_btn[st], b.box);
            const int ty = b.box.y + (b.box.h - party_text_h()) / 2 + 1;
            text_at(cv, glyphs10b, b.box.x + b.box.w / 2, ty, b.text, 24, 20,
                    16, true, false);
        }
    }

    // TCityScreen mode 1 (the page `CityBtn1Click`'s Change opens). The six
    // pages are the B1CB..B6CB captures; the live controls are the activity
    // cells (2100+i), the 12 production cells (2200+i), the three RightPnl
    // buttons and the QueueLB rows. The overview's panels are not drawn.
    void render_city_change(Canvas& cv) const {
        const int a = (city_activity >= 0 && city_activity < 6) ? city_activity
                                                               : 0;
        const Sprite& f = city_prod_frame[a];
        // The pages were captured at 640x480 with the original bar band; clip
        // them to the 432 px dialog so the live bar below stays visible.
        const int dlg_h = kGameH - kTbBarH;
        if (f.ok) {
            cv.blit(f, 0, 0, kAlphaFull, 0, 0, cv.width(), dlg_h);
        } else if (city_bg.ok) {
            cv.blit(city_bg, 0, 0, kAlphaFull, 0, 0, cv.width(), dlg_h);
        }
        const bool reference = city_is_reference();
        if (!reference) {
            // Keep the live banner/name and the live map behind the page: the
            // overview's clean skin owns y<203, while the activity page keeps
            // the 12-cell production row and everything below it.
            if (city_bg_clean.ok) {
                cv.blit_clip(city_bg_clean, 0, 0, 0, 0, 640, 203);
            }
            text_at(cv, glyphs, 320, 13, city_name, 0, 0, 0, true, true);
        }
        // A checked P1CB..P12CB card (`city_prod_sel`, mode 1) - or a unit
        // production row selected in the queue editor - shows `UnitPnl`
        // (`FUN_0043c43c` cases 7/8) over the page area. It is drawn live from
        // the DFM art, so it does not depend on the activity capture.
        if (city_prod_sel >= 0 && city_unit_page.valid) {
            draw_city_unit_page(cv);
        }
        // Re-draw the state the page capture cannot carry: a city other than
        // the reference, a checked production card (mode 1) or the queue
        // editor (mode 2). The reference page with no selection stays an
        // exact blit of the capture, which is what QueueRef.png benchmarks.
        if (!reference || city_prod_sel >= 0 || city_queue_edit) {
            draw_city_production_cards(cv);
        }
        if (!reference || !city_queue.empty() || city_queue_edit) {
            draw_city_queue_live(cv);
        }
        draw_city_change_buttons(cv);
    }

    void render_city_screen(Canvas& cv) const {
        if (!game_show_city) return;
        if (city_change) {
            render_city_change(cv);
            return;
        }
        const bool exact = city_is_reference();
        const Sprite& bg =
            (!exact && city_bg_clean.ok) ? city_bg_clean : city_bg;
        if (bg.ok) {
            // Clip to the dialog: rows 432..479 of the captured backdrop hold
            // the original (frozen) bar and must not replace the live one.
            cv.blit(bg, 0, 0, kAlphaFull, 0, 0, cv.width(),
                    kGameH - kTbBarH);
        }
        // RacePortrait (IPPnl+5,4, 48x60): TCityScreen.RacePortraitDraw asks
        // TRaceResource.GetFace for the city's race and stretch-blits its
        // 103x128 portrait. The engine's stretch samples with floor(), which is
        // exactly Canvas::blit_stretch's rule. The captured face is the same
        // r_human.BMP, so drawing it live is pixel-exact for the reference and
        // swaps the portrait for every other race.
        {
            const int race =
                (game_struct_race_index >= 0 && game_struct_race_index < 12)
                    ? game_struct_race_index
                    : 0;
            if (city_race_face[race].ok) {
                cv.blit_stretch(city_race_face[race], 61, 229, 48, 60);
            }
        }
        // The backdrop already carries the reference city's text; any other
        // city gets live labels over it. The buttons are always live so hover
        // and press states show on the reference screen too.
        if (!city_is_reference()) render_city_live(cv);
        draw_city_buttons(cv);
    }

    // One memo block: TAOWMemo splits its Text on CR and draws each line at the
    // control's line height (Age8 headers 12 px, lower memos 13 px - measured
    // off the reference capture's ink rows).
    void draw_city_memo(Canvas& cv, int x, int y, int line_h,
                        const std::string& text, std::uint8_t r,
                        std::uint8_t g, std::uint8_t b,
                        const std::vector<Glyph>& fs) const {
        int line = 0;
        std::size_t start = 0;
        for (;;) {
            const std::size_t cr = text.find('\r', start);
            const std::string s =
                text.substr(start, cr == std::string::npos ? std::string::npos
                                                           : cr - start);
            if (!s.empty()) {
                text_at(cv, fs, x, y + line * line_h, s, r, g, b, false, false);
            }
            ++line;
            if (cr == std::string::npos) break;
            start = cr + 1;
        }
    }

    // The live TCityScreen content, drawn when the city is not the reference
    // capture: StructName, the three activity panels, the production row, the
    // queue and the button skins. Coordinates are the DFM's controls after the
    // form's centring (TCityScreen.Dlg is 640x432).
    void render_city_live(Canvas& cv) const {
        // StructName (AoW15WhiteGrey, clBlack), centred on the Dlg at y=16.
        text_at(cv, glyphs, 320, 13, city_name, 0, 0, 0, true, true);
        const int tr = 160, tg = 169, tb = 143;   // DFM TextColor 9415072
        // PopulationHdr: "Population:" / <race> / "Status:" / <status>.
        text_at(cv, glyphs8b, 112, 231, "Population:", tr, tg, tb, false, false);
        text_at(cv, glyphs8b, 112, 243, city_population, tr, tg, tb, false, false);
        text_at(cv, glyphs8b, 112, 255, "Status:", tr, tg, tb, false, false);
        text_at(cv, glyphs8b, 112, 267, city_status, tr, tg, tb, false, false);
        // PopulationMemo: TCity.ListCivilStatusInfo.
        draw_city_memo(cv, 58, 296, 13, city_race_lines, tr, tg, tb, glyphs8b);
        // IncomeHdr / IncomeMemo / IncomeGoldMemo.
        draw_city_memo(cv, 291, 231, 12, city_income_header, tr, tg, tb, glyphs8b);
        draw_city_memo(cv, 237, 296, 13, city_income_lines, tr, tg, tb, glyphs8b);
        {
            int line = 0;
            std::size_t start = 0;
            for (;;) {
                const std::size_t cr = city_income_gold.find('\r', start);
                const std::string s = city_income_gold.substr(
                    start, cr == std::string::npos ? std::string::npos
                                                   : cr - start);
                if (!s.empty()) {
                    text_at(cv, glyphs8b, 404 - text_w(glyphs8b, s),
                            296 + line * 13, s, tr, tg, tb, false, false);
                }
                ++line;
                if (cr == std::string::npos) break;
                start = cr + 1;
            }
        }
        // EnhancementsHdr / EnhancementsMemo.
        text_at(cv, glyphs8b, 470, 231, city_enh_header, tr, tg, tb, false, false);
        draw_city_memo(cv, 416, 296, 13, city_enh_lines, tr, tg, tb, glyphs8b);
        // ActiveAction / ActiveActionTurns, centred in CurrentInfo.
        const int qr = 118, qg = 131, qb = 103;   // DFM TextColor 6783862
        text_at(cv, glyphs8b, 322, 367, city_production, qr, qg, qb, true,
                false);
        text_at(cv, glyphs8b, 322, 382, city_turns_left_label(), qr, qg, qb,
                true, false);
        // QueueLB: five numbered slots, fed from the city's TProductionQueue
        // (a city with no queue shows <empty>). The clean overview backdrop
        // has this box erased, so the live rows are drawn for every
        // non-reference city - not only when the queue is non-empty.
        draw_city_queue_live(cv);
        // LoopQueueBox: the GenericI 12x11 checkbox (47 off / 49 lit).
        if (city_check[city_repeat ? 1 : 0].ok) {
            cv.blit(city_check[city_repeat ? 1 : 0], 186, 381, kAlphaFull, 0, 0,
                    cv.width(), cv.height());
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
        // TurnButton (TAOWTurnButton.Draw 0x59704e44): the 102-frame turn art -
        // the seated race's block at rest, its transition/glow frame while
        // Checked (the seat has ended), and the one new-day rotation while the
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
        char gold[32], mana[32];
        int gold_v = game_gold, ginc_v = game_gold_income;
        int mana_v = game_mana, minc_v = game_mana_income;
        game_live_stats(gold_v, ginc_v, mana_v, minc_v);
        // The green/red number is the NET per-turn value: the gold side
        // subtracts TPlayer.GetUpkeep() (army + magic), the mana side is the
        // already-net GetNetPowerToMana result.
        int gold_net = ginc_v;
        if (const aow::Player* seat = game_seat()) {
            gold_net = seat->income - seat->upkeep();
        }
        std::snprintf(gold, sizeof(gold), "%d", gold_v);
        std::snprintf(mana, sizeof(mana), "%d", mana_v);
        draw_readout(cv, L.gold, gold, gold_net);
        draw_readout(cv, L.mana, mana, minc_v);

        if (game_show_scan) draw_game_scan(cv, L);
        if (game_show_events) draw_game_events(cv, L);
        // The Party floater (TControlWin) rides the right edge over everything
        // but the pointer, exactly as the engine's Priority-10 window does.
        if (game_show_party) draw_game_party(cv, party_layout());

        // TCityScreen's Dlg is modal (Priority 55) and covers the map and every
        // in-game window, including the Party floater. Its backdrop blit stops
        // at y=432, so the live bottom bar drawn at step 2 stays visible and is
        // never replaced or overlapped by the city screen.
        render_city_screen(cv);
        // TUnitWindow (Priority 300, Modal = True) covers the party floater
        // and the city screen; it is the last window before the pointer.
        draw_game_unit_window(cv);

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

    // The engine's own 9-slice: TInterfaceIL.DrawILF keeps the corner images
    // at their native size and *tiles* the four edge images 1:1 through the
    // span (it never stretches them). The DialogFr top edge is 190x22 over the
    // 420 px window, so the reference shows two repeats - a stretch would
    // blur that pattern.
    void blit9_tiled(Canvas& cv, const Sprite* fr, const AowBox& r,
                     int edge_phase = 0) const {
        if (r.w <= 0 || r.h <= 0 || !fr[0].ok) return;
        auto sw = [](const Sprite& s) {
            return static_cast<int>(s.slot_w ? s.slot_w : s.w);
        };
        auto sh = [](const Sprite& s) {
            return static_cast<int>(s.slot_h ? s.slot_h : s.h);
        };
        // Draw one slot cell: the cropped visual is positioned inside the cell
        // by base+shift (TSprite16.ShowOpaque's dest = anchor + base + shift),
        // then clipped to the cell.
        auto cell = [&](const Sprite& s, int x, int y, int cw, int ch) {
            cv.blit_clip(s, x + s.base_x + s.shift_x,
                         y + s.base_y + s.shift_y, x, y, cw, ch);
        };
        const int cw = sw(fr[0]);
        const int th = sh(fr[0]);
        const int bh = sh(fr[5]);
        // DrawILF tiles the edge images, clipping only the last cell at the
        // far cap; the middle span is [corner, w-corner). `edge_phase` shifts
        // the horizontal tiling's texture start (the shipped DialogFr edge is
        // four pixels ahead of the corner origin); the corners are painted
        // last so they cover the overlap.
        const int ew = sw(fr[1]);
        for (int tx = r.x + cw - edge_phase; tx < r.x + r.w - cw; tx += ew) {
            const int w = std::min(ew, r.x + r.w - cw - tx);
            cell(fr[1], tx, r.y, w, th);
        }
        const int bw = sw(fr[6]);
        for (int tx = r.x + cw - edge_phase; tx < r.x + r.w - cw; tx += bw) {
            const int w = std::min(bw, r.x + r.w - cw - tx);
            cell(fr[6], tx, r.y + r.h - bh, w, bh);
        }
        const int eh = sh(fr[3]);
        for (int ty = r.y + th; ty < r.y + r.h - bh; ty += eh) {
            const int h = std::min(eh, r.y + r.h - bh - ty);
            cell(fr[3], r.x, ty, cw, h);
        }
        const int rh = sh(fr[4]);
        for (int ty = r.y + th; ty < r.y + r.h - bh; ty += rh) {
            const int h = std::min(rh, r.y + r.h - bh - ty);
            cell(fr[4], r.x + r.w - cw, ty, cw, h);
        }
        cell(fr[0], r.x, r.y, cw, th);
        cell(fr[2], r.x + r.w - cw, r.y, cw, th);
        cell(fr[5], r.x, r.y + r.h - bh, cw, bh);
        cell(fr[7], r.x + r.w - cw, r.y + r.h - bh, cw, bh);
    }

    // 3-piece button whose middle image is a repeating skin: the caps keep
    // their width and the middle art is tiled 1:1 across the span (the list
    // headers' 97 px NGHeadLight tile runs twice across a 168 px header).
    void blit3_tile(Canvas& cv, const Sprite* b, const AowBox& r) const {
        if (!b[1].ok) return;
        const int lw = static_cast<int>(b[0].w);
        const int rw = static_cast<int>(b[2].w);
        cv.blit_clip(b[0], r.x, r.y, r.x, r.y, lw, r.h);
        const int x0 = r.x + lw;
        const int x1 = r.x + r.w - rw;
        for (int tx = x0; tx < x1; tx += static_cast<int>(b[1].w)) {
            cv.blit_clip(b[1], tx, r.y, x0, r.y, x1 - x0, r.h);
        }
        cv.blit_clip(b[2], x1, r.y, x1, r.y, rw, r.h);
    }

    // The engine's 3-piece button: the caps keep their native width and the
    // 170 px middle is blitted 1:1 and clipped inside the 119 px button. The
    // replica used to shrink the middle to fit, compressing the celtic braid.
    void blit3_clip(Canvas& cv, const Sprite* b, const AowBox& r) const {
        if (!b[1].ok) return;
        const int lw = static_cast<int>(b[0].w);
        const int rw = static_cast<int>(b[2].w);
        cv.blit_clip(b[0], r.x, r.y, r.x, r.y, lw, r.h);
        if (r.w - lw - rw > 0) {
            cv.blit_clip(b[1], r.x + lw, r.y, r.x + lw, r.y, r.w - lw - rw,
                         r.h);
        }
        cv.blit_clip(b[2], r.x + r.w - rw, r.y, r.x + r.w - rw, r.y, rw, r.h);
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
    // --structure-panel (or --rgb555): write the PNG through the engine's
    // 16-bit RGB555 back-surface quantisation, as every reference capture was
    // taken from the original game's 16-bit display.
    bool rgb555 = false;
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
    // The Settings panel's own controls, so a head-less render can pin them:
    // --scen-playmode 1/2, --scen-turntime 0..20, --scen-turns 0..40,
    // --scen-checks <4 digits, e.g. 1010> and --scen-mode-dd (open the list).
    int scen_playmode = 0;
    int scen_turntime = -1;
    int scen_turns = -1;
    std::string scen_checks;
    bool scen_mode_dd = false;
    // --scen-click X Y (repeatable): head-less press + release on the Scenario
    // screen, through the same scen_hit / scen_slider_down / scen_action paths
    // the window's mouse handlers use.
    std::vector<std::pair<int, int>> scen_clicks;
    // --scen-drag X Y X2: press on a Duration slider's thumb at (X,Y), then drag
    // it to X2 (TAOWHScrollBar.CheckMouseMove's Position mapping).
    std::vector<std::array<int, 3>> scen_drags;
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
    // --game-capture X Y: head-less, run the structure ArmyPlaced ownership pass
    // on that map hex (the same helper the move code calls on arrival). Lets a
    // smoke test prove a walked-in capture without scripting a whole path.
    std::vector<std::pair<int, int>> game_captures;
    // --game-gold X Y: head-less, deliver the field's 0x1102 (ArmyPlaced) to the
    // Gems.TGems handler on that map hex for the seated player - the same
    // `game_gems_army_update` an arriving army runs. Lets a smoke test prove a
    // gold-pile pickup without scripting a whole walk.
    std::vector<std::pair<int, int>> game_golds;
    // --game-turns N: head-less, end the human's turn N times and run the turn
    // loop until each new day has handed the seat back (used by the city
    // production smoke test).
    int game_turns = 0;
    // --game-after-click X Y: clicks applied *after* --game-turns (the normal
    // --game-click list runs before it), so a smoke test can select/move an
    // army that a city produced during those turns.
    std::vector<GameClick> game_after_clicks;
    // --game-key K: head-less, press one in-game map key ('L' = CenterOnLeader,
    // 'C' = centre on the selection) after the clicks.
    int game_key = 0;
    // --game-prekey K: the same key press, but *before* --game-click. A test
    // that needs the map's focus on the leader (L) before clicking a party
    // portrait uses this; the normal order is clicks then key.
    int game_prekey = 0;
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
    // first structure when the map has no city); the name/race/level/income come
    // from the map itself (TCity rw tags 0xc/0xd/0x10 + the city race), and the
    // overrides only exist because the reference capture uses a scenario that is
    // not part of the install.
    bool structure_panel = false;
    // --city-screen: head-less capture hook for TCityScreen. It opens the first
    // city (or --structure-tile) through the same TIWindow Enter path the game
    // uses, then renders the 640x432 Dlg over the world map.
    bool city_screen = false;
    // --city-change / --city-activity N: open the production picker (mode 1)
    // instead of the overview, on activity page N (0..5: Produce Merchandise,
    // Upgrade, Fortify, Migrate, Loot, Raze).
    bool city_change = false;
    int city_activity = -1;
    // --city-level / --city-wall: the capture's TCity.CityLevelLbl and
    // EnhancementsMemo values (the scenario used for the reference is not part
    // of the install). -1 / empty = take them from the map.
    int city_level = -1;
    std::string city_wall;
    std::string structure_name, structure_race, structure_status;
    int structure_gold = -1, structure_mana = -1;
    int structure_relation = kStructureRelationDefault;
    int structure_race_index = -1;  // force the BCS_<race> city view
    // --structure-tile X Y: open the structure selectable at that tile (used to
    // prove a large building answers on every tile its sprite occupies, not only
    // on its home hex). -1 = unused.
    int structure_tile_x = -1, structure_tile_y = -1;
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
    // --- the Settings panel's controls ---------------------------------------
    if (hit == -5200) {                    // a click-away closes PlayModeCB
        s.scen_mode_dd = false;
        s.scen_mode_dd_hover = -1;
        return;
    }
    if (hit <= -5100 && hit > -5100 - kSetModeRows) {
        // A row of PlayModeCB's list: PlayModeCBChange (0x004129e8) resolves
        // the row's TObject through Items.GetObject and hands it to
        // hSetPlayMode, which stores it as TSetupSettings+0x24.
        const int row = -hit - 5100;
        s.scen_set_play_mode(Scene::set_mode_value(row));
        std::printf("scenario: turns -> %s\n",
                    Scene::set_mode_name(s.scen_play_mode));
        s.scen_mode_dd = false;
        s.scen_mode_dd_hover = -1;
        std::fflush(stdout);
        return;
    }
    if (hit == -5000) {                    // the closed PlayModeCB field
        s.scen_mode_dd = true;
        s.scen_mode_dd_hover = s.scen_play_mode == 1 ? 0 : 1;
        return;
    }
    if (hit == -5010 || hit == -5011) {
        // The sliders act on the *press*, not the release: TAOWHScrollBar
        // .CheckMouseDown steps / arms the drag (see WM_LBUTTONDOWN).
        return;
    }
    if (hit <= -5020 && hit > -5024) {
        // TAOWCheckBox.CheckMouseUp fires OnChange when the release is still
        // over the box; the handler stores Checked in TSetupSettings and plays
        // Sfx\Int\Btn5.wav (on) / Btn6.wav (off) - FUN_00455dfc 1 / 2.
        const int idx = -hit - 5020;
        s.scen_check[idx] = !s.scen_check[idx];
        play_game_sfx(s.scen_check[idx] ? "Sfx\\Int\\Btn5.wav"
                                        : "Sfx\\Int\\Btn6.wav");
        static const char* const kSetChkName[4] = {
            "leaders on map", "customize leaders", "allied victory",
            "exploration"};
        std::printf("scenario: %s -> %s\n", kSetChkName[idx],
                    s.scen_check[idx] ? "on" : "off");
        std::fflush(stdout);
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
        s.scen_mode_dd = false;  // the modal dialog closes the combo's list
        std::printf("scenario: pick map\n");
        break;
    case 1:  // ExitGameClick 0x00411f50 -> back to the game-type screen
        std::printf("scenario: exit\n");
        s.scen_maps_open = false;
        s.scen_mode_dd = false;
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
        // TSetupControl.SetupGame (AoWEPACK 0x557e0f88) hands the settings
        // record to the game: the turn mode, the per-turn time limit in minutes
        // and the turn limit (TurnsSl.Position * 10), plus the four option
        // flags. The port keeps them for the game state and logs them here.
        std::printf("scenario: start -> game (map=%s %s towns=%d players=%d "
                    "turns=%s turnTime=%s mode=%s leaders=%d customize=%d "
                    "allied=%d exploration=%d)\n",
                    s.game_map_name.c_str(), s.scen_map_size.c_str(),
                    s.scen_map_towns, s.scen_map_players,
                    Scene::set_turns_text(s.scen_turns).c_str(),
                    Scene::set_turn_time_text(s.scen_turn_time).c_str(),
                    Scene::set_mode_name(s.scen_play_mode),
                    static_cast<int>(s.scen_check[0]),
                    static_cast<int>(s.scen_check[1]),
                    static_cast<int>(s.scen_check[2]),
                    static_cast<int>(s.scen_check[3]));
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
    if (hit >= 2000) {  // TCityScreen's own controls
        s.city_action(hit);
        return;
    }
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
        case 1004:  // MainBtnClick (Enter): hand a city to TCityScreen
            if (s.game_struct_city) {
                s.open_city_screen(s.game_struct_hx, s.game_struct_hy);
                s.game_say("enter city");
            } else {
                s.close_structure();
                s.game_say("enter structure");
                std::printf("game: structure enter\n");
            }
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
        // The selected object belongs to the level being left.
        if (s.game_show_structure) s.close_structure();
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
            } else if (s.scen_drag_slider >= 0) {
                // Dragging a Duration slider's thumb (TAOWHScrollBar
                // .CheckMouseMove): the pointer position becomes Position.
                s.scen_slider_move(s.mouse_x);
            } else {
                s.scen_hover = s.scen_hit(s.mouse_x, s.mouse_y);
                // Track the hovered row of an open race PlayerCB drop-down.
                s.scen_dd_hover =
                    (s.scen_dd_gi >= 0 && s.scen_hover <= -4100 &&
                     s.scen_hover > -4100 - kScenPTypeCount)
                        ? (-s.scen_hover - 4100)
                        : -1;
                // ...and of PlayModeCB's, when it is dropped down.
                s.scen_mode_dd_hover =
                    (s.scen_mode_dd && s.scen_hover <= -5100 &&
                     s.scen_hover > -5100 - kSetModeRows)
                        ? (-s.scen_hover - 5100)
                        : -1;
                // The four checkboxes light up under the pointer (NGCBox 49).
                s.scen_chk_hover = s.scen_chk_at(s.mouse_x, s.mouse_y);
            }
        } else if (s.screen == 3) {
            if (s.game_show_city) {
                s.city_hover = s.game_hit(s.mouse_x, s.mouse_y);
                s.game_hover = -1;
                return 0;
            }
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
        } else if (s.screen == 3 && s.game_show_city && s.city_change &&
                   s.city_prod_sel >= 0) {
            // The production unit page's UAbilitiesLB (T2SBar): one row a notch,
            // clamped to its five visible rows.
            const int total = static_cast<int>(s.city_unit_page.abilities.size());
            const int max_off =
                total > kUnitAbilRows ? total - kUnitAbilRows : 0;
            s.city_unit_abil_top += GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1;
            if (s.city_unit_abil_top < 0) s.city_unit_abil_top = 0;
            if (s.city_unit_abil_top > max_off) s.city_unit_abil_top = max_off;
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
            // The Duration sliders act on the press (TAOWHScrollBar
            // .CheckMouseDown): a track click steps one, a thumb press arms
            // the drag that WM_MOUSEMOVE then tracks.
            if (sc == -5010 || sc == -5011) {
                s.scen_slider_down(sc == -5010 ? 0 : 1, s.mouse_x);
            }
            // A held checkbox keeps its armed face until the release.
            if (sc <= -5020 && sc > -5024) s.scen_chk_press = -sc - 5020;
        } else if (s.screen == 3) {
            if (s.game_show_unit) {
                // TUnitWindow is modal (Priority 300, Masked): arm its control
                // and swallow every other click.
                s.unitwin_pressed =
                    s.unit_window_hit_test(s.mouse_x, s.mouse_y);
                return 0;
            }
            if (s.game_show_city) {
                // TCityScreen is modal: arm its own control and swallow the rest.
                s.city_pressed = s.city_hit_test(s.mouse_x, s.mouse_y);
                return 0;
            }
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
            if (s.scen_drag_slider >= 0) {  // a slider drag ends here too
                s.scen_drag_slider = -1;
                s.scen_hover = -1;
                s.scen_pressed = -1;
                s.scen_chk_press = -1;
                return 0;
            }
            // The border of a TAOWScrollBar does not re-fire OnChange when the
            // thumb is released: only the buttons / checkboxes / combos act on
            // the release (scen_action), as before.
            const int hit = s.scen_hit(s.mouse_x, s.mouse_y);
            if (hit != -1 && hit == s.scen_hover) scen_action(hit);
            s.scen_hover = -1;
            s.scen_pressed = -1;
            s.scen_chk_press = -1;
            return 0;
        }
        if (s.screen == 3) {
            if (s.game_show_unit) {
                const int hit = s.unit_window_hit_test(s.mouse_x, s.mouse_y);
                if (hit >= 0 && hit == s.unitwin_pressed) {
                    s.unit_window_action(hit);
                }
                s.unitwin_pressed = -1;
                s.unitwin_hover = -1;
                return 0;
            }
            if (s.game_show_city) {
                const int hit = s.city_hit_test(s.mouse_x, s.mouse_y);
                if (hit >= 0 && hit == s.city_pressed) {
                    game_action(hit);
                }
                s.city_pressed = -1;
                s.city_hover = -1;
                return 0;
            }
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
        if (s.screen == 3 && !s.game_show_unit && !s.game_show_city &&
            s.game_in_map(s.mouse_x, s.mouse_y)) {
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
        // TCityScreen.DlgKeyDown (AoW.exe 0x43e290): Escape hides `Dlg` in
        // every mode, so it closes the modal city screen even from the
        // production picker. (The picker's Done button is CityBtn3Click, which
        // returns to the overview instead.) The arrow keys are `MapView`'s
        // camera nudge; the port's city view is static.
        if (g.scene.screen == 3 && g.scene.game_show_city) {
            if (wp == VK_ESCAPE) g.scene.close_city_screen();
            return 0;
        }
        if (g.scene.screen == 3 && wp == VK_RETURN &&
            g.scene.game_show_structure && g.scene.game_struct_city) {
            g.scene.open_city_screen(g.scene.game_struct_hx,
                                     g.scene.game_struct_hy);
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
        "  --scen-playmode N     PlayModeCB: 1 = Classic, 2 = Simultaneous turns\n"
        "  --scen-turntime N     TurnTimeSl position: 0 = no timer, else minutes\n"
        "                        (0..20)\n"
        "  --scen-turns N        TurnsSl position: 0 = no limit, else N x 10 turns\n"
        "                        (0..40)\n"
        "  --scen-checks 4x01    the Misc checkboxes: Leaders on map, Customize\n"
        "                        leaders, Allied victory, Exploration\n"
        "  --scen-mode-dd        draw PlayModeCB's drop-down list open\n"
        "  --scen-click X Y      head-less: press+release at (X,Y) on the Scenario\n"
        "                        screen (the same paths the mouse handlers use;\n"
        "                        repeatable)\n"
        "  --scen-drag X Y X2    head-less: press a Duration slider's thumb at\n"
        "                        (X,Y) and drag it to X2\n"
        "  --gtype-frame N       Game Type slide counter (0..286)\n"
        "  --game-click X Y      head-less: one map-window update, then a click at\n"
        "                        (X,Y) on the world map or its scanner (repeatable)\n"
        "  --game-right-click X Y  head-less: one map-window update, then a right-\n"
        "                        click at (X,Y) on the world map (clears selection)\n"
        "  --game-after-click X Y  head-less: the same click, but applied after\n"
        "                        --game-turns (repeatable)\n"
        "  --game-key K          head-less: press one map key (L = centre on the\n"
        "                        leader, C = centre on the selection)\n"
        "  --game-prekey K       the same key, but before --game-click\n"
        "  --game-gold X Y       head-less: run the Gems.TGems ArmyPlaced pickup on\n"
        "                        hex X,Y for the seated player\n"
        "  --game-bench N        head-less: time the hover refresh N times\n"
        "  --game-anim N         head-less: advance a walk animation N frames\n"
        "  --game-turns N        head-less: end the human's turn N times and run\n"
        "                        the loop (city production between days)\n"
        "  --game-clock N        head-less: force the map clock (building anim)\n"
        "  --turn-frame N        head-less: freeze the TurnButton on frame N (0..101)\n"
        "  --next-held           head-less: draw the NextButton held down\n"
        "  --structure-panel     head-less: open the Structure floater on the first\n"
        "                        city/structure after loading --start-map\n"
        "  --city-screen         head-less: open the first city's TCityScreen (the\n"
        "                        modal 640x432 unit-production panel)\n"
        "  --city-change         head-less: open its production picker (mode 1)\n"
        "  --city-activity N     with --city-change: page 0..5 (Produce Merchandise,\n"
        "                        Upgrade, Fortify, Migrate, Loot, Raze)\n"
        "  --city-level N        override the city's upgrade level (Enhancements)\n"
        "  --city-wall S         override the wall label (\"Stone Wall\" etc.)\n"
        "  --structure-tile X Y  head-less with --structure-panel: open the structure\n"
        "                        selectable at tile (X,Y) instead of the first one\n"
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
    // TUnitWindow is modal (DFM `Modal = True`, Priority 300), so it takes the
    // click before the city screen, the party and the map; a click outside its
    // rectangle is swallowed exactly like a masked modal window's.
    if (s.game_show_unit) {
        const int uh = s.unit_window_hit_test(cx, cy);
        if (uh >= 0) s.unit_window_action(uh);
        std::printf("game_click=unit %d,%d id=%d\n", cx, cy, uh);
        std::fflush(stdout);
        return;
    }
    if (s.game_show_city) {
        if (!right) {
            const int ch = s.city_hit_test(cx, cy);
            if (ch >= 0) s.city_action(ch);
            std::printf("game_click=city %d,%d id=%d\n", cx, cy, ch);
        } else {
            std::printf("game_rclick=city %d,%d\n", cx, cy);
        }
        std::fflush(stdout);
        return;
    }
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
            // The click only sets Checked (the glowing still). The window's 30
            // fps ticks then play the AI seats' turns and the single new-day
            // rotation, so the scripted driver runs until the seat comes back.
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

// Runs the structure ownership pass on one map hex without a click path: the
// exact `game_structure_army_update` the move code calls when a party arrives
// (`ArmyPlaced -> UpdatePlayer`). Prints `game_capture_hook=<x>,<y> ok=<0|1>`.
void apply_game_capture(int x, int y) {
    Scene& s = g.scene;
    s.mouse_x = -1;
    s.mouse_y = -1;
    while (s.mv_anim) s.game_update();
    const bool ok = s.game_structure_army_update(x, y, true);
    std::printf("game_capture_hook=%d,%d ok=%d\n", x, y, ok ? 1 : 0);
    std::fflush(stdout);
}

// Runs the gold-pile pickup on one map hex without a click path: the exact
// `game_gems_army_update` the move code calls when a party arrives (the field's
// 0x1102 message -> `Gems.TGems.ArmyEntered`). Prints
// `game_gold_hook=<x>,<y> ok=<0|1>`.
void apply_game_gold(int x, int y) {
    Scene& s = g.scene;
    s.mouse_x = -1;
    s.mouse_y = -1;
    while (s.mv_anim) s.game_update();
    const bool ok = s.game_gems_army_update(x, y);
    std::printf("game_gold_hook=%d,%d ok=%d\n", x, y, ok ? 1 : 0);
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
        if (g.structure_panel || g.city_screen) {
            bool opened = false;
            auto find_and_open = [&](bool city_only) {
                for (std::size_t l = 0; l < g.scene.game_state_map.levels.size() &&
                                       !opened; ++l) {
                    const aow::MapLevel& lv = g.scene.game_state_map.levels[l];
                    for (std::uint32_t y = 0; y < lv.height && !opened; ++y) {
                        for (std::uint32_t x = 0; x < lv.width; ++x) {
                            // Only a structure *stored* on this tile counts, so
                            // the hook keeps selecting the first city by its
                            // home hex - a large building would otherwise be
                            // found early through one of its footprint tiles.
                            int home_x = 0, home_y = 0;
                            const aow::MapHs* hs = g.scene.structure_on_hex(
                                static_cast<int>(x), static_cast<int>(y),
                                &home_x, &home_y);
                            if (hs == nullptr || home_x != static_cast<int>(x) ||
                                home_y != static_cast<int>(y)) {
                                continue;
                            }
                            if (city_only && hs->class_id != aow::kCityClassId) {
                                continue;
                            }
                            g.scene.game_level = static_cast<int>(l);
                            const bool ok =
                                g.city_screen
                                    ? g.scene.open_city_screen(
                                          static_cast<int>(x),
                                          static_cast<int>(y))
                                    : g.scene.open_structure(
                                          static_cast<int>(x),
                                          static_cast<int>(y));
                            if (ok) {
                                opened = true;
                                break;
                            }
                        }
                    }
                }
            };
            if (g.structure_tile_x >= 0 && g.structure_tile_y >= 0) {
                opened = g.city_screen
                             ? g.scene.open_city_screen(g.structure_tile_x,
                                                        g.structure_tile_y)
                             : g.scene.open_structure(g.structure_tile_x,
                                                      g.structure_tile_y);
                if (opened) {
                    std::printf("structure_tile=%d,%d\n", g.structure_tile_x,
                                g.structure_tile_y);
                }
            } else {
                find_and_open(true);
                if (!opened) find_and_open(false);
            }
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
                g.scene.game_struct_relation = g.structure_relation;
                if (g.structure_race_index >= 0) {
                    g.scene.game_struct_race_index = g.structure_race_index;
                    g.scene.load_structure_art(g.structure_race_index);
                }
                if (g.city_screen) {
                    if (!g.structure_name.empty()) g.scene.city_name = g.structure_name;
                    if (!g.structure_status.empty()) g.scene.city_status = g.structure_status;
                    if (g.structure_race_index >= 0) {
                        g.scene.city_population = aow::race_label_plural(
                            static_cast<std::uint8_t>(g.structure_race_index));
                    }
                    if (g.structure_gold >= 0) {
                        g.scene.city_gold = g.structure_gold;
                        g.scene.city_income_header =
                            "Income\r" + std::to_string(g.scene.city_gold) + " gold";
                    }
                    if (g.city_level > 0) {
                        g.scene.city_upgrade_level = g.city_level;
                    }
                    if (!g.city_wall.empty()) g.scene.city_wall = g.city_wall;
                    g.scene.city_enh_lines =
                        "Upgrade Level: " +
                        std::to_string(g.scene.city_upgrade_level) + "\r" +
                        g.scene.city_wall;
                    if (g.city_change) {
                        g.scene.city_change_production();
                        if (g.city_activity >= 0) {
                            g.scene.city_activity_select(g.city_activity);
                        }
                    }
                }
                // race=[...] is CityRaceLbl's "Race: <name>" text, i.e. the
                // shipped `AoWE.RaceNamesRStr[race]` string for the city's own
                // race byte (see Scene::open_structure), so the label can be
                // asserted head-lessly against the map data.
                std::printf("structure_panel=%d,%d city=%d name=[%s] race=[%s] "
                            "race_id=%d\n",
                            g.scene.game_struct_hx, g.scene.game_struct_hy,
                            g.scene.game_struct_city ? 1 : 0,
                            g.scene.game_struct_name.c_str(),
                            g.scene.game_struct_race.c_str(),
                            g.scene.game_struct_race_index);
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
        // The Settings panel's own controls, for a head-less fidelity render:
        // --scen-playmode / --scen-turntime / --scen-turns / --scen-checks
        // drive exactly the state the mouse handlers write.
        if (g.scen_playmode) g.scene.scen_set_play_mode(g.scen_playmode);
        if (g.scen_turntime >= 0) g.scene.scen_slider_set(0, g.scen_turntime);
        if (g.scen_turns >= 0) g.scene.scen_slider_set(1, g.scen_turns);
        for (std::size_t i = 0; i < 4 && i < g.scen_checks.size(); ++i) {
            g.scene.scen_check[i] = g.scen_checks[i] == '1';
        }
        g.scene.scen_mode_dd = g.scen_mode_dd;
        // --scen-click X Y: press + release at that point. The press path is
        // the sliders' (TAOWHScrollBar.CheckMouseDown) and the release path is
        // scen_action's (buttons, combos, checkboxes).
        for (const auto& c : g.scen_clicks) {
            g.scene.mouse_x = c.first;
            g.scene.mouse_y = c.second;
            const int hit = g.scene.scen_hit(c.first, c.second);
            g.scene.scen_hover = hit;
            if (hit == -5010 || hit == -5011) {
                g.scene.scen_slider_down(hit == -5010 ? 0 : 1, c.first);
            } else if (hit != -1) {
                scen_action(hit);
            }
            g.scene.scen_drag_slider = -1;
        }
        // --scen-drag X Y X2: press on a slider's thumb, then drag it to X2.
        for (const auto& d : g.scen_drags) {
            g.scene.mouse_x = d[0];
            g.scene.mouse_y = d[1];
            const int hit = g.scene.scen_hit(d[0], d[1]);
            if (hit == -5010 || hit == -5011) {
                g.scene.scen_slider_down(hit == -5010 ? 0 : 1, d[0]);
                g.scene.scen_slider_move(d[2]);
            }
            g.scene.scen_drag_slider = -1;
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
        // A parked pointer behaves like a real WM_MOUSEMOVE: the settings
        // controls track their hover state (checkbox Lit face, combo row).
        if (g.scene.screen == 2) {
            g.scene.scen_hover = g.scene.scen_hit(g.scene.mouse_x,
                                                  g.scene.mouse_y);
            g.scene.scen_chk_hover =
                g.scene.scen_chk_at(g.scene.mouse_x, g.scene.mouse_y);
            if (g.scene.scen_mode_dd && g.scene.scen_hover <= -5100 &&
                g.scene.scen_hover > -5100 - kSetModeRows) {
                g.scene.scen_mode_dd_hover = -g.scene.scen_hover - 5100;
            }
        }
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
        if (g.game_prekey != 0) g.scene.game_key(g.game_prekey);
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
        for (const auto& c : g.game_captures) {
            apply_game_capture(c.first, c.second);
        }
        for (const auto& c : g.game_golds) {
            apply_game_gold(c.first, c.second);
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
        // --game-turns N: play N whole days. Each pass waits for the human's
        // turn, clicks the TurnButton (Checked glow), then runs the AI seats and
        // the one new-day rotation until the next day hands the seat back. This
        // is what exercises a city's production between turns head-lessly.
        if (g.game_turns > 0) {
            Scene& s = g.scene;
            for (int t = 0; t < g.game_turns; ++t) {
                for (int i = 0;
                     i < 20000 &&
                     (s.mv_gs.current() != s.mv_human || s.tb_turn_animating);
                     ++i) {
                    s.game_update();
                }
                if (s.mv_gs.current() != s.mv_human) break;
                s.game_turn_button_click();
                const int day0 = s.game_day;
                for (int i = 0; i < 20000; ++i) {
                    s.game_update();
                    if (s.game_day > day0 && s.mv_gs.current() == s.mv_human &&
                        !s.tb_turn_animating) {
                        break;
                    }
                }
            }
        }
        // Clicks that must land after the days above ran (select / move an army
        // a city produced during them). Finish any walk they start, then settle
        // the hover so the render log reports the final army position.
        if (!g.game_after_clicks.empty()) {
            for (const GameClick& c : g.game_after_clicks) {
                apply_game_click(c.x, c.y, c.right);
            }
            while (g.scene.mv_anim) g.scene.game_update();
            g.scene.game_update();
        }
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
                             cv.to_rgba(g.rgb555), err)) {
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
        // The Settings panel's live state (docs/08_SCENARIO_SCREEN.md sec. 10):
        // the turn mode, the two Duration sliders and the four checkboxes.
        std::printf("scen_set mode=%d turntime=%d turns=%d checks=%d%d%d%d "
                    "mode_dd=%d chk=%u lit=%u\n",
                    g.scene.scen_play_mode, g.scene.scen_turn_time,
                    g.scene.scen_turns,
                    static_cast<int>(g.scene.scen_check[0]),
                    static_cast<int>(g.scene.scen_check[1]),
                    static_cast<int>(g.scene.scen_check[2]),
                    static_cast<int>(g.scene.scen_check[3]),
                    static_cast<int>(g.scene.scen_mode_dd),
                    static_cast<unsigned>(g.scene.scen_ngcbox[1].w),
                    static_cast<unsigned>(g.scene.scen_ngcbox[2].w));
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
        else if (a == "--game-capture") {
            next();
            const int cx = std::atoi(v.c_str());
            int cy = 0;
            if (i + 1 < argc) cy = std::atoi(acp_from_w(argv[++i]).c_str());
            g.game_captures.emplace_back(cx, cy);
        }
        else if (a == "--game-gold") {
            next();
            const int gx = std::atoi(v.c_str());
            int gy = 0;
            if (i + 1 < argc) gy = std::atoi(acp_from_w(argv[++i]).c_str());
            g.game_golds.emplace_back(gx, gy);
        }
        else if (a == "--game-right-click") {
            next();
            const int rx = std::atoi(v.c_str());
            int ry = 0;
            if (i + 1 < argc) ry = std::atoi(acp_from_w(argv[++i]).c_str());
            g.game_clicks.push_back(GameClick{rx, ry, true});
        }
        else if (a == "--game-after-click" || a == "--game-after-right-click") {
            next();
            const int ax = std::atoi(v.c_str());
            int ay = 0;
            if (i + 1 < argc) ay = std::atoi(acp_from_w(argv[++i]).c_str());
            g.game_after_clicks.push_back(
                GameClick{ax, ay, a == "--game-after-right-click"});
        }
        else if (a == "--game-key") {
            next();
            if (!v.empty()) g.game_key = static_cast<unsigned char>(v[0]);
        }
        else if (a == "--game-prekey") {
            next();
            if (!v.empty()) g.game_prekey = static_cast<unsigned char>(v[0]);
        }
        else if (a == "--game-bench") { next(); g.game_bench = std::atoi(v.c_str()); }
        else if (a == "--game-anim") { next(); g.game_anim = std::atoi(v.c_str()); }
        else if (a == "--game-turns") {
            next();
            g.game_turns = std::atoi(v.c_str());
        }
        else if (a == "--game-clock") {
            next();
            g.game_clock_override = std::atoi(v.c_str());
        }
        else if (a == "--turn-frame") {
            next();
            g.turn_frame_override = std::atoi(v.c_str());
        }
        else if (a == "--structure-panel") {
            g.structure_panel = true;
            g.rgb555 = true;   // the reference captures are RGB555 screenshots
        }
        else if (a == "--city-screen") {
            g.city_screen = true;
            g.rgb555 = true;   // the reference captures are RGB555 screenshots
        }
        else if (a == "--city-change") {
            g.city_change = true;
            g.city_screen = true;
            g.rgb555 = true;
        }
        else if (a == "--city-activity") {
            next();
            g.city_activity = std::atoi(v.c_str());
        }
        else if (a == "--city-level") { next(); g.city_level = std::atoi(v.c_str()); }
        else if (a == "--city-wall") { next(); g.city_wall = v; }
        else if (a == "--rgb555") { g.rgb555 = true; }
        else if (a == "--structure-tile") {
            next();
            g.structure_tile_x = std::atoi(v.c_str());
            next();
            g.structure_tile_y = std::atoi(v.c_str());
        }
        else if (a == "--structure-name") { next(); g.structure_name = v; }
        else if (a == "--structure-race") { next(); g.structure_race = v; }
        else if (a == "--structure-race-index") { next(); g.structure_race_index = std::atoi(v.c_str()); }
        else if (a == "--structure-status") { next(); g.structure_status = v; }
        else if (a == "--structure-gold") { next(); g.structure_gold = std::atoi(v.c_str()); }
        else if (a == "--structure-mana" || a == "--structure-level") {
            // CityLevelLbl's value (the DFM has no mana label here); the old
            // flag name is kept for the existing scripts.
            next();
            g.structure_mana = std::atoi(v.c_str());
        }
        else if (a == "--structure-relation") { next(); g.structure_relation = std::atoi(v.c_str()); }
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
        else if (a == "--scen-playmode") {
            next();
            g.scen_playmode = std::atoi(v.c_str());
        }
        else if (a == "--scen-turntime") {
            next();
            g.scen_turntime = std::atoi(v.c_str());
        }
        else if (a == "--scen-turns") {
            next();
            g.scen_turns = std::atoi(v.c_str());
        }
        else if (a == "--scen-checks") { next(); g.scen_checks = v; }
        else if (a == "--scen-mode-dd") { g.scen_mode_dd = true; }
        else if (a == "--scen-click") {
            next();
            const int cx = std::atoi(v.c_str());
            const int cy = (i + 1 < argc)
                               ? std::atoi(acp_from_w(argv[++i]).c_str())
                               : 0;
            g.scen_clicks.emplace_back(cx, cy);
        }
        else if (a == "--scen-drag") {
            next();
            std::array<int, 3> d{std::atoi(v.c_str()), 0, 0};
            if (i + 1 < argc) d[1] = std::atoi(acp_from_w(argv[++i]).c_str());
            if (i + 1 < argc) d[2] = std::atoi(acp_from_w(argv[++i]).c_str());
            g.scen_drags.push_back(d);
        }
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
