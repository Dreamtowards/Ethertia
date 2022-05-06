//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKDIRT_H
#define ETHERTIA_BLOCKDIRT_H

#include "Block.h"

class BlockDirt : public Block
{

    void getVertexData(VertexBuffer* vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos) override {

        internalPutCube(vbuf, world, chunkpos, rpos, BlockTextures::DIRT);
    }
};

#endif //ETHERTIA_BLOCKDIRT_H
