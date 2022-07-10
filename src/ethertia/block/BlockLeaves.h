//
// Created by Dreamtowards on 2022/7/8.
//

#ifndef ETHERTIA_BLOCKLEAVES_H
#define ETHERTIA_BLOCKLEAVES_H

#include "Block.h"

class BlockLeaves : public Block
{

    void getVertexData(VertexBuffer *vbuf, World *world, glm::vec3 chunkpos, glm::vec3 rpos) override {

        internalPutCube(vbuf, world, chunkpos, rpos, BlockTextures::LEAVES);
    }
};

#endif //ETHERTIA_BLOCKLEAVES_H
