
#include <stdio.h>
#include <stdlib.h>

#include "../game_engine.h"
#include "../score_manager.h"

static int failures = 0;
static int total_tests = 0;

#define CHECK(condition, message) do { \
    total_tests++; \
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

    /* Test 1: Initialize and start the first game. */
    engine_initialize(&state);
    session_start(&state);

    CHECK(state.status == GAME_RUNNING,
          "Initial game starts");

    /* Simulate a game with a longer snake and a score. */
    state.snake_length = 5;
    state.score = 30;
    state.direction = DIR_DOWN;

    /* Test 2: Restart during gameplay. */
    engine_initialize(&state);
    session_start(&state);

    CHECK(state.status == GAME_RUNNING,
          "Restart starts the game");

    CHECK(state.score == 0,
          "Restart resets score to zero");

    CHECK(state.final_score == 0,
          "Restart resets final score");

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

    /* Test 3: Restart after Game Over. */
    session_add_food_score(&state);
    session_add_food_score(&state);
    session_end(&state);

    CHECK(state.status == GAME_OVER,
          "Game enters Game Over state");

    CHECK(state.final_score == 20,
          "Final score is saved before restart");

    engine_initialize(&state);
    session_start(&state);

    CHECK(state.status == GAME_RUNNING,
          "Restart after Game Over starts a new game");

    CHECK(state.score == 0,
          "Restart after Game Over resets score");

    CHECK(state.final_score == 0,
          "Restart after Game Over clears final score");

    CHECK(state.snake_length == 3,
          "Restart after Game Over restores snake length");

    CHECK(state.direction == DIR_RIGHT,
          "Restart after Game Over restores direction");

    /* Test 4: New game works after restart. */
    session_add_food_score(&state);

    CHECK(state.score == SCORE_PER_FOOD,
          "New game can score after restart");

    CHECK(state.status == GAME_RUNNING,
          "New game remains running after scoring");

    /* Test 5: Restart after another game session. */
    state.snake_length = 7;
    state.score = 50;

    engine_initialize(&state);
    session_start(&state);

    CHECK(state.score == 0,
          "Repeated restart resets score");

    CHECK(state.snake_length == 3,
          "Repeated restart restores initial snake length");

    CHECK(state.status == GAME_RUNNING,
          "Repeated restart starts a running game");

    if (failures == 0) {
        printf("\nTotal tests: %d\n", total_tests);
        printf("Failures: %d\n", failures);
        printf("\nAll SG-26 restart tests passed!\n");
        return 0;
    }

    printf("\nTotal tests: %d\n", total_tests);
    printf("Failures: %d\n", failures);
    printf("\nSome SG-26 restart tests failed.\n");

    return 1;
}
