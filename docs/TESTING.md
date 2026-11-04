# Testing

This document shows what is tested, how to run it, and how each test connects
to the requirements (SRS) and the test cases in the Test Plan.

## Automated unit tests

Build and run commands are in the [README](../README.md#run-the-tests).
Each program prints `All ... tests passed.` and exits with code 0; on a failure
it prints `FAIL: <what>` with the line number and exits with code 1.

| Test program | Jira story | Requirements | Test Plan / SRS test cases | What it checks |
|---|---|---|---|---|
| `tests/test_food.c` | SG-13 | SNK-F-008, SNK-SR-001 | TC-Food-01 | `food_generate()` alone: food stays on the board and off the snake and obstacles (10 000 draws), full board returns false, a single free cell is always chosen, bad arguments are rejected, positions are random. |
| `tests/test_food_growth.c` | SG-24 | SNK-F-008, 009, 010, 015 | TC-Food-01, TC-Food-02, TC-Food-03 | Food in the running game: first food is valid (2 000 starts), eating grows the snake by one and adds 10 points, new food appears at once on a free cell, no growth or score without eating, five foods in a row, replacement food avoids obstacles (300 meals). |
| `tests/test_collision.c` | SG-17 | SNK-F-014, SNK-SR-001 | TC-Score-01 | Self-collision: hitting a body segment is detected, the tail cell is free when the snake is not growing and blocked when it is, a corrupted length cannot read past the array, a real U-turn into the body ends the game, the final score is recorded. |
| `tests/test_game_over.c` | SG-18 | SNK-F-012, 013, 014, 017, 018 | TC-Obs-02, TC-Obs-03, TC-Score-01, TC-Session-01, TC-Session-02 | Game over on all four walls, no early game over next to a wall, obstacle hit, self-hit, final score equals the score at that moment, the game is frozen afterwards (no movement, no key input, no pause, no scoring), restart resets everything, a full board ends the game. |
| `tests/test_pause_resume.c` | SG-22 | SNK-F-019 | TC-Session-03 | Pause and resume, snake frozen while paused, direction keys ignored while paused, the snake carries on from where it stopped, score kept, pause ignored before the game starts and after game over, 1 001 rapid toggles, NULL safety. |

## Manual checks

| Check | Test case | How |
|---|---|---|
| Title menu offers console and graphics mode and rejects bad input | TC-Menu-01, TC-Menu-03 (SNK-F-001, 003) | Run the game and try `1`, `2`, `0` and an invalid entry. |
| Keyboard controls | TC-Move-02, TC-Sec-03 (SNK-F-005, SNK-SR-003) | Run `input_demo` and press arrows, W/A/S/D, Space, Enter, Q and some unmapped keys. Mapped keys print their action; others print nothing. |
| Snake moves one cell per tick; no instant reverse | TC-Move-01, TC-Move-03 (SNK-F-004, 006) | Play the game; also covered by story SG-23. |
| Rapid or invalid key presses do not crash the game | TC-Sec-01 (SNK-SR-001, 002) | Hold and mash keys for a minute while playing. |
| Tick processing time | TC-Perf-01 (SNK-NF-001) | Not automated. See DEVIATIONS.md. |

Stories SG-23, SG-25 and SG-26 cover the movement, collision/game-over and
scoring/restart tests for the whole team; the tables above list only the
automated tests that exist in this repository.

## Optional: check for memory errors

On Linux or macOS the tests can be built with the sanitizers, which report any
read or write outside an array:

```
gcc -std=c99 -g -fsanitize=address,undefined -o test_game_over tests/test_game_over.c game_engine.c score_manager.c food.c
./test_game_over
```

(The MinGW compiler on Windows usually does not include the sanitizers.)

## What was verified while writing the tests

Each test program was run with the sanitizers, and each one was shown to fail
when the matching feature was deliberately broken (for example: the tail cell
no longer excluded, the right wall off by one, pause doing nothing, no growth
when eating, final score not recorded, no score for food).
