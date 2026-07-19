#include <raylib.h>
#include <vpconfig.h>
#include <potion.h>

static anim_asset_t dummy_anim_asset;
static anim_t dummy_anim;

void potion_init(potion_t *potion) {
    potion->tex = LoadTexture(POTION_TEXTURE);
    potion->terminal_y = GAME_GROUND_Y - (potion->tex.height);
    potion->obj.cs = COORDS_WORLD;

    // load dummy animation for collision bbox
    dummy_anim_asset.texture = potion->tex;
    dummy_anim_asset.bbox = bbox_parse(POTION_BBOX);
    dummy_anim.asset = &dummy_anim_asset;
    potion->obj.curr_anim = &dummy_anim;
}

void potion_activate_at(potion_t *potion, Vector2 pos, float dt) {
    potion->obj.is_active = true;
    potion->obj.pos = pos;
    potion->obj.vel = (Vector2){0,0};
    if(pos.y != GAME_GROUND_Y) {
        potion->obj.vel.y = ACCEL_G * dt;
    }
}

void potion_update(potion_t *potion, float dt) {
    if(potion->obj.is_active) {
        potion->obj.vel.y += (ACCEL_G * dt);
        potion->obj.pos.y += (potion->obj.vel.y * dt);
        // clamp if reached ground
        if(potion->obj.pos.y >= potion->terminal_y) {
            potion->obj.vel.y = 0;
            potion->obj.pos.y = potion->terminal_y;
        }
    }
}

void potion_draw(potion_t *potion) {
    if(potion->obj.is_active) {
        DrawTexture(potion->tex, SPREAD_VEC(potion->obj.pos), WHITE);
    }
}
