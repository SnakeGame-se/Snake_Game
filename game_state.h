#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include <stddef.h>

#define BOARD_WIDTH  40
#define BOARD_HEIGHT 20
#define MAX_SNAKE_LENGTH (BOARD_WIDTH * BOARD_HEIGHT)
#define MAX_OBSTACLES 32
#define SCORE_PER_FOOD 10

typedef struct {
    int x;
    int y;
} Position;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum {
    GAME_READY,
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_QUIT
} GameStatus;

typedef enum {
    MODE_CONSOLE = 1,
    MODE_GRAPHICS = 2
} InterfaceMode;

/* Shared in-memory state. The game engine should own movement/collision updates. */
typedef struct {
    Position snake[MAX_SNAKE_LENGTH];
    size_t snake_length;
    Position food;
    Position obstacles[MAX_OBSTACLES];
    size_t obstacle_count;
    Direction direction;
    int score;
    int final_score;
    GameStatus status;
} GameState;

#endif
