//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_NOISEGENERATORPERLIN_H
#define ETHERTIA_NOISEGENERATORPERLIN_H

#include <glm/vec3.hpp>

class NoiseGeneratorPerlin
{
    int permutations[512];
    glm::vec3 coord;

public:
    NoiseGeneratorPerlin(JRand rand) {

    }

    void grad(int hash, float x) {

    }

    float noise(float x) {

    }



};

#endif //ETHERTIA_NOISEGENERATORPERLIN_H
