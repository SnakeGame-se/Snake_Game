/*
 * input_controls.h
 * SG-11: Implement keyboard controls (Snake Game)
 *
 * Reads the keyboard WITHOUT blocking the game loop and translates keys
 * into game actions. Works on Windows (conio.h) and Linux/macOS (termios).
 *
 * Key mapping (follows SRS section 3.2):
 *   Move up      : Up arrow    or W
 *   Move down    : Down arrow  or S
 *   Move left    : Left arrow  or A
 *   Move right   : Right arrow or D
 *   Pause/Resume : Space       or P
 *   Confirm      : Enter       (start / restart from menus)
 *   Quit         : Esc         or Q
 *
 * Any other key is ignored (SRS SNK-SR-003: unmapped input must not change
 * game state).
 *
 * Note: this module only REPORTS what the player pressed. Rejecting a
 * reverse move (e.g. Left while moving Right) is a separate story (SG-12).
 */
#ifndef INPUT_CONTROLS_H
#define INPUT_CONTROLS_H

typedef enum {
    INPUT_NONE = 0, /* no (recognised) key pressed since the last poll */
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_PAUSE,
    INPUT_ENTER,
    INPUT_QUIT
} InputAction;

/* Call once at program start (sets the terminal up on Linux/macOS). */
void input_init(void);

/* Call once before exiting (restores the terminal). Safe to call twice;
 * it is also registered with atexit() by input_init(). */
void input_shutdown(void);

/* Call once per game tick. Never blocks. If several keys were pressed since
 * the last call, the most recent recognised one is returned. */
InputAction input_poll(void);

/* Human-readable name of an action, for debugging/logging. */
const char *input_action_name(InputAction action);

#endif /* INPUT_CONTROLS_H */
