#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <karikku.h>

extern anim_asset_t karikku_rotate;

Vector2 kpos[TOTAL_KARIKKU];
static bool kposinit = false;
static int kcount = 0;

void karikku_randomize_pos(void) {
    // keep two different heights.
    // choose random n < MAX_KARIKKU
    // first n at first height, rest at second height
    // for xpos, instead of purely random,
    // choose so that they don't collide
    Vector2 dim = anim_asset_get_frame_dim(&karikku_rotate);
    
    for(int offset = 0, xl = 0; offset < TOTAL_KARIKKU; offset += MAX_KARIKKU, xl += G_W) {

        float h1 = GAME_GROUND_Y - dim.y - 10.0f;
        float h2 = h1 - 180.0f;
        int first_n = 1 + rand() % (MAX_KARIKKU - 1);
    
        // Set ypos
        for(int i = 0; i < first_n; ++i) {
            int idx = offset + i;
            if(idx >= TOTAL_KARIKKU) break;
            kpos[idx].y = h1;
        }
        for(int i = first_n; i < MAX_KARIKKU; ++i) {
            int idx = offset + i;
            if(idx >= TOTAL_KARIKKU) break;
            kpos[idx].y = h2;
        }
        // equal spacing
        float eq_w = G_W/dim.x;
    
        // choose first one as ∈ [eq_w, 2*eq_w]
        float x0 = (float)xl + eq_w + (float)(rand() % ((int)eq_w));
        kpos[offset].x = x0;
    
        for(int i = 1; i < MAX_KARIKKU; i++) {
            int idx = offset + i;
            if(idx >= TOTAL_KARIKKU) break;
            kpos[idx].x = kpos[idx - 1].x + dim.x + (float)(rand() % ((int)dim.x));
        }
    }
}

void karikku_init(karikku_t *k) {
    if(!kposinit) {
        karikku_randomize_pos();
        kposinit = true;
    }
    // ANIM
    Vector2 dim;
    dim = anim_asset_get_frame_dim(&karikku_rotate);
    k->anim_rotate.asset = &karikku_rotate;
    k->anim_rotate.timer = 0.0f;
    k->anim_rotate.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    // STATE
    k->obj.curr_anim = &k->anim_rotate;
    k->obj.size = (Vector2){
        k->obj.curr_anim->curr_frame.width,
        k->obj.curr_anim->curr_frame.height
    };
    // contrary to others, karikkus are active always
    // until picked up
    k->obj.is_active = true;
    k->obj.pos = kpos[kcount++];
}

void karikku_draw(karikku_t *k) {
    DrawTextureRec(
        k->obj.curr_anim->asset->texture,
        k->obj.curr_anim->curr_frame,
        k->obj.pos,
        LIGHTGRAY);
}



