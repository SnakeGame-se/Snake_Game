
#include <stdio.h>
#include <stdlib.h>

#include "../game_engine.h"
#include "../score_manager.h"

static int failures = 0;

#define CHECK(condition, message) do { \
    if (!(condition)) { \
        printf("FAIL: %s\n", message); \
        failures++; \
    } else { \
        printf("PASS: %s\n", message); \
    } \
} while (0)

int main(void)
{
    GameState state;

    /* Initialize and start the first game. */
    engine_initialize(&state);
    session_start(&state);

    /* Simulate a game with a longer snake and a score. */
    state.snake_length = 5;
    state.score = 30;
    state.direction = DIR_DOWN;

    /* Restart the game. */
    engine_initialize(&state);
    session_start(&state);

    CHECK(state.status == GAME_RUNNING,
          "Restart starts the game");

    CHECK(state.score == 0,
          "Restart resets score to zero");

    CHECK(state.snake_length == 3,
          "Restart resets snake length to three");

    CHECK(state.direction == DIR_RIGHT,
          "Restart resets direction to right");

    CHECK(state.snake[0].x == BOARD_WIDTH / 2 &&
          state.snake[0].y == BOARD_HEIGHT / 2,
          "Restart resets snake head position");

    CHECK(state.snake[1].x == BOARD_WIDTH / 2 - 1 &&
          state.snake[1].y == BOARD_HEIGHT / 2,
          "Restart resets second snake segment");

    CHECK(state.snake[2].x == BOARD_WIDTH / 2 - 2 &&
          state.snake[2].y == BOARD_HEIGHT / 2,
          "Restart resets third snake segment");

    CHECK(state.obstacle_count == 0,
          "Restart clears obstacles");

    if (failures == 0) {
        printf("\nAll SG-21 restart tests passed!\n");
        return 0;
    }

    printf("\n%d test(s) failed.\n", failures);
    return 1;
}

