#pragma once

#include "board.h"

typedef struct {
    char name[50];
    Board own_board;
} Player;

void player_init(Player *player, const char *name);
