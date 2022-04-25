//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_NOISEGENERATOROCTAVES_H
#define ETHERTIA_NOISEGENERATOROCTAVES_H

class NoiseGeneratorOctaves
{
    NoiseGeneratorPerlin generators[];

    NoiseGeneratorOctaves(long seed, int n)
    {
        generators = new NoiseGeneratorPerlin[n];


    }

};

#endif //ETHERTIA_NOISEGENERATOROCTAVES_H
