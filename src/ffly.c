#include <raylib.h>
#include <math.h>
#include <ffly.h>
#include <rand.h>

anim_asset_t ff_fly;

static bool g_anim_asset_loaded = false;

#define Y_L (150)
#define Y_H (G_H/2)
#define XDIV (10)
#define FFLY_VEL (100 + (vp_rand() % 50))

const float dec_x = (float)(G_W)/(float)(XDIV);

#include <stdio.h>
Vector2 ffly_get_vel(Vector2 prev, Vector2 next) {
    float dx = next.x - prev.x;
    float dy = next.y - prev.y;
    float d = sqrtf(dx*dx + dy*dy);
    if(d == 0) d = 1;

    Vector2 v;
    float speed = (float)(FFLY_VEL);
    v.x = speed * (dx/d);
    v.y = speed * (dy/d);

    return v;
}

void ffly_init(ffly_t *ff) {
    // ANIM
    if(!g_anim_asset_loaded) {
        // load animation assets
        anim_asset_load(ANIM_FIREFLY, &ff_fly);
    }
    Vector2 dim;
    dim = anim_asset_get_frame_dim(&ff_fly);
    ff->anim_fly.asset = &ff_fly;
    ff->anim_fly.timer = 0.0f;
    ff->anim_fly.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    ff->obj.curr_anim = &ff->anim_fly;
    ff->obj.size = (Vector2){
        ff->obj.curr_anim->curr_frame.width,
        ff->obj.curr_anim->curr_frame.height
    };
    ff->obj.is_active = false;
    ff->obj.cs = COORDS_WORLD;
}

void ffly_init_all(ffly_t *fflys) {
    for(int i = 0; i < MAX_FFLY; ++i) {
        ffly_init(&fflys[i]);
    }
}

void ffly_activate(ffly_t *ff) {
    ff->prev.y = vp_rand_lim(Y_L, Y_H);
    ff->prev.x = G_W - ff->obj.size.x;
    ff->prev = obj_s2w_pos(ff->prev);
    ff->next.y = vp_rand_lim(Y_L, Y_H);
    ff->next.x = ff->prev.x - dec_x;
    ff->reached_target = false;
    ff->obj.pos = ff->prev;
    ff->obj.is_active = true;
    ff->obj.vel = ffly_get_vel(ff->prev, ff->next);
}

void ffly_activate_all(ffly_t *ffs) {
    for(int i = 0; i < MAX_FFLY; ++i) {
        ffly_t *ff = &ffs[i];
        if(!ff->obj.is_active) {
            ffly_activate(ff);
        }
    }
}

void ffly_update(ffly_t *ff, float dt) {
    if(obj_is_oob(&ff->obj)) {
        ff->obj.is_active = false;
    } else {
        if(ff->reached_target) {
            Vector2 temp = ff->prev;
            ff->prev = ff->next;
            ff->next.x = temp.x - dec_x;
            ff->next.y = vp_rand_lim(Y_L, Y_H);
            ff->obj.vel = ffly_get_vel(ff->prev, ff->next);
            ff->reached_target = false;
        } else {
            float fdx = ff->obj.pos.x - ff->next.x;
            float fdy = ff->obj.pos.y - ff->next.y;
            float fd = sqrt((fdx*fdx) + (fdy*fdy));
            if(fd < 3.0) {
                // snap pos
                ff->obj.pos = ff->next;
                ff->reached_target = true;
            } else {
                ff->obj.pos.x += ff->obj.vel.x * dt;
                ff->obj.pos.y += ff->obj.vel.y * dt;
            }
        }
        anim_advance(ff->obj.curr_anim, dt);
    }
}

void ffly_update_all(ffly_t *ffs, float dt) {
    for(int i = 0; i < MAX_FFLY; ++i) {
        ffly_t *ff = &ffs[i];
        if(ff->obj.is_active) {
            ffly_update(ff, dt);
        }
    }
}

void ffly_draw(ffly_t *ff) {
    DrawTextureRec(
        ff->obj.curr_anim->asset->texture,
        ff->obj.curr_anim->curr_frame,
        ff->obj.pos,
        WHITE);
}

void ffly_draw_all(ffly_t *fflys) {
    for(int i = 0; i < MAX_FFLY; ++i) {
        if(fflys[i].obj.is_active) {
            ffly_draw(&fflys[i]);
        }
    }
}