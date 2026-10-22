
#include "game_interface.h"
#include "game_engine.h"
#include "score_manager.h"

#include <stdio.h>
#include <windows.h>

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
            Sleep(TICK_MS);
            engine_tick(&state);
        } else {
            Sleep(50);
        }
    }

    return 0;
}
