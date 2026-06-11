#include <player.h>
#include <anim_asset.h>


// PLAYER ANIMATIONS
extern anim_asset_t player_run;
extern anim_asset_t player_idle;
extern anim_asset_t player_jump;

void player_init(player_t *p) {
    // ANIM
    Vector2 dim;
    // idle
    dim = anim_asset_get_frame_dim(&player_idle);
    p->anim_idle_r.asset = &player_idle;
    p->anim_idle_r.timer = 0.0f;
    p->anim_idle_r.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // run
    dim = anim_asset_get_frame_dim(&player_run);
    p->anim_run_r.asset = &player_run;
    p->anim_run_r.timer = 0.0f;
    p->anim_run_r.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    // jump
    dim = anim_asset_get_frame_dim(&player_jump);
    p->anim_jump_r.asset = &player_jump;
    p->anim_jump_r.timer = 0.0f;
    p->anim_jump_r.curr_frame = (Rectangle){0, 0, dim.x, dim.y};
    
    // STATE
    p->obj.curr_anim = &p->anim_idle_r;
    float player_initx = G_W/2 - p->obj.curr_anim->curr_frame.width;
    float player_inity = GAME_GROUND_Y - p->obj.curr_anim->curr_frame.height;
    p->obj.is_active = true;
    p->obj.pos = (Vector2){player_initx, player_inity};
    p->obj.vel = (Vector2){0, 0};
    p->obj.size = (Vector2){p->obj.curr_anim->curr_frame.width, p->obj.curr_anim->curr_frame.height};
    p->obj.hdir = RIGHT;
    p->max_health = PLAYER_MAX_HEALTH_INITIAL;
    p->health = p->max_health;
    p->is_dying = false;
    p->is_jumping = false;

    // init player healthbar
    hbar_init(&p->hbar, (Vector2){10, 10}, 300, 20, 2);
}