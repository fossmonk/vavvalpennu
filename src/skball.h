#ifndef _SKBALL_H_
#define _SKBALL_H_

#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>

typedef struct {
    obj_t obj;
    Shader shader;
    float r;
    Texture2D skball_tex;
    int time_loc;
    int id;
} skball_t;

void skball_init(skball_t *skball);
void skball_draw_all(skball_t *skballs);
void skball_activate(skball_t *skball);
void skball_update(skball_t *skball, float dt);

#endif