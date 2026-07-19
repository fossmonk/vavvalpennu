#include <raylib.h>
#include <vpconfig.h>
#include <artifact.h>
#include <rand.h>
#include <anim.h>

#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))

static anim_asset_t dummy_anim_asset;
static anim_t dummy_anim;

static void artifact_get_texbbox_fname(char **texfname, char**bboxfname) {
    static char *art_tex_fnames[] = {
        ARTIF_TEXTURE0, ARTIF_TEXTURE1, ARTIF_TEXTURE2,
        ARTIF_TEXTURE3, ARTIF_TEXTURE4
    };
    static char *art_bbox_fnames[] = {
        ARTIF_BBOX0, ARTIF_BBOX1, ARTIF_BBOX2,
        ARTIF_BBOX3, ARTIF_BBOX4
    };

    int r_id = vp_rand_lim(0, ARRAY_SIZE(art_tex_fnames));

    *texfname = art_tex_fnames[r_id];
    *bboxfname = art_bbox_fnames[r_id];
}

void artifact_init(artifact_t *artif) {
    char *tex_fname, *bbox_fname;
    artifact_get_texbbox_fname(&tex_fname, &bbox_fname);
    artif->artifact_tex = LoadTexture(tex_fname);
    artif->terminal_y = GAME_GROUND_Y - (artif->artifact_tex.height);
    artif->gravity = true;
    artif->obj.cs = COORDS_WORLD;

    // load dummy animation for collision bbox
    dummy_anim_asset.texture = artif->artifact_tex;
    dummy_anim_asset.bbox = bbox_parse(bbox_fname);
    dummy_anim.asset = &dummy_anim_asset;
    artif->obj.curr_anim = &dummy_anim;
}

void artifact_activate_at(artifact_t *artif, Vector2 pos, float dt) {
    artif->obj.is_active = true;
    artif->obj.pos = pos;
    artif->obj.vel = (Vector2){0,0};
    if(pos.y != GAME_GROUND_Y && artif->gravity) {
        artif->obj.vel.y = ACCEL_G * dt;
    }
}

void artifact_update(artifact_t* artif, float dt) {
    if(artif->obj.is_active) {
        if(artif->gravity) {
            artif->obj.vel.y += (ACCEL_G * dt);
        }
        artif->obj.pos.y += (artif->obj.vel.y * dt);
        // clamp if reached ground
        if(artif->obj.pos.y >= artif->terminal_y) {
            artif->obj.vel.y = 0;
            artif->obj.pos.y = artif->terminal_y;
        }
    }
}

void artifact_draw(artifact_t *artif) {
    if(artif->obj.is_active) {
        DrawTexture(artif->artifact_tex, SPREAD_VEC(artif->obj.pos), WHITE);
    }
}
