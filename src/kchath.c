#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include <vpconfig.h>
#include <hbar.h>
#include <kchath.h>
#include <skball.h>
#include <rand.h>

#define KCH_INIT_POS              ((Vector2){G_W/2, G_H/2})
#define KCH_POS_CHANGE_DURATION   (1.5f)

anim_asset_t kchath_laugh;
anim_asset_t kchath_hurt;
anim_asset_t kchath_death;

#define LAUGH      0xB10
#define HURT       0xB11
#define DEATH      0xB12

const anim_id_t kch_laugh_id = { LAUGH };
const anim_id_t kch_hurt_id = { HURT };
const anim_id_t kch_death_id = { DEATH };

static bool g_anim_asset_loaded = false;
static float kch_update_timer = 0.0f;

void kchath_init(kchath_t* kch) {
    // ANIM
    if(!g_anim_asset_loaded) {
        anim_asset_load(ANIM_KCH_LAUGH, &kchath_laugh);
        anim_asset_load(ANIM_KCH_HURT, &kchath_hurt);
        anim_asset_load(ANIM_KCH_DEATH, &kchath_death);
    }
    Vector2 dim;
    // die
    dim = anim_asset_get_frame_dim(&kchath_death);
    kch->anim_death.asset = &kchath_death;
    kch->anim_death.timer = 0.0f;
    kch->anim_death.id    = kch_death_id;
    kch->anim_death.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // laugh
    dim = anim_asset_get_frame_dim(&kchath_laugh);
    kch->anim_laugh.asset = &kchath_laugh;
    kch->anim_laugh.timer = 0.0f;
    kch->anim_laugh.id    = kch_laugh_id;
    kch->anim_laugh.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // hurt
    dim = anim_asset_get_frame_dim(&kchath_hurt);
    kch->anim_hurt.asset = &kchath_hurt;
    kch->anim_hurt.timer = 0.0f;
    kch->anim_hurt.id    = kch_hurt_id;
    kch->anim_hurt.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // AUDIO
    // laugh
    kch->laugh = LoadMusicStream(AUD_KCH_LAUGH);
    // hurt
    kch->hurt = LoadSound(SOUND_KCH_HURT);

    // STATE
    kch->obj.curr_anim = &kch->anim_laugh;
    kch->obj.size = (Vector2){kch->obj.curr_anim->curr_frame.width, kch->obj.curr_anim->curr_frame.height};
    kch->obj.is_active = false;
    kch->obj.cs = COORDS_WORLD;
    kch->actionmask = 0;
    kch->max_health = 150;
    kch->health = kch->max_health;

    // init kch healthbar
    Vector2 hbar_pos = (Vector2){57, G_H - 60};
    hbar_init(&kch->hbar, hbar_pos, G_W - 120, 30, 2, 
        NULL, hbar_pos, RED, kch->max_health);

    // init skballs
    for(int i = 0; i < MAX_SKBALLS; ++i) {
        skball_init(&kch->skballs[i]);
    }
}

void kchath_activate(kchath_t *kch) {
    kch->obj.is_active = true;
    kch->obj.pos = obj_s2w_pos(KCH_INIT_POS);
    kch->obj.vel = (Vector2){0, 0};
    kch->health = kch->max_health;
    kch->actionmask = 0;
    PlayMusicStream(kch->laugh);
}

void kchath_update(kchath_t *kch, float dt) {
    if(!kch->obj.is_active) return;

    if(kch->health <= 0) {
        kch_set_die(kch);
    }

    if(kch_is_dying(kch)) {
        if(kch->obj.curr_anim->id.id == kch->anim_death.id.id) {
            if(anim_is_lastframe(kch->obj.curr_anim)) {
                kch->obj.is_active = false;
            }
        } else {
            kch->obj.curr_anim = &kch->anim_death;
            anim_reset(kch->obj.curr_anim);
        }

        for(int i = 0; i < MAX_SKBALLS; ++i) {
            kch->skballs[i].obj.is_active = false;
        }
    } else if(kch_is_hurting(kch)) {
        if(kch->obj.curr_anim->id.id == kch->anim_hurt.id.id) {
            if(anim_is_lastframe(kch->obj.curr_anim)) {
                kch_clr_hurt_shock(kch);
                kch->obj.curr_anim = &kch->anim_laugh;
            }
        } else {
            kch->obj.curr_anim = &kch->anim_hurt;
            anim_reset(kch->obj.curr_anim);
            PlaySound(kch->hurt);
        }
    } else {
        kch_update_timer += dt;
        UpdateMusicStream(kch->laugh);
        if(kch_update_timer >= KCH_POS_CHANGE_DURATION) {
            // get next random pos
            Vector2 randpos;
            randpos.x = GetRandomValue(300, G_W-300);
            randpos.y = GetRandomValue(50, GAME_GROUND_Y - kch->obj.size.y);
            kch->obj.pos = obj_s2w_pos(randpos);
            kch_update_timer -= KCH_POS_CHANGE_DURATION;
        }
        // activate skull balls
        for(int i = 0; i < MAX_SKBALLS; ++i) {
            skball_t *skball = &kch->skballs[i];
            if(!skball->obj.is_active) {
                skball_activate(skball);
            }
        }
        // update skull balls
        for(int i = 0; i < MAX_SKBALLS; ++i) {
            skball_t *skball = &kch->skballs[i];
            if(skball->obj.is_active) {
                skball_update(skball, dt);
            }
        }
    }

    // kch is active, so update anim
    anim_advance(kch->obj.curr_anim, dt);
}

void kchath_decr_health(kchath_t *kch, int amount) {
    kch->health -= amount;
    if(kch->health < 0) kch->health = 0;
}

void kchath_draw(kchath_t *kch) {
    DrawTextureRec(
        kch->obj.curr_anim->asset->texture, 
        kch->obj.curr_anim->curr_frame, kch->obj.pos, 
        WHITE
    );
    #ifdef DEBUG
    bbox_draw(kch->obj.curr_anim->asset->bbox, kch->obj.pos, RED);
    #endif
}

void kchath_handle_death(kchath_t *kch) {
    if(kch->health <= 0) {
        kch->health = 0;
        kch->obj.is_active = false;
        for(int i = 0; i < MAX_SKBALLS; ++i) {
            kch->skballs[i].obj.is_active = false;
        }
    }
}