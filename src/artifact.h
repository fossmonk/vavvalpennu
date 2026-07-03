#ifndef _ARTIFACT_H_
#define _ARTIFACT_H_

#include <raylib.h>
#include <obj.h>

typedef struct {
    obj_t obj;
    Texture2D artifact_tex;
    bool is_won;
    float terminal_y;
} artifact_t;

void artifact_init(artifact_t *artif);
void artifact_activate_at(artifact_t *artif, Vector2 pos, float dt);
void artifact_update(artifact_t* artif, float dt);
void artifact_draw(artifact_t *artif);

#endif