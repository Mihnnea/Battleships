#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "player.h"

static int shouldIgnoreCell(Player_t *player, int checkRow, int checkCol, int ignoreShipIdx);

void initPlayer(Player_t *player, const char *name) {
    strncpy(player->name, name, PLAYER_NAME_SIZE);
    memset(player->battleships, 0, sizeof(player->battleships));
    memset(player->attacked_battleships, 0, sizeof(player->attacked_battleships));
    player->ships_ready = 0;
    
    int lengths[NUM_SHIPS] = {5, 4, 3, 2};
    for (int i = 0; i < NUM_SHIPS; i++) {
        player->ships[i].length = lengths[i];
        player->ships[i].dir = HORIZONTAL;
        player->ships[i].selected = 0;
        player->ships[i].row = 0;
        player->ships[i].col = 0;
    }
    
    placeShipsRandomly(player);
}

int canPlaceShip(Player_t *player, int row, int col, int length, ShipOrientation dir, int ignoreShipIdx) {
    if (dir == HORIZONTAL) {
        if (col + length > MAP_SIZE) return 0;
    } else {
        if (row + length > MAP_SIZE) return 0;
    }
    
    if (dir == HORIZONTAL) {
        for (int i = 0; i < length; i++) {
            if (player->battleships[row][col + i] == ship) {
                if (ignoreShipIdx != -1) {
                    Ship_t *ignoreShip = &player->ships[ignoreShipIdx];
                    if (ignoreShip->dir == HORIZONTAL) {
                        if (row == ignoreShip->row && col + i >= ignoreShip->col && col + i < ignoreShip->col + ignoreShip->length) {
                            continue;
                        }
                    }
                    else {
                        if (col + i == ignoreShip->col && row >= ignoreShip->row && row < ignoreShip->row + ignoreShip->length) {
                            continue;
                        }
                    }
                }
                return 0;
            }
        }
    } else {
        for (int i = 0; i < length; i++) {
            if (player->battleships[row + i][col] == ship) {
                if (ignoreShipIdx != -1) {
                    Ship_t *ignoreShip = &player->ships[ignoreShipIdx];
                    if (ignoreShip->dir == HORIZONTAL) {
                        if (row + i == ignoreShip->row && col >= ignoreShip->col && col < ignoreShip->col + ignoreShip->length) {
                            continue;
                        }
                    }
                    else {
                        if (col == ignoreShip->col && row + i >= ignoreShip->row && row + i < ignoreShip->row + ignoreShip->length) {
                            continue;
                        }
                    }
                }
                return 0;
            }
        }
    }
    
    if (dir == HORIZONTAL) {
        for (int i = -1; i < length + 1; i++) {
            if (row - 1 >= 0 && col + i >= 0 && col + i < MAP_SIZE) {
                if (player->battleships[row - 1][col + i] == ship) {
                    if (!shouldIgnoreCell(player, row - 1, col + i, ignoreShipIdx)) {
                        return 0;
                    }
                }
            }
            if (row + 1 < MAP_SIZE && col + i >= 0 && col + i < MAP_SIZE) {
                if (player->battleships[row + 1][col + i] == ship) {
                    if (!shouldIgnoreCell(player, row + 1, col + i, ignoreShipIdx)) {
                        return 0;
                    }
                }
            }
        }
        if (col - 1 >= 0 && player->battleships[row][col - 1] == ship) {
            if (!shouldIgnoreCell(player, row, col - 1, ignoreShipIdx)) {
                return 0;
            }
        }
        if (col + length < MAP_SIZE && player->battleships[row][col + length] == ship) {
            if (!shouldIgnoreCell(player, row, col + length, ignoreShipIdx)) {
                return 0;
            }
        }
    } else {
        for (int i = -1; i < length + 1; i++) {
            if (col - 1 >= 0 && row + i >= 0 && row + i < MAP_SIZE) {
                if (player->battleships[row + i][col - 1] == ship) {
                    if (!shouldIgnoreCell(player, row + i, col - 1, ignoreShipIdx)) {
                        return 0;
                    }
                }
            }
            if (col + 1 < MAP_SIZE && row + i >= 0 && row + i < MAP_SIZE) {
                if (player->battleships[row + i][col + 1] == ship) {
                    if (!shouldIgnoreCell(player, row + i, col + 1, ignoreShipIdx)) {
                        return 0;
                    }
                }
            }
        }
        if (row - 1 >= 0 && player->battleships[row - 1][col] == ship) {
            if (!shouldIgnoreCell(player, row - 1, col, ignoreShipIdx)) {
                return 0;
            }
        }
        if (row + length < MAP_SIZE && player->battleships[row + length][col] == ship) {
            if (!shouldIgnoreCell(player, row + length, col, ignoreShipIdx)) {
                return 0;
            }
        }
    }
    
    return 1;
}

static int shouldIgnoreCell(Player_t *player, int checkRow, int checkCol, int ignoreShipIdx) {
    if (ignoreShipIdx == -1) return 0;
    
    Ship_t *ignoreShip = &player->ships[ignoreShipIdx];
    
    if (ignoreShip->dir == HORIZONTAL) {
        return checkRow == ignoreShip->row && checkCol >= ignoreShip->col && 
               checkCol < ignoreShip->col + ignoreShip->length;
    } else {
        return checkCol == ignoreShip->col && checkRow >= ignoreShip->row && 
               checkRow < ignoreShip->row + ignoreShip->length;
    }
}

void placeShip(Player_t *player, int shipIdx, int row, int col, ShipOrientation dir) {
    if (shipIdx < 0 || shipIdx >= NUM_SHIPS) return;
    if (!canPlaceShip(player, row, col, player->ships[shipIdx].length, dir, shipIdx)) return;
    
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (player->battleships[i][j] == ship) {
                int belongsToShip = 0;
                if (player->ships[shipIdx].dir == HORIZONTAL) {
                    if (i == player->ships[shipIdx].row &&
                        j >= player->ships[shipIdx].col &&
                        j < player->ships[shipIdx].col + player->ships[shipIdx].length) {
                        belongsToShip = 1;
                    }
                } else {
                    if (j == player->ships[shipIdx].col &&
                        i >= player->ships[shipIdx].row &&
                        i < player->ships[shipIdx].row + player->ships[shipIdx].length) {
                        belongsToShip = 1;
                    }
                }
                if (belongsToShip) {
                    player->battleships[i][j] = water;
                }
            }
        }
    }
    
    player->ships[shipIdx].row = row;
    player->ships[shipIdx].col = col;
    player->ships[shipIdx].dir = dir;
    
    if (dir == HORIZONTAL) {
        for (int i = 0; i < player->ships[shipIdx].length; i++) {
            player->battleships[row][col + i] = ship;
        }
    } else {
        for (int i = 0; i < player->ships[shipIdx].length; i++) {
            player->battleships[row + i][col] = ship;
        }
    }
}

void rotateShip(Player_t *player, int shipIdx) {
    if (shipIdx < 0 || shipIdx >= NUM_SHIPS) return;
    
    ShipOrientation newDir = (player->ships[shipIdx].dir == HORIZONTAL) ? VERTICAL : HORIZONTAL;
    int row = player->ships[shipIdx].row;
    int col = player->ships[shipIdx].col;
    
    if (canPlaceShip(player, row, col, player->ships[shipIdx].length, newDir, shipIdx)) {
        placeShip(player, shipIdx, row, col, newDir);
    }
}

void updateBoardFromShips(Player_t *player) {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (player->battleships[i][j] == ship) {
                player->battleships[i][j] = water;
            }
        }
    }
    
    for (int s = 0; s < NUM_SHIPS; s++) {
        if (player->ships[s].dir == HORIZONTAL) {
            for (int i = 0; i < player->ships[s].length; i++) {
                player->battleships[player->ships[s].row][player->ships[s].col + i] = ship;
            }
        } else {
            for (int i = 0; i < player->ships[s].length; i++) {
                player->battleships[player->ships[s].row + i][player->ships[s].col] = ship;
            }
        }
    }
}

int getShipAtCell(Player_t *player, int row, int col) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        Ship_t *ship = &player->ships[i];
        if (ship->dir == HORIZONTAL) {
            if (row == ship->row && col >= ship->col && col < ship->col + ship->length) {
                return i;
            }
        } else {
            if (col == ship->col && row >= ship->row && row < ship->row + ship->length) {
                return i;
            }
        }
    }
    return -1;
}

void placeShipsRandomly(Player_t *player) {
    srand(time(NULL) + (uintptr_t)player);
    
    memset(player->battleships, 0, sizeof(player->battleships));
    
    for (int shipIdx = 0; shipIdx < NUM_SHIPS; shipIdx++) {
        int placed = 0;
        int attempts = 0;
        
        while (!placed && attempts < 100) {
            int row = rand() % MAP_SIZE;
            int col = rand() % MAP_SIZE;
            int dir = rand() % 2;
            
            if (canPlaceShip(player, row, col, player->ships[shipIdx].length, (ShipOrientation)dir, -1)) {
                placeShip(player, shipIdx, row, col, (ShipOrientation)dir);
                placed = 1;
            }
            attempts++;
        }
    }
}