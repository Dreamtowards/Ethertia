//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_JRAND_H
#define ETHERTIA_JRAND_H

#include <ethertia/util/QuickTypes.h>

class JRand
{
public:
    static constexpr long MULTIPLIER = 0x5DEECE66DL;
    static constexpr long ADDEND = 0xBL;
    static constexpr long MASK = (1L << 48) - 1;

    static constexpr double DOUBLE_UNIT = 0x1.0p-53;

    long seed;
    JRand(long sd) {
        seed = initSeed(sd);
    }

    static long initSeed(long seed) {
        return (seed ^ MULTIPLIER) & MASK;
    }

    int next(uint bits) {
        seed = (seed * MULTIPLIER + ADDEND) & MASK;
        return ((ulong)seed >> (48 - bits));
    }
    int nextInt() {
        return (int)next(32);
    }
    int nextInt(int bound) {
        int r = next(31);
        int m = bound - 1;
        if ((bound & m) == 0) r = (int)((bound * (long)r) >> 31);
        else {
            for (int u = r;u - (r = u % bound) + m < 0;u = next(31));
        }
        return r;
    }

    double nextDouble() {
        return (((ulong)next(26) << 27) + next(27)) * DOUBLE_UNIT;
    }
    long nextLong() {
        return ((long)next(32) << 32) + next(32);
    }

    static uint hash_jstr(const char* str) {
        uint h = 0;
        int i = 0;
        char ch;
        while ((ch=str[i])) {
            h = 31 * h + ch;
            i++;
        }
        return h;
    }
};

#endif //ETHERTIA_JRAND_H
