#include <stdio.h>

#include "ui.h"
#include "player.h"

int main() {
    Player_t player1, player2;
    initPLayer(&player1, "Player 1");
    initPLayer(&player2, "Player 2");
    
    initWindow("Battleships");
    
    while (!windowShouldClose()) {
        drawFrame(&player1);
    }
    
    closeWindow();
    return 0;
}