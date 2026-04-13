#pragma once

#include "player.h"

typedef enum {
    GAME_STATE_PLAYER1_SETUP,
    GAME_STATE_PLAYER2_SETUP,
    GAME_STATE_PLAYING,
    GAME_STATE_GAME_OVER
} GameState_t;

typedef struct {
    Player_t player1;
    Player_t player2;

    GameState_t state;
    int current_player;
    
    int selected_ship;
    int setup_complete[2];
} Game_t;

void initGame(Game_t*);
void gameUpdate(Game_t*);
void gameSetup(Game_t*);
void gamePlaying(Game_t*);
void gameOver(Game_t*);

void handleInput(Game_t*);