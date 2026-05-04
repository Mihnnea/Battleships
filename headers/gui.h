#pragma once

#include "board.h"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 750

#define CELL_SIZE  40
#define LABEL_SIZE 30
#define BOARD_PX   (LABEL_SIZE + BOARD_SIZE * CELL_SIZE)

void gui_init(void);
void gui_cleanup(void);
void gui_draw_title(const char *text, int x, int y, int fontSize, Color color);
void gui_draw_text(const char *text, int x, int y, int fontSize, Color color);
int gui_measure_text(const char *text, int fontSize);
int gui_measure_title(const char *text, int fontSize);


void gui_draw_board_at(const Board *board, int bx, int by, int hide_ships);
void gui_draw_ship_preview(int bx, int by, const Board *board, int row, int col, int size, int horizontal);
void gui_draw_hover_cell(int bx, int by, int row, int col, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

int gui_draw_button(const char *text, int x, int y, int w, int h);
int gui_get_hovered_cell(int bx, int by, int *out_row, int *out_col);