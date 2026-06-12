#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <orb.h>

#define ORB_RAND_POS_X         (100 + (rand() % (G_W - 200)))

void orb_init(orb_t *orb) {
    Image noise_img = GenImageCellular(64, 64, 20);
    orb->noise_tex = LoadTextureFromImage(noise_img);
    SetTextureWrap(orb->noise_tex, TEXTURE_WRAP_REPEAT);
    UnloadImage(noise_img);
    orb->orb_tex = LoadTexture(ORB_TEXTURE);
    orb->shader = LoadShader(NULL, ORB_SHADER);
    orb->r = orb->orb_tex.width/2;
    orb->time_loc = GetShaderLocation(orb->shader, "u_time");
    int noise_tex_loc = GetShaderLocation(orb->shader, "u_noiseTex");
    SetShaderValueTexture(orb->shader, noise_tex_loc, orb->noise_tex);
    orb->obj.is_active = false;
    orb->obj.size = (Vector2){orb->orb_tex.width, orb->orb_tex.width};
    orb->is_hostile = false;
    // purposely don't set pos and vel.
    // this has to be init'ed when spawned.
}

void orb_draw(orb_t *orb) {
    float time = (float)GetTime();
    SetShaderValue(orb->shader, orb->time_loc, &time, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(orb->shader);
    Rectangle src = {0.0f, 0.0f, (float)orb->orb_tex.width, (float)orb->orb_tex.height};
    Rectangle dst = {orb->obj.pos.x, orb->obj.pos.y, 64.0f, 64.0f};
    DrawTexturePro(orb->orb_tex, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndShaderMode();
}

void orb_activate(orb_t *orb, float dt) {
    orb->obj.is_active = true;
    Vector2 pos, vel;
    pos.y = 0;
    pos.x = ORB_RAND_POS_X;
    vel.x = 0;
    vel.y = ACCEL_G*dt;
    orb->obj.pos = pos;
    orb->obj.vel = vel;
}

void orb_update(orb_t *orb, float dt) {
    // check for bounds
    if(obj_is_oob(&orb->obj, COORDS_SCREEN)) {
        orb->obj.is_active = false;
    } else {
        // update y pos and velocity
        orb->obj.vel.y += ACCEL_G*dt;
        orb->obj.pos.y += orb->obj.vel.y * dt;
    }
    // there is no animation for orb, shader magic!
}