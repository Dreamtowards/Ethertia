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
            w /= 2.0;
        }

    }

    void generateNoiseOctavesXZ(double dest[], int x, int z, int nX, int nZ, double mX, double mZ) {
        return generateNoiseOctaves(dest, x, 10.0, z, nX, 1, nZ, mX, 1.0, mZ);
    }

};

#endif //ETHERTIA_NOISEGENERATOROCTAVES_H
