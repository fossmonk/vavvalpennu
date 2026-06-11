#include <raylib.h>
#include <vpconfig.h>
#include <hbar.h>
#include <vy.h>

// VADAYAKSHI ANIMATIONS
extern anim_asset_t vy_rise;

void vy_init(vy_t *vy) {
    // ANIM
    Vector2 dim;
    // idle
    // rise
    dim = anim_asset_get_frame_dim(&vy_rise);
    vy->anim_vy_rise.asset = &vy_rise;
    vy->anim_vy_rise.timer = 0.0f;
    vy->anim_vy_rise.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    vy->obj.curr_anim = &vy->anim_vy_rise;
    vy->obj.size = (Vector2){vy->obj.curr_anim->curr_frame.width, vy->obj.curr_anim->curr_frame.height};
    vy->obj.is_active = false;
    vy->is_dying = false;
    vy->is_orbpos = false;
    vy->max_health = 500;
    vy->health = vy->max_health;

    // init vy healthbar
    hbar_init(&vy->hbar, (Vector2){60, G_H - 40}, G_W - 120, 30, 2);
}