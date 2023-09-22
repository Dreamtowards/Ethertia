//
// Created by Dreamtowards on 12/28/2022.
//

#ifndef ETHERTIA_FASTNOISE2TESTS_H
#define ETHERTIA_FASTNOISE2TESTS_H


#include <FastNoise/FastNoise.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ethertia/world/gen/NoiseGeneratorPerlin.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Colors.h>


void TestFN2() {

    NoiseGeneratorPerlin noise{};  // Ethereal
    noise.initPermutations(1);


    auto fnSimplex = FastNoise::New<FastNoise::Perlin>();
    auto fnFrac = FastNoise::New<FastNoise::FractalFBm>();

    fnFrac->SetSource(fnSimplex);
    fnFrac->SetOctaveCount(5);


    float noiseVal[128 * 128];
    auto range = fnSimplex->GenUniformGrid4D(noiseVal, 0, 0, 0, 0, 128, 128, 1, 1, 0.1, 1432);
    Log::info("Min: {}, Max: {}", range.min, range.max);

    BitmapImage img(128, 128);

    int idx = 0;
    for (int x = 0; x < img.getWidth(); ++x) {
        for (int y = 0; y < img.getHeight(); ++y) {
            float f = noiseVal[idx++];
            f += 1.0;
            f /= 2.0;  // range 0-1.

            img.setPixel(x,y,Colors::intRGBA(glm::vec4(0,f,0,1.0f)));
        }
    }
    Loader::savePNG(&img, "test.png");


    return 0;

    for (int i = 0; i < 3; ++i) {

        {
            Log::info("Test1 FastNoise2 BatchGen \1");
            BenchmarkTimer _tm;

        }
        {
            Log::info("Test2 FN2 SingleGen \1");
            BenchmarkTimer _tm;
            for (int rx = 0; rx < 16; ++rx) {
                for (int ry = 0; ry < 16; ++ry) {
                    for (int rz = 0; rz < 16; ++rz) {

                        const float freq = 0.02;
                        float f = fnFrac->GenSingle3D(rx * freq, ry * freq, rz * freq, 1432);
                    }
                }
            }
        }
        {
            Log::info("Test3 My Perlin SingleGen \1");
            BenchmarkTimer _tm;

            for (int rx = 0; rx < 16; ++rx) {
                for (int ry = 0; ry < 16; ++ry) {
                    for (int rz = 0; rz < 16; ++rz) {
                        const float freq = 0.02;
                        float f = noise.fbm(rx * freq, ry * freq, rz * freq, 5);
                    }
                }
            }
        }
        Log::info("");
    }


}

#endif //ETHERTIA_FASTNOISE2TESTS_H
