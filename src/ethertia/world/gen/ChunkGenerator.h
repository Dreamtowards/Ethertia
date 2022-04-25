//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>
#include <ethertia/world/chunk/Chunk.h>

class ChunkGenerator
{


public:
    Chunk* generateChunk(glm::vec3 chunkpos)
    {
        Chunk* chunk = new Chunk();
        chunk->position = chunkpos;

        for (int rx = 0; rx < 16; ++rx) {
            for (int rz = 0; rz < 16; ++rz) {
                for (int ry = 0; ry < 16; ++ry) {


                    chunk->setBlock(0, 0, 0, 1);
                    chunk->setBlock(1, 0, 0, 1);

                }
            }
        }

        return chunk;
    }
};

#endif //ETHERTIA_CHUNKGENERATOR_H
