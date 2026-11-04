/*
 * test_self_collision.c
 * Automated tests for SG-17 (detect self-collision).
 * Supports SRS SNK-F-014 and Test Plan case TC-Score-01.
 *
 * Build and run from the repository root:
 *   gcc -std=c99 -Wall -Wextra -o test_self_collision tests/test_self_collision.c game_engine.c score_manager.c food.c
 *   .\test_self_collision.exe        (./test_self_collision on Linux/macOS)
 *
 * Exit code 0 = all tests passed, 1 = at least one failed.
 */
#include <stdio.h>

#include "../collision.h"
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

/* A snake curled into a hook (x,y), moving RIGHT:
 *   head (5,5), (5,6), (6,6), (6,5), tail (6,4)
 * The cell right of the head, (6,5), is body segment #3 - NOT the tail. */
static void make_hook(GameState *s)
{
    engine_initialize(s);
    session_start(s);
    s->snake_length = 5;
    s->snake[0] = (Position){5, 5};
    s->snake[1] = (Position){5, 6};
    s->snake[2] = (Position){6, 6};
    s->snake[3] = (Position){6, 5};
    s->snake[4] = (Position){6, 4};
    s->direction = DIR_RIGHT;
    s->food = (Position){30, 15};
    s->obstacle_count = 0;
}

/* A 2x2 square: head (5,5), (5,6), (6,6), tail (6,5), moving RIGHT.
 * The cell right of the head is the TAIL, which moves away. */
static void make_square(GameState *s)
{
    engine_initialize(s);
    session_start(s);
    s->snake_length = 4;
    s->snake[0] = (Position){5, 5};
    s->snake[1] = (Position){5, 6};
    s->snake[2] = (Position){6, 6};
    s->snake[3] = (Position){6, 5};
    s->direction = DIR_RIGHT;
    s->food = (Position){30, 15};
    s->obstacle_count = 0;
}

static void test_function_directly(void)
{
    GameState s;

    make_hook(&s);
    CHECK(collision_hits_self(&s, (Position){6, 5}, false), "body segment #3 is a hit");
    CHECK(collision_hits_self(&s, (Position){5, 6}, false), "body segment #1 is a hit");
    CHECK(collision_hits_self(&s, (Position){6, 6}, false), "body segment #2 is a hit");
    CHECK(!collision_hits_self(&s, (Position){9, 9}, false), "empty cell is not a hit");
    CHECK(!collision_hits_self(&s, (Position){6, 4}, false), "tail cell is free when not growing");
    CHECK(collision_hits_self(&s, (Position){6, 4}, true), "tail cell is a hit when growing");
    CHECK(!collision_hits_self(NULL, (Position){6, 5}, false), "NULL state is safe");
}

static void test_length_is_clamped(void)
{
    GameState s;

    make_hook(&s);
    s.snake_length = MAX_SNAKE_LENGTH + 50; /* corrupted length */
    /* Must not read past the array (run with AddressSanitizer to prove it). */
    CHECK(!collision_hits_self(&s, (Position){1, 1}, false), "huge length does not crash");
}

static void test_engine_ends_game_on_self_collision(void)
{
    GameState s;

    make_hook(&s);
    s.score = 40;
    engine_tick(&s);
    CHECK(s.status == GAME_OVER, "moving into own body ends the game");
    CHECK(s.final_score == 40, "final score is the score at the moment of the hit");
    CHECK(s.snake[0].x == 5 && s.snake[0].y == 5, "snake does not move on the fatal tick");
}

static void test_engine_allows_following_the_tail(void)
{
    GameState s;

    make_square(&s);
    engine_tick(&s);
    CHECK(s.status == GAME_RUNNING, "moving into the vacated tail cell is allowed");
    CHECK(s.snake[0].x == 6 && s.snake[0].y == 5, "head moved into the old tail cell");
}

/* Real gameplay: grow to length 5, then curl the snake into itself. */
static void test_real_gameplay_u_turn(void)
{
    GameState s;
    int i;

    engine_initialize(&s);
    session_start(&s);
    s.obstacle_count = 0;

    for (i = 0; i < 2; i++) { /* eat two foods placed right in front of the head */
        s.food = (Position){s.snake[0].x + 1, s.snake[0].y};
        engine_tick(&s);
    }
    CHECK(s.snake_length == 5, "snake grew to length 5");
    s.food = (Position){35, 2}; /* park the food far away from the path */

    engine_set_direction(&s, 's'); engine_tick(&s);
    CHECK(s.status == GAME_RUNNING, "turn down is safe");
    engine_set_direction(&s, 'a'); engine_tick(&s);
    CHECK(s.status == GAME_RUNNING, "turn left is safe");
    engine_set_direction(&s, 'w'); engine_tick(&s);
    CHECK(s.status == GAME_OVER, "turning up into the body ends the game");
}

int main(void)
{
    test_function_directly();
    test_length_is_clamped();
    test_engine_ends_game_on_self_collision();
    test_engine_allows_following_the_tail();
    test_real_gameplay_u_turn();

    if (g_failed == 0) {
        puts("All collision tests passed.");
        return 0;
    }
    printf("%d check(s) failed.\n", g_failed);
    return 1;
}
