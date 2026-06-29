#include <obj.h>
#include <anim.h>

#ifndef _AANAM_H_
#define _AANAM_H_

typedef struct {
    obj_t obj;
    bool is_dying;
    bool hit_player;

    // animations
    anim_t anim_run;
    anim_t anim_death;

    // sounds
    Sound growl;
} aanam_t;

void aanam_init_all(aanam_t *aanams);
void aanam_activate_all(aanam_t *aanas, bool boss_active, float dt);
void aanam_update_all(aanam_t *aanas, float dt);
void aanam_draw_all(aanam_t *aanas);

#endif