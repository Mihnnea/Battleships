#pragma once

#define MAX_SHIPS 5

typedef struct {
    char name[20];
    int size;
    int hits;
    int sunk;
} Ship;

void ship_init(Ship *ship, const char *name, int size);
int ship_hit(Ship *ship);
int ship_is_sunk(const Ship *ship); 