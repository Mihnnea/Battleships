#include <string.h>
#include "player.h"

void player_init(Player *player, const char *name) {
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    board_init(&player->own_board);
    player->tactical_points = 10;
    player->sonar_pings = 0;
    player->carpet_bombs = 0;
}
