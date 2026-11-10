# Testing

This document describes the automated and manual testing performed for the Snake Game project. It also maps the test programs to the relevant Jira stories, SRS requirements and Test Plan cases.

## 1. Automated Unit Tests

The project contains **11 automated test programs** in the `tests/` directory.

Each test program runs independently and returns:
- Exit code `0` when all its checks pass.
- Exit code `1` when one or more checks fail.

### 1.1 Test Coverage and Traceability

| Test program | Jira story | Requirements / Test Plan | What it checks |
|---|---|---|---|
| `tests/test_food.c` | SG-13 | SNK-F-008, SNK-SR-001; TC-Food-01 | Standalone food generation, valid board positions, avoidance of snake and obstacles, full-board handling, invalid arguments and random placement. |
| `tests/test_food_growth.c` | SG-24 | SNK-F-008, 009, 010, 015; TC-Food-01, TC-Food-02, TC-Food-03 | Food generation in the running game, snake growth, 10-point scoring, replacement food placement, no growth without eating and obstacle avoidance. |
| `tests/test_self_collision.c` | SG-17 | SNK-F-014; TC-Score-01 | Self-collision detection, tail-cell handling, invalid snake length, U-turn collision and game-over behaviour. |
| `tests/test_collision.c` | SG-25 | SNK-F-012, 013, 014; TC-Obs-02, TC-Obs-03, TC-Score-01 | Collision with all four walls, obstacle collision, self-collision, preservation of the final score and stopping movement after Game Over. |
| `tests/test_game_over.c` | SG-18 | SNK-F-012, 013, 014, 017, 018; TC-Obs-02, TC-Obs-03, TC-Score-01, TC-Session-01, TC-Session-02 | Game-over conditions, final score, frozen game state, restart behaviour and full-board handling. |
| `tests/test_pause_resume.c` | SG-22 | SNK-F-019; TC-Session-03 | Pause and resume, movement and input handling while paused, score preservation, invalid session states, repeated toggles and NULL safety. |
| `tests/test_restart.c` | SG-26 | Restart and session behaviour | Restart during gameplay and after Game Over, resetting score, final score, snake length, direction, positions and initial obstacles. |
| `tests/test_score.c` | SG-26 | Score and session management | Score initialization, food scoring, pause/resume, final score, restart, repeated scoring and NULL safety. |
| `tests/test_score_display.c` | SG-20 | Console display behaviour | Verifies that the current score and game status are displayed correctly by the console interface. |
| `tests/test_snake_movement.c` | SG-23 | SNK-F-004, 005, 006; TC-Move-01, TC-Move-02, TC-Move-03 | Movement in all four directions, direction input, invalid keys, reverse-direction rejection, paused-state input handling and invalid direction safety. |
| `tests/test_engine_initialization.c` | SG-30 | Game initialization behaviour | Initial game status, snake length and positions, direction, obstacle initialization, valid food placement and NULL initialization safety. |

The test programs are separate executables so that failures can be identified independently.

## 2. Running Automated Tests Locally

Run commands from the repository root.

The following commands use GCC and C11.

### 2.1 Compile the test programs

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_collision.c game_engine.c score_manager.c food.c -o test_collision

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_engine_initialization.c game_engine.c score_manager.c food.c -o test_engine_initialization

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_food.c food.c -o test_food

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_food_growth.c game_engine.c score_manager.c food.c -o test_food_growth

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_game_over.c game_engine.c score_manager.c food.c -o test_game_over

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_pause_resume.c game_engine.c score_manager.c food.c -o test_pause_resume

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_restart.c game_engine.c score_manager.c food.c -o test_restart

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_score.c score_manager.c -o test_score

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_score_display.c game_interface.c -o test_score_display

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_self_collision.c game_engine.c score_manager.c food.c -o test_self_collision

gcc -std=c11 -Wall -Wextra -Wpedantic tests/test_snake_movement.c game_engine.c score_manager.c food.c -o test_snake_movement
```

### 2.2 Execute the test programs

On Linux or macOS:

```bash
./test_collision
./test_engine_initialization
./test_food
./test_food_growth
./test_game_over
./test_pause_resume
./test_restart
./test_score
./test_score_display
./test_self_collision
./test_snake_movement
```

On Windows using MinGW, append `.exe` to the executable names and run them from PowerShell using `.\test_name.exe`.

The interactive keyboard demo, `tests/input_demo.c`, is not included in automated testing because it requires keyboard input.

## 3. Manual Testing

The following checks require running the application or interacting with its input controls.

| Check | Test case | Procedure |
|---|---|---|
| Title menu | TC-Menu-01, TC-Menu-03 | Run the game and try console mode, graphics mode, exit and invalid menu entries. |
| Keyboard controls | TC-Move-02, TC-Session-03 | Run `input_demo` and test arrow keys, W/A/S/D, Space, Enter, Q and unmapped keys. |
| Snake movement | TC-Move-01, TC-Move-03 | Play the game and verify that the snake moves one cell per tick and cannot reverse directly into itself. |
| Invalid key handling | TC-Sec-01 | Press invalid and repeated keys while playing and check that the application remains stable. |
| Tick processing time | TC-Perf-01 | Performance timing is not currently automated. See `DEVIATIONS.md`. |

### 3.1 Interactive Keyboard Demo

The keyboard demo is a manual testing utility, not an automated unit test.

Compile from the repository root:

```bash
gcc -std=c99 -Wall -Wextra -o input_demo input_controls.c tests/input_demo.c
```

Run:

```bash
./input_demo
```

On Windows, run `input_demo.exe`.

## 4. Sanitizer Testing

GitHub Actions also compiles and executes the automated test programs with:

- **AddressSanitizer (ASan):** detects memory access errors, including out-of-bounds accesses.
- **UndefinedBehaviorSanitizer (UBSan):** detects certain forms of undefined behaviour.

The sanitizer build uses:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic \
    -g -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    tests/test_game_over.c game_engine.c score_manager.c food.c \
    -o test_game_over
```

Run the executable with:

```bash
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1 \
./test_game_over
```

The GitHub Actions workflow applies sanitizer compilation and execution to all 11 automated test programs.

The MinGW GCC compiler on Windows may not provide the same sanitizer support. Sanitizer verification is therefore performed in the Ubuntu CI environment.

## 5. Continuous Integration

The project uses GitHub Actions through:

`.github/workflows/c-build.yml`

The workflow is triggered by:
- Pushes to `main` and branches matching `SG-*`.
- Pull requests targeting `main`.

The CI workflow performs the following operations:

1. Checks out the repository.
2. Installs GCC on Ubuntu.
3. Compiles the Snake Game application.
4. Compiles and executes all 11 automated test programs.
5. Rebuilds and executes all 11 test programs with AddressSanitizer and UndefinedBehaviorSanitizer.

The workflow uses compiler warning flags:

`-Wall -Wextra -Wpedantic`

A failed compilation or test causes the workflow to fail.

## 6. Testing Limitations

- The keyboard demo requires manual interaction and is not executed by CI.
- Performance timing is not currently automated.
- Graphics mode and its platform-specific behaviour require separate verification.
- Sanitizer results depend on the CI execution environment.

Stories SG-17, SG-18, SG-20, SG-22, SG-23, SG-24, SG-25, SG-26 and SG-30 contribute to the automated test coverage documented above. The manual checks cover additional interaction scenarios.