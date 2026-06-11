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