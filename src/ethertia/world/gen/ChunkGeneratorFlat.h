//
// Created by Dreamtowards on 2023/5/14.
//

#ifndef ETHERTIA_CHUNKGENERATORFLAT_H
#define ETHERTIA_CHUNKGENERATORFLAT_H

#include "ChunkGenerator.h"

class ChunkGeneratorFlat : public ChunkGenerator
{

    void GenerateChunk(Chunk* chunk) override
    {
        for (int rx = 0; rx < 16; ++rx)
        {
            for (int ry = 0; ry < 16; ++ry)
            {
                for (int rz = 0; rz < 16; ++rz)
                {
                    int y = chunk->position.y + ry;

                    float f = -y;

                    chunk->setCell(rx,ry,rz,
                                   Cell(f > 0 ? Materials::STONE : Materials::AIR, f));
                }
            }
        }
    }
};

#endif //ETHERTIA_CHUNKGENERATORFLAT_H
