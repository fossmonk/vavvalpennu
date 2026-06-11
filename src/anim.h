#include <raylib.h>
#include <anim_asset.h>

#ifndef _ANIM_H_
#define _ANIM_H_

typedef struct {
    anim_asset_t *asset;
    float timer;
    Rectangle curr_frame;
} anim_t;

#endif