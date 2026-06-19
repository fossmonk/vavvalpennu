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

void ffly_init(ffly_t *ff);
void ffly_activate(ffly_t *ff);
void ffly_update(ffly_t *ff, float dt);
void ffly_draw(ffly_t *ff);

#endif 