#include <raylib.h>
#include <anim_asset.h>
#include <math.h>

#ifndef _ANIM_H_
#define _ANIM_H_

typedef struct {
    anim_asset_t *asset;
    float timer;
    Rectangle curr_frame;
} anim_t;

#define anim_hflipl(obj) do {                  \
    if((obj)->curr_anim->curr_frame.width > 0) {  \
        (obj)->curr_anim->curr_frame.width *= -1; \
    }                                             \
} while(0)
#define anim_hflipr(obj) do {                  \
    if((obj)->curr_anim->curr_frame.width < 0) {  \
        (obj)->curr_anim->curr_frame.width *= -1; \
    }                                             \
} while(0)

void anim_advance(anim_t *anim, float dt);

static inline Vector2 anim_get_framesize(anim_t *anim) {
    return (Vector2){fabsf(anim->curr_frame.width), anim->curr_frame.height};
}

#endif