#include <obj.h>
#include <anim.h>

#ifndef _KARIKKU_H_
#define _KARIKKU_H_

typedef struct {
    obj_t obj;

    // animations
    anim_t anim_rotate;
} karikku_t;

void karikku_init_all(karikku_t *karikkus);
void karikku_update_all(karikku_t *karikkus, float dt);
void karikku_draw_all(karikku_t *karikkus);

#endif