
#include <ethertia/util/Log.h>



#include "Ethertia.h"

Ethertia* Ethertia::INST;

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

int main()
{
    Ethertia().run();


//    NoiseGeneratorOctaves n;
//    JRand jr1(100);
//    n.init(jr1, 5);
//
//    Log::info(std::to_string(n.fbm(123*2.3133, 3455*4.341)));
//
//    NoiseGeneratorOctaves n2;
//    JRand jr2(100);
//    n2.init(jr2, 5);
//
//    double da[9] = {};
//    n2.generateNoiseOctavesXZ(da, 123, 3453, 3,3, 2.3133,4.341);
//    Log::info(std::to_string(da[2]));
//    NoiseGeneratorOctaves n2;
//    JRand jr2(100);
//    n2.init(jr2, 2);
//
//    double da[4] = {};
//    n2.generateNoiseOctaves(da, 123, 123, 3453, 1,2,1, 1,1,1);
//    Log::info(std::to_string(da[1]));
//
//    NoiseGeneratorOctaves n3;
//    JRand jr3(100);
//    n3.init(jr3, 2);
//
//    double d2[4] = {};
//    n3.generateNoiseOctaves(d2, 123, 124, 3453, 1,2,1, 1,1,1);
//    Log::info(std::to_string(d2[0]));


//
//    ChunkGenerator chunkGenerator;
//    chunkGenerator.generateChunk({0, 0, 0});

//    JRand jr;
//    jr.seed = JRand::initSeed(1772835215);
//    Log::info(std::to_string(jr.nextInt()));
//    Log::info(std::to_string(jr.nextInt()));
//    Log::info(std::to_string(jr.nextDouble()));
//    Log::info(std::to_string(jr.nextDouble()));
//    Log::info(std::to_string(jr.nextLong()));
//    Log::info(std::to_string(jr.nextLong()));
//    Log::info(std::to_string(jr.nextInt(100)));
//    Log::info(std::to_string(jr.nextInt(10000)));
//
//    Log::info(std::to_string(JRand::hash_jstr("Glacier")));  // 1772835215
//
//    NoiseGeneratorPerlin n(jr);
//    Log::info(std::to_string(n.noise(13245, 61321, 19834)));
//    Log::info(std::to_string(n.noise(14282, 21993)));
    return 0;
}