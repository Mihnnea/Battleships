#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#include "ui.h"
#include "utils.h"
#include "player.h"
#include "game.h"

static int textInputMode = 0;
static char inputBuffer[PLAYER_NAME_SIZE + 1] = {0};
static int inputIndex = 0;

static void getBoardDimensions(int *boardWidth, int *boardHeight) {
    *boardWidth = MAP_SIZE * CELL_SIZE + (MAP_SIZE - 1) * CELL_GAP;
    *boardHeight = MAP_SIZE * CELL_SIZE + (MAP_SIZE - 1) * CELL_GAP + PLAYER_NAME_FONT_SIZE + NAME_PADDING;
}

void getBoardPosition(int playerIndex, int *startX, int *startY) {
    int boardWidth, boardHeight;
    getBoardDimensions(&boardWidth, &boardHeight);
    
    if (playerIndex == 0) {
        *startX = BOARD_OFFSET_X;
        *startY = (WINDOW_HEIGHT - boardHeight) / 2;
    } else {
        *startX = WINDOW_WIDTH - boardWidth - BOARD_OFFSET_X;
        *startY = (WINDOW_HEIGHT - boardHeight) / 2;
    }
}

static Color getCellColor(int cellType) {
    switch(cellType) {
        case water:
            return COLOR_WATER;
        case ship:
            return COLOR_SHIP;
        case d_ship:
            return COLOR_DESTROYED_SHIP;
        case d_water:
            return COLOR_DESTROYED_WATER;
        default:
            return COLOR_WATER;
    }
}

static Color getShipColor(int length, int isSelected) {
    if (isSelected) {
        return (Color){255, 215, 0, 255};
    }
    switch(length) {
        case 5: return (Color){34, 139, 34, 255};
        case 4: return (Color){50, 160, 50, 255};
        case 3: return (Color){100, 180, 100, 255};
        case 2: return (Color){144, 238, 144, 255};
        default: return COLOR_SHIP;
    }
}

static int pixelToBoardCell(int pixelX, int pixelY, int boardStartX, int boardStartY, 
                            int *row, int *col) {
    int relX = pixelX - boardStartX;
    int relY = pixelY - boardStartY - PLAYER_NAME_FONT_SIZE - NAME_PADDING;
    
    if (relX < 0 || relY < 0) return 0;
    
    int boardWidth, boardHeight;
    getBoardDimensions(&boardWidth, &boardHeight);
    
    if (relX > boardWidth || relY > (MAP_SIZE * CELL_SIZE + (MAP_SIZE - 1) * CELL_GAP)) 
        return 0;
    int tempCol = -1, tempRow = -1;
    int cellX = 0;
    
    for (int i = 0; i < MAP_SIZE; i++) {
        cellX = i * (CELL_SIZE + CELL_GAP);
        if (relX >= cellX && relX < cellX + CELL_SIZE) {
            tempCol = i;
            break;
        }
    }
    
    int cellY = 0;
    for (int i = 0; i < MAP_SIZE; i++) {
        cellY = i * (CELL_SIZE + CELL_GAP);
        if (relY >= cellY && relY < cellY + CELL_SIZE) {
            tempRow = i;
            break;
        }
    }
    
    if (tempRow != -1 && tempCol != -1) {
        *row = tempRow;
        *col = tempCol;
        return 1;
    }
    
    return 0;
}

void initWindow(const char *title) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);
    SetTargetFPS(TARGET_FPS);
}

int windowShouldClose(void) {
    return WindowShouldClose();
}

void closeWindow(void) {
    CloseWindow();
}

void beginFrame(void) {
    BeginDrawing();
    ClearBackground(COLOR_BACKGROUND);
}

void endFrame(void) {
    EndDrawing();
}

void clearScreen(void) {
    ClearBackground(COLOR_BACKGROUND);
}

void drawCell(int row, int col, int cellType, int boardStartX, int boardStartY) {
    int x = boardStartX + col * (CELL_SIZE + CELL_GAP);
    int y = boardStartY + PLAYER_NAME_FONT_SIZE + NAME_PADDING + row * (CELL_SIZE + CELL_GAP);
    
    Color cellColor = getCellColor(cellType);
    DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, cellColor);
    DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, COLOR_GRID);
}

void drawPlayerBoard(Player_t *player, int playerIndex, int reveal) {
    int boardStartX, boardStartY;
    getBoardPosition(playerIndex, &boardStartX, &boardStartY);
    
    DrawText(player->name, boardStartX, boardStartY, PLAYER_NAME_FONT_SIZE, COLOR_TEXT);
    
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            int cellType = player->battleships[i][j];
            
            if (!reveal) {
                if (player->attacked_battleships[i][j] == d_ship) {
                    cellType = d_ship;
                } else if (player->attacked_battleships[i][j] == d_water) {
                    cellType = d_water;
                } else {
                    cellType = water;
                }
            }
            
            drawCell(i, j, cellType, boardStartX, boardStartY);
        }
    }
}

void drawText(const char *text, int x, int y, int fontSize) {
    DrawText(text, x, y, fontSize, COLOR_TEXT);
}

void drawTitle(const char *text, int x, int y) {
    DrawText(text, x, y, 44, COLOR_TEXT);
}

int drawButton(const char *text, int x, int y, int width, int height) {
    Vector2 mousePos = GetMousePosition();
    
    int isHovered = (mousePos.x >= x && mousePos.x <= x + width &&
                     mousePos.y >= y && mousePos.y <= y + height);
    
    Color buttonColor = isHovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
    DrawRectangle(x, y, width, height, buttonColor);
    DrawRectangleLines(x, y, width, height, COLOR_TEXT);
    
    int textWidth = MeasureText(text, 20);
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - 20) / 2;
    DrawText(text, textX, textY, 20, COLOR_TEXT);
    
    if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return 1;
    }
    
    return 0;
}

void drawGameLogo(void) {
    const char *title = "BATTLESHIP GAME";
    int textWidth = MeasureText(title, 60);
    DrawText(title, (WINDOW_WIDTH - textWidth) / 2, 40, 60, COLOR_TEXT);
}

void drawShipInfo(Player_t *player, int selectedShip) {
    int infoPanelX = WINDOW_WIDTH - 300;
    int infoPanelY = 150;
    
    DrawText("SHIPS INFO", infoPanelX, infoPanelY, 20, COLOR_TEXT);
    DrawLine(infoPanelX, infoPanelY + 25, infoPanelX + 280, infoPanelY + 25, COLOR_GRID);
    
    int shipLengths[NUM_SHIPS] = {5, 4, 3, 2};
    
    for (int i = 0; i < NUM_SHIPS; i++) {
        int y = infoPanelY + 40 + i * 50;
        Color shipCol = getShipColor(shipLengths[i], (i == selectedShip));
        
        DrawRectangle(infoPanelX, y, 20, 20, shipCol);
        DrawRectangleLines(infoPanelX, y, 20, 20, COLOR_TEXT);
        
        char shipInfo[64];
        snprintf(shipInfo, sizeof(shipInfo), "Ship %d: Len=%d", i + 1, shipLengths[i]);
        DrawText(shipInfo, infoPanelX + 30, y, 16, COLOR_TEXT);
        
        char posInfo[64];
        snprintf(posInfo, sizeof(posInfo), "Pos:(%d,%d) %s", 
                 player->ships[i].row, player->ships[i].col,
                 player->ships[i].dir == HORIZONTAL ? "H" : "V");
        DrawText(posInfo, infoPanelX + 30, y + 18, 14, COLOR_SECONDARY);
    }
    
    int ctrlY = infoPanelY + 250;
    DrawText("CONTROLS:", infoPanelX, ctrlY, 16, COLOR_TEXT);
    DrawText("Click ship to select", infoPanelX, ctrlY + 25, 12, COLOR_SECONDARY);
    DrawText("R: Rotate", infoPanelX, ctrlY + 42, 12, COLOR_SECONDARY);
    DrawText("Drag: Move ship", infoPanelX, ctrlY + 59, 12, COLOR_SECONDARY);
}

void drawShipsWithHighlight(Player_t *player, int playerIndex, int selectedShip) {
    int boardStartX, boardStartY;
    getBoardPosition(playerIndex, &boardStartX, &boardStartY);
    
    DrawText(player->name, boardStartX, boardStartY, PLAYER_NAME_FONT_SIZE, COLOR_TEXT);
    
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            int x = boardStartX + j * (CELL_SIZE + CELL_GAP);
            int y = boardStartY + PLAYER_NAME_FONT_SIZE + NAME_PADDING + i * (CELL_SIZE + CELL_GAP);
            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, COLOR_WATER);
            DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, COLOR_GRID);
        }
    }
    
    for (int shipIdx = 0; shipIdx < NUM_SHIPS; shipIdx++) {
        Ship_t *ship = &player->ships[shipIdx];
        Color shipColor = getShipColor(ship->length, (shipIdx == selectedShip));
        
        if (ship->dir == HORIZONTAL) {
            for (int i = 0; i < ship->length; i++) {
                int x = boardStartX + (ship->col + i) * (CELL_SIZE + CELL_GAP);
                int y = boardStartY + PLAYER_NAME_FONT_SIZE + NAME_PADDING + ship->row * (CELL_SIZE + CELL_GAP);
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, shipColor);
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, COLOR_GRID);
            }
        } else {
            for (int i = 0; i < ship->length; i++) {
                int x = boardStartX + ship->col * (CELL_SIZE + CELL_GAP);
                int y = boardStartY + PLAYER_NAME_FONT_SIZE + NAME_PADDING + (ship->row + i) * (CELL_SIZE + CELL_GAP);
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, shipColor);
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, COLOR_GRID);
            }
        }
    }
}

void getMousePosition(int *x, int *y) {
    Vector2 mousePos = GetMousePosition();
    *x = (int)mousePos.x;
    *y = (int)mousePos.y;
}

int getCellClick(int playerIndex, int *row, int *col) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return 0;
    }
    
    int boardStartX, boardStartY;
    getBoardPosition(playerIndex, &boardStartX, &boardStartY);
    
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    
    return pixelToBoardCell(mouseX, mouseY, boardStartX, boardStartY, row, col);
}

int getTextInput(char *buffer, int maxLength) {
    textInputMode = 1;
    inputIndex = 0;
    memset(inputBuffer, 0, PLAYER_NAME_SIZE + 1);
    
    while (textInputMode && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);
        
        DrawText("Enter your name:", 50, 100, 40, COLOR_TEXT);
        DrawRectangleLines(50, 180, 400, 50, COLOR_TEXT);
        DrawText(inputBuffer, 60, 190, 30, COLOR_TEXT);

        DrawLine(60 + MeasureText(inputBuffer, 30), 190, 60 + MeasureText(inputBuffer, 30), 220, COLOR_TEXT);
        
        int key = GetCharPressed();
        while (key > 0 && inputIndex < maxLength - 1) {
            if ((key >= 32 && key <= 125) && inputIndex < maxLength - 1) {
                inputBuffer[inputIndex] = (char)key;
                inputIndex++;
                inputBuffer[inputIndex] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && inputIndex > 0) {
            inputIndex--;
            inputBuffer[inputIndex] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER) && inputIndex > 0) {
            strcpy(buffer, inputBuffer);
            textInputMode = 0;
            EndDrawing();
            return 1;
        }
        
        EndDrawing();
    }
    
    return 0;
}

void drawFrame(Game_t *game) {
    beginFrame();
    
    int boardStartX1, boardStartY1;
    int boardStartX2, boardStartY2;
    getBoardPosition(0, &boardStartX1, &boardStartY1);
    getBoardPosition(1, &boardStartX2, &boardStartY2);
    
    switch (game->state) {
        case GAME_STATE_PLAYER1_SETUP: {
            drawTitle("PLAYER 1 - SETUP", 50, 30);
            drawText("Place and arrange your ships", 50, 80, 18);
            drawText("Click to select, R to rotate, drag to move", 50, 100, 16);
            
            drawShipsWithHighlight(&game->player1, 0, game->selected_ship);
            drawShipInfo(&game->player1, game->selected_ship);
            
            int ready1 = drawButton("READY", WINDOW_WIDTH - 150, WINDOW_HEIGHT - 70, 120, 50);
            if (ready1) {
                game->setup_complete[0] = 1;
                game->state = GAME_STATE_PLAYER2_SETUP;
                game->selected_ship = -1;
            }
            break;
        }
        
        case GAME_STATE_PLAYER2_SETUP: {
            drawTitle("PLAYER 2 - SETUP", 50, 30);
            drawText("Place and arrange your ships", 50, 80, 18);
            drawText("Click to select, R to rotate, drag to move", 50, 100, 16);
            
            drawShipsWithHighlight(&game->player2, 0, game->selected_ship);
            drawShipInfo(&game->player2, game->selected_ship);
            
            int ready2 = drawButton("READY", WINDOW_WIDTH - 150, WINDOW_HEIGHT - 70, 120, 50);
            if (ready2) {
                game->setup_complete[1] = 1;
                game->state = GAME_STATE_PLAYING;
                game->current_player = 1;
                game->selected_ship = -1;
            }
            break;
        }
        
        case GAME_STATE_PLAYING: {
            if (game->current_player == 1) {
                drawTitle("Player 1's Turn - ATTACK!", 50, 30);
                drawText("Your Board:", 50, 90, 20);
                drawPlayerBoard(&game->player1, 0, 1);
                
                drawText("Enemy Board (Click to Attack):", WINDOW_WIDTH / 2 + 50, 90, 20);
                drawPlayerBoard(&game->player2, 1, 0);
            } else {
                drawTitle("Player 2's Turn - ATTACK!", 50, 30);
                drawText("Your Board:", 50, 90, 20);
                drawPlayerBoard(&game->player2, 0, 1);
                
                drawText("Enemy Board (Click to Attack):", WINDOW_WIDTH / 2 + 50, 90, 20);
                drawPlayerBoard(&game->player1, 1, 0);
            }
            break;
        }
        
        case GAME_STATE_GAME_OVER: {
            drawTitle("GAME OVER!", 50, 50);
            drawText("Player 1 Wins! Congratulations!", 50, 150, 32);
            if (drawButton("Play Again", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 100, 200, 50)) {
                initGame(game);
            }
            break;
        }
    }
    
    endFrame();
}