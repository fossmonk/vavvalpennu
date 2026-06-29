#ifndef _VPCONFIG_H_
#define _VPCONFIG_H_

#include <anim_asset.h>

// Game Title
#define G_TITLE "VAVVALPENNU🦇"

// Global width and height
#define G_W (1920)
#define G_H (1080)
#define GAME_GROUND_Y (942)

#define WORLD_XL (-G_W)
#define WORLD_XR (G_W * 50)

// Camera deadzone parameters
#define DZ_RL (200)
#define DZ_LL (-600)

// Common Physics constants
#define ACCEL_G (600.0f)

// Max Healths
#define PLAYER_MAX_HEALTH_INITIAL (100)
#define VY_MAX_HEALTH             (50)

// Asset files
#define MAIN_FONT      "assets/fonts/StackSansTextB.ttf"
#define MENU_FONT      "assets/fonts/AladinR.ttf"
#define BACKGROUND     "assets/backdrop/keralabackdrop.png"
#define SPLASH_BG      "assets/backdrop/splash.png"
#define PAUSEMENU_BG   "assets/backdrop/pausemenu.png"

// Animation assets
// PLAYER
#define ANIM_P_IDLE       (anim_info_t){"assets/sprites/player/idle.png", "assets/sprites/player/idle.bbox", "P IDLE", 16, 0.07f, true}
#define ANIM_P_RUN        (anim_info_t){"assets/sprites/player/run.png", "assets/sprites/player/run.bbox", "P RUN", 16, 0.07f, true}
#define ANIM_P_JUMP       (anim_info_t){"assets/sprites/player/jump.png", "assets/sprites/player/jump.bbox", "P JUMP", 8, 0.07f, false}
#define ANIM_P_HITHURT    (anim_info_t){"assets/sprites/player/hithurt.png", "assets/sprites/player/hithurt.bbox", "P HITHURT", 25, 0.05f, false}
#define ANIM_P_FLASH      (anim_info_t){"assets/sprites/player/flash.png", "assets/sprites/player/flash.bbox", "P FLASH", 16, 0.01f, false}
#define ANIM_P_SHOCK      (anim_info_t){"assets/sprites/player/shock.png", "assets/sprites/player/shock.bbox", "P SHOCK", 16, 0.05f, false}
#define ANIM_P_WLASH      (anim_info_t){"assets/sprites/player/whiplash.png", "assets/sprites/player/whiplash.bbox", "P WLASH", 16, 0.05f, false}

// BONFIRE
#define ANIM_BF_BURN      (anim_info_t){"assets/sprites/bonfire/burn.png", "assets/sprites/bonfire/burn.bbox", "BONFIRE BURN", 16, 0.07f, true}

// BATARANG
#define ANIM_BATARANG     (anim_info_t){"assets/sprites/batarang/batarang_rotate.png", "assets/sprites/batarang/batarang_rotate.bbox", "BATARANG", 8, 0.1f, true}

// KARIKKU
#define ANIM_KARIKKU     (anim_info_t){"assets/sprites/karikku/rotate.png", "assets/sprites/karikku/rotate.bbox", "KARIKKU", 25, 0.07f, true}

// FIREFLY
#define ANIM_FIREFLY     (anim_info_t){"assets/sprites/firefly/fly.png", "assets/sprites/firefly/fly.bbox", "FF FLY", 25, 0.005f, true}

// VADAYAKSHI
#define ANIM_VY_RISE      (anim_info_t){"assets/sprites/vadayakshi/rise.png", "assets/sprites/vadayakshi/rise.bbox", "VY RISE", 25, 0.07f, true}
#define ANIM_VY_SHOCK     (anim_info_t){"assets/sprites/vadayakshi/shock.png", "assets/sprites/vadayakshi/shock.bbox", "VY SHOCK", 25, 0.07f, false}

// KUTTICHATHAN
#define ANIM_KCH_LAUGH     (anim_info_t){"assets/sprites/kchath/laugh.png", "assets/sprites/kchath/laugh.bbox", "KCH LAUGH", 25, 0.07f, true}


// AANAMARUTHA
#define ANIM_AANAM_RUN    (anim_info_t){"assets/sprites/aanamarutha/run.png", "assets/sprites/aanamarutha/run.bbox", "AANAM RUN", 16, 0.05f, true}
#define ANIM_AANAM_DEATH  (anim_info_t){"assets/sprites/aanamarutha/die.png", "assets/sprites/aanamarutha/die.bbox", "AANAM DEATH", 12, 0.05f, false}

// EENAMPECHI
#define ANIM_EPECHI_ROLL  (anim_info_t){"assets/sprites/epechi/roll.png", "assets/sprites/epechi/roll.bbox", "EPECHI ROLL", 25, 0.04f, true}

// Textures
#define ORB_TEXTURE      "assets/textures/orb.png"
#define ORB_BBOX         "assets/textures/orb.bbox"
#define SKBALL_TEXTURE   "assets/textures/skball.png"
#define PLAYER_HBAR_ICON "assets/textures/vp_head.png"
#define VY_HBAR_ICON     "assets/textures/vy_head.png"
#define KARIKKU_ICON     "assets/textures/karikku.png"

// Shader files
#define HBAR_SHADER    "shaders/hbar.fs"
#define ORB_SHADER     "shaders/orb.fs"
#define SKBALL_SHADER  "shaders/skball.fs"

// AUDIO
#define AUD_AMBIENT    "assets/audio/ambient.mp3"
#define AUD_VY_LAUGH   "assets/audio/vy/laugh.mp3"
#define SOUND_VY_HURT  "assets/audio/vy/hurt.mp3"
#define AUD_KCH_LAUGH  "assets/audio/kch/laugh.mp3"
#define SOUND_KCH_HURT "assets/audio/kch/hurt.mp3" 
#define SOUND_VY_HURT  "assets/audio/vy/hurt.mp3"
#define SOUND_PWHIP    "assets/audio/player/whip.mp3"
#define SOUND_PJUMP    "assets/audio/player/jump.mp3"
#define SOUND_PSLURP   "assets/audio/player/slurp.mp3"
#define SOUND_PBATR    "assets/audio/player/batr_whoosh.mp3"
#define SOUND_PPAIN    "assets/audio/player/pain.mp3"
#define SOUND_AGROWL   "assets/audio/aanam/growl.mp3"

// CURSOR
#define BAT_CURSOR     "assets/textures/batcursor.png"

// Other Config
#define MAX_BATRS (8)
#define MAX_ORBS  (5)
#define MAX_SKBALLS  (2)
#define MAX_AANAS (2)
#define MAX_BONFIRES (5)
#define MAX_FFLY (5)
#define MAX_KARIKKU (8) // per screen width count?
#define TOTAL_KARIKKU (MAX_KARIKKU * (WORLD_XR / G_W))


#endif /* _VPCONFIG_H_ */