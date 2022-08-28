//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_NOISEGENERATOROCTAVES_H
#define ETHERTIA_NOISEGENERATOROCTAVES_H

class NoiseGeneratorOctaves
{
public:
    NoiseGeneratorPerlin* generators;
    int numgens;

    void init(JRand& rand, int n) {
        generators = new NoiseGeneratorPerlin[n];
        numgens = n;

        for (int i = 0; i < n; ++i) {
            generators[i].init(rand);
        }

    }


    void generateNoiseOctaves(double dest[], double x, double y, double z, int nX, int nY, int nZ, double mX, double mY, double mZ) {
        // dest = new float[nX * nY * nZ];

        double w = 1.0;

        for (int i = 0; i < numgens; ++i) {
            generators[i].generateNoiseBatched(dest, x, y, z, nX, nY, nZ, mX * w, mY * w, mZ * w, w);
            w *= 0.5;
        }

    }

    void generateNoiseOctavesXZ(double dest[], int x, int z, int nX, int nZ, double mX, double mZ) {
//        int i = 0;
//        for (int dX = 0; dX < nX; ++dX) {
//            for (int dZ = 0; dZ < nZ; ++dZ) {
//                dest[i++] = fbm((x+dX)*nX, (z+dZ)*nZ);
//            }
//        }
        return generateNoiseOctaves(dest, x, 0, z, nX, 1, nZ, mX, 1.0, mZ);
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

#endif //ETHERTIA_NOISEGENERATOROCTAVES_H
