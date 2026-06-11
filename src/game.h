#ifndef _GAME_H_
#define _GAME_H_

#include <vpconfig.h>
#include <raylib.h>
#include <anim_asset.h>
#include <obj.h>
#include <anim.h>
#include <hbar.h>
#include <orb.h>
#include <batr.h>
#include <vy.h>
#include <aanam.h>
#include <player.h>

typedef struct {
    RenderTexture2D *canvas;
    Camera2D cam;
    bool is_gameover;
    bool is_game_wclosed;
    bool is_boss_active;

    // player and weapons
    player_t p;
    batr_t batrs[MAX_BATRS];

    // npcs and weapons
    vy_t vy;
    orb_t orbs[MAX_ORBS];

    aanam_t aanas[MAX_AANAS];

    Font game_font;
    Texture2D bg;
} game_t;

void game_init(RenderTexture2D *canvas);
void game_start_scene(void);
void game_start_main_loop(void);

#endif
