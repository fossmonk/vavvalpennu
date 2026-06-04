#ifndef _GAME_H_
#define _GAME_H_

#include <vpconfig.h>
#include <raylib.h>
#include <anim_asset.h>

typedef enum {
    LEFT,
    RIGHT
} hdir_t;

typedef struct {
    anim_asset_t *asset;
    float timer;
    Rectangle curr_frame;
} anim_t;

typedef struct {
    int max_inner_w;
    int spacing;
    Rectangle outer_rec;
    Rectangle inner_rec;
    Shader shader;
    Texture icon;
} hbar_t;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 size;
    bool is_active;
    anim_t *curr_anim;
    hdir_t hdir;
} obj_t;

typedef struct {
    obj_t obj;

    // animations
    anim_t anim_rotate;
} batr_t;

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    bool is_dying;
    bool is_jumping;

    // animations
    anim_t anim_run_r;
    anim_t anim_idle_r;
    anim_t anim_jump_r;
} player_t;

typedef struct {
    obj_t obj;
    Shader shader;
    float r;
    Texture2D noise_tex;
    Texture2D orb_tex;
    int time_loc;
} orb_t;

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    bool is_dying;
    bool is_jumping;
    bool is_orbpos;

    // animations
    anim_t anim_vy_rise;
} vy_t;

typedef struct {
    Camera2D cam;
    bool is_gameover;
    bool is_game_wclosed;
    bool is_boss_active;

    player_t p;
    batr_t batrs[MAX_BATRS];
    orb_t orbs[MAX_ORBS];
    vy_t vy;

    Font game_font;
    Texture2D bg;
} game_t;

void game_init(void);
void game_start_scene(void);
void game_start_main_loop(void);

#endif
