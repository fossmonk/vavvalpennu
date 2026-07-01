#ifndef _PUZZLE_H_
#define _PUZZLE_H_

void puzzle_init(void);
int puzzle_get(void);
void puzzle_draw_q(int puzzle_id);
bool puzzle_check(char *user_ans);

#endif