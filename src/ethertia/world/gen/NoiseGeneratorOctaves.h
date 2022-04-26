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

    double fbm(double x, double y, double z) {
        double f = 0;
        double w = 0.5;
        for (int i = 0; i < numgens; ++i) {
            f += generators[i].noise(x,y,z) * w;
            x *= 2.0; y *= 2.0; z*= 2.0;
            w *= 0.5;
        }
        return f;
    }
    double fbm(double x, double z) {
        double f = 0;
        double w = 1.0;
        for (int i = 0; i < numgens; ++i) {
            f += generators[i].noise(x*w,z*w) / w;
            w *= 0.5;
        }
        return f;
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

};

#endif //ETHERTIA_NOISEGENERATOROCTAVES_H
