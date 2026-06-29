#ifndef _ANIM_ASSET_H_
#define _ANIM_ASSET_H_

#include <raylib.h>
#include <bbox.h>

typedef struct {
    const char *anim_filepath;
    const char *bbox_filepath;
    const char *name;
    int framecount;
    float duration;
    bool repeat;
} anim_info_t;

typedef struct {
    Texture2D texture;
    bbox_t bbox;
    const char *name;
    int framecount;
    float duration;
    bool repeat;
} anim_asset_t;

Vector2 anim_asset_get_frame_dim(anim_asset_t *asset);
void anim_asset_load(anim_info_t info, anim_asset_t *asset);

#endif