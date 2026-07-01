#include <anim.h>
#include <math.h>

void anim_advance(anim_t *anim, float dt) {
    anim->timer += dt;
    if(anim->timer >= anim->asset->duration) {
        // advance frame
        int fw = fabsf(anim->curr_frame.width);
        int tw = anim->asset->texture.width;
        int curr_x = anim->curr_frame.x;
        if(!anim->asset->repeat) {
            curr_x = (int)fmin(curr_x + fw, tw-fw);
        } else {
            curr_x = (curr_x + fw) % tw;
        }
        anim->curr_frame.x = curr_x;
        anim->timer -= anim->asset->duration;
    }
}

bool anim_is_lastframe(anim_t *anim) {
    int fc = anim->asset->framecount;
    int frame_w = anim->asset->texture.width/fc;
    int curr_frame_idx = anim->curr_frame.x / frame_w;
    return (curr_frame_idx == (fc - 1));
}

int anim_get_curr_frame_idx(anim_t *anim) {
    int fc = anim->asset->framecount;
    int frame_w = anim->asset->texture.width/fc;
    int curr_frame_idx = anim->curr_frame.x / frame_w;
    return curr_frame_idx;
}

void anim_reset(anim_t *anim) {
    anim->curr_frame.x = 0;
    anim->timer = 0.0f;
}