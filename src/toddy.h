#ifndef _TODDY_H_
#define _TODDY_H_

#include <raylib.h>
#include <obj.h>

typedef struct {
    obj_t obj;
    Shader shader;
    Texture2D toddy_tex;
    int time_loc;
    bool is_won;
} toddy_t;

void toddy_init(void);
void toddy_update(void);
void toddy_draw(void);

#endif