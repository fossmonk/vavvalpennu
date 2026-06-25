#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <player.h>
#include <anim_asset.h>
#include <anim.h>
#include <batr.h>
#include <obj.h>

#define PLAYER_VEL_X_DECAY     (-12)
#define PLAYER_VEL_Y_DECAY     (-2)
#define PLAYER_CENTER_TO_CHEST (70.0f)
#define JUMP_VEL_Y_0           (800.0f)
#define ACCEL_PUSH             (5800.0f)
#define P_HBAR_POS             (Vector2){5, 5}
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
extern anim_asset_t player_run;
extern anim_asset_t player_idle;
extern anim_asset_t player_jump;
extern anim_asset_t player_hithurt;
extern anim_asset_t player_flash;
extern anim_asset_t player_shock;
extern anim_asset_t player_wlash;

void player_init(player_t *p) {
    // ANIM
    Vector2 dim;
    // idle
    dim = anim_asset_get_frame_dim(&player_idle);
    p->anim_idle_r.asset = &player_idle;
    p->anim_idle_r.timer = 0.0f;
    p->anim_idle_r.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // run
    dim = anim_asset_get_frame_dim(&player_run);
    p->anim_run_r.asset = &player_run;
    p->anim_run_r.timer = 0.0f;
    p->anim_run_r.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // jump
    dim = anim_asset_get_frame_dim(&player_jump);
    p->anim_jump_r.asset = &player_jump;
    p->anim_jump_r.timer = 0.0f;
    p->anim_jump_r.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // hit hurt
    dim = anim_asset_get_frame_dim(&player_hithurt);
    p->anim_hithurt.asset = &player_hithurt;
    p->anim_hithurt.timer = 0.0f;
    p->anim_hithurt.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // flash
    dim = anim_asset_get_frame_dim(&player_flash);
    p->anim_flash.asset = &player_flash;
    p->anim_flash.timer = 0.0f;
    p->anim_flash.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // shock
    dim = anim_asset_get_frame_dim(&player_shock);
    p->anim_shock.asset = &player_shock;
    p->anim_shock.timer = 0.0f;
    p->anim_shock.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // whiplash
    dim = anim_asset_get_frame_dim(&player_wlash);
    p->anim_wlash.asset = &player_wlash;
    p->anim_wlash.timer = 0.0f;
    p->anim_wlash.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // AUDIO
    // whiplash
    p->whip = LoadSound(SOUND_PWHIP);
    // slurp
    p->slurp = LoadSound(SOUND_PSLURP);
    // jump
    p->jump = LoadSound(SOUND_PJUMP);
    // batarang whoosh
    p->batr_whoosh = LoadSound(SOUND_PBATR);
    // player hit hurt
    p->hurt = LoadSound(SOUND_PPAIN);
    
    // STATE
    p->obj.curr_anim = &p->anim_idle_r;
    float player_initx = G_W/2 - p->obj.curr_anim->curr_frame.width;
    float player_inity = GAME_GROUND_Y - p->obj.curr_anim->curr_frame.height;
    p->obj.is_active = true;
    p->obj.pos = (Vector2){player_initx, player_inity};
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
    p->score = 0;
    p->curr_level = 0;

    // init player healthbar
    hbar_init(&p->hbar, 
        P_HBAR_POS, P_HBAR_MAXW, P_HBAR_HEIGHT, P_HBAR_SPACING,
        PLAYER_HBAR_ICON, P_HBAR_POS, RED, p->max_health);
}

void player_activate_move_r(player_t *p, float dt) {
    p->obj.vel.x += ACCEL_PUSH*dt;
    // set curr_anim to run
    if(!player_is_jumping(p) && (p->obj.curr_anim != &p->anim_wlash)) {
        p->obj.curr_anim = &p->anim_run_r;
        p->obj.size = anim_get_framesize(p->obj.curr_anim);
    }
    // set hdir to right
    p->obj.hdir = RIGHT;
    // flip animation to right if left
    anim_hflipr(p->obj.curr_anim);
}

void player_activate_move_l(player_t *p, float dt) {
    p->obj.vel.x -= ACCEL_PUSH*dt;
    // set curr_anim to run
    if(!player_is_jumping(p) && (p->obj.curr_anim != &p->anim_wlash)) {
        p->obj.curr_anim = &p->anim_run_r;
        p->obj.size = anim_get_framesize(p->obj.curr_anim);
    }
    // set hdir to left
    p->obj.hdir = LEFT;
    // flip animation to left if right
    anim_hflipl(p->obj.curr_anim);
}

void player_activate_jump(player_t *p, float dt) {
    player_set_jump(p);
    p->obj.vel.y -= JUMP_VEL_Y_0;
    p->obj.curr_anim = &p->anim_jump_r;
    p->obj.size = anim_get_framesize(p->obj.curr_anim);
    // set the direction for jump animation
    p->obj.hdir == RIGHT ? anim_hflipr(p->obj.curr_anim) : anim_hflipl(p->obj.curr_anim);
    PlaySound(p->jump);
}

void player_activate_hurting(player_t *p, float dt) {
    bool act = true;
    if(p->actionmask & P_IS_HURTING_F) {
        p->obj.curr_anim = &p->anim_flash;
    } else if(p->actionmask & P_IS_HURTING_S) {
        p->obj.curr_anim = &p->anim_shock;
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
    p->obj.curr_anim = &p->anim_wlash;
    float psx = obj_w2s_pos(p->obj.pos).x;
    if(mouse_pos.x > psx) {
        anim_hflipr(p->obj.curr_anim);
    } else {
        anim_hflipl(p->obj.curr_anim);
    }
    // reset current animation to start
    anim_reset(p->obj.curr_anim);
    // play whip sound (async)
    PlaySound(p->whip);
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
    PlaySound(p->batr_whoosh);
}

void player_update(player_t *p, bool boss_active, float dt) {
    if(!player_is_hurting(p)) {
        // store initial position
        Vector2 initpos = p->obj.pos;
        // decay player velocity
        p->obj.vel.x *= expf(PLAYER_VEL_X_DECAY*dt);
        p->obj.vel.y *= expf(PLAYER_VEL_Y_DECAY*dt);
        // update yvel with gravity
        p->obj.vel.y += ACCEL_G*dt;
        // now update position
        p->obj.pos.x += p->obj.vel.x*dt;
        p->obj.pos.y += p->obj.vel.y*dt;

        // clamp y
        p->obj.pos.y = _min(p->obj.pos.y, GAME_GROUND_Y - p->obj.size.y);

        if(boss_active) {
            // clamp x to screen boundaries if boss is active
            // get world coordinates for screen x limits
            Vector2 player_screen_pos = obj_w2s_pos(p->obj.pos);
            player_screen_pos.x = _max(player_screen_pos.x, 0);
            player_screen_pos.x = _min(player_screen_pos.x, G_W - p->obj.size.x);
            p->obj.pos = obj_s2w_pos(player_screen_pos);
        } else {
            // clamp x with global limits
            p->obj.pos.x = _max(p->obj.pos.x, WORLD_XL);
            p->obj.pos.x = _min(p->obj.pos.x, WORLD_XR - p->obj.size.x);
        }

        // switch to idle animation if player is in ground
        if(player_is_jumping(p) && (p->obj.pos.y >= (float)(GAME_GROUND_Y - p->obj.size.y))) {
            player_clr_jump(p);
        }

        bool anim_check = (
                        (p->obj.curr_anim == &p->anim_run_r) || 
                        (p->obj.curr_anim == &p->anim_jump_r)
                        );

        if(!player_is_jumping(p) && anim_check && ((int)(p->obj.vel.x) == 0)) {
            p->obj.curr_anim = &p->anim_idle_r;
            // set direction for idle animation
            p->obj.hdir == RIGHT ? anim_hflipr(p->obj.curr_anim) : anim_hflipl(p->obj.curr_anim);
        }

        // handle whiplash
        if(p->obj.curr_anim == &p->anim_wlash) {
            p->obj.pos.x = initpos.x;
            if(anim_is_lastframe(p->obj.curr_anim)) {
                p->obj.curr_anim = &p->anim_idle_r;
            }
        }
    } else {
        // handle hurting
        if(!p->in_hurt_anim) {
            // switch to hurt animation
            p->in_hurt_anim = true;
            player_activate_hurting(p, dt);
        } else {
            // if hurt animation is over, switch to idle
            if(anim_is_lastframe(p->obj.curr_anim)) {
                p->in_hurt_anim = false;
                player_clr_hurt_burn(p);
                player_clr_hurt_shock(p);
                player_clr_hurt_flash(p);
                p->obj.curr_anim = &p->anim_idle_r;
            }
        }
    }

    // update animation
    anim_advance(p->obj.curr_anim, dt);
}

void player_draw(player_t *p) {
    DrawTextureRec(
        p->obj.curr_anim->asset->texture,
        p->obj.curr_anim->curr_frame,
        p->obj.pos,
        WHITE
    );
}