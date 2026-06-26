#include <raylib.h>
#include <vpconfig.h>
#include <aanam.h>
#include <rand.h>

#define AANA_RAND_CHANCE       ((vp_rand() % 12283 == 0))
#define AANA_VEL_X             (-400.0f)

// AANAMARUTHA ANIMATIONS
extern anim_asset_t aanam_run;
extern anim_asset_t aanam_death;

void aanam_init(aanam_t *aana) {
    // ANIM
    Vector2 dim;
    // run
    dim = anim_asset_get_frame_dim(&aanam_run);
    aana->anim_run.asset = &aanam_run;
    aana->anim_run.timer = 0.0f;
    aana->anim_run.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // die
    dim = anim_asset_get_frame_dim(&aanam_death);
    aana->anim_death.asset = &aanam_death;
    aana->anim_death.timer = 0.0f;
    aana->anim_death.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    
    // STATE
    aana->obj.curr_anim = &aana->anim_run;
    aana->obj.size = (Vector2){ 
        aana->anim_run.curr_frame.width, 
        aana->anim_run.curr_frame.height
    };
    aana->obj.is_active = false;
    aana->is_dying = false;
    aana->hit_player = false;

    // SOUND
    aana->growl = LoadSound(SOUND_AGROWL);
}

void aanam_activate(aanam_t *aana) {
    aana->is_dying = false;
    aana->obj.is_active = true;
    aana->hit_player = false;
    aana->obj.curr_anim = &aana->anim_run;
    Vector2 pos, vel;
    pos.y = GAME_GROUND_Y - aana->obj.size.y;
    pos.x = G_W - aana->obj.size.x;
    pos = obj_s2w_pos(pos);
    vel.y = 0;
    vel.x = AANA_VEL_X;
    aana->obj.pos = pos;
    aana->obj.vel = vel;
    PlaySound(aana->growl);
}

void aanam_activate_all(aanam_t *aanas, bool boss_active) {
    for(int i = 0; i < MAX_AANAS; ++i) {
        if(!aanas[i].obj.is_active && AANA_RAND_CHANCE && !boss_active) {
            aanam_activate(&aanas[i]);
        }
    }
}

void aanam_update(aanam_t *aana, float dt) {
    if(aana->obj.is_active && !aana->is_dying) {
        // check for bounds
        if(obj_is_oob(&aana->obj, COORDS_WORLD)) {
            aana->obj.is_active = false;
        } else {
            // update x pos
            aana->obj.pos.x += aana->obj.vel.x * dt;
            anim_advance(aana->obj.curr_anim, dt);
        }
    }
}

void aanam_update_all(aanam_t *aanas, float dt) {
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_update(&aanas[i], dt);
    }
}

void aanam_draw(aanam_t *aana) {
    DrawTextureRec(
        aana->obj.curr_anim->asset->texture,
        aana->obj.curr_anim->curr_frame,
        aana->obj.pos,
        WHITE
    );
}

void aanam_draw_all(aanam_t *aanas) {
    for(int i = 0; i < MAX_AANAS; ++i) {
        if(aanas[i].obj.is_active) {
            aanam_draw(&aanas[i]);
        }
    }
}