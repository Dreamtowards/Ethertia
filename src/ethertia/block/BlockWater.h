//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKWATER_H
#define ETHERTIA_BLOCKWATER_H

#include "Block.h"

class BlockWater : public Block
{

    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

        internalPutCube(vbuf, rpos, chunk, BlockTextures::WATER);
    }
};

#endif //ETHERTIA_BLOCKWATER_H
