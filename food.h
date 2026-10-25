/*
 * food.h
 * SG-13: Generate food randomly (Snake Game)
 *
 * Implements SRS SNK-F-008: place one food item on a random EMPTY cell,
 * i.e. a cell not occupied by the snake or by an obstacle.
 *
 * Grid convention used here: valid cells are x = 0..grid_w-1 and
 * y = 0..grid_h-1. If the game board has a wall border, pass the size of
 * the PLAYABLE area (or offset the result) so food never lands on a wall.
 *
 * Detecting that the snake ate the food (SG-14) and growing the snake
 * (SG-15) are separate stories and are not part of this module.
 */
#ifndef FOOD_H
#define FOOD_H

#include <stdbool.h>

/* A grid cell. If the project's snake/obstacle arrays use another struct,
 * copy x/y into FoodCell arrays before calling food_generate(). */
typedef struct {
    int x;
    int y;
} FoodCell;

/* Seed the random generator. Pass 0 to seed from the clock (normal play);
 * pass a fixed number for repeatable tests. If you never call this,
 * food_generate() seeds from the clock on first use. */
void food_seed(unsigned int seed);

/*
 * Pick a random free cell and store it in *out.
 *
 * snake / snake_len        : every cell the snake occupies (head and body)
 * obstacles / obstacle_count: every obstacle cell
 *
 * Returns true on success.
 * Returns false (and leaves *out untouched) if:
 *   - out is NULL, grid_w/grid_h <= 0, a count is negative, or an array
 *     pointer is NULL while its count is > 0 (bad arguments), or
 *   - there is no free cell left (the board is full).
 */
bool food_generate(FoodCell *out,
                   int grid_w, int grid_h,
                   const FoodCell *snake, int snake_len,
                   const FoodCell *obstacles, int obstacle_count);

#endif /* FOOD_H */
