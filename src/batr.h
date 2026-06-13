#include <obj.h>
#include <anim.h>

#ifndef _BATR_H_
#define _BATR_H_

#define BATARANG_VEL_R         (800.0f)

typedef struct {
    obj_t obj;

    // animations
    anim_t anim_rotate;
} batr_t;

void batr_init(batr_t *b);
void batr_update(batr_t *b, float dt);
void batr_draw(batr_t *b);

#endif