#pragma once

#include "ship.h"

#define BOARD_SIZE 10

typedef enum {
    CELL_EMPTY,
    CELL_SHIP,
    CELL_HIT,
    CELL_MISS
} CellType;

typedef struct {
    CellType grid[BOARD_SIZE][BOARD_SIZE];
    int ship_id[BOARD_SIZE][BOARD_SIZE];
    Ship ships[MAX_SHIPS];
    int ship_count;

    int decoy_grid[BOARD_SIZE][BOARD_SIZE];
    int shield_grid[BOARD_SIZE][BOARD_SIZE];
} Board;

void board_init(Board *board);
int board_can_place_ship(const Board *board, int size, int row, int col, int direction);
int board_place_ship(Board *board, const char *name, int size, int row, int col, int direction);
// Returns: -1 on invalid, 0 on miss, 1 on hit, 2 on shield blocked, 3 on decoy tripped
int board_attack(Board *board, int row, int col);
int board_all_sunk(const Board *board);
int board_scan_row(const Board *board, int row);
int board_scan_col(const Board *board, int col);