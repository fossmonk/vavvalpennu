#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <player.h>
#include <anim_asset.h>
#include <anim.h>
#include <batr.h>
#include <obj.h>
#include <levels.h>
#include <hud.h>
#include <collisions.h>
#include <obstacles.h>

#define PLAYER_VEL_X_DECAY     (-12)
#define PLAYER_VEL_Y_DECAY     (-2.5)
#define PLAYER_VEL_X_ROUNDOFF  (2.0f)
#define PLAYER_VEL_Y_ROUNDOFF  (1E-1)
#define PLAYER_CENTER_TO_CHEST (70.0f)
#define JUMP_VEL_Y_0           (700.0f)
#define ACCEL_PUSH             (6500.0f)
#define P_HBAR_POS             (Vector2){12, 12}
#define P_HBAR_ICON_POS        (Vector2){5, 5}
#define P_HBAR_MAXW            300
#define P_HBAR_HEIGHT          20
#define P_HBAR_SPACING         1

#ifndef _max
#define _max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef _min
#define _min(a, b) ((a) < (b) ? (a) : (b))
#endif

// PLAYER ANIMATIONS
anim_asset_t player_idle;
anim_asset_t player_run;
anim_asset_t player_jump;
anim_asset_t player_hithurt;
anim_asset_t player_flash;
anim_asset_t player_shock;
anim_asset_t player_wlash;

#define IDLE      0xAB0
#define RUN       0xAB1
#define JUMP      0xAB2
#define HITHURT   0xAB3
#define FLASH     0xAB4
#define SHOCK     0xAB5
#define WLASH     0xAB6

const anim_id_t player_idle_id = { IDLE };
const anim_id_t player_run_id = { RUN };
const anim_id_t player_jump_id = { JUMP };
const anim_id_t player_hithurt_id = { HITHURT };
const anim_id_t player_flash_id = { FLASH };
const anim_id_t player_shock_id = { SHOCK };
const anim_id_t player_wlash_id = { WLASH };

static bool g_anim_asset_loaded = false;

void player_init(player_t *p, obj_t **obstacle_list) {
    // ANIM
    if(!g_anim_asset_loaded) {
        // load animation assets
        anim_asset_load(ANIM_P_RUN, &player_run);
        anim_asset_load(ANIM_P_IDLE, &player_idle);
        anim_asset_load(ANIM_P_JUMP, &player_jump);
        anim_asset_load(ANIM_P_HITHURT, &player_hithurt);
        anim_asset_load(ANIM_P_FLASH, &player_flash);
        anim_asset_load(ANIM_P_SHOCK, &player_shock);
        anim_asset_load(ANIM_P_WLASH, &player_wlash);
    }

    Vector2 dim;
    // idle
    dim = anim_asset_get_frame_dim(&player_idle);
    p->anims.idle.asset = &player_idle;
    p->anims.idle.timer = 0.0f;
    p->anims.idle.id = player_idle_id;
    p->anims.idle.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // run
    dim = anim_asset_get_frame_dim(&player_run);
    p->anims.run.asset = &player_run;
    p->anims.run.timer = 0.0f;
    p->anims.run.id = player_run_id;
    p->anims.run.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // jump
    dim = anim_asset_get_frame_dim(&player_jump);
    p->anims.jump.asset = &player_jump;
    p->anims.jump.timer = 0.0f;
    p->anims.jump.id = player_jump_id;
    p->anims.jump.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // hit hurt
    dim = anim_asset_get_frame_dim(&player_hithurt);
    p->anims.hithurt.asset = &player_hithurt;
    p->anims.hithurt.timer = 0.0f;
    p->anims.hithurt.id = player_hithurt_id;
    p->anims.hithurt.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // flash
    dim = anim_asset_get_frame_dim(&player_flash);
    p->anims.flash.asset = &player_flash;
    p->anims.flash.timer = 0.0f;
    p->anims.flash.id = player_flash_id;
    p->anims.flash.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // shock
    dim = anim_asset_get_frame_dim(&player_shock);
    p->anims.shock.asset = &player_shock;
    p->anims.shock.timer = 0.0f;
    p->anims.shock.id = player_shock_id;
    p->anims.shock.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // whiplash
    dim = anim_asset_get_frame_dim(&player_wlash);
    p->anims.wlash.asset = &player_wlash;
    p->anims.wlash.timer = 0.0f;
    p->anims.wlash.id = player_wlash_id;
    p->anims.wlash.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // AUDIO
    // whiplash
    p->sounds.whip = LoadSound(SOUND_PWHIP);
    // slurp
    p->sounds.slurp = LoadSound(SOUND_PSLURP);
    // jump
    p->sounds.jump = LoadSound(SOUND_PJUMP);
    // batarang whoosh
    p->sounds.batr_whoosh = LoadSound(SOUND_PBATR);
    // player hit hurt
    p->sounds.hurt = LoadSound(SOUND_PPAIN);
    // player drink potion
    p->sounds.potion = LoadSound(SOUND_PPOTION);
    // player theme
    p->sounds.theme = LoadSound(SOUND_PTHEME);
    
    // STATE
    p->obj.curr_anim = &p->anims.idle;
    float player_initx = G_W/2 - p->obj.curr_anim->curr_frame.width;
    float player_inity = GAME_GROUND_Y - p->obj.curr_anim->curr_frame.height;
    p->obj.is_active = true;
    p->obj.pos = (Vector2){player_initx, player_inity};
    p->prev_pos = (Vector2){-1, -1};
    p->obj.vel = (Vector2){0, 0};
    p->obj.size = (Vector2){
        p->obj.curr_anim->curr_frame.width, 
        p->obj.curr_anim->curr_frame.height
    };
    p->obj.hdir = RIGHT;
    p->max_health = PLAYER_MAX_HEALTH_INITIAL;
    p->health = p->max_health;
    p->actionmask = 0;
    p->k_count = 0;
    p->a_count = 0;
    p->score = 0;
    p->curr_level = 0;
    p->obstacle_list = obstacle_list;

    // init player healthbar
    hbar_init(&p->hbar, 
        P_HBAR_POS, P_HBAR_MAXW, P_HBAR_HEIGHT, P_HBAR_SPACING,
        PLAYER_HBAR_ICON, P_HBAR_ICON_POS, RED, p->max_health);
    // init player hud
    hud_init();
}


bool player_can_move(player_t *p, hdir_t hdir) {
    // the player can move to the right, if there are no obstacles to the right
    // loop through list of obstacles, check if player bbox is colliding with obstacle bbox
    // and player right limit is >= obstacle left limit
    bool can_move = true;
    int num_obs = obs_da_len(p->obstacle_list);
    col_details_2d c_details;
    for(int i = 0; i < num_obs; ++i) {
        obj_t *obs = p->obstacle_list[i];
        if(obs->is_active) {
            bool is_colliding = col_check_bbox(&p->obj, COORDS_WORLD, obs, COORDS_WORLD, &c_details);
            bool check = (hdir == RIGHT) ? c_details.rl : c_details.lr;
            can_move = !(is_colliding && check);
            if(!can_move) break;
        }
    }
    return can_move;
}

// TODO: CLEAN UP THIS FUNCTION
bool player_is_grounded(player_t *p, float *curr_ground) {
    bool is_grounded = false;
    Rectangle bbox_rect = obj_get_bbox_rect(&p->obj);
    float ground_clearance = 2.0f;
    float player_b_y = p->obj.pos.y + bbox_rect.y + bbox_rect.height;
    
    // check if there are any obstacles below
    int num_obs = obs_da_len(p->obstacle_list);
    col_details_2d c_details;
    bool obs_below = false;
    float ground_y = -1;
    for(int i = 0; i < num_obs; ++i) {
        obj_t *obs = p->obstacle_list[i];

        if(!obs->is_active) continue;
        
        bool is_colliding = col_check_bbox(&p->obj, COORDS_WORLD, obs, COORDS_WORLD, &c_details);
        if(is_colliding && c_details.bt) {
            ground_y = obs->pos.y;
            obs_below = true;
            break;
        }
    }

    if(obs_below) {
        is_grounded = true;
    } else {
        ground_y = GAME_GROUND_Y;
        float delta_y = ground_y - player_b_y;
        if((delta_y >= 0) && (delta_y <  ground_clearance)) {
            is_grounded = true;
        } else {
            is_grounded = false;
        }
    }

    if(is_grounded && curr_ground) {
        *curr_ground = ground_y;
    }

    return is_grounded;
}

void player_activate_hmove(player_t *p, hdir_t hdir, float dt) {
    // set hdir
    p->obj.hdir = hdir;
    float vel_mult = (hdir == RIGHT) ? 1 : -1;
    // switch to run only if player can move.
    if(player_can_move(p, hdir)) {
        // change player animation to run only if current animation is idle
        if(p->obj.curr_anim->id.id == p->anims.idle.id.id) {
            p->obj.curr_anim = &p->anims.run;
            p->obj.size = anim_get_framesize(p->obj.curr_anim);
        }
        // flip animation if hdir has changed
        hdir == RIGHT ? anim_hflipr(p->obj.curr_anim) : anim_hflipl(p->obj.curr_anim);
        p->obj.vel.x += vel_mult*ACCEL_PUSH*dt;
    } else {
        p->obj.vel.x = 0;
    }
}

void player_activate_jump(player_t *p, float dt) {
    // check if the player is grounded
    if(player_is_grounded(p, NULL)) {
        // set animation to jump
        p->obj.curr_anim = &p->anims.jump;
        p->obj.size = anim_get_framesize(p->obj.curr_anim);
        // set animation orientation
        p->obj.hdir == RIGHT ? anim_hflipr(p->obj.curr_anim) : anim_hflipl(p->obj.curr_anim);
        // set initial vertical velocity
        p->obj.vel.y = -JUMP_VEL_Y_0;
        PlaySound(p->sounds.jump);
    }
}

void player_activate_hurting(player_t *p, float dt) {
    bool act = true;
    if(p->actionmask & P_IS_HURTING_F) {
        p->obj.curr_anim = &p->anims.flash;
    } else if(p->actionmask & P_IS_HURTING_S) {
        p->obj.curr_anim = &p->anims.shock;
    } else {
        act = false;
    }
    if(act) {
        p->obj.size = anim_get_framesize(p->obj.curr_anim);
        if(p->obj.hdir == LEFT) {
            anim_hflipl(p->obj.curr_anim);
        } else {
            anim_hflipr(p->obj.curr_anim);
        }
    }
    // reset current animation to start
    anim_reset(p->obj.curr_anim);
}

void player_activate_whiplash(player_t *p, Vector2 mouse_pos) {
    p->obj.curr_anim = &p->anims.wlash;
    float psx = obj_w2s_pos(p->obj.pos).x;
    if(mouse_pos.x > psx) {
        p->obj.hdir = RIGHT;
        anim_hflipr(p->obj.curr_anim);
    } else {
        p->obj.hdir = LEFT;
        anim_hflipl(p->obj.curr_anim);
    }
    // reset current animation to start
    anim_reset(p->obj.curr_anim);
    // play whip sound (async)
    PlaySound(p->sounds.whip);
}

void player_activate_batr(player_t *p, batr_t *b, Vector2 pos) {
    b->obj.is_active = true;
    b->obj.curr_anim->curr_frame.x = 0;
    float delta = PLAYER_CENTER_TO_CHEST;
    b->obj.pos = obj_cxy(&p->obj);
    b->obj.pos.y -= delta;
    Vector2 spawn_pos_s = obj_w2s_pos(b->obj.pos);
    float dx = pos.x - spawn_pos_s.x;
    float dy = pos.y - spawn_pos_s.y;
    float d  = sqrt(dx*dx + dy*dy);
    if(d == 0.0f)d = 1.0f;
    float sintheta = dy/d;
    float costheta = dx/d;
    b->obj.vel.x = BATARANG_VEL_R * costheta;
    b->obj.vel.y = BATARANG_VEL_R * sintheta;
    PlaySound(p->sounds.batr_whoosh);
}

void player_clamp_to_screenx(player_t *p) {
    // clamp x to screen boundaries
    // get world coordinates for screen x limits
    Vector2 player_screen_pos = obj_w2s_pos(p->obj.pos);
    player_screen_pos.x = _max(player_screen_pos.x, 0);
    player_screen_pos.x = _min(player_screen_pos.x, G_W - p->obj.size.x);
    p->obj.pos = obj_s2w_pos(player_screen_pos);
}

void player_update(player_t *p, float dt) {
    // store initial position
    p->prev_pos = p->obj.pos;
    // friction, air drag, gravity
    p->obj.vel.x *= expf(PLAYER_VEL_X_DECAY*dt);
    p->obj.vel.y *= expf(PLAYER_VEL_Y_DECAY*dt);
    p->obj.vel.y += ACCEL_G*dt;

    // round to zero
    if(fabsf(p->obj.vel.x) < PLAYER_VEL_X_ROUNDOFF) p->obj.vel.x = 0;
    if(fabsf(p->obj.vel.y) < PLAYER_VEL_Y_ROUNDOFF) p->obj.vel.y = 0;
    /*
    position update is conditional on certain things
    if the player is hurting, they are stunned. 
    so they can't move. So we don't update position
    if the player is blocked due to an obstacle
    then we don't update position.
    */
    // handle player_is_hurting
    if(player_is_hurting(p)) {
        if(!p->in_hurt_anim) {
            // switch to hurt animation
            p->in_hurt_anim = true;
            player_activate_hurting(p, dt);
        } else if(anim_is_lastframe(p->obj.curr_anim)) {
            // if hurt animation is over, switch to idle
            p->in_hurt_anim = false;
            player_clr_hurt_burn(p);
            player_clr_hurt_shock(p);
            player_clr_hurt_flash(p);
            p->obj.curr_anim = &p->anims.idle;
        }
    } else {
        Rectangle bbox_rect = obj_get_bbox_rect(&p->obj);
        float player_b_y = bbox_rect.height + bbox_rect.y;
        /*
        Y Position
        find out if the player is grounded
        check for obstacles below player, 
        update y position. If there are none,
        then clamp y to GAME_GROUND_Y.
        */
        // update y
        float curr_ground;
        bool is_grounded = player_is_grounded(p, &curr_ground);
        if(!is_grounded) {
            p->obj.pos.y += p->obj.vel.y*dt;
        } else if (p->obj.vel.y > 0.0f) {
            p->obj.vel.y = 0.0f;
        }
        /*
        X position
        velocity is already zeroed out if there are obstacles.
        so unconditionally update the x position
        */
        p->obj.pos.x += p->obj.vel.x*dt;
       
       // clamp x and y with world boundaries
        p->obj.pos.y = _min(p->obj.pos.y, GAME_GROUND_Y - player_b_y);
        p->obj.pos.x = _max(p->obj.pos.x, WORLD_XL);
        p->obj.pos.x = _min(p->obj.pos.x, WORLD_XR - p->obj.size.x);

        // handle whiplash
        if(p->obj.curr_anim->id.id == p->anims.wlash.id.id) {
            // undo all the position updates
            p->obj.pos.x = p->prev_pos.x;
            if(anim_is_lastframe(p->obj.curr_anim)) p->obj.curr_anim = &p->anims.idle;
        } else if(is_grounded) {
            // if player is grounded, stop jump
            p->obj.pos.y = curr_ground - player_b_y;
            player_clr_jump(p);
            // in grounded position if player has x vel switch to run anim, else idle.
            p->obj.curr_anim = p->obj.vel.x != 0.0f ? &p->anims.run : &p->anims.idle;
            p->obj.hdir == RIGHT ? anim_hflipr(p->obj.curr_anim) : anim_hflipl(p->obj.curr_anim);
        }

    }
    // always update health bar
    hbar_update(&p->hbar, p->health);
    // update animation as last step
    anim_advance(p->obj.curr_anim, dt);
}

bool player_can_level_up(player_t *p) {
    static const int l_up_scores[] = {
        [VP_L0] = 1000,
        [VP_L1] = 2500,
        [VP_L2] = 4000,
        [VP_L3] = 5500
    };

    return (p->score > l_up_scores[p->curr_level]);
}

void player_decr_health(player_t *p, int amount) {
    p->health -= amount;
    if(p->health < 0) p->health = 0;
}

void player_draw(player_t *p) {
    DrawTextureRec(
        p->obj.curr_anim->asset->texture,
        p->obj.curr_anim->curr_frame,
        p->obj.pos,
        WHITE
    );

    #ifdef DEBUG
    bbox_draw(p->obj.curr_anim->asset->bbox, p->obj.pos, WHITE);
    #endif
}