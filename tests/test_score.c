
#include <stdio.h>
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
    GameState state = {0};

    /* Test 1: Initialize score */
    session_init(&state);
    CHECK(state.score == 0, "Initial score is zero");

    /* Test 2: Start session */
    session_start(&state);
    CHECK(state.status == GAME_RUNNING, "Session starts");

    /* Test 3: Add food score */
    session_add_food_score(&state);
    CHECK(state.score == SCORE_PER_FOOD, "Eating food adds 10 points");

    /* Test 4: Multiple food scores */
    session_add_food_score(&state);
    CHECK(state.score == 20, "Two food items add 20 points");

    /* Test 5: Paused session cannot gain points */
    session_pause(&state);
    session_add_food_score(&state);
    CHECK(state.score == 20, "Paused session does not gain points");

    /* Test 6: Final score */
    session_resume(&state);
    session_end(&state);
    CHECK(state.final_score == 20, "Final score is saved");

    /* Test 7: Restart resets score */
    session_restart(&state);
    CHECK(state.score == 0, "Restart resets score");
    CHECK(state.status == GAME_RUNNING, "Restart starts a new session");

    if (failures == 0) {
        printf("\nAll SG-19 score tests passed!\n");
        return 0;
    }

    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
