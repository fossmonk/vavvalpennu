#include <raylib.h>
#include <vpconfig.h>
#include <epechi.h>

// EENAMPECHI ANIMATION EXTERNS
anim_asset_t epechi_roll;

static bool g_anim_asset_loaded = false;

void epechi_init(epechi_t *ep) {
    // ANIM
    if(!g_anim_asset_loaded) {
        anim_asset_load(ANIM_EPECHI_ROLL, &epechi_roll);
    }
    Vector2 dim;
    // idle
    // roll
    dim = anim_asset_get_frame_dim(&epechi_roll);
    ep->anim_roll.asset = &epechi_roll;
    ep->anim_roll.timer = 0.0f;
    ep->anim_roll.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    ep->obj.curr_anim = &ep->anim_roll;
    ep->obj.size = (Vector2){
        ep->obj.curr_anim->curr_frame.width,
        ep->obj.curr_anim->curr_frame.height
    };
    ep->obj.is_active = false;
    ep->is_dying = false;
    ep->is_rolling = false;
    ep->max_health = 300;
    ep->health = ep->max_health;

    // init vy healthbar
    // Vector2 hbar_pos = (Vector2){60, G_H - 40};
    // hbar_init(&ep->hbar, hbar_pos, G_W - 120, 30, 2);
    // // load hbar icon texture
    // ep->hbar_icon = LoadTexture(VY_HBAR_ICON);
    // ep->hbar_iconpos = hbar_pos;
    // ep->hbar_iconpos.x -= 3.0;
    // ep->hbar_iconpos.y -= 20.0;
}

void epechi_draw(epechi_t *ep) {
    DrawTextureRec(
        ep->obj.curr_anim->asset->texture,
        ep->obj.curr_anim->curr_frame, ep->obj.pos,
        WHITE
    );
}