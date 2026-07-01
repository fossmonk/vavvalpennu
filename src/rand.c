#include <rand.h>

typedef unsigned int u32;

static u32 gseed = 123456789;

void vp_rand_seed(unsigned int seed) {
    gseed = seed;
}

u32 vp_rand(void) {
    gseed = gseed * 1103515245 + 12345;
    return (u32)(gseed / 65536) % 32768;
}

// includes min, doesn't include max
u32 vp_rand_lim(unsigned int min, unsigned int max) {
    u32 res = min;
    if(min != max) {
        res = min + (vp_rand() % (max - min));
    }
    return res;
}
