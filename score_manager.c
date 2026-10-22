#include "score_manager.h"

#include <string.h>

void session_init(GameState *state) {
    if (state == NULL) return;
    memset(state, 0, sizeof(*state));
    state->status = GAME_READY;
    state->score = 0;
    state->final_score = 0;
}

void session_start(GameState *state) {
    if (state == NULL || state->status == GAME_QUIT) return;
    state->score = 0;
    state->final_score = 0;
    state->status = GAME_RUNNING;
}

void session_add_food_score(GameState *state) {
    if (state == NULL || state->status != GAME_RUNNING) return;
    if (state->score <= 2147483647 - SCORE_PER_FOOD) {
        state->score += SCORE_PER_FOOD;
    }
}

void session_pause(GameState *state) {
    if (state != NULL && state->status == GAME_RUNNING) {
        state->status = GAME_PAUSED;
    }
}

void session_resume(GameState *state) {
    if (state != NULL && state->status == GAME_PAUSED) {
        state->status = GAME_RUNNING;
    }
}

void session_toggle_pause(GameState *state) {
    if (state == NULL) return;
    if (state->status == GAME_RUNNING) {
        session_pause(state);
    } else if (state->status == GAME_PAUSED) {
        session_resume(state);
    }
}

void session_end(GameState *state) {
    if (state == NULL || state->status == GAME_QUIT) return;
    state->final_score = state->score;
    state->status = GAME_OVER;
}

void session_restart(GameState *state) {
    if (state == NULL) return;
    /* Preserve no prior session data: SRS v1.0 scores are session-only. */
    memset(state, 0, sizeof(*state));
    state->status = GAME_RUNNING;
}

bool session_is_running(const GameState *state) {
    return state != NULL && state->status == GAME_RUNNING;
}
