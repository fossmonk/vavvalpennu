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
    Vector2 hbar_pos = (Vector2){60, G_H - 40};
    hbar_init(&vy->hbar, hbar_pos, G_W - 120, 30, 2);
    // load hbar icon texture
    vy->hbar_icon = LoadTexture(VY_HBAR_ICON);
    vy->hbar_iconpos = hbar_pos;
    vy->hbar_iconpos.x -= 3.0;
    vy->hbar_iconpos.y -= 20.0;
}