#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t xorshift32()
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    static uint32_t x = 1;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

uint32_t xorshift8()
{
    return xorshift32() & 0xFF;
}

int main()
{
    int count[256] = {0};

    for (int i = 0; i < 100000000; i++) {
        count[xorshift8()]++;
    }

    for (int i = 1; i < 256; i++) {
        printf("%d %d\n", i, count[i]);
    }
    return 0;
}