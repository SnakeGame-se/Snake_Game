#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include "game_state.h"

/* Session/score management. No persistent high-score file is used in v1.0. */
void session_init(GameState *state);
void session_start(GameState *state);
void session_add_food_score(GameState *state);
void session_pause(GameState *state);
void session_resume(GameState *state);
void session_toggle_pause(GameState *state);
void session_end(GameState *state);
void session_restart(GameState *state);

/* True when the session is active and movement updates are allowed. */
bool session_is_running(const GameState *state);

#endif
