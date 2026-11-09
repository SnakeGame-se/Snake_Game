
#include <stdio.h>
#include "../game_engine.h"

static int checks = 0;
static int failures = 0;

#define CHECK(condition, message) do { \
    checks++; \
    if (condition) { \
        printf("PASS: %s\n", message); \
    } else { \
        printf("FAIL: %s (line %d)\n", message, __LINE__); \
        failures++; \
    } \
} while (0)

static int same_position(Position a, Position b)
{
    return a.x == b.x && a.y == b.y;
}

static void test_initialization(void)
{
    GameState state;
    size_t i;
    int food_overlaps_snake = 0;
    int food_overlaps_obstacle = 0;

    engine_initialize(&state);

    CHECK(state.status == GAME_READY,
          "Initialization sets status to GAME_READY");

    CHECK(state.snake_length == 3,
          "Initial snake length is three");

    CHECK(state.direction == DIR_RIGHT,
          "Initial direction is right");

    CHECK(same_position(state.snake[0],
          (Position){BOARD_WIDTH / 2, BOARD_HEIGHT / 2}),
          "Snake head starts at board center");

    CHECK(same_position(state.snake[1],
          (Position){BOARD_WIDTH / 2 - 1, BOARD_HEIGHT / 2}),
          "First body segment starts behind head");

    CHECK(same_position(state.snake[2],
          (Position){BOARD_WIDTH / 2 - 2, BOARD_HEIGHT / 2}),
          "Second body segment starts behind first segment");

    CHECK(state.obstacle_count == 8,
          "Eight fixed obstacles are initialized");

    CHECK(state.food.x >= 0 && state.food.x < BOARD_WIDTH &&
          state.food.y >= 0 && state.food.y < BOARD_HEIGHT,
          "Food is placed within board boundaries");

    for (i = 0; i < state.snake_length; ++i) {
        if (same_position(state.food, state.snake[i])) {
            food_overlaps_snake = 1;
        }
    }

    for (i = 0; i < state.obstacle_count; ++i) {
        if (same_position(state.food, state.obstacles[i])) {
            food_overlaps_obstacle = 1;
        }
    }

    CHECK(!food_overlaps_snake,
          "Food does not overlap the snake");

    CHECK(!food_overlaps_obstacle,
          "Food does not overlap an obstacle");
}

static void test_null_initialization(void)
{
    engine_initialize(NULL);

    CHECK(1, "NULL initialization is handled safely");
}

int main(void)
{
    test_initialization();
    test_null_initialization();

    printf("\nSG-30 Initialization: %d checks, %d failure(s).\n",
           checks, failures);

    if (failures == 0) {
        puts("ALL INITIALIZATION TESTS PASSED.");
        return 0;
    }

    return 1;
}
