#ifndef _FFLY_H_
#define _FFLY_H_

#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <anim.h>

typedef struct {
    obj_t obj;
    Vector2 prev;
    Vector2 next;
    bool in_flight;
    bool reached_target;

    // anim
    anim_t anim_fly;
} ffly_t;

void ffly_init_all(ffly_t *fflys);
void ffly_activate_all(ffly_t *ffs);
void ffly_update_all(ffly_t *ffs, float dt);
void ffly_draw_all(ffly_t *fflys);

#endif 