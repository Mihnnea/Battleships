#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "player.h"

void initPlayer(Player_t *player, const char *name) {
    strncpy(player->name, name, PLAYER_NAME_SIZE);
    memset(player->battleships, 0, sizeof(player->battleships));
    memset(player->attacked_battleships, 0, sizeof(player->attacked_battleships));
}