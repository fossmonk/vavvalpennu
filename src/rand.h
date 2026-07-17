#ifndef _VP_RAND_H_
#define _VP_RAND_H_

void vp_rand_seed(long long seed);
unsigned int vp_rand(void);
unsigned int vp_rand_lim(unsigned int min, unsigned int max);

#endif