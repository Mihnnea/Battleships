#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "utils.h"

#define BLOCK "██"
#define C_BLUE "\033[0;34m"
#define C_RESET "\033[0m"

void printMap() {
    system("clear");
    for(int i = 0; i < MAP_SIZE; i++) {
        for(int j = 0; j < MAP_SIZE; j++) {
            printf("%s%s%s", C_BLUE, BLOCK, C_RESET);
        }
        printf("\n");
    }
}