#include <raylib.h>
#include <vpconfig.h>
#include <hbar.h>
#include <vy.h>

// VADAYAKSHI ANIMATIONS
extern anim_asset_t vy_rise;
extern anim_asset_t vy_shock;

void vy_init(vy_t *vy) {
    // ANIM
    Vector2 dim;
    // idle
    // rise
    dim = anim_asset_get_frame_dim(&vy_rise);
    vy->anim_vy_rise.asset = &vy_rise;
    vy->anim_vy_rise.timer = 0.0f;
    vy->anim_vy_rise.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // shock
    dim = anim_asset_get_frame_dim(&vy_shock);
    vy->anim_vy_shock.asset = &vy_shock;
    vy->anim_vy_shock.timer = 0.0f;
    vy->anim_vy_shock.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // AUDIO
    // laugh
    vy->laugh = LoadMusicStream(AUD_VY_LAUGH);
    // hurt
    vy->hurt = LoadSound(SOUND_VY_HURT);

    // STATE
    vy->obj.curr_anim = &vy->anim_vy_rise;
    vy->obj.size = (Vector2){vy->obj.curr_anim->curr_frame.width, vy->obj.curr_anim->curr_frame.height};
    vy->obj.is_active = false;
    vy->actionmask = 0;
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
void vy_activate_hurting(vy_t *vy, float dt) {
    bool act = true;
    if(vy->actionmask & VY_IS_HURTING_S) {
        vy->obj.curr_anim = &vy->anim_vy_shock;
    } else {
        act = false;
    }
    if(act) {
        vy->obj.size = anim_get_framesize(vy->obj.curr_anim);
    }
    // reset current animation to start
    anim_reset(vy->obj.curr_anim);
}

void vy_draw(vy_t *vy) {
    DrawTextureRec(
        vy->obj.curr_anim->asset->texture, 
        vy->obj.curr_anim->curr_frame, vy->obj.pos, 
        WHITE
    );
}