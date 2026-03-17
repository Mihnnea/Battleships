#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "utils.h"
#include "player.h"

#define WATER "▒▒"
#define SOLID "██"

#define C_BLACK "\033[0;30m"
#define C_RED "\033[0;31m"
#define C_GREEN "\033[0;32m"
#define C_YELLOW "\033[0;33m"
#define C_BLUE "\033[0;34m"
#define C_PURPLE "\033[0;35m"
#define C_CYAN "\033[0;36m"
#define C_WHITE "\033[0;37m"

#define C_RESET "\033[0m"

void printMap(Player_t *player) {
    system("clear");
    
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            if(player->battleships[i][j] == ship)
                printf("%s%s%s", C_GREEN, SOLID, C_RESET);

            else if(player->battleships[i][j] == d_ship)
                printf("%s%s%s", C_RED, SOLID, C_RESET);

            else if(player->battleships[i][j] == d_water)
                printf("%s%s%s", C_BLACK, SOLID, C_RESET);

            else
                printf("%s%s%s", C_BLUE, WATER, C_RESET);
        }
        printf("\n");
    }
}