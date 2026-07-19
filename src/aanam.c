#include <raylib.h>
#include <vpconfig.h>
#include <aanam.h>
#include <rand.h>

#define AANA_SPAWN_COOLDOWN_TIME (5.0f)
#define AANA_VEL_X (-400.0f)

static float g_aanam_spawn_timer = 0.0f;

// AANAMARUTHA ANIMATIONS
anim_asset_t aanam_run;
anim_asset_t aanam_death;

static bool g_anim_asset_loaded = false;

void aanam_init(aanam_t *aana) {
    // ANIM
    if(!g_anim_asset_loaded) {
        anim_asset_load(ANIM_AANAM_RUN, &aanam_run);
        anim_asset_load(ANIM_AANAM_DEATH, &aanam_death);
    }
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
    aana->obj.cs = COORDS_WORLD;
    aana->is_dying = false;
    aana->hit_player = false;

    // SOUND
    aana->growl = LoadSound(SOUND_AGROWL);
}

void aanam_init_all(aanam_t *aanams) {
    for(int i = 0; i < MAX_AANAS; ++i) {
        aanam_init(&aanams[i]);
    }
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

void aanam_activate_all(aanam_t *aanas, bool boss_active, float dt) {
    if(boss_active) return;

    g_aanam_spawn_timer += dt;

    if (g_aanam_spawn_timer >= AANA_SPAWN_COOLDOWN_TIME) {
        // Reset timer
        g_aanam_spawn_timer -= AANA_SPAWN_COOLDOWN_TIME;

        // Find the inactive aanam
        for(int i = 0; i < MAX_AANAS; ++i) {
            if(!aanas[i].obj.is_active) {
                aanam_activate(&aanas[i]);
                break;
            }
        }
    }
}

void aanam_update(aanam_t *aana, float dt) {
    if(aana->obj.is_active && !aana->is_dying) {
        // check for bounds
        if(obj_is_oob(&aana->obj)) {
            aana->obj.is_active = false;
        } else {
            // update x pos
            aana->obj.pos.x += aana->obj.vel.x * dt;
            anim_advance(aana->obj.curr_anim, dt);
        }
    }

    if(aana->is_dying) {
        if(aana->obj.curr_anim == &aana->anim_death) {
            if(anim_is_lastframe(aana->obj.curr_anim)) {
                aana->obj.is_active = false;
                anim_reset(aana->obj.curr_anim);
            } else {
                anim_advance(aana->obj.curr_anim, dt);
            }
        } else {
            aana->obj.curr_anim = &aana->anim_death;
            aana->obj.curr_anim->curr_frame.x = 0;
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

    #ifdef DEBUG
    bbox_draw(aana->obj.curr_anim->asset->bbox, aana->obj.pos, RED);
    #endif
}

void aanam_draw_all(aanam_t *aanas) {
    for(int i = 0; i < MAX_AANAS; ++i) {
        if(aanas[i].obj.is_active) {
            aanam_draw(&aanas[i]);
        }
    }
}