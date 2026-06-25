#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <batr.h>

// BATARANG ANIMATIONS
extern anim_asset_t batr_rotate;

void batr_init(batr_t *b) {
    // ANIM
    Vector2 dim;
    dim = anim_asset_get_frame_dim(&batr_rotate);
    b->anim_rotate.asset = &batr_rotate;
    b->anim_rotate.timer = 0.0f;
    b->anim_rotate.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    b->obj.curr_anim = &b->anim_rotate;
    b->obj.size = (Vector2){
        b->obj.curr_anim->curr_frame.width,
        b->obj.curr_anim->curr_frame.height
    };
    b->obj.is_active = false;
    // purposely don't set pos and vel.
    // this has to be init'ed when spawned.
}

void batr_update(batr_t *b, float dt) {
    // check for bounds
    if(obj_is_oob(&b->obj, COORDS_WORLD)) {
        b->obj.is_active = false;
    } else {
        // update positions with velocity
        b->obj.pos.x += b->obj.vel.x * dt;
        b->obj.pos.y += b->obj.vel.y * dt;

        // update animation
        anim_advance(b->obj.curr_anim, dt);
    }
}

void batr_update_all(batr_t *batrs, float dt) {
    for(int i = 0; i < MAX_BATRS; ++i) {
        if(batrs[i].obj.is_active) {
            batr_update(&batrs[i], dt);
        }
    }
}

batr_t *batr_get_empty_slot(batr_t *batrs) {
    batr_t *b = NULL;
    for(int i = 0; i < MAX_BATRS; ++i) {
        if(!batrs[i].obj.is_active) {
            b = &batrs[i];
            break;
        }
    }
    return b;
}

void batr_draw(batr_t *b) {
    DrawTextureRec(
        b->obj.curr_anim->asset->texture,
        b->obj.curr_anim->curr_frame,
        b->obj.pos,
        WHITE);
}

void batr_draw_all(batr_t *batrs) {
    for(int i = 0; i < MAX_BATRS; ++i) {
        if(batrs[i].obj.is_active) {
            batr_draw(&batrs[i]);
        }
    }
}