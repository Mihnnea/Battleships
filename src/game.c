#include "raylib.h"
#include "game.h"
#include "gui.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static const Color BG_COLOR     = {10, 20, 40, 255};
static const Color TITLE_COLOR  = {70, 190, 255, 255};
static const Color TEXT_COLOR   = {200, 220, 240, 255};
static const Color DIM_COLOR    = {90, 125, 160, 255};
static const Color HIT_TEXT     = {255, 90, 60, 255};
static const Color MISS_TEXT    = {130, 200, 230, 255};
static const Color P1_COLOR     = {70, 160, 255, 255};
static const Color P2_COLOR     = {255, 100, 80, 255};
static const Color SUNK_TEXT    = {93, 168, 89, 255};
static const Color ALIVE_TEXT    = {255, 200, 50, 255};

#define CENTER_BOARD_X  ((WINDOW_WIDTH  - BOARD_PX) / 2)
#define CENTER_BOARD_Y  (140)

#define LEFT_BOARD_X    (50)
#define LEFT_BOARD_Y    (100)
#define RIGHT_BOARD_X   (WINDOW_WIDTH - BOARD_PX - 50)
#define RIGHT_BOARD_Y   (100)

void game_init(Game *game) {
    memset(game, 0, sizeof(Game));

    player_init(&game->players[0], "Player 1");
    player_init(&game->players[1], "Player 2");
    board_init(&game->attack_boards[0]);
    board_init(&game->attack_boards[1]);

    game->state = STATE_MENU;
    game->next_state = STATE_MENU;
    game->placing_ship_idx = 0;
    game->place_horizontal = 1;
    game->last_result = -1;
    game->result_timer = 0.0f;
    game->winner = -1;
    game->transition_player = 0;

    game->ship_names[0] = "Oblivion";
    game->ship_names[1] = "Leviathan";
    game->ship_names[2] = "Imperator";
    game->ship_names[3] = "Bastion";
    game->ship_names[4] = "Talon";

    game->ship_sizes[0] = 5;
    game->ship_sizes[1] = 4;
    game->ship_sizes[2] = 3;
    game->ship_sizes[3] = 3;
    game->ship_sizes[4] = 2;
}

static int current_player_index(const Game *game) {
    switch(game->state) {
        case STATE_P2_PLACE:
        case STATE_P2_ATTACK: 
            return 1;
        default:
            return 0;
    }
}

void game_update(Game *game) {
    float dt = GetFrameTime();

    switch(game->state) {
        case STATE_MENU:
            break;

        case STATE_P1_PLACE:
        case STATE_P2_PLACE: {
            int pi = current_player_index(game);
            Board *board = &game->players[pi].own_board;

            if(IsKeyPressed(KEY_R))
                game->place_horizontal = !game->place_horizontal;

            int hr, hc;
            if(gui_get_hovered_cell(CENTER_BOARD_X, CENTER_BOARD_Y, &hr, &hc)) {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    int idx = game->placing_ship_idx;
                    int dir = game->place_horizontal ? 0 : 1;
                    int size = game->ship_sizes[idx];
                    const char *name = game->ship_names[idx];

                    if(board_place_ship(board, name, size, hr, hc, dir)) {
                        game->placing_ship_idx++;
                        if(game->placing_ship_idx >= MAX_SHIPS) {
                            game->placing_ship_idx = 0;
                            if(game->state == STATE_P1_PLACE) {
                                game->transition_player = 2;
                                game->next_state = STATE_P2_PLACE;
                            } else {
                                game->transition_player = 1;
                                game->next_state = STATE_P1_ATTACK;
                            }
                            game->state = STATE_TRANSITION;
                        }
                    }
                }
            }
            break;
        }

        case STATE_TRANSITION:
            if(IsKeyPressed(KEY_SPACE))
                game->state = game->next_state;
            break;

        case STATE_P1_ATTACK:
        case STATE_P2_ATTACK: {
            if(game->result_timer > 0.0f) {
                game->result_timer -= dt;
                if(game->result_timer <= 0.0f) {
                    game->result_timer = 0.0f;
                    game->last_result  = -1;

                    if(game->winner >= 0) {
                        game->state = STATE_GAME_OVER;
                    } else {
                        if(game->state == STATE_P1_ATTACK) {
                            game->transition_player = 2;
                            game->next_state = STATE_P2_ATTACK;
                        } else {
                            game->transition_player = 1;
                            game->next_state = STATE_P1_ATTACK;
                        }
                        game->state = STATE_TRANSITION;
                    }
                }
                break;
            }

            int pi = current_player_index(game);
            int opponent  = 1 - pi;
            Board *target = &game->players[opponent].own_board;
            Board *view   = &game->attack_boards[pi];

            int hr, hc;
            if(gui_get_hovered_cell(RIGHT_BOARD_X, RIGHT_BOARD_Y, &hr, &hc)) {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    int result = board_attack(target, hr, hc);
                    if(result >= 0) {
                        view->grid[hr][hc] = (result == 1) ? CELL_HIT : CELL_MISS;
                        game->last_result  = result;
                        game->result_timer = 1.2f;

                        if(board_all_sunk(target))
                            game->winner = pi;
                    }
                }
            }
            break;
        }

        case STATE_GAME_OVER:
            break;
    }
}

void game_draw(Game *game) {
    BeginDrawing();
    ClearBackground(BG_COLOR);

    switch(game->state) {

        case STATE_MENU: {
            const char *title = "BATTLESHIPS";
            int tw = gui_measure_title(title, 96);
            gui_draw_title(title, (WINDOW_WIDTH - tw) / 2, 180, 96, TITLE_COLOR);

            const char *sub = "Two-Player Naval Warfare";
            int sw = gui_measure_text(sub, 24);
            gui_draw_text(sub, (WINDOW_WIDTH - sw) / 2, 280, 24, DIM_COLOR);

            DrawRectangle((WINDOW_WIDTH - 300) / 2, 320, 300, 2, (Color){40, 80, 130, 200});

            if(gui_draw_button("START GAME", (WINDOW_WIDTH - 220) / 2, 360, 220, 55)) {
                game_init(game);
                game->state = STATE_P1_PLACE;
            }

            const char *hint = "Left Click: place / fire  |  R: rotate ship  |  Space: continue";
            int hw = gui_measure_text(hint, 24);
            gui_draw_text(hint, (WINDOW_WIDTH - hw) / 2, WINDOW_HEIGHT - 50, 24, DIM_COLOR);
            break;
        }

        case STATE_P1_PLACE:
        case STATE_P2_PLACE: {
            int pi = current_player_index(game);
            Board *board = &game->players[pi].own_board;
            Color pc = (pi == 0) ? P1_COLOR : P2_COLOR;

            char title[64];
            snprintf(title, sizeof(title), "Player %d - Place Your Ships", pi + 1);
            int tw = gui_measure_text(title, 32);
            gui_draw_text(title, (WINDOW_WIDTH - tw) / 2, 20, 32, pc);

            int idx  = game->placing_ship_idx;
            char info[64];
            snprintf(info, sizeof(info), "%s  (size: %d)  [%d / %d]", game->ship_names[idx], game->ship_sizes[idx], idx + 1, MAX_SHIPS);
            int iw = gui_measure_text(info, 22);
            gui_draw_text(info, (WINDOW_WIDTH - iw) / 2, 65, 22, TEXT_COLOR);

            const char *rot = game->place_horizontal ? "Horizontal  (press R to rotate)" : "Vertical  (press R to rotate)";
            int rw = gui_measure_text(rot, 18);
            gui_draw_text(rot, (WINDOW_WIDTH - rw) / 2, 95, 18, DIM_COLOR);

            gui_draw_board_at(board, CENTER_BOARD_X, CENTER_BOARD_Y, 0);

            int hr, hc;
            if (gui_get_hovered_cell(CENTER_BOARD_X, CENTER_BOARD_Y, &hr, &hc)) {
                gui_draw_ship_preview(CENTER_BOARD_X, CENTER_BOARD_Y, board, hr, hc, game->ship_sizes[idx], game->place_horizontal);
            }
            break;
        }

        case STATE_TRANSITION: {
            char msg[64];
            snprintf(msg, sizeof(msg), "Pass the device to Player %d", game->transition_player);
            int mw = gui_measure_text(msg, 36);
            gui_draw_text(msg, (WINDOW_WIDTH - mw) / 2, WINDOW_HEIGHT / 2 - 60, 36, TEXT_COLOR);

            const char *cont = "Press SPACE to continue";
            int cw = gui_measure_text(cont, 22);

            float t = (float)GetTime();
            unsigned char alpha = (unsigned char)(128 + 127 * sinf(t * 3.0f));
            gui_draw_text(cont, (WINDOW_WIDTH - cw) / 2, WINDOW_HEIGHT / 2 + 10, 22, (Color){200, 220, 240, alpha});
            break;
        }

        case STATE_P1_ATTACK:
        case STATE_P2_ATTACK: {
            int pi = current_player_index(game);
            int opponent = 1 - pi;
            Board *own = &game->players[pi].own_board;
            Board *view = &game->attack_boards[pi];
            Color pc = (pi == 0) ? P1_COLOR : P2_COLOR;

            char title[64];
            snprintf(title, sizeof(title), "Player %d's Turn", pi + 1);
            int tw = gui_measure_text(title, 32);
            gui_draw_text(title, (WINDOW_WIDTH - tw) / 2, 15, 32, pc);

            const char *own_label = "Your Fleet";
            gui_draw_text(own_label, LEFT_BOARD_X + LABEL_SIZE + (BOARD_SIZE * CELL_SIZE - gui_measure_text(own_label, 20)) / 2, LEFT_BOARD_Y - 25, 20, DIM_COLOR);

            const char *atk_label = "Enemy Waters";
            gui_draw_text(atk_label, RIGHT_BOARD_X + LABEL_SIZE + (BOARD_SIZE * CELL_SIZE - gui_measure_text(atk_label, 20)) / 2, RIGHT_BOARD_Y - 25, 20, DIM_COLOR);

            gui_draw_board_at(own, LEFT_BOARD_X, LEFT_BOARD_Y, 0);
            gui_draw_board_at(view, RIGHT_BOARD_X, RIGHT_BOARD_Y, 0);

            if(game->result_timer <= 0.0f) {
                int hr, hc;
                if(gui_get_hovered_cell(RIGHT_BOARD_X, RIGHT_BOARD_Y, &hr, &hc)) {
                    if(view->grid[hr][hc] == CELL_EMPTY) {
                        gui_draw_hover_cell(RIGHT_BOARD_X, RIGHT_BOARD_Y, hr, hc, 50, 140, 220, 100);
                    }
                }
            }

            if(game->result_timer > 0.0f) {
                const char *rtxt;
                Color rcol;
                if(game->last_result == 1) {
                    rtxt = "HIT!";
                    rcol = HIT_TEXT;
                } else {
                    rtxt = "MISS";
                    rcol = MISS_TEXT;
                }
                int rw = gui_measure_text(rtxt, 52);
                float t = game->result_timer;
                float scale = 1.0f + 0.1f * sinf(t * 10.0f);
                int fs = (int)(52 * scale);
                rw = gui_measure_text(rtxt, fs);
                gui_draw_text(rtxt, (WINDOW_WIDTH - rw) / 2, WINDOW_HEIGHT - 100, fs, rcol);
            }

            int cx = WINDOW_WIDTH / 2;
            int sy = LEFT_BOARD_Y + LABEL_SIZE + 20;
            Board *tb = &game->players[opponent].own_board;

            const char *status_title = "Enemy Ships";
            int stw = gui_measure_text(status_title, 18);
            gui_draw_text(status_title, cx - stw / 2, sy - 5, 18, DIM_COLOR);
            sy += 25;

            for(int i = 0; i < tb->ship_count; i++) {
                Color sc = ship_is_sunk(&tb->ships[i]) ? SUNK_TEXT : ALIVE_TEXT;
                char line[40];
                snprintf(line, sizeof(line), "%s", game->ship_names[i]);
                int lw = gui_measure_text(line, 16);
                gui_draw_text(line, cx - lw / 2, sy, 16, sc);
                sy += 22;
            }
            
            break;
        }

        case STATE_GAME_OVER: {
            Color wc = (game->winner == 0) ? P1_COLOR : P2_COLOR;

            char msg[64];
            snprintf(msg, sizeof(msg), "Player %d Wins!", game->winner + 1);
            int mw = gui_measure_text(msg, 56);
            gui_draw_text(msg, (WINDOW_WIDTH - mw) / 2, 200, 56, wc);

            const char *sub = "All enemy ships destroyed!";
            int sw = gui_measure_text(sub, 24);
            gui_draw_text(sub, (WINDOW_WIDTH - sw) / 2, 275, 24, TEXT_COLOR);

            DrawRectangle((WINDOW_WIDTH - 300) / 2, 320, 300, 2, (Color){40, 80, 130, 200});

            if(gui_draw_button("PLAY AGAIN", (WINDOW_WIDTH - 220) / 2, 360, 220, 55)) {
                game_init(game);
                game->state = STATE_MENU;
            }
            break;
        }
    }

    EndDrawing();
}