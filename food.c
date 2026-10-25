/*
 * food.c
 * SG-13: Generate food randomly (Snake Game)
 * See food.h for the contract.
 *
 * Method: count the free cells, pick a random number k below that count,
 * then walk the grid again and return the k-th free cell. Every free cell
 * is equally likely, and the function always finishes (no retry loop that
 * could spin forever on a nearly full board).
 */
#include "food.h"

#include <stdlib.h>
#include <time.h>

static bool g_seeded = false;

void food_seed(unsigned int seed)
{
    if (seed == 0) {
        seed = (unsigned int)time(NULL);
    }
    srand(seed);
    g_seeded = true;
}

static bool cell_in_list(int x, int y, const FoodCell *cells, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        if (cells[i].x == x && cells[i].y == y) {
            return true;
        }
    }
    return false;
}

static bool cell_is_free(int x, int y,
                         const FoodCell *snake, int snake_len,
                         const FoodCell *obstacles, int obstacle_count)
{
    return !cell_in_list(x, y, snake, snake_len) &&
           !cell_in_list(x, y, obstacles, obstacle_count);
}

/* Random integer in [0, n). rand()/(RAND_MAX+1.0) is always < 1, so the
 * result can never reach n. */
static long random_below(long n)
{
    return (long)((double)rand() / ((double)RAND_MAX + 1.0) * (double)n);
}

bool food_generate(FoodCell *out,
                   int grid_w, int grid_h,
                   const FoodCell *snake, int snake_len,
                   const FoodCell *obstacles, int obstacle_count)
{
    long free_count = 0;
    long pick;
    int x, y;

    /* Validate arguments (SRS SNK-SR-001: never trust sizes/pointers). */
    if (out == NULL || grid_w <= 0 || grid_h <= 0 ||
        snake_len < 0 || obstacle_count < 0 ||
        (snake_len > 0 && snake == NULL) ||
        (obstacle_count > 0 && obstacles == NULL)) {
        return false;
    }

    if (!g_seeded) {
        food_seed(0);
    }

    /* Pass 1: how many cells are free? */
    for (y = 0; y < grid_h; y++) {
        for (x = 0; x < grid_w; x++) {
            if (cell_is_free(x, y, snake, snake_len, obstacles, obstacle_count)) {
                free_count++;
            }
        }
    }
    if (free_count == 0) {
        return false; /* board is full */
    }

    /* Pass 2: return the pick-th free cell. */
    pick = random_below(free_count);
    for (y = 0; y < grid_h; y++) {
        for (x = 0; x < grid_w; x++) {
            if (cell_is_free(x, y, snake, snake_len, obstacles, obstacle_count)) {
                if (pick == 0) {
                    out->x = x;
                    out->y = y;
                    return true;
                }
                pick--;
            }
        }
    }
    return false; /* not reachable; keeps the compiler satisfied */
}
