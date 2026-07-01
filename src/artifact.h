#ifndef _ARTIFACT_H_
#define _ARTIFACT_H_

#include <raylib.h>
#include <obj.h>

typedef struct {
    obj_t obj;
    Shader shader;
    Texture2D artifact_tex;
    int time_loc;
    bool is_won;
} artifact_t;

void artifact_init(void);
void artifact_update(void);
void artifact_draw(void);

#endif