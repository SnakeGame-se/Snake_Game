# Snake Game

A console Snake game written in C for the Software Engineering mini-project.
The snake moves around a 40 x 20 board, eats food to grow and score points,
and the game ends when it hits a wall, an obstacle or itself.

> Graphics mode is part of the original design but is **not implemented**;
> see [DEVIATIONS.md](DEVIATIONS.md).

## Features

- Console interface drawn in the terminal (flicker-free redraw)
- Keyboard control with arrow keys or W/A/S/D; reversing into the snake's own
  body is ignored
- Random food placed on a free cell; each food gives 10 points and grows the
  snake by one segment
- Eight fixed obstacles (`#`) on the board, the same in every game
- Game over on wall, obstacle or self-collision, or when the board is full
- Pause/resume, restart and quit at any time
- Score is kept for the current session only (no high-score file)

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys or W / A / S / D |
| Pause / resume | Space or P |
| Restart | R |
| Quit | Q or Esc |
| Start (title prompt) | Enter |

On the game-over screen press **R** to play again or **Q** / **Esc** to quit.

## Build and run

You need `gcc` (for example MinGW-w64 on Windows). From the repository root:

```
gcc -Wall -Wextra -o snake_game main.c game_engine.c game_interface.c score_manager.c input_controls.c food.c
.\snake_game.exe
```

Use `./snake_game` on Linux or macOS. Choose `1` (Console mode) in the menu.

Tip: the screen needs about 31 lines of height (board plus the game-over box).
Maximise the terminal window if the picture jumps or scrolls.

## Run the tests

Every test program prints `All ... tests passed.` and exits with code 0.

```
gcc -std=c99 -Wall -Wextra -o test_food          tests/test_food.c          food.c
gcc -std=c99 -Wall -Wextra -o test_food_growth   tests/test_food_growth.c   game_engine.c score_manager.c food.c
gcc -std=c99 -Wall -Wextra -o test_collision     tests/test_collision.c     game_engine.c score_manager.c food.c
gcc -std=c99 -Wall -Wextra -o test_game_over     tests/test_game_over.c     game_engine.c score_manager.c food.c
gcc -std=c99 -Wall -Wextra -o test_pause_resume  tests/test_pause_resume.c  game_engine.c score_manager.c food.c

.\test_food.exe
.\test_food_growth.exe
.\test_collision.exe
.\test_game_over.exe
.\test_pause_resume.exe
```

`tests/input_demo.c` is an interactive keyboard demo, not an automated test:

```
gcc -std=c99 -Wall -Wextra -o input_demo input_controls.c tests/input_demo.c
.\input_demo.exe
```

What each test covers, and how it maps to the requirements, is in
[docs/TESTING.md](docs/TESTING.md).

## Project layout

| File | Purpose |
|---|---|
| `main.c` | Game loop: read key, update, draw, wait one tick (120 ms) |
| `game_state.h` | Shared data: board size, snake, food, obstacles, score, status |
| `game_engine.c / .h` | Setup, direction changes, movement, growth, wall and obstacle collision |
| `collision.h` | Self-collision check (SG-17) |
| `score_manager.c / .h` | Session handling: start, score, pause/resume, game over |
| `game_interface.c / .h` | Title menu, instructions, board drawing, game-over screen |
| `input_controls.c / .h` | Non-blocking keyboard input for Windows and Linux/macOS (SG-11) |
| `food.c / .h` | Random food placement on a free cell (SG-13) |
| `tests/` | Unit tests and the keyboard demo |
| `DEVIATIONS.md` | Where the build differs from the SRS, SAD and Test Plan |
| `docs/TESTING.md` | Test coverage and traceability |

## Project documents

The requirements specification, `Snake_Game_SAD.pdf` (architecture and design)
and `Snake_Game_Test_Plan.pdf` are in the repository root.

## How we worked

- Work was planned in Jira (project key `SG`) in two sprints.
- Each story was built on its own branch named after its Jira key, for example
  `SG-17-self-collision`, and merged through a pull request that the other
  team members reviewed.
- GitHub Actions builds the C code automatically (see `.github/workflows`).

## Team

| Name | Role |
|---|---|
| Name | Name |
| Name | Name |
| Name | Name |
| Name | Name |
