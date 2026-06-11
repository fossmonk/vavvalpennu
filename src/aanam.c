#include <raylib.h>
#include <vpconfig.h>
#include <aanam.h>

// AANAMARUTHA ANIMATIONS
extern anim_asset_t aanam_run;

void aanam_init(aanam_t *aana) {
    // ANIM
    Vector2 dim;
    // run
    dim = anim_asset_get_frame_dim(&aanam_run);
    aana->anim_run.asset = &aanam_run;
    aana->anim_run.timer = 0.0f;
    aana->anim_run.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    aana->obj.curr_anim = &aana->anim_run;
    aana->obj.size = (Vector2){ 
        aana->anim_run.curr_frame.width, 
        aana->anim_run.curr_frame.height
    };
    aana->obj.is_active = false;
    aana->is_dying = false;
}