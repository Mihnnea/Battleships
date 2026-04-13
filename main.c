#include <stdio.h>
#include <raylib.h>

#include "ui.h"
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