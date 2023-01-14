//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_NOISE_H
#define ETHERTIA_NOISE_H

#include <FastNoise/FastNoise.h>

class Noise
{
public:
    inline static FastSIMD::eLevel g_SIMDLevel = FastSIMD::Level_Null;  // i.e. if it's null, it is dyn max


    static const FastNoise::SmartNode<FastNoise::Perlin>& Perlin() {
        static FastNoise::SmartNode<FastNoise::Perlin> perlin = FastNoise::New<FastNoise::Perlin>(g_SIMDLevel);
        return perlin;
    }
    static const FastNoise::SmartNode<FastNoise::Simplex>& Simplex() {
        static FastNoise::SmartNode<FastNoise::Simplex> simplex = FastNoise::New<FastNoise::Simplex>(g_SIMDLevel);
        return simplex;
    }

};

#endif //ETHERTIA_NOISE_H
