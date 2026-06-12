#include <raylib.h>
#include <obj.h>

#ifndef _BONFIRE_H_
#define _BONFIRE_H_

typedef struct {
    obj_t obj;
    // animations
    anim_t anim_burn;
} bf_t;

void bf_init(bf_t *bf);

#endif