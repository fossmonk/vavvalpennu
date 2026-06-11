#include <vpconfig.h>
#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <player.h>
#include <game.h>

// SOME HELPER MACROS

#ifndef _max
#define _max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef _min
#define _min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef _abs
#define _abs(a) ((a) > 0) ? (a) : -(a)
#endif

#define vec_equals(a, b) ((a).x == (b).x && (a).y == (b).y) ? true : false

#define player_center_xy(obj) (Vector2){((obj).pos.x + fabsf(((obj).curr_anim->curr_frame.width))/2) , ((obj).pos.y + ((obj).curr_anim->curr_frame.height)/2)} 
#define flip_anim_left(obj) do {                  \
    if((obj)->curr_anim->curr_frame.width > 0) {  \
        (obj)->curr_anim->curr_frame.width *= -1; \
    }                                             \
} while(0)
#define flip_anim_right(obj) do {                  \
    if((obj)->curr_anim->curr_frame.width < 0) {  \
        (obj)->curr_anim->curr_frame.width *= -1; \
    }                                             \
} while(0)

// GLOBALS
// top level game object
static game_t g;

// AANAMARUTHA
extern anim_asset_t aanam_run;

void game_load_assets(void) {
    // background
    g.bg = LoadTexture(BACKGROUND);
    
    // fonts
    g.game_font = LoadFontEx(MAIN_FONT, 96, NULL, 0);

    // animation assets
    anim_asset_load_all();
}

void aanam_init(aanam_t *aana) {
    // ANIM
    Vector2 dim;
    // run
    dim = anim_asset_get_frame_dim(&aanam_run);
    aana->anim_run.asset = &aanam_run;
    aana->anim_run.timer = 0.0f;
    aana->anim_run.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    aana->obj.curr_anim = &aana->anim_run;
    aana->obj.size = (Vector2){dim.x, dim.y};
    aana->obj.is_active = false;
    aana->is_dying = false;
}

void game_init(void) {
    // load all assets
    game_load_assets();

    // init player
    player_init(&g.p);

    // init other actors/sprites
    // BATARANG
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_init(&g.batrs[i]);
    }

    // init vy
    vy_init(&g.vy);

    // ORBS
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_init(&g.orbs[i]);
    }

    // init aanam
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_init(&g.aanas[i]);
    }

    // init general things
    g.is_gameover = false;
    g.is_game_wclosed = false;
    g.cam.rotation = 0;
    g.cam.zoom = 1;
    g.cam.target = player_center_xy(g.p.obj);
    g.cam.offset = (Vector2){G_W/2, (GAME_GROUND_Y - g.p.obj.curr_anim->curr_frame.height/2)};
}

void game_advance_anim(anim_t *anim, float dt) {
    anim->timer += dt;
    if(anim->timer >= anim->asset->duration) {
        // advance frame
        int fw = fabsf(anim->curr_frame.width);
        int tw = anim->asset->texture.width;
        int curr_x = anim->curr_frame.x;
        if(!anim->asset->repeat) {
            curr_x = _min(curr_x + fw, tw-fw); 
        } else {
            curr_x = (curr_x + fw) % tw;
        }
        anim->curr_frame.x = curr_x;
        anim->timer -= anim->asset->duration;
    }
}

bool game_obj_is_xr_lim(obj_t *obj, float lim) {
    Vector2 screenpos = GetWorldToScreen2D(obj->pos, g.cam);
    if(screenpos.x > lim) return true;
    return false;
}

bool game_obj_is_xl_lim(obj_t *obj, float lim) {
    Vector2 screenpos = GetWorldToScreen2D(obj->pos, g.cam);
    if(screenpos.x < lim) return true;
    return false;
}

bool game_obj_is_yr_lim(obj_t *obj, float lim) {
    Vector2 screenpos = GetWorldToScreen2D(obj->pos, g.cam);
    if(screenpos.y > lim) return true;
    return false;
}

bool game_obj_is_yl_lim(obj_t *obj, float lim) {
    Vector2 screenpos = GetWorldToScreen2D(obj->pos, g.cam);
    if(screenpos.y < lim) return true;
    return false;
}

void game_update(float dt) {
    if(dt > 0.1f)dt = 0.1f;
    float ax = 4000.0f;

    // handle inputs only when player is alive.
    if(!g.p.is_dying) {
        if(IsKeyDown(KEY_D)) {
            g.p.obj.vel.x += ax*dt;
            // set curr_anim to run
            if(!g.p.is_jumping) {
                g.p.obj.curr_anim = &g.p.anim_run_r;
                g.p.obj.size = (Vector2){fabsf(g.p.obj.curr_anim->curr_frame.width), g.p.obj.curr_anim->curr_frame.height};
            }
            // reverse direction if left
            if(g.p.obj.curr_anim->curr_frame.width < 0) {
                g.p.obj.curr_anim->curr_frame.width *= -1;
            }
            g.p.obj.hdir = RIGHT;
            flip_anim_right(&g.p.obj);
        }

        if(IsKeyDown(KEY_A)) {
            g.p.obj.vel.x -= ax*dt;
            if(!g.p.is_jumping) {
                g.p.obj.curr_anim = &g.p.anim_run_r;
                g.p.obj.size = (Vector2){fabsf(g.p.obj.curr_anim->curr_frame.width), g.p.obj.curr_anim->curr_frame.height};
            }
            // reverse direction if right
            if(g.p.obj.curr_anim->curr_frame.width > 0) {
                g.p.obj.curr_anim->curr_frame.width *= -1;
            }
            g.p.obj.hdir = LEFT;
            flip_anim_left(&g.p.obj);
        }

        if(IsKeyPressed(KEY_SPACE) && !g.p.is_jumping) {
            g.p.obj.vel.y -= 700;
            g.p.is_jumping = true;
            g.p.obj.curr_anim = &g.p.anim_jump_r;
            g.p.obj.size = (Vector2){fabsf(g.p.obj.curr_anim->curr_frame.width), g.p.obj.curr_anim->curr_frame.height};
            if(g.p.obj.hdir == RIGHT) {
                flip_anim_right(&g.p.obj);
            } else {
                flip_anim_left(&g.p.obj);
            }
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_pos = GetMousePosition();
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
                b->obj.pos = player_center_xy(g.p.obj);
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
        g.p.obj.pos.y = _min(g.p.obj.pos.y, GAME_GROUND_Y - g.p.obj.curr_anim->curr_frame.height);

        // clamp x if boss is active
        if(g.is_boss_active) {
            // get world coordinates for screen x limits
            Vector2 player_screen_pos = GetWorldToScreen2D(g.p.obj.pos, g.cam);
            player_screen_pos.x = _max(player_screen_pos.x, 0);
            player_screen_pos.x = _min(player_screen_pos.x, G_W - g.p.obj.curr_anim->curr_frame.width);
            g.p.obj.pos = GetScreenToWorld2D(player_screen_pos, g.cam);
        }

        // switch to idle animation if player is in ground
        if(g.p.is_jumping && (g.p.obj.pos.y >= (float)(GAME_GROUND_Y - g.p.obj.curr_anim->curr_frame.height))) {
            g.p.is_jumping = false;
        }

        if(!g.p.is_jumping && ((int)(g.p.obj.vel.x) == 0)) {
            g.p.obj.curr_anim = &g.p.anim_idle_r;
            if(g.p.obj.hdir == LEFT) {
                flip_anim_left(&g.p.obj);
            }
            if(g.p.obj.hdir == RIGHT) {
                flip_anim_right(&g.p.obj);
            }
        }
        
        // update animation
        game_advance_anim(g.p.obj.curr_anim, dt);

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
                    game_advance_anim(b->obj.curr_anim, dt);
                }
            }
        }
    }

    if(IsKeyPressed(KEY_V)) {
        if(!g.vy.obj.is_active) {
            g.vy.obj.pos = GetScreenToWorld2D((Vector2){G_W-300, GAME_GROUND_Y-250}, g.cam);
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
        Vector2 vy_sfinalpos = GetScreenToWorld2D((Vector2){G_W/2 - 150, 40}, g.cam);
        if(!g.vy.is_orbpos) {
            g.vy.obj.pos.x += g.vy.obj.vel.x * dt;
            g.vy.obj.pos.y += g.vy.obj.vel.y * dt;
            g.vy.obj.pos.x = _max(g.vy.obj.pos.x, vy_sfinalpos.x);
            g.vy.obj.pos.y = _max(g.vy.obj.pos.y, vy_sfinalpos.y);
            g.vy.is_orbpos = vec_equals(g.vy.obj.pos, vy_sfinalpos);
        } else {
            if(game_obj_is_xl_lim(&g.vy.obj, 300)) {
                g.vy.obj.vel.x = 2.5E2;
            }
            if(game_obj_is_xr_lim(&g.vy.obj, G_W - 300)) {
                g.vy.obj.vel.x = -2.5E2;
            }
            g.vy.obj.pos.x += g.vy.obj.vel.x * dt;
        }
        game_advance_anim(g.vy.obj.curr_anim, dt);
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
                game_advance_anim(aana->obj.curr_anim, dt);
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
            Vector2 bspos = GetWorldToScreen2D(b->obj.pos, g.cam);
            Vector2 vyspos = GetWorldToScreen2D(g.vy.obj.pos, g.cam);
            float b_c_y = bspos.y + b->obj.size.y/2;
            float b_c_x = bspos.x + b->obj.size.x/2;
            float vy_c_y = vyspos.y + g.vy.obj.size.y/2;
            float vy_c_x = vyspos.x + g.vy.obj.size.x/2;
            float cart_d = (b_c_y - vy_c_y)*(b_c_y - vy_c_y) + (b_c_x - vy_c_x)*(b_c_x - vy_c_x);
            float lim = b->obj.size.x/2;
            if(cart_d < lim*lim) {
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

//////////////////////////////
//////// SCENES //////////////
//////////////////////////////

void game_start_scene(void) {
    char *welcome_text = "PRESS P TO PLAY";
    Vector2 fontWH = MeasureTextEx(g.game_font, welcome_text, 50, 1.5);
    while(!IsKeyPressed(KEY_P) && !g.is_game_wclosed) {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawTextEx(g.game_font, welcome_text, (Vector2){G_W/2 - fontWH.x/2, G_H/2 - fontWH.y/2}, 50, 1.5, WHITE);
        EndDrawing();
        g.is_game_wclosed = WindowShouldClose();
    }
}

void game_start_main_loop(void) {
    while(!g.is_game_wclosed && !g.is_gameover) {
        game_update(GetFrameTime());
        
        BeginDrawing();
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

        // Draw lines from orb to player
        Vector2 ppos = GetWorldToScreen2D(g.p.obj.pos, g.cam);
        float pcx = ppos.x + g.p.obj.size.x/2;
        float pcy = ppos.y + g.p.obj.size.y/2 - 50;
        for(int i = 0; i < MAX_ORBS; ++i) {
            orb_t *orb = &g.orbs[i];
            if(orb->obj.is_active) {
                float orb_cx = orb->obj.pos.x + orb->obj.size.x/2;
                float orb_cy = orb->obj.pos.y + orb->obj.size.y/2;
                DrawLine(pcx, pcy, orb_cx, orb_cy, RED);
            }
        }

        // Draw lines from BATR to VY
        Vector2 vypos = GetWorldToScreen2D(g.vy.obj.pos, g.cam);
        float vy_cx = vypos.x + g.vy.obj.size.x/2;
        float vy_cy = vypos.y + g.vy.obj.size.y/2;
        for(int i = 0; i < MAX_BATRS; ++i) {
            batr_t *b = &g.batrs[i];
            Vector2 bpos = GetWorldToScreen2D(b->obj.pos, g.cam);
            if(b->obj.is_active) {
                float b_cx = bpos.x + b->obj.size.x/2;
                float b_cy = bpos.y + b->obj.size.y/2;
                DrawLine(vy_cx, vy_cy, b_cx, b_cy, RED);
            }
        }

        hbar_draw(&g.p.hbar);
        if(g.vy.obj.is_active) {
            hbar_draw(&g.vy.hbar);
        }
        EndDrawing();
        g.is_game_wclosed = WindowShouldClose();
    }
}