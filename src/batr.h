#include <obj.h>
#include <anim.h>

#ifndef _BATR_H_
#define _BATR_H_

typedef struct {
    obj_t obj;

    // animations
    anim_t anim_rotate;
} batr_t;

void batr_init(batr_t *b);
void batr_draw(batr_t *b);

#endif