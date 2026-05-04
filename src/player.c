#include <string.h>
#include "player.h"

void player_init(Player *player, const char *name) {
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    board_init(&player->own_board);
}
