#ifndef _CRATE_H_
#define _CRATE_H_

#include <raylib.h>
#include <obj.h>
#include <anim.h>
#include <artifact.h>
#include <potion.h>

#define CR_ARTIF(p_cr) (cr->content.content.artifact)
#define CR_POTION(p_cr) (cr->content.content.potion)

typedef enum {
    POTION,
    ARTIFACT
} crate_content_type;

typedef struct {
    crate_content_type type;
    union {
        artifact_t artifact;
        potion_t potion;
    } content;
} crate_content_t;

typedef struct {
    obj_t obj;
    crate_content_t content;
    anim_t anim_burst;
    Texture2D crate_tex;
    bool is_broken;
    bool is_open;
} crate_t;

void crate_init(crate_t* cr);
void crate_activate(crate_t *cr);
void crate_update(crate_t* cr, float dt);
void crate_content_update(crate_t *cr, float dt);
bool crate_content_is_grounded_and_active(crate_t* cr);
void crate_artif_setpos(crate_t *cr, Vector2 pos);
void crate_draw(crate_t* cr);
void crate_content_draw(crate_t *cr);

#endif