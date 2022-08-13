//
// Created by Dreamtowards on 2022/7/8.
//

#ifndef ETHERTIA_BLOCKLEAVES_H
#define ETHERTIA_BLOCKLEAVES_H

#include "Block.h"

class BlockLeaves : public Block
{

    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

//        internalPutCube(vbuf, rpos, chunk, BlockTextures::GLASS);

        internalPutLeaves(vbuf, rpos, chunk, BlockTextures::LEAVES_2);
    }
};

#endif //ETHERTIA_BLOCKLEAVES_H
