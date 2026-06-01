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
static const Color GOLD_COLOR    = {255, 200, 50, 255};
static const Color PURPLE_COLOR  = {180, 80, 255, 255};
static const Color SHIELD_COLOR  = {0, 190, 255, 255};

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
        case STATE_P2_SHOP:
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
                                game->state = STATE_P1_SHOP;
                            } else {
                                game->state = STATE_P2_SHOP;
                            }
                        }
                    }
                }
            }
            break;
        }

        case STATE_P1_SHOP:
        case STATE_P2_SHOP: {
            int pi = current_player_index(game);
            Player *player = &game->players[pi];
            Board *board = &player->own_board;

            // Handle Board Clicks for Decoy/Shield Placement
            if(game->shop_selected_powerup == 1 || game->shop_selected_powerup == 3) {
                int hr, hc;
                if(gui_get_hovered_cell(CENTER_BOARD_X, CENTER_BOARD_Y, &hr, &hc)) {
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if(game->shop_selected_powerup == 1) { // Decoy
                            if(board->grid[hr][hc] == CELL_EMPTY && board->decoy_grid[hr][hc] == 0) {
                                board->decoy_grid[hr][hc] = 1;
                                player->tactical_points -= 2;
                                game->shop_selected_powerup = 0; // Deselect after placing
                            }
                        } else if(game->shop_selected_powerup == 3) { // Naval Shield
                            if(board->grid[hr][hc] == CELL_SHIP && board->shield_grid[hr][hc] == 0) {
                                board->shield_grid[hr][hc] = 1;
                                player->tactical_points -= 4;
                                game->shop_selected_powerup = 0; // Deselect after placing
                            }
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
            if(game->sonar_message_timer > 0.0f) {
                game->sonar_message_timer -= dt;
                if(game->sonar_message_timer < 0.0f) {
                    game->sonar_message_timer = 0.0f;
                }
            }

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
            Player *player = &game->players[pi];
            int opponent  = 1 - pi;
            Board *target = &game->players[opponent].own_board;
            Board *view   = &game->attack_boards[pi];

            // R key toggles sonar mode when active
            if(game->active_powerup == 1 && IsKeyPressed(KEY_R)) {
                game->sonar_scan_mode = !game->sonar_scan_mode;
            }

            int hr, hc;
            if(gui_get_hovered_cell(RIGHT_BOARD_X, RIGHT_BOARD_Y, &hr, &hc)) {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if(game->active_powerup == 1) { // Sonar Scan
                        if(game->sonar_scan_mode == 0) { // Row Scan
                            int found = board_scan_row(target, hr);
                            snprintf(game->sonar_message, sizeof(game->sonar_message), "Row %d Scan: %s", hr + 1, found ? "YES (Ships Detected!)" : "NO (All Clear)");
                        } else { // Column Scan
                            int found = board_scan_col(target, hc);
                            snprintf(game->sonar_message, sizeof(game->sonar_message), "Column %c Scan: %s", 'A' + hc, found ? "YES (Ships Detected!)" : "NO (All Clear)");
                        }
                        player->sonar_pings--;
                        game->sonar_message_timer = 2.5f;
                        game->active_powerup = 0; // Clear active powerup, player still gets their attack!
                    }
                    else if(game->active_powerup == 2) { // Carpet Bomb
                        // Strike a 3x3 grid centered at (hr, hc)
                        for(int dr = -1; dr <= 1; dr++) {
                            for(int dc = -1; dc <= 1; dc++) {
                                int r = hr + dr;
                                int c = hc + dc;
                                if(r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                                    int res = board_attack(target, r, c);
                                    if(res >= 0) {
                                        if(res == 1 || res == 3) {
                                            view->grid[r][c] = CELL_HIT;
                                        } else if(res == 0) {
                                            view->grid[r][c] = CELL_MISS;
                                        } else if(res == 2) {
                                            view->grid[r][c] = CELL_EMPTY; // Keep empty (shield blocked)
                                        }
                                    }
                                }
                            }
                        }
                        player->carpet_bombs--;
                        game->last_result = 4; // Carpet Bomb result code
                        game->result_timer = 1.5f;
                        game->active_powerup = 0;

                        if(board_all_sunk(target))
                            game->winner = pi;
                    }
                    else { // Normal Attack
                        int result = board_attack(target, hr, hc);
                        if(result >= 0) {
                            if(result == 1) {
                                view->grid[hr][hc] = CELL_HIT;
                            } else if(result == 0) {
                                view->grid[hr][hc] = CELL_MISS;
                            } else if(result == 2) {
                                view->grid[hr][hc] = CELL_EMPTY; // Blocked by shield, stays empty for next turn
                            } else if(result == 3) {
                                view->grid[hr][hc] = CELL_HIT; // Decoy tripped, attacker sees it as a Hit!
                            }
                            
                            game->last_result  = result;
                            game->result_timer = 1.2f;

                            if(board_all_sunk(target))
                                game->winner = pi;
                        }
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

        case STATE_P1_SHOP:
        case STATE_P2_SHOP: {
            int pi = current_player_index(game);
            Player *player = &game->players[pi];
            Board *board = &player->own_board;
            Color pc = (pi == 0) ? P1_COLOR : P2_COLOR;

            char title[64];
            snprintf(title, sizeof(title), "Player %d - Tactical Shop", pi + 1);
            int tw = gui_measure_text(title, 32);
            gui_draw_text(title, (WINDOW_WIDTH - tw) / 2, 20, 32, pc);

            // Draw Board in Center
            gui_draw_board_at(board, CENTER_BOARD_X, CENTER_BOARD_Y, 0);

            // Draw active decoys and shields
            for(int r = 0; r < BOARD_SIZE; r++) {
                for(int c = 0; c < BOARD_SIZE; c++) {
                    int x = CENTER_BOARD_X + LABEL_SIZE + c * CELL_SIZE;
                    int y = CENTER_BOARD_Y + LABEL_SIZE + r * CELL_SIZE;
                    if(board->decoy_grid[r][c] == 1) {
                        DrawCircle(x + CELL_SIZE/2, y + CELL_SIZE/2, 10, PURPLE_COLOR);
                        gui_draw_text("D", x + 15, y + 12, 16, WHITE);
                    }
                    if(board->shield_grid[r][c] == 1) {
                        DrawRectangleLines(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4, SHIELD_COLOR);
                        DrawRectangle(x + 4, y + 4, CELL_SIZE - 8, CELL_SIZE - 8, (Color){0, 190, 255, 60});
                    }
                }
            }

            // Draw hover preview
            if(game->shop_selected_powerup == 1 || game->shop_selected_powerup == 3) {
                int hr, hc;
                if(gui_get_hovered_cell(CENTER_BOARD_X, CENTER_BOARD_Y, &hr, &hc)) {
                    int px = CENTER_BOARD_X + LABEL_SIZE + hc * CELL_SIZE;
                    int py = CENTER_BOARD_Y + LABEL_SIZE + hr * CELL_SIZE;
                    if(game->shop_selected_powerup == 1 && board->grid[hr][hc] == CELL_EMPTY) {
                        DrawCircle(px + CELL_SIZE/2, py + CELL_SIZE/2, 10, (Color){180, 80, 255, 120});
                    } else if(game->shop_selected_powerup == 3 && board->grid[hr][hc] == CELL_SHIP) {
                        DrawRectangleLines(px + 2, py + 2, CELL_SIZE - 4, CELL_SIZE - 4, SHIELD_COLOR);
                        DrawRectangle(px + 4, py + 4, CELL_SIZE - 8, CELL_SIZE - 8, (Color){0, 190, 255, 100});
                    }
                }
            }

            // Draw Sidebar background
            DrawRectangle(840, 130, 310, 480, (Color){20, 35, 65, 255});
            DrawRectangleLines(840, 130, 310, 480, (Color){40, 80, 150, 255});

            gui_draw_text("TACTICAL SHOP", 860, 150, 22, TITLE_COLOR);

            char points_text[32];
            snprintf(points_text, sizeof(points_text), "Tactical Points: %d", player->tactical_points);
            gui_draw_text(points_text, 860, 185, 18, GOLD_COLOR);

            // Draw sidebar buttons
            if(gui_draw_button("DECOY (2 pts)", 855, 220, 280, 42)) {
                if(player->tactical_points >= 2) {
                    game->shop_selected_powerup = 1;
                }
            }
            if(gui_draw_button("SONAR PING (3 pts)", 855, 270, 280, 42)) {
                if(player->tactical_points >= 3) {
                    player->tactical_points -= 3;
                    player->sonar_pings++;
                    game->shop_selected_powerup = 0;
                }
            }
            if(gui_draw_button("NAVAL SHIELD (4 pts)", 855, 320, 280, 42)) {
                if(player->tactical_points >= 4) {
                    game->shop_selected_powerup = 3;
                }
            }
            if(gui_draw_button("CARPET BOMB (6 pts)", 855, 370, 280, 42)) {
                if(player->tactical_points >= 6) {
                    player->tactical_points -= 6;
                    player->carpet_bombs++;
                    game->shop_selected_powerup = 0;
                }
            }

            // Inventories text inside sidebar
            char inv_text[64];
            snprintf(inv_text, sizeof(inv_text), "Sonar Pings: %d | Carpet Bombs: %d", player->sonar_pings, player->carpet_bombs);
            gui_draw_text(inv_text, 860, 430, 15, DIM_COLOR);

            // Selection indicator
            if(game->shop_selected_powerup == 1) {
                gui_draw_text("Selected: Decoy", 860, 460, 16, PURPLE_COLOR);
                gui_draw_text("Click empty tile on board", 860, 480, 14, TEXT_COLOR);
            } else if(game->shop_selected_powerup == 3) {
                gui_draw_text("Selected: Naval Shield", 860, 460, 16, SHIELD_COLOR);
                gui_draw_text("Click ship tile on board", 860, 480, 14, TEXT_COLOR);
            } else {
                gui_draw_text("Select placement power-up", 860, 460, 14, DIM_COLOR);
            }

            if(gui_draw_button("CONFIRM & LAUNCH", 855, 540, 280, 50)) {
                game->shop_selected_powerup = 0;
                if(game->state == STATE_P1_SHOP) {
                    game->transition_player = 2;
                    game->next_state = STATE_P2_PLACE;
                } else {
                    game->transition_player = 1;
                    game->next_state = STATE_P1_ATTACK;
                }
                game->state = STATE_TRANSITION;
            }
            break;
        }

        case STATE_P1_ATTACK:
        case STATE_P2_ATTACK: {
            int pi = current_player_index(game);
            int opponent = 1 - pi;
            Player *own_player = &game->players[pi];
            Board *own = &own_player->own_board;
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

            // Draw shields and decoys on own board (left board)
            for(int r = 0; r < BOARD_SIZE; r++) {
                for(int c = 0; c < BOARD_SIZE; c++) {
                    int x = LEFT_BOARD_X + LABEL_SIZE + c * CELL_SIZE;
                    int y = LEFT_BOARD_Y + LABEL_SIZE + r * CELL_SIZE;
                    if(own->decoy_grid[r][c] == 1) {
                        DrawCircle(x + CELL_SIZE/2, y + CELL_SIZE/2, 8, PURPLE_COLOR);
                        gui_draw_text("D", x + 15, y + 12, 16, WHITE);
                    }
                    if(own->shield_grid[r][c] == 1) {
                        DrawRectangleLines(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4, SHIELD_COLOR);
                        DrawRectangle(x + 4, y + 4, CELL_SIZE - 8, CELL_SIZE - 8, (Color){0, 190, 255, 60});
                    }
                }
            }

            // Draw Hover Effects on Enemy Board
            if(game->result_timer <= 0.0f) {
                int hr, hc;
                if(gui_get_hovered_cell(RIGHT_BOARD_X, RIGHT_BOARD_Y, &hr, &hc)) {
                    if(game->active_powerup == 1) { // Sonar Scan Hover
                        int px = RIGHT_BOARD_X + LABEL_SIZE;
                        int py = RIGHT_BOARD_Y + LABEL_SIZE;
                        if(game->sonar_scan_mode == 0) { // Row
                            DrawRectangle(px, py + hr * CELL_SIZE, BOARD_SIZE * CELL_SIZE, CELL_SIZE, (Color){255, 200, 50, 80});
                        } else { // Column
                            DrawRectangle(px + hc * CELL_SIZE, py, CELL_SIZE, BOARD_SIZE * CELL_SIZE, (Color){255, 200, 50, 80});
                        }
                    } else if(game->active_powerup == 2) { // Carpet Bomb Hover
                        for(int dr = -1; dr <= 1; dr++) {
                            for(int dc = -1; dc <= 1; dc++) {
                                int r = hr + dr;
                                int c = hc + dc;
                                if(r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                                    gui_draw_hover_cell(RIGHT_BOARD_X, RIGHT_BOARD_Y, r, c, 255, 100, 0, 120);
                                }
                            }
                        }
                    } else { // Normal Hover
                        if(view->grid[hr][hc] == CELL_EMPTY) {
                            gui_draw_hover_cell(RIGHT_BOARD_X, RIGHT_BOARD_Y, hr, hc, 50, 140, 220, 100);
                        }
                    }
                }
            }

            // Draw active Sonar Message banner
            if(game->sonar_message_timer > 0.0f) {
                int sw = gui_measure_text(game->sonar_message, 24);
                DrawRectangle((WINDOW_WIDTH - sw - 40) / 2, WINDOW_HEIGHT - 120, sw + 40, 45, (Color){10, 15, 30, 220});
                DrawRectangleLines((WINDOW_WIDTH - sw - 40) / 2, WINDOW_HEIGHT - 120, sw + 40, 45, GOLD_COLOR);
                gui_draw_text(game->sonar_message, (WINDOW_WIDTH - sw) / 2, WINDOW_HEIGHT - 110, 24, GOLD_COLOR);
            }

            if(game->result_timer > 0.0f) {
                const char *rtxt;
                Color rcol;
                if(game->last_result == 1) {
                    rtxt = "HIT!";
                    rcol = HIT_TEXT;
                } else if(game->last_result == 2) {
                    rtxt = "SHIELD BLOCKED!";
                    rcol = SHIELD_COLOR;
                } else if(game->last_result == 3) {
                    rtxt = "HIT!"; // Fool the attacker
                    rcol = HIT_TEXT;
                } else if(game->last_result == 4) {
                    rtxt = "CARPET BOMB FIRED!";
                    rcol = (Color){255, 150, 50, 255};
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

            // Draw Enemy Ships List in Center
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

            // Draw Tactical Arsenal in Center
            DrawRectangle(490, 320, 220, 310, (Color){15, 25, 50, 255});
            DrawRectangleLines(490, 320, 220, 310, (Color){40, 80, 150, 255});

            gui_draw_text("TACTICAL ARSENAL", 505, 335, 18, TITLE_COLOR);

            char btn_sonar[32];
            snprintf(btn_sonar, sizeof(btn_sonar), "Sonar Ping (%d)", own_player->sonar_pings);
            if(gui_draw_button(btn_sonar, 500, 370, 200, 40)) {
                if(own_player->sonar_pings > 0 && game->result_timer <= 0.0f) {
                    if(game->active_powerup == 1) {
                        game->active_powerup = 0;
                    } else {
                        game->active_powerup = 1;
                    }
                }
            }

            char btn_bomb[32];
            snprintf(btn_bomb, sizeof(btn_bomb), "Carpet Bomb (%d)", own_player->carpet_bombs);
            if(gui_draw_button(btn_bomb, 500, 420, 200, 40)) {
                if(own_player->carpet_bombs > 0 && game->result_timer <= 0.0f) {
                    if(game->active_powerup == 2) {
                        game->active_powerup = 0;
                    } else {
                        game->active_powerup = 2;
                    }
                }
            }

            if(game->active_powerup == 1) {
                gui_draw_text("ACTIVE: SONAR PING", 505, 475, 14, GOLD_COLOR);
                const char *mode_str = game->sonar_scan_mode == 0 ? "ROW SCAN (R to toggle)" : "COLUMN SCAN (R to toggle)";
                gui_draw_text(mode_str, 505, 495, 12, TEXT_COLOR);

                if(gui_draw_button("TOGGLE ROW/COL", 500, 520, 200, 32)) {
                    game->sonar_scan_mode = !game->sonar_scan_mode;
                }
                if(gui_draw_button("CANCEL", 500, 560, 200, 32)) {
                    game->active_powerup = 0;
                }
            } else if(game->active_powerup == 2) {
                gui_draw_text("ACTIVE: CARPET BOMB", 505, 475, 14, (Color){255, 100, 50, 255});
                gui_draw_text("Click target for 3x3 strike", 505, 495, 12, TEXT_COLOR);

                if(gui_draw_button("CANCEL", 500, 545, 200, 35)) {
                    game->active_powerup = 0;
                }
            } else {
                gui_draw_text("Select a tactical", 505, 475, 14, DIM_COLOR);
                gui_draw_text("power-up to launch", 505, 495, 14, DIM_COLOR);
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