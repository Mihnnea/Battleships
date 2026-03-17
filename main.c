#include <stdio.h>

#include "ui.h"
#include "player.h"

int main() {
    printMap();
    Player_t *player1 = initPLayer("Player 1");
    Player_t *player2 = initPLayer("Player 2");

    destroyPlayer(player1);
    destroyPlayer(player2);
    return 0;
}