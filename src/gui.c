#include "raylib.h"
#include "gui.h"
#include <stdio.h>

static const Color COL_WATER_1   = {20, 45, 75, 255};
static const Color COL_WATER_2   = {17, 38, 65, 255};
static const Color COL_GRID      = {35, 70, 110, 180};
static const Color COL_SHIP      = {70, 90, 115, 255};
static const Color COL_HIT       = {230, 60, 40, 255};
static const Color COL_MISS_FILL = {130, 175, 200, 180};
static const Color COL_LABEL     = {90, 125, 160, 255};
static const Color COL_TEXT      = {200, 220, 240, 255};
static const Color COL_BTN       = {30, 80, 150, 255};
static const Color COL_BTN_HOV   = {45, 110, 200, 255};
static const Color COL_BTN_BRD   = {60, 150, 240, 255};
static const Color COL_VALID     = {40, 200, 80, 130};
static const Color COL_INVALID   = {220, 50, 50, 130};

static Font gui_font;
static Font gui_font_title;

void gui_init(void) {
    gui_font = LoadFontEx("fonts/roboto.ttf", 96, NULL, 0);
    SetTextureFilter(gui_font.texture, TEXTURE_FILTER_BILINEAR);

    gui_font_title = LoadFontEx("fonts/damageplan.ttf", 128, NULL, 0);
    SetTextureFilter(gui_font_title.texture, TEXTURE_FILTER_BILINEAR);
}

void gui_cleanup(void) {
    UnloadFont(gui_font);
    UnloadFont(gui_font_title);
}

void gui_draw_title(const char *text, int x, int y, int fontSize, Color color) {
    DrawTextEx(gui_font_title, text, (Vector2){(float)x, (float)y}, (float)fontSize, 1, color);
}

void gui_draw_text(const char *text, int x, int y, int fontSize, Color color) {
    DrawTextEx(gui_font, text, (Vector2){(float)x, (float)y}, (float)fontSize, 1, color);
}

int gui_measure_text(const char *text, int fontSize) {
    return (int)MeasureTextEx(gui_font, text, (float)fontSize, 1).x;
}

int gui_measure_title(const char *text, int fontSize) {
    return (int)MeasureTextEx(gui_font_title, text, (float)fontSize, 1).x;
}

void gui_draw_board_at(const Board *board, int bx, int by, int hide_ships) {

    for(int c = 0; c < BOARD_SIZE; c++) {
        char ch[2] = {(char)('A' + c), '\0'};
        Vector2 size = MeasureTextEx(gui_font, ch, 18, 1);
        DrawTextEx(gui_font, ch, (Vector2){bx + LABEL_SIZE + c * CELL_SIZE + CELL_SIZE / 2 - size.x / 2, by + 6}, 18, 1, COL_LABEL);
    }

    for(int r = 0; r < BOARD_SIZE; r++) {
        char num[3];
        snprintf(num, sizeof(num), "%d", r + 1);
        Vector2 size = MeasureTextEx(gui_font, num, 18, 1);
        DrawTextEx(gui_font, num, (Vector2){bx + LABEL_SIZE / 2 - size.x / 2, by + LABEL_SIZE + r * CELL_SIZE + CELL_SIZE / 2 - 9}, 18, 1, COL_LABEL);
    }

    int gx = bx + LABEL_SIZE;
    int gy = by + LABEL_SIZE;

    DrawRectangleLinesEx((Rectangle){(float)(gx - 2), (float)(gy - 2), (float)(BOARD_SIZE * CELL_SIZE + 4), (float)(BOARD_SIZE * CELL_SIZE + 4)}, 2.0f, COL_GRID);

    for(int r = 0; r < BOARD_SIZE; r++) {
        for(int c = 0; c < BOARD_SIZE; c++) {
            int cx = gx + c * CELL_SIZE;
            int cy = gy + r * CELL_SIZE;
            char cell = board->grid[r][c];

            Color fill;
            if(cell == CELL_HIT) {
                fill = COL_HIT;
            } else if(cell == CELL_SHIP && !hide_ships) {
                fill = COL_SHIP;
            } else {
                fill = ((r + c) % 2 == 0) ? COL_WATER_1 : COL_WATER_2;
            }
            DrawRectangle(cx + 1, cy + 1, CELL_SIZE - 2, CELL_SIZE - 2, fill);

            if(cell == CELL_HIT) {
                DrawLine(cx + 8, cy + 8, cx + CELL_SIZE - 9, cy + CELL_SIZE - 9, WHITE);
                DrawLine(cx + CELL_SIZE - 9, cy + 8, cx + 8, cy + CELL_SIZE - 9, WHITE);
            }

            if(cell == CELL_MISS) {
                DrawCircle(cx + CELL_SIZE / 2, cy + CELL_SIZE / 2, 8.0f, COL_MISS_FILL);
            }

            DrawRectangleLines(cx, cy, CELL_SIZE, CELL_SIZE, COL_GRID);
        }
    }
}

void gui_draw_ship_preview(int bx, int by, const Board *board, int row, int col, int size, int horizontal) {
    int valid = board_can_place_ship(board, size, row, col, horizontal ? 0 : 1);
    Color preview = valid ? COL_VALID : COL_INVALID;
    int gx = bx + LABEL_SIZE;
    int gy = by + LABEL_SIZE;

    for(int i = 0; i < size; i++) {
        int r = horizontal ? row : row + i;
        int c = horizontal ? col + i : col;
        if(r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
            DrawRectangle(gx + c * CELL_SIZE + 1, gy + r * CELL_SIZE + 1, CELL_SIZE - 2, CELL_SIZE - 2, preview);
        }
    }
}

void gui_draw_hover_cell(int bx, int by, int row, int col, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int gx = bx + LABEL_SIZE;
    int gy = by + LABEL_SIZE;
    DrawRectangle(gx + col * CELL_SIZE + 1, gy + row * CELL_SIZE + 1, CELL_SIZE - 2, CELL_SIZE - 2, (Color){r, g, b, a});
}

int gui_draw_button(const char *text, int x, int y, int w, int h) {
    Vector2 mouse = GetMousePosition();
    int hovered = (mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h);

    Color bg = hovered ? COL_BTN_HOV : COL_BTN;
    DrawRectangleRounded((Rectangle){(float)x, (float)y, (float)w, (float)h}, 0.3f, 8, bg);
    DrawRectangleRoundedLines((Rectangle){(float)x + 1, (float)y + 1, (float)w - 2, (float)h - 2}, 0.3f, 8, COL_BTN_BRD);

    Vector2 size = MeasureTextEx(gui_font, text, 24, 1);
    DrawTextEx(gui_font, text, (Vector2){x + (w - size.x) / 2, y + (h - 24) / 2}, 24, 1, COL_TEXT);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

int gui_get_hovered_cell(int bx, int by, int *out_row, int *out_col) {
    Vector2 mouse = GetMousePosition();
    int gx = bx + LABEL_SIZE;
    int gy = by + LABEL_SIZE;

    if(mouse.x < gx || mouse.y < gy) return 0;

    int col = (int)(mouse.x - gx) / CELL_SIZE;
    int row = (int)(mouse.y - gy) / CELL_SIZE;

    if(col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
        *out_row = row;
        *out_col = col;
        return 1;
    }
    return 0;
}
