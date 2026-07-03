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
#include <input.h>
#include <puzzle.h>
#include <textengine.h>
#include <crate.h>

// SOME CONSTANTS

#define VY_BATR_HEALTH_DECR    (1)
#define KCH_BATR_HEALTH_DECR   (5)
#define PLAYER_ORB_HEALTH_DECR (20)
#define PLAYER_SKBALL_HEALTH_DECR (5)

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

static void game_load_assets(void) {
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
    // sound assets
    g->bgmusic = LoadMusicStream(AUD_AMBIENT);
}

static void game_reset_typebuffer(void) {
    g->is_type_buffer_done = false;
    memset(g->typebuffer, 0, TYPEBUFFER_SIZE);
    g->t_ptr = 0;
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
    // initialize puzzle
    puzzle_init();
    g->curr_puzzle = -1;
    // initialize player
    player_init(&g->p);
    // init all bosses
    boss_init_bosses();
    g->levelbosses = boss_get_bosses();
    // initialize environment stuff
    // BONFIRE
    // TODO bf_init(&g->bonfire);
    // CRATE
    crate_init(&g->crate);
    // KARIKKU
    karikku_init_all(g->karikku);
    // FIREFLY
    ffly_init_all(g->ffly);
    // initialize other actors/sprites
    // BATARANGS
    batr_init_all(g->batrs);
    // AANAMARUTHAS
    aanam_init_all(g->aanas);
    // initialize generic game object stuff
    g->t_ptr = 0;
    memset(g->typebuffer, 0, TYPEBUFFER_SIZE);
    g->is_gameover = false;
    g->is_game_wclosed = false;
    g->is_game_paused = false;
    g->is_boss_active = false;
    g->is_type_mode = false;
    g->is_type_buffer_done = false;
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
    // If game is in type mode, mostly because of puzzles,
    // our only job is to get the keypressed and put it into a buffer
    if(g->is_type_mode) {
        int key = GetKeyPressed();
        while(key > 0) {
            // only space and numbers allowed
            // using key type is not correct, but since raylib numbers
            // match with unicode this is a convenience to detect enter key
            // as well.
            if(key == KEY_SPACE || (key >= KEY_A && key <= KEY_Z)) {
                g->typebuffer[g->t_ptr] = key;
                g->t_ptr = (g->t_ptr + 1) % TYPEBUFFER_SIZE;
            } else if(key == KEY_ENTER) {
                g->typebuffer[g->t_ptr] = '\0';
                g->is_type_buffer_done = true;
            }
            key = GetKeyPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            g->t_ptr--;
            if (g->t_ptr < 0) g->t_ptr = 0;
            g->typebuffer[g->t_ptr] = '\0';
        }
    }
    // PLAYER
    // handle inputs and execute player actions
    // handle inputs only if player is not hurting
    bool player_move_conditions = !player_is_dying(&g->p) &&
        !player_is_hurting(&g->p) && !g->is_type_mode;
    
    if(player_move_conditions) {
        if(input_iskeydown(KEY_D)) {
            player_activate_move_r(&g->p, dt);
        }

        if(input_iskeydown(KEY_A)) {
            player_activate_move_l(&g->p, dt);
        }

        if(input_iskeypressed(KEY_SPACE) && !player_is_jumping(&g->p)) {
            player_activate_jump(&g->p, dt);
        }

        if(input_ismousepressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_pos = input_get_mouse_pos();
            // check for empty slot
            batr_t *b = batr_get_empty_slot(g->batrs);
            if(b) {
                player_activate_batr(&g->p, b, mouse_pos);
            }
        }

        if(input_ismousepressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_pos = input_get_mouse_pos();
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

    // CRATE
    crate_activate(&g->crate);
    crate_update(&g->crate, dt);

    if(g->crate.is_open) {
        crate_content_update(&g->crate, dt);
    }

    // karikkus: already activated, just update
    karikku_update_all(g->karikku, dt);

    // fireflys: active empty slots
    ffly_activate_all(g->ffly);

    // fireflys: update active ones
    ffly_update_all(g->ffly, dt);

    // aanams: activate empty slots
    // if game is in type mode, do not init new aanams
    bool aanam_ban = g->is_type_mode || (g->crate.is_open && g->crate.content.type == ARTIFACT);
    if(!aanam_ban) {
        aanam_activate_all(g->aanas, g->is_boss_active, dt);
    }

    // aanams: update active ones anyway
    aanam_update_all(g->aanas, dt);

    //////////////////////////////
    ////////// BOSS //////////////
    //////////////////////////////
    bool boss_activate_conditions = player_can_level_up(&g->p) && 
        !g->is_boss_active && !g->is_type_mode;
    if(boss_activate_conditions) {
        boss_set(g->p.curr_level);
        boss_activate();
        g->is_boss_active = true;
    }

    // active check happens inside update call
    if(g->is_boss_active && boss_is_dead()) {
        g->is_boss_active = false;
        g->p.curr_level++;
    }
    
    boss_update(dt);

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
            bool check = false;
            if(b_conds && orb_conds) {
                check = col_check_bbox(&b->obj, COORDS_WORLD, &orb->obj, COORDS_SCREEN);
            }
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
        bool vy_conds = g->levelbosses->vy.obj.is_active && !vy_is_dying(&g->levelbosses->vy);
        bool check = false;
        if(b_conds && vy_conds) {
            check = col_check_bbox(&g->levelbosses->vy.obj, COORDS_WORLD, &b->obj, COORDS_WORLD); 
        }
        if(check) {
            b->obj.is_active = false;
            g->levelbosses->vy.health -= VY_BATR_HEALTH_DECR;
            hbar_update(&g->levelbosses->vy.hbar, g->levelbosses->vy.health);
        }
    }
    // BATARANG WITH KCHATHAN
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g->batrs[i];
        bool b_conds = b->obj.is_active;
        bool kch_conds = g->levelbosses->kch.obj.is_active && !kch_is_dying(&g->levelbosses->kch);
        bool check = false;
        if(b_conds && kch_conds) {
            check = col_check_bbox(&g->levelbosses->kch.obj, COORDS_WORLD, &b->obj, COORDS_WORLD);
        }
        if(check) {
            b->obj.is_active = false;
            g->levelbosses->kch.health -= KCH_BATR_HEALTH_DECR;
            hbar_update(&g->levelbosses->kch.hbar, g->levelbosses->kch.health);
        }
    }
    // BATARANG WITH CRATE
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g->batrs[i];
        bool b_conds = b->obj.is_active;
        bool crate_conds = g->crate.obj.is_active && !g->crate.is_broken && (g->crate.obj.pos.y != (GAME_GROUND_Y - g->crate.crate_tex.height));
        bool check = false;
        if(b_conds && crate_conds) {
            check = col_check_bbox(&g->crate.obj, COORDS_WORLD, &b->obj, COORDS_WORLD);
        }
        if(check) {
            b->obj.is_active = false;
            g->crate.is_broken = true;
        }
    }
    // ORB WITH PLAYER
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_t *orb = &g->levelbosses->vy.orbs[i];
        bool orb_conds = orb->obj.is_active && !orb->is_hostile;
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(orb_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, COORDS_WORLD, &orb->obj, COORDS_SCREEN);
        }
        if(check) {
            orb->obj.is_active = false;
            player_set_hurt_shock(&g->p);
            g->p.health -= PLAYER_ORB_HEALTH_DECR;
        }
    }
    // SKBALL WITH PLAYER
    for(int i = 0; i < MAX_SKBALLS; ++i) {
        skball_t *skb = &g->levelbosses->kch.skballs[i];
        bool skb_conds = skb->obj.is_active;
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(skb_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, COORDS_WORLD, &skb->obj, COORDS_SCREEN);
        }
        if(check) {
            skb->obj.is_active = false;
            player_set_hurt_shock(&g->p);
            g->p.health -= PLAYER_SKBALL_HEALTH_DECR;
        }
    }
    // KARIKKU WITH PLAYER
    for(int i = 0; i < TOTAL_KARIKKU; ++i) {
        karikku_t *k = &g->karikku[i];
        bool k_conds = k->obj.is_active && !obj_is_oob(&k->obj, COORDS_WORLD);
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(k_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, COORDS_WORLD, &k->obj, COORDS_WORLD);
        }
        if(check) {
            k->obj.is_active = false;
            g->p.k_count++;
            g->p.score += 5;
            PlaySound(g->p.slurp);
        }
    }
    // CRATE WITH PLAYER
    {
        crate_t *cr = &g->crate;
        player_t *p = &g->p;

        bool crate_cond = cr->obj.is_active && !cr->is_broken && (cr->obj.pos.y == (GAME_GROUND_Y - cr->crate_tex.height));
        
        if(crate_cond && (p->obj.curr_anim == &p->anim_wlash)) {
            int curr_f_idx = anim_get_curr_frame_idx(g->p.obj.curr_anim);
            bool frame_cond = (curr_f_idx >= 2 && curr_f_idx <= 7);
            if(frame_cond) {
                if(col_check_bbox(&p->obj, COORDS_WORLD, &cr->obj, COORDS_WORLD)) {
                    g->crate.is_broken = true;
                }
            }
        }
        if(cr->is_open) {
            // check for collision with artifact
            bool content_on_ground = false;
            if(cr->content.type == ARTIFACT && CR_ARTIF(cr).obj.is_active && (g->curr_puzzle < 0)) {
                content_on_ground = (CR_ARTIF(cr).obj.pos.y == CR_ARTIF(cr).terminal_y);
                bool col_check = col_check_bbox(&p->obj, COORDS_WORLD, &CR_ARTIF(cr).obj, COORDS_WORLD);
                if(content_on_ground && col_check && !g->is_type_mode) {
                    g->is_type_mode = true;
                    g->curr_puzzle = puzzle_get();
                    // set artifact position to above the puzzle, centered
                    Vector2 puzzle_pos = puzzle_get_pos();
                    CR_ARTIF(cr).obj.pos.y = puzzle_pos.y - CR_ARTIF(cr).artifact_tex.height;
                    CR_ARTIF(cr).obj.pos.x = G_W/2 - CR_ARTIF(cr).artifact_tex.width/2;
                    CR_ARTIF(cr).obj.pos = obj_s2w_pos(CR_ARTIF(cr).obj.pos);
                    CR_ARTIF(cr).obj.vel.y = 0;
                    // make it stay there
                    CR_ARTIF(cr).gravity = false;
                }
            } else if(cr->content.type == TODDY && CR_TODDY(cr).obj.is_active) {
                content_on_ground = (CR_TODDY(cr).obj.pos.y == CR_TODDY(cr).terminal_y);
                if(content_on_ground && col_check_bbox(&p->obj, COORDS_WORLD, &CR_TODDY(cr).obj, COORDS_WORLD)) {
                    // pick up toddy
                    CR_TODDY(cr).obj.is_active = false;
                    p->health += 10;
                    p->health = _min(p->health, p->max_health);
                }
            }
            if(g->is_type_mode && g->is_type_buffer_done && (g->curr_puzzle >= 0)) {
                bool answered = puzzle_check(g->typebuffer, g->curr_puzzle);
                if(answered) {
                    g->curr_puzzle = -1;
                    g->is_type_mode = false;
                    CR_ARTIF(cr).is_won = true;
                    CR_ARTIF(cr).obj.is_active = false;
                    p->a_count++;
                    puzzle_play_solved();
                } else {
                    puzzle_play_wrong();
                }
                game_reset_typebuffer();
            }
        }
    }
    // KARIKKU WITH PLAYER
    for(int i = 0; i < TOTAL_KARIKKU; ++i) {
        karikku_t *k = &g->karikku[i];
        bool k_conds = k->obj.is_active && !obj_is_oob(&k->obj, COORDS_WORLD);
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(k_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, COORDS_WORLD, &k->obj, COORDS_WORLD);
        }
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
        bool check = false;
        if(orb_conds && vy_conds) {
            check = col_check_bbox(&g->levelbosses->vy.obj, COORDS_WORLD, &orb->obj, COORDS_SCREEN);
        }
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
        bool check = false;
        if(aana_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, COORDS_WORLD, &aana->obj, COORDS_WORLD);
        }
        if(check) {
            bool anim_cond = g->p.obj.curr_anim == &g->p.anim_wlash;
            int curr_f_idx = anim_get_curr_frame_idx(g->p.obj.curr_anim);
            bool frame_cond = (curr_f_idx >= 2 && curr_f_idx <= 7);
            bool pos_cond = ((g->p.obj.hdir == RIGHT) && (aana->obj.pos.x > g->p.obj.pos.x)) ||
                            ((g->p.obj.hdir == LEFT) && (aana->obj.pos.x < g->p.obj.pos.x));
            if(anim_cond && frame_cond && pos_cond) {
                // hit whip on aanam
                aana->is_dying = true;
                g->p.score += 50;
            } else {
                aana->hit_player = true;
                player_set_hurt_flash(&g->p);
                PlaySound(g->p.hurt);
                g->p.health -= 5;
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
                aana->obj.curr_anim->curr_frame.x = 0;
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

    if(g->levelbosses->vy.health <= 0) {
        g->levelbosses->vy.health = 0;
        g->levelbosses->vy.obj.is_active = 0;
    }

    if(g->levelbosses->kch.health <= 0) {
        g->levelbosses->kch.health = 0;
        g->levelbosses->kch.obj.is_active = false;
    }
}

// pause wrapper
void game_update(float dt) {
    bool escp = IsKeyPressed(KEY_ESCAPE);
    #ifdef __EMSCRIPTEN__
    escp = IsKeyDown(KEY_ESCAPE);
    #endif
    if(!escp && !g->is_game_paused) {
        _game_update(dt);
    } else {
        if(escp) {
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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void game_yield_frame(void) {
#ifdef __EMSCRIPTEN__
    // This tells the browser: "Pause this C loop, draw the frame, and come back to me in 2ms"
    emscripten_sleep(0); 
#else
    // Desktop fallback just handles standard frame pacing
#endif
}

void game_draw_canvas_to_screen(void) {
    Color overlay = (Color){ 255, 203, 0, 10};
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
        #ifdef __EMSCRIPTEN__
        PollInputEvents();
        #endif
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
        game_yield_frame();
    }
}

void game_start_main_loop(void) {
    while(!g->is_game_wclosed && !g->is_gameover) {
        float dt = GetFrameTime();
        #ifdef __EMSCRIPTEN__
        PollInputEvents();
        dt = 1.0f/60.0f;
        #endif
        
        game_update(dt);

        if(!g->is_game_paused) {
            BeginTextureMode(*g->canvas);

            BeginMode2D(g->cam);
            // Draw everything that moves with camera
            game_draw_inf_bg();
            player_draw(&g->p);
            // Draw bonfireTODO
            crate_draw(&g->crate);
            if(g->crate.is_open) {
                crate_content_draw(&g->crate);
            }
            karikku_draw_all(g->karikku);
            batr_draw_all(g->batrs);
            ffly_draw_all(g->ffly);
            aanam_draw_all(g->aanas);

            // Draw VY
            if(g->levelbosses->vy.obj.is_active) {
                vy_draw(&g->levelbosses->vy);
            }
            // Draw KCH
            if(g->levelbosses->kch.obj.is_active) {
                kchath_draw(&g->levelbosses->kch);
            }

            // Draw EPECHI
            // TODO
            EndMode2D();

            if(g->curr_puzzle >= 0) {
                puzzle_draw_q(g->curr_puzzle);
                puzzle_draw_textbox(g->typebuffer);
            }
            
            // Draw everything that doesn't move with camera
            orb_draw_all(g->levelbosses->vy.orbs);
            skball_draw_all(g->levelbosses->kch.skballs);
            if(g->levelbosses->vy.obj.is_active) {
                hbar_draw(&g->levelbosses->vy.hbar);
            }
            if(g->levelbosses->kch.obj.is_active) {
                hbar_draw(&g->levelbosses->kch.hbar);
            }
            hud_draw(&g->p);

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
        game_yield_frame();
    }
}

void game_deinit(void) {
    // deinit audio engine
    audio_deinit();
    // free game object memory
    free(g);
}