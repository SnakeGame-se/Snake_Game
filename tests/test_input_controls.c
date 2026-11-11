#include "../input_controls.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

static void expect_action(int key, InputAction expected, const char *description)
{
    InputAction actual = input_map_character(key);

    if (actual != expected) {
        fprintf(stderr, "FAIL: %s (key %d): expected %s, got %s\n",
                description, key, input_action_name(expected),
                input_action_name(actual));
        ++failures;
    }
}

int main(void)
{
    expect_action('w', INPUT_UP, "lowercase up key");
    expect_action('W', INPUT_UP, "uppercase up key");
    expect_action('s', INPUT_DOWN, "lowercase down key");
    expect_action('S', INPUT_DOWN, "uppercase down key");
    expect_action('a', INPUT_LEFT, "lowercase left key");
    expect_action('A', INPUT_LEFT, "uppercase left key");
    expect_action('d', INPUT_RIGHT, "lowercase right key");
    expect_action('D', INPUT_RIGHT, "uppercase right key");
    expect_action(' ', INPUT_PAUSE, "space pauses");
    expect_action('p', INPUT_PAUSE, "lowercase pause key");
    expect_action('P', INPUT_PAUSE, "uppercase pause key");
    expect_action('r', INPUT_RESTART, "lowercase restart key");
    expect_action('R', INPUT_RESTART, "uppercase restart key");
    expect_action('\r', INPUT_ENTER, "carriage return confirms");
    expect_action('\n', INPUT_ENTER, "newline confirms");
    expect_action('q', INPUT_QUIT, "lowercase quit key");
    expect_action('Q', INPUT_QUIT, "uppercase quit key");
    expect_action('x', INPUT_NONE, "unmapped key is ignored");

    if (strcmp(input_action_name(INPUT_NONE), "NONE") != 0 ||
        strcmp(input_action_name(INPUT_RESTART), "RESTART") != 0 ||
        strcmp(input_action_name(INPUT_QUIT), "QUIT") != 0 ||
        strcmp(input_action_name((InputAction)99), "UNKNOWN") != 0) {
        fprintf(stderr, "FAIL: action names should be stable and unknown values safe\n");
        ++failures;
    }

    if (failures != 0) {
        fprintf(stderr, "%d input-control assertion(s) failed.\n", failures);
        return 1;
    }

    printf("All keyboard character mapping tests passed.\n");
    return 0;
}
