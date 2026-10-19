# CMakeLists.txt - AoW1 C++ port skeleton.
#
# Build (with CMake >= 3.16):
#   cmake -S port/src -B port/build
#   cmake --build port/build --config Release
#
# A CMake-free fallback lives in port/build.ps1 (uses g++/clang++ directly).
cmake_minimum_required(VERSION 3.16)
project(aow_port CXX)

# The game executable (aow_menu -> AoW.exe) embeds the game's icon from a .rc
# resource, so the RC compiler must be available.
if(WIN32)
  enable_language(RC)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(aow_engine STATIC
    engine/ilb.cpp
    engine/pfs.cpp
    engine/pfs_types.cpp
    engine/unit_catalog.cpp
    engine/inflate.cpp
    engine/map.cpp
    engine/map_resources.cpp
    engine/mapviz.cpp
    engine/game_state.cpp
    engine/movement.cpp
    engine/unit_anim.cpp
    engine/png.cpp
    engine/audio.cpp
    engine/galaxy_it.cpp
    engine/galaxy_player.cpp
    engine/openmpt_player.cpp
    engine/playlist.cpp
)
target_include_directories(aow_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# libopenmpt decodes the game's .IT/.S3M music.  On MSYS2 install
# mingw-w64-ucrt-x86_64-libopenmpt (or your distro's libopenmpt dev package).
find_library(OPENMPT_LIBRARY NAMES openmpt)
if(NOT OPENMPT_LIBRARY)
  message(FATAL_ERROR
    "libopenmpt not found. Install mingw-w64-ucrt-x86_64-libopenmpt (MSYS2) "
    "or the libopenmpt development package for your toolchain.")
endif()
message(STATUS "libopenmpt: ${OPENMPT_LIBRARY}")

# `aow_tools`: the bootstrap CLI that decodes the game's data files (ilb / png /
# mapviz / unitgfx / ...).  It backs the reverse-engineering helper scripts
# (tools/sheet*.py).  It used to be built as `aow.exe`; that name now belongs to
# the game executable below (the title screen / menu).
add_executable(aow_tools
    game/main.cpp
)
target_link_libraries(aow_tools PRIVATE aow_engine)

# `aow_view`: the double-clickable GUI map viewer (Win32 + GDI). Built as a
# Windows-subsystem binary so launching it opens the file dialog directly.
add_executable(aow_view WIN32
    game/viewer.cpp
)
target_link_libraries(aow_view PRIVATE aow_engine comdlg32 gdi32 user32 shell32 ole32)

# `aow_unitview`: the unit viewer (Win32 + GDI). Browse every unit of the game
# (its idle sprite); click / Enter opens the unit's page with the name,
# description, stats and attributes. `--png` renders one view head-lessly.
add_executable(aow_unitview WIN32
    game/unitviewer.cpp
)
target_link_libraries(aow_unitview PRIVATE aow_engine gdi32 user32 shell32)

# `aow_audio`: the audio showcase.  Replays the game's .IT music and .WAV/.VAG
# samples through the ported engine using the WinMM waveOut API.
add_executable(aow_audio
    game/audio_showcase.cpp
)
target_link_libraries(aow_audio PRIVATE aow_engine winmm ${OPENMPT_LIBRARY})

add_executable(aow_audio_tests
    tests/audio_test.cpp
)
target_link_libraries(aow_audio_tests PRIVATE aow_engine winmm ${OPENMPT_LIBRARY})

# `aow_playlist_tests`: the scenario music playlist (aow::scenario_playlist +
# aow::PlaylistPlayer) - the song list a map plays, in order, wrapping at the end.
add_executable(aow_playlist_tests
    tests/playlist_test.cpp
)
target_link_libraries(aow_playlist_tests PRIVATE aow_engine ${OPENMPT_LIBRARY})

# `aow_menu`: the title screen / main menu replica (Win32 + GDI + WinMM).  It
# composes the game's own assets exactly as AoW.exe does - see
# docs/06_TITLE_SCREEN.md.  It *is* the port's game executable, so it is built
# as `AoW.exe` and carries the game's own icon (Age of Wonders.ico).
set(AOW_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Age of Wonders.ico")
configure_file(resources/aow.rc.in ${CMAKE_CURRENT_BINARY_DIR}/aow.rc @ONLY)
add_executable(aow_menu WIN32
    game/menu.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/aow.rc
)
set_target_properties(aow_menu PROPERTIES OUTPUT_NAME AoW)
target_link_libraries(aow_menu PRIVATE aow_engine gdi32 user32 winmm shell32 ${OPENMPT_LIBRARY})

add_executable(aow_galaxy_tests
    tests/galaxy_test.cpp
)
target_link_libraries(aow_galaxy_tests PRIVATE aow_engine)

add_executable(aow_tests
    tests/pfs_types_test.cpp
)
target_link_libraries(aow_tests PRIVATE aow_engine)

# `aow_unit_tests`: the reconstructed AoWE.TUnit class (rank/experience, stat
# getters, morale, ability bit array, exact 32-bit object layout). Header-only.
add_executable(aow_unit_tests
    tests/unit_test.cpp
)
target_link_libraries(aow_unit_tests PRIVATE aow_engine)

# `aow_unit_catalog_tests`: loads every unit of the game from the shipped
# Unitres.pfs + Unitgfx.pfs + Release.hss + *.ahl and checks the roster.
add_executable(aow_unit_catalog_tests
    tests/unit_catalog_test.cpp
)
target_link_libraries(aow_unit_catalog_tests PRIVATE aow_engine)

add_executable(aow_map_tests
    tests/map_test.cpp
)
target_link_libraries(aow_map_tests PRIVATE aow_engine)

# `aow_game_state_tests`: the scenario player roster + the turn/day loop.
add_executable(aow_game_state_tests
    tests/game_state_test.cpp
)
target_link_libraries(aow_game_state_tests PRIVATE aow_engine)

# `aow_movement_tests`: the recovered world-map movement rules - the extracted
# move-cost tables, the ability/army fold, the hex geometry, the bounded
# pathfinder and the Cursors.ilb overlay frames.
add_executable(aow_movement_tests
    tests/movement_test.cpp
)
target_link_libraries(aow_movement_tests PRIVATE aow_engine)

# `aow_unit_anim_tests`: the recovered unit image-sequence animation - the
# sequence layout, the direction -> sequence mapping, the frame-index rule, the
# per-hex step count and the sub-hex walk (docs/16_UNIT_ANIM.md).
add_executable(aow_unit_anim_tests
    tests/unit_anim_test.cpp
)
target_link_libraries(aow_unit_anim_tests PRIVATE aow_engine)

add_executable(aow_mapres_tests
    tests/mapres_test.cpp
)
target_link_libraries(aow_mapres_tests PRIVATE aow_engine)

add_executable(aow_mapviz_golden
    tests/mapviz_golden.cpp
)
target_link_libraries(aow_mapviz_golden PRIVATE aow_engine)

if(MSVC)
  target_compile_options(aow_engine PRIVATE /W4)
  target_compile_options(aow_tools PRIVATE /W4)
  target_compile_options(aow_view PRIVATE /W4)
  target_compile_options(aow_unitview PRIVATE /W4)
  target_compile_options(aow_audio PRIVATE /W4)
  target_compile_options(aow_menu PRIVATE /W4)
  target_compile_options(aow_audio_tests PRIVATE /W4)
  target_compile_options(aow_playlist_tests PRIVATE /W4)
  target_compile_options(aow_tests PRIVATE /W4)
  target_compile_options(aow_unit_tests PRIVATE /W4)
  target_compile_options(aow_unit_catalog_tests PRIVATE /W4)
  target_compile_options(aow_map_tests PRIVATE /W4)
  target_compile_options(aow_mapres_tests PRIVATE /W4)
  target_compile_options(aow_game_state_tests PRIVATE /W4)
  target_compile_options(aow_movement_tests PRIVATE /W4)
  target_compile_options(aow_unit_anim_tests PRIVATE /W4)
  target_compile_options(aow_mapviz_golden PRIVATE /W4)
else()
  target_compile_options(aow_engine PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_tools PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_view PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_unitview PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_audio PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_menu PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_audio_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_playlist_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_unit_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_unit_catalog_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_map_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_mapres_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_game_state_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_movement_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_unit_anim_tests PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(aow_mapviz_golden PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Rule-fidelity tests: `ctest` loads the shipped Release/*.PFS.
enable_testing()
add_test(NAME pfs_types
         COMMAND aow_tests ${CMAKE_CURRENT_SOURCE_DIR}/../../Release)
# Unit-class tests: reconstructed TUnit rules + object layout (no data files).
add_test(NAME unit
         COMMAND aow_unit_tests)
# Unit-catalog test: load every unit of the game from the shipped game data.
add_test(NAME unit_catalog
         COMMAND aow_unit_catalog_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Unit-viewer smoke test: head-lessly render the browser + one unit page.
add_test(NAME unit_viewer_render
         COMMAND aow_unitview ${CMAKE_CURRENT_SOURCE_DIR}/../..
                 --png ${CMAKE_CURRENT_BINARY_DIR}/unitview_browse.png --unit 0)
add_test(NAME unit_viewer_page
         COMMAND aow_unitview ${CMAKE_CURRENT_SOURCE_DIR}/../..
                 --png ${CMAKE_CURRENT_BINARY_DIR}/unitview_detail.png
                 --unit 18 --detail)
# Map-format tests: `ctest` loads every Scenario/* map.
add_test(NAME map_format
         COMMAND aow_map_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Game-state tests: load the roster + troops and run the turn loop.
add_test(NAME game_state
         COMMAND aow_game_state_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Movement tests: the cost tables, the army budget, the pathfinder, the overlay.
add_test(NAME movement
         COMMAND aow_movement_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Unit-animation tests: the sequence layout, the frame rule and the sub-hex walk.
add_test(NAME unit_anim
         COMMAND aow_unit_anim_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Resource/render tests: `ctest` reads Release.hss and renders a map.
add_test(NAME map_resources
         COMMAND aow_mapres_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Golden render harness: writes eyeball PNGs under re/golden.
add_test(NAME mapviz_golden
         COMMAND aow_mapviz_golden ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Viewer smoke test: the GUI binary's head-less render path.
add_test(NAME viewer_render
         COMMAND aow_view
                 ${CMAKE_CURRENT_SOURCE_DIR}/../../Scenario/Custom/Tutorial.hsm
                 --root ${CMAKE_CURRENT_SOURCE_DIR}/../..
                 --render ${CMAKE_CURRENT_BINARY_DIR}/viewer_test.png)
# Audio decoder tests: every .IT song + a sample of the .WAV/.VAG library.
add_test(NAME audio_decode
         COMMAND aow_audio_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Playlist tests: the scenario music list is played in order, wrapping at the end.
add_test(NAME playlist
         COMMAND aow_playlist_tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# Title-screen smoke test: head-less render + the recovered geometry.
add_test(NAME menu_render
         COMMAND aow_menu
                 --root ${CMAKE_CURRENT_SOURCE_DIR}/../..
                 --render ${CMAKE_CURRENT_BINARY_DIR}/menu_test.png
                 --frame 90 --no-music)
# Scenario screen smoke test: head-less render (fully lit) + the map info strip.
add_test(NAME scenario_render
         COMMAND aow_menu
                 --root ${CMAKE_CURRENT_SOURCE_DIR}/../..
                 --render ${CMAKE_CURRENT_BINARY_DIR}/scenario_test.png
                 --screen scenario --scen-frame 90 --no-music --no-cursor)
