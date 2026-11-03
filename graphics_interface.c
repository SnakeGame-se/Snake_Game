
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "graphics_interface.h"
#include "game_engine.h"
#include "score_manager.h"

#define CELL_SIZE 20
#define BOARD_X 20
#define BOARD_Y 100

#define BOARD_PIXEL_WIDTH (BOARD_WIDTH * CELL_SIZE)
#define BOARD_PIXEL_HEIGHT (BOARD_HEIGHT * CELL_SIZE)

#define WINDOW_WIDTH (BOARD_PIXEL_WIDTH + 60)
#define WINDOW_HEIGHT (BOARD_PIXEL_HEIGHT + 160)

#define TIMER_ID 1
#define TICK_MS 120
#define GRAPHICS_OBSTACLE_COUNT 8

static GameState *game_state = NULL;
static HWND game_window = NULL;

/*
 * Add graphics-only obstacles.
 * Candidate positions are checked against the snake and food
 * before being added to the shared game state.
 */
static void setup_graphics_obstacles(void)
{
    static const Position candidates[] = {
        {7, 4}, {8, 4}, {9, 4},
        {30, 4}, {31, 4}, {32, 4},
        {7, 15}, {8, 15}, {9, 15},
        {30, 15}, {31, 15}, {32, 15}
    };

    size_t i;
    size_t j;
    size_t candidate_count =
        sizeof(candidates) / sizeof(candidates[0]);

    if (game_state == NULL) {
        return;
    }

    game_state->obstacle_count = 0;

    for (i = 0;
         i < candidate_count &&
         game_state->obstacle_count < GRAPHICS_OBSTACLE_COUNT;
         ++i) {

        bool occupied = false;

        /* Do not place an obstacle on the snake. */
        for (j = 0; j < game_state->snake_length; ++j) {
            if (game_state->snake[j].x == candidates[i].x &&
                game_state->snake[j].y == candidates[i].y) {
                occupied = true;
                break;
            }
        }

        /* Do not place an obstacle on the food. */
        if (game_state->food.x == candidates[i].x &&
            game_state->food.y == candidates[i].y) {
            occupied = true;
        }

        if (!occupied &&
            game_state->obstacle_count < MAX_OBSTACLES) {
            game_state->obstacles[game_state->obstacle_count] =
                candidates[i];

            ++game_state->obstacle_count;
        }
    }
}

/* Draw one cell on the board. */
static void draw_cell(
    HDC hdc,
    int x,
    int y,
    COLORREF color
)
{
    RECT rect;
    HBRUSH brush;

    rect.left = x;
    rect.top = y;
    rect.right = x + CELL_SIZE;
    rect.bottom = y + CELL_SIZE;

    brush = CreateSolidBrush(color);
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
}

/* Draw text using a selected font and color. */
static void draw_text(
    HDC hdc,
    int x,
    int y,
    const char *message,
    COLORREF color,
    int font_size
)
{
    HFONT font;
    HFONT old_font;

    font = CreateFontA(
        font_size,
        0, 0, 0,
        FW_BOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH,
        "Arial"
    );

    old_font = (HFONT)SelectObject(hdc, font);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    TextOutA(
        hdc,
        x,
        y,
        message,
        (int)strlen(message)
    );

    SelectObject(hdc, old_font);
    DeleteObject(font);
}

/* Render the complete game screen. */
static void render_game(HDC hdc)
{
    size_t i;
    char score_text[80];

    RECT background;
    RECT board_rect;

    HBRUSH background_brush;
    HBRUSH board_brush;

    if (game_state == NULL) {
        return;
    }

    GetClientRect(game_window, &background);

    background_brush = CreateSolidBrush(RGB(18, 24, 38));
    FillRect(hdc, &background, background_brush);
    DeleteObject(background_brush);

    draw_text(hdc, 20, 15, "SNAKE GAME",
              RGB(80, 220, 150), 30);

    sprintf(score_text, "Score: %d", game_state->score);

    draw_text(hdc, 20, 58, score_text,
              RGB(255, 255, 255), 18);

    draw_text(hdc, 160, 60,
              "Move: WASD / Arrow Keys",
              RGB(190, 200, 215), 13);

    draw_text(hdc, 160, 80,
              "P: Pause   R: Restart   Q / ESC: Quit",
              RGB(190, 200, 215), 13);

    board_rect.left = BOARD_X;
    board_rect.top = BOARD_Y;
    board_rect.right = BOARD_X + BOARD_PIXEL_WIDTH;
    board_rect.bottom = BOARD_Y + BOARD_PIXEL_HEIGHT;

    board_brush = CreateSolidBrush(RGB(29, 39, 55));
    FillRect(hdc, &board_rect, board_brush);
    DeleteObject(board_brush);

    /* Draw obstacles. */
    for (i = 0;
         i < game_state->obstacle_count &&
         i < MAX_OBSTACLES;
         ++i) {

        draw_cell(
            hdc,
            BOARD_X + game_state->obstacles[i].x * CELL_SIZE,
            BOARD_Y + game_state->obstacles[i].y * CELL_SIZE,
            RGB(145, 145, 155)
        );
    }

    /* Draw food. */
    draw_cell(
        hdc,
        BOARD_X + game_state->food.x * CELL_SIZE,
        BOARD_Y + game_state->food.y * CELL_SIZE,
        RGB(240, 75, 85)
    );

    /* Draw snake body. */
    for (i = game_state->snake_length; i > 1; --i) {
        draw_cell(
            hdc,
            BOARD_X + game_state->snake[i - 1].x * CELL_SIZE,
            BOARD_Y + game_state->snake[i - 1].y * CELL_SIZE,
            RGB(65, 190, 125)
        );
    }

    /* Draw snake head. */
    if (game_state->snake_length > 0) {
        draw_cell(
            hdc,
            BOARD_X + game_state->snake[0].x * CELL_SIZE,
            BOARD_Y + game_state->snake[0].y * CELL_SIZE,
            RGB(120, 255, 165)
        );
    }

    if (game_state->status == GAME_PAUSED) {
        draw_text(hdc, BOARD_X + 280, BOARD_Y + 170,
                  "PAUSED", RGB(255, 220, 100), 32);

        draw_text(hdc, BOARD_X + 245, BOARD_Y + 220,
                  "Press P to resume",
                  RGB(255, 255, 255), 17);
    }
    else if (game_state->status == GAME_OVER) {
        draw_text(hdc, BOARD_X + 270, BOARD_Y + 160,
                  "GAME OVER", RGB(255, 90, 90), 32);

        draw_text(hdc, BOARD_X + 230, BOARD_Y + 210,
                  "Press R to restart",
                  RGB(255, 255, 255), 17);

        draw_text(hdc, BOARD_X + 230, BOARD_Y + 240,
                  "Press Q or ESC to quit",
                  RGB(255, 255, 255), 17);
    }
}

/* Restart the game and restore graphics obstacles. */
static void restart_game(void)
{
    if (game_state == NULL) {
        return;
    }

    engine_initialize(game_state);
    setup_graphics_obstacles();
    session_start(game_state);

    InvalidateRect(game_window, NULL, TRUE);
}

/* Handle Windows messages and keyboard input. */
static LRESULT CALLBACK game_window_proc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    PAINTSTRUCT ps;
    HDC hdc;

    (void)lParam;

    switch (message) {

    case WM_CREATE:
        SetTimer(hwnd, TIMER_ID, TICK_MS, NULL);
        return 0;

    case WM_ACTIVATE:
        if (LOWORD(wParam) != WA_INACTIVE) {
            SetFocus(hwnd);
        }
        return 0;

    case WM_TIMER:
        if (game_state != NULL &&
            game_state->status == GAME_RUNNING) {

            engine_tick(game_state);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_KEYDOWN:

        if (game_state == NULL) {
            return 0;
        }

        if (wParam == 'Q' || wParam == 'q' ||
            wParam == VK_ESCAPE) {

            game_state->status = GAME_QUIT;
            DestroyWindow(hwnd);
            return 0;
        }

        if (wParam == 'R' || wParam == 'r') {
            restart_game();
            return 0;
        }

        if (wParam == 'P' || wParam == 'p') {

            if (game_state->status == GAME_RUNNING ||
                game_state->status == GAME_PAUSED) {
                session_toggle_pause(game_state);
            }

            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        if (game_state->status == GAME_RUNNING) {

            switch (wParam) {

            case VK_UP:
            case 'W':
            case 'w':
                engine_set_direction(game_state, 'w');
                break;

            case VK_DOWN:
            case 'S':
            case 's':
                engine_set_direction(game_state, 's');
                break;

            case VK_LEFT:
            case 'A':
            case 'a':
                engine_set_direction(game_state, 'a');
                break;

            case VK_RIGHT:
            case 'D':
            case 'd':
                engine_set_direction(game_state, 'd');
                break;

            default:
                break;
            }
        }

        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        render_game(hdc);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_CLOSE:
        if (game_state != NULL) {
            game_state->status = GAME_QUIT;
        }

        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, message, wParam, lParam);
}

/* Start the Windows graphics interface. */
int graphics_run(GameState *state)
{
    WNDCLASSA window_class;
    MSG message;
    ATOM class_atom;

    if (state == NULL) {
        return 1;
    }

    game_state = state;

    /* Add obstacles only for Graphics Mode. */
    setup_graphics_obstacles();

    ZeroMemory(&window_class, sizeof(window_class));

    window_class.lpfnWndProc = game_window_proc;
    window_class.hInstance = GetModuleHandleA(NULL);
    window_class.lpszClassName = "SnakeGameWindow";
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    class_atom = RegisterClassA(&window_class);

    if (class_atom == 0 &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {

        game_state = NULL;
        return 1;
    }

    game_window = CreateWindowExA(
        0,
        "SnakeGameWindow",
        "Snake Game - Graphics Mode",
        WS_OVERLAPPED | WS_CAPTION |
        WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        NULL,
        NULL,
        GetModuleHandleA(NULL),
        NULL
    );

    if (game_window == NULL) {
        game_state = NULL;
        return 1;
    }

    ShowWindow(game_window, SW_SHOW);
    UpdateWindow(game_window);

    SetForegroundWindow(game_window);
    SetActiveWindow(game_window);
    SetFocus(game_window);

    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    game_window = NULL;
    game_state = NULL;

    return 0;
}
