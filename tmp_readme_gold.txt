32:  28_ECONOMY.md       <- gold income from buildings + unit upkeep per turn
83: | `src/game/menu.cpp` (`--screen scenario`) | the **Scenario screen (TScenSetup)**: the 12-race grid, the Turns/Duration/Misc settings, the Scenario/Exit/Start! buttons, the "Select a Map..." `TFileDlgWindow` (real `Scen
85: | `src/game/menu.cpp` (`--screen game`) | the **in-game screen (TWorldMap)** shown after a map is chosen: the port's map renderer paints the level through the engine's **world-map camera**, then the recovered UI on top -
95: | `src/engine/game_state.cpp` + `docs/14_GAME_STATE.md` | **the game state** (Stage 2.3): the scenario's player roster (`TAoWHSMap` tag 0x15 = `AoWE.TPlayerList`, class `0x20020`) parsed into typed `aow::Player` records 
