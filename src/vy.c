#include <raylib.h>
#include <raymath.h>
#include <vpconfig.h>
#include <hbar.h>
#include <vy.h>
#include <orb.h>
#include <rand.h>

#define VY_INIT_POS ((Vector2){G_W-300, GAME_GROUND_Y-250})
#define VY_INIT_VEL ((Vector2){-3E2, -2.5E2})

#define VY_FINAL_POS ((Vector2){G_W/2 - 150, 40})

#define VY_RISE_VEL_X_1        (2.5E2)

#define ORB_RAND_CHANCE        ((vp_rand() % 5557 == 0))

anim_asset_t vy_rise;
anim_asset_t vy_shock;

static bool g_anim_asset_loaded = false;

void vy_init(vy_t *vy) {
    // ANIM
    if(!g_anim_asset_loaded) {
        // load animation assets
        anim_asset_load(ANIM_VY_RISE, &vy_rise);
        anim_asset_load(ANIM_VY_SHOCK, &vy_shock);
    }
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
    vy->max_health = VY_MAX_HEALTH;
    vy->health = vy->max_health;

    // init vy healthbar
    Vector2 hbar_pos = (Vector2){57, G_H - 60};
    hbar_init(&vy->hbar, hbar_pos, G_W - 120, 30, 2, 
        VY_HBAR_ICON, hbar_pos, RED, vy->max_health);

    // init orbs
    for(int i = 0; i < MAX_ORBS; ++i) {
        orb_init(&vy->orbs[i]);
    }
}

void vy_activate(vy_t *vy) {
    vy->obj.is_active = true;
    vy->is_orbpos = false;
    vy->obj.pos = obj_s2w_pos(VY_INIT_POS);
    vy->obj.vel = VY_INIT_VEL;
    PlayMusicStream(vy->laugh);
}

void vy_update(vy_t *vy, float dt) {
    if(vy->obj.is_active && !vy_is_dying(vy)) {
        UpdateMusicStream(vy->laugh);
        Vector2 vy_sfinalpos = obj_s2w_pos(VY_FINAL_POS);
        if(!vy->is_orbpos) {
            vy->obj.pos.x += vy->obj.vel.x * dt;
            vy->obj.pos.y += vy->obj.vel.y * dt;
            vy->obj.pos = Vector2Max(vy->obj.pos, vy_sfinalpos);
            vy->is_orbpos = Vector2Equals(vy->obj.pos, vy_sfinalpos);
        } else {
            if(obj_w2s_pos(vy->obj.pos).x < 300) {
                vy->obj.vel.x = VY_RISE_VEL_X_1;
            }
            if(obj_w2s_pos(vy->obj.pos).x > (G_W - 300)) {
                vy->obj.vel.x = -VY_RISE_VEL_X_1;
            }
            vy->obj.pos.x += vy->obj.vel.x * dt;
        }
        anim_advance(vy->obj.curr_anim, dt);
    }

    if(vy->is_orbpos) {
        for(int i = 0; i < MAX_ORBS; i++) {
            orb_t *orb = &vy->orbs[i];
            if(!orb->obj.is_active && ORB_RAND_CHANCE) {
                orb_activate(orb, dt);
            }
        }
    }

    if(vy_is_hurting(vy)) {
        if(!vy->in_hurt_anim) {
            // switch to hurt animation
            vy->in_hurt_anim = true;
            vy_activate_hurting(vy, dt);
        } else {
            // if hurt animation is over, switch to idle
            if(anim_is_lastframe(vy->obj.curr_anim)) {
                vy->in_hurt_anim = false;
                vy_clr_hurt_shock(vy);
                vy->obj.curr_anim = &vy->anim_vy_rise;
            }
        }
        anim_advance(vy->obj.curr_anim, dt);
    }

    for(int i = 0; i < MAX_ORBS; i++) {
        orb_t *orb = &vy->orbs[i];
        if(orb->obj.is_active) {
            orb_update(orb, dt);
        }
    }
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
    anim_reset(vy->obj.curr_anim);
}

void vy_draw(vy_t *vy) {
    DrawTextureRec(
        vy->obj.curr_anim->asset->texture, 
        vy->obj.curr_anim->curr_frame, vy->obj.pos, 
        WHITE
    );
}