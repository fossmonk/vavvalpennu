#include <raylib.h>
#include <vpconfig.h>
#include <toddy.h>

static anim_asset_t dummy_anim_asset;
static anim_t dummy_anim;

void toddy_init(toddy_t *toddy) {
    toddy->toddy_tex = LoadTexture(TODDY_TEXTURE);
    toddy->terminal_y = GAME_GROUND_Y - (toddy->toddy_tex.height);

    // load dummy animation for collision bbox
    dummy_anim_asset.texture = toddy->toddy_tex;
    dummy_anim_asset.bbox = bbox_parse(TODDY_BBOX);
    dummy_anim.asset = &dummy_anim_asset;
    toddy->obj.curr_anim = &dummy_anim;
}

void toddy_activate_at(toddy_t *toddy, Vector2 pos, float dt) {
    toddy->obj.is_active = true;
    toddy->obj.pos = pos;
    toddy->obj.vel = (Vector2){0,0};
    if(pos.y != GAME_GROUND_Y) {
        toddy->obj.vel.y = ACCEL_G * dt;
    }
}

void toddy_update(toddy_t *toddy, float dt) {
    if(toddy->obj.is_active) {
        toddy->obj.vel.y += (ACCEL_G * dt);
        toddy->obj.pos.y += (toddy->obj.vel.y * dt);
        // clamp if reached ground
        if(toddy->obj.pos.y >= toddy->terminal_y) {
            toddy->obj.vel.y = 0;
            toddy->obj.pos.y = toddy->terminal_y;
        }
    }
}

void toddy_draw(toddy_t *toddy) {
    if(toddy->obj.is_active) {
        DrawTexture(toddy->toddy_tex, SPREAD_VEC(toddy->obj.pos), WHITE);
    }
}
