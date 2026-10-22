
#ifndef _WIN32
#define _POSIX_C_SOURCE 199309L
#endif
#include "game_interface.h"
#include "game_engine.h"
#include "score_manager.h"

#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

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
#define TICK_MS 120

int main(void) {
    GameState state;
    int mode;
    int key;

    mode = interface_select_mode();

    if (mode == 0) {
        return 0;
    }

    if (mode != MODE_CONSOLE) {
        interface_show_message(
            "Graphics mode is not implemented yet. "
            "Please run console mode."
        );
        return 0;
    }

    engine_initialize(&state);
    interface_show_instructions();
    session_start(&state);

    while (state.status != GAME_QUIT) {
        interface_render_console(&state);

        if (state.status == GAME_OVER) {
            interface_show_game_over(&state);
            key = interface_wait_key();

            if (key == 'r') {
                engine_initialize(&state);
                session_start(&state);
                continue;
            }

            break;
        }

        key = interface_read_key();

        if (key == 'q') {
            state.status = GAME_QUIT;
            break;
        }

        if (key == 'p') {
            session_toggle_pause(&state);
        } else if (key == 'r') {
            engine_initialize(&state);
            session_start(&state);
        } else {
            engine_set_direction(&state, key);
        }

        if (state.status == GAME_RUNNING) {
            delay_ms(TICK_MS);
            engine_tick(&state);
        } else {
            delay_ms(50);
        }
    }

    return 0;
}
