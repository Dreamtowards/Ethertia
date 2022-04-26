
#include <ethertia/util/Log.h>



#include "Ethertia.h"

Ethertia* Ethertia::INST;

#include <ethertia/world/gen/JRand.h>
#include <ethertia/world/gen/NoiseGeneratorPerlin.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

int main()
{
    Ethertia().run();
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