#include "game_interface.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#endif

#ifndef _WIN32
static int read_key_nonblocking(void) {
    struct termios old_term, raw_term;
    int ch;
    if (tcgetattr(STDIN_FILENO, &old_term) != 0) {
        return 0;
    }
    raw_term = old_term;
    raw_term.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    raw_term.c_cc[VMIN] = 0;
    raw_term.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw_term) != 0) {
        return 0;
    }

    fd_set read_set;
    struct timeval timeout = {0, 0};
    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    if (select(STDIN_FILENO + 1, &read_set, NULL, NULL, &timeout) > 0) {
        ch = getchar();
    } else {
        ch = 0;
    }
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
    return ch;
}
#endif

int interface_select_mode(void) {
    char line[64];
    for (;;) {
        printf("\n========================================\n");
        printf("             NOKIA SNAKE\n");
        printf("========================================\n");
        printf("1. Console mode\n");
        printf("2. Graphics mode\n");
        printf("0. Exit\n");
        printf("Choose an option: ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }
        if (line[0] == '1') {
            return MODE_CONSOLE;
        }
        if (line[0] == '2') {
            printf("Graphics mode selected. Connect the team's graphics renderer here.\n");
            return MODE_GRAPHICS;
        }
        if (line[0] == '0') {
            return 0;
        }
        printf("Invalid choice. Please enter 1, 2, or 0.\n");
    }
}

void interface_show_instructions(void) {
    printf("\nControls: W/A/S/D move | P pause/resume | R restart | Q quit\n");
    printf("Eat * to grow and earn %d points. Avoid walls, obstacles (#), and yourself.\n",
           SCORE_PER_FOOD);
    printf("Press Enter to start...");
    fflush(stdout);
    {
        char line[8];
        (void)fgets(line, sizeof(line), stdin);
    }
}

void interface_render_console(const GameState *state) {
    int x, y;
    if (state == NULL) {
        return;
    }

    /* ANSI clear-screen works in modern Windows Terminal and POSIX terminals. */
    printf("\033[2J\033[H");
    printf("NOKIA SNAKE   Score: %d   Status: ", state->score);
    switch (state->status) {
        case GAME_READY:   printf("READY"); break;
        case GAME_RUNNING: printf("RUNNING"); break;
        case GAME_PAUSED:  printf("PAUSED"); break;
        case GAME_OVER:    printf("GAME OVER"); break;
        case GAME_QUIT:    printf("QUIT"); break;
        default:           printf("UNKNOWN"); break;
    }
    printf("\n+");
    for (x = 0; x < BOARD_WIDTH; ++x) printf("-");
    printf("+\n");

    for (y = 0; y < BOARD_HEIGHT; ++y) {
        printf("|");
        for (x = 0; x < BOARD_WIDTH; ++x) {
            char cell = ' ';
            size_t i;
            Position p = {x, y};

            for (i = 0; i < state->obstacle_count && i < MAX_OBSTACLES; ++i) {
                if (state->obstacles[i].x == p.x && state->obstacles[i].y == p.y) {
                    cell = '#';
                    break;
                }
            }
            if (state->food.x == x && state->food.y == y) {
                cell = '*';
            }
            for (i = 0; i < state->snake_length && i < MAX_SNAKE_LENGTH; ++i) {
                if (state->snake[i].x == x && state->snake[i].y == y) {
                    cell = (i == 0) ? 'O' : 'o';
                    break;
                }
            }
            putchar(cell);
        }
        printf("|\n");
    }
    printf("+");
    for (x = 0; x < BOARD_WIDTH; ++x) printf("-");
    printf("+\n");
    printf("W/A/S/D: move | P: pause/resume | R: restart | Q: quit\n");
    fflush(stdout);
}

void interface_show_game_over(const GameState *state) {
    if (state == NULL) return;
    printf("\n========================================\n");
    printf("               GAME OVER\n");
    printf("          Final score: %d\n", state->final_score);
    printf("       R = restart | Q = quit\n");
    printf("========================================\n");
}

void interface_show_message(const char *message) {
    if (message != NULL) {
        printf("%s\n", message);
        fflush(stdout);
    }
}

int interface_read_key(void) {
#ifdef _WIN32
    if (!_kbhit()) return 0;
    {
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int arrow = _getch();
            switch (arrow) {
                case 72: return 'w';
                case 80: return 's';
                case 75: return 'a';
                case 77: return 'd';
                default: return 0;
            }
        }
        return tolower((unsigned char)ch);
    }
#else
    return tolower((unsigned char)read_key_nonblocking());
#endif
}

int interface_wait_key(void) {
    int key;
    do {
#ifdef _WIN32
        key = _getch();
        if (key == 0 || key == 224) {
            int arrow = _getch();
            switch (arrow) {
                case 72: return 'w';
                case 80: return 's';
                case 75: return 'a';
                case 77: return 'd';
                default: return 0;
            }
        }
#else
        key = getchar();
#endif
    } while (key == EOF);
    return tolower((unsigned char)key);
}
