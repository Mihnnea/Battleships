#include <stdio.h>

#include "ui.h"
#include "player.h"
#include "game.h"

int main() {
    Game_t game;
    initGame(&game);

    initWindow("Battleships");
    
    while (!windowShouldClose()) {
        gameUpdate(&game);
        drawFrame(&game);
    }
    
    closeWindow();
    return 0;
}