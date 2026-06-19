#include <raylib.h>
#include <anim_asset.h>
#include <vpconfig.h>

// PLAYER
anim_asset_t player_run;
anim_asset_t player_idle;
anim_asset_t player_jump;
anim_asset_t player_hithurt;
anim_asset_t player_flash;
anim_asset_t player_shock;
anim_asset_t player_wlash;

// BONFIRE
anim_asset_t bonfire_burn;

// BATARANG
anim_asset_t batr_rotate;

// KARIKKU
anim_asset_t karikku_rotate;

// FIREFLY
anim_asset_t ff_fly;

// VADAYAKSHI
anim_asset_t vy_rise;
anim_asset_t vy_shock;

// AANAMARUTHA
anim_asset_t aanam_run;

// EPECHI
anim_asset_t epechi_roll;

void anim_asset_load(anim_info_t info, anim_asset_t *asset) {
    asset->texture = LoadTexture(info.filepath);
    asset->name = info.name;
    asset->framecount = info.framecount;
    asset->duration = info.duration;
    asset->repeat = info.repeat;
}

Vector2 anim_asset_get_frame_dim(anim_asset_t *asset) {
    int w = asset->texture.width/asset->framecount;
    int h = asset->texture.height;
    return (Vector2){w, h};
}

void anim_asset_load_all(void) {
    //player assets
    anim_asset_load(ANIM_P_RUN, &player_run);
    anim_asset_load(ANIM_P_IDLE, &player_idle);
    anim_asset_load(ANIM_P_JUMP, &player_jump);
    anim_asset_load(ANIM_P_HITHURT, &player_hithurt);
    anim_asset_load(ANIM_P_FLASH, &player_flash);
    anim_asset_load(ANIM_P_SHOCK, &player_shock);
    anim_asset_load(ANIM_P_WLASH, &player_wlash);

    // bonfire assets
    anim_asset_load(ANIM_BF_BURN, &bonfire_burn);

    //batr assets
    anim_asset_load(ANIM_BATARANG, &batr_rotate);

    // karikku assets
    anim_asset_load(ANIM_KARIKKU, &karikku_rotate);

    // ffly assets
    anim_asset_load(ANIM_FIREFLY, &ff_fly);

    //vy assets
    anim_asset_load(ANIM_VY_RISE, &vy_rise);
    anim_asset_load(ANIM_VY_SHOCK, &vy_shock);

    // aanam assets
    anim_asset_load(ANIM_AANAM_RUN, &aanam_run);

    // epechi assets
    anim_asset_load(ANIM_EPECHI_ROLL, &epechi_roll);
}

