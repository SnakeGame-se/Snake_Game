/*
 * input_controls.c
 * SG-11: Implement keyboard controls (Snake Game)
 * See input_controls.h for the key mapping.
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "input_controls.h"

#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

/* Plain (non-arrow) keys, shared by both platforms. */
static InputAction map_char(int c)
{
    switch (c) {
    case 'w': case 'W': return INPUT_UP;
    case 's': case 'S': return INPUT_DOWN;
    case 'a': case 'A': return INPUT_LEFT;
    case 'd': case 'D': return INPUT_RIGHT;
    case ' ': case 'p': case 'P': return INPUT_PAUSE;
    case '\r': case '\n': return INPUT_ENTER;
    case 'q': case 'Q': return INPUT_QUIT;
    default: return INPUT_NONE; /* unmapped keys are ignored */
    }
}

const char *input_action_name(InputAction action)
{
    switch (action) {
    case INPUT_NONE:  return "NONE";
    case INPUT_UP:    return "UP";
    case INPUT_DOWN:  return "DOWN";
    case INPUT_LEFT:  return "LEFT";
    case INPUT_RIGHT: return "RIGHT";
    case INPUT_PAUSE: return "PAUSE";
    case INPUT_ENTER: return "ENTER";
    case INPUT_QUIT:  return "QUIT";
    }
    return "UNKNOWN";
}

/* ------------------------------------------------------------------ */
#ifdef _WIN32
/* ----------------------------- Windows ----------------------------- */

void input_init(void) { /* nothing to set up with conio.h */ }
void input_shutdown(void) { /* nothing to restore */ }

InputAction input_poll(void)
{
    InputAction result = INPUT_NONE;

    while (_kbhit()) {
        int c = _getch();
        InputAction a;

        if (c == 0 || c == 224) { /* special key: a second code follows */
            switch (_getch()) {
            case 72: a = INPUT_UP;    break;
            case 80: a = INPUT_DOWN;  break;
            case 75: a = INPUT_LEFT;  break;
            case 77: a = INPUT_RIGHT; break;
            default: a = INPUT_NONE;  break;
            }
        } else if (c == 27) {
            a = INPUT_QUIT; /* Esc */
        } else {
            a = map_char(c);
        }

        if (a != INPUT_NONE) {
            result = a; /* most recent recognised key wins */
        }
    }
    return result;
}

#else
/* --------------------------- Linux / macOS -------------------------- */

static struct termios g_saved;
static int g_raw_mode = 0;

void input_shutdown(void)
{
    if (g_raw_mode) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_saved);
        g_raw_mode = 0;
    }
}

void input_init(void)
{
    struct termios raw;

    if (g_raw_mode) {
        return;
    }
    if (tcgetattr(STDIN_FILENO, &g_saved) != 0) {
        return; /* stdin is not a terminal (e.g. piped input): nothing to do */
    }
    raw = g_saved;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO); /* no line buffering, no echo */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
        g_raw_mode = 1;
        atexit(input_shutdown); /* restore the terminal even on exit() */
    }
}

/* Is a byte waiting on stdin? Waits at most timeout_ms. */
static int byte_ready(int timeout_ms)
{
    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

static int read_byte(void)
{
    unsigned char c;
    return (read(STDIN_FILENO, &c, 1) == 1) ? (int)c : -1;
}

/* Called after an Esc byte: arrow keys arrive as Esc [ A/B/C/D
 * (or Esc O A/B/C/D). A lone Esc means the player pressed Esc to quit. */
static InputAction parse_escape(void)
{
    int b1, b2;

    if (!byte_ready(5)) {
        return INPUT_QUIT;
    }
    b1 = read_byte();
    if (b1 != '[' && b1 != 'O') {
        return INPUT_NONE;
    }
    if (!byte_ready(5)) {
        return INPUT_NONE;
    }
    b2 = read_byte();
    switch (b2) {
    case 'A': return INPUT_UP;
    case 'B': return INPUT_DOWN;
    case 'C': return INPUT_RIGHT;
    case 'D': return INPUT_LEFT;
    default:  return INPUT_NONE;
    }
}

InputAction input_poll(void)
{
    InputAction result = INPUT_NONE;

    while (byte_ready(0)) {
        int c = read_byte();
        InputAction a;

        if (c < 0) {
            break; /* end of input or read error: avoid spinning */
        }
        a = (c == 27) ? parse_escape() : map_char(c);
        if (a != INPUT_NONE) {
            result = a; /* most recent recognised key wins */
        }
    }
    return result;
}

#endif
