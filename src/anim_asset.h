#ifndef _ANIM_ASSET_H_
#define _ANIM_ASSET_H_

#include <raylib.h>

typedef struct {
    const char *filepath;
    const char *name;
    int framecount;
    float duration;
    bool repeat;
} anim_info_t;

typedef struct {
    Texture2D texture;
    const char *name;
    int framecount;
    float duration;
    bool repeat;
} anim_asset_t;

Vector2 anim_asset_get_frame_dim(anim_asset_t *asset);
void anim_asset_load_all(void);

#endif