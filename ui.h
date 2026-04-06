#pragma once
#include "game.h"

void initWindow(const char *title);
int windowShouldClose(void);
void closeWindow(void);

void drawFrame(Game_t *game);