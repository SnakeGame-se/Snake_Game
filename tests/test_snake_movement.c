
/*
 * SG-23: Test snake movement and controls.
 * Tests the movement/control functions used by the current game engine.
 *
 * Build from repository root:
 * gcc -std=c11 -Wall -Wextra -Wpedantic -Werror \
 *     game_engine.c score_manager.c tests/test_snake_movement.c \
 *     -o test_snake_movement
 */

#include <stdio.h>
#include <string.h>

#include "../game_engine.h"
#include "../score_manager.h"

static int checks = 0;
static int failures = 0;

#define CHECK(condition, description) do { \
    ++checks; \
    if (condition) printf("PASS: %s\n", description); \
    else { printf("FAIL: %s (line %d)\n", description, __LINE__); ++failures; } \
} while (0)

static void setup(GameState *s)
{
    memset(s, 0, sizeof(*s));
    s->status = GAME_RUNNING;
    s->snake_length = 3;
    s->snake[0] = (Position){10, 10};
    s->snake[1] = (Position){9, 10};
    s->snake[2] = (Position){8, 10};
    s->direction = DIR_RIGHT;
    s->food = (Position){30, 15};
}

static void test_forward_movement(void)
{
    GameState s;

    setup(&s);
    engine_tick(&s);

    CHECK(s.snake[0].x == 11 && s.snake[0].y == 10,
          "Right direction moves head exactly one cell right");
    CHECK(s.snake_length == 3,
          "Normal movement keeps snake length unchanged");

    setup(&s);
    s.direction = DIR_UP;
    engine_tick(&s);

    CHECK(s.snake[0].x == 10 && s.snake[0].y == 9,
          "Up direction moves head exactly one cell up");

    setup(&s);
    s.direction = DIR_DOWN;
    engine_tick(&s);

    CHECK(s.snake[0].x == 10 && s.snake[0].y == 11,
          "Down direction moves head exactly one cell down");

    setup(&s);
    s.direction = DIR_LEFT;
    s.snake[0] = (Position){10, 10};
    s.snake[1] = (Position){10, 9};
    s.snake[2] = (Position){10, 8};
    engine_tick(&s);

    CHECK(s.snake[0].x == 9 && s.snake[0].y == 10,
          "Left direction moves head exactly one cell left");
}

static void test_direction_controls(void)
{
    GameState s;

    setup(&s);
    engine_set_direction(&s, 'w');

    CHECK(s.direction == DIR_UP, "W changes direction to up");

    engine_tick(&s);

    CHECK(s.snake[0].x == 10 && s.snake[0].y == 9,
          "W direction is applied on the next game tick");

    setup(&s);
    engine_set_direction(&s, 's');

    CHECK(s.direction == DIR_DOWN, "S changes direction to down");

    setup(&s);
    s.direction = DIR_UP;
    engine_set_direction(&s, 'a');

    CHECK(s.direction == DIR_LEFT, "A changes direction to left from up");

    setup(&s);
    engine_set_direction(&s, 'd');

    CHECK(s.direction == DIR_RIGHT, "D keeps/sets direction to right");
}

static void test_reverse_and_invalid_keys(void)
{
    GameState s;

    setup(&s);
    engine_set_direction(&s, 'a');

    CHECK(s.direction == DIR_RIGHT,
          "A direct reverse from right to left is rejected");

    setup(&s);
    s.direction = DIR_UP;
    engine_set_direction(&s, 's');

    CHECK(s.direction == DIR_UP,
          "A direct reverse from up to down is rejected");

    setup(&s);
    engine_set_direction(&s, 'x');

    CHECK(s.direction == DIR_RIGHT,
          "Unmapped key does not change direction");

    setup(&s);
    engine_set_direction(&s, '1');

    CHECK(s.direction == DIR_RIGHT,
          "Numeric/invalid key does not change direction");

    setup(&s);
    s.direction = (Direction)99;
    engine_set_direction(&s, 'w');

    CHECK(s.direction == (Direction)99,
          "Invalid current direction is rejected safely");
}

static void test_controls_when_not_running(void)
{
    GameState s;

    setup(&s);
    s.status = GAME_PAUSED;
    engine_set_direction(&s, 'w');

    CHECK(s.direction == DIR_RIGHT,
          "Direction input is ignored while game is paused");

    engine_tick(&s);

    CHECK(s.snake[0].x == 10 && s.snake[0].y == 10,
          "Snake does not move while game is paused");

    s.status = GAME_OVER;
    engine_set_direction(&s, 'w');

    CHECK(s.direction == DIR_RIGHT,
          "Direction input is ignored after Game Over");
}

/* SG-30: Test that an invalid direction prevents movement. */
static void test_invalid_direction_stops_movement(void)
{
    GameState s;

    setup(&s);
    s.direction = (Direction)99;

    engine_tick(&s);

    CHECK(s.snake[0].x == 10 && s.snake[0].y == 10,
          "Invalid direction does not move the snake");

    CHECK(s.status == GAME_RUNNING,
          "Invalid direction does not end the game");
}

/* SG-30: Test that an invalid obstacle count prevents movement. */
static void test_invalid_obstacle_count_stops_movement(void)
{
    GameState s;

    setup(&s);
    s.obstacle_count = MAX_OBSTACLES + 1;

    engine_tick(&s);

    CHECK(s.snake[0].x == 10 && s.snake[0].y == 10,
          "Invalid obstacle count does not move the snake");

    CHECK(s.status == GAME_RUNNING,
          "Invalid obstacle count does not end the game");
}

int main(void)
{
    test_forward_movement();
    test_direction_controls();
    test_reverse_and_invalid_keys();
    test_controls_when_not_running();
    test_invalid_direction_stops_movement();
    test_invalid_obstacle_count_stops_movement();

    printf("\nSG-30: %d checks, %d failure(s).\n", checks, failures);

    if (failures == 0) {
        puts("ALL AUTOMATED TESTS PASSED.");
        return 0;
    }

    return 1;
}
