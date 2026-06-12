#include <raylib.h>
#include <vpconfig.h>
#include <bonfire.h>

extern anim_asset_t bonfire_burn;

void bf_init(bf_t *bf) {
    // ANIM
    Vector2 dim;
    dim = anim_asset_get_frame_dim(&bonfire_burn);
    bf->anim_burn.asset = &bonfire_burn;
    bf->anim_burn.timer = 0.0f;
    bf->anim_burn.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    bf->obj.curr_anim = &bf->anim_burn;
    bf->obj.size = (Vector2){
        bf->obj.curr_anim->curr_frame.width,
        bf->obj.curr_anim->curr_frame.height
    };
    bf->obj.is_active = false;
    float frac = 0.75;
    bf->obj.pos.y = GAME_GROUND_Y - bf->obj.size.y * frac;
    // purposely don't set xpos.
    // this has to be init'ed when spawned.
}

void bf_draw(bf_t *bf) {
    DrawTextureRec(
        bf->obj.curr_anim->asset->texture,
        bf->obj.curr_anim->curr_frame,
        bf->obj.pos,
        WHITE);
}