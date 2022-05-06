//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKSTONE_H
#define ETHERTIA_BLOCKSTONE_H

#include "Block.h"

class BlockStone : public Block
{


    void getVertexData(VertexBuffer* vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos) override {

        internalPutCube(vbuf, world, chunkpos, rpos, BlockTextures::STONE);
    }
};

#endif //ETHERTIA_BLOCKSTONE_H
