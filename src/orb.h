#include <raylib.h>
#include <obj.h>
#include <vy.h>

#ifndef _ORB_H_
#define _ORB_H_

typedef struct {
    obj_t obj;
    Shader shader;
    float r;
    float xpos;
    Texture2D noise_tex;
    Texture2D orb_tex;
    int time_loc;
    int shade_loc;
    bool is_hostile;
} orb_t;

void orb_init(orb_t *orb);
void orb_draw(orb_t *orb);
void orb_activate(orb_t *orb, float dt);
void orb_update(orb_t *orb, float dt);
Vector2 orb_get_hostile_vel(vy_t *vy, orb_t *orb, float v_mag);

#endif