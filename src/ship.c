#include <string.h>
#include "ship.h"

void ship_init(Ship *ship, const char *name, int size) {
    strncpy(ship->name, name, sizeof(ship->name) - 1);
    ship->name[sizeof(ship->name) - 1] = '\0';
    ship->size = size;
    ship->hits = 0;
    ship->sunk = 0;
}

int ship_hit(Ship *ship) {
    ship->hits++;
    if(ship->hits >= ship->size) {
        ship->sunk = 1;
        return 1;
    }
    return 0;
}

int ship_is_sunk(const Ship *ship) {
    return ship->sunk;
}
