/*
 * test_game_over.c
 * Automated tests for SG-18 (implement Game Over condition).
 * Supports SRS SNK-F-012, SNK-F-013, SNK-F-014, SNK-F-017, SNK-F-018 and
 * Test Plan cases TC-Obs-02, TC-Obs-03, TC-Score-01, TC-Session-02.
 *
 * Build and run from the repository root:
 *   gcc -std=c99 -Wall -Wextra -o test_game_over tests/test_game_over.c game_engine.c score_manager.c food.c
 *   .\test_game_over.exe        (./test_game_over on Linux/macOS)
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

/* A running game with a straight 3-segment snake whose head is at (hx,hy)
 * and which moves in direction dir. Food is parked far from every test. */
static void place(GameState *s, int hx, int hy, Direction dir)
{
    int dx = 0, dy = 0;

    engine_initialize(s);
    session_start(s);

    switch (dir) {
        case DIR_UP:    dy = -1; break;
        case DIR_DOWN:  dy = 1;  break;
        case DIR_LEFT:  dx = -1; break;
        case DIR_RIGHT: dx = 1;  break;
    }
    s->snake_length = 3;
    s->snake[0] = (Position){hx, hy};
    s->snake[1] = (Position){hx - dx, hy - dy};
    s->snake[2] = (Position){hx - 2 * dx, hy - 2 * dy};
    s->direction = dir;
    s->food = (Position){BOARD_WIDTH / 2, BOARD_HEIGHT / 2 + 5};
    s->obstacle_count = 0;
}

static void test_wall_collision_all_four_sides(void)
{
    GameState s;

    place(&s, BOARD_WIDTH - 1, 10, DIR_RIGHT); engine_tick(&s);
    CHECK(s.status == GAME_OVER, "right wall ends the game");
    place(&s, 0, 10, DIR_LEFT); engine_tick(&s);
    CHECK(s.status == GAME_OVER, "left wall ends the game");
    place(&s, 20, 0, DIR_UP); engine_tick(&s);
    CHECK(s.status == GAME_OVER, "top wall ends the game");
    place(&s, 20, BOARD_HEIGHT - 1, DIR_DOWN); engine_tick(&s);
    CHECK(s.status == GAME_OVER, "bottom wall ends the game");
}

static void test_no_early_game_over_next_to_wall(void)
{
    GameState s;

    place(&s, BOARD_WIDTH - 2, 10, DIR_RIGHT);
    engine_tick(&s);
    CHECK(s.status == GAME_RUNNING, "one cell from the wall is still safe");
    CHECK(s.snake[0].x == BOARD_WIDTH - 1, "head reached the last column");
    engine_tick(&s);
    CHECK(s.status == GAME_OVER, "the next move into the wall ends the game");
}

static void test_obstacle_collision(void)
{
    GameState s;

    place(&s, 20, 10, DIR_RIGHT);
    s.obstacles[0] = (Position){21, 10};
    s.obstacle_count = 1;
    engine_tick(&s);
    CHECK(s.status == GAME_OVER, "moving into an obstacle ends the game");

    place(&s, 20, 10, DIR_RIGHT);
    s.obstacles[0] = (Position){21, 12};
    s.obstacle_count = 1;
    engine_tick(&s);
    CHECK(s.status == GAME_RUNNING, "an obstacle elsewhere does not end the game");
}

static void test_self_collision(void)
{
    GameState s;

    place(&s, 5, 5, DIR_RIGHT);
    s.snake_length = 5;
    s.snake[0] = (Position){5, 5};
    s.snake[1] = (Position){5, 6};
    s.snake[2] = (Position){6, 6};
    s.snake[3] = (Position){6, 5};
    s.snake[4] = (Position){6, 4};
    engine_tick(&s);
    CHECK(s.status == GAME_OVER, "hitting its own body ends the game");
}

static void test_final_score(void)
{
    GameState s;

    place(&s, BOARD_WIDTH - 1, 10, DIR_RIGHT);
    s.score = 40;
    engine_tick(&s);
    CHECK(s.status == GAME_OVER, "game is over");
    CHECK(s.final_score == 40, "final score equals the score when the game ended");
}

static void test_game_is_frozen_after_game_over(void)
{
    GameState s;
    Position head_before;
    int score_before;
    int i;

    place(&s, BOARD_WIDTH - 1, 10, DIR_RIGHT);
    s.score = 20;
    engine_tick(&s);
    head_before = s.snake[0];
    score_before = s.score;

    for (i = 0; i < 3; i++) {
        engine_tick(&s);
    }
    CHECK(s.snake[0].x == head_before.x && s.snake[0].y == head_before.y,
          "snake does not move after game over");

    engine_set_direction(&s, 'w');
    CHECK(s.direction == DIR_RIGHT, "direction keys are ignored after game over");

    session_toggle_pause(&s);
    CHECK(s.status == GAME_OVER, "pause does nothing after game over");

    session_add_food_score(&s);
    CHECK(s.score == score_before, "score cannot change after game over");
}

static void test_restart_resets_everything(void)
{
    GameState s;

    place(&s, BOARD_WIDTH - 1, 10, DIR_RIGHT);
    s.score = 70;
    engine_tick(&s);
    CHECK(s.status == GAME_OVER, "game over before restart");

    engine_initialize(&s); /* exactly what main.c does when the player presses R */
    session_start(&s);
    CHECK(s.status == GAME_RUNNING, "restart starts a running game");
    CHECK(s.score == 0 && s.final_score == 0, "score and final score are reset");
    CHECK(s.snake_length == 3, "snake is back to its starting length");
    CHECK(s.snake[0].x == BOARD_WIDTH / 2 && s.snake[0].y == BOARD_HEIGHT / 2,
          "snake is back at the centre of the board");
}

/* A snake filling every cell but one, eating the last food: the board is
 * full, so there is nowhere to place new food and the game must end. */
static void test_full_board_ends_the_game(void)
{
    GameState s;
    Position path[BOARD_WIDTH * BOARD_HEIGHT];
    int i;

    engine_initialize(&s);
    session_start(&s);
    s.obstacle_count = 0;

    /* Serpentine path through every cell: row 0 left-to-right, row 1 right-
     * to-left, and so on. path[0] is the single free cell. */
    for (i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; i++) {
        int y = i / BOARD_WIDTH;
        int xi = i % BOARD_WIDTH;
        path[i].x = (y % 2 == 0) ? xi : BOARD_WIDTH - 1 - xi;
        path[i].y = y;
    }
    s.snake_length = MAX_SNAKE_LENGTH - 1;
    for (i = 0; i < (int)s.snake_length; i++) {
        s.snake[i] = path[i + 1]; /* head = path[1], tail = last cell */
    }
    s.direction = DIR_LEFT;       /* head (1,0) moves left into path[0] = (0,0) */
    s.food = path[0];

    engine_tick(&s);
    CHECK(s.snake_length == MAX_SNAKE_LENGTH, "snake grew to fill the board");
    CHECK(s.score == SCORE_PER_FOOD, "the last food was scored");
    CHECK(s.status == GAME_OVER, "a full board ends the game");
    CHECK(s.final_score == SCORE_PER_FOOD, "final score is recorded");
}

int main(void)
{
    test_wall_collision_all_four_sides();
    test_no_early_game_over_next_to_wall();
    test_obstacle_collision();
    test_self_collision();
    test_final_score();
    test_game_is_frozen_after_game_over();
    test_restart_resets_everything();
    test_full_board_ends_the_game();

    if (g_failed == 0) {
        puts("All game-over tests passed.");
        return 0;
    }
    printf("%d check(s) failed.\n", g_failed);
    return 1;
}
