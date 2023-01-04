//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_NOISEGENERATORPERLIN_H
#define ETHERTIA_NOISEGENERATORPERLIN_H

#include <glm/vec3.hpp>
#include <ethertia/util/Mth.h>

class NoiseGeneratorPerlin
{
    int perm[512];

public:
    void initPermutations(long seed) {
        for (int i = 0; i < 256; ++i) {
            perm[i] = i;
        }
        for (int i = 0; i < 256; ++i) {
            int r = /*rand.nextInt*/(256 - i) + i;  //TODO: Rand
            int j = perm[i];
            perm[i] = perm[r];
            perm[r] = j;
            perm[i + 256] = perm[i];
        }
    }

    static double grad(int hash, double x, double y, double z) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : (h != 12 && h != 14 ? z : x);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
    static double grad(int hash, double x, double y) {
        int h = hash & 15;
        double u = (1 - ((h & 8) >> 3)) * x;
        double v = h < 4 ? 0 : (h != 12 && h != 14 ? y : x);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    double noise(double x, double y, double z) {
        int ix = Mth::floor(x);
        int iy = Mth::floor(y);
        int iz = Mth::floor(z);
        x -= ix;
        y -= iy;
        z -= iz;
        int X = ix & 255;
        int Y = iy & 255;
        int Z = iz & 255;
        double u = Mth::fade(x);
        double v = Mth::fade(y);
        double w = Mth::fade(z);
        int A = perm[X] + Y,   AA = perm[A] + Z, AB = perm[A+1] + Z;
        int B = perm[X+1] + Y, BA = perm[B] + Z, BB = perm[B+1] + Z;
        return Mth::lerp(w, Mth::lerp(v, Mth::lerp(u, grad(perm[AA], x, y, z), grad(perm[BA], x-1, y, z)),
                                         Mth::lerp(u, grad(perm[AB], x, y-1, z), grad(perm[BB], x-1, y-1, z))),
                            Mth::lerp(v, Mth::lerp(u, grad(perm[AA+1], x, y, z-1), grad(perm[BA+1], x-1, y, z-1)),
                                         Mth::lerp(u, grad(perm[AB+1], x, y-1, z-1), grad(perm[BB+1], x-1, y-1, z-1))));
    }

    float noise(float x, float y) {
        int ix = Mth::floor(x);
        int iy = Mth::floor(y);
        x -= ix;
        y -= iy;
        int X = ix & 255;
        int Y = iy & 255;
        double u = Mth::fade(x);
        double v = Mth::fade(y);
        int A = perm[X],   AA = perm[A] + Y;
        int B = perm[X+1], BB = perm[B] + Y;
        return Mth::lerp(v, Mth::lerp(u, grad(perm[AA], x, y), grad(perm[BB], x-1, 0.0, y)),
                            Mth::lerp(u, grad(perm[AA+1], x, 0.0, y-1), grad(perm[BB + 1], x-1, 0.0, y-1)));
    }


    double fbm(double x, double y, double z, int octaves) {
        double f = 0;
        double w = 0.5;
        for (int i = 0; i < octaves; ++i) {
            f += noise(x,y,z) * w;
            x *= 2.0; y *= 2.0; z*= 2.0;
            w *= 0.5;
        }
        return f;
    }

    double fbm(double x, double y, int octaves) {
        double f = 0;
        double w = 0.5;
        for (int i = 0; i < octaves; ++i) {
            f += noise(x, y) * w;
            x *= 2.0; y *= 2.0;
            w *= 0.5;
        }
        return f;
    }


};

#endif //ETHERTIA_NOISEGENERATORPERLIN_H
