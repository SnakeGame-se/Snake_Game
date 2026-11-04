# Deviations from the submitted documents

This file lists where the finished game differs from the SRS, the SAD
(`Snake_Game_SAD.pdf`) and the Test Plan (`Snake_Game_Test_Plan.pdf`), and why.
Requirement IDs (SNK-F-xxx, SNK-NF-xxx, SNK-SR-xxx) are the ones used in the SRS.

## 1. Features not delivered or only partly delivered

| # | Documented | What was built | Status |
|---|---|---|---|
| 1 | **Graphics interface** (SRS SNK-F-001 to 003 and section 2.2; SAD "Graphics UI Component"): the player can choose console or graphics mode. | Console mode only. Option 2 in the title menu is shown, but it prints "Graphics mode is not implemented yet" and exits. | Not delivered. The game logic is separate from the drawing code, so a graphics front-end could be added without changing the engine. |
| 2 | **Tick-time measurement** (SRS SNK-NF-001: tick processing at most 50 ms; Test Plan TC-Perf-01). | The game waits a fixed 120 ms per tick (`TICK_MS` in `main.c`). The time spent processing a tick is tiny, but it is not measured in code. | Not verified. TC-Perf-01 is not automated. |
| 3 | **Security tooling** (Test Plan section 5.1 and 6: Valgrind, cppcheck, fuzzing of key input). | These tools are not part of the repository's automated tests. Safety is handled in code instead: argument checks in `food_generate()`, length clamping in `collision_hits_self()`, a bounded frame buffer in the renderer, and unmapped keys are ignored. | Replaced by unit tests and code checks. |

## 2. Controls differ from the SRS

| Documented | Built |
|---|---|
| SRS 3.2: Enter or Space starts, pauses and restarts; Esc or Q quits. | Move with arrow keys or W/A/S/D. **Space or P** pauses and resumes. **R** restarts. **Q or Esc** quits. Enter is used only at the "Press Enter to start" prompt. |
| SAD 4.5: P pauses, Q quits. | Matches (P and Q both work). |

Reason: R for restart was already used by the game loop when the keyboard
module (SG-11) was written, and it avoids restarting by accident while playing.
On the game-over screen only R and Q/Esc are accepted, as the on-screen text says.

## 3. Architecture: how the SAD components map to the code

The layered design in the SAD was followed, but some component and function
names are different.

| SAD component | Where it is in the code |
|---|---|
| Console UI Component | `game_interface.c` (`interface_render_console`, `interface_read_key`) and `input_controls.c` |
| Graphics UI Component | Not implemented (see deviation 1) |
| Mode Selection Controller | `interface_select_mode()` in `game_interface.c` |
| Game Engine Component | `game_engine.c` and the game loop in `main.c` |
| Collision Detection Component | Wall and obstacle checks are inside `engine_tick()`; self-collision is in `collision.h` (SG-17) |
| Score & Session Manager | `score_manager.c` |
| Game State Store | The `GameState` structure in `game_state.h` (held in memory only) |

| SAD interface (section 4.3) | Implemented as |
|---|---|
| `GameControlInterface`: `startGame`, `moveSnake`, `pauseGame`, `resumeGame`, `restartGame`, `getRenderState` | `engine_initialize`, `engine_set_direction`, `engine_tick`, `session_start`, `session_pause`, `session_resume`, `session_toggle_pause`. The screen-drawing code receives the whole `GameState`, so there is no separate `getRenderState`. |
| `CollisionCheckInterface`: wall, obstacle, self and food checks | Inline in `engine_tick()`, except self-collision (`collision_hits_self`). |

## 4. Other changes while building

- Food placement uses `food_generate()` (SG-13), which always finds a free cell
  if one exists. The first version in the engine tried random cells up to 800
  times and could give up too early on a nearly full board.
- Obstacles (SRS SNK-F-011) were delivered as specified: 8 fixed obstacle cells,
  placed at the start of every game and the same for every player. Hitting one
  ends the game, and food is never placed on one.
- When the snake fills the whole board the game ends, because no new food can
  be placed.
- Key presses are read with `input_poll()` (SG-11), which returns the most
  recent key since the last tick, so the snake does not keep turning after the
  player lets go of a key.
- The screen is redrawn in place instead of being cleared every frame, which
  removed the flicker.

## 5. Added during final testing

_Add anything else you find while fixing bugs (SG-27) and testing (SG-23, SG-25,
SG-26), with the reason._
