#include <raylib.h>
#include <obj.h>

#ifndef _ORB_H_
#define _ORB_H_

typedef struct {
    obj_t obj;
    Shader shader;
    float r;
    Texture2D noise_tex;
    Texture2D orb_tex;
    int time_loc;
} orb_t;

void orb_init(orb_t *orb);
void orb_draw(orb_t *orb);

#endif