//
// Created by Dreamtowards on 2023/5/14.
//

#ifndef ETHERTIA_CHUNKGENERATOROVERWORLD_H
#define ETHERTIA_CHUNKGENERATOROVERWORLD_H

#include "ChunkGenerator.h"

class ChunkGeneratorOverworld : public ChunkGenerator
{

    void GenerateChunk(Chunk* chunk) override
    {
        uint64_t seed = chunk->m_World->getSeed();
        glm::vec3 chunkpos = chunk->position;

//        GenChunk_Flat(chunk);
//        return chunk;

        auto fnFrac = NoiseGen::NewFractalFBM();

        fnFrac->SetSource(NoiseGen::Perlin());
        fnFrac->SetOctaveCount(6);

        float noiseVal[16 * 16 * 16];  // chunkpos is Block Coordinate,,
        fnFrac->GenUniformGrid3D(noiseVal, chunkpos.x, chunkpos.y, chunkpos.z, 16, 16, 16, 1 / 200.0f, seed);

        float noiseTerrHeight[16*16];
        fnFrac->GenUniformGrid2D(noiseTerrHeight, chunkpos.x, chunkpos.z, 16, 16, 1 / 400.0f, seed);

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {  // rz*256+ry*16+rx
                    float x = chunkpos.x + rx,
                            y = chunkpos.y + ry,
                            z = chunkpos.z + rz;

                    float terr2d = noiseTerrHeight[NoiseGen::IdxXZ(rx, rz)];// - terrRg.min;
                    float noise3d = noiseVal[NoiseGen::Idx3(rx,ry,rz)];

                    float f = terr2d
                            - y/50.0f;

//                    if (f > -0.2) {
//                        float add = noise3d * 8;//Mth::max(0.0f, noise3d * 8);
//                        if (f < -0.4) {
//                            f = Mth::lerp(0.5f, f, add);
//                        } else {

f += noise3d * 18;

//                        }
//                    }

//                    f = x + y + z;


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
    }

};

#endif //ETHERTIA_CHUNKGENERATOROVERWORLD_H
