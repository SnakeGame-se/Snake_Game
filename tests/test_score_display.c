
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#define dup _dup
#define dup2 _dup2
#define close _close
#define fileno _fileno
#else
#include <unistd.h>
#endif

#include "../game_interface.h"

int main(void)
{
    GameState state = {0};
    FILE *output;
    int saved_stdout;
    char buffer[4096];
    size_t bytes_read;

    state.score = 50;
    state.status = GAME_RUNNING;

    output = tmpfile();

    if (output == NULL) {
        perror("Could not create temporary output file");
        return 1;
    }

    fflush(stdout);
    saved_stdout = dup(fileno(stdout));

    if (saved_stdout == -1) {
        perror("Could not save stdout");
        fclose(output);
        return 1;
    }

    if (dup2(fileno(output), fileno(stdout)) == -1) {
        perror("Could not redirect stdout");
        fclose(output);
        return 1;
    }

    interface_render_console(&state);
    fflush(stdout);

    if (dup2(saved_stdout, fileno(stdout)) == -1) {
        perror("Could not restore stdout");
        fclose(output);
        return 1;
    }

    close(saved_stdout);

    rewind(output);
    bytes_read = fread(buffer, 1, sizeof(buffer) - 1, output);
    buffer[bytes_read] = '\0';

    fclose(output);

    if (strstr(buffer, "Score: 50") != NULL) {
        printf("PASS: Current score is displayed correctly\n");
    } else {
        printf("FAIL: Current score was not displayed\n");
        return 1;
    }

    if (strstr(buffer, "RUNNING") != NULL) {
        printf("PASS: Game status is displayed correctly\n");
    } else {
        printf("FAIL: Game status was not displayed\n");
        return 1;
    }

    printf("\nAll SG-20 display tests passed!\n");
    return 0;
}
