//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_NOISEGENERATORPERLIN_H
#define ETHERTIA_NOISEGENERATORPERLIN_H

#include <glm/vec3.hpp>
#include <ethertia/util/Mth.h>

#include "JRand.h"

class NoiseGeneratorPerlin
{
    int perm[512];  // 512len
    glm::vec3 coord;

public:
    void init(JRand& rand) {
        coord = glm::vec3(
            rand.nextDouble() * 256.0,
            rand.nextDouble() * 256.0,
            rand.nextDouble() * 256.0
        );

        for (int i = 0; i < 256; ++i) {
            perm[i] = i;
        }
        for (int i = 0; i < 256; ++i) {
            int r = rand.nextInt(256 - i) + i;
            int j = perm[i];
            perm[i] = perm[r];
            perm[r] = j;
            perm[i + 256] = perm[i];
        }
    }

    double grad(int hash, double x, double y, double z) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : (h != 12 && h != 14 ? z : x);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
    double grad(int hash, double x, double y) {
        int h = hash & 15;
        double u = (1 - ((h & 8) >> 3)) * x;
        double v = h < 4 ? 0 : (h != 12 && h != 14 ? y : x);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    double noise(double _x, double _y, double _z) {
        double x = _x + coord.x;
        double y = _y + coord.y;
        double z = _z + coord.z;
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
    float noise(float _x, float _y) {
        double x = _x + coord.x;
        double y = _y + coord.z;  // TempReq: coord.z not y.
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


    void generateNoiseBatched(double dest[], double x, double y, double z, int nX, int nY, int nZ, double mX, double mY, double mZ, double facInv) {
        int var10001;
        int var19;
        int var22;
        double var31;
        double var35;
        int var37;
        double var38;
        int var40;
        int var41;
        double u;
        if (nY == 1) {
            bool var64 = false;
            bool var65 = false;
            bool var21 = false;
            bool var68 = false;
            double var70 = 0.0;
            double var73 = 0.0;
            int var75 = 0;
            double var77 = 1.0 / facInv;

            for(int var30 = 0; var30 < nX; ++var30) {
                var31 = (x + (double)var30) * mX + coord.x;
                int var78 = (int)var31;
                if (var31 < (double)var78) {
                    --var78;
                }

                int var34 = var78 & 255;
                var31 -= (double)var78;
                var35 = var31 * var31 * var31 * (var31 * (var31 * 6.0 - 15.0) + 10.0);

                for(var37 = 0; var37 < nZ; ++var37) {
                    var38 = (z + (double)var37) * mZ + coord.z;
                    var40 = (int)var38;
                    if (var38 < (double)var40) {
                        --var40;
                    }

                    var41 = var40 & 255;
                    var38 -= (double)var40;
                    u = var38 * var38 * var38 * (var38 * (var38 * 6.0 - 15.0) + 10.0);
                    var19 = perm[var34] + 0;
                    int var66 = perm[var19] + var41;
                    int var67 = perm[var34 + 1] + 0;
                    var22 = perm[var67] + var41;
                    var70 = Mth::lerp(var35, grad(perm[var66], var31, var38), grad(perm[var22], var31 - 1.0, 0.0, var38));
                    var73 = Mth::lerp(var35, grad(perm[var66 + 1], var31, 0.0, var38 - 1.0), grad(perm[var22 + 1], var31 - 1.0, 0.0, var38 - 1.0));
                    double var79 = Mth::lerp(u, var70, var73);
                    var10001 = var75++;
                    dest[var10001] += var79 * var77;
                }
            }

        } else {
            var19 = 0;
            double var20 = 1.0 / facInv;
            var22 = -1;
            bool var23 = false;
            bool var24 = false;
            bool var25 = false;
            bool var26 = false;
            bool var27 = false;
            bool var28 = false;
            double var29 = 0.0;
            var31 = 0.0;
            double var33 = 0.0;
            var35 = 0.0;

            for(var37 = 0; var37 < nX; ++var37) {
                var38 = (x + (double)var37) * mX + coord.x;
                var40 = (int)var38;
                if (var38 < (double)var40) {
                    --var40;
                }

                var41 = var40 & 255;
                var38 -= (double)var40;
                u = var38 * var38 * var38 * (var38 * (var38 * 6.0 - 15.0) + 10.0);

                for(int var44 = 0; var44 < nZ; ++var44) {
                    double var45 = (z + (double)var44) * mZ + coord.z;
                    int var47 = (int)var45;
                    if (var45 < (double)var47) {
                        --var47;
                    }

                    int var48 = var47 & 255;
                    var45 -= (double)var47;
                    double v = var45 * var45 * var45 * (var45 * (var45 * 6.0 - 15.0) + 10.0);

                    for(int dY = 0; dY < nY; ++dY) {
                        double fy = (y + (double)dY) * mY + coord.y;
                        int iy = (int)fy;
                        if (fy < (double)iy) {
                            --iy;
                        }

                        int Y = iy & 255;
                        fy -= (double)iy;
                        double w = fy * fy * fy * (fy * (fy * 6.0 - 15.0) + 10.0);

                        // Y Not Continuous.
//                        if (dY == 0 || Y != var22) {
                            var22 = Y;
                            int var69 = perm[var41] + Y;
                            int var71 = perm[var69] + var48;
                            int var72 = perm[var69 + 1] + var48;
                            int var74 = perm[var41 + 1] + Y;
                            int var75 = perm[var74] + var48;
                            int var76 = perm[var74 + 1] + var48;
                            var29 = Mth::lerp(u, grad(perm[var71], var38, fy, var45), grad(perm[var75], var38 - 1.0, fy, var45));
                            var31 = Mth::lerp(u, grad(perm[var72], var38, fy - 1.0, var45), grad(perm[var76], var38 - 1.0, fy - 1.0, var45));
                            var33 = Mth::lerp(u, grad(perm[var71 + 1], var38, fy, var45 - 1.0), grad(perm[var75 + 1], var38 - 1.0, fy, var45 - 1.0));
                            var35 = Mth::lerp(u, grad(perm[var72 + 1], var38, fy - 1.0, var45 - 1.0), grad(perm[var76 + 1], var38 - 1.0, fy - 1.0, var45 - 1.0));
//                        } // else {
//                            throw std::exception();
//                        }

                        double var62 = Mth::lerp(v, Mth::lerp(w, var29, var31), Mth::lerp(w, var33, var35));
                        var10001 = var19++;
                        dest[var10001] += var62 * var20;
                    }
                }
            }

        }
    }

};

#endif //ETHERTIA_NOISEGENERATORPERLIN_H
