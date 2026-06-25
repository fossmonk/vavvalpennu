#include <vpconfig.h>
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <game.h>
#include <menu.h>
#include <collisions.h>
#include <audio.h>
#include <hud.h>
#include <boss.h>

// SOME CONSTANTS

#define VY_BATR_HEALTH_DECR    (1)
#define PLAYER_ORB_HEALTH_DECR (20)

// SOME MACRO FUNCTIONS
#ifndef _max
#define _max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef _min
#define _min(a, b) ((a) < (b) ? (a) : (b))
#endif

// GLOBALS
// top level game object
static game_t *g;

int g_levelup_scores[] = {
    [0] = 1000,
    [1] = 2000,
    [2] = 3500,
    [3] = 5000 
};

Vector2 game_get_scaled_mouse_pos(void) {
    Vector2 mouseScalePos = GetMousePosition();
    mouseScalePos.x = (mouseScalePos.x / GetScreenWidth()) * G_W;
    mouseScalePos.y = (mouseScalePos.y / GetScreenHeight()) * G_H;
    return mouseScalePos;
}

void game_load_assets(void) {
    // background texture
    g->bg = LoadTexture(BACKGROUND);
    // splash screen texture
    g->splash = LoadTexture(SPLASH_BG);
    // pause menu texture
    g->pausemenu = LoadTexture(PAUSEMENU_BG);
    // custom cursor
    g->cursor = LoadTexture(BAT_CURSOR);
    // karikku thumbnail
    g->ktex = LoadTexture(KARIKKU_ICON);
    // fonts
    g->game_font = LoadFontEx(MAIN_FONT, 96, NULL, 0);
    SetTextureFilter(g->game_font.texture, TEXTURE_FILTER_BILINEAR);
    // animation assets
    anim_asset_load_all();
    // sound assets
    g->bgmusic = LoadMusicStream(AUD_AMBIENT);
}

void game_init(RenderTexture2D *canvas) {
    // allocate memory first
    g = malloc(sizeof(*g));
    // init audio engine
    audio_init();
    // set canvas
    g->canvas = canvas;
    // kind of a hack, but makes stuff easy
    obj_global_set_cam2d(&g->cam);
    // load all assets
    game_load_assets();
    // initialize menu
    menu_init();
    // initialize player
    player_init(&g->p);
    
    // init all bosses
    boss_init_bosses();

    g->levelbosses = boss_get_bosses();

    // initialize environment stuff
    // BONFIRE
    // TODO bf_init(&g->bonfire);

    // KARIKKU
    for(int i = 0; i < TOTAL_KARIKKU; ++i) {
        karikku_init(&g->karikku[i]);
    }
    
    // FIREFLY
    for(int i = 0; i < MAX_FFLY; ++i) {
        ffly_init(&g->ffly[i]);
    }

    // initialize other actors/sprites
    
    // BATARANGS
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_init(&g->batrs[i]);
    }

    // AANAMARUTHAS
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_init(&g->aanas[i]);
    }
    // initialize generic game object stuff
    g->is_gameover = false;
    g->is_game_wclosed = false;
    g->is_game_paused = false;
    g->is_boss_active = false;
    g->cam.rotation = 0;
    g->cam.zoom = 1;
    g->cam.target = obj_cxy(&g->p.obj);
    g->cam.offset = (Vector2){G_W/2, (GAME_GROUND_Y - g->p.obj.curr_anim->curr_frame.height/2)};

    // START THE MUSIC
    PlayMusicStream(g->bgmusic);
}

void _game_update(float dt) {
    if(dt > 0.1f)dt = 0.1f;

    // update bgmusic
    UpdateMusicStream(g->bgmusic);

    // PLAYER
    // handle inputs and execute player actions
    // handle inputs only if player is not hurting
    if(!player_is_dying(&g->p) && !player_is_hurting(&g->p)) {
        if(IsKeyDown(KEY_D)) {
            player_activate_move_r(&g->p, dt);
        }

        if(IsKeyDown(KEY_A)) {
            player_activate_move_l(&g->p, dt);
        }

        if(IsKeyPressed(KEY_SPACE) && !player_is_jumping(&g->p)) {
            player_activate_jump(&g->p, dt);
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_pos = game_get_scaled_mouse_pos();
            // check for empty slot
            batr_t *b = batr_get_empty_slot(g->batrs);
            if(b) {
                player_activate_batr(&g->p, b, mouse_pos);
            }
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_pos = game_get_scaled_mouse_pos();
            player_activate_whiplash(&g->p, mouse_pos);
        }

        
        // update camera position w.r.t player considering deadzone
        if(!g->is_boss_active) {
            // DEADZONE RIGHT LIMIT
            float px_rl = g->p.obj.pos.x + g->p.obj.curr_anim->curr_frame.width;
            if((px_rl - g->cam.target.x) > DZ_RL) {
                g->cam.target.x = px_rl - DZ_RL;
            }
            // DEADZONE LEFT LIMIT
            float px_ll = g->p.obj.pos.x;
            if(px_ll - g->cam.target.x < DZ_LL) {
                g->cam.target.x = px_ll - DZ_LL;
            }
        }
    }

    player_update(&g->p, g->is_boss_active, dt);

    // batarangs: already activated by player input
    // now update the active ones
    batr_update_all(g->batrs, dt);

    // BONFIRE
    // TODO

    // karikkus: already activated, just update
    karikku_update_all(g->karikku, dt);

    // fireflys: active empty slots
    ffly_activate_all(g->ffly);

    // fireflys: update active ones
    ffly_update_all(g->ffly, dt);

    // aanams: activate empty slots
    aanam_activate_all(g->aanas, g->is_boss_active);

    // aanams: update active ones
    aanam_update_all(g->aanas, dt);

    //////////////////////////////
    ////////// BOSS //////////////
    //////////////////////////////

    if(g->p.score >= g_levelup_scores[g->p.curr_level] && !g->is_boss_active) {
        boss_set(VADAYAKSHI);
        boss_activate();
        g->is_boss_active = true;
    }

    boss_update(dt);

    if(boss_is_dead()) {
        g->is_boss_active = false;
        g->p.curr_level++;
    }

    /////////////////////////////////////////////////
    //////////// COLLISION DETECTION ////////////////
    /////////////////////////////////////////////////
    // BATARANG WITH ORB
    for(int i = 0; i < MAX_BATRS; ++i) {
        for(int j = 0; j < MAX_ORBS; ++j) {
            batr_t *b = &g->batrs[i];
            orb_t *orb = &g->levelbosses->vy.orbs[j];
            bool b_conds = b->obj.is_active;
            bool orb_conds = orb->obj.is_active;
            bool col_conds = col_check_batr_orb(b, orb);
            bool check = b_conds && orb_conds && col_conds;
            if(check) {
                // deactivate batr
                b->obj.is_active = false;
                // change velocity of orb and aim it at vy
                float v = Vector2Length(b->obj.vel);
                orb->obj.vel = orb_get_hostile_vel(g->levelbosses->vy.obj.pos, orb, v);
                // set orb as hostile
                orb->is_hostile = true;
            }
        }
    }
    // BATARANG WITH VY
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g->batrs[i];
        bool b_conds = b->obj.is_active;
        bool vy_conds = !vy_is_dying(&g->levelbosses->vy);
        bool col_conds = col_check_vy_batr(&g->levelbosses->vy, b);
        bool check = b_conds && vy_conds && col_conds;
        if(check) {
            b->obj.is_active = false;
            g->levelbosses->vy.health -= VY_BATR_HEALTH_DECR;
            hbar_update(&g->levelbosses->vy.hbar, g->levelbosses->vy.health);
        }
    }
    // ORB WITH PLAYER
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_t *orb = &g->levelbosses->vy.orbs[i];
        bool orb_conds = orb->obj.is_active && !orb->is_hostile;
        bool player_conds = !player_is_dying(&g->p);
        bool col_conds = col_check_player_orb(&g->p, orb);
        bool check = orb_conds && player_conds && col_conds;
        if(check) {
            orb->obj.is_active = false;
            player_set_hurt_shock(&g->p);
            g->p.health -= PLAYER_ORB_HEALTH_DECR;
            hbar_update(&g->p.hbar, g->p.health);
        }
    }
    // KARIKKU WITH PLAYER
    for(int i = 0; i < TOTAL_KARIKKU; ++i) {
        karikku_t *k = &g->karikku[i];
        bool k_conds = k->obj.is_active && !obj_is_oob(&k->obj, COORDS_WORLD);
        bool player_conds = !player_is_dying(&g->p);
        bool col_conds = col_check_player_karikku(&g->p, k);
        bool check = k_conds && player_conds && col_conds;
        if(check) {
            k->obj.is_active = false;
            g->p.k_count++;
            g->p.score += 5;
            PlaySound(g->p.slurp);
        }
    }
    // HOSTILE ORB WITH VY
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_t *orb = &g->levelbosses->vy.orbs[i];
        bool orb_conds = orb->obj.is_active && orb->is_hostile;
        bool vy_conds = !vy_is_dying(&g->levelbosses->vy);
        bool col_conds = col_check_vy_orb(&g->levelbosses->vy, orb);
        bool check = orb_conds && vy_conds && col_conds;
        if(check) {
            orb->obj.is_active = false;
            vy_set_hurt_shock(&g->levelbosses->vy);
            g->levelbosses->vy.health -= VY_BATR_HEALTH_DECR*5;
            hbar_update(&g->levelbosses->vy.hbar, g->levelbosses->vy.health);
            PlaySound(g->levelbosses->vy.hurt);
        }
    }
    // AANAM WITH PLAYER
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_t *aana = &g->aanas[i];
        bool aana_conds = aana->obj.is_active && !aana->is_dying && !aana->hit_player;
        bool player_conds = !player_is_dying(&g->p);
        bool col_conds = col_check_player_aanam(&g->p, aana);
        bool check = aana_conds && player_conds && col_conds;
        if(check) {
            if(g->p.obj.curr_anim == &g->p.anim_wlash) {
                // hit whip on aanam
                aana->is_dying = true;
                g->p.score += 50;
            } else {
                aana->hit_player = true;
                player_set_hurt_flash(&g->p);
                PlaySound(g->p.hurt);
                g->p.health -= 5;
                hbar_update(&g->p.hbar, g->p.health);
            }
        }
    }

    /// HANDLE SPECIAL ANIMATIONS ///

    // handle aanam death
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_t *aana = &g->aanas[i];
        if(aana->is_dying) {
            if(aana->obj.curr_anim == &aana->anim_death) {
                if(anim_is_lastframe(aana->obj.curr_anim)) {
                    aana->obj.is_active = false;
                    anim_reset(aana->obj.curr_anim);
                } else {
                    anim_advance(aana->obj.curr_anim, dt);
                }
            } else {
                aana->obj.curr_anim = &aana->anim_death;
            }
        }
    }

    // handle vy hurting
    if(vy_is_hurting(&g->levelbosses->vy)) {
        if(!g->levelbosses->vy.in_hurt_anim) {
            // switch to hurt animation
            g->levelbosses->vy.in_hurt_anim = true;
            vy_activate_hurting(&g->levelbosses->vy, dt);
        } else {
            // if hurt animation is over, switch to idle
            if(anim_is_lastframe(g->levelbosses->vy.obj.curr_anim)) {
                g->levelbosses->vy.in_hurt_anim = false;
                vy_clr_hurt_shock(&g->levelbosses->vy);
                g->levelbosses->vy.obj.curr_anim = &g->levelbosses->vy.anim_vy_rise;
            }
        }
        anim_advance(g->levelbosses->vy.obj.curr_anim, dt);
    }

    // check for player death
    if(g->p.health <= 0) {
        player_set_die(&g->p);
    }
}

// pause wrapper
void game_update(float dt) {
    if(!IsKeyPressed(KEY_ESCAPE) && !g->is_game_paused) {
        _game_update(dt);
    } else {
        if(IsKeyPressed(KEY_ESCAPE)) {
            g->is_game_paused = !g->is_game_paused;
        }
    }
}

// background draw wrapper
void game_draw_inf_bg(void) {
    float cam_left_edge = g->cam.target.x - (G_W/2);
    float bg_offset = (int)cam_left_edge % g->bg.width;
    if(bg_offset < 0) {
        bg_offset += g->bg.width;
    }
    float drawX = cam_left_edge - bg_offset;
    
    // Draw infinite background
    DrawTexture(g->bg, drawX, 0, DARKGRAY);
    DrawTexture(g->bg, drawX + g->bg.width, 0, DARKGRAY);
    DrawTexture(g->bg, drawX + 2*g->bg.width, 0, DARKGRAY);
}

//////////////////////////////
//////// SCENES //////////////
//////////////////////////////

void game_draw_canvas_to_screen(void) {
    Color overlay = (Color){ 50, 50, 250, 15 };
    Vector2 mpos = GetMousePosition();
    BeginDrawing();
    ClearBackground(BLACK);
    // Y is negative because textures are inverted in OpenGL
    Rectangle sourceRec = { 0.0f, 0.0f, (float)g->canvas->texture.width, -(float)g->canvas->texture.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(g->canvas->texture, sourceRec, destRec, origin, 0.0f, WHITE);
    DrawRectangle(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), overlay);
    DrawTexture(g->cursor, mpos.x, mpos.y, WHITE);
    EndDrawing();
}

void game_start_scene(void) {
    bool game_start = false;
    bool game_quit = false;
    menu_action m_act = MENU_NO_ACTION;
    while(!game_start && !g->is_game_wclosed) {
        BeginTextureMode(*g->canvas);
        DrawTexture(g->splash, 0, 0, DARKGRAY);
        menu_draw(m_act, MENU_START);
        EndTextureMode();
        game_draw_canvas_to_screen();

        // check menu interaction
        m_act = menu_get_action();
        game_start = (m_act == MENU_CLICK_START);
        game_quit = (m_act == MENU_CLICK_QUIT);
        g->is_game_wclosed = WindowShouldClose() || game_quit;
    }
}

void game_start_main_loop(void) {
    while(!g->is_game_wclosed && !g->is_gameover) {
        game_update(GetFrameTime());

        if(!g->is_game_paused) {
            BeginTextureMode(*g->canvas);

            BeginMode2D(g->cam);
            // Draw everything that moves with camera
            game_draw_inf_bg();
            player_draw(&g->p);
            // Draw bonfireTODO
            karikku_draw_all(g->karikku);
            batr_draw_all(g->batrs);
            ffly_draw_all(g->ffly);
            aanam_draw_all(g->aanas);

            // Draw VY
            if(g->levelbosses->vy.obj.is_active) {
                vy_draw(&g->levelbosses->vy);
            }
            // Draw EPECHI
            // TODO
            EndMode2D();
            
            // Draw everything that doesn't move with camera
            orb_draw_all(g->levelbosses->vy.orbs);
            if(g->levelbosses->vy.obj.is_active) {
                hbar_draw(&g->levelbosses->vy.hbar);
            }
            hud_draw(g->ktex, &g->p);

            EndTextureMode();
            game_draw_canvas_to_screen();
            g->is_game_wclosed = WindowShouldClose();
        } else {
            bool game_resumed = false;
            bool game_quit = false;
            menu_action m_act = menu_get_action();
            BeginTextureMode(*g->canvas);
            DrawTexture(g->pausemenu, 0, 0, WHITE);
            menu_draw(m_act, MENU_PAUSE);
            EndTextureMode();
            game_draw_canvas_to_screen();

            // check menu interaction
            game_quit = (m_act == MENU_CLICK_QUIT);
            game_resumed = (m_act == MENU_CLICK_START);
            g->is_game_wclosed = WindowShouldClose() || game_quit;
            g->is_game_paused = !game_resumed;
        }
    }
}

void game_deinit(void) {
    // deinit audio engine
    audio_deinit();
    // free game object memory
    free(g);
}