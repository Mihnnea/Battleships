#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "player.h"

#define PLAYER_NAME_SIZE 4

struct Player {
    char name[PLAYER_NAME_SIZE + 1];
    int battleships[MAP_SIZE][MAP_SIZE];
};

Player_t *initPLayer(char *name) {
    Player_t *player = malloc(sizeof(Player_t));

    strncpy(player->name, name, PLAYER_NAME_SIZE);
    memset(player->battleships, 0, sizeof(player->battleships));

    return player;
}

void destroyPlayer(Player_t *player) {
    if(player != NULL)
        free(player);
}