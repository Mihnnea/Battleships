#pragma once
#include "game.h"

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 900
#define TARGET_FPS 60

#define CELL_SIZE 40
#define CELL_GAP 8
#define PLAYER_NAME_FONT_SIZE 24
#define NAME_PADDING 20
#define BOARD_OFFSET_X 50

#define COLOR_BACKGROUND (Color){20, 20, 30, 255}
#define COLOR_WATER (Color){30, 144, 255, 255}
#define COLOR_WATER_ATTACKED (Color){100, 149, 237, 255}
#define COLOR_SHIP (Color){34, 139, 34, 255}
#define COLOR_SHIP_HIGHLIGHTED (Color){50, 200, 50, 255}
#define COLOR_DESTROYED_SHIP (Color){220, 20, 60, 255}
#define COLOR_DESTROYED_WATER (Color){50, 50, 50, 255}
#define COLOR_GRID (Color){80, 80, 100, 255}
#define COLOR_TEXT (Color){255, 255, 255, 255}
#define COLOR_SECONDARY (Color){200, 200, 220, 255}
#define COLOR_BUTTON (Color){70, 130, 180, 255}
#define COLOR_BUTTON_HOVER (Color){100, 160, 210, 255}
#define COLOR_ERROR (Color){255, 100, 100, 255}

void initWindow(const char *title);
int windowShouldClose(void);
void closeWindow(void);

void beginFrame(void);
void endFrame(void);
void drawFrame(Game_t *game);

void drawCell(int row, int col, int cellType, int boardStartX, int boardStartY);

void drawPlayerBoard(Player_t *player, int playerIndex, int reveal);

void drawText(const char *text, int x, int y, int fontSize);

int drawButton(const char *text, int x, int y, int width, int height);

void drawTitle(const char *text, int x, int y);

void drawShipInfo(Player_t *player, int selectedShip);

void drawShipsWithHighlight(Player_t *player, int playerIndex, int selectedShip);

void getMousePosition(int *x, int *y);

int getCellClick(int playerIndex, int *row, int *col);

int getTextInput(char *buffer, int maxLength);

void drawGameLogo(void);
void clearScreen(void);

void getBoardPosition(int playerIndex, int *startX, int *startY);