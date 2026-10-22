
#include "game_engine.h"
#include "score_manager.h"

#include <stdlib.h>
#include <time.h>

/* Check whether a position is occupied by an obstacle. */
static bool is_obstacle(const GameState *state, Position p) {
    size_t i;

    for (i = 0; i < state->obstacle_count &&
                i < MAX_OBSTACLES; ++i) {
        if (state->obstacles[i].x == p.x &&
            state->obstacles[i].y == p.y) {
            return true;
        }
    }

    return false;
}

/* Place food on a valid, unoccupied cell. */
static void spawn_food(GameState *state) {
    int attempts;
    Position p;

    for (attempts = 0; attempts < BOARD_WIDTH * BOARD_HEIGHT;
         ++attempts) {
        size_t i;
        bool occupied = false;

        p.x = rand() % BOARD_WIDTH;
        p.y = rand() % BOARD_HEIGHT;

        for (i = 0; i < state->snake_length; ++i) {
            if (state->snake[i].x == p.x &&
                state->snake[i].y == p.y) {
                occupied = true;
                break;
            }
        }

        if (!occupied && !is_obstacle(state, p)) {
            state->food = p;
            return;
        }
    }

    /* No free cell remains. */
    session_end(state);
}

/* SG-9: Initialize the snake and starting game state. */
void engine_initialize(GameState *state) {
    if (state == NULL) {
        return;
    }

    session_init(state);

    state->snake_length = 3;

    state->snake[0] = (Position){BOARD_WIDTH / 2,
                                 BOARD_HEIGHT / 2};
    state->snake[1] = (Position){BOARD_WIDTH / 2 - 1,
                                 BOARD_HEIGHT / 2};
    state->snake[2] = (Position){BOARD_WIDTH / 2 - 2,
                                 BOARD_HEIGHT / 2};

    state->direction = DIR_RIGHT;
    state->obstacle_count = 0;

    srand((unsigned int)time(NULL));
    spawn_food(state);
}

/* SG-10: Update the direction based on keyboard input.
   Ignore direct reversals into the snake's own body. */
void engine_set_direction(GameState *state, int key) {
    Direction next;

    if (state == NULL || state->status != GAME_RUNNING) {
        return;
    }

    switch (key) {
        case 'w': next = DIR_UP;    break;
        case 's': next = DIR_DOWN;  break;
        case 'a': next = DIR_LEFT;  break;
        case 'd': next = DIR_RIGHT; break;
        default: return;
    }

    if ((state->direction == DIR_UP    && next == DIR_DOWN) ||
        (state->direction == DIR_DOWN  && next == DIR_UP) ||
        (state->direction == DIR_LEFT  && next == DIR_RIGHT) ||
        (state->direction == DIR_RIGHT && next == DIR_LEFT)) {
        return;
    }

    state->direction = next;
}

/* SG-10: Move one cell, then check collisions and food. */
void engine_tick(GameState *state) {
    Position new_head;
    size_t i;
    bool ate_food;

    if (state == NULL || state->status != GAME_RUNNING ||
        state->snake_length == 0 ||
        state->snake_length > MAX_SNAKE_LENGTH) {
        return;
    }

    new_head = state->snake[0];

    switch (state->direction) {
        case DIR_UP:    --new_head.y; break;
        case DIR_DOWN:  ++new_head.y; break;
        case DIR_LEFT:  --new_head.x; break;
        case DIR_RIGHT: ++new_head.x; break;
    }

    /* Wall collision. */
    if (new_head.x < 0 || new_head.x >= BOARD_WIDTH ||
        new_head.y < 0 || new_head.y >= BOARD_HEIGHT) {
        session_end(state);
        return;
    }

    /* Obstacle collision. */
    if (is_obstacle(state, new_head)) {
        session_end(state);
        return;
    }

    ate_food = (new_head.x == state->food.x &&
                new_head.y == state->food.y);

    /* Self-collision. The tail can move away unless food is eaten. */
    for (i = 0; i < state->snake_length; ++i) {
        if (!ate_food && i == state->snake_length - 1) {
            break;
        }

        if (state->snake[i].x == new_head.x &&
            state->snake[i].y == new_head.y) {
            session_end(state);
            return;
        }
    }

    /* Grow only when there is space in the snake array. */
    if (ate_food && state->snake_length >= MAX_SNAKE_LENGTH) {
        session_end(state);
        return;
    }

    if (ate_food) {
        ++state->snake_length;
        session_add_food_score(state);
    }

    /* Shift the body toward the tail. */
    for (i = state->snake_length - 1; i > 0; --i) {
        state->snake[i] = state->snake[i - 1];
    }

    state->snake[0] = new_head;

    if (ate_food) {
        spawn_food(state);
    }
}
