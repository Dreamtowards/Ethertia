//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKDIRT_H
#define ETHERTIA_BLOCKDIRT_H

#include "Block.h"

class BlockDirt : public Block
{

    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

        internalPutCube(vbuf, rpos, chunk, BlockTextures::DIRT);
    }
};

#endif //ETHERTIA_BLOCKDIRT_H
