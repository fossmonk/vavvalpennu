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
#define ACCEL_PUSH             (4400.0f)
#define P_HBAR_POS             (Vector2){12, 12}
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

    // init player healthbar
    hbar_init(&p->hbar, P_HBAR_POS, P_HBAR_MAXW, P_HBAR_HEIGHT, P_HBAR_SPACING);
    // load hbar icon
    p->hbar_icon = LoadTexture(PLAYER_HBAR_ICON);
    p->hbar_iconpos = P_HBAR_POS;
    p->hbar_iconpos.x -= 7.0;
    p->hbar_iconpos.y -= 7.0;
}

void player_activate_move_r(player_t *p, float dt) {
    p->obj.vel.x += ACCEL_PUSH*dt;
    // set curr_anim to run
    if(!player_is_jumping(p)) {
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
    if(!player_is_jumping(p)) {
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

}

void player_activate_hurting(player_t *p, float dt) {
    bool act = true;
    if(p->actionmask & IS_HURTING_F) {
        p->obj.curr_anim = &p->anim_flash;
    } else if(p->actionmask & IS_HURTING_S) {
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

void player_activate_batr(player_t *p, batr_t *b, Vector2 pos, float dt) {
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
}

void player_update(player_t *p, bool boss_active, float dt) {
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

    // clamp x if boss is active
    if(boss_active) {
        // get world coordinates for screen x limits
        Vector2 player_screen_pos = obj_w2s_pos(p->obj.pos);
        player_screen_pos.x = _max(player_screen_pos.x, 0);
        player_screen_pos.x = _min(player_screen_pos.x, G_W - p->obj.size.x);
        p->obj.pos = obj_s2w_pos(player_screen_pos);
    }

    // switch to idle animation if player is in ground
    if(player_is_jumping(p) && (p->obj.pos.y >= (float)(GAME_GROUND_Y - p->obj.size.y))) {
        player_clr_jump(p);
    }

    if(!player_is_jumping(p) && ((int)(p->obj.vel.x) == 0)) {
        p->obj.curr_anim = &p->anim_idle_r;
        // set direction for idle animation
        p->obj.hdir == RIGHT ? anim_hflipr(p->obj.curr_anim) : anim_hflipl(p->obj.curr_anim);
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