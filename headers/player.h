#pragma once

#include "board.h"

typedef struct {
    char name[50];
    Board own_board;
    int tactical_points;
    int sonar_pings;
    int carpet_bombs;
} Player;

void player_init(Player *player, const char *name);
