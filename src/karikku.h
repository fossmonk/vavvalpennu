#include <obj.h>
#include <anim.h>

#ifndef _KARIKKU_H_
#define _KARIKKU_H_

typedef struct {
    obj_t obj;

    // animations
    anim_t anim_rotate;
} karikku_t;

void karikku_init(karikku_t *k);
void karikku_draw(karikku_t *k);

#endif