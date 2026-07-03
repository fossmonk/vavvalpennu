#include <raylib.h>
#include <anim_asset.h>
#include <vpconfig.h>

void anim_asset_load(anim_info_t info, anim_asset_t *asset) {
    asset->texture = LoadTexture(info.anim_filepath);
    if(info.bbox_filepath) {
        asset->bbox = bbox_parse(info.bbox_filepath);
    } else {
        asset->bbox.type = INVALID_BBOX_TYPE;
    }
    asset->name = info.name;
    asset->framecount = info.framecount;
    asset->duration = info.duration;
    asset->repeat = info.repeat;
}

Vector2 anim_asset_get_frame_dim(anim_asset_t *asset) {
    int w = asset->texture.width/asset->framecount;
    int h = asset->texture.height;
    return (Vector2){w, h};
}
