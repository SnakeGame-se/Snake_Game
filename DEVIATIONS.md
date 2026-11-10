# Deviations from the Submitted Documents

This file documents where the finished game differs from the Software Requirements Specification (SRS), Software Architecture and Design document (SAD), and Test Plan, and explains the implementation decisions.

Requirement IDs (SNK-F-xxx, SNK-NF-xxx and SNK-SR-xxx) refer to the SRS.

## 1. Features Not Delivered or Partly Delivered

| # | Documented | What Was Built | Status |
|---|---|---|---|
| 1 | **Tick-time measurement** (SRS SNK-NF-001: tick processing at most 50 ms; Test Plan TC-Perf-01). | The game uses a fixed 120 ms tick interval (`TICK_MS` in `main.c`). Tick-processing duration is not measured in the application. | Not verified. TC-Perf-01 is not automated. The 120 ms interval is the delay between game updates, not a measurement of processing time. |
| 2 | **Security tooling** (Test Plan sections 5.1 and 6: Valgrind, cppcheck and fuzzing of key input). | These tools are not integrated into the repository's automated tests or GitHub Actions workflow. The project includes input validation, argument checks in `food_generate()`, length clamping in `collision_hits_self()` and automated unit tests. | Partly addressed through code-level checks and unit testing. These do not replace dedicated security analysis or fuzz testing. |

### Platform Support

The graphics interface is implemented using the Windows API (GDI) and is available on Windows.

The console interface is available on Windows, Linux and macOS. On Linux and macOS, selecting graphics mode displays a message that graphics mode is available only on Windows.

## 2. Controls Differ from the SRS

| Documented | Built |
|---|---|
| SRS 3.2: Enter or Space starts, pauses and restarts; Esc or Q quits. | Arrow keys or W/A/S/D move the snake. Space or P pauses and resumes. R restarts. Q or Esc quits. Enter is used at the start prompt. |
| SAD 4.5: P pauses, Q quits. | Matches: P pauses and Q quits. |

**Reason:** R was used for restart when the keyboard module (SG-11) was implemented. This avoids restarting accidentally during gameplay.

On the game-over screen, only R and Q/Esc are accepted, as indicated by the on-screen instructions.

## 3. Architecture: How the SAD Components Map to the Code

The layered architecture described in the SAD was followed, although some component and function names differ.

| SAD Component | Implementation |
|---|---|
| Console UI Component | `game_interface.c` (`interface_render_console`, `interface_read_key`) and `input_controls.c` |
| Graphics UI Component | `graphics_interface.c` and `graphics_interface.h` (Windows GDI implementation) |
| Mode Selection Controller | `interface_select_mode()` in `game_interface.c` |
| Game Engine Component | `game_engine.c` and the game loop in `main.c` |
| Collision Detection Component | Wall and obstacle checks inside `engine_tick()`; self-collision in `collision.h` (`collision_hits_self()`) |
| Score & Session Manager | `score_manager.c` |
| Game State Store | The `GameState` structure in `game_state.h`, held in memory |

### SAD Interfaces (Section 4.3)

| SAD Interface | Implementation |
|---|---|
| `GameControlInterface`: `startGame`, `moveSnake`, `pauseGame`, `resumeGame`, `restartGame`, `getRenderState` | `engine_initialize`, `engine_set_direction`, `engine_tick`, `session_start`, `session_pause`, `session_resume` and `session_toggle_pause`. The rendering code receives the shared `GameState`, so there is no separate `getRenderState` function. |
| `CollisionCheckInterface`: wall, obstacle, self and food checks | Wall and obstacle checks are handled in `engine_tick()`, while self-collision is handled by `collision_hits_self()`. |

## 4. Other Changes During Implementation

- **Food placement:** Food generation uses `food_generate()` (SG-13), which finds a free cell when one exists. The earlier engine implementation tried random positions up to 800 times and could give up too early on a nearly full board.
- **Obstacles:** The console game initializes eight fixed obstacle cells. The graphics interface also sets up graphics-specific obstacles. Hitting an obstacle ends the game, and food placement avoids occupied cells.
- **Full board handling:** When no free cell remains for food placement, the game ends because no further food can be generated.
- **Keyboard input:** Key presses are read using `input_poll()` (SG-11), which returns the most recent key since the previous tick. This prevents the snake from repeatedly changing direction after a key is released.
- **Console rendering:** The screen is redrawn in place rather than cleared every frame, reducing flicker.
- **Platform-specific graphics:** The graphics interface uses the Windows API and is conditionally compiled for Windows. The Linux CI build compiles the console application without the Windows-specific graphics source.

## 5. Improvements During Final Testing

The following improvements were made during implementation and testing:

- Added automated tests for snake movement, direction validation and invalid input handling (SG-23).
- Added collision tests covering walls, obstacles, self-collision and game-over behaviour (SG-25).
- Added restart and score-management tests to verify session reset behaviour (SG-26).
- Corrected the restart test to verify that the initial eight obstacles are restored.
- Integrated the automated test programs into the GitHub Actions workflow.
- Added AddressSanitizer and UndefinedBehaviorSanitizer execution to the CI workflow to detect memory errors and certain forms of undefined behaviour.

The automated test coverage and execution instructions are documented in `docs/TESTING.md`.
