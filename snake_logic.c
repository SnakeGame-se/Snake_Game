#include "snake_logic.h"

static bool positions_equal(Position a, Position b)
{
    return a.x == b.x && a.y == b.y;
}

static bool direction_is_valid(Direction direction)
{
    return direction == DIR_UP || direction == DIR_DOWN ||
           direction == DIR_LEFT || direction == DIR_RIGHT;
}

/* SG-12: Opposite directions are rejected; perpendicular turns are allowed. */
bool snake_set_direction(GameState *game, Direction requested)
{
    if (game == NULL || !direction_is_valid(requested)) {
        return false;
    }

    if ((game->direction == DIR_UP && requested == DIR_DOWN) ||
        (game->direction == DIR_DOWN && requested == DIR_UP) ||
        (game->direction == DIR_LEFT && requested == DIR_RIGHT) ||
        (game->direction == DIR_RIGHT && requested == DIR_LEFT)) {
        return false;
    }

    game->direction = requested;
    return true;
}

/* SG-14: Food collision is based on equality of head and food coordinates. */
bool check_food_collision(Position head, Position food)
{
    return positions_equal(head, food);
}

/* SG-16: Board coordinates are valid from 0 to width-1 / height-1. */
bool check_wall_collision(Position head)
{
    return head.x < 0 || head.x >= BOARD_WIDTH ||
           head.y < 0 || head.y >= BOARD_HEIGHT;
}

static Position next_head_position(Position head, Direction direction)
{
    switch (direction) {
        case DIR_UP:    --head.y; break;
        case DIR_DOWN:  ++head.y; break;
        case DIR_LEFT:  --head.x; break;
        case DIR_RIGHT: ++head.x; break;
        default: break;
    }
    return head;
}

/* SG-15: Preserve the tail when food is eaten, increasing length by one. */
SnakeMoveResult snake_update(GameState *game)
{
    Position new_head;
    bool ate_food;
    size_t i;

    if (game == NULL || game->snake_length == 0 ||
        game->snake_length > MAX_SNAKE_LENGTH ||
        !direction_is_valid(game->direction)) {
        return SNAKE_MOVE_INVALID;
    }

    new_head = next_head_position(game->snake[0], game->direction);

    /* SG-16: End the session immediately if the next head is outside the board. */
    if (check_wall_collision(new_head)) {
        game->status = GAME_OVER;
        return SNAKE_MOVE_HIT_WALL;
    }

    /* SG-14: Check the next head cell against the current food position. */
    ate_food = check_food_collision(new_head, game->food);

    if (ate_food && game->snake_length >= MAX_SNAKE_LENGTH) {
        game->status = GAME_OVER;
        return SNAKE_MOVE_INVALID;
    }

    /* Shift body from tail to head. If food is eaten, do not discard the tail. */
    if (ate_food) {
        for (i = game->snake_length; i > 0; --i) {
            game->snake[i] = game->snake[i - 1];
        }
        ++game->snake_length;
    } else {
        for (i = game->snake_length - 1; i > 0; --i) {
            game->snake[i] = game->snake[i - 1];
        }
    }

    game->snake[0] = new_head;
    return ate_food ? SNAKE_MOVE_ATE_FOOD : SNAKE_MOVE_NORMAL;
}
