#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <ftext.h>

#define MAX_FTEXT       (16)
#define FTEXT_VEL_Y     (100.0f)
#define FTEXT_ZIGZAG    (2.0f)
#define FTEXT_FONTSIZE  (40.0f)
#define FTEXT_DURATION  (1.5f)
#define FTEXT_ZDURATION (0.07f)

typedef struct {
    obj_t obj;
    char *text;
    float timer;
    float zigtimer;
    float zigduration;
    float duration;
    int alpha;
} ftext_t;

static ftext_t ft_arr[MAX_FTEXT];
static bool zig = true;
static Font ftext_font;

void ftext_init_all(void) {
    for(int i = 0; i < MAX_FTEXT; ++i) {
        ftext_t *ft = &ft_arr[i];
        ft->obj.is_active = false;
        ft->obj.cs = COORDS_SCREEN;
        ft->obj.vel.y = FTEXT_VEL_Y;
        ft->timer = 0.0f;
        ft->zigtimer = 0.0f;
        ft->duration = FTEXT_DURATION;
        ft->zigduration = FTEXT_ZDURATION;
        ft->alpha = 255;
    }
    ftext_font = LoadFontEx(FTEXT_FONT, 96, NULL, 0);
    SetTextureFilter(ftext_font.texture, TEXTURE_FILTER_BILINEAR);

}

void ftext_spawn(char *text, Vector2 pos) {
    ftext_t *ft = NULL;
    for(int i = 0; i < MAX_FTEXT; ++i) {
        if(!ft_arr[i].obj.is_active) {
            ft = &ft_arr[i];
            break;
        }
    }
    if(ft == NULL) {
        // soft failure, error print is fine, in debug mode
        #ifdef DEBUG
        printf("[WARNING] Couldn't spawn %s ... \n", text);
        #endif
    } else {
        // activate ftext
        ft->obj.is_active = true;
        ft->text = text;
        ft->obj.pos = pos;
    }
}

void ftext_update_all(float dt) {
    for(int i = 0; i < MAX_FTEXT; ++i) {
        ftext_t *ft = &ft_arr[i];
        if(!ft->obj.is_active) continue;
        
        ft->timer += dt;
        ft->zigtimer += dt;
        if(ft->timer >= ft->duration) {
            // mark as inactive
            ft->obj.is_active = false;
            ft->timer = 0.0f;
            ft->zigtimer = 0.0f;
            ft->alpha = 255;
        } else {
            // update y pos, text floats up
            ft->obj.pos.y -= ft->obj.vel.y*dt;
            if(ft->zigtimer >= ft->zigduration) {
                // update alpha with zigtimer
                ft->alpha -= 20;
                // update x pos, slightly zig zag
                float mult = zig ? -1 : 1;
                ft->obj.pos.x += (mult * FTEXT_ZIGZAG);
                ft->zigtimer = 0.0f;
            }
        }
    }
    // zig-zag
    zig = !zig;
}

void ftext_draw_all(void) {
    for(int i = 0; i < MAX_FTEXT; ++i) {
        ftext_t *ft = &ft_arr[i];
        if(!ft->obj.is_active) continue;

        int alpha = ft->alpha < 0 ? 0 : ft->alpha;
        Color text_color = (Color) {255, 255, 255, (unsigned char)(alpha)};
        DrawTextEx(ftext_font, ft->text, ft->obj.pos, FTEXT_FONTSIZE, 1, text_color);
    }
}

