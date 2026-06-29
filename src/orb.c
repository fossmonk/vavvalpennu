#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <orb.h>
#include <vy.h>
#include <rand.h>
#include <shader.h>
#include <bbox.h>

static float g_orb_xpos[MAX_ORBS];
bool xpos_init_done = false;
int g_orbcount = 0;

anim_t dummy_anim;
anim_asset_t dummy_anim_asset;

static void swap(float *a, float *b) {
    float temp = *a; *a = *b; *b = temp;
}

static void _arr_shuffle(float *arr, int sz) {
    for(int i = sz-2; i > 0 ; --i) {
        int ridx = vp_rand_lim(0, i);
        swap(&arr[ridx], &arr[i+1]);
    }
}

static void _orb_xpos_init(void) {
    float margin = 0.1 * G_W;
    float eff_w = G_W - margin*2;
    float d = eff_w/MAX_ORBS;
    g_orb_xpos[0] = margin;
    for(int i = 1; i < MAX_ORBS; ++i) {
        g_orb_xpos[i] = g_orb_xpos[i-1] + d;
    }
    _arr_shuffle(g_orb_xpos, MAX_ORBS);
}

void orb_init(orb_t *orb) {
    if(!xpos_init_done) {
        _orb_xpos_init();
        xpos_init_done = true;
    }
    Image noise_img = GenImageCellular(64, 64, 20);
    orb->noise_tex = LoadTextureFromImage(noise_img);
    SetTextureWrap(orb->noise_tex, TEXTURE_WRAP_REPEAT);
    UnloadImage(noise_img);
    orb->orb_tex = LoadTexture(ORB_TEXTURE);
    orb->shader = shader_load_custom(0, ORB_SHADER);
    orb->r = orb->orb_tex.width/2;
    orb->time_loc = GetShaderLocation(orb->shader, "u_time");
    orb->shade_loc = GetShaderLocation(orb->shader, "u_shade");
    int noise_tex_loc = GetShaderLocation(orb->shader, "u_noiseTex");
    SetShaderValueTexture(orb->shader, noise_tex_loc, orb->noise_tex);
    orb->obj.is_active = false;
    orb->obj.size = (Vector2){orb->orb_tex.width, orb->orb_tex.height};
    orb->is_hostile = false;
    orb->xpos = g_orb_xpos[g_orbcount++];

    // setup a dummy animation for bbox
    dummy_anim_asset.texture = orb->orb_tex;
    dummy_anim_asset.bbox = bbox_parse(ORB_BBOX);
    dummy_anim.asset = &dummy_anim_asset;
    orb->obj.curr_anim = &dummy_anim;
    // purposely don't set pos and vel.
    // this has to be init'ed when spawned.
}

void orb_draw(orb_t *orb) {
    float time = (float)GetTime();
    Vector3 shade = (Vector3){1.0, 1.0, 1.0}; //rgb white
    SetShaderValue(orb->shader, orb->time_loc, &time, SHADER_UNIFORM_FLOAT);
    if(orb->is_hostile) {
        shade = (Vector3){1.0, 0.0, 0.0}; // rgb red
    }
    SetShaderValue(orb->shader, orb->shade_loc, &shade, SHADER_UNIFORM_VEC3);
    BeginShaderMode(orb->shader);
    Rectangle src = {0.0f, 0.0f, (float)orb->orb_tex.width, (float)orb->orb_tex.height};
    Rectangle dst = {orb->obj.pos.x, orb->obj.pos.y, 64.0f, 64.0f};
    DrawTexturePro(orb->orb_tex, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndShaderMode();
}

void orb_draw_all(orb_t *orbs) {
    for(int i = 0; i < MAX_ORBS; ++i) {
        if(orbs[i].obj.is_active) {
            orb_draw(&orbs[i]);
        }
    }
}

void orb_activate(orb_t *orb, float dt) {
    orb->obj.is_active = true;
    orb->is_hostile = false;
    Vector2 pos, vel;
    pos.y = 0;
    pos.x = orb->xpos;
    vel.x = 0;
    vel.y = ACCEL_G*dt;
    orb->obj.pos = pos;
    orb->obj.vel = vel;
}

void orb_update(orb_t *orb, float dt) {
    // check for bounds
    if(obj_is_oob(&orb->obj, COORDS_SCREEN)) {
        orb->obj.is_active = false;
        orb->is_hostile = false;
    } else {
        if(orb->is_hostile) {
            // keep updating position, orb is hit
            orb->obj.pos.x += orb->obj.vel.x * dt;
            orb->obj.pos.y += orb->obj.vel.y * dt;
        } else {
            // update y pos and velocity
            orb->obj.vel.y += ACCEL_G*dt;
            orb->obj.pos.y += orb->obj.vel.y * dt;
        }
    }
    // there is no animation for orb, shader magic!
}

Vector2 orb_get_hostile_vel(Vector2 vypos, orb_t *orb, float v_mag) {
    Vector2 v;
    Vector2 vyspos = obj_w2s_pos(vypos);
    float dx = vyspos.x - orb->obj.pos.x;
    float dy = vyspos.y - orb->obj.pos.y;
    float theta = atan2(dy, dx);
    v.x = v_mag * cos(theta);
    v.y = v_mag * sin(theta);
    return v;
}