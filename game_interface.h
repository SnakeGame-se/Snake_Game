#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include "game_state.h"

/* Returns MODE_CONSOLE, MODE_GRAPHICS, or 0 to exit.
   Graphics mode is a selection hook; its renderer must be supplied by the team. */
int interface_select_mode(void);

/* Console UI functions. */
void interface_show_instructions(void);
void interface_render_console(const GameState *state);
void interface_show_game_over(const GameState *state);
void interface_show_message(const char *message);

/* Reads one key without waiting for a full line.
   Returns 0 if no key is available; normalizes movement to WASD,
   P = pause/resume, R = restart, Q = quit, Enter/Space = start. */
int interface_read_key(void);

/* Wait for one key press (used on menus/game-over screen). */
int interface_wait_key(void);

#endif
