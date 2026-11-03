#ifndef SNAKE_LOGIC_H
#define SNAKE_LOGIC_H

#include "game_state.h"
#include <stdbool.h>

/* SG-12: Reject a direction that would reverse directly into the snake. */
bool snake_set_direction(GameState *game, Direction requested);

/* SG-14: True when the snake head occupies the food cell. */
bool check_food_collision(Position head, Position food);

/* SG-16: True when a position is outside the playable board. */
bool check_wall_collision(Position head);

typedef enum {
    SNAKE_MOVE_INVALID = -1,
    SNAKE_MOVE_NORMAL = 0,
    SNAKE_MOVE_ATE_FOOD = 1,
    SNAKE_MOVE_HIT_WALL = 2
} SnakeMoveResult;

/* Moves one cell and reports whether it was normal, ate food, hit a wall,
   or could not run because the state was invalid. */
SnakeMoveResult snake_update(GameState *game);

#endif
