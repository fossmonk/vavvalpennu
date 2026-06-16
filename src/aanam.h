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
} aanam_t;

void aanam_init(aanam_t *aana);
void aanam_draw(aanam_t *aana);

#endif