#include <vpconfig.h>
#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <game.h>
#include <menu.h>

// SOME HELPER MACROS

#ifndef _max
#define _max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef _min
#define _min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define vec_equals(a, b) ((a).x == (b).x && (a).y == (b).y) ? true : false

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
    // load all assets
    game_load_assets();
    // initialize menu
    menu_init();
    // initialize player
    player_init(&g.p);
    // initialize vadayakshi
    vy_init(&g.vy);

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
    float ax = 4000.0f;

    // handle inputs only when player is alive.
    if(!g.p.is_dying) {
        if(IsKeyDown(KEY_D)) {
            g.p.obj.vel.x += ax*dt;
            // set curr_anim to run
            if(!g.p.is_jumping) {
                g.p.obj.curr_anim = &g.p.anim_run_r;
                g.p.obj.size = anim_get_framesize(g.p.obj.curr_anim);
            }
            // reverse direction if left
            if(g.p.obj.curr_anim->curr_frame.width < 0) {
                g.p.obj.curr_anim->curr_frame.width *= -1;
            }
            g.p.obj.hdir = RIGHT;
            anim_hflipr(&g.p.obj);
        }

        if(IsKeyDown(KEY_A)) {
            g.p.obj.vel.x -= ax*dt;
            if(!g.p.is_jumping) {
                g.p.obj.curr_anim = &g.p.anim_run_r;
                g.p.obj.size = anim_get_framesize(g.p.obj.curr_anim);
            }
            // reverse direction if right
            if(g.p.obj.curr_anim->curr_frame.width > 0) {
                g.p.obj.curr_anim->curr_frame.width *= -1;
            }
            g.p.obj.hdir = LEFT;
            anim_hflipl(&g.p.obj);
        }

        if(IsKeyPressed(KEY_SPACE) && !g.p.is_jumping) {
            g.p.obj.vel.y -= 700;
            g.p.is_jumping = true;
            g.p.obj.curr_anim = &g.p.anim_jump_r;
            g.p.obj.size = anim_get_framesize(g.p.obj.curr_anim);
            if(g.p.obj.hdir == RIGHT) {
                anim_hflipr(&g.p.obj);
            } else {
                anim_hflipl(&g.p.obj);
            }
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_pos = game_get_scaled_mouse_pos();
            // check for empty slot
            batr_t *b = NULL;
            // int b_idx = 0;
            for(int i = 0; i < MAX_BATRS; ++i) {
                if(!g.batrs[i].obj.is_active) {
                    b = &g.batrs[i];
                    // b_idx = i;
                    break;
                }
            }
            if(b) {
                b->obj.is_active = true;
                b->obj.curr_anim->curr_frame.x = 0;
                float delta = 70.0;
                b->obj.pos = obj_cxy(&g.p.obj);
                b->obj.pos.y -= delta;
                Vector2 spawn_pos_s = GetWorldToScreen2D(b->obj.pos, g.cam);
                float dx = mouse_pos.x - spawn_pos_s.x;
                float dy = mouse_pos.y - spawn_pos_s.y;
                float d  = sqrt(dx*dx + dy*dy);
                if(d == 0.0f)d = 1.0f;
                float sintheta = dy/d;
                float costheta = dx/d;
                b->obj.vel.x = 500.0 * costheta;
                b->obj.vel.y = 500.0 * sintheta;
            }
        }

        // decay player velocity
        g.p.obj.vel.x *= expf(-12*dt);
        g.p.obj.vel.y *= expf(-2*dt);
        // update yvel with gravity
        g.p.obj.vel.y += 400*dt;
        // now update position
        g.p.obj.pos.x += g.p.obj.vel.x*dt;
        g.p.obj.pos.y += g.p.obj.vel.y*dt;

        // clamp y
        g.p.obj.pos.y = _min(g.p.obj.pos.y, GAME_GROUND_Y - g.p.obj.size.y);

        // clamp x if boss is active
        if(g.is_boss_active) {
            // get world coordinates for screen x limits
            Vector2 player_screen_pos = GetWorldToScreen2D(g.p.obj.pos, g.cam);
            player_screen_pos.x = _max(player_screen_pos.x, 0);
            player_screen_pos.x = _min(player_screen_pos.x, G_W - g.p.obj.size.x);
            g.p.obj.pos = GetScreenToWorld2D(player_screen_pos, g.cam);
        }

        // switch to idle animation if player is in ground
        if(g.p.is_jumping && (g.p.obj.pos.y >= (float)(GAME_GROUND_Y - g.p.obj.curr_anim->curr_frame.height))) {
            g.p.is_jumping = false;
        }

        if(!g.p.is_jumping && ((int)(g.p.obj.vel.x) == 0)) {
            g.p.obj.curr_anim = &g.p.anim_idle_r;
            if(g.p.obj.hdir == LEFT) {
                anim_hflipl(&g.p.obj);
            }
            if(g.p.obj.hdir == RIGHT) {
                anim_hflipr(&g.p.obj);
            }
        }
        
        // update animation
        anim_advance(g.p.obj.curr_anim, dt);

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
                // check for bounds
                if(obj_is_oob(&b->obj, COORDS_WORLD, g.cam)) {
                    b->obj.is_active = false;
                } else {
                    // update positions with velocity
                    b->obj.pos.x += b->obj.vel.x * dt;
                    b->obj.pos.y += b->obj.vel.y * dt;

                    // update animation
                    anim_advance(b->obj.curr_anim, dt);
                }
            }
        }
    }

    if(IsKeyPressed(KEY_V)) {
        if(!g.vy.obj.is_active) {
            g.vy.obj.pos = GetScreenToWorld2D(vy_get_initial_spos(), g.cam);
            g.vy.obj.is_active = true;
            g.is_boss_active = true;
            g.vy.obj.vel.x = -3E2;
            g.vy.obj.vel.y = -2.5E2;
        } else {
            g.vy.obj.is_active = false;
            g.is_boss_active = false;
        }
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
                g.vy.obj.vel.x = 2.5E2;
            }
            if(obj_wx_gt_s(&g.vy.obj, G_W - 300, g.cam)) {
                g.vy.obj.vel.x = -2.5E2;
            }
            g.vy.obj.pos.x += g.vy.obj.vel.x * dt;
        }
        anim_advance(g.vy.obj.curr_anim, dt);
    }

    if(g.vy.is_orbpos) {
        for(int i = 0; i < MAX_ORBS; i++) {
            orb_t *orb = &g.orbs[i];
            if(!orb->obj.is_active && (rand() % 100003 == 0)) {
                orb->obj.is_active = true;
                Vector2 pos, vel;
                pos.y = 0;
                pos.x = 100 + (rand() % (G_W - 200));
                vel.x = 0;
                vel.y = 400.0f*dt;
                orb->obj.pos = pos;
                orb->obj.vel = vel;
            }
        }

        for(int i = 0; i < MAX_ORBS; i++) {
            orb_t *orb = &g.orbs[i];
            if(orb->obj.is_active) {
                // check for bounds
                if(obj_is_oob(&orb->obj, COORDS_SCREEN, g.cam)) {
                    orb->obj.is_active = false;
                } else {
                    // update y pos and velocity
                    orb->obj.vel.y += 400.0f*dt;
                    orb->obj.pos.y += orb->obj.vel.y * dt;
                }
            }
        }
    }

    // AANAMARUTHAS
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_t *aana = &g.aanas[i];
        if(!aana->obj.is_active && (rand() % 12283 == 0)) {
            aana->obj.is_active = true;
            Vector2 pos, vel;
            pos.y = GAME_GROUND_Y - aana->obj.size.y;
            pos.x = G_W - aana->obj.size.x;
            pos = GetScreenToWorld2D(pos, g.cam);
            vel.y = 0;
            vel.x = -400.0f;
            aana->obj.pos = pos;
            aana->obj.vel = vel;
        }
    }

    for(int i = 0; i < MAX_AANAS; i++) {
        aanam_t *aana = &g.aanas[i];
        if(aana->obj.is_active) {
            // check for bounds
            if(obj_is_oob(&aana->obj, COORDS_WORLD, g.cam)) {
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
    // BATARANG WITH VY
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g.batrs[i];
        if(b->obj.is_active && !g.vy.is_dying) {
            float cart_d2 = obj_cartd2(&b->obj, COORDS_WORLD, &g.vy.obj, COORDS_WORLD, g.cam);
            float lim = b->obj.size.x/2;
            if(cart_d2 < lim*lim) {
                b->obj.is_active = false;
                g.vy.health -= 5;
                hbar_update(&g.vy.hbar, g.vy.health, g.vy.max_health);
            }
        }
    }
    // ORB WITH PLAYER
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_t *orb = &g.orbs[i];
        if(orb->obj.is_active && !g.p.is_dying) {
            Vector2 pspos = GetWorldToScreen2D(g.p.obj.pos, g.cam);
            float orb_down_y = orb->obj.pos.y + orb->obj.size.y;
            float orb_c_x = orb->obj.pos.x + orb->obj.size.x/2;
            float p_up_y = pspos.y;
            float p_c_x = pspos.x + g.p.obj.size.x/2;
            if((fabsf(orb_down_y - p_up_y) < 40) && (fabsf(orb_c_x - p_c_x) < 40)) {
                orb->obj.is_active = false;
                g.p.health -= 20;
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
    // Calculate destination rectangle
    // If aspect ratios differ, you would calculate letterboxing math here.
    // Note: Y is negative because textures are inverted in OpenGL
    Rectangle sourceRec = { 0.0f, 0.0f, (float)g.canvas->texture.width, -(float)g.canvas->texture.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 origin = { 0.0f, 0.0f };
    // The GPU will scale the game correctly
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
            DrawTextureRec(g.p.obj.curr_anim->asset->texture, g.p.obj.curr_anim->curr_frame, g.p.obj.pos, WHITE);
            // Draw all batarangs
            for(int i = 0; i < MAX_BATRS; ++i) {
                batr_t *b = &g.batrs[i];
                if(b->obj.is_active) {
                    DrawTextureRec(b->obj.curr_anim->asset->texture, b->obj.curr_anim->curr_frame, b->obj.pos, WHITE);
                }
            }
            // Draw VY
            if(g.vy.obj.is_active) {
                DrawTextureRec(g.vy.obj.curr_anim->asset->texture, g.vy.obj.curr_anim->curr_frame, g.vy.obj.pos, WHITE);
            }
            // Draw all aanas
            for(int i = 0; i < MAX_AANAS; ++i) {
                aanam_t *aana = &g.aanas[i];
                if(aana->obj.is_active) {
                    DrawTextureRec(aana->obj.curr_anim->asset->texture, aana->obj.curr_anim->curr_frame, aana->obj.pos, LIGHTGRAY);
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