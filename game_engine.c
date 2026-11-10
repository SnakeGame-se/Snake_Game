
#include "game_engine.h"
#include "collision.h"
#include "score_manager.h"
#include "food.h"

#include <time.h>

/* Generate food using the shared, reliable food module. */
static void spawn_food(GameState *state)
{
    FoodCell snake_cells[MAX_SNAKE_LENGTH];
    FoodCell obstacle_cells[MAX_OBSTACLES];
    FoodCell generated_food;
    size_t i;

    if (state == NULL ||
        state->snake_length == 0 ||
        state->snake_length > MAX_SNAKE_LENGTH ||
        state->obstacle_count > MAX_OBSTACLES) {
        return;
    }

    /* Convert snake positions into FoodCell entries. */
    for (i = 0; i < state->snake_length; ++i) {
        snake_cells[i].x = state->snake[i].x;
        snake_cells[i].y = state->snake[i].y;
    }

    /* Convert obstacle positions into FoodCell entries. */
    for (i = 0; i < state->obstacle_count; ++i) {
        obstacle_cells[i].x = state->obstacles[i].x;
        obstacle_cells[i].y = state->obstacles[i].y;
    }

    /* Generate food on a free cell. */
    if (!food_generate(
            &generated_food,
            BOARD_WIDTH,
            BOARD_HEIGHT,
            snake_cells,
            (int)state->snake_length,
            obstacle_cells,
            (int)state->obstacle_count)) {

        /* No free cell remains. */
        session_end(state);
        return;
    }

    state->food.x = generated_food.x;
    state->food.y = generated_food.y;
}

/* SG-9: Initialize the snake and starting game state. */
void engine_initialize(GameState *state)
{
    if (state == NULL) {
        return;
    }

    session_init(state);

    state->snake_length = 3;

    state->snake[0] = (Position){
        BOARD_WIDTH / 2,
        BOARD_HEIGHT / 2
    };

    state->snake[1] = (Position){
        BOARD_WIDTH / 2 - 1,
        BOARD_HEIGHT / 2
    };

    state->snake[2] = (Position){
        BOARD_WIDTH / 2 - 2,
        BOARD_HEIGHT / 2
    };

    state->direction = DIR_RIGHT;

    /* Initialize fixed obstacles. */
    state->obstacle_count = 8;

    state->obstacles[0] = (Position){8, 5};
    state->obstacles[1] = (Position){15, 5};
    state->obstacles[2] = (Position){25, 5};
    state->obstacles[3] = (Position){32, 5};

    state->obstacles[4] = (Position){8, 15};
    state->obstacles[5] = (Position){15, 15};
    state->obstacles[6] = (Position){25, 15};
    state->obstacles[7] = (Position){32, 15};

    /* Seed the shared food generator. */
    food_seed((unsigned int)time(NULL));

    spawn_food(state);
}

/* SG-10: Update direction based on keyboard input. */
void engine_set_direction(GameState *state, int key)
{
    Direction next;

    if (state == NULL || state->status != GAME_RUNNING) {
        return;
    }

    /* Reject an invalid current direction. */
    if (state->direction < DIR_UP ||
        state->direction > DIR_RIGHT) {
        return;
    }

    switch (key) {
        case 'w': next = DIR_UP;    break;
        case 's': next = DIR_DOWN;  break;
        case 'a': next = DIR_LEFT;  break;
        case 'd': next = DIR_RIGHT; break;
        default: return;
    }

    /* Prevent direct reversal. */
    if ((state->direction == DIR_UP && next == DIR_DOWN) ||
        (state->direction == DIR_DOWN && next == DIR_UP) ||
        (state->direction == DIR_LEFT && next == DIR_RIGHT) ||
        (state->direction == DIR_RIGHT && next == DIR_LEFT)) {
        return;
    }

    state->direction = next;
}

/* SG-10: Move one cell, then check collisions and food. */
void engine_tick(GameState *state)
{
    Position new_head;
    size_t i;
    bool ate_food;

    /* SG-30: Validate game state before processing movement. */
    if (state == NULL ||
        state->status != GAME_RUNNING ||
        state->snake_length == 0 ||
        state->snake_length > MAX_SNAKE_LENGTH ||
        state->obstacle_count > MAX_OBSTACLES ||
        state->direction < DIR_UP ||
        state->direction > DIR_RIGHT) {
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
    for (i = 0; i < state->obstacle_count; ++i) {
        if (state->obstacles[i].x == new_head.x &&
            state->obstacles[i].y == new_head.y) {
            session_end(state);
            return;
        }
    }

    ate_food = (new_head.x == state->food.x &&
                new_head.y == state->food.y);

    /* Self-collision (SG-17). */
    if (collision_hits_self(state, new_head, ate_food)) {
        session_end(state);
        return;
    }

    /* Prevent snake array overflow during growth. */
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

    /* Generate the next food item after eating. */
    if (ate_food) {
        spawn_food(state);
    }
}
