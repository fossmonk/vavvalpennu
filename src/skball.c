#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <skball.h>

static anim_t dummy_anim;
static anim_asset_t dummy_anim_asset;

static int id = 1;
int get_id(void) {
    int k = id;
    id = 3 - id;
    return k;
}

void skball_init(skball_t *skball) {
    skball->skball_tex = LoadTexture(SKBALL_TEXTURE);
    skball->shader     = LoadShader(NULL, SKBALL_SHADER);
    skball->r = skball->skball_tex.width/2;
    skball->time_loc = GetShaderLocation(skball->shader, "u_time");
    skball->obj.is_active = false;
    skball->id = get_id();
    skball->obj.size = (Vector2){skball->skball_tex.width, skball->skball_tex.height};

    // setup a dummy animation for bbox
    dummy_anim_asset.texture = skball->skball_tex;
    dummy_anim_asset.bbox = bbox_parse(SKBALL_BBOX);
    dummy_anim.asset = &dummy_anim_asset;
    skball->obj.curr_anim = &dummy_anim;
}

void skball_draw(skball_t *skball) {
    float time = (float)GetTime();
    SetShaderValue(skball->shader, skball->time_loc, &time, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(skball->shader);
    Rectangle src = {0.0f, 0.0f, (float)skball->skball_tex.width, (float)skball->skball_tex.height};
    Rectangle dst = {skball->obj.pos.x, skball->obj.pos.y, (float)skball->skball_tex.width, (float)skball->skball_tex.height};
    DrawTexturePro(skball->skball_tex, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndShaderMode();
}

void skball_draw_all(skball_t *skballs) {
    for(int i = 0; i < MAX_SKBALLS; ++i) {
        if(skballs[i].obj.is_active) {
            skball_draw(&skballs[i]);
        }
    }
}

void skball_activate(skball_t *skball) {
    skball->obj.is_active = true;
    skball->obj.pos.y = GAME_GROUND_Y - skball->obj.size.y;
    skball->obj.vel.y = 0;

    // if skball id is even, go left to right. odd => right to left
    if(skball->id % 2 == 0) {
        skball->obj.pos.x = 0;
        skball->obj.vel.x = 2.5E2;
    } else {
        skball->obj.pos.x = G_W - skball->obj.size.x;
        skball->obj.vel.x = -2.5E2;
    }
}

void skball_update(skball_t *skball, float dt) {
    if(obj_is_oob(&skball->obj, COORDS_SCREEN)) {
        skball->obj.is_active = false;
    } else {
        skball->obj.pos.x += skball->obj.vel.x * dt;
    }
    // no animation, shader magic!
}
