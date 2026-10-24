/*
 * input_demo.c
 * Small manual-test program for SG-11 (keyboard controls).
 *
 * Build (Linux/macOS/MinGW):
 *   gcc -std=c99 -Wall -Wextra -o input_demo input_controls.c tests/input_demo.c
 * Run:
 *   ./input_demo      (input_demo.exe on Windows)
 *
 * Press arrow keys / WASD / Space / Enter and see the detected action.
 * Press Q or Esc to quit.
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>

#include "../input_controls.h"

#ifdef _WIN32
#include <windows.h>
static void sleep_ms(int ms) { Sleep((DWORD)ms); }
#else
#include <time.h>
static void sleep_ms(int ms)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = (long)ms * 1000000L;
    nanosleep(&ts, NULL);
}
#endif

int main(void)
{
    puts("SG-11 input demo");
    puts("Arrows/WASD = move, Space/P = pause, Enter = confirm, Q/Esc = quit");

    input_init();
    for (;;) {
        InputAction a = input_poll();

        if (a != INPUT_NONE) {
            printf("Detected: %s\n", input_action_name(a));
            fflush(stdout);
        }
        if (a == INPUT_QUIT) {
            break;
        }
        sleep_ms(50); /* stands in for one game tick */
    }
    input_shutdown();
    puts("Bye.");
    return 0;
}
