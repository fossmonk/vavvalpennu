#include <input.h>
#include <raylib.h>
#include <vpconfig.h>

bool mouse_down[8]  = { false };
bool keys_down[400] = { false };

bool input_iskeypressed(KeyboardKey k) {
    bool keydown = IsKeyDown(k);
    bool ret = keydown && !keys_down[k];
    keys_down[k] = keydown;
    return ret;
}

bool input_iskeydown(KeyboardKey k) {
    return IsKeyDown(k);
}

bool input_ismousepressed(MouseButton m) {
    bool mousedown = IsMouseButtonDown(m);
    bool ret = mousedown && !mouse_down[m];
    mouse_down[m] = mousedown;
    return ret;
}

Vector2 input_get_mouse_pos(void) {
    Vector2 mouseScalePos = GetMousePosition();
    mouseScalePos.x = (mouseScalePos.x / GetScreenWidth()) * G_W;
    mouseScalePos.y = (mouseScalePos.y / GetScreenHeight()) * G_H;
    return mouseScalePos;
}

