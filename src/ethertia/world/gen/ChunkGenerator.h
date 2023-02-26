//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <FastNoise/FastNoise.h>
#include <ethertia/world/gen/NoiseGen.h>

#include <ethertia/material/Materials.h>
#include <ethertia/world/Cell.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/util/Log.h>


class ChunkGenerator
{

public:
    using vec3 = glm::vec3;


    ChunkGenerator() {

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
        uint64_t seed = world->getSeed();

//        GenChunk_Flat(chunk);
//        return chunk;

        auto fnFrac = NoiseGen::NewFractalFBM();

        fnFrac->SetSource(NoiseGen::Perlin());
        fnFrac->SetOctaveCount(6);

        float noiseVal[16 * 16 * 16];  // chunkpos is Block Coordinate,,
        fnFrac->GenUniformGrid3D(noiseVal, chunkpos.x, chunkpos.y, chunkpos.z, 16, 16, 16, 1 / 200.0f, seed);

        float noiseTerrHeight[16*16];
        fnFrac->GenUniformGrid2D(noiseTerrHeight, chunkpos.x, chunkpos.z, 16, 16, 1 / 400.0f, seed);

        int idxXZY = 0;
        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {  // rz*256+ry*16+rx
                    float x = chunkpos.x + rx,
                          y = chunkpos.y + ry,
                          z = chunkpos.z + rz;

                    float terr = noiseTerrHeight[NoiseGen::IdxXZ(rx, rz)];// - terrRg.min;
                    float noise3d = noiseVal[NoiseGen::Idx3(rx,ry,rz)];

                    float f = terr - y/64.0f;//

                    if (f > -0.2) {
                        float add = Mth::max(0.0f, noise3d * 8);
                        if (f < -0.4) {
                            f = Mth::lerp(0.5f, f, add);
                        } else {
                            f += add;
                        }
                    }


                    Material* mtl = 0;
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
