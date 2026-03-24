#pragma once
#include "player.h"

void initWindow(const char *title);
int windowShouldClose(void);
void closeWindow(void);

void drawFrame(Player_t *player);