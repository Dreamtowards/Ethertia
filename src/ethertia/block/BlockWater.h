//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKWATER_H
#define ETHERTIA_BLOCKWATER_H

#include "Block.h"

class BlockWater : public Block
{

    void getVertexData(VertexBuffer* vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos) override {

        internalPutCube(vbuf, world, chunkpos, rpos, BlockTextures::WATER);
    }
};

#endif //ETHERTIA_BLOCKWATER_H
