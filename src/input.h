#include <raylib.h>

#ifndef _VP_INPUT_H_
#define _VP_INPUT_H_

typedef enum {
    MOUSE = 0,
    KEYBOARD
} input_device;

typedef struct {
    const char *description;
    const char *name;
    int value;
    input_device device;
} input_item_t;

typedef struct {
    input_item_t move_left;
    input_item_t move_right;
    input_item_t jump;
    input_item_t throw_batr;
    input_item_t lash_whip;
} input_cfg_t;

void input_init(void);
void input_cfg_sync_to_file(void);
input_cfg_t * input_get_cfg(void);
bool input_move_right(void);
bool input_move_left(void);
bool input_jump(void);
bool input_lash_whip(void);
bool input_throw_batr(void);
bool input_iskeypressed(KeyboardKey k);
bool input_ismousepressed(MouseButton m);
Vector2 input_get_mouse_pos(void);
const char * input_get_kb_keyname(KeyboardKey k);
const char * input_get_mouse_button_name(MouseButton m);

#endif