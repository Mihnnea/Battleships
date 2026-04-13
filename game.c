#include <stdio.h>
#include <raylib.h>

#include "game.h"
#include "player.h"
#include "ui.h"

static int draggedShipIdx = -1;
static int isDragging = 0;

void initGame(Game_t* game) {
    initPlayer(&game->player1, "Player 1");
    initPlayer(&game->player2, "Player 2");
    game->state = GAME_STATE_PLAYER1_SETUP;
    game->current_player = 1;
    game->selected_ship = -1;
    game->setup_complete[0] = 0;
    game->setup_complete[1] = 0;
    draggedShipIdx = -1;
    isDragging = 0;
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
    if (game->state != GAME_STATE_PLAYER1_SETUP && game->state != GAME_STATE_PLAYER2_SETUP)
        return;
    
    Player_t *currentPlayer = (game->state == GAME_STATE_PLAYER1_SETUP) ? &game->player1 : &game->player2;
    
    int row, col;
    if (getCellClick(0, &row, &col)) {
        int shipIdx = getShipAtCell(currentPlayer, row, col);
        if (shipIdx != -1) {
            game->selected_ship = shipIdx;
            draggedShipIdx = shipIdx;
            isDragging = 1;
        }
    }
    
    if (IsKeyPressed(KEY_R) && game->selected_ship != -1) {
        rotateShip(currentPlayer, game->selected_ship);
    }
    
    if (isDragging && draggedShipIdx != -1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        int boardStartX, boardStartY;
        getBoardPosition(0, &boardStartX, &boardStartY);
        
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        
        int relX = mouseX - boardStartX;
        int relY = mouseY - boardStartY - PLAYER_NAME_FONT_SIZE - NAME_PADDING;
        
        if (relX >= 0 && relY >= 0) {
            int tempCol = relX / (CELL_SIZE + CELL_GAP);
            int tempRow = relY / (CELL_SIZE + CELL_GAP);
            
            if (tempCol < MAP_SIZE && tempRow < MAP_SIZE) {
                if (canPlaceShip(currentPlayer, tempRow, tempCol, 
                               currentPlayer->ships[draggedShipIdx].length,
                               currentPlayer->ships[draggedShipIdx].dir, draggedShipIdx)) {
                    placeShip(currentPlayer, draggedShipIdx, tempRow, tempCol, 
                             currentPlayer->ships[draggedShipIdx].dir);
                }
            }
        }
    } else if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        isDragging = 0;
        draggedShipIdx = -1;
    }
}

void gamePlaying(Game_t *game) {
    if (game->state != GAME_STATE_PLAYING)
        return;
    
    int opponentPlayerIdx = (game->current_player == 1) ? 1 : 0;
    int row, col;
    
    if (getCellClick(opponentPlayerIdx, &row, &col)) {
        printf("Player %d attacked cell (%d, %d)\n", game->current_player, row, col);
        game->current_player = (game->current_player == 1) ? 2 : 1;
    }
}

void gameOver(Game_t *game) {
    if (game->state != GAME_STATE_GAME_OVER)
        return;
    
}

void handleInput(Game_t *game) {
    int row, col;
    switch (game->state)
    {
        case GAME_STATE_PLAYER1_SETUP:
        case GAME_STATE_PLAYER2_SETUP: {
            if (getCellClick(0, &row, &col)) {
                printf("Cell (%d, %d) was chosen.\n", row, col);
            }
            break;
        }
        
        case GAME_STATE_PLAYING: {
            if (getCellClick(game->current_player == 1 ? 1 : 0, &row, &col)) {
                printf("Player %d clicked on cell (%d, %d)\n", game->current_player, row, col);
            }
            break;
        }
        
        default:
            break;
    }
}