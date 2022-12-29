//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <FastNoise/FastNoise.h>

#include <ethertia/init/Materials.h>
#include <ethertia/world/Cell.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/world/gen/NoiseGeneratorPerlin.h>
#include <ethertia/util/Log.h>


class ChunkGenerator
{

public:
    using vec3 = glm::vec3;

    NoiseGeneratorPerlin noise{};
    ChunkGenerator() {
        noise.initPermutations(1);
    }

    void GenChunk_Flat(Chunk* chunk)
    {

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {
                    int y = chunk->position.y + ry;

                    chunk->setCell(rx,ry,rz, Cell(Materials::STONE, 10 - y));

                }
            }
        }
    }

    Chunk* generateChunk(glm::vec3 chunkpos, World* world) {
        Chunk* chunk = new Chunk(chunkpos, world);
        uint32_t seed = 1243;

//        GenChunk_Flat(chunk);
//        return chunk;

        auto fnSimplex = FastNoise::New<FastNoise::Perlin>();
        auto fnFrac = FastNoise::New<FastNoise::FractalFBm>();

        fnFrac->SetSource(fnSimplex);
        fnFrac->SetOctaveCount(10);

        float noiseVal[16 * 16 * 16];  // chunkpos is Block Coordinate,,
        fnFrac->GenUniformGrid3D(noiseVal, chunkpos.x, chunkpos.y, chunkpos.z, 16, 16, 16, 0.01, seed);

        float terrHeight[16*16];
        auto terrRg = fnFrac->GenUniformGrid2D(terrHeight, chunkpos.x, chunkpos.z, 16, 16, 0.01, seed);

        int idxXZY = 0;
        for (int rz = 0; rz < 16; ++rz) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rx = 0; rx < 16; ++rx) {
                    float terr = terrHeight[rz*16+rx];// - terrRg.min;

                    float x = chunkpos.x + rx,
                          y = chunkpos.y + ry,
                          z = chunkpos.z + rz;

                    float f = noiseVal[idxXZY++];

//                    f += terr;

                    u8 mtl = 0;
                    if (f > 0) {
                        mtl = Materials::STONE;
                    } else if (y < 0) {
                        mtl = Materials::WATER;
                    }

                    chunk->setCell(rx,ry,rz, Cell(mtl, f));
                }
            }
        }

        return chunk;
    }



};

#endif //ETHERTIA_CHUNKGENERATOR_H
