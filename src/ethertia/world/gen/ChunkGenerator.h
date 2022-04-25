//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>

class ChunkGenerator
{


public:
    Chunk* generateChunk(glm::vec3 chunkpos)
    {
        Chunk* chunk = new Chunk();

        for (int rx = 0; rx < 16; ++rx) {
            for (int rz = 0; rz < 16; ++rz) {



            }
        }

        return chunk;
    }
};

#endif //ETHERTIA_CHUNKGENERATOR_H
