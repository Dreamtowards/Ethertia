//
// Created by Dreamtowards on 2022/7/8.
//

#ifndef ETHERTIA_BLOCKLOG_H
#define ETHERTIA_BLOCKLOG_H

#include "Block.h"

class BlockLog : public Block
{

    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

        internalPutCube(vbuf, rpos, chunk, BlockTextures::LOG);
    }

};

#endif //ETHERTIA_BLOCKLOG_H
