#pragma once
#include "utils.h"

#define PLAYER_NAME_SIZE 32

typedef enum {
    water,
    ship,
    d_ship,
    d_water
} cell_types_t;

typedef struct Player {
    char name[PLAYER_NAME_SIZE + 1];
    cell_types_t battleships[MAP_SIZE][MAP_SIZE];
} Player_t;

void initPLayer(Player_t *, const char *);