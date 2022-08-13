//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKSTONE_H
#define ETHERTIA_BLOCKSTONE_H

#include "Block.h"

class BlockStone : public Block
{


    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

        internalPutCube(vbuf, rpos, chunk, BlockTextures::STONE);
    }
};

#endif //ETHERTIA_BLOCKSTONE_H
