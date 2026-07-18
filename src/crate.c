#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <crate.h>
#include <obj.h>
#include <rand.h>
#include <shader.h>

// we spawn crates at certain positions
// we have to make sure they're at least G_W*2 apart.

#define MAX_POTION_CRATES (15)
#define MAX_ARTIF_CRATES (10)
#define MAX_CRATES (MAX_POTION_CRATES + MAX_ARTIF_CRATES)
#define MAX_CRATE_POS_Y (3)

// randomly fill this array
static float crate_spos_x[MAX_CRATES];
// keep known values here
static float crate_spos_y[MAX_CRATE_POS_Y] =  {
    GAME_GROUND_Y, GAME_GROUND_Y - 500, GAME_GROUND_Y - 700
};

// fill this array in order and shuffle it
static crate_content_type crate_c[MAX_CRATES] = {
    POTION, POTION, POTION, POTION, POTION, POTION, POTION, POTION, 
    POTION, POTION, POTION, POTION, POTION, POTION, POTION, ARTIFACT, 
    ARTIFACT, ARTIFACT, ARTIFACT, ARTIFACT, ARTIFACT, ARTIFACT,
    ARTIFACT, ARTIFACT, ARTIFACT 
};

static anim_asset_t crate_burst;
static anim_asset_t dummy_anim_asset;
static anim_t dummy_anim;
const float spawn_a = 300;
static int spawn_n = 2;
const float spawn_d = G_W*1.5;
static int crate_index = 0;
static bool crate_once_init_done = false;

Sound s_crate_appear;
Sound s_crate_break;

#define cct crate_content_type
static void _swap(cct *a, cct *b) {
    cct temp = *a; *a = *b; *b = temp;
}

static void _arr_shuffle(cct *arr, int sz) {
    for(int i = sz-2; i > 0 ; --i) {
        int ridx = vp_rand_lim(0, i);
        _swap(&arr[ridx], &arr[i+1]);
    }
}
#undef cct

static bool crate_can_spawn(void) {
    bool ret = false;
    float current_check_x = spawn_a + spawn_n*spawn_d;
    if(obj_s2w_pos((Vector2){0,0}).x > current_check_x) {
        ret = true;
        spawn_n++;
    }
    return ret;
}

static void crate_init_once(void) {
    // fill crate_pos_x with random x values spread through a portion of SCREENWIDTH
    for(int i = 0; i < MAX_CRATES; ++i) {
        crate_spos_x[i] = vp_rand_lim(G_W/3, G_W - 100);
    }
    // shuffle crate_c
    _arr_shuffle(crate_c, MAX_CRATES);
    // Load sounds
    s_crate_appear = LoadSound(SOUND_C_APPEAR);
    s_crate_break = LoadSound(SOUND_C_BREAK);

    crate_once_init_done = true;
}

void crate_init(crate_t* cr) {
    if(!crate_once_init_done) crate_init_once();

    // ANIM
    Vector2 dim;
    anim_asset_load(ANIM_CRATE_BURST, &crate_burst);
    dim = anim_asset_get_frame_dim(&crate_burst);
    cr->anim_burst.asset = &crate_burst;
    cr->anim_burst.timer = 0.0f;
    cr->anim_burst.curr_frame = (Rectangle){0, 0, dim.x, dim.y};

    cr->crate_tex = LoadTexture(CRATE_TEXTURE);
    dummy_anim_asset.texture = cr->crate_tex;
    dummy_anim_asset.bbox = bbox_parse(CRATE_BBOX);
    dummy_anim.asset = &dummy_anim_asset;
    cr->obj.curr_anim = &dummy_anim;
    cr->obj.is_active = false;
}

void crate_activate(crate_t *cr) {
    if(crate_can_spawn() && !cr->obj.is_active) {
        // activate the crate
        int crate_idx_running = (crate_index++) % MAX_CRATES;
        cr->content.type = crate_c[crate_idx_running];
        if(cr->content.type == ARTIFACT) {
            // setup artifact details
            artifact_init(&cr->content.content.artifact);
        } else {
            // setup potion details
            potion_init(&cr->content.content.potion);
        }
        // set position
        cr->obj.pos.x = crate_spos_x[crate_idx_running];
        cr->obj.pos.y = crate_spos_y[vp_rand_lim(0, MAX_CRATE_POS_Y)] - cr->crate_tex.height;
        cr->obj.pos = obj_s2w_pos(cr->obj.pos);
        cr->is_broken = false;
        cr->is_open = false;
        cr->obj.curr_anim = &dummy_anim;
        cr->obj.is_active = true;
        PlaySound(s_crate_appear);
    }
}

void crate_update(crate_t* cr, float dt) {
    if(cr->is_broken && cr->obj.is_active) {
        if(cr->obj.curr_anim == &cr->anim_burst) {
            if(anim_is_lastframe(cr->obj.curr_anim)) {
                // this is set to false in game code later when user picks up content
                cr->is_open = true;
                // time to spawn whatever is inside
                if(cr->content.type == ARTIFACT) {
                    artifact_activate_at(&cr->content.content.artifact, cr->obj.pos, dt);
                } else {
                    potion_activate_at(&cr->content.content.potion, cr->obj.pos, dt);
                }
                // now make it inactive
                cr->is_broken = false;
                cr->obj.is_active = false;
            } else {
                // just advance burst animation
                anim_advance(cr->obj.curr_anim, dt);
            }
        } else {
            // setup burst animation
            cr->obj.curr_anim = &cr->anim_burst;
            cr->obj.curr_anim->curr_frame.x = 0;
            cr->obj.curr_anim->timer = 0.0f;
            PlaySound(s_crate_break);
        }
    } else {
        if(obj_is_oob(&cr->obj, COORDS_WORLD)) {
            cr->obj.is_active = false;
        }
    }
}

void crate_content_update(crate_t *cr, float dt) {
    if(cr->content.type == ARTIFACT) {
        artifact_update(&cr->content.content.artifact, dt);
    } else {
        potion_update(&cr->content.content.potion, dt);
    }
}

void crate_draw(crate_t* cr) {
    if(cr->obj.is_active) {
        if(cr->is_broken) {
            DrawTextureRec(
                cr->obj.curr_anim->asset->texture,
                cr->obj.curr_anim->curr_frame,
                cr->obj.pos,
                WHITE
            );
        } else {
            DrawTexture(cr->crate_tex, cr->obj.pos.x, cr->obj.pos.y, WHITE);
        }
        #ifdef DEBUG
        bbox_draw(cr->obj.curr_anim->asset->bbox, cr->obj.pos, MAGENTA);
        #endif
    }
}

void crate_content_draw(crate_t *cr) {
    if(cr->content.type == ARTIFACT) {
        artifact_draw(&cr->content.content.artifact);
    } else {
        potion_draw(&cr->content.content.potion);
    }
}
