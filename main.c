
#ifndef _WIN32
#define _POSIX_C_SOURCE 199309L
#endif

#include "game_interface.h"
#include "game_engine.h"
#include "score_manager.h"
#include "database.h"
#include "input_controls.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include "graphics_interface.h"
#include <windows.h>
#else
#include <time.h>
#endif

#define TICK_MS 120

static void delay_ms(int milliseconds);

/* Translate the platform-neutral keyboard action into the existing game API. */
static int read_game_key(void)
{
    InputAction action = input_poll();
    switch (action) {
        case INPUT_UP: return 'w';
        case INPUT_DOWN: return 's';
        case INPUT_LEFT: return 'a';
        case INPUT_RIGHT: return 'd';
        case INPUT_PAUSE: return 'p';
        case INPUT_RESTART: return 'r';
        case INPUT_QUIT: return 'q';
        case INPUT_ENTER: return '\n';
        case INPUT_NONE: default: return 0;
    }
}

static int wait_game_key(void)
{
    int key;
    while ((key = read_game_key()) == 0) delay_ms(10);
    return key;
}

static void delay_ms(int milliseconds)
{
#ifdef _WIN32
    Sleep((DWORD)milliseconds);
#else
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (long)(milliseconds % 1000) * 1000000L;

    nanosleep(&ts, NULL);
#endif
}

int main(void)
{
    GameState state;
    int mode;
    int key;
    int player_id;
    int score_saved = 0;
    char username[DB_USERNAME_MAX + 2];

    printf("Enter username (1-%d characters): ", DB_USERNAME_MAX);
    fflush(stdout);
    if (fgets(username, sizeof(username), stdin) == NULL) {
        return 1;
    }
    username[strcspn(username, "\r\n")] = '\0';
    if (username[0] == '\0' || strlen(username) > DB_USERNAME_MAX) {
        fprintf(stderr, "Invalid username.\n");
        return 1;
    }

    if (!db_connect()) {
        fprintf(stderr, "Cannot start game without database connection. Check DATABASE_SETUP.md.\n");
        return 1;
    }
    player_id = db_get_or_create_player(username);
    if (player_id < 1) {
        db_close();
        return 1;
    }

    mode = interface_select_mode();

    if (mode == 0) {
        db_close();
        return 0;
    }

    engine_initialize(&state);
    session_start(&state);

    /* Graphics mode is available on Windows only. */
    if (mode == MODE_GRAPHICS) {
#ifdef _WIN32
        {
            int result = graphics_run(&state, player_id);
            db_close();
            return result;
        }
#else
        interface_show_message(
            "Graphics mode is available only on Windows."
        );
        db_close();
        return 0;
#endif
    }

    if (mode != MODE_CONSOLE) {
        interface_show_message("Invalid game mode selected.");
        db_close();
        return 1;
    }

    interface_show_instructions();
    input_init();

    while (state.status != GAME_QUIT) {

        interface_render_console(&state);

        if (state.status == GAME_OVER) {

            if (!score_saved) {
                if (!db_save_game_result(player_id, state.final_score)) {
                    interface_show_message("Warning: score could not be saved.");
                }
                score_saved = 1;
            }
            interface_show_game_over(&state);
            interface_show_leaderboard();

            key = wait_game_key();

            if (key == 'r') {
                engine_initialize(&state);
                session_start(&state);
                score_saved = 0;
                continue;
            }

            break;
        }

        key = read_game_key();

        if (key == 'q') {
            state.status = GAME_QUIT;
            break;
        }

        if (key == 'p') {
            session_toggle_pause(&state);
        }
        else if (key == 'r') {
            engine_initialize(&state);
            session_start(&state);
            score_saved = 0;
        }
        else {
            engine_set_direction(&state, key);
        }

        if (state.status == GAME_RUNNING) {
            delay_ms(TICK_MS);
            engine_tick(&state);
        }
        else {
            delay_ms(50);
        }
    }

    input_shutdown();
    db_close();
    return 0;
}
