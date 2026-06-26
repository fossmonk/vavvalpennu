#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include <vpconfig.h>
#include <hbar.h>
#include <kchath.h>
#include <skball.h>

#define KCH_INIT_POS              ((Vector2){G_W/2, G_H/2})
#define KCH_POS_CHANGE_CHANCE     (rand() % 423 == 0)

extern anim_asset_t kchath_laugh;

void kchath_init(kchath_t* kch) {
    // ANIM
    Vector2 dim;
    // die
    // laugh
    dim = anim_asset_get_frame_dim(&kchath_laugh);
    kch->anim_laugh.asset = &kchath_laugh;
    kch->anim_laugh.timer = 0.0f;
    kch->anim_laugh.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // AUDIO
    // laugh
    kch->laugh = LoadMusicStream(AUD_KCH_LAUGH);
    // hurt
    kch->hurt = LoadSound(SOUND_KCH_HURT);

    // STATE
    kch->obj.curr_anim = &kch->anim_laugh;
    kch->obj.size = (Vector2){kch->obj.curr_anim->curr_frame.width, kch->obj.curr_anim->curr_frame.height};
    kch->obj.is_active = false;
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
    PlayMusicStream(kch->laugh);
}

void kchath_update(kchath_t *kch, float dt) {
    if(kch->obj.is_active && !kch_is_dying(kch)) {
        UpdateMusicStream(kch->laugh);
        if(KCH_POS_CHANGE_CHANCE && anim_is_lastframe(kch->obj.curr_anim)) {
            // get next random pos
            Vector2 randpos;
            randpos.x = GetRandomValue(300, G_W-300);
            randpos.y = GetRandomValue(50, GAME_GROUND_Y - kch->obj.size.y);
            kch->obj.pos = obj_s2w_pos(randpos);
        }
        // activate skull balls
        for(int i = 0; i < MAX_SKBALLS; ++i) {
            skball_t *skball = &kch->skballs[i];
            if(!skball->obj.is_active) {
                skball_activate(skball);
            }
        }
        
        anim_advance(kch->obj.curr_anim, dt);
    }


    // update skull balls
    for(int i = 0; i < MAX_SKBALLS; ++i) {
        skball_t *skball = &kch->skballs[i];
        if(skball->obj.is_active) {
            skball_update(skball, dt);
        }
    }
}

void kchath_draw(kchath_t *kch) {
    DrawTextureRec(
        kch->obj.curr_anim->asset->texture, 
        kch->obj.curr_anim->curr_frame, kch->obj.pos, 
        WHITE
    );
}