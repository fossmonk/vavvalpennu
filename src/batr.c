#include <stdlib.h>
#include <raylib.h>
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