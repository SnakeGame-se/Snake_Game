/*
 * test_pause_resume.c
 * Automated tests for SG-22 (implement pause/resume functionality).
 * Supports SRS SNK-F-019.
 *
 * Build and run from the repository root:
 *   gcc -std=c99 -Wall -Wextra -o test_pause_resume tests/test_pause_resume.c game_engine.c score_manager.c food.c
 *   .\test_pause_resume.exe        (./test_pause_resume on Linux/macOS)
 *
 * Exit code 0 = all tests passed, 1 = at least one failed.
 */
#include <stdio.h>

#include "../game_engine.h"
#include "../score_manager.h"

static int g_failed = 0;

#define CHECK(cond, msg)                                            \
    do {                                                            \
        if (!(cond)) {                                              \
            printf("FAIL: %s  (line %d)\n", (msg), __LINE__);       \
            g_failed++;                                             \
        }                                                           \
    } while (0)

static void running_game(GameState *s)
{
    engine_initialize(s);
    session_start(s);
    s->obstacle_count = 0;
    s->food = (Position){35, 2}; /* far from the snake's path */
}

static void test_pause_and_resume(void)
{
    GameState s;

    running_game(&s);
    session_toggle_pause(&s);
    CHECK(s.status == GAME_PAUSED, "P pauses a running game");
    session_toggle_pause(&s);
    CHECK(s.status == GAME_RUNNING, "P again resumes it");
}

static void test_explicit_pause_and_resume_functions(void)
{
    GameState s;

    running_game(&s);
    session_pause(&s);
    CHECK(s.status == GAME_PAUSED, "session_pause pauses");
    session_resume(&s);
    CHECK(s.status == GAME_RUNNING, "session_resume resumes");
}

static void test_snake_frozen_while_paused(void)
{
    GameState s;
    Position head_before;
    size_t length_before;
    int i;

    running_game(&s);
    engine_tick(&s); /* move once so we are not testing the start position */
    session_toggle_pause(&s);
    head_before = s.snake[0];
    length_before = s.snake_length;

    for (i = 0; i < 5; i++) {
        engine_tick(&s);
    }
    CHECK(s.snake[0].x == head_before.x && s.snake[0].y == head_before.y,
          "snake does not move while paused");
    CHECK(s.snake_length == length_before, "snake length does not change while paused");
}

static void test_direction_keys_ignored_while_paused(void)
{
    GameState s;

    running_game(&s);
    session_toggle_pause(&s);
    engine_set_direction(&s, 'w');
    CHECK(s.direction == DIR_RIGHT, "direction cannot be changed while paused");
}

static void test_game_continues_from_same_place(void)
{
    GameState s;
    int x_before;

    running_game(&s);
    engine_tick(&s);
    x_before = s.snake[0].x;
    session_toggle_pause(&s);
    session_toggle_pause(&s);
    CHECK(s.snake[0].x == x_before, "resuming does not move the snake by itself");
    engine_tick(&s);
    CHECK(s.snake[0].x == x_before + 1, "after resume the snake moves on from where it stopped");
}

static void test_score_survives_pause(void)
{
    GameState s;

    running_game(&s);
    s.score = 30;
    session_toggle_pause(&s);
    session_toggle_pause(&s);
    CHECK(s.score == 30, "score is unchanged by pause and resume");
}

static void test_pause_ignored_outside_a_running_game(void)
{
    GameState s;

    engine_initialize(&s); /* status READY, game not started */
    session_pause(&s);
    CHECK(s.status == GAME_READY, "cannot pause a game that has not started");
    session_toggle_pause(&s);
    CHECK(s.status == GAME_READY, "toggle does nothing before the game starts");

    running_game(&s);
    session_end(&s);
    session_pause(&s);
    CHECK(s.status == GAME_OVER, "cannot pause after game over");
    session_toggle_pause(&s);
    CHECK(s.status == GAME_OVER, "toggle does nothing after game over");
}

static void test_resume_ignored_when_not_paused(void)
{
    GameState s;

    running_game(&s);
    session_resume(&s);
    CHECK(s.status == GAME_RUNNING, "resume on a running game changes nothing");
}

static void test_rapid_toggling(void)
{
    GameState s;
    int i;

    running_game(&s);
    for (i = 0; i < 1001; i++) {
        session_toggle_pause(&s);
    }
    CHECK(s.status == GAME_PAUSED, "an odd number of toggles leaves the game paused");
    session_toggle_pause(&s);
    CHECK(s.status == GAME_RUNNING, "one more toggle resumes it");
}

static void test_null_is_safe(void)
{
    session_pause(NULL);
    session_resume(NULL);
    session_toggle_pause(NULL);
    CHECK(1, "NULL state does not crash");
}

int main(void)
{
    test_pause_and_resume();
    test_explicit_pause_and_resume_functions();
    test_snake_frozen_while_paused();
    test_direction_keys_ignored_while_paused();
    test_game_continues_from_same_place();
    test_score_survives_pause();
    test_pause_ignored_outside_a_running_game();
    test_resume_ignored_when_not_paused();
    test_rapid_toggling();
    test_null_is_safe();

    if (g_failed == 0) {
        puts("All pause/resume tests passed.");
        return 0;
    }
    printf("%d check(s) failed.\n", g_failed);
    return 1;
}
