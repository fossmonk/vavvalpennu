#include <raylib.h>

#ifndef _VP_INPUT_H_
#define _VP_INPUT_H_

bool input_iskeypressed(KeyboardKey k);
bool input_iskeydown(KeyboardKey k);
bool input_ismousepressed(MouseButton m);
Vector2 input_get_mouse_pos(void);

#endif