/*
 * collision.h
 * SG-17: Detect self-collision (Snake Game)
 *
 * The "Collision Detection Component" from the SAD (section 3.4), self-
 * collision part. The check used to live inline inside engine_tick(); it is
 * now a separate, testable function that engine_tick() calls.
 *
 * Implements SRS SNK-F-014: the game ends when the snake's head moves into a
 * cell occupied by its own body.
 *
 * The function is "static inline" in this header, so no extra .c file has to
 * be added to the build or to the GitHub Actions workflow.
 */
#ifndef COLLISION_H
#define COLLISION_H

#include "game_state.h"

/*
 * Would a snake head moving to new_head hit the snake's own body?
 *
 * will_grow is true when the head lands on food this tick. Normally the tail
 * cell is vacated as the snake moves, so the head may enter it; when the
 * snake grows, the tail stays put and that cell is still occupied.
 *
 * Returns false for a NULL state. snake_length is clamped to
 * MAX_SNAKE_LENGTH so a corrupted length can never read past the array
 * (SRS SNK-SR-001).
 */
static inline bool collision_hits_self(const GameState *state,
                                       Position new_head,
                                       bool will_grow)
{
    size_t i;
    size_t count;

    if (state == NULL) {
        return false;
    }

    count = state->snake_length;
    if (count > MAX_SNAKE_LENGTH) {
        count = MAX_SNAKE_LENGTH;
    }
    if (!will_grow && count > 0) {
        count--; /* the tail moves away this tick, so it is not an obstacle */
    }

    for (i = 0; i < count; ++i) {
        if (state->snake[i].x == new_head.x &&
            state->snake[i].y == new_head.y) {
            return true;
        }
    }
    return false;
}

#endif /* COLLISION_H */
