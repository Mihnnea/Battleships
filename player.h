#pragma once
#include "utils.h"

#define PLAYER_NAME_SIZE 32
#define NUM_SHIPS 4

typedef enum {
    water,
    ship,
    d_ship,
    d_water
} cell_types_t;

typedef enum {
    HORIZONTAL,
    VERTICAL
} ShipOrientation;

typedef struct {
    int row;
    int col;
    int length;
    ShipOrientation dir;
    int selected;
} Ship_t;

typedef struct Player {
    char name[PLAYER_NAME_SIZE + 1];
    cell_types_t battleships[MAP_SIZE][MAP_SIZE];
    cell_types_t attacked_battleships[MAP_SIZE][MAP_SIZE];
    Ship_t ships[NUM_SHIPS];
    int ships_ready;
} Player_t;

void initPlayer(Player_t *, const char *);
void placeShipsRandomly(Player_t *);
int canPlaceShip(Player_t *, int row, int col, int length, ShipOrientation dir, int ignoreShipIdx);
void placeShip(Player_t *, int shipIdx, int row, int col, ShipOrientation dir);
void rotateShip(Player_t *, int shipIdx);
void updateBoardFromShips(Player_t *);
int getShipAtCell(Player_t *, int row, int col);