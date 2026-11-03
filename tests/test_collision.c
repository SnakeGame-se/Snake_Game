/*
 * test_collision.c
 * SG-25: Test collision detection and Game Over behaviour.
 *
 * Covers:
 *   - TC-Obs02 / SNK-F-012: wall collision ends the game.
 *   - TC-Obs03 / SNK-F-013: obstacle collision ends the game.
 *   - TC-Score01 / SNK-F-014: self-collision ends the game.
 *
 * Build from the repository root:
 *   gcc -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *       game_engine.c score_manager.c tests/test_collision.c \
 *       -o test_collision
 *
 * Run:
 *   ./test_collision       (test_collision.exe on Windows)
 */

#include <stdio.h>
#include <string.h>

#include "../game_engine.h"
#include "../score_manager.h"

static int failures = 0;
static int tests_run = 0;

#define CHECK(condition, message)                                      \
    do {                                                                \
        tests_run++;                                                    \
        if (condition) {                                                \
            printf("PASS: %s\n", message);                              \
        } else {                                                        \
            printf("FAIL: %s (line %d)\n", message, __LINE__);          \
            failures++;                                                 \
        }                                                               \
    } while (0)

static void prepare_state(GameState *state)
{
    memset(state, 0, sizeof(*state));
    state->status = GAME_RUNNING;
    state->score = 30;
    state->final_score = 0;
    state->snake_length = 3;
    state->snake[0] = (Position){10, 10};
    state->snake[1] = (Position){9, 10};
    state->snake[2] = (Position){8, 10};
    state->direction = DIR_RIGHT;
    state->food = (Position){30, 15};
}

/* Check all four board edges, not just the common right-edge case. */
static void test_wall_collisions(void)
{
    GameState state;

    prepare_state(&state);
    state.snake[0] = (Position){0, 10};
    state.direction = DIR_LEFT;
    engine_tick(&state);
    CHECK(state.status == GAME_OVER,
          "Moving beyond the left wall triggers Game Over");
    CHECK(state.final_score == 30,
          "Wall collision preserves the final score");

    prepare_state(&state);
    state.snake[0] = (Position){BOARD_WIDTH - 1, 10};
    state.direction = DIR_RIGHT;
    engine_tick(&state);
    CHECK(state.status == GAME_OVER,
          "Moving beyond the right wall triggers Game Over");

    prepare_state(&state);
    state.snake[0] = (Position){10, 0};
    state.direction = DIR_UP;
    engine_tick(&state);
    CHECK(state.status == GAME_OVER,
          "Moving beyond the top wall triggers Game Over");

    prepare_state(&state);
    state.snake[0] = (Position){10, BOARD_HEIGHT - 1};
    state.direction = DIR_DOWN;
    engine_tick(&state);
    CHECK(state.status == GAME_OVER,
          "Moving beyond the bottom wall triggers Game Over");
}

static void test_obstacle_collision(void)
{
    GameState state;

    prepare_state(&state);
    state.obstacle_count = 1;
    state.obstacles[0] = (Position){11, 10};

    engine_tick(&state);

    CHECK(state.status == GAME_OVER,
          "Moving into an obstacle triggers Game Over");
    CHECK(state.final_score == 30,
          "Obstacle collision preserves the final score");
    CHECK(state.snake[0].x == 10 && state.snake[0].y == 10,
          "Snake does not move into the obstacle cell");
}

static void test_self_collision(void)
{
    GameState state;

    prepare_state(&state);
    state.snake_length = 4;
    state.snake[0] = (Position){5, 5};
    state.snake[1] = (Position){5, 4};
    state.snake[2] = (Position){4, 4};
    state.snake[3] = (Position){4, 5};
    state.direction = DIR_UP;
    state.food = (Position){20, 10};

    engine_tick(&state);

    CHECK(state.status == GAME_OVER,
          "Moving into the snake's own body triggers Game Over");
    CHECK(state.final_score == 30,
          "Self-collision preserves the final score");
    CHECK(state.snake[0].x == 5 && state.snake[0].y == 5,
          "Snake head position remains unchanged after self-collision");
}

static void test_game_over_stops_future_ticks(void)
{
    GameState state;
    Position original_head;

    prepare_state(&state);
    state.snake[0] = (Position){BOARD_WIDTH - 1, 10};
    state.direction = DIR_RIGHT;
    original_head = state.snake[0];

    engine_tick(&state);
    CHECK(state.status == GAME_OVER,
          "Collision changes session status to GAME_OVER");

    engine_tick(&state);
    CHECK(state.snake[0].x == original_head.x &&
          state.snake[0].y == original_head.y,
          "Further ticks do not move the snake after Game Over");
    CHECK(state.final_score == 30,
          "Further ticks do not alter the saved final score");
}

int main(void)
{
    test_wall_collisions();
    test_obstacle_collision();
    test_self_collision();
    test_game_over_stops_future_ticks();

    printf("\nSG-25 collision/Game Over tests: %d checks, %d failure(s).\n",
           tests_run, failures);

    if (failures == 0) {
        puts("ALL SG-25 TESTS PASSED.");
        return 0;
    }

    return 1;
}
