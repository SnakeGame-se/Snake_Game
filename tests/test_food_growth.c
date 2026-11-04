/*
 * test_food_growth.c
 * Automated tests for SG-24 (test food generation and growth).
 * Supports SRS SNK-F-008, SNK-F-009, SNK-F-010, SNK-F-015 and Test Plan
 * cases TC-Food-01, TC-Food-02, TC-Food-03.
 *
 * These tests run the real game engine. (tests/test_food.c from SG-13 tests
 * the food_generate() function on its own.)
 *
 * Build and run from the repository root:
 *   gcc -std=c99 -Wall -Wextra -o test_food_growth tests/test_food_growth.c game_engine.c score_manager.c food.c
 *   .\test_food_growth.exe        (./test_food_growth on Linux/macOS)
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

static int on_snake(const GameState *s, Position p)
{
    size_t i;
    for (i = 0; i < s->snake_length; i++) {
        if (s->snake[i].x == p.x && s->snake[i].y == p.y) {
            return 1;
        }
    }
    return 0;
}

static int on_obstacle(const GameState *s, Position p)
{
    size_t i;
    for (i = 0; i < s->obstacle_count; i++) {
        if (s->obstacles[i].x == p.x && s->obstacles[i].y == p.y) {
            return 1;
        }
    }
    return 0;
}

static int food_is_valid(const GameState *s)
{
    return s->food.x >= 0 && s->food.x < BOARD_WIDTH &&
           s->food.y >= 0 && s->food.y < BOARD_HEIGHT &&
           !on_snake(s, s->food) && !on_obstacle(s, s->food);
}

/* TC-Food-01: the first food is on the board and not on the snake. */
static void test_initial_food_is_valid(void)
{
    GameState s;
    int i, ok = 1;

    for (i = 0; i < 2000; i++) {
        engine_initialize(&s);
        if (!food_is_valid(&s)) {
            ok = 0;
            break;
        }
    }
    CHECK(ok, "initial food is on the board and not on the snake (2000 starts)");
}

/* TC-Food-02: eating food grows the snake by one and scores. */
static void test_eating_grows_and_scores(void)
{
    GameState s;
    Position target;

    engine_initialize(&s);
    session_start(&s);
    s.obstacle_count = 0;
    target = (Position){s.snake[0].x + 1, s.snake[0].y};
    s.food = target;

    engine_tick(&s);
    CHECK(s.snake_length == 4, "snake grew from 3 to 4 segments");
    CHECK(s.score == SCORE_PER_FOOD, "score went up by SCORE_PER_FOOD");
    CHECK(s.snake[0].x == target.x && s.snake[0].y == target.y, "head is on the eaten food's cell");
    CHECK(s.status == GAME_RUNNING, "game keeps running after eating");
}

/* TC-Food-03: new food appears straight away, on a free cell. */
static void test_new_food_after_eating(void)
{
    GameState s;
    Position eaten;

    engine_initialize(&s);
    session_start(&s);
    s.obstacle_count = 0;
    eaten = (Position){s.snake[0].x + 1, s.snake[0].y};
    s.food = eaten;

    engine_tick(&s);
    CHECK(food_is_valid(&s), "new food is on the board and not on the snake");
    CHECK(!(s.food.x == s.snake[0].x && s.food.y == s.snake[0].y), "new food is not under the head");
}

static void test_moving_without_food_does_not_grow(void)
{
    GameState s;
    int i;

    engine_initialize(&s);
    session_start(&s);
    s.obstacle_count = 0;
    s.food = (Position){35, 2};

    for (i = 0; i < 5; i++) {
        engine_tick(&s);
    }
    CHECK(s.snake_length == 3, "snake length is unchanged when no food is eaten");
    CHECK(s.score == 0, "score is unchanged when no food is eaten");
}

static void test_eating_five_in_a_row(void)
{
    GameState s;
    int i, all_valid = 1;

    engine_initialize(&s);
    session_start(&s);
    s.obstacle_count = 0;

    for (i = 0; i < 5; i++) {
        s.food = (Position){s.snake[0].x + 1, s.snake[0].y};
        engine_tick(&s);
        if (!food_is_valid(&s)) {
            all_valid = 0;
        }
    }
    CHECK(s.snake_length == 8, "five foods grew the snake from 3 to 8");
    CHECK(s.score == 5 * SCORE_PER_FOOD, "five foods scored 50");
    CHECK(all_valid, "every replacement food was on a free cell");
}

/* New food must also avoid obstacles. */
static void test_food_avoids_obstacles(void)
{
    GameState s;
    int i, k, ok = 1;

    engine_initialize(&s);
    session_start(&s);
    s.obstacle_count = 0;
    for (k = 0; k < 8; k++) { /* 16 obstacles (limit is MAX_OBSTACLES), away from row 10 */
        s.obstacles[s.obstacle_count++] = (Position){k * 4, 2};
        s.obstacles[s.obstacle_count++] = (Position){k * 4 + 1, 17};
    }

    for (i = 0; i < 300; i++) {
        /* Reset to a short straight snake, with food right in front of it. */
        s.snake_length = 3;
        s.snake[0] = (Position){10, 10};
        s.snake[1] = (Position){9, 10};
        s.snake[2] = (Position){8, 10};
        s.direction = DIR_RIGHT;
        s.status = GAME_RUNNING;
        s.food = (Position){11, 10};

        engine_tick(&s); /* eats the food; the engine places a new one */
        if (s.snake_length != 4 || !food_is_valid(&s)) {
            ok = 0;
            break;
        }
    }
    CHECK(ok, "replacement food never lands on an obstacle or the snake (300 meals)");
}

int main(void)
{
    test_initial_food_is_valid();
    test_eating_grows_and_scores();
    test_new_food_after_eating();
    test_moving_without_food_does_not_grow();
    test_eating_five_in_a_row();
    test_food_avoids_obstacles();

    if (g_failed == 0) {
        puts("All food-and-growth tests passed.");
        return 0;
    }
    printf("%d check(s) failed.\n", g_failed);
    return 1;
}
