#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "ui.h"
#include "utils.h"
#include "player.h"
#include "game.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define CELL_SIZE 40
#define CELL_GAP 10
#define PLAYER_NAME_FONT_SIZE 20
#define NAME_PADDING 15

#define COLOR_WATER (Color){30, 144, 255, 255}
#define COLOR_SHIP (Color){34, 139, 34, 255}
#define COLOR_DESTROYED_SHIP (Color){220, 20, 60, 255}
#define COLOR_DESTROYED_WATER (Color){0, 0, 0, 255}
#define COLOR_BACKGROUND (Color){40, 40, 40, 255}
#define COLOR_GRID (Color){100, 100, 100, 255}
#define COLOR_TEXT (Color){255, 255, 255, 255}

void initWindow(const char *title) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);
    SetTargetFPS(60);
}

void getBoardDimensions(int *boardWidth, int *boardHeight) {
    *boardWidth = MAP_SIZE * CELL_SIZE + (MAP_SIZE - 1) * CELL_GAP;
    *boardHeight = MAP_SIZE * CELL_SIZE + (MAP_SIZE - 1) * CELL_GAP + PLAYER_NAME_FONT_SIZE + NAME_PADDING;
}

void getGameContainerPosition(int *startX, int *startY) {
    int boardWidth, boardHeight;
    getBoardDimensions(&boardWidth, &boardHeight);
    *startX = (WINDOW_WIDTH - boardWidth) / 2;
    *startY = (WINDOW_HEIGHT - boardHeight) / 2;
}

int windowShouldClose() {
    return WindowShouldClose();
}

void closeWindow() {
    CloseWindow();
}

void drawBoard(Player_t *player, int containerX, int containerY, int reveal) {
    DrawText(player->name, containerX, containerY, PLAYER_NAME_FONT_SIZE, COLOR_TEXT);
    
    int boardStartX = containerX;
    int boardStartY = containerY + PLAYER_NAME_FONT_SIZE + NAME_PADDING;
    
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            int x = boardStartX + j * (CELL_SIZE + CELL_GAP);
            int y = boardStartY + i * (CELL_SIZE + CELL_GAP);
            
            Color cellColor = COLOR_WATER;
            
            if(reveal && player->battleships[i][j] == ship)
                cellColor = COLOR_SHIP;
            
            if(player->battleships[i][j] == d_ship)
                cellColor = COLOR_DESTROYED_SHIP;
            else if(player->battleships[i][j] == d_water)
                cellColor = COLOR_DESTROYED_WATER;
            
            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, cellColor);
            DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, COLOR_GRID);
        }
    }
}

void drawFrame(Game_t *game) {
    BeginDrawing();
    ClearBackground(COLOR_BACKGROUND);
    
    int containerX, containerY;
    getGameContainerPosition(&containerX, &containerY);
    
    switch (game->state) {
        case GAME_STATE_PLAYER1_SETUP:
            DrawText("Player 1 Setup - Place your ships", 50, 50, 40, COLOR_TEXT);
            drawBoard(&game->player1, containerX, containerY, 1);
            break;
        
        case GAME_STATE_PLAYER2_SETUP:
            DrawText("Player 2 Setup - Place your ships", 50, 50, 40, COLOR_TEXT);
            drawBoard(&game->player2, containerX, containerY, 1);
            break;
        
        case GAME_STATE_PLAYING:
            if(game->current_player == 1) {
                DrawText("Player 1's Turn", 50, 50, 40, COLOR_TEXT);
                DrawText("Your Board:", 50, 120, 20, COLOR_TEXT);
                drawBoard(&game->player1, 50, 150, 1);
                DrawText("Enemy Board:", 650, 120, 20, COLOR_TEXT);
                drawBoard(&game->player2, 650, 150, 0);
            } else {
                DrawText("Player 2's Turn", 50, 50, 40, COLOR_TEXT);
                DrawText("Your Board:", 50, 120, 20, COLOR_TEXT);
                drawBoard(&game->player2, 50, 150, 1);
                DrawText("Enemy Board:", 650, 120, 20, COLOR_TEXT);
                drawBoard(&game->player1, 650, 150, 0);
            }
            break;
        
        case GAME_STATE_GAME_OVER:
            DrawText("Game Over!", 50, 50, 40, COLOR_TEXT);
            DrawText("Press R to restart or ESC to exit", 50, 150, 30, COLOR_TEXT);
            break;
    }
    
    EndDrawing();
}