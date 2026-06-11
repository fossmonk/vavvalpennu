#include <raylib.h>
#include <anim_asset.h>

#ifndef _ANIM_H_
#define _ANIM_H_

typedef struct {
    anim_asset_t *asset;
    float timer;
    Rectangle curr_frame;
} anim_t;

#define flip_anim_left(obj) do {                  \
    if((obj)->curr_anim->curr_frame.width > 0) {  \
        (obj)->curr_anim->curr_frame.width *= -1; \
    }                                             \
} while(0)
#define flip_anim_right(obj) do {                  \
    if((obj)->curr_anim->curr_frame.width < 0) {  \
        (obj)->curr_anim->curr_frame.width *= -1; \
    }                                             \
} while(0)

void anim_advance(anim_t *anim, float dt);

#endif