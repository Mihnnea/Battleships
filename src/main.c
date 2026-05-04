#include "raylib.h"
#include "game.h"
#include "gui.h"

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Battleships");
    SetTargetFPS(60);
    gui_init();

    Game game;
    game_init(&game);

    while(!WindowShouldClose()) {
        game_update(&game);
        game_draw(&game);
    }

    gui_cleanup();
    CloseWindow();
    return 0;
}
