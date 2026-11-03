
#include <stdio.h>
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
    GameState state = {0};

    /* Test 1: Initialize score */
    session_init(&state);
    CHECK(state.score == 0, "Initial score is zero");
    CHECK(state.final_score == 0, "Initial final score is zero");

    /* Test 2: Start session */
    session_start(&state);
    CHECK(state.status == GAME_RUNNING, "Session starts");
    CHECK(session_is_running(&state), "Running status is detected");

    /* Test 3: Add food score */
    session_add_food_score(&state);
    CHECK(state.score == SCORE_PER_FOOD,
          "Eating food adds 10 points");

    /* Test 4: Multiple food scores */
    session_add_food_score(&state);
    session_add_food_score(&state);
    CHECK(state.score == 30,
          "Three food items add 30 points");

    /* Test 5: Paused session cannot gain points */
    session_pause(&state);
    session_add_food_score(&state);
    CHECK(state.score == 30,
          "Paused session does not gain points");
    CHECK(!session_is_running(&state),
          "Paused session is not running");

    /* Test 6: Resume session */
    session_resume(&state);
    CHECK(state.status == GAME_RUNNING,
          "Paused session resumes");

    /* Test 7: Final score */
    session_end(&state);
    CHECK(state.final_score == 30,
          "Final score is saved");
    CHECK(state.status == GAME_OVER,
          "Session ends with Game Over");

    /* Test 8: Game Over session cannot gain points */
    session_add_food_score(&state);
    CHECK(state.score == 30,
          "Game Over session cannot gain points");

    /* Test 9: Restart resets score */
    session_restart(&state);
    CHECK(state.score == 0,
          "Restart resets score");
    CHECK(state.final_score == 0,
          "Restart resets final score");
    CHECK(state.status == GAME_RUNNING,
          "Restart starts a new session");
    CHECK(session_is_running(&state),
          "Restarted session is running");

    /* Test 10: New session can score again */
    session_add_food_score(&state);
    CHECK(state.score == SCORE_PER_FOOD,
          "Scoring works after restart");

    /* Test 11: Toggle pause */
    session_toggle_pause(&state);
    CHECK(state.status == GAME_PAUSED,
          "Toggle pause pauses the session");

    session_toggle_pause(&state);
    CHECK(state.status == GAME_RUNNING,
          "Toggle pause resumes the session");

    /* Test 12: NULL safety */
    session_init(NULL);
    session_start(NULL);
    session_add_food_score(NULL);
    session_pause(NULL);
    session_resume(NULL);
    session_toggle_pause(NULL);
    session_end(NULL);
    session_restart(NULL);
    CHECK(!session_is_running(NULL),
          "NULL state is handled safely");

    printf("\nTotal tests: %d\n", total_tests);
    printf("Failures: %d\n", failures);

    if (failures == 0) {
        printf("\nAll SG-26 scoring tests passed!\n");
        return 0;
    }

    printf("\nSome SG-26 scoring tests failed.\n");
    return 1;
}
