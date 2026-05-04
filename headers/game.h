#pragma once

#include "player.h"

typedef enum {
    STATE_MENU,
    STATE_P1_PLACE,
    STATE_TRANSITION,
    STATE_P2_PLACE,
    STATE_P1_ATTACK,
    STATE_P2_ATTACK,
    STATE_GAME_OVER
} GameState;

typedef struct {
    Player players[2];
    Board attack_boards[2];
    GameState state;
    GameState next_state;

    int placing_ship_idx;
    int place_horizontal;

    int last_result;
    float result_timer;

    int transition_player;
    int winner;

    const char *ship_names[MAX_SHIPS];
    int ship_sizes[MAX_SHIPS];
} Game;

void game_init(Game *game);
void game_update(Game *game);
void game_draw(Game *game);