#ifndef _GAME_H_
#define _GAME_H_

#include <vpconfig.h>
#include <raylib.h>
#include <anim_asset.h>
#include <obj.h>
#include <anim.h>
#include <hbar.h>
#include <batr.h>
#include <boss.h>
#include <aanam.h>
#include <player.h>
#include <bonfire.h>
#include <karikku.h>
#include <ffly.h>
#include <crate.h>

#define TYPEBUFFER_SIZE (64)

typedef struct {
    // player and weapons
    player_t p;
    batr_t batrs[MAX_BATRS];
    // environment
    crate_t crate;
    bf_t bonfire[MAX_BONFIRES];
    ffly_t ffly[MAX_FFLY];
    // pickups
    karikku_t karikku[TOTAL_KARIKKU];
    // npcs and weapons
    aanam_t aanas[MAX_AANAS];
    // Type buffer. This will be filled in type mode
    // and flushed when someone uses it, responsibility 
    // of user (e.g puzzle check)
    char typebuffer[TYPEBUFFER_SIZE];
    RenderTexture2D *canvas;
    Camera2D cam;
    // reference to bosses
    bosses* levelbosses;
    // game fonts
    Font game_font;
    // game textures
    Texture2D bg;
    Texture2D splash;
    Texture2D pausemenu;
    Texture2D ktex;
    // game sounds/music
    Music bgmusic;
    // custom cursor
    Texture2D cursor;
    // type pointer, to be used with typebuffer
    int t_ptr;
    // game boolean states
    bool is_gameover;
    bool is_game_wclosed;
    bool is_game_paused;
    bool is_boss_active;
    bool is_type_mode;
    bool is_type_buffer_done;
} game_t;

void game_init(RenderTexture2D *canvas);
void game_start_scene(void);
void game_start_main_loop(void);
void game_deinit(void);

#endif
