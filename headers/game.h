#pragma once

#include "player.h"

typedef enum {
    STATE_MENU,
    STATE_P1_PLACE,
    STATE_TRANSITION,
    STATE_P2_PLACE,
    STATE_P1_SHOP,
    STATE_P2_SHOP,
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

    int shop_selected_powerup; // 0 = None, 1 = Decoy (2 pts), 2 = Sonar Ping (3 pts), 3 = Naval Shield (4 pts), 4 = Carpet Bomb (6 pts)
    int active_powerup;        // 0 = None, 1 = Sonar Ping, 2 = Carpet Bomb
    int sonar_scan_mode;
    char sonar_message[128];
    float sonar_message_timer;
} Game;

void game_init(Game *game);
void game_update(Game *game);
void game_draw(Game *game);