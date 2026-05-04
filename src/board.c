#include <string.h>
#include "board.h"

void board_init(Board *board) {
    for(int r = 0; r < BOARD_SIZE; r++) {
        for(int c = 0; c < BOARD_SIZE; c++) {
            board->grid[r][c] = CELL_EMPTY;
            board->ship_id[r][c] = -1;
        }
    }
    board->ship_count = 0;
}

int board_can_place_ship(const Board *board, int size, int row, int col, int direction) {
    if(direction == 0) {
        if(col + size > BOARD_SIZE) return 0;
    } else {
        if(row + size > BOARD_SIZE) return 0;
    }

    for(int i = 0; i < size; i++) {
        int r = row + (direction == 1 ? i : 0);
        int c = col + (direction == 0 ? i : 0);
        if (board->grid[r][c] != CELL_EMPTY) return 0;
    }
    return 1;
}

int board_place_ship(Board *board, const char *name, int size, int row, int col, int direction) {
    if(board->ship_count >= MAX_SHIPS) return 0;
    if(!board_can_place_ship(board, size, row, col, direction)) return 0;

    int idx = board->ship_count;
    ship_init(&board->ships[idx], name, size);

    for(int i = 0; i < size; i++) {
        int r = row + (direction == 1 ? i : 0);
        int c = col + (direction == 0 ? i : 0);
        board->grid[r][c] = CELL_SHIP;
        board->ship_id[r][c] = idx;
    }
    board->ship_count++;
    return 1;
}

int board_attack(Board *board, int row, int col) {
    if(row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
        return -1;

    char cell = board->grid[row][col];
    if(cell == CELL_HIT || cell == CELL_MISS)
        return -1;

    if(cell == CELL_SHIP) {
        board->grid[row][col] = CELL_HIT;
        int sid = board->ship_id[row][col];
        if(sid >= 0 && sid < board->ship_count) {
            ship_hit(&board->ships[sid]);
        }
        return 1;
    }

    board->grid[row][col] = CELL_MISS;
    return 0;
}

int board_all_sunk(const Board *board) {
    for(int i = 0; i < board->ship_count; i++) {
        if(!ship_is_sunk(&board->ships[i])) return 0;
    }
    return (board->ship_count > 0);
}
