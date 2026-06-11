#ifndef _VPCONFIG_H_
#define _VPCONFIG_H_

#include <anim_asset.h>

// Game Title
#define G_TITLE "VAVVALPENNU🦇"

// Global width and height
#define G_W (1920)
#define G_H (1080)
#define GAME_GROUND_Y (942)

// Deadzone parameters
#define DZ_RL (400)
#define DZ_LL (-600)

// Max Healths
#define PLAYER_MAX_HEALTH_INITIAL (100)
#define VY_MAX_HEALTH             (300)

// Asset files
#define MAIN_FONT      "assets/fonts/StackSansTextB.ttf"
#define MENU_FONT      "assets/fonts/AladinR.ttf"
#define BACKGROUND     "assets/backdrop/keralabackdrop.png"
#define SPLASH_BG      "assets/backdrop/splash.png"
#define PAUSEMENU_BG   "assets/backdrop/pausemenu.png"

// Animation assets
// PLAYER
#define ANIM_P_IDLE     (anim_info_t){"assets/sprites/player/idle.png", "P IDLE", 16, 0.07f, true}
#define ANIM_P_RUN      (anim_info_t){"assets/sprites/player/run.png", "P RUN", 16, 0.07f, true}
#define ANIM_P_JUMP     (anim_info_t){"assets/sprites/player/jump.png", "P JUMP", 8, 0.07f, false}

// BATARANG
#define ANIM_BATARANG   (anim_info_t){"assets/sprites/batarang/batarang_rotate.png", "BATARANG", 8, 0.1f, true}

// VADAYAKSHI
#define ANIM_VY_RISE    (anim_info_t){"assets/sprites/vadayakshi/rise.png", "VY RISE", 25, 0.07f, true}

// AANAMARUTHA
#define ANIM_AANAM_RUN  (anim_info_t){"assets/sprites/aanamarutha/run.png", "AANAM RUN", 16, 0.2f, true}

// Textures
#define ORB_TEXTURE   "assets/textures/orb.png"

// Shader files
#define HBAR_SHADER    "shaders/hbar.fs"
#define ORB_SHADER     "shaders/orb.fs"

// Other Config
#define MAX_BATRS (10)
#define MAX_ORBS  (6)
#define MAX_AANAS (2)

#endif /* _VPCONFIG_H_ */