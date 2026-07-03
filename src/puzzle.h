#ifndef _PUZZLE_H_
#define _PUZZLE_H_

#include <raylib.h>

void puzzle_init(void);
int puzzle_get(void);
Vector2 puzzle_get_pos(void);
void puzzle_draw_q(int puzzle_id);
void puzzle_play_solved(void);
void puzzle_play_wrong(void);
void puzzle_draw_textbox(char * typebuffer);
bool puzzle_check(char *user_ans, int id);

#endif