#include "game.h"
#include "player.h"

void initGame(Game_t* game) {
    initPlayer(&game->player1, "Player 1");
    initPlayer(&game->player2, "Player 2");
    game->state = GAME_STATE_PLAYER1_SETUP;
    game->current_player = 1;
}

void gameUpdate(Game_t *game) {
    switch (game->state) {
        case GAME_STATE_PLAYER1_SETUP:
        case GAME_STATE_PLAYER2_SETUP:
            gameSetup(game);
            break;
        case GAME_STATE_PLAYING:
            gamePlaying(game);
            break;
        case GAME_STATE_GAME_OVER:
            gameOver(game);
            break;
    }
}

void gameSetup(Game_t *game) {
    if (game->state == GAME_STATE_PLAYER1_SETUP) {

    } 
    else if (game->state == GAME_STATE_PLAYER2_SETUP) {

    }
}

void gamePlaying(Game_t *game) {
    if (game->state != GAME_STATE_PLAYING)
        return;
}

void gameOver(Game_t *game) {
    if (game->state != GAME_STATE_GAME_OVER)
        return;
    
}