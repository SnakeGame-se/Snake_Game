# Snake Game

A Snake game written in C for the Software Engineering mini-project.

The game runs on a 40 × 20 board. Players control a snake, collect food, grow the snake and earn points. The game ends when the snake collides with a wall, an obstacle or itself, or when the board is full.

The project supports a terminal-based console interface on Windows, Linux and macOS, along with a Windows-only graphical interface using the Windows API (GDI).

## Features

- Console interface with in-place terminal redraw.
- Windows graphical interface using the Windows API.
- Keyboard controls using arrow keys or W/A/S/D.
- Prevention of direct reverse movement into the snake's body.
- Random food placement on valid free cells.
- Each food item increases the score by 10 points and grows the snake by one segment.
- Eight obstacles in the game board.
- Collision detection for walls, obstacles and the snake's own body.
- Pause, resume, restart and quit functionality.
- Game-over handling and session score management.

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys or W / A / S / D |
| Pause / resume | Space or P |
| Restart | R |
| Quit | Q or Esc |
| Start | Enter |

On the game-over screen, press R to restart or Q/Esc to quit.

## Build and Run

You need GCC, a running MySQL Server, and the MySQL C development connector to compile the database-enabled project. Configure the connector include/library paths for your platform.

### Windows — Console and Graphics Modes

Using MinGW GCC, run this command from the repository root:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic main.c game_engine.c game_interface.c graphics_interface.c input_controls.c score_manager.c food.c database.c -I"%MYSQL_INCLUDE%" -L"%MYSQL_LIB%" -lmysql -lgdi32 -o snake_game.exe
```

Run:

```powershell
.\snake_game.exe
```

Choose console mode or graphics mode from the title menu. The game asks for a username and requires a reachable MySQL database before starting. Set the `SNAKE_DB_HOST`, `SNAKE_DB_PORT`, `SNAKE_DB_NAME`, `SNAKE_DB_USER`, and `SNAKE_DB_PASSWORD` environment variables as described in `DATABASE_SETUP.md`.

### Linux and macOS — Console Mode

Run:

```bash
gcc -D_POSIX_C_SOURCE=200809L -std=c11 -Wall -Wextra -Wpedantic main.c game_engine.c game_interface.c input_controls.c score_manager.c food.c database.c $(mysql_config --cflags --libs) -o snake_game
```

Start the application:

```bash
./snake_game
```

Graphics mode is available only on Windows.

## Automated Tests

The project contains 12 independent automated test programs in the `tests/` directory.

The test programs cover:

- Food generation and placement.
- Food collection and snake growth.
- Wall, obstacle and self-collision.
- Game-over behaviour.
- Pause and resume.
- Restart and session management.
- Score calculation and display.
- Snake movement and direction validation.
- Game engine initialization.
- Keyboard character-to-action mapping.

Each test program returns exit code 0 when its checks pass and exit code 1 when a check fails.

For complete compilation commands, execution instructions, test coverage and requirement traceability, see [docs/TESTING.md](docs/TESTING.md).

## Manual Keyboard Demo

The interactive keyboard demo is located at `tests/input_demo.c`. It is a manual testing utility and is not included in the automated test suite.

Compile:

```bash
gcc -std=c99 -Wall -Wextra -o input_demo input_controls.c tests/input_demo.c
```

Run on Windows:

```powershell
.\input_demo.exe
```

Run on Linux/macOS:

```bash
./input_demo
```

## Project Structure

| File / Directory | Purpose |
|---|---|
| `main.c` | Application entry point and game loop |
| `game_state.h` | Shared game state and board definitions |
| `game_engine.c / .h` | Initialization, movement, direction handling and game logic |
| `collision.h` | Self-collision detection |
| `score_manager.c / .h` | Score and session management |
| `game_interface.c / .h` | Console menu, input, rendering and game-over display |
| `graphics_interface.c / .h` | Windows graphical interface |
| `input_controls.c / .h` | Keyboard input handling |
| `food.c / .h` | Food generation and placement |
| `tests/` | Automated tests and interactive keyboard demo |
| `docs/TESTING.md` | Test coverage and traceability |
| `DEVIATIONS.md` | Differences from the submitted SRS, SAD and Test Plan |

## Project Documents

The repository includes:

- Software Requirements Specification (SRS).
- `Snake_Game_SAD.pdf` — Software Architecture and Design.
- `Snake_Game_Test_Plan.pdf` — Test Plan.
- `DEVIATIONS.md` — Documented implementation differences.
- `docs/TESTING.md` — Testing instructions and traceability.

## Continuous Integration

GitHub Actions is configured in `.github/workflows/c-build.yml`.

The workflow:

1. Builds the console application on Ubuntu.
2. Compiles and executes all 12 automated test programs.
3. Executes the automated tests with AddressSanitizer and UndefinedBehaviorSanitizer.

The workflow runs on pushes to `main` and `SG-*` branches and on pull requests targeting `main`.

## How We Worked

- Work was organized using Jira under project key `SG`.
- Development tasks were implemented on separate feature branches.
- Changes were committed with Jira story references.
- Pull requests were used to review and merge completed work.
- GitHub Actions was used for automated build and test verification.

## Team

| Name |
|---|
| Pradyumna Panigrahy |
| Sai Sudeshna |
| Noel Jose |
