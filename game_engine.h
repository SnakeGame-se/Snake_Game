
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "game_state.h"

/* SG-9: Set up the initial snake and game state. */
void engine_initialize(GameState *state);

/* SG-10: Change direction for the next movement tick. */
void engine_set_direction(GameState *state, int key);

/* SG-10: Advance the snake by one grid cell. */
void engine_tick(GameState *state);

#endif
