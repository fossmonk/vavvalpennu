#include <vpconfig.h>
#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <game.h>
#include <menu.h>
#include <collisions.h>

// SOME CONSTANTS
#define VY_RISE_VEL_X_0        (-3E2)
#define VY_RISE_VEL_Y_0        (-2.5E2)
#define VY_RISE_VEL_X_1        (2.5E2)

#define ORB_RAND_CHANCE        ((rand() % 99 == 0))

#define AANA_RAND_CHANCE       ((rand() % 12283 == 0))
#define AANA_VEL_X             (-400.0f)

#define VY_BATR_HEALTH_DECR    (5)
#define PLAYER_ORB_HEALTH_DECR (20)

// SOME MACRO FUNCTIONS
#ifndef _max
#define _max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef _min
#define _min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define vec_equals(a, b) ((a).x == (b).x && (a).y == (b).y) ? true : false
#define vec_magnitude(v) ((float)sqrt((v).x * (v).x + (v).y * (v).y))

// GLOBALS
// top level game object
static game_t g;

Vector2 game_get_scaled_mouse_pos(void) {
    Vector2 mouseScalePos = GetMousePosition();
    mouseScalePos.x = (mouseScalePos.x / GetScreenWidth()) * G_W;
    mouseScalePos.y = (mouseScalePos.y / GetScreenHeight()) * G_H;
    return mouseScalePos;
}

void game_load_assets(void) {
    // background texture
    g.bg = LoadTexture(BACKGROUND);
    // splash screen texture
    g.splash = LoadTexture(SPLASH_BG);
    // pause menu texture
    g.pausemenu = LoadTexture(PAUSEMENU_BG);
    // fonts
    g.game_font = LoadFontEx(MAIN_FONT, 96, NULL, 0);
    SetTextureFilter(g.game_font.texture, TEXTURE_FILTER_BILINEAR);
    // animation assets
    anim_asset_load_all();
}

void game_init(RenderTexture2D *canvas) {
    // set canvas
    g.canvas = canvas;
    // kind of a hack, but makes stuff easy
    obj_global_set_cam2d(&g.cam);
    // load all assets
    game_load_assets();
    // initialize menu
    menu_init();
    // initialize player
    player_init(&g.p);
    // initialize vadayakshi
    vy_init(&g.vy);
    // initialize epechi
    epechi_init(&g.ep);

    // initialize environment stuff
    // BONFIRE
    bf_init(&g.bonfire);

    // initialize other actors/sprites
    
    // BATARANGS
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_init(&g.batrs[i]);
    }
    // ORBS
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_init(&g.orbs[i]);
    }
    // AANAMARUTHAS
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_init(&g.aanas[i]);
    }
    // initialize generic game object stuff
    g.is_gameover = false;
    g.is_game_wclosed = false;
    g.is_game_paused = false;
    g.cam.rotation = 0;
    g.cam.zoom = 1;
    g.cam.target = obj_cxy(&g.p.obj);
    g.cam.offset = (Vector2){G_W/2, (GAME_GROUND_Y - g.p.obj.curr_anim->curr_frame.height/2)};
}

void _game_update(float dt) {
    if(dt > 0.1f)dt = 0.1f;

    // handle inputs only when player is alive.
    if(!g.p.is_dying && !g.p.is_hurting) {
        if(IsKeyDown(KEY_D)) {
            player_activate_move_r(&g.p, dt);
        }

        if(IsKeyDown(KEY_A)) {
            player_activate_move_l(&g.p, dt);
        }

        if(IsKeyPressed(KEY_SPACE) && !g.p.is_jumping) {
            player_activate_jump(&g.p, dt);
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_pos = game_get_scaled_mouse_pos();
            // check for empty slot
            batr_t *b = NULL;
            for(int i = 0; i < MAX_BATRS; ++i) {
                if(!g.batrs[i].obj.is_active) {
                    b = &g.batrs[i];
                    break;
                }
            }
            if(b) {
                player_activate_batr(&g.p, b, mouse_pos, dt);
            }
        }

        player_update(&g.p, g.is_boss_active, dt);

        // update camera position w.r.t player considering deadzone
        // DEADZONE RIGHT LIMIT
        if(!g.is_boss_active) {
            float px_rl = g.p.obj.pos.x + g.p.obj.curr_anim->curr_frame.width;
            if((px_rl - g.cam.target.x) > DZ_RL) {
                g.cam.target.x = px_rl - DZ_RL;
            }
            // DEADZONE LEFT LIMIT
            float px_ll = g.p.obj.pos.x;
            if(px_ll - g.cam.target.x < DZ_LL) {
                g.cam.target.x = px_ll - DZ_LL;
            }
        }

        // Update batarangs
        for(int i = 0; i < MAX_BATRS; ++i) {
            batr_t *b = &g.batrs[i];
            if(b->obj.is_active) {
                batr_update(b, dt);
            }
        }
    }

    // handle hurting
    if(g.p.is_hurting) {
        // switch to hurt animation
        player_activate_hurting(&g.p, dt);
        // if hurt animation is over, switch to idle
        
    }

    // BONFIRE
    if(!g.bonfire.obj.is_active) {
        g.bonfire.obj.is_active = true;
    }

    if(g.bonfire.obj.is_active) {
        g.bonfire.obj.pos.x = G_W/2;
        anim_advance(g.bonfire.obj.curr_anim, dt);
    }

    // VADAYAKSHI
    // TODO: move to automatic appearance as level boss
    if(IsKeyPressed(KEY_V)) {
        if(!g.vy.obj.is_active) {
            g.vy.obj.is_active = true;
            g.is_boss_active = true;
        } else {
            g.vy.obj.is_active = false;
            g.is_boss_active = false;
            g.vy.is_orbpos = false;
        }
        g.vy.obj.pos = GetScreenToWorld2D(vy_get_initial_spos(), g.cam);
        g.vy.obj.vel.x = VY_RISE_VEL_X_0;
        g.vy.obj.vel.y = VY_RISE_VEL_Y_0;
    }

    if(g.vy.obj.is_active && !g.vy.is_dying) {
        Vector2 vy_sfinalpos = GetScreenToWorld2D(vy_get_final_spos(), g.cam);
        if(!g.vy.is_orbpos) {
            g.vy.obj.pos.x += g.vy.obj.vel.x * dt;
            g.vy.obj.pos.y += g.vy.obj.vel.y * dt;
            g.vy.obj.pos.x = _max(g.vy.obj.pos.x, vy_sfinalpos.x);
            g.vy.obj.pos.y = _max(g.vy.obj.pos.y, vy_sfinalpos.y);
            g.vy.is_orbpos = vec_equals(g.vy.obj.pos, vy_sfinalpos);
        } else {
            if(obj_wx_lt_s(&g.vy.obj, 300, g.cam)) {
                g.vy.obj.vel.x = VY_RISE_VEL_X_1;
            }
            if(obj_wx_gt_s(&g.vy.obj, G_W - 300, g.cam)) {
                g.vy.obj.vel.x = -VY_RISE_VEL_X_1;
            }
            g.vy.obj.pos.x += g.vy.obj.vel.x * dt;
        }
        anim_advance(g.vy.obj.curr_anim, dt);
    }

    if(g.vy.is_orbpos) {
        for(int i = 0; i < MAX_ORBS; i++) {
            orb_t *orb = &g.orbs[i];
            if(!orb->obj.is_active && ORB_RAND_CHANCE) {
                orb_activate(orb, dt);
            }
        }
    }

    for(int i = 0; i < MAX_ORBS; i++) {
        orb_t *orb = &g.orbs[i];
        if(orb->obj.is_active) {
            orb_update(orb, dt);
        }
    }

    // EENAMPECHI
    if(IsKeyPressed(KEY_E)) {
        if(!g.ep.obj.is_active) {
            g.ep.obj.is_active = true;
            g.is_boss_active = true;
        } else {
            g.ep.obj.is_active = false;
            g.is_boss_active = false;
        }
        g.ep.obj.pos = GetScreenToWorld2D(epechi_get_initial_spos(), g.cam);
        g.ep.obj.vel.x = -6E2;
        g.ep.obj.vel.y = 0;
    }

    if(g.ep.obj.is_active && !g.ep.is_dying) {
        g.ep.obj.pos.x += g.ep.obj.vel.x*dt;
        anim_advance(g.ep.obj.curr_anim, dt);
    }

    // AANAMARUTHAS
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_t *aana = &g.aanas[i];
        if(!aana->obj.is_active && AANA_RAND_CHANCE && !g.is_boss_active) {
            aana->obj.is_active = true;
            aana->hit_player = false;
            Vector2 pos, vel;
            pos.y = GAME_GROUND_Y - aana->obj.size.y;
            pos.x = G_W - aana->obj.size.x;
            pos = GetScreenToWorld2D(pos, g.cam);
            vel.y = 0;
            vel.x = AANA_VEL_X;
            aana->obj.pos = pos;
            aana->obj.vel = vel;
        }
    }

    for(int i = 0; i < MAX_AANAS; i++) {
        aanam_t *aana = &g.aanas[i];
        if(aana->obj.is_active) {
            // check for bounds
            if(obj_is_oob(&aana->obj, COORDS_WORLD)) {
                aana->obj.is_active = false;
            } else {
                // update x pos
                aana->obj.pos.x += aana->obj.vel.x * dt;
                anim_advance(aana->obj.curr_anim, dt);
            }
        }
    }

    /////////////////////////////////////////////////
    //////////// COLLISION DETECTION ////////////////
    /////////////////////////////////////////////////
    // BATARANG WITH ORB
    for(int i = 0; i < MAX_BATRS; ++i) {
        for(int j = 0; j < MAX_ORBS; ++j) {
            batr_t *b = &g.batrs[i];
            orb_t *orb = &g.orbs[j];
            if(b->obj.is_active && orb->obj.is_active) {
                if(col_check_batr_orb(b, orb)) {
                    // deactivate batr, change velocity of orb and aim it at vy
                    b->obj.is_active = false;
                    float v = vec_magnitude(b->obj.vel);
                    orb->obj.vel = orb_get_hostile_vel(&g.vy, orb, v);
                    // set orb as hostile
                    orb->is_hostile = true;
                }
            }
        }
    }
    // BATARANG WITH VY
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g.batrs[i];
        if(b->obj.is_active && !g.vy.is_dying && col_check_vy_batr(&g.vy, b)) {
            if(col_check_vy_batr(&g.vy, b)) {
                b->obj.is_active = false;
                g.vy.health -= VY_BATR_HEALTH_DECR;
                hbar_update(&g.vy.hbar, g.vy.health, g.vy.max_health);
            }
        }
    }
    // ORB WITH PLAYER
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_t *orb = &g.orbs[i];
        if(orb->obj.is_active && !g.p.is_dying) {
            if(col_check_player_orb(&g.p, orb)) {
                orb->obj.is_active = false;
                g.p.health -= PLAYER_ORB_HEALTH_DECR;
                hbar_update(&g.p.hbar, g.p.health, g.p.max_health);
            }
        }
    }
    // AANAM WITH PLAYER
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_t *aana = &g.aanas[i];
        if(aana->obj.is_active && !g.p.is_dying) {
            if(col_check_player_aanam(&g.p, aana) && !aana->hit_player) {
                aana->hit_player = true;
                g.p.health -= 5;
                hbar_update(&g.p.hbar, g.p.health, g.p.max_health);
            }
        }
    } 

    // check for player death
    if(g.p.health <= 0) {
        g.p.is_dying = true;
    }
}

// pause wrapper
void game_update(float dt) {
    if(!IsKeyPressed(KEY_ESCAPE) && !g.is_game_paused) {
        _game_update(dt);
    } else {
        if(IsKeyPressed(KEY_ESCAPE)) {
            g.is_game_paused = !g.is_game_paused;
        }
    }
}

//////////////////////////////
//////// SCENES //////////////
//////////////////////////////

void game_draw_canvas_to_screen(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    // Y is negative because textures are inverted in OpenGL
    Rectangle sourceRec = { 0.0f, 0.0f, (float)g.canvas->texture.width, -(float)g.canvas->texture.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(g.canvas->texture, sourceRec, destRec, origin, 0.0f, WHITE);
    EndDrawing();
}

void game_start_scene(void) {
    bool game_start = false;
    bool game_quit = false;
    menu_action m_act = MENU_NO_ACTION;
    while(!game_start && !g.is_game_wclosed) {
        BeginTextureMode(*g.canvas);
        DrawTexture(g.splash, 0, 0, DARKGRAY);
        menu_draw(m_act, MENU_START);
        EndTextureMode();
        game_draw_canvas_to_screen();

        // check menu interaction
        m_act = menu_get_action();
        game_start = (m_act == MENU_CLICK_START);
        game_quit = (m_act == MENU_CLICK_QUIT);
        g.is_game_wclosed = WindowShouldClose() | game_quit;
    }
}

void game_start_main_loop(void) {
    while(!g.is_game_wclosed && !g.is_gameover) {
        game_update(GetFrameTime());

        if(!g.is_game_paused) {
            BeginTextureMode(*g.canvas);
            BeginMode2D(g.cam);
        
            float cam_left_edge = g.cam.target.x - (G_W/2);
            float bg_offset = (int)cam_left_edge % g.bg.width;
            if(bg_offset < 0) {
                bg_offset += g.bg.width;
            }
            float drawX = cam_left_edge - bg_offset;
            
            // Draw infinite background
            DrawTexture(g.bg, drawX, 0, DARKGRAY);
            DrawTexture(g.bg, drawX + g.bg.width, 0, DARKGRAY);
            DrawTexture(g.bg, drawX + 2*g.bg.width, 0, DARKGRAY);
            // Draw player
            player_draw(&g.p);
            // Draw bonfire
            if(g.bonfire.obj.is_active) {
                bf_draw(&g.bonfire);
            }
            // Draw all batarangs
            for(int i = 0; i < MAX_BATRS; ++i) {
                batr_t *b = &g.batrs[i];
                if(b->obj.is_active) {
                    batr_draw(b);
                }
            }
            // Draw VY
            if(g.vy.obj.is_active) {
                vy_draw(&g.vy);
            }
            // Draw EPECHI
            if(g.ep.obj.is_active) {
                epechi_draw(&g.ep);
            }
            // Draw all aanas
            for(int i = 0; i < MAX_AANAS; ++i) {
                aanam_t *aana = &g.aanas[i];
                if(aana->obj.is_active) {
                    aanam_draw(aana);
                }
            }
            EndMode2D();
            
            for(int i = 0; i < MAX_ORBS; ++i) {
                orb_t *orb = &g.orbs[i];
                if(orb->obj.is_active) {
                    orb_draw(orb);
                }
            }

            hbar_draw(&g.p.hbar, &g.p.hbar_icon, g.p.hbar_iconpos, (Color){0x3E, 0, 0, 0xFF});
            if(g.vy.obj.is_active) {
                hbar_draw(&g.vy.hbar, &g.vy.hbar_icon, g.vy.hbar_iconpos, RED);
            }

            EndTextureMode();
            game_draw_canvas_to_screen();
            g.is_game_wclosed = WindowShouldClose();
        } else {
            bool game_resumed = false;
            bool game_quit = false;
            menu_action m_act = menu_get_action();
            BeginTextureMode(*g.canvas);
            DrawTexture(g.pausemenu, 0, 0, WHITE);
            menu_draw(m_act, MENU_PAUSE);
            EndTextureMode();
            game_draw_canvas_to_screen();

            // check menu interaction
            game_quit = (m_act == MENU_CLICK_QUIT);
            game_resumed = (m_act == MENU_CLICK_START);
            g.is_game_wclosed = WindowShouldClose() | game_quit;
            g.is_game_paused = !game_resumed;
        }
    }
}