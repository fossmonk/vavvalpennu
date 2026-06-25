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
void batr_update_all(batr_t *batrs, float dt);
batr_t *batr_get_empty_slot(batr_t *batrs);
void batr_draw_all(batr_t *batrs);

#endif