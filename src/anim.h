#include <raylib.h>
#include <anim_asset.h>
#include <math.h>
#include <bbox.h>

#ifndef _ANIM_H_
#define _ANIM_H_

typedef struct {
    int id;
} anim_id_t;

typedef struct {
    anim_id_t id;
    anim_asset_t *asset;
    float timer;
    Rectangle curr_frame;
} anim_t;

void anim_advance(anim_t *anim, float dt);
int anim_get_curr_frame_idx(anim_t *anim);
bool anim_is_lastframe(anim_t *anim);
void anim_reset(anim_t *anim);

static inline Vector2 anim_get_framesize(anim_t *anim) {
    return (Vector2){fabsf(anim->curr_frame.width), anim->curr_frame.height};
}

static inline void anim_hflipr(anim_t *anim) {
    if(anim->curr_frame.width < 0) {
        anim->curr_frame.width *= -1;
    }
}

static inline void anim_hflipl(anim_t *anim) {
    if(anim->curr_frame.width > 0) {
        anim->curr_frame.width *= -1;
    }
}

#endif