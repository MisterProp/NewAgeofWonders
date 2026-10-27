# 22 - The two turn buttons (`TBNextButton` / `TurnButton`) and the turn loop

This document records the two controls at the right end of the bottom bar
(`TTBWindow`), the class behind the gold/blue round button, and the turn loop
they drive - all recovered with **GhidraMCP** from `AoW.exe`,
`AOWTools.dpl`, `AoWInterface.dpl` and `AoWEPACK.dpl`, and reproduced in
`src/game/menu.cpp` + `src/engine/game_state.cpp`.

| control | class | module | handler |
|---------|-------|--------|---------|
| `TBNextButton` (568,7 26x38) | `TAOWButton` | AoWInterface.dpl | `TBNextButtonClick` @ `AoW.exe 0x004573e8` |
| `TurnButton` (596,7 41x38) | `TAOWTurnButton` | AOWTools.dpl, unit `TurnButton` | `TurnButtonChange` @ `0x004574ac`, `TurnButtonAnimDone` @ `0x004572f4` |

## 1. How the handlers were found

Delphi does not export a form's own methods. `TTBWINDOW.dfm` names them, and the
published-method table holds one `<u16><u32 addr><u8 len><name>` record per
handler (the same technique as `docs/20_PARTY_WINDOW.md`). Decoding the records
around the pooled DFM strings at `0x00457052..0x00457135` gives:

```
TBNextButtonClick   0x004573e8
TurnButtonChange    0x004574ac
TurnButtonAnimDone  0x004572f4
```

The decode validates against the Party window's documented entries
(`Unit1FramePreDraw 0x0040674c`, `CheckEnd 0x004062a0`,
`PGuardChange 0x00406848`). Ghidra had not created functions at the two button
addresses; `create_function` + `decompile_function` over the MCP produced:

```c
void TBNextButtonClick(void) {
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420, 0);   // Sfx\Int\Btn1.wav
  AoWE_TAoWHSMap_SelectNextArmy_23EDC2EF(*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref);
}

void TurnButtonChange(int param_1) {                  // param_1 = the TTBWindow
  int i = TPlayerList_GetPlayers(map->players, map->seated /*+0xa5*/);
  if (players[i].endTurn /*+0x2c*/ == 0) {
    TurnButton->checked /*+0x13c*/ = TAoWHSMap_EndTurn(map);
  } else {
    TurnButton->checked = TAoWHSMap_CancelEndTurn(map) ^ 1;
  }
}

void TurnButtonAnimDone(int param_1) {                // OnAnimDone
  TurnButton->checked = 0;
  if (AoWCombatMap != 0) AoWCombatMap->flag0x128 = 1;
}
```

`TurnButtonChange` runs from `TAOWTurnButton.CheckMouseDown`, i.e. on
**mouse-down**, not on release (section 2).

## 2. `TAOWTurnButton` - the class behind the round button

The class lives in the `TurnButton` unit compiled into `AOWTools.dpl`
(image base `0x59700000`), which `AoW.exe` imports (`TurnButton.TAOWTurnButton`
in its import table, next to `TurnButton.TAOWTurnButton.Draw`, `.Update`, ...).
The export table gives the real addresses:

| method | address | role |
|--------|---------|------|
| `Create` | `0x59704c3c` | instance size **0x144**; `AnimStart (+0x128) = 0xB` |
| `Validate` | `0x59704c74` | enables the button when the image library is set |
| `SetSize` | `0x59704c88` | takes the width/height from image 0 |
| `SetILB` | `0x59704cc0` | `ImageLib (+0x114)` |
| `Update` | `0x59704cd4` | the turn-change animation (below) |
| `Draw` | `0x59704e44` | frame selection + the image blit |
| `CheckMouseDown` | `0x59705018` | toggles `Checked`, fires `OnChange` |
| `CheckMouseMove` | `0x59705188` | hover state |
| `CheckMouseUp` | `0x59705100` | clears the pressed state |

The DFM's published properties (their names sit in the class's property table at
`0x59704b05..0x59704c32`) map to the fields the code uses:

| property | offset | meaning |
|----------|--------|---------|
| `ImageLib` | `+0x114` | the 102-frame `Int\TBTurnBt.ILB` |
| `CurPlayer` | `+0x118` | the block to show - the **seated** player's race id (0..11), 12 = none; written only by the map-open setup |
| `LastPlayer` | `+0x11c` | the block the last animation ended on; `-1` forces the new-day rotation |
| `Animating` | `+0x120` | the change animation is playing |
| `AnimStart` | `+0x128` | 11 at rest, 0 while the glow block plays |
| `Frame` | `+0x140` | the animation's own frame |
| `Checked` | `+0x13c` | "this seat has ended its turn" |
| `OnAnimDone` / `OnChange` | `+0x12c..` / `+0x134..` | the DFM's events |

### The frame layout of `Int\TBTurnBt.ILB`

102 frames, `41x38`, class 0x10:

```
frames  0..10          the "turn changing" swirl (the glow block, Update mode 2)
frames 11+7*r..17+7*r  one block per race r = 0..12 (12 = no seat)
                       the block's last frame (+6) is its transition swirl, and
                       is also the "turn has ended" picture
```

### Update (0x59704cd4), once per frame

```
mode 0  idle:  if LastPlayer != CurPlayer: Frame = 7*LastPlayer; AnimStart = 11;
                                             mode = 1; Animating = 1
mode 1  ++Frame; if Frame > 7*LastPlayer + 5: Frame = 0; mode = 2; AnimStart = 0
mode 2  ++Frame; if Frame > 10:               mode = 3; Frame = 7*CurPlayer + 5;
                                              AnimStart = 11
mode 3  --Frame; if Frame <= 7*CurPlayer:     Frame = 7*CurPlayer; mode = 0;
                                              AnimStart = 11; LastPlayer = CurPlayer;
                                              Animating = 0; OnAnimDone()
```

One step per call, i.e. one step per **game frame** (the window's `Update`).
Note that mode 1 compares against **`LastPlayer`** (`+0x11c`), not `CurPlayer`:
`0x59704d2c` reads `[param_1+0x11c]*7+5 < [param_1+0x140]`. The first leg is
therefore always the *old* block's six steps, whatever block the new seat uses.
For any pair of races that is `6 + 11 + 6 = 23` displayed frames (~0.77 s at the
game's 30 fps): the first leg always walks the old block from its first frame
and the last leg always lands the new block from its `+5` frame.

### The new-day trigger

`CurPlayer` is written in exactly one place in the original game: the map-open
setup at `AoW.exe 0x00454d2c` loads the **seated** player's race
(`TPlayer+0xa5`) into both `CurPlayer` and `LastPlayer` and snaps `Frame`.
In a single-player game that seat never changes, so the button would never move
again - except that the day-label handler `FUN_0044ece8` (the routine that
formats `"Day X"`) does this when the day rolls over:

```c
if ((map->flags3c & 4) != 0)                 // single-player rule
    turnButton->LastPlayer /*+0x11c*/ = -1;
```

Because `Update` mode 0 only compares `LastPlayer` with `CurPlayer`, that
invalidation starts a change even though the seated race is unchanged:
`Frame = 7*(-1) = -7`, drawn as library frame `11-7 = 4`. The walk is the glow
tail (`4..9`), the glow block (`0..10`) and the landing on the seat's own block
(`5..0`) - exactly one 23-frame rotation. `TurnButtonAnimDone` clears `Checked`
when it ends, so the button returns to the seat's normal first frame.

The new-day sound is `TTurnPlayerControl.NewTurn` (`0x55756714`) calling
`Sound.PlaySample("SFX\\EVENTS\\NEWTURN.WAV")` for the new player when that
player is human-controlled (`TPlayer+0xa7 == 0`). The port's turn order can put
the independents between the new day and the human's hand-over, so playing it
literally at `NewTurn` would delay it past the 23-frame rotation. It is
therefore played at the same instant as the `LastPlayer = -1` rotation trigger;
the later hand-over to the human's seat does not replay it. No sound is played
for an AI or independent seat, and the turn button itself has no click sound.

### Draw (0x59704e44)

```
if not Visible or not Enabled: return
draw the frame, if any (word [+0x3e])  -> call [+0x3c]([+0x40], Self, rect)
index = (Checked and not Animating) ? AnimStart + 7*CurPlayer + 6
                                    : AnimStart + Frame
ImageLib.Get(index).Show(...)  at (Self.x + Canvas.x, Self.y + Canvas.y)
draw the after-frame if any (word [+0x36])
```

The `Checked` picture is therefore the *block's transition frame* - the button
looks "in progress" while the seat waits for the other players.

### CheckMouseDown (0x59705018)

```
inside the button box?
    if left button and not Animating:
        Checked = !Checked
        if OnChange assigned: OnChange(Self)      -> TurnButtonChange
        if OnClick  assigned: OnClick(Self, x, y, button)
```

Because `TurnButtonChange` writes the result of `EndTurn`/`CancelEndTurn` back
into `Checked`, the toggle above is only provisional; the map decides.

## 3. The turn loop (`AoWEPACK.dpl`)

`TAoWHSMap.EndTurn` (`0x5577838c`) refuses when there is no seated player, and
while the event log is executing and still holds entries; otherwise it posts the
end-turn token (`FUN_5574757c`). The token's execution reaches
`TPlayerControl.EndTurn` (`0x5575530c`) - which calls
`TPlayer.SetEndTurn(player, 1)` for the seated player - and then
`TTurnPlayerControl.EndTurn` (`0x55756db8`):

```
if the player who ended is the one on turn (map+0xa4):
    if the player's +0x45 == 1: clear the execute-event-log buffer
    unselect the map's hex selection
    log "ended turn"
    idx = turn_order.IndexOf(player)
    loop:
        ++idx
        if idx == turn_order.Count:  NextTurn()  (the round wraps -> NewDay)
                                     idx = 0
        p = turn_order[idx]
        if players[p].+0x45 != 1:    TPlayerControl.NewTurn(p)
                                     TPlayerControl.EndTurn(p)      // skipped seat
                                     continue
        break
    SetCurrentPlayer(turn_order[idx])
```

`TPlayer.SetEndTurn` (`0x55752d00`) stores `+0x2c` and fires
`TAoWHSMap.TriggerEndTurnEvent` (`0x55776474`).
`TPlayerControl.NewTurn` (`0x557553c4`), from day 2 on, credits income minus
upkeep, clamps a deficit (food penalty -5, floor -50) and **clears the player's
end-turn flag**; it always bumps the player's turn count.
`TPlayerControl.NewDay` (`0x55754da4`) increments the day (map `+0x174`), and
`GenerateNewTurnOrder` (`0x557782fc`) rebuilds the order as `[0]` plus a random
permutation of `1..N-1`.

So the rules the port reproduces are exactly:

* a turn passes **only** when the player on turn has ended it;
* the next turn goes to the next seat in the order (inactive seats, `+0x45 != 1`,
  are started and ended in the same pass);
* the **day** advances only when the order wraps - i.e. after every player has
  ended their turn.

The port's `aow::GameState` carries the flag as `Player::end_turn`, the latch as
`Player::playing`, and exposes `set_end_turn`, `end_turn(who)` and
`cancel_end_turn(who)`; `advance_turn` is the loop above.
`src/tests/game_state_test.cpp` locks the rules (96 checks).

## 4. The port's implementation

`src/game/menu.cpp`:

* `tb_next_idle` / `tb_next_pressed` - `TBWin.ILB` 3 / 4. The **idle** picture
  is frame 3 and the **held** picture frame 4; the old replica drew a 3-piece
  `{3,3,4}`, so the right third of the button showed the pressed art at rest.
  `TAOWButton.Draw` selects the library's index 0 while at rest and index 1 after
  `CheckMouseDown` set `+0x110 = -1`; `CheckMouseUp` clears it and fires
  `OnClick`. The port draws `tb_next_pressed` only while `tb_next_down` is set
  between `WM_LBUTTONDOWN` and `WM_LBUTTONUP`.
* `tb_turn_frames` - all 102 `Int\TBTurnBt.ILB` frames, decoded at load.
* `tb_turn_cur/last/mode/start/animating/checked/frame` - the class's fields;
  `tb_turn_update()` is the state machine above, run once from `game_update()`
  (the 30 fps frame clock = the window's `Update` cadence). `CurPlayer` is the
  **seated** seat's race and is only written by `tb_turn_set_player()`, the
  map-open snap; `frame` is intentionally allowed to go negative while mode 1
  walks `LastPlayer == -1`.
* `tb_turn_draw_frame()` - `AnimStart + Frame`, or
  `AnimStart + 7*CurPlayer + 6` while Checked and idle, with a bounds check so
  the negative mode-1 frames (`4..9` once `AnimStart` is added) resolve.
* `game_turn_button_click()` - `CheckMouseDown` + `TurnButtonChange`:
  `GameState::end_turn(mv_human)` / `cancel_end_turn(mv_human)`, with the result
  filed back into `tb_turn_checked`; it is ignored while the change animation
  plays (`+0x120`). It only sets the glowing still - it does not move
  `CurPlayer`/`Frame`.
* `game_turn_changed()` - updates the day/read-outs. When the round wrapped
  (`day` advanced) it writes `tb_turn_last = -1`, the analogue of the day
  handler `0x0044ece8`, and starts `SFX\EVENTS\NEWTURN.WAV` at that same
  instant (rotation start); the later hand-over to the human's seat does not
  replay the sound. The next `tb_turn_update()` plays the one 23-frame rotation.
* `game_next_button_click()` - `Sfx\Int\Btn1.wav` + `game_select_army(+1)`
  (`SelectNextArmy`): the seat's armies in map order, continuing from
  `TPlayer+0xb4` (`tb_next_index`), skipping any army with fewer than 2 move
  points or with `(TArmy+0x28 & 3) != 0` (the Guard stance). With no candidate
  the map's selection is cleared.
* `game_turn_tick()` - the seats the port does not simulate (the independents
  and the CPU wizards) play their empty turn and end it through the same
  `GameState::end_turn`. The original spends a real turn on each seat; the port
  holds each for `kGameAiTurnHoldFrames` (23, the button's own change span)
  while the button keeps its Checked glow, then the round wraps and the single
  new-day rotation plays.
* `mv_reset_army_turn()` and `MapHs::army_mp_left` / `army_flags` - the move
  points an army has left this turn (`TAbstractUnit.NewTurn` refills them when
  its owner's turn starts) and the Guard bit the next-army loop tests.
* `play_game_sfx()` + `mix_sfx()` - one-shot samples (`Sfx\Int\Btn1.wav`, and
  `SFX\EVENTS\NEWTURN.WAV` when the human seat's turn starts, exactly as
  `TTurnPlayerControl.NewTurn` does) mixed over the music stream.

`game_begin_turns()` starts the loop on the seated player (`TAoWHSMap.Activate`
hands the first turn to the seat; the order's entry 0 is the independents).

## 5. Proof

`tools/turnbtn_bench.ps1` renders the five states into `bin/TurnButtons.png`
(5x zoom): NextButton up (id 3), NextButton down (id 4), TurnButton idle
(frame 46 = Halflings seat), TurnButton ended (frame 52 = that block's
transition frame), TurnButton swirl (frame 5).

The `-Test` build (`build.ps1`) also checks the rendered bar **pixel for pixel**
against the library entries the class selects (`aow_tools.exe png
Int\TBTurnBt.ILB 46` vs the render's `(596,439 41x38)`, and `Int\TBWin.ILB 3`
vs `(568,439 26x38)`): 0 differing pixels. The scripted flow proves the loop:

```
game_click=turn 616,458 checked=1 day=1             (the Checked glow)
game_endturn=player=1 ok=1 from=1 to=2 day=1        (Halflings -> Goblins)
game_turn=player=2 name=Borok day=1                 (button still Halflings 46)
game_newturn_sound=day=2 at=turnanim-start          (NEWTURN.WAV starts it)
game_turn=player=0 name=Independents day=2          (the order wrapped: day 2)
game_turnanim=start from=-1 to=5                    (day handler: LastPlayer = -1)
game_turnanim=done frame=35                         (23 frames, Halflings block)
game_turn=player=1 name=Trodsen day=2               (button still Halflings 46)
game_msg=your turn - day 2
game_turnnow=player=1 day=2 frame=46
```

and the next-army button:

```
game_click=next 581,458
game_next=15,34 index=0
game_msg=party 1/1
```

with a guarded party producing `game_msg=no army to select` and
`game_army=none` (the engine's `UnselectLocation`).

## 6. Known divergences / TODO(RE)

* `CurPlayer` now follows the **seated** player exactly as `0x00454d2c` does,
  and the only in-game rotation is the `LastPlayer = -1` new-day change
  (`0x0044ece8`). The earlier port followed the player on turn and animated
  between every AI seat; that was the wrong model and has been removed.
* The AI seats' turns are empty (the port has no strategic AI), so they are
  held for one button change span each instead of after their real actions.
* `TPlayer.GetBusy` and the event-log gate are modelled as fields but nothing
  sets them yet (no combat/modal transitions in the port).




