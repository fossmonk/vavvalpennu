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
#include <crate.h>
#include <dyn_arr.h>

// SOME MACRO FUNCTIONS
#ifndef _max
#define _max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef _min
#define _min(a, b) ((a) < (b) ? (a) : (b))
#endif

// GLOBALS
// top level game object
game_t *g;

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
    g->bg = LoadTexture(BACKGROUND);
    g->splash = LoadTexture(SPLASH_BG);
    g->pausemenu = LoadTexture(PAUSEMENU_BG);
    g->cursor = LoadTexture(BAT_CURSOR);
    g->game_font = LoadFontEx(MAIN_FONT, 96, NULL, 0);
    SetTextureFilter(g->game_font.texture, TEXTURE_FILTER_BILINEAR);
    g->bgmusic = LoadMusicStream(AUD_AMBIENT);
    
    // initialize menu
    menu_init();

    // initialize puzzle
    puzzle_init();
    g->curr_puzzle = -1;
    g->puzzle_tries = 3;

    // initiate and populate obstacle list. This needs to be passed to player_init
    obj_t **obstacle_list = NULL;
    dyn_arr_append(obstacle_list, &g->crate.obj);
    
    // initialize player
    player_init(&g->p, obstacle_list);
    
    // init all bosses
    boss_init_bosses(&g->bosses);
    g->curr_boss = NONE;
    
    // initialize environment stuff
    // TODO bf_init(&g->bonfire);
    crate_init(&g->crate);
    karikku_init_all(g->karikku);
    ffly_init_all(g->ffly);

    // initialize other actors/sprites
    batr_init_all(g->batrs);
    aanam_init_all(g->aanas);

    // initialize generic game object stuff
    game_reset_typebuffer();
    g->is_type_mode = false;

    g->is_gameover = false;
    g->is_game_wclosed = false;
    g->is_game_paused = false;
    g->is_boss_active = false;
    g->cam.rotation = 0;
    g->cam.zoom = 1;
    g->cam.target = obj_cxy(&g->p.obj);
    g->cam.offset = (Vector2){G_W/2, (GAME_GROUND_Y - g->p.obj.curr_anim->curr_frame.height/2)};

    // try player drop
    g->p.obj.pos = (Vector2){G_W/2 - 200, 10};
    g->p.obj.curr_anim = &g->p.anims.jump;
    g->p.obj.vel.y = 150.0f;
    
    // ALL DONE, START THE MUSIC!
    PlayMusicStream(g->bgmusic);
}

void _game_update(float dt) {
    // this is required to prevent long dt's due to a window resize etc
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
    // PLAYER CAN MOVE if !(DYING, HURTING, DANCING)
    bool player_move_conditions = !player_is_dying(&g->p) &&
                                  !player_is_hurting(&g->p) &&
                                  !player_is_dancing(&g->p);
    
    if(player_move_conditions) {
        if(input_iskeydown(KEY_D) && !g->is_type_mode) {
            player_activate_hmove(&g->p, RIGHT, dt);
        }

        if(input_iskeydown(KEY_A) && !g->is_type_mode) {
            player_activate_hmove(&g->p, LEFT, dt);
        }

        if(input_iskeypressed(KEY_SPACE)) {
            player_activate_jump(&g->p, dt);
        }

        if(input_ismousepressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_pos = input_get_mouse_pos();
            // check for empty slot
            batr_t *b = batr_get_empty_slot(g->batrs);
            if(b) {
                player_activate_batr(&g->p, b, mouse_pos);
            }
        }

        if(input_iskeypressed(KEY_R) && !g->is_type_mode) {
            Vector2 mouse_pos = input_get_mouse_pos();
            player_activate_whiplash(&g->p, mouse_pos);
        }
        
    }
    player_update(&g->p, dt);
    // clamp player x if boss is active or type mode
    if(g->is_boss_active || g->is_type_mode) {
        player_clamp_to_screenx(&g->p);
    } else {
        float player_x = g->p.obj.pos.x;
        // DEADZONE RIGHT LIMIT
        if((player_x - g->cam.target.x) > DZ_RL) {
            g->cam.target.x = player_x - DZ_RL;
        }
        // DEADZONE LEFT LIMIT
        if(player_x - g->cam.target.x < DZ_LL) {
            g->cam.target.x = player_x - DZ_LL;
        }
    }

    // BATARANGS: already activated by player input
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

    // KARIKKUS: already activated, just update
    karikku_update_all(g->karikku, dt);

    // FIREFLYS: active empty slots
    ffly_activate_all(g->ffly);

    // FIREFLYS: update active ones
    ffly_update_all(g->ffly, dt);

    // AANAMS: activate empty slots
    // if game is in type mode, do not init new aanams
    bool aanam_ban = g->is_type_mode || (g->crate.is_open && g->crate.content.type == ARTIFACT);
    if(!aanam_ban) {
        aanam_activate_all(g->aanas, g->is_boss_active, dt);
    }

    // AANAMS: update active ones anyway
    aanam_update_all(g->aanas, dt);

    //////////////////////////////
    ////////// BOSS //////////////
    //////////////////////////////
    bool boss_activate_conditions = player_can_level_up(&g->p) && 
        !g->is_boss_active && !g->is_type_mode;
    if(boss_activate_conditions) {
        g->curr_boss = boss_get_for_level(g->p.curr_level);
        boss_activate(g->curr_boss);
        g->is_boss_active = true;
    }

    // boss specific active check happens inside update call
    if(g->is_boss_active && boss_is_dead(g->curr_boss)) {
        g->is_boss_active = false;
        g->curr_boss = NONE;
        g->p.curr_level++;
    }
    
    boss_update(g->curr_boss, dt);

    // TODO: move this to colman.c -> collision manager..

    /////////////////////////////////////////////////
    //////////// COLLISION DETECTION ////////////////
    /////////////////////////////////////////////////
    // BATARANG WITH ORB
    for(int i = 0; i < MAX_BATRS; ++i) {
        for(int j = 0; j < MAX_ORBS; ++j) {
            batr_t *b = &g->batrs[i];
            orb_t *orb = &g->bosses.vy.orbs[j];
            bool b_conds = b->obj.is_active;
            bool orb_conds = orb->obj.is_active;
            bool check = false;
            if(b_conds && orb_conds) {
                check = col_check_bbox(&b->obj, &orb->obj, NULL);
            }
            if(check) {
                // deactivate batr
                b->obj.is_active = false;
                // change velocity of orb and aim it at vy
                float v = Vector2Length(b->obj.vel);
                orb->obj.vel = orb_get_hostile_vel(g->bosses.vy.obj.pos, orb, v);
                // set orb as hostile
                orb->is_hostile = true;
            }
        }
    }
    // BATARANG WITH VY
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g->batrs[i];
        bool b_conds = b->obj.is_active;
        bool vy_conds = g->bosses.vy.obj.is_active && !vy_is_dying(&g->bosses.vy);
        bool check = false;
        if(b_conds && vy_conds) {
            check = col_check_bbox(&g->bosses.vy.obj, &b->obj, NULL); 
        }
        if(check) {
            b->obj.is_active = false;
            vy_decr_health_batr(&g->bosses.vy);
            hbar_update(&g->bosses.vy.hbar, g->bosses.vy.health);
        }
    }
    // BATARANG WITH KCHATHAN
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g->batrs[i];
        bool b_conds = b->obj.is_active;
        bool kch_conds = g->bosses.kch.obj.is_active && !kch_is_dying(&g->bosses.kch);
        bool check = false;
        if(b_conds && kch_conds) {
            check = col_check_bbox(&g->bosses.kch.obj, &b->obj, NULL);
        }
        if(check) {
            b->obj.is_active = false;
            kch_decr_health_batr(&g->bosses.kch);
            hbar_update(&g->bosses.kch.hbar, g->bosses.kch.health);
        }
    }
    // BATARANG WITH CRATE
    for(int i = 0; i < MAX_BATRS; ++i) {
        batr_t *b = &g->batrs[i];
        bool b_conds = b->obj.is_active;
        bool crate_conds = g->crate.obj.is_active && !g->crate.is_broken && (g->crate.obj.pos.y != (GAME_GROUND_Y - g->crate.crate_tex.height));
        bool check = false;
        if(b_conds && crate_conds) {
            check = col_check_bbox(&g->crate.obj, &b->obj, NULL);
        }
        if(check) {
            b->obj.is_active = false;
            g->crate.is_broken = true;
        }
    }
    // ORB WITH PLAYER
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_t *orb = &g->bosses.vy.orbs[i];
        bool orb_conds = orb->obj.is_active && !orb->is_hostile;
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(orb_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, &orb->obj, NULL);
        }
        if(check) {
            orb->obj.is_active = false;
            player_set_hurt_shock(&g->p);
            player_decr_health_orb(&g->p);
        }
    }
    // SKBALL WITH PLAYER
    for(int i = 0; i < MAX_SKBALLS; ++i) {
        skball_t *skb = &g->bosses.kch.skballs[i];
        bool skb_conds = skb->obj.is_active;
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(skb_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, &skb->obj, NULL);
        }
        if(check) {
            skb->obj.is_active = false;
            player_set_hurt_shock(&g->p);
            player_decr_health_skball(&g->p);
        }
    }
    // KARIKKU WITH PLAYER
    for(int i = 0; i < TOTAL_KARIKKU; ++i) {
        karikku_t *k = &g->karikku[i];
        bool k_conds = k->obj.is_active && !obj_is_oob(&k->obj);
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(k_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, &k->obj, NULL);
        }
        if(check) {
            k->obj.is_active = false;
            g->p.k_count++;
            g->p.score += 5;
            PlaySound(g->p.sounds.slurp);
        }
    }
    // CRATE WITH PLAYER
    {
        crate_t *cr = &g->crate;
        player_t *p = &g->p;

        bool crate_cond = cr->obj.is_active && !cr->is_broken && (cr->obj.pos.y == (GAME_GROUND_Y - cr->crate_tex.height));
        
        if(crate_cond && (p->obj.curr_anim->id.id == p->anims.wlash.id.id)) {
            int curr_f_idx = anim_get_curr_frame_idx(g->p.obj.curr_anim);
            bool frame_cond = (curr_f_idx >= 2 && curr_f_idx <= 7);
            if(frame_cond) {
                if(col_check_bbox(&p->obj, &cr->obj, NULL)) {
                    g->crate.is_broken = true;
                }
            }
        }
        if(cr->is_open) {
            // check for collision with artifact
            bool content_on_ground = false;
            if(cr->content.type == ARTIFACT && CR_ARTIF(cr).obj.is_active && (g->curr_puzzle < 0)) {
                content_on_ground = (CR_ARTIF(cr).obj.pos.y == CR_ARTIF(cr).terminal_y);
                bool col_check = col_check_bbox(&p->obj, &CR_ARTIF(cr).obj, NULL);
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
            } else if(cr->content.type == POTION && CR_POTION(cr).obj.is_active) {
                content_on_ground = (CR_POTION(cr).obj.pos.y == CR_POTION(cr).terminal_y);
                if(content_on_ground && col_check_bbox(&p->obj, &CR_POTION(cr).obj, NULL)) {
                    // pick up potion
                    CR_POTION(cr).obj.is_active = false;
                    p->health += 10;
                    p->health = _min(p->health, p->max_health);
                    PlaySound(p->sounds.potion);
                }
            }
            if(g->is_type_mode && g->is_type_buffer_done && (g->curr_puzzle >= 0) && (g->puzzle_tries > 0)) {
                bool answered = puzzle_check(g->typebuffer, g->curr_puzzle);
                if(answered) {
                    g->curr_puzzle = -1;
                    g->puzzle_tries = 3;
                    g->is_type_mode = false;
                    CR_ARTIF(cr).is_won = true;
                    CR_ARTIF(cr).obj.is_active = false;
                    p->a_count++;
                    p->score += 100;
                    puzzle_play_solved();
                } else {
                    --g->puzzle_tries;
                    puzzle_play_wrong();
                }
                if(g->puzzle_tries == 0) {
                    // reset everything
                    g->curr_puzzle = -1;
                    g->puzzle_tries = 3;
                    g->is_type_mode = false;
                    CR_ARTIF(cr).is_won = false;
                    CR_ARTIF(cr).obj.is_active = false;
                }
                game_reset_typebuffer();
            }
        }
    }
    // KARIKKU WITH PLAYER
    for(int i = 0; i < TOTAL_KARIKKU; ++i) {
        karikku_t *k = &g->karikku[i];
        bool k_conds = k->obj.is_active && !obj_is_oob(&k->obj);
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(k_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, &k->obj, NULL);
        }
        if(check) {
            k->obj.is_active = false;
            g->p.k_count++;
            g->p.score += 5;
            PlaySound(g->p.sounds.slurp);
        }
    }
    // HOSTILE ORB WITH VY
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_t *orb = &g->bosses.vy.orbs[i];
        bool orb_conds = orb->obj.is_active && orb->is_hostile;
        bool vy_conds = !vy_is_dying(&g->bosses.vy);
        bool check = false;
        if(orb_conds && vy_conds) {
            check = col_check_bbox(&g->bosses.vy.obj, &orb->obj, NULL);
        }
        if(check) {
            orb->obj.is_active = false;
            vy_set_hurt_shock(&g->bosses.vy);
            g->bosses.vy.health -= VY_BATR_HEALTH_DECR*5;
            hbar_update(&g->bosses.vy.hbar, g->bosses.vy.health);
            PlaySound(g->bosses.vy.hurt);
        }
    }
    // AANAM WITH PLAYER
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_t *aana = &g->aanas[i];
        bool aana_conds = aana->obj.is_active && !aana->is_dying && !aana->hit_player;
        bool player_conds = !player_is_dying(&g->p);
        bool check = false;
        if(aana_conds && player_conds) {
            check = col_check_bbox(&g->p.obj, &aana->obj, NULL);
        }
        if(check) {
            bool anim_cond = g->p.obj.curr_anim->id.id == g->p.anims.wlash.id.id;
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
                PlaySound(g->p.sounds.hurt);
                g->p.health -= 5;
            }
        }
    }

    // check for player death
    if(g->p.health <= 0) {
        player_set_die(&g->p);
    }

    if(g->bosses.vy.health <= 0) {
        g->bosses.vy.health = 0;
        g->bosses.vy.obj.is_active = false;
    }

    if(g->bosses.kch.health <= 0) {
        g->bosses.kch.health = 0;
        g->bosses.kch.obj.is_active = false;
        for(int i = 0; i < MAX_SKBALLS; ++i) {
            g->bosses.kch.skballs[i].obj.is_active = false;
        }
    }
}

// pause wrapper
void game_update(float dt) {
    bool escp = IsKeyPressed(KEY_ESCAPE);
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
    DrawTexture(g->cursor, SPREAD_VEC(mpos), WHITE);
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

void game_fade_into_main(void) {
    if(!g->is_game_wclosed && !WindowShouldClose()) {
        const float fade_duration = 3.0f;
        float fade_timer = 0.0f;
    
        Shader fade_in = LoadShader(NULL, FADE_IN_SHADER);
        Texture2D fade_bg = LoadTexture(FADE_IN_TEXTURE);
        Sound howl = LoadSound(SOUND_HOWL);
        PlaySound(howl);
        int utime = GetShaderLocation(fade_in, "time");
        
        while(fade_timer <= fade_duration) {
            SetShaderValue(fade_in, utime, &fade_timer, SHADER_UNIFORM_FLOAT);
            float dt = GetFrameTime();
            fade_timer += dt;
    
            BeginTextureMode(*g->canvas);
            DrawTexture(fade_bg, 0, 0, DARKGRAY);
            BeginShaderMode(fade_in);
            Rectangle src = {0.0f, 0.0f, fade_bg.width, fade_bg.height};
            Rectangle dst = {0.0f, 0.0f, fade_bg.width, fade_bg.height};
            DrawTexturePro(fade_bg, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            EndShaderMode();
            EndTextureMode();
            game_draw_canvas_to_screen();
        }
    
        UnloadSound(howl);
        UnloadShader(fade_in);
    }
}

void game_start_main_loop(void) {
    // Play the batgirl theme
    PlaySound(g->p.sounds.theme);
    while(!g->is_game_wclosed && !g->is_gameover) {
        float dt = GetFrameTime();
        
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

            if(boss_is_active(g->curr_boss)) {
                boss_draw(g->curr_boss);
            }
            EndMode2D();
            // Draw everything that doesn't move with camera
            
            if(g->curr_puzzle >= 0) {
                puzzle_draw_q(g->curr_puzzle);
                puzzle_draw_textbox(g->typebuffer);
            }

            if(boss_is_active(g->curr_boss)) {
                if(g->curr_boss == VADAYAKSHI) {
                    orb_draw_all(g->bosses.vy.orbs);
                    hbar_draw(&g->bosses.vy.hbar);
                } else if(g->curr_boss == KCHATHAN) {

                    hbar_draw(&g->bosses.kch.hbar);
                    skball_draw_all(g->bosses.kch.skballs);
                }
            }

            hud_draw(&g->p);
            EndTextureMode();
            // All things drawn to canvas, now blit
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