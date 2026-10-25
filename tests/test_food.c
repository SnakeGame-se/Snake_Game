/*
 * test_food.c
 * Automated tests for SG-13 (random food generation).
 * Supports Test Plan case TC-Food-01 (SNK-F-008): food never spawns on the
 * snake body or an obstacle.
 *
 * Build and run from the repository root:
 *   gcc -std=c99 -Wall -Wextra -o test_food food.c tests/test_food.c
 *   ./test_food          (test_food.exe on Windows)
 *
 * Exit code 0 = all tests passed, 1 = at least one failed.
 */
#include <stdio.h>

#include "../food.h"

static int g_failed = 0;

#define CHECK(cond, msg)                                            \
    do {                                                            \
        if (!(cond)) {                                              \
            printf("FAIL: %s  (line %d)\n", (msg), __LINE__);       \
            g_failed++;                                             \
        }                                                           \
    } while (0)

static int in_list(FoodCell c, const FoodCell *list, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (list[i].x == c.x && list[i].y == c.y) {
            return 1;
        }
    }
    return 0;
}

/* 1. Food is always inside the grid and never on the snake or an obstacle. */
static void test_never_on_snake_or_obstacle(void)
{
    const FoodCell snake[] = { {5, 4}, {4, 4}, {3, 4}, {2, 4}, {2, 5} };
    const FoodCell obst[]  = { {0, 0}, {9, 7}, {5, 2}, {6, 2}, {7, 2} };
    int i, ok = 1;

    for (i = 0; i < 10000; i++) {
        FoodCell f;
        if (!food_generate(&f, 10, 8, snake, 5, obst, 5)) { ok = 0; break; }
        if (f.x < 0 || f.x >= 10 || f.y < 0 || f.y >= 8) { ok = 0; break; }
        if (in_list(f, snake, 5) || in_list(f, obst, 5)) { ok = 0; break; }
    }
    CHECK(ok, "food stays inside grid and off snake/obstacles (10000 runs)");
}

/* 2. Returns false when every cell is taken (board full). */
static void test_board_full(void)
{
    const FoodCell snake[] = { {0,0},{1,0},{2,0},{0,1},{1,1} };
    const FoodCell obst[]  = { {2,1},{0,2},{1,2},{2,2} };
    FoodCell f = { -1, -1 };

    CHECK(!food_generate(&f, 3, 3, snake, 5, obst, 4), "full board returns false");
    CHECK(f.x == -1 && f.y == -1, "output untouched when board is full");
}

/* 3. With exactly one free cell, that cell is always chosen. */
static void test_single_free_cell(void)
{
    const FoodCell snake[] = { {0,0},{1,0},{2,0},{0,1},{1,1} };
    const FoodCell obst[]  = { {0,2},{1,2},{2,2} };   /* only (2,1) is free */
    int i, ok = 1;

    for (i = 0; i < 100; i++) {
        FoodCell f;
        if (!food_generate(&f, 3, 3, snake, 5, obst, 3) || f.x != 2 || f.y != 1) {
            ok = 0;
            break;
        }
    }
    CHECK(ok, "single free cell is always picked");
}

/* 4. Bad arguments are rejected instead of crashing. */
static void test_bad_arguments(void)
{
    FoodCell f;
    const FoodCell one[] = { {0, 0} };

    CHECK(!food_generate(NULL, 5, 5, NULL, 0, NULL, 0), "NULL out rejected");
    CHECK(!food_generate(&f, 0, 5, NULL, 0, NULL, 0), "zero width rejected");
    CHECK(!food_generate(&f, 5, -1, NULL, 0, NULL, 0), "negative height rejected");
    CHECK(!food_generate(&f, 5, 5, NULL, 3, NULL, 0), "NULL snake with count>0 rejected");
    CHECK(!food_generate(&f, 5, 5, one, 1, NULL, 2), "NULL obstacles with count>0 rejected");
    CHECK(!food_generate(&f, 5, 5, one, -1, NULL, 0), "negative snake_len rejected");
}

/* 5. Food really is random: many different cells show up on an empty grid. */
static void test_randomness(void)
{
    int seen[100] = { 0 };
    int i, distinct = 0;

    for (i = 0; i < 1000; i++) {
        FoodCell f;
        if (food_generate(&f, 10, 10, NULL, 0, NULL, 0)) {
            seen[f.y * 10 + f.x] = 1;
        }
    }
    for (i = 0; i < 100; i++) {
        distinct += seen[i];
    }
    CHECK(distinct >= 90, "1000 draws on a 10x10 grid cover at least 90 cells");
}

int main(void)
{
    food_seed(12345); /* fixed seed: same results on every run */

    test_never_on_snake_or_obstacle();
    test_board_full();
    test_single_free_cell();
    test_bad_arguments();
    test_randomness();

    if (g_failed == 0) {
        puts("All food tests passed.");
        return 0;
    }
    printf("%d check(s) failed.\n", g_failed);
    return 1;
}
